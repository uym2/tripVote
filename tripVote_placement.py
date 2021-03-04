#! /usr/bin/env python

import sys
import os
import argparse
import time
from tripVote.placement_lib import place_one_taxon
from statistics import median
from math import exp, log


def main():
    MY_VERSION='1.0.4b'

    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--input', required=True, help="Input tree")
    parser.add_argument('-r', '--references', required=True, help="Reference trees")
    parser.add_argument('-p', '--placement', required=True, help="The taxon to do placement")
    parser.add_argument('-d', '--depth', required=False, help="The maximum depth of any voting triplet. Can be a positive integer or string 'max' or 'log2'. Default: max of tree height.")
    parser.add_argument('-s', '--sampling', required=False, help="The sample size  and number of sample. Default: do not do sampling")
    parser.add_argument('--usebrlen', action='store_true', help="Use branch length to do sampling. Default: FALSE")
    parser.add_argument('--pseudo', required=False, default=0.003, type=float, help="The pseudo count added to each branch length; only used with --usebrlen. Default: 1e-3.")
    parser.add_argument('--alpha', required=False, default=0, type=float, help="The parameter to control the weights per reference tree (w = exp(-alpha*qdist). Default: 0")
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

    lb,d2root = place_one_taxon(inputTree,refTrees,missing_taxon,max_depth=d,sample_size=sample_size,nsample=nsample,use_brlen=args.usebrlen,pseudo=args.pseudo,alpha=args.alpha) 

    print("Placement: " + str(lb) + " " + str(d2root))
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
