#include "subtree_sampler.h"
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <vector>

SubtreeSampler::SubtreeSampler(RootedTree *tree){
    string treeStr = tree->toString();
    RootedTreeFactory *factory = new RootedTreeFactory();
    this->myTree = factory->getRootedTree();
    this->myTree->factory = factory;
    this->myTree->read_newick_str(treeStr);
    this->myTree->countChildren();
    this->myTree->compute_d2root();
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

vector<RootedTree*> SubtreeSampler::sample_by_d2root(unsigned int k, double size_proportion){    
    unsigned int n = this->myTree->n;
    unsigned int step = n/k;
    unsigned int m = n*size_proportion;
    
    unsigned int start = 0;
    unsigned int end = m-1;

    vector<RootedTree*> my_subtrees;
    
    for (int i=0; i<k; i++){
        // create a new tree identical to myTree then prune it
        string treeStr = this->myTree->toString();
        RootedTreeFactory *factory = new RootedTreeFactory();
        RootedTree* subtree = factory->getRootedTree();
        subtree->factory = factory;
        subtree->read_newick_str(treeStr);
    
        // get the sorted leafset
        vector<RootedTree*> L = subtree->sort_leaf_by_d2root();

        // prune the left of start
        for (int j=0; j<start; j++)
            subtree->prune_subtree(L[j]);
        
        // prune the right of end
        for (int j=end+1; j<n; j++){
            subtree->prune_subtree(L[j]);
        }
        
        // update start and end
        start += step;
        end += step;
        my_subtrees.push_back(subtree);    
    }

    return my_subtrees;
}
