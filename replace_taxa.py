#! /usr/bin/env python

from tripVote.placement_lib import replace_taxa
from sys import argv
from treeswift import *

with open(argv[1],'r') as fin:
    myTree = fin.read().strip()

with open(argv[2],'r') as fin:
    refTrees = fin.read().strip().split()

queries = argv[3:]

results = replace_taxa(myTree,refTrees,queries)

for (q,p) in results:
    print(q + " " + p)
