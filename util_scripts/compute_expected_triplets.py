#! /usr/bin/env python

import treeswift
from sys import argv
from math import exp
from heapq import *

def sort_edges_by_level_and_length(t):
# prioritize the edges such that 
# for any pair u,v: if u is an ancestor of v, the edge above u must have a higher priority than that of v;
#                   otherwise, the longer edge will have higher priority
# Note: "higher priority" means the node will be placed further behind the list
# We will do the opposite here: first order the nodes by its priority, then reverse the list afterward
# We do it this way to fit the list to the collapse_edges function
    heap = [(0,t.root)]
    brList = []
    
    while heap:
        _,u = heappop(heap)
        if not u.is_root():
            brList.append(u)
        for v in u.child_nodes():
            if not v.is_leaf():
                heappush(heap,(-v.get_edge_length(),v))      
    brList.reverse()
    return brList

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

def maxTrpl_collapse(myTree,N,respect_level=False):
# collapse myTree to maximize its expected normalized triplet score to gene trees
    nleaves = myTree.num_nodes(leaves=True,internal=False)
    
    expTrpl = expected_triplets(myTree,N)
    totalTrpl = nleaves*(nleaves-1)*(nleaves-2)/6

    orgScore = expTrpl/totalTrpl
    optScore = orgScore
    brList = sort_edges_by_level_and_length(myTree) if respect_level else sort_edges_by_length(myTree) 
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
    
    return orgScore, optScore
