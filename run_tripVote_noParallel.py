#! /usr/bin/env python

from subprocess import call, run
from tempfile import NamedTemporaryFile
import sys
import os
import argparse

import time
start = time.time()

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
parser.add_argument('-r', '--references', required=False, default=None, help="Reference trees (i.e. OG, MV, MP). If not given, the input trees will be rooted by MV first, then the MV trees are used as references. Default: None")
parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")
parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")
parser.add_argument('-w', '--weight', required=False, default=None, help="A file that contains the weight matrix. Given as a LOWER triangular matrix")
args = parser.parse_args()

print("Step1a: processing reference trees")

if args.references is not None:
    refTrees = args.references
else:
    print("Running MV to use as references")
    if args.mv == "temp":
        MVrootedTrees = NamedTemporaryFile(mode='w+t', delete=False)
        call(["MVRoot", args.input, MVrootedTrees.name])
        refTrees = MVrootedTrees.name
    else: 
        call(["MVRoot", args.input, args.mv])
        refTrees = args.mv

print("Step1b: computing weight matrix")

f = open(refTrees)    
trees = f.readlines()
f.close()

weightsFileTemp = NamedTemporaryFile(mode='w+t', delete=False) if args.weight is None else None
weightsFile = weightsFileTemp.name if weightsFileTemp else args.weight

if weightsFileTemp:
    call(["matrix_quartet_dist_to_ref", refTrees, weightsFile])

numTree = len(trees)
weightMatrix = [[0]*numTree for i in range(numTree)]

f = open(weightsFile)
ws = f.read().split()
k = 0
for i in range(numTree):
    for j in range(i+1):
        weightMatrix[i][j] = float(ws[k])
        k += 1
        
for i in range(numTree):
    for j in range(i+1,numTree):        
        weightMatrix[i][j] = weightMatrix[j][i]
f.close()

print("Step2: running all-pairs tripRoot")
count = 1
lines = []
for tree in trees:
    print("Processing tree {}".format(count))

    tempIn = NamedTemporaryFile(mode='w+t', delete=False)
    tempOut = NamedTemporaryFile(mode='w+t', delete=False) 
    tempWeights = NamedTemporaryFile(mode='w+t', delete=False)

    tempIn.write(tree) # write() method takes input in bytes
    tempIn.close()
    
    tempWeights.writelines("%s\n" % x for x in weightMatrix[count - 1])
    tempWeights.close()
        
    call(["tripVote", tempIn.name, tempOut.name, refTrees, tempWeights.name])
    
    tempOut.seek(0)
    line = tempOut.readlines()[0]
    tempOut.close()

    lines.append(line)

    count += 1

    os.remove(tempIn.name)
    os.remove(tempOut.name)
    os.remove(tempWeights.name)

if args.references is None and args.mv == "temp":
    os.remove(MVrootedTrees.name)
    
f2 = open(args.output, 'w')
f2.writelines(lines)
f2.close()   

end = time.time()
print("Runtime: ", end - start) 
