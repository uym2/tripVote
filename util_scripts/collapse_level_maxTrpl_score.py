#! /usr/bin/env python

from compute_expected_triplets import maxTrpl_collapse
import treeswift
from sys import argv
from math import exp

myTree = treeswift.read_tree_newick(argv[1])
N = float(argv[2])
outTree = argv[3]

orgScore, optScore = maxTrpl_collapse(myTree,N,respect_level=True)
myTree.write_tree_newick(outTree)
print("Expected normalized triplet score increased from " + str(orgScore) + " to " + str(optScore))
print("The contracted tree was written to " + str(outTree))
