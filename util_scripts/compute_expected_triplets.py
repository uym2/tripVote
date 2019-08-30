#! /usr/bin/env python

import treeswift
from sys import argv
from math import exp
from edge_collapsing_lib import sort_edges_by_length, collapse_edge 

def count_pairs(node):
    children = node.child_nodes()
    count = 0
    for i in range(len(children)-1):
        for j in range(i+1,len(children)):
            count += children[i].n * children[j].n
    return count              

def expected_pairs(node,height,N):
    if node.is_leaf():
        return 0
    l = (height+node.get_edge_length())/N        
    EPs = count_pairs(node)*(1-2*exp(-l)/3)

    for c in node.child_nodes():
        EPs += expected_pairs(c,height+node.get_edge_length(),N) 
        
    return EPs   


def expected_triplets(myTree,N):
    nTrpl = 0
    for node in myTree.traverse_postorder():
        if node.is_leaf():
            node.n = 1
            node.P = 0
            node.T = 0
        else:
            node.n = sum([c.n for c in node.child_nodes()])
            node.P = expected_pairs(node,0,N) if not node.is_root() else 0
            node.T = 0
            for c in node.child_nodes():
                node.T += c.P * (node.n - c.n)
        nTrpl += node.T
    return nTrpl    

def sort_edges_by_length(myTree):
   brList = []

   for node in myTree.traverse_postorder():
        if not node.is_leaf() and not node.is_root():
            brList.append(node) 
   brList.sort(key=lambda node:node.get_edge_length())

   return brList 

def collapse_edge(myTree,v):
# collapse the branch above node v in tree t
    if v.is_leaf() or v.is_root():
        return False
    
    u = v.get_parent()
    u.remove_child(v)
    
    Children = v.child_nodes()
    
    for c in Children:
        l = c.edge_length
        v.remove_child(c)
        u.add_child(c)
        c.parent_node = u
        c.edge_length = l
    
    return True    

def maxTrpl_collapse(myTree,N):
# collapse myTree to maximize its expected normalized triplet score to gene trees
    nleaves = myTree.num_nodes(leaves=True,internal=False)
    
    expTrpl = expected_triplets(myTree,N)
    totalTrpl = nleaves*(nleaves-1)*(nleaves-2)/6

    optScore = expTrpl/totalTrpl
    brList = sort_edges_by_length(myTree)
    cList = []

    for u in brList:
        l = u.get_edge_length()/N
        D = count_pairs(u)*(u.get_parent().n - u.n) 
        newExp = expTrpl - (1-2*exp(-l)/3)*D
        newTotal = totalTrpl - D
        newScore = newExp/newTotal

        if newScore > optScore:
            optScore = newScore
            expTrpl = newExp
            totalTrpl = newTotal
            cList.append(u)
        else:
            break
    
    for u in cList:
        u.contract() 
    
myTree = treeswift.read_tree_newick(argv[1])
N = float(argv[2])
outTree = argv[3]

maxTrpl_collapse(myTree,N)
myTree.write_tree_newick(outTree)
