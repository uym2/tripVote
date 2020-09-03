from subprocess import call, run
from tempfile import NamedTemporaryFile
from shutil import copyfile
import sys
import os
import argparse
import multiprocessing

import time

def main():
    start = time.time()
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
    parser.add_argument('-o', '--output', required=True, help="Output Rooted Trees")
    parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")

    #parser.add_argument('-w', '--weight', required=False, default="y", help="Weight")
    args = parser.parse_args()

    print("Step1a: running MinVar")
    weightsFile = NamedTemporaryFile(delete=False)

    print(args.mv)

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
            
#mid = time.time()
#print(mid - start) 

    print("Step2: running all-pairs tripRoot")

    def tripvote(tree, count):
        print("Processing tree {}".format(count))

        tempIn = NamedTemporaryFile(delete=False)
        tempOut = NamedTemporaryFile(delete=False) 
        tempWeights = NamedTemporaryFile(delete=False)

        tempMV = NamedTemporaryFile(delete=False) 
        if args.mv == "temp":
            copyfile(MVrootedTrees.name, tempMV.name)
        else:
            copyfile(args.mv, tempMV.name)
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

    if args.mv == "temp": 
        os.remove(MVrootedTrees.name)
    os.remove(weightsFile.name)

if __name__ == "__main__": 
    multiprocessing.set_start_method('fork')
    main()
