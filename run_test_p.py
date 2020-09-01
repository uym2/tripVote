from subprocess import call, run
from tempfile import NamedTemporaryFile
from shutil import copyfile
import sys
import os
import argparse
import multiprocessing

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

# count = 1

def tripvote(tree, count):
    # global count
    print("Processing tree {}".format(count))

    tempIn = NamedTemporaryFile(delete=False)
    tempOut = NamedTemporaryFile(delete=False) 
    tempWeights = NamedTemporaryFile(delete=False)

    tempMV = NamedTemporaryFile(delete=False) 
    copyfile(MVrootedTrees, tempMV.name)

    tempIn.write(tree.encode()) # write() method takes input in bytes
    tempIn.close()

    f1 = open(tempWeights.name, 'w')
    f1.writelines(weightMatrix[count - 1])
    f1.close()
    # print(tempWeights.name)
    
    call(["./bin/tripVote", tempIn.name, tempOut.name, tempMV.name, tempWeights.name])
    # call(["./bin/tripVote", tempIn.name, tempOut.name, MVrootedTrees, tempWeights.name])
    
    f2 = open(tempOut.name, 'r')
    line = f2.readline()
    f2.close()

    tempOut.close()
    tempWeights.close()

    os.remove(tempIn.name)
    os.remove(tempOut.name)
    os.remove(tempWeights.name)
    os.remove(tempMV.name)
    return line

counts = [x for x in range(1, numTree + 1)]
cores = os.cpu_count()
pool = multiprocessing.Pool(2)
# results = pool.starmap(tripvote, zip(trees, counts))
results = pool.starmap(tripvote, zip(trees, counts))
pool.close()
pool.join()

# print(results)
f3 = open(args.output, 'w')
f3.writelines(results)
f3.close()

end = time.time()
print(end - start) 

os.remove(MVrootedTrees)
os.remove(weightsFile)