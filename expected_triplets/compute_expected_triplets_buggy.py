#! /usr/bin/env python

import treeswift
from sys import argv
from math import exp

myTree = treeswift.read_tree_newick(argv[1])
N = float(argv[2])
nTrpl = 0

for node in myTree.traverse_postorder():
    if node.is_leaf():
        node.n = 1
        node.P = 0
        node.T = 0
    else:
        node.n = sum([c.n for c in node.child_nodes()])
        if not node.is_root():
            l = node.get_edge_length()/N
            Pc = sum([c.P for c in node.child_nodes()])
            node.P = (node.n*(node.n-1)/2 - Pc)*(1-exp(-l)) + Pc
        
        node.T = 0
        for c in node.child_nodes():
            lc = c.get_edge_length()/N
            node.T += c.P*(node.n-c.n) + (c.n*(c.n-1)/2 - c.P)*(node.n-c.n)*exp(-lc)/3
    nTrpl += node.T
    
print(nTrpl)            

