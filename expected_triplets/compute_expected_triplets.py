#! /usr/bin/env python

import treeswift
from sys import argv
from math import exp

myTree = treeswift.read_tree_newick(argv[1])
N = float(argv[2])
nTrpl = 0

def count_pairs(node):
    children = node.child_nodes()
    count = 0
    for i in range(len(children)-1):
        for j in range(i+1,len(children)):
            count += children[i].n * children[j].n
    return count              

def __expected_pairs__(node,height,N):
    if node.is_leaf():
        return 0
    l = (height+node.get_edge_length())/N        
    expected_pairs = count_pairs(node)*(1-2*exp(-l)/3)

    for c in node.child_nodes():
        expected_pairs += __expected_pairs__(c,height+node.get_edge_length(),N) 
        
    return expected_pairs   

for node in myTree.traverse_postorder():
    if node.is_leaf():
        node.n = 1
        node.P = 0
        node.T = 0
    else:
        node.n = sum([c.n for c in node.child_nodes()])
        node.P = __expected_pairs__(node,0,N) if not node.is_root() else 0
        node.T = 0
        for c in node.child_nodes():
            node.T += c.P * (node.n - c.n)
    nTrpl += node.T
    
print(nTrpl)            

