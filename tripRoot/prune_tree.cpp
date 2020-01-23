#include "rooted_tree.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "rooted_tree_factory.h"

int main(int argc, char** argv){
    char *inTreeFile = argv[1];
    char *outTreeFile = argv[2];
    vector<string> pruning_set;

    for (int i=3; i<argc; i++){
        pruning_set.push_back(argv[i]);
    }
    
    RootedTreeFactory *tFactory = new RootedTreeFactory();
    RootedTree *myTree = tFactory->getRootedTree();
    myTree->factory = tFactory;
    myTree->read_newick_file(inTreeFile);    
    myTree->countChildren();
    myTree->count_nodes();
    myTree->set_all_idx(0);

    for (vector<string>::iterator it = pruning_set.begin(); it != pruning_set.end(); it++){
        string leafName = *it;
        RootedTree* leaf = myTree->search_name(leafName);
        myTree->prune_subtree(leaf);
    }

    ofstream fout;
    fout.open(outTreeFile);
    myTree->write_newick(fout);
    fout.close(); 
}
