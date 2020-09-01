from subprocess import call, run
from tempfile import NamedTemporaryFile
# from ctypes import *
import sys
import os
import argparse

import time
start = time.time()

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")

# parser.add_argument('-i', '--input', required=False, default="test_cases/unrooted.trees", help="Input Unrooted Trees")
# parser.add_argument('-o', '--output', required=False, default="test_cases/wMVvote_rooted.trees", help="Output rooted Trees")
parser.add_argument('-w', '--weight', required=False, default="y", help="Weight")
args = parser.parse_args()

print("Step1a: running MinVar")

MVrootedTrees = "./test_cases/MV_rooted.trees"
weightsFile = "./test_cases/weights.txt"
call(["./bin/MVRoot", args.input, MVrootedTrees])
 
print("Step1b: computing weight matrix")

f = open(MVrootedTrees)
trees = f.readlines()
f.close()

call(["./bin/matrix_quartet_dist_to_ref", MVrootedTrees, weightsFile])

numTree = len(trees)
weightMatrix = [[0]*numTree for i in range(numTree)]

f = open(weightsFile)
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
for tree in trees:
    print("Processing tree {}".format(count))

    tempIn = NamedTemporaryFile(delete=False)
    tempOut = NamedTemporaryFile(delete=False) 
    tempWeights = NamedTemporaryFile(delete=False)

    tempIn.write(tree.encode()) # write() method takes input in bytes
    tempIn.close()

    f = open(tempWeights.name, 'w')
    f.writelines(weightMatrix[count - 1])
    f.close()
    # print(tempWeights.name)
    
    call(["./bin/tripVote", tempIn.name, tempOut.name, MVrootedTrees, tempWeights.name])
    
    f = open(tempOut.name, 'r')
    lines = f.readlines()
    f.close()
    
    f2 = open(args.output, 'a')
    f2.writelines(lines)
    f2.close()
    count += 1

    tempOut.close()
    tempWeights.close()

    os.remove(tempIn.name)
    os.remove(tempOut.name)
    os.remove(tempWeights.name)
      
    # exit()
        
end = time.time()
print(end - start) 
