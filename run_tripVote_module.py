#! /usr/bin/env python

import sys
import os
import argparse
import time
from tripVote.tripvote_lib import *

def main():
    MY_VERSION='1.0.2b'

    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
    parser.add_argument('-r', '--references', required=False, default=None, help="Reference trees (i.e. OG, MV, MP). If not given, the input trees will be rooted by MV first, then the MV trees are used as references. Default: None")
    parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")
    parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")
    parser.add_argument('-w', '--weight', required=False, default=None, help="A file that contains the weight matrix. Given as a LOWER triangular matrix")
    parser.add_argument('-v', '--version',action='version', version=MY_VERSION, help="Show program version and exit")
    parser.add_argument('-a', '--alpha',default=10,type=float,help="Weighting hyper-parameter")

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
    alpha = args.alpha
    if alpha != 0:
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
    outtrees = []
    for tree in trees:
        print("Processing tree {}".format(count))
        
        t0 = time.time()
        rerooted,_,_ = tripVote(tree,reftrees,weightMatrix[count-1],alpha=alpha)
        print("tripVote runtime: " + str(time.time()-t0))
        outtrees.append(rerooted)

        count += 1

    with open(args.output,'w') as fout:
        for outtree in outtrees:
            fout.write(outtree + "\n")
    
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
