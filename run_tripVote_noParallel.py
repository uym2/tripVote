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
parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")
parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")
# parser.add_argument('-w', '--weight', required=False, default="y", help="Weight")
args = parser.parse_args()

print("Step1a: running MinVar")
weightsFile = NamedTemporaryFile(delete=False)

if args.mv == "temp":
    MVrootedTrees = NamedTemporaryFile(delete=False)
    call(["./bin/MVRoot", args.input, MVrootedTrees.name])
else: 
    call(["./bin/MVRoot", args.input, args.mv])

print("Step1b: computing weight matrix")

if args.mv == "temp":
    f = open(MVrootedTrees.name)
else:
    f = open(args.mv)
trees = f.readlines()
f.close()

if args.mv == "temp":
    call(["./bin/matrix_quartet_dist_to_ref", MVrootedTrees.name, weightsFile.name])
else:
    call(["./bin/matrix_quartet_dist_to_ref", args.mv , weightsFile.name])

numTree = len(trees)
weightMatrix = [[0]*numTree for i in range(numTree)]

f = open(weightsFile.name)
for i in range(numTree):
    for j in range(numTree):
        if i <= j:
            weightMatrix[i][j] = f.readline()   # line is str
        else:
            weightMatrix[i][j] = weightMatrix[j][i]
f.close()
        
mid = time.time()
print(mid - start) 

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

    tempWeights.writelines(weightMatrix[count - 1])
    tempWeights.close()
    
    if args.mv == "temp":
        call(["./bin/tripVote", tempIn.name, tempOut.name, MVrootedTrees.name, tempWeights.name])
    else:
        call(["./bin/tripVote", tempIn.name, tempOut.name, args.mv, tempWeights.name])
                    
    tempOut.seek(0)
    line = tempOut.readlines()[0]
    tempOut.close()

    lines.append(line)

    count += 1

    os.remove(tempIn.name)
    os.remove(tempOut.name)
    os.remove(tempWeights.name)

f2 = open(args.output, 'a')
f2.writelines(lines)
f2.close()   

end = time.time()
print(end - start) 
