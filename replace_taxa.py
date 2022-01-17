#! /usr/bin/env python

from tripVote.placement_lib import replace_taxa
from sys import argv
from treeswift import *

with open(argv[1],'r') as fin:
    myTree = fin.read().strip()

with open(argv[2],'r') as fin:
    refTrees = fin.read().strip().split()

nsample=int(argv[3])
queries = argv[4:]

results = replace_taxa(myTree,refTrees,queries,nsample=nsample)

for (q,p) in results:
    print(q + " " + p)
