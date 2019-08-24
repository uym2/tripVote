#! /usr/bin/env python

from edge_collapsing_lib import sort_edges_by_level_and_length, collapse_portion_edges
from sys import argv
from dendropy import Tree

myTree = Tree.get_from_path(argv[1],"newick")
out_prefix = argv[2]
portions = sorted([float(x) for x in argv[3:]])

brList = sort_edges_by_level_and_length(myTree)

for node in brList:
    print(node.label,node.edge_length)

collapse_portion_edges(myTree, portions, brList = brList, out_prefix = out_prefix)
