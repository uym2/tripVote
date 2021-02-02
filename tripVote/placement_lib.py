from tqdist import *
from triproot import *
from tripVote.tripvote_lib import reroot_at_edge,tripVote 
from treeswift import *

def place_one_taxon(myTree,refTrees,missing_taxon):
# remove all trees in refTrees that do not have the missing_taxon
# and assume myTree is missing the missing_taxon
# myTree: a newick string. Assume it has unique labeling for all nodes
# refTrees: a list of newick strings
# missing_taxon: a leaf label (string)
    rerooted_refTrees = []
    W = []
    for rstr in refTrees:
        tree_obj = read_tree_newick(rstr)
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
            rerooted = tree_obj.newick()
            rerooted_refTrees.append(rerooted)
            W.append(quartet_distance(myTree,rerooted)) 
    _,_,placement_label = tripVote(myTree,rerooted_refTrees,W)
    tree_obj = read_tree_newick(myTree)
    placement_node = None
    for node in tree_obj.traverse_preorder():
        if node.label == placement_label:
            placement_node = node
            break
    node = placement_node
    d = 0
    while not node.is_root():
        node = node.get_parent()
        d += 1
    return placement_label, d                    
