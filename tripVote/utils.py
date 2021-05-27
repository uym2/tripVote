from treeswift import *
from math import log2, ceil, sqrt
from random import choices
from tqdist import *
from sys import stdout

def restrict_to_same_leafset(nwk_str1,nwk_str2):
    tree1 = read_tree_newick(nwk_str1)
    tree2 = read_tree_newick(nwk_str2)

    L1 = set(node.label for node in tree1.traverse_leaves())
    L2 = set(node.label for node in tree2.traverse_leaves())
    L = L1.intersection(L2)

    tree1_L = tree1.extract_tree_with(L)
    tree2_L = tree2.extract_tree_with(L)

    return tree1_L.newick()[5:], tree2_L.newick()[5:]

metrics = {'quartet':quartet_distance,'triplet':triplet_distance,'quartet_raw':quartet_distance_raw,'triplet_raw':triplet_distance_raw}

def distance_matrix(nwk_strs,metric,IDs=None,fout=None):
# metric can either be "quartet" for quartet_distance or "triplet" for triplet_distance 
# if IDs is None, tree IDs will be assigned based on their order
# otherwise, IDs must have the same length as nwk_strs
    distance_func = metrics[metric]
    N = len(nwk_strs)
    D = {}

    if IDs is None:
        IDs = [str(i+1) for i in range(N)]

    for i in range(N-1):
        for j in range(i+1,N):
            t1,t2 = restrict_to_same_leafset(nwk_strs[i],nwk_strs[j])
            d = distance_func(t1,t2)
            if fout is not None:
                fout.write(IDs[i] + " " + IDs[j] + " " + str(d) + "\n")
            else:    
                D[(IDs[i],IDs[j])] = d
    return D       

def distance_to_refs(refs,trees,metric,fout=stdout):
    distance_func = metrics[metric]
    for i,r in enumerate(refs):
        for j,t in enumerate(trees):
            r_p,t_p = restrict_to_same_leafset(r,t)
            d = distance_func(r_p,t_p)
            fout.write(str(i+1) + " " + str(j+1) + " " + str(d) + "\n")

def reroot_at_edge(tree, node, length, root_label=None):
# change edge to opt_root
    length1 = node.edge_length-length if node.edge_length is not None else None
    length2 = length if length is not None else None
    if not node:
        return

    head = node #opt_root = v = node
    tail = node.parent #u parent of opt_root
    if not tail:
        return

    #if (length2 == 0) and head.is_leaf():
    #    return 0, 0

    new_root = Node()
    new_root.label = root_label

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
        head = tail
        tail = p
        p = tail.parent
        q = head.parent

        br2currRoot += 1
        d2currRoot = d2currRoot + l if d2currRoot is not None else None
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
        sis.edge_length = l + tail.edge_length if tail.edge_length is not None else None
        head.remove_child(tail)

    tree.root = new_root

    return d2currRoot,br2currRoot


def sample_by_depth(tree,nleaf,nsample,do_extract=True):
# return a list of newick trees if do_extract is True
# otherwise, return a nested list of labels
    leaf_labels = []
    leaf_weights = []
    for node in tree.traverse_preorder():
        log_p_inv = log2(len(node.children)) if not node.is_leaf() else 0
        if node.is_root():
            node.log_p_inv = log_p_inv
        else:
            node.log_p_inv = node.parent.log_p_inv + log_p_inv
        if node.is_leaf():
            leaf_labels.append(node.label)
            leaf_weights.append(2**(-node.log_p_inv))
    smpl_trees = []
    for s in range(nsample):
        sample = set(choices(leaf_labels,weights=leaf_weights,k=nleaf))
        if do_extract:
            if len(sample) >= 3:
                smpl_trees.append(tree.extract_tree_with(sample, suppress_unifurcations=True).newick())
        else:
            smpl_trees += list(sample)
        
    return smpl_trees                    

def sample_by_brlen(tree,nleaf,nsample,pseudo=1e-3):
    leaf_labels = []
    leaf_weights = []
    for node in tree.traverse_preorder():
        if node.is_root():
            node.p = 1.0
        n = len(node.children)-1    
        s = n*sum(c.edge_length + pseudo for c in node.children)
        if node.is_leaf():
            leaf_labels.append(node.label)
            leaf_weights.append(node.p)
        else:
            for c in node.children:
                c.p = c.parent.p * (s-n*(c.edge_length+pseudo))/n/s

    smpl_trees = []
    for s in range(nsample):
        sample = set(choices(leaf_labels,weights=leaf_weights,k=nleaf))
        if len(sample) >= 3:
            smpl_trees.append(tree.extract_tree_with(sample, suppress_unifurcations=True).newick())
        
    return smpl_trees                    

def prune_long(tree,max_depth):
    cutting_list = []
    for node in tree.traverse_preorder():
        if node.is_root():
            node.depth = 0
        else:
            node.depth = node.get_parent().depth + 1
        if node.depth == max_depth+1:
            cutting_list.append(node)
    while cutting_list:
        node = cutting_list.pop()
        p = node.get_parent()
        if p is not None:
            p.remove_child(node)
            if p.is_leaf():
                cutting_list.append(p)
    tree.suppress_unifurcations()
