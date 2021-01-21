#! /usr/bin/env python

from sys import argv
import os
import time
from tripVote.tripvote_lib import *
from triproot import *
from tqdist import *
import random
from treeswift import *

def alpha_selection(myTree,voteTrees,alphas,nbootstraps):
# input: assume the trees have unique labeling.
    myTree_obj = read_tree_newick(myTree)
    root_ids = [x.label for x in myTree_obj.root.children + [myTree_obj.root]]

# run tripRootScore and compute quartet distances
    tripScores = []
    Q = []
    for R in voteTrees:
        scoreStr = tripRootScore(myTree,R)
        S = {}
        for item in scoreStr.split(','):
            ID,s = item.split(':')
            S[ID] = float(s)
        tripScores.append(S)
        
        Q.append(quartet_distance(myTree,R))

# bootstrapping
    k = len(voteTrees)
    root_votes = {alpha:[] for alpha in alphas}
    for b in range(nbootstraps):
        b_indices = random.choices(list(range(k)),k=k)
        for alpha in alphas:
            curr_score = {}
            for i in b_indices:
                w = exp(-alpha*Q[i])
                for x in tripScores[i]:
                    s = w*tripScores[i][x]
                    curr_score[x] = s if s not in curr_score else curr_score[x]+s
            max_score = 0
            best_root = None        
            for x in curr_score:    
                if curr_score[x] > max_score:
                    max_score = curr_score[x]
                    best_root = x
            if best_root in root_ids:
                best_root = myTree_obj.root.label        
            root_votes[alpha].append(best_root)
            
# measure consistency of votings, using entropy
    E_scores = {}
    for alpha in root_votes:
        vote_count = {}
        for x in root_votes[alpha]:
            vote_count[x] = 1 if x not in vote_count else vote_count[x]+1
        E_scores[alpha] = entropy(vote_count.values())
    return E_scores, root_votes

def main():
    from sys import argv

    inFile = argv[1]
     
    with open(inFile,'r') as fin:
        trees = fin.read().split("\n")[:-1]
    
    alphas = [0,1,2,3,4,5,6,7,8,9,10]
    nbootstraps = 1000

    for t in trees:
        E_scores,_ = alpha_selection(t,trees,alphas,nbootstraps)     
        min_e = float("inf")
        best_alpha = None

        for alpha in E_scores:
            if E_scores[alpha] < min_e:
                min_e = E_scores[alpha]
                best_alpha = alpha
        print("Best alpha: " + str(best_alpha) + ". Entropy = " + str(min_e))

if __name__ == "__main__":
    main()        
    
            
