#! /usr/bin/env python

from compute_expected_triplets import maxTrpl_collapse, compute_reduced_triplets, preprocessing
import treeswift
from sys import argv
from math import exp
from edge_collapsing_lib import sort_edges_by_length, collapse_edge 

myTree = treeswift.read_tree_newick(argv[1])
N = float(argv[2])
outTree = argv[3]

before_totalTrpl, after_totalTrpl, before_expTrpl, after_expTrpl = maxTrpl_collapse(myTree,N,respect_level=False)

myTree.write_tree_newick(outTree)

print("Before collapsing: ")
print("Total triplets = " + str(before_totalTrpl))
print("Expected triplets = " + str(before_expTrpl))
print("After collapsing: " )
print("Total triplets: " + str(after_totalTrpl))
print("Expected triplets: " + str(after_expTrpl))

#print("Expected normalized triplet score increased from " + str(orgScore) + " to " + str(optScore))
#print("The contracted tree was written to " + str(outTree))
