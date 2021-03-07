from tqdist import *
from triproot import *
from tripVote.tripvote_lib import tripVote 
from tripVote.utils import reroot_at_edge, sample_by_depth, prune_long, sample_by_brlen
from treeswift import *
from math import log2, ceil, sqrt, exp
from random import choices

                

def place_one_taxon(myTree,refTrees,missing_taxon,max_depth='max',sample_size='full',nsample=None,use_brlen=False,pseudo=1e-3,alpha=0):
# remove all trees in refTrees that do not have the missing_taxon
# and assume myTree is missing the missing_taxon
# myTree: a newick string. Assume it has unique labeling for all nodes
# refTrees: a list of newick strings
# missing_taxon: a leaf label (string)
# max_depth: only count the triplets with depth up to max_depth.
# Special case:     max_depth = 'max': include all triplets (default)
#                   max_depth = 'log2': ceil(log2(n)) where n is the number of leaves

    rerooted_refTrees = []
    W = []
    i = 0
    for rstr in refTrees:
        tree_obj = read_tree_newick(rstr)
        n = len(list(tree_obj.traverse_leaves()))
        if max_depth == 'max':
            d = n
        elif max_depth == 'log2':
            d = ceil(log2(n))
        else:
            d = int(max_depth)    
        new_root = None
        for leaf in tree_obj.traverse_leaves():
            if leaf.label == missing_taxon:
                new_root = leaf
                break     
        if new_root is not None:
            reroot_at_edge(tree_obj,new_root,new_root.edge_length/2)  
            C = tree_obj.root.children
            c_star = None
            for c in C:
                if c.label == missing_taxon:
                    c_star = c
                    break
            if c_star is not None:
                tree_obj.root.remove_child(c_star) 
            tree_obj.suppress_unifurcations()
            
            if alpha != 0:
                qdist = quartet_distance(myTree,tree_obj.newick())
                w = exp(-alpha*qdist)
            else:
                w = 1    
            
            if d < n:
                prune_long(tree_obj,d)    
            
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
            rerooted_refTrees += sample_trees
            W += [w]*len(sample_trees)

    _,_,placement_label = tripVote(myTree,rerooted_refTrees,W)
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
    return placement_label, d                    
