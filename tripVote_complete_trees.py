#! /usr/bin/env python

import sys
import os
import argparse
import time
from tripVote.placement_lib import place_one_taxon, place_one_taxon_iter, complete_gene_trees
from math import exp, log
import random
from treeswift import *
import tripVote

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--input', required=True, help="Input trees to be completed")
    parser.add_argument('-r', '--refs', required=False, help="Reference trees that are used to complete input trees. Default: None, so use the other trees in myTrees to complete each of them.")
    parser.add_argument('-o', '--output', required=True, help="Output trees")
    parser.add_argument('-v', '--version',action='version', version=tripVote.PROGRAM_VERSION, help="Show program version and exit")
    parser.add_argument('-s', '--sampling', required=False, help="The sample size (s) and the number of samples (r). E.g: to set s=10 and r=9, then put in -s \"10 9\", including the quotes. Set s and r to 0 to NOT do sampling. Default: s=sqrt(n) and r=90/s.")
    
    args = parser.parse_args()

    random.seed(a=1105)

    start = time.time()
    print("Running " + tripVote.PROGRAM_NAME + " version " + tripVote.PROGRAM_VERSION)
    print("tripVote_complete_trees was called as follow: " + " ".join(sys.argv))
    
    sample_size = 'sqrt' # default
    nsample = 'default' # default: 90/sample_size
    if args.sampling is not None:
        sample_size, nsample = args.sampling.strip().split()
        if sample_size != 'sqrt' and sample_size != 'full':
            sample_size = int(sample_size)
        nsample = int(nsample)        

    with open(args.input,'r') as f:
        inputTrees = f.read().strip().split("\n")

    if args.refs is None:
        refTrees = None
    else:
        with open(args.refs,'r') as f:
            refTrees = f.read().strip().split("\n")
            

    inputTrees_nobrlen = []
    for treeStr in inputTrees:
        tree_obj = read_tree_newick(treeStr)
        for node in tree_obj.traverse_preorder():
            node.edge_length = None
        inputTrees_nobrlen.append(tree_obj.newick())

    outputTrees = complete_gene_trees(inputTrees_nobrlen,refTrees=refTrees,sample_size=sample_size,nsample=nsample)
    with open(args.output,'w') as f:
        f.write('\n'.join(outputTrees))
    
    end = time.time()
    print("Runtime: ", end - start) 

if __name__ == "__main__":
    main()    
