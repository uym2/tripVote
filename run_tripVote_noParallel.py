#! /usr/bin/env python

from subprocess import call, run
from tempfile import NamedTemporaryFile
import sys
import os
import argparse
import time
from tqdist import quartet_distance
from triproot import MVroot

MY_VERSION='1.0.2b'

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
parser.add_argument('-r', '--references', required=False, default=None, help="Reference trees (i.e. OG, MV, MP). If not given, the input trees will be rooted by MV first, then the MV trees are used as references. Default: None")
parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")
parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")
parser.add_argument('-w', '--weight', required=False, default=None, help="A file that contains the weight matrix. Given as a LOWER triangular matrix")
parser.add_argument('-v', '--version',action='version', version=MY_VERSION, help="Show program version and exit")

args = parser.parse_args()

start = time.time()
print("Running tripVote version " + MY_VERSION)
print("tripVote was called as follow: " + " ".join(sys.argv))

trees = []
with open(args.input,'r') as f:
    for line in f:
        trees.append(line.strip())

print("Step1a: processing reference trees")
reftrees = []
if args.references is not None:
    self_vote = False
    refFile = args.references
    with open(refFile,'r') as f:
        for line in f:
            reftrees.append(line.strip())
else:
    self_vote = True    
    print("Running MV to use as references")
    for tree in trees:
        reftrees.append(MVroot(tree))
    if args.mv is not None:
        refFile = args.mv
        with open(refFile,'w') as fout:
            for tree in reftrees:
                fout.write(tree + "\n")

print("Step1b: computing weight matrix")
n = len(trees)
m = len(reftrees)

weightMatrix = [[0]*m for i in range(n)]

if self_vote:
    for i in range(n):
        for j in range(i+1):
            weightMatrix[i][j] = quartet_distance(trees[i],reftrees[j])
            weightMatrix[j][i] = weightMatrix[i][j]
else:
    for i in range(n):
        for j in range(m):
            weightMatrix[i][j] = quartet_distance(trees[i],reftrees[j])

print("Step2: running all-pairs tripRoot")
count = 1
lines = []
for tree in trees:
    print("Processing tree {}".format(count))

    tempIn = NamedTemporaryFile(mode='w+t', delete=False)
    tempOut = NamedTemporaryFile(mode='w+t', delete=False) 
    tempWeights = NamedTemporaryFile(mode='w+t', delete=False)

    tempIn.write(tree + "\n") # write() method takes input in bytes
    tempIn.close()
    
    tempWeights.writelines("%s\n" % x for x in weightMatrix[count - 1])
    tempWeights.close()
        
    call(["tripVote", tempIn.name, tempOut.name, refFile, tempWeights.name])
    
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
