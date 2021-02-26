from treeswift import *
from math import log2, ceil, sqrt
from random import choices

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
        head = tail
        tail = p
        p = tail.parent
        q = head.parent

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


def sample_by_depth(tree,nleaf,nsample):
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
