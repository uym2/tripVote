#! /usr/bin/env python

import sys
import os
import argparse
import time
from tripVote.tripvote_lib import *
import tripVote

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--input', required=True, help="Input Unrooted Trees")
    parser.add_argument('-r', '--references', required=False, default=None, help="Reference trees (i.e. OG, MV, MP). If not given, the input trees will be rooted by MV first, then the MV trees are used as references. Default: None")
    parser.add_argument('-d', '--depth', required=False, help="The maximum depth of any voting triplet. Can be a positive integer or string 'max' or 'log2'. Default: max of tree height.")
    parser.add_argument('-s', '--sampling', required=False, help="The sample size and number of sample. Default: sqrt 1")
    #parser.add_argument('--alpha', required=False, type=float,default=0,help="The alpha parameter of weights. Default: 0")
    parser.add_argument('-o', '--output', required=True, help="Output rooted Trees")
    #parser.add_argument('-m', '--mv', required=False, default="temp", help="MV Rooted Trees")
    parser.add_argument('-v', '--version',action='version', version=tripVote.PROGRAM_VERSION, help="Show program version and exit")

    args = parser.parse_args()

    start = time.time()
    print("Running " + tripVote.PROGRAM_NAME + " version " + tripVote.PROGRAM_VERSION)
    print("tripVote was called as follow: " + " ".join(sys.argv))

    trees = []
    with open(args.input,'r') as f:
        for line in f:
            trees.append(line.strip())
    
    if args.depth is None:
        d = "max"
    elif args.depth == 'max' or args.depth == 'log2':
        d = args.depth
    else:
        d = int(args.depth)

    #sample_size = 'sqrt'
    #nsample = 1
    sample_size = None
    nsample = None
    if args.sampling is not None:
        sample_size, nsample = args.sampling.strip().split()
        nsample = int(nsample)   
        if sample_size != 'sqrt' and sample_size != 'full':     
            sample_size = int(sample_size)

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
        #if args.mv is not None:
        #    refFile = args.mv
        #    with open(refFile,'w') as fout:
        #        for tree in reftrees:
        #            fout.write(tree + "\n")

    print("Step2: running all-pairs tripRoot")
    count = 1
    outtrees = []

    for tree in trees:
        print("Processing tree {}".format(count))
        rerooted,d2root = tripVote_root(tree,reftrees,max_depth=d,sample_size=sample_size,nsample=nsample) #,alpha=args.alpha)
        outtrees.append(rerooted)
        print("Distance to original root: " + str(d2root))
        count += 1

    with open(args.output,'w') as fout:
        for outtree in outtrees:
            fout.write(outtree + "\n")
    
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
