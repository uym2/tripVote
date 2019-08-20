#! /usr/bin/env python

from dendropy import Tree
from sys import argv

def sort_edges(t):
   brList = []

   for node in t.postorder_node_iter():
        if not node.is_leaf() and node is not t.seed_node:
            brList.append(node) 
   brList.sort(key=lambda node:node.edge_length)

   return brList 

def collapse_edges(myTree, nBrs, out_prefix="collaped", brList = None):
    nBrs.sort()
    if brList is None:
        brList = sort_edges(myTree)

    i = 0
    for n,lb in nBrs:
        if n > len(brList):
            return False
            break
        while i < n:
            collapse_edge(myTree,brList[i])
            i += 1
        myTree.write_to_path(out_prefix + "_" + str(lb) + ".tre","newick")    

    return True

def collapse_edge(t,v):
# collapse the branch above node v in tree t
    if v.is_leaf() or v is t.seed_node:
        return False
    
    u = v.parent_node
    u.remove_child(v)
    
    Children = v.child_nodes()
    
    for c in Children:
        l = c.edge_length
        v.remove_child(c)
        u.add_child(c)
        c.parent_node = u
        c.edge_length = l
    
    return True    

def collapse_portion_edges(myTree, portions, out_prefix="collapsed"):
# collapse p*N edges for p in portions where N is the number of internal edges in myTree
# note: 0 < p < 1 for p in portions. If the list portions contains invalid values, the code will automatically remove them
    brList = sort_edges(myTree)
    N = len(brList)

    portions.sort()
    nBrs = [(round(p*N),p) for p in portions if p > 0 and p < 1] # only process valid values p
    
    collapse_edges(myTree, nBrs, out_prefix = out_prefix, brList = brList)
