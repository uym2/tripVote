from treeswift import *
from tqdist import *
from triproot import *
from math import exp

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

def tripVote(myTree,refTrees,weights,alpha=10):
    myTree_obj = read_tree_newick(myTree)
    # indexing
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
        if not node.is_leaf():
            node.label = id2lb[node.label]
    
    print("Score at original root: " + str(id2score['I0']))
    print("Optimal score: " + str(max_score))
    
    # reroot
    if optimal_root is not None: 
        reroot_at_edge(myTree_obj,optimal_root,optimal_root.edge_length/2)
    
    return myTree_obj.newick()
