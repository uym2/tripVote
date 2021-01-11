from treeswift import *
from tqdist import *
from triproot import *
from math import exp,log
import random

def reroot_at_edge(tree, node, length):
# change edge to opt_root
    length1 = node.edge_length-length
    length2 = length
    if not node:
        return

    head = node #opt_root = v = node
    tail = node.parent #u parent of opt_root
    if not tail:
        return

    if (length2 == 0) and head.is_leaf():
        return 0, 0

    new_root = Node()

    tail.remove_child(head)

    new_root.add_child(head)
    head.edge_length=length2

    p = tail.parent
    l = tail.edge_length

    new_root.add_child(tail)
    tail.edge_length = length1

    br2currRoot = 0
    d2currRoot = length1

    if (tail is tree.root):
        head = new_root


    while tail is not tree.root:
        q = head.parent
        head = tail
        tail = p
        p = tail.parent

        br2currRoot += 1
        d2currRoot += l
        l1 = tail.edge_length
        tail.remove_child(head)
        head.parent = q
        head.add_child(tail)
        tail.edge_length=l
        l = l1

    # out of while loop: tail IS now tree.root
    if tail.num_children() == 1:
        # merge the 2 branches of the old root and adjust the branch length
        sis = tail.child_nodes()[0]
        l = sis.edge_length
        tail.remove_child(sis)    
        head.add_child(sis)
        sis.edge_length = l + tail.edge_length
        head.remove_child(tail)

    tree.root = new_root

    return d2currRoot,br2currRoot

def entropy(C):
# compute entropy 
    # first normalize
    s = sum(C)
    C_norm = [x/s for x in C]
    return sum(-x*log(x) for x in C_norm)

def avg_all_pairs_dtrpl(trees):
    # compute the average triplet distances of all pairs
    # trees are a list of newick strings
    count = 0
    Sum = 0
    n = len(trees)
    for i in range(n):
        for j in range(i):
            count += 1
            Sum += triplet_distance(trees[i],trees[j])            
    return Sum/count

def tripVote_crossValidate(refTrees,fraction_test=0.1,n_vote_groups=10,alphas=list(range(1,21))):
    # inputs: the refTrees are all newick strings
    
    # indexing 
    refTrees_idx = []
    for treestr in refTrees:
        tree_obj = read_tree_newick(treestr)
        idx = 0
        for node in tree_obj.traverse_preorder():
            if not node.is_leaf():
                ID = 'I' + str(idx)
                node.label = ID 
                idx += 1
        refTrees_idx.append(tree_obj.newick())
    
    # shuffle
    temp = refTrees_idx[1:] 
    random.shuffle(temp)
    refTrees_idx = [refTrees_idx[0]] + temp
    #random.shuffle(refTrees_idx)
           
    # split out into 1 testing group and multiple voting groups
    ntrees = len(refTrees_idx)
    test_size = round(fraction_test * ntrees)
    nv = (ntrees - test_size) // n_vote_groups
    rv = (ntrees - test_size) % n_vote_groups
    
    test_group = refTrees_idx[ntrees-test_size:]
    vote_groups = []
    j = 0
    for i in range(rv):
        vote_groups.append(refTrees_idx[j:j+nv+1])
        j += nv+1
    for i in range(n_vote_groups-rv):
        vote_groups.append(refTrees_idx[j:j+nv])
        j += nv

    # compute weight matrices
    Ws = []
    for vTrees in vote_groups:
        W = [ [0]*len(vTrees) for i in range(test_size) ]
        for i,tT in enumerate(test_group):    
            for j,vT in enumerate(vTrees):
                W[i][j] = quartet_distance(tT,vT)
        Ws.append(W)                  
            
    # voting
    alpha_scoring = {alpha:[]*test_size for alpha in alphas}
    for alpha in alphas:
        for i,myTree in enumerate(test_group):
            #votes = {}
            rerooted_trees = []
            myTree_obj = read_tree_newick(myTree)
            root_ids = [x.label for x in myTree_obj.root.children + [myTree_obj.root]]
            for j,vTrees in enumerate(vote_groups):
                weights = Ws[j][i]  
                rT,best_id,_ = tripVote(myTree,vTrees,weights,alpha=alpha,do_indexing=False)
                rerooted_trees.append(rT)
                #if best_id in root_ids:
                #    best_id = 'I0'                        
                #votes[best_id] = 1 if best_id not in votes else votes[best_id]+1
            #e = entropy(votes.values())
            #e = max(votes.values())
            #alpha_scoring[alpha].append(e)
            #alpha_scoring[alpha].append(votes)
            S = avg_all_pairs_dtrpl(rerooted_trees)
            alpha_scoring[alpha].append(S)
                
    return alpha_scoring            

def tripVote(myTree,refTrees,weights,alpha=10,do_indexing=True):
    myTree_obj = read_tree_newick(myTree)

    # indexing
    if do_indexing:
        id2lb = {}
        idx = 0
        for node in myTree_obj.traverse_preorder():
            if not node.is_leaf():
                ID = 'I' + str(idx)
                id2lb[ID] = node.label
                node.label = ID 
                idx += 1
    treestr = myTree_obj.newick()

    # calling tripRootScore    
    id2score = {}
    for w,rtree in zip(weights,refTrees):
        mystr = tripRootScore(rtree,treestr)
        myweight = exp(-alpha*w)
        for item in mystr.split(','):
            ID,s = item.split(':')
            score = myweight*float(s)
            if ID not in id2score:
                id2score[ID] = score
            else:
                id2score[ID] += score    
    
    best_id = None
    max_score = -1    
    for ID in id2score:
        if id2score[ID] > max_score:
           best_id = ID
           max_score = id2score[ID]

    for ID in id2score:
        if abs(id2score[ID]-max_score) < 1e-4 and ID != best_id:
            break

    # turn back the original label and find the rooting position
    optimal_root = None
    for node in myTree_obj.traverse_preorder():
        if node.label == best_id and not node.is_root():
            optimal_root = node
        if not node.is_leaf() and do_indexing:
            node.label = id2lb[node.label]
    
    #print("Score at original root: " + str(id2score['I0']))
    #print("Optimal score: " + str(max_score))
    
    # reroot
    if optimal_root is not None: 
        reroot_at_edge(myTree_obj,optimal_root,optimal_root.edge_length/2)
    
    return myTree_obj.newick(),best_id,id2lb[best_id] if do_indexing and best_id in id2lb else best_id

if __name__ == "__main__":
    infile = "test_cases/MV.trees" 
    with open(infile,'r') as fin:
        refTrees = fin.read().split("\n")[:-1]
    alpha_scoring = tripVote_crossValidate(refTrees,fraction_test=0.01,n_vote_groups=10,alphas=list(x for x in range(0,21)))
    for alpha in alpha_scoring:
        print(alpha,sum(alpha_scoring[alpha]))
