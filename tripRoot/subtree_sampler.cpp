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

struct item {
    unsigned int idx;
    unsigned int value;
};

bool itemCmp(item x, item y){
    return x.value < y.value;
}

vector<unsigned int> shuffle(unsigned int N){
    //std::srand(std::time(0));
    vector<item> S;
    for (int i=0; i<N; i++){
        item x;
        x.idx = i;
        x.value = rand();
        S.push_back(x);
    }
    sort(S.begin(),S.end(),itemCmp);
    
    vector<unsigned int> shuffled_idx;

    for (vector<item>::iterator it = S.begin(); it != S.end(); ++it){
        shuffled_idx.push_back(it->idx);
    }

    return shuffled_idx;
}

RootedTree* SubtreeSampler::sample_subtree(unsigned int k){
    string treeStr = this->myTree->toString();
    RootedTreeFactory *factory = new RootedTreeFactory();
    RootedTree* subtree = factory->getRootedTree();
    subtree->factory = factory;
    subtree->read_newick_str(treeStr);
    vector<RootedTree*> L = *subtree->getList(); // leafset
    
   // create the list R = [ 0 1 2 ... n ] then shuffle it

    /*std::vector<int> R;
    for (int i=0; i<this->N; i++){
        R.push_back(i);  
    }*/
    //std::srand(std::time(0));
    //std::random_shuffle(R.begin(), R.end());
    //std::random_shuffle(R.begin(), R.end(), myrandom);
    
    /*unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto gen = std::default_random_engine(seed);
    std::shuffle (R.begin(), R.end(), gen); */
    
    vector<unsigned int> R = shuffle(this->N);

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

void topdown_traverse(RootedTree* t, vector<RootedTree*> &nodeList){
    if (!t->isRoot())
        nodeList.push_back(t);
                
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        RootedTree *c = i->data;
        topdown_traverse(c,nodeList);
    }
}

vector<string> SubtreeSampler::sample_by_splits(){
    // list all the splits (represented by nodes)
    vector<RootedTree*> nodeList;
    topdown_traverse(this->myTree,nodeList);

    vector<string> my_subtrees;
    for (vector<RootedTree*>::iterator it = nodeList.begin() ; it != nodeList.end(); ++it){
        RootedTree* v = *it;
        RootedTree* u = v->parent;
        
        // get clade v
        string treeStr1 = v->toString();
        /*
        RootedTreeFactory *factory = new RootedTreeFactory();
        RootedTree* subtree1 = factory->getRootedTree();
        subtree1->factory = factory;
        subtree1->read_newick_str(treeStr); */
    
        // get subtree after removing clade v
        double el = v->edge_length;
        u->remove_child(v);
        string treeStr2 = this->myTree->toString();
        
        /*factory = new RootedTreeFactory();
        RootedTree* subtree2 = factory->getRootedTree();
        subtree2->factory = factory;
        subtree2->read_newick_str(treeStr);*/

        my_subtrees.push_back(treeStr1);
        my_subtrees.push_back(treeStr2);

        u->addChild(v);
        v->edge_length = el;
    }

    return my_subtrees;
}
