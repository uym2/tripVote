#ifndef REF_TREE_GEN_H
#define REF_TREE_GEN_H

#include "int_stuff.h"
#include "rooted_tree.h"
#include "rooted_tree_factory.h"
#include <vector>
#include <string>

class RefTreeGen {   
 public:
    RootedTree* collapse_from_depth(RootedTree* aTree, unsigned int maxDepth); // branches with depth > maxDepth will be collapsed
    vector<string> list_leaves(RootedTree* aTree);
 private:
    void __collapse_tree__(RootedTree* aTree);
    void __list_leaves__(RootedTree* aTree, vector<string> & myLeaves);
    void __list_nodes_with_depth__(RootedTree* aTree, unsigned int depth, vector<RootedTree*> & myNodes, unsigned int curr_depth=0);   
};
#endif
