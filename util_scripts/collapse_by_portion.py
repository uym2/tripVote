#! /usr/bin/env python

from edge_collapsing_lib import collapse_portion_edges
from sys import argv
from dendropy import Tree

myTree = Tree.get_from_path(argv[1],"newick")
out_prefix = argv[2]
portions = sorted([float(x) for x in argv[3:]])

collapse_portion_edges(myTree, portions, out_prefix = out_prefix)

