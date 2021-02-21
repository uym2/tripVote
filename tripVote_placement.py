#! /usr/bin/env python

import sys
import os
import argparse
import time
from tripVote.placement_lib import place_one_taxon
from statistics import median
from math import exp, log


def main():
    MY_VERSION='1.0.2b'

    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--input', required=True, help="Input tree")
    parser.add_argument('-r', '--references', required=True, help="Reference trees")
    parser.add_argument('-p', '--placement', required=True, help="The taxon to do placement")
    parser.add_argument('-d', '--depth', required=False, help="The maximum depth of any voting triplet. Can be a positive integer or string 'max' or 'log2'. Default: max of tree height.")
    parser.add_argument('-s', '--sampling', required=False, help="The sample size depth of any voting triplet. Can be a positive integer or string 'max' or 'log2'. Default: log2 of treesize.")
    parser.add_argument('-v', '--version',action='version', version=MY_VERSION, help="Show program version and exit")

    args = parser.parse_args()

    start = time.time()
    print("Running tripVote_placement version " + MY_VERSION)
    print("tripVote_placement was called as follow: " + " ".join(sys.argv))

    with open(args.input,'r') as f:
        inputTree = f.read().strip()

    with open(args.references,'r') as f:
        refTrees = f.read().strip().split('\n')

    missing_taxon = args.placement
    if args.depth is None:
        d = "max"
    elif args.depth == 'max' or args.depth == 'log2':
        d = args.depth
    else:
        d = int(args.depth)

    sample_size = None
    nsample = None
    if args.sampling is not None:
        sample_size, nsample = args.sampling.strip().split()
        nsample = int(nsample)        

    lb,d = place_one_taxon(inputTree,refTrees,missing_taxon,max_depth=d,sample_size=sample_size,nsample=nsample) 

    print("Placement: " + str(lb) + " " + str(d))
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
