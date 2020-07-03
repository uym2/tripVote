#include "OutgroupRooting.h"

RootedTree* OutgroupRooting::__find_one_ingroup__(RootedTree *tree, set<string> &outgroups){
    if (tree->isLeaf()){
        std::set<string>::iterator it = outgroups.find(tree->name);
        if (it != outgroups.end())
            return NULL;
        else
            return tree;    
    }
    else {
        for(TemplatedLinkedList<RootedTree*> *i = tree->children; i != NULL; i = i->next){
            RootedTree* candidate = __find_one_ingroup__(i->data, outgroups);
            if (candidate != NULL)
                return candidate;
        }
        return NULL;
    }
}

RootedTree* OutgroupRooting::__find_biggest_clade__(RootedTree *tree){
    if (tree->isLeaf()){
        std::set<string>::iterator it = this->myOGs.find(tree->name);
        if (it != this->myOGs.end()){
            this->myCounters[tree->idx] = 1;
            return tree;
        }
        else{
            this->myCounters[tree->idx] = 0;
            return NULL;
        }
    }
    else{
        unsigned int curr_counter = 0;
        bool is_clade = true;
        RootedTree *maxClade = NULL;
        for(TemplatedLinkedList<RootedTree*> *i = tree->children; i != NULL; i = i->next){
            RootedTree* mc = this->__find_biggest_clade__(i->data);
            if (maxClade == NULL || (mc != NULL && this->myCounters[mc->idx] > this->myCounters[maxClade->idx]))
                maxClade = mc;
            if (this->myCounters[i->data->idx] == 0)
                is_clade = false;
            if (!is_clade)
                curr_counter = 0;
            else    
                curr_counter += this->myCounters[i->data->idx];
        }
        this->myCounters[tree->idx] = curr_counter;
        if (curr_counter) // this current node is a clade of outgroups
            maxClade = tree;
        return maxClade;    
    }
}

void OutgroupRooting::initialize(RootedTree *myTree, set<string> outgroups){
    RootedTree* ig = this->__find_one_ingroup__(myTree, outgroups);
    RootedTree* rTree = NULL;
    if (ig != NULL){
        rTree = myTree->reroot_at_edge(ig,ig->edge_length/2);
    }
    if (rTree == NULL)
        this->myTree = myTree;
    else        
        this->myTree = rTree;

    this->myOGs = outgroups;
    this->N = this->myTree->set_all_idx(0);
    myCounters = new unsigned int [this->N];
    for (int i = 0; i < N; i++)
        myCounters[i] = 0;
}

RootedTree* OutgroupRooting::findRoot(){
   return this->__find_biggest_clade__(myTree); 
}

RootedTree* OutgroupRooting::reroot(){
   RootedTree* newRoot = this->findRoot();
   if (newRoot != NULL)
       return myTree->reroot_at_edge(newRoot,newRoot->edge_length/2);
   else{
        cerr << "Could not find any clade of outgroups. Possibly because the tree contains none of the specified outgroups. Randomly rerooted at one leaf!" << endl;
        return myTree;
   }
}

OutgroupRooting::OutgroupRooting(){
    this->myTree = NULL;
    this->myCounters = NULL;
    this->N = 0;
}

OutgroupRooting::~OutgroupRooting(){
    delete [] myCounters;
}
