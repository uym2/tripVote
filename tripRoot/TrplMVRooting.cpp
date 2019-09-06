#include "TrplMVRooting.h"
#include "hdt.h"
#include "hdt_factory.h"

TrplMVRooting::TrplMVRooting(){
    this->myRef = this->myTree = NULL;
    this->hdt = NULL;
    this->tripCount = NULL;
    this->dummyHDTFactory = NULL;
    this->ambiguity = 0;
    this->mvCount = NULL; 
}

bool TrplMVRooting::initialize(RootedTree *ref, RootedTree *tree){
    this->myRef = ref;
    this->myTree = tree;
    this->hdt = NULL;
    unsigned int N = this->myTree->set_all_idx(0);
    tripCount = new TripletCounter(N);
    mvCount = new MinVarCounter(N);
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    return true;
}

TrplMVRooting::~TrplMVRooting(){
    delete mvCount;
}

float TrplMVRooting::compute_root_var(){
    float nleaves = 0;
    float acc_sum = 0, acc_sumSquare = 0;

    this->__compute_root_var__(this->myTree, nleaves, 0, acc_sum, acc_sumSquare);
    return acc_sumSquare/nleaves - (acc_sum*acc_sum)/(nleaves*nleaves);
}

void TrplMVRooting::__compute_root_var__(RootedTree *t, float & nleaves, float d2root, float & acc_sum, float & acc_sumSquare){
    if (t->isLeaf()){
        nleaves += 1;
        acc_sum += d2root;
        acc_sumSquare += d2root*d2root;
    } else {
        for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
            this->__compute_root_var__(i->data, nleaves, d2root + i->data->edge_length, acc_sum, acc_sumSquare);
        }
    }
}

void TrplMVRooting::__compute_SI__(RootedTree *t){
    float mySI = 0;
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        this->__compute_SI__(i->data);
        mySI += this->mvCount->SI[i->data->idx] + i->data->edge_length * i->data->n;    
    }
    this->mvCount->SI[t->idx] = mySI;     
}

void TrplMVRooting::compute_varScore(){
    this->mvCount->var[this->myTree->idx] = this->compute_root_var();
    this->__compute_SI__(this->myTree);
    
}

void TrplMVRooting::__compute_ST__(RootedTree *t){
}
