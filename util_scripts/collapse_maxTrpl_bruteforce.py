#! /usr/bin/env python
from treeswift import *
from sys import argv
from compute_expected_triplets import expected_triplets, sort_edges_by_length, compute_reduced_triplets

def reset(myTree):
    for node in myTree.traverse_preorder():
        node.marked = False

def mark(nodeList):
    for node in nodeList:
        node.marked = True

def num_2_listIdx(num,L):
    m = 1
    idxList = []
    for i in range(L):
        if num & m > 0:
            idxList.append(i)         
        m <<= 1
    return idxList
        
def find_best_marking(myTree,popSize):                
    allNodes = sort_edges_by_length(myTree)
    L = len(allNodes)
    print("The tree has " + str(L) + " edges")
    S = 2**L-1

    nleaves = myTree.num_nodes(leaves=True,internal=False)
    org_total = nleaves*(nleaves-1)*(nleaves-2)/6
    org_exp = expected_triplets(myTree,popSize)
    org_score = float(org_exp)/org_total
    opt_score = org_score
    optList = []

    print("Original score: " + str(org_score))

    num = 1
    while num < S:
        print(num)
        print("Computing idex list ...")
        idxList = num_2_listIdx(num,L)
        print("The new list has " + str(len(idxList)) + " items")
        markingNodes = [ allNodes[i] for i in idxList ]
        reset(myTree)
        mark(markingNodes)
        total_reduce, expected_reduce = compute_reduced_triplets(myTree,N)
        new_score = float(org_exp-expected_reduce)/(org_total-total_reduce)
        print("New score: " + str(new_score))
        if new_score > opt_score:
            optList = markingNodes
            opt_score = new_score
        num += 1    
    
    for node in optList:
        node.contract()

    return opt_score
    

inTreeFile = argv[1]
N = float(argv[2])
outTreeFile = argv[3]

myTree = read_tree_newick(inTreeFile)

print("Finish reading tree!")

opt_score = find_best_marking(myTree,N)

myTree.write_tree_newick(outTreeFile)
print("Optimal score: " + str(opt_score)) 
