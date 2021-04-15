from treeswift import *
from tqdist import *
from triproot import *
from math import exp,log,ceil,sqrt
import random
from tripVote.utils import sample_by_depth, prune_long, reroot_at_edge

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

def tripVote(myTree,refTrees,W=None,do_indexing=True,search_space=None):
    myTree_obj = read_tree_newick(myTree)
    if W is None:
        W = [1.0]*len(refTrees)
    
    # indexing
    id2lb = {}
    if do_indexing:
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
    #count=1
    for rtree,w in zip(refTrees,W):
        mystr = tripRootScore(rtree,treestr)
        if not mystr: # empty string returned by tripRootScore; usually because the refTree is too small after pruning to pair with the query tree
            continue
        for item in mystr.split(','):
            ID,s = item.split(':')
            score = float(s)
            if ID not in id2score:
                id2score[ID] = w*score
            else:
                id2score[ID] += w*score  
    best_id = None
    max_score = -1    
    for ID in id2score:
        if id2score[ID] > max_score:
           lb = id2lb[ID] if ID in id2lb else ID
           if (search_space is None) or (lb in search_space):
               best_id = ID
               max_score = id2score[ID]

    #for ID in id2score:
    #    if abs(id2score[ID]-max_score) < 1e-4 and ID != best_id:
    #        break

    # turn back the original label and find the rooting position
    optimal_root = None
    for node in myTree_obj.traverse_preorder():
        if node.label == best_id and not node.is_root():
            optimal_root = node
        if not node.is_leaf() and do_indexing:
            node.label = id2lb[node.label]
    return best_id,id2lb[best_id] if best_id in id2lb else best_id, max_score

def tripVote_root(myTree,refTrees,max_depth='max',sample_size='full',nsample=None): #,alpha=0):
    new_refTrees = []
    #W = []
    for rstr in refTrees:
        #qdist = quartet_distance(rstr,myTree)
        #w = exp(-alpha*qdist)
        tree_obj = read_tree_newick(rstr)
        n = len(list(tree_obj.traverse_leaves()))
        if max_depth == 'max':
            d = n
        elif max_depth == 'log2':
            d = ceil(log2(n))
        else:
            d = int(max_depth)    
        # prune deep nodes
        if d < n:
            prune_long(tree_obj,d)    
        # sampling
        if (len(list(tree_obj.traverse_leaves())) >= 3):
            if nsample is None:
                new_tree = tree_obj.newick()
                new_refTrees.append(new_tree)
                #W.append(w)
            else:
                nleaf = len(list(tree_obj.traverse_leaves()))
                S = sample_by_depth(tree_obj,nleaf if sample_size == 'full' else ceil(sqrt(nleaf)),nsample)
                new_refTrees += S
                #W += [w]*len(S)
    # read and label myTree
    tree_obj = read_tree_newick(myTree)
    idx = 0
    for node in tree_obj.traverse_preorder():
        if not node.is_leaf():
            ID = 'I' + str(idx)
            node.label = ID 
            idx += 1
    _,root_label,score = tripVote(tree_obj.newick(),new_refTrees,do_indexing=False) #,W=W)
    print("TripVote score: " + str(score))
    #tree_obj = read_tree_newick(myTree)
    root_node = None
    for node in tree_obj.traverse_preorder():
        if node.label == root_label:
            root_node = node
            break
    node = root_node
    d2root = 0
    while not node.is_root() and not node.parent.is_root():
        d2root += 1
        node = node.get_parent()

    if not root_node.is_root:
        reroot_at_edge(tree_obj,root_node,root_node.edge_length/2)
    
    rerooted_tree = tree_obj.newick()

    return rerooted_tree, d2root                    
            
if __name__ == "__main__":
    infile = "test_cases/MV.trees" 
    with open(infile,'r') as fin:
        refTrees = fin.read().split("\n")[:-1]
    alpha_scoring = tripVote_crossValidate(refTrees,fraction_test=0.01,n_vote_groups=10,alphas=list(x for x in range(0,21)))
    for alpha in alpha_scoring:
        print(alpha,sum(alpha_scoring[alpha]))
