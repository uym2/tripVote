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
    #parser.add_argument('-o', '--output', required=True, help="Output file")
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
    lb,d = place_one_taxon(inputTree,refTrees,missing_taxon) 

    print(lb,d)
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
