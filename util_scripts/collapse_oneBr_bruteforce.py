#! /usr/bin/env python
from treeswift import *
from sys import argv
from compute_expected_triplets import expected_triplets, sort_edges_by_length, compute_reduced_triplets

def reset(myTree):
    for node in myTree.traverse_preorder():
        if node.is_root():
            node.marked = False
        else:    
            node.marked = True

def mark_one(myTree,popSize):                
    allNodes = sort_edges_by_length(myTree)
    L = len(allNodes)
    print("The tree has " + str(L) + " edges")

    nleaves = myTree.num_nodes(leaves=True,internal=False)

    org_total = nleaves*(nleaves-1)*(nleaves-2)/6
    org_exp = expected_triplets(myTree,popSize)
    org_score = float(org_exp)/org_total
    opt_score = None
    optMark = None

    print("Original score: " + str(org_score))

    count = 1

    for node in allNodes:
        reset(myTree)
        node.marked = False
        total_reduce, expected_reduce = compute_reduced_triplets(myTree,N)
        new_score = float(org_exp-expected_reduce)/(org_total-total_reduce)
        print("Count: " + str(count) + ". Edge length: " + str(node.get_edge_length()) + ". Score: " + str(new_score))
        if opt_score is None or new_score > opt_score:
            opt_score = new_score
            optMark = node
        count += 1    
    
    reset(myTree)
    optMark.marked = False
    opt_edge = optMark.get_edge_length()

    for node in allNodes:
        if node.marked:
            node.contract()

    return opt_score, opt_edge
    

inTreeFile = argv[1]
N = float(argv[2])
outTreeFile = argv[3]

myTree = read_tree_newick(inTreeFile)

print("Finish reading tree!")

opt_score, opt_edge = mark_one(myTree,N)

myTree.write_tree_newick(outTreeFile)
print("Optimal score: " + str(opt_score) + ". Length of the remained edge: " + str(opt_edge)) 
