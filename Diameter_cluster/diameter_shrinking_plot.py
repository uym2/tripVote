#! /usr/bin/env python

from treeshrink.optimal_filter_lib import TreeFilter
from sys import argv
import matplotlib.pyplot as plt
from math import *
from scipy.stats import lognorm
import numpy as np

TreeFile = argv[1]
myFilter = TreeFilter(tree_file=TreeFile,scaling=(0,0))
myFilter.optFilter(d=myFilter.nleaf)
x = []
y = []

d0 = myFilter.min_diams[0]

for i in range(1,len(myFilter.min_diams)):
    x.append(i)
    nu = log(myFilter.min_diams[i-1]/myFilter.min_diams[i])
    #nu = d0/myFilter.min_diams[i]
    y.append(nu)
    print(i,nu,sorted(myFilter.list_removals(d=i)))

sigma = np.std(np.array([log(m) for m in y]))
scale = exp(sum([log(m) for m in y])/len(y))

th = lognorm.ppf(0.75,sigma,0,scale)

print("Threshold: " + str(th))

filtered = set([])
ranking = {}
k = 10

with open("grouping.txt","w") as f:
    for i,nu in enumerate(y):
        S = set(myFilter.list_removals(d=i+1))
        for s in S:
            if s not in ranking:
                ranking[s] = i
        if nu > th:
            R = []
            for s in S:
                if s not in filtered:
                    R.append((ranking[s],s))                
            R.sort()
            print(i+1,nu,R)
            filtered = filtered.union(S)
            
            if len(R) < 3:
                continue
             
            j = k
            while (j < len(R)):
                for _,r in R[j-k:j]:
                    f.write(str(r) + " ")
                f.write("\n")
                j += 1
            
            for _,r in R[j-k:]:
                f.write(str(r) + " ")           
            f.write("\n") 
    
#plt.plot(x,y)     
#plt.show()    
