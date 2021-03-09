from tqdist import *
from triproot import *
from tripVote.tripvote_lib import tripVote 
from tripVote.utils import reroot_at_edge, sample_by_depth, prune_long, sample_by_brlen
from treeswift import *
from math import log2, ceil, sqrt, exp
from random import choices

def __local_search__(tree_obj,refTrees,diam,nsample=1):
# tree_obj is a TreeSwift object; refTrees are Newick strings
# Assume: 1. tree_obj has been rerooted at the desired center of the local search 
#         2. all ref trees have been preprocessed to be rooted at the missing taxon and removed it out
    subtrees = []
    close_leaves = []
    search_space = []
    for node in tree_obj.traverse_preorder():
        if node.is_root():
            node.d = 0
        else:
            node.d = node.parent.d + 1
        if node.d <= diam+1:
            search_space.append(node.label)
            if node.is_leaf():
                close_leaves.append(node.label)
            elif node.d == diam+1:            
                subtrees.append(node)    

    #print(diam,search_space)
    sample_refTrees = []
    for i in range(nsample):
        sample = [x for x in close_leaves]
        for u in subtrees:
            T = tree_obj.extract_subtree(u)
            nleaf = ceil(sqrt(len(list(T.traverse_leaves()))))
            s = sample_by_depth(T,nleaf,1,do_extract=False)
            sample += s       

        #nleaf = ceil(sqrt(len(list(tree_obj.traverse_leaves()))))
        #sample = sample_by_depth(tree_obj,nleaf,1,do_extract=False)    
        
        for treeStr in refTrees:
            r_tree = read_tree_newick(treeStr)
            sample_refTrees.append(r_tree.extract_tree_with(sample,suppress_unifurcations=True).newick())
    
    _,placement_label,tripScore = tripVote(tree_obj.newick(),sample_refTrees,search_space=set(search_space))
    placement_node = None
    for node in tree_obj.traverse_preorder():
        if node.label == placement_label:
            placement_node = node
            break
    if placement_node is None:
        placement_edge = (None,None)
    elif placement_node.is_root():
        u,v = placement_node.children
        placement_edge = (v.label,u.label)
    else:
        placement_edge = (placement_label,placement_node.parent.label)    
    return placement_edge, tripScore
    
def compute_d2root(myTree,placement_edge): 
    v_lb,u_lb = placement_edge    
    tree_obj = read_tree_newick(myTree)
    placement_node = None

    C = tree_obj.root.children
    if len(C) == 2 and ((C[0].label == v_lb and C[1].label == u_lb) or (C[1].label == v_lb and C[0].label == u_lb)):
        return 0

    for root in C:
        for node in root.traverse_preorder():
            if (node.label == v_lb and (u_lb is None or node.parent.label == u_lb)) or (node.label == u_lb and node.parent.label == v_lb):
                placement_node = node
                found = True
                break
        if placement_node is not None:
            break

    node = placement_node
    d = 0
    
    if node is None:
        print("Warning: Could not find optimal placement by tripVote. Aborting ...")
        return None

    while not node.is_root() and not node.parent.is_root():
        d += 1
        node = node.get_parent()
    return d    

def place_one_taxon_iter(myTree,refTrees,missing_taxon,max_depth='max',sample_size='sqrt',nsample=None):               
# get initial guess
    placement_label,score,d,rerooted_refTrees = place_one_taxon(myTree,refTrees,missing_taxon,max_depth=max_depth,sample_size=sample_size,nsample=nsample,use_brlen=False,alpha=0)
    print("Initial guess: " + placement_label + " " + str(score) + " " + str(d))
    
    tree_obj = read_tree_newick(myTree)
    diam = ceil(log2(len(list(tree_obj.traverse_leaves())))/2)
   
    placement_edge = None 
    if placement_label == tree_obj.root.label:
        u,v = tree_obj.root.children # assume the root has exactly 2 children
        placement_edge = (u.label,v.label)
    else:
        for node in tree_obj.traverse_preorder():
            if node.label == placement_label:
                placement_edge = (node.label,node.parent.label)
                break 
        if placement_edge is None:
            print("Warning: could not find placement by tripVote. Aborting ...")
            return placement_edge,d

