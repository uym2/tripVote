#! /usr/bin/env python

from tripVote.utils import distance_matrix
import argparse
    
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--input', required=True, help="Input trees. Allow multiple files")
parser.add_argument('-m', '--metric', required=True, help="Either 'quartet' or 'triplet'")
parser.add_argument('--prefix', required=False, help="Must be of the same length as input (-i)")
parser.add_argument('-o', '--output', required=True, help="Output file")

args = parser.parse_args()

treefiles = args.input.split()
prefixes = args.prefix.split() if args.prefix is not None else None

myTrees = []
myIDs = []
M = len(treefiles)

for i,treefile in enumerate(treefiles):
    if prefixes is not None:
        prfx = prefixes[i]
    elif M > 1:
        prfx = "group" + str(i+1) 
    else:
        prfx = ''
    with open(treefile,'r') as f:
        trees = f.read().split("\n")[:-1]
    N = len(trees)
    if N > 1:
        names = [ prfx + "_" + str(j+1) for j in range(N) ]  
    else:
        names = [prfx]
    myTrees += trees
    myIDs += names       

#D = distance_matrix(myTrees,args.metric,IDs=myIDs)
with open(args.output,'w') as fout: 
    distance_matrix(myTrees,args.metric,IDs=myIDs,fout=fout)
    #for t1,t2 in D:
    #    fout.write(t1 + " " + t2 + " " + str(D[(t1,t2)]) + "\n")
