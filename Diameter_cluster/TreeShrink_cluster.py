#! /usr/bin/env python

from treeshrink.optimal_filter_lib import TreeFilter
from treeshrink.tree_lib import prune_tree
from sys import argv
import matplotlib.pyplot as plt
from math import *
from scipy.stats import lognorm
import numpy as np
from dendropy import Tree

TreeFile = argv[1]
outFile = argv[2]

myTree = Tree.get_from_path(TreeFile,schema='newick')

# TreeShrink round 0 (outlier detection)
myFilter = TreeFilter(ddpTree=myTree,scaling=(5,2))
myFilter.optFilter()
x = []
y = []

for i in range(1,len(myFilter.min_diams)):
    x.append(i)
    nu = myFilter.min_diams[i-1]/myFilter.min_diams[i]
    y.append(nu)
    #print(i,nu,sorted(myFilter.list_removals(d=i)))

sigma = np.std(np.array([log(m) for m in y]))
scale = exp(sum([log(m) for m in y])/len(y))

th = lognorm.ppf(0.95,sigma,0,scale)

RS = [] # removing set
for i in range(len(y)-1,-1,-1):
    nu = y[i]
    if nu > th:
        RS = myFilter.list_removals(d=i+1)
        break

# output RS
# remove leaves in RS from myTree
prune_tree(myTree,RS)

# TreeShrink round 1,2,...  
n = myFilter.nleaf - len(RS)
k = min(n,int(4/3*sqrt(n)))
m = 6
#k = int(sqrt(n))
ranking = {}
curr_rank = 0

with open(outFile,'w') as fout:
    while n > k+1:
        print(n,k)
        myFilter = TreeFilter(ddpTree=myTree,scaling=(0,0))
        myFilter.optFilter(d=k)


        R = []
        for i in range(k):
            for s in myFilter.list_removals(d=i+1):
                if s not in ranking:
                    ranking[s] = curr_rank + i            
        
        RS = myFilter.list_removals(d=k)
        R = []
        for s in RS:
            R.append((ranking[s],s))
        R.sort()

        j = m

        while (j < len(R)):
            for _,r in R[j-m:j]:
                fout.write(str(r) + " ")
            fout.write("\n")
            j += 1    

        # remove leaves in RS from myTree
        prune_tree(myTree,RS)
        n = myFilter.nleaf - len(RS) 
        
        curr_rank += k+1

    # the last RS is the set of the remaining leaves in myTree
    #RS = [ f.taxon.label for f in myTree.leaf_nodes() ]
    # output the last RS
    #for r in RS:
    #    fout.write(r + "\t") 
