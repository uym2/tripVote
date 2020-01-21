#include "subtree_sampler.h"
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <vector>

SubtreeSampler::SubtreeSampler(RootedTree *tree){
    this->myTree = tree;
    this->myTree->countChildren();
    this->N = this->myTree->n;
}

// random generator function:
int myrandom (int i) { return std::rand()%i;}

RootedTree* SubtreeSampler::sample_subtree(unsigned int k){
    string treeStr = this->myTree->toString();
    RootedTreeFactory *factory = new RootedTreeFactory();
    RootedTree* subtree = factory->getRootedTree();
    subtree->factory = factory;
    subtree->read_newick_str(treeStr);
    vector<RootedTree*> L = *subtree->getList(); // leafset
    
   // create the list R = [ 0 1 2 ... n ] then shuffle it
    std::vector<int> R;
    for (int i=0; i<this->N; i++){
        R.push_back(i);  
    }
    std::srand(std::time(0));
    //std::random_shuffle(R.begin(), R.end());
    std::random_shuffle(R.begin(), R.end(), myrandom);
    
    // retain only the last k leaves and remove the rest according to the shuffled list L
    for (int i=0; i < this->N-k; i++){
        RootedTree* leaf = L[R[i]];
        subtree->prune_subtree(leaf);
    }
    return subtree;
}
