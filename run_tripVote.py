#! /usr/bin/env python

from subprocess import call, run
from tempfile import NamedTemporaryFile
from shutil import copyfile
import sys
import os
import argparse
import multiprocessing
import time

if __name__ == "__main__": 
    multiprocessing.set_start_method('fork')
    start = time.time()
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
    parser.add_argument('-o', '--output', required=True, help="Output Rooted Trees")
    parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")

    #parser.add_argument('-w', '--weight', required=False, default="y", help="Weight")
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

    print("Step2: running all-pairs tripRoot")

    def tripvote(tree, count):
        print("Processing tree {}".format(count))

        tempIn = NamedTemporaryFile(mode='w+t', delete=False) # text data
        tempOut = NamedTemporaryFile(mode='w+t', delete=False) 
        tempWeights = NamedTemporaryFile(mode='w+t', delete=False)

        # tempMV = NamedTemporaryFile(delete=False) 
        # if args.mv == "temp":
        #     copyfile(MVrootedTrees.name, tempMV.name)
        # else:
        #     copyfile(args.mv, tempMV.name)

        tempIn.write(tree) # write() method takes input in bytes
        tempIn.close()
        
        tempWeights.writelines(weightMatrix[count - 1])
        tempWeights.close()
    
        # call(["./bin/tripVote", tempIn.name, tempOut.name, tempMV.name, tempWeights.name])
        if args.mv == "temp":
            call(["./bin/tripVote", tempIn.name, tempOut.name, MVrootedTrees, tempWeights.name])
        else:
            call(["./bin/tripVote", tempIn.name, tempOut.name, args.mv, tempWeights.name])
                
        tempOut.seek(0)
        line = tempOut.read()
        tempOut.close()

        os.remove(tempIn.name)
        os.remove(tempOut.name)
        os.remove(tempWeights.name)
        # os.remove(tempMV.name)
        return line

    counts = [x for x in range(1, numTree + 1)]
    pool = multiprocessing.Pool(6)
    results = pool.starmap(tripvote, zip(trees, counts))
    pool.close()
    pool.join()

    f3 = open(args.output, 'w')
    f3.writelines(results)
    f3.close()

    end = time.time()
    print(end - start) 

    if args.mv == "temp": 
        os.remove(MVrootedTrees.name)
    os.remove(weightsFile.name)
