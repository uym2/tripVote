#include "TrplMVRooting.h"
#include "hdt.h"
#include "hdt_factory.h"

TrplMVRooting::TrplMVRooting(){
    this->factory = NULL;
    this->myRef = this->myTree = NULL;
    this->hdt = NULL;
    this->tripCount = NULL;
    this->dummyHDTFactory = NULL;
    this->ambiguity = 0;
    this->mvCount = NULL; 
    this->optimalVarScore = -1;
}

bool TrplMVRooting::initialize(RootedTree *ref, RootedTree *tree){
    TripletRooting::initialize(ref,tree);
    unsigned int N = this->myTree->nodeCounts;
    mvCount = new MinVarCounter(N);
    return true;
}

TrplMVRooting::~TrplMVRooting(){
    //delete this->tripCount;
    //delete this->factory;

    /*while (this->optimaltripRoots != NULL){
        TemplatedLinkedList<RootedTree*> *curr = this->optimaltripRoots;
        this->optimaltripRoots = this->optimaltripRoots->next;
        delete curr;
    }*/
    delete mvCount;
}

double TrplMVRooting::compute_root_var(){
    double nleaves = 0;
    double acc_sum = 0, acc_sumSquare = 0;

    this->__compute_root_var__(this->myTree, nleaves, 0, acc_sum, acc_sumSquare);
    return acc_sumSquare/nleaves - (acc_sum*acc_sum)/(nleaves*nleaves);
}

void TrplMVRooting::__compute_root_var__(RootedTree *t, double & nleaves, double d2root, double & acc_sum, double & acc_sumSquare){
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
    double mySI = 0;
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        this->__compute_SI__(i->data);
        mySI += this->mvCount->SI[i->data->idx] + i->data->edge_length * i->data->n;    
    }
    this->mvCount->SI[t->idx] = mySI;     
}

bool TrplMVRooting::compute_varScore(){
    this->countChildren(myTree);
    this->mvCount->var[this->myTree->idx] = this->compute_root_var();
    this->optimalVarScore = this->mvCount->var[this->myTree->idx]; 
    this->__compute_SI__(this->myTree);
    this->mvCount->ST[this->myTree->idx] = this->mvCount->SI[this->myTree->idx]; 
    this->__compute_ST__(this->myTree);
    this->__compute_var__(this->myTree);

    return true;
}

void TrplMVRooting::__compute_ST__(RootedTree *t){
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        this->mvCount->ST[i->data->idx] = this->mvCount->ST[t->idx] + (this->myTree->n - 2*i->data->n)*i->data->edge_length;
        this->__compute_ST__(i->data);
    } 
}

void TrplMVRooting::__compute_var__(RootedTree *t){
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        double alpha, beta, a, b, c, x_star;

        alpha = (2*this->mvCount->ST[t->idx] - 4*(this->mvCount->SI[i->data->idx] + i->data->n*i->data->edge_length))/this->myTree->n;
        beta = 1 - 2*(double)i->data->n/this->myTree->n;
        
        a =  1-beta*beta;
        b = alpha - 2*mvCount->ST[t->idx]*beta/myTree->n;

        this->mvCount->var[i->data->idx] = a*i->data->edge_length*i->data->edge_length + b*i->data->edge_length + this->mvCount->var[t->idx];                                    
        x_star = -b/(2*a);
        
        if (x_star >= 0 && x_star <= i->data->edge_length){
            mvCount->xStar[i->data->idx] = x_star;
            this->mvCount->minVar[i->data->idx] = a*x_star*x_star + b*x_star + this->mvCount->var[t->idx];
        } else if (mvCount->var[i->data->idx] < mvCount->var[t->idx]) {
            mvCount->xStar[i->data->idx] = i->data->edge_length;
            this->mvCount->minVar[i->data->idx] = this->mvCount->var[i->data->idx];  
        } else {
            mvCount->xStar[i->data->idx] = 0;
            this->mvCount->minVar[i->data->idx] = this->mvCount->var[t->idx];  
        }
        if (this->mvCount->minVar[i->data->idx] < this->optimalVarScore)
            this->optimalVarScore = this->mvCount->minVar[i->data->idx]; 
        this->__compute_var__(i->data);
    }
}

bool TrplMVRooting::find_optimal_root(){
    countChildren(myTree);

    if (this->compute_varScore() && this->compute_tripScore()){
        //this->optimalRoot = this->optimaltripRoots->data;
        this->optimalRoot = this->optimaltripRoots[0];
        double mvScore = this->mvCount->minVar[this->optimalRoot->idx];
        
        /*for (TemplatedLinkedList<RootedTree*> *i = this->optimaltripRoots->next; i != NULL; i = i->next){
            if (mvCount->minVar[i->data->idx] < mvScore){
                this->optimalRoot = i->data;
                mvScore = mvCount->minVar[i->data->idx];
            }
        } */   
        for (int i=1; i< this->optimaltripRoots.size(); i++){
            RootedTree* curr = this->optimaltripRoots[i];
            if (mvCount->minVar[curr->idx] < mvScore){
                this->optimalRoot = curr;
                mvScore = mvCount->minVar[curr->idx];
            }
        }    
        return true;
    } else {
        return false;
    }
}