# iterate
    while diam > 0:
        tree_obj = read_tree_newick(myTree)
        placement_label = placement_edge[0]
        # reroot at the previous placement site
        new_root_node = None
        for node in tree_obj.traverse_preorder():
            if node.label == placement_label:
                new_root_node = node
                break 
        if new_root_node is None:
            print("Warning: could not find placement by tripVote. Aborting ...")
            return placement_edge,d
        if not new_root_node is tree_obj.root:
            reroot_at_edge(tree_obj,new_root_node,new_root_node.edge_length/2,root_label="ROOT") 
        # find new placement
        placement_edge,_ = __local_search__(tree_obj,refTrees,diam,nsample=nsample)
        d = compute_d2root(myTree,placement_edge)
        print("Diameter " + str(diam) + ": " + str(placement_edge) + " " + str(d))
        # reduce d to go to the next iteration
        diam -= 1

    return placement_edge,d
        
def __reroot_ref_tree__(tree_obj,missing_taxon):                       
# tree_obj: a treeswift object for a reference tree
# reroot the tree at the missing_taxon
    new_root = None
    for leaf in tree_obj.traverse_leaves():
        if leaf.label == missing_taxon:
            new_root = leaf
            break     
    
    if new_root is None:
        return False
    
    reroot_at_edge(tree_obj,new_root,new_root.edge_length/2)  
    C = tree_obj.root.children
    c_star = None
    
    for c in C:
        if c.label == missing_taxon:
            c_star = c
            break
    if c_star is None:
        return False

    tree_obj.root.remove_child(c_star) 
    tree_obj.suppress_unifurcations()
    
    return True

def place_one_taxon(myTree,refTrees,missing_taxon,max_depth='max',sample_size='sqrt',nsample=None,use_brlen=False,pseudo=1e-3,alpha=0):
# remove all trees in refTrees that do not have the missing_taxon
# and assume myTree is missing the missing_taxon
# myTree: a newick string. Assume it has unique labeling for all nodes
# refTrees: a list of newick strings
# missing_taxon: a leaf label (string)
# max_depth: only count the triplets with depth up to max_depth.
# Special case:     max_depth = 'max': include all triplets (default)
#                   max_depth = 'log2': ceil(log2(n)) where n is the number of leaves

    rerooted_refTrees = []
    sample_refTrees = []
    W = []
    i = 0
    for rstr in refTrees:
        tree_obj = read_tree_newick(rstr)
        n = len(list(tree_obj.traverse_leaves()))
        
        if max_depth == 'max':
            D = n
        elif max_depth == 'log2':
            D = ceil(log2(n))
        else:
            D = int(max_depth)    
        
        if __reroot_ref_tree__(tree_obj,missing_taxon):           
            rerooted_refTrees.append(tree_obj.newick())
             
            if alpha != 0:
                qdist = quartet_distance(myTree,tree_obj.newick())
                w = exp(-alpha*qdist)
            else:
                w = 1    
            
            if D < n:
                prune_long(tree_obj,D)    
            
            sample_trees = []
            if (len(list(tree_obj.traverse_leaves())) >= 3):
                if nsample is None:
                    sample_trees = [tree_obj.newick()]
                else:
                    nleaf = len(list(tree_obj.traverse_leaves()))
                    if use_brlen:
                        sample_trees = sample_by_brlen(tree_obj,nleaf if sample_size == 'full' else ceil(sqrt(nleaf)),nsample,pseudo=1e-3)
                    else: 
                        sample_trees = sample_by_depth(tree_obj,nleaf if sample_size == 'full' else ceil(sqrt(nleaf)),nsample)
            sample_refTrees += sample_trees
            W += [w]*len(sample_trees)

    _,placement_label,tripScore = tripVote(myTree,sample_refTrees,W)
    tree_obj = read_tree_newick(myTree)
    placement_node = None
    for node in tree_obj.traverse_preorder():
        if node.label == placement_label:
            placement_node = node
            break
    node = placement_node
    d = 0
    if node is None:
        print("Warning: Could not find optimal placement by tripVote. Aborting ...")
        return None,None
    while not node.is_root() and not node.parent.is_root():
        d += 1
        node = node.get_parent()
    return placement_label, tripScore, d, rerooted_refTrees                    
