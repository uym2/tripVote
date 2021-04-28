#! /usr/bin/env python

from tripVote.utils import distance_matrix,distance_to_refs
import argparse
from sys import stdout
    
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--input', required=True, help="Input trees.")
parser.add_argument('-r', '--ref', required=True, help="Reference trees.")
parser.add_argument('-m', '--metric', required=True, help="Either 'quartet' or 'triplet'")
parser.add_argument('-o', '--output', required=True, help="Output file")

args = parser.parse_args()

treefile = args.input
reffile = args.ref

with open(treefile,'r') as fin:
    myTrees = fin.read().strip().split("\n")

with open(reffile,'r') as fin:
    myRefs = fin.read().strip().split("\n")

with open(args.output,'w') as fout:
    distance_to_refs(myRefs,myTrees,args.metric,fout=fout)
