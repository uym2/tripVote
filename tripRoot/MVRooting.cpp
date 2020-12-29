#include "MVRooting.h"
#include "rooted_tree.h"


MVRooting::MVRooting(){
    this->myTree = NULL;
    this->mvCount = NULL; 
    this->optimalVarScore = -1;
    this->optimalRoot = NULL;
}

bool MVRooting::initialize(RootedTree *tree){
    this->myTree = tree;
    unsigned int N = this->myTree->nodeCounts;
    mvCount = new MinVarCounter(N);
    return true;
}

MVRooting::~MVRooting(){
    delete mvCount;
}

double MVRooting::compute_root_var(){
    double nleaves = 0;
    double acc_sum = 0, acc_sumSquare = 0;

    this->__compute_root_var__(this->myTree, nleaves, 0, acc_sum, acc_sumSquare);
    return acc_sumSquare/nleaves - (acc_sum*acc_sum)/(nleaves*nleaves);
}

void MVRooting::__compute_root_var__(RootedTree *t, double & nleaves, double d2root, double & acc_sum, double & acc_sumSquare){
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

void MVRooting::__compute_SI__(RootedTree *t){
    double mySI = 0;
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        this->__compute_SI__(i->data);
        mySI += this->mvCount->SI[i->data->idx] + i->data->edge_length * i->data->n;    
    }
    this->mvCount->SI[t->idx] = mySI;     
}

void MVRooting::countChildren(RootedTree *t) {
  if (t->isLeaf()) {
    t->n = 1;
    return;
  }
  
  int nSum = 0;
  for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next) {
    RootedTree *childI = i->data;
    countChildren(childI);
    nSum += childI->n;
  }
  t->n = nSum;
}

void MVRooting::compute_score(){
    this->mvCount->minVar[this->myTree->idx] = this->mvCount->var[this->myTree->idx] = this->compute_root_var();
    this->optimalVarScore = this->mvCount->var[this->myTree->idx];
    this->optimalRoot = this->myTree; 
    this->__compute_SI__(this->myTree);
    this->mvCount->ST[this->myTree->idx] = this->mvCount->SI[this->myTree->idx]; 
    this->__compute_ST__(this->myTree);
    this->__compute_var__(this->myTree);
}

RootedTree* MVRooting::root_tree(){
    this->mvCount->minVar[this->myTree->idx] = this->mvCount->var[this->myTree->idx] = this->compute_root_var();
    this->optimalVarScore = this->mvCount->var[this->myTree->idx];
    this->optimalRoot = this->myTree; 
    this->__compute_SI__(this->myTree);
    this->mvCount->ST[this->myTree->idx] = this->mvCount->SI[this->myTree->idx]; 
    this->__compute_ST__(this->myTree);
    this->__compute_var__(this->myTree);
    RootedTree* rerooted = myTree->reroot_at_edge(this->optimalRoot,mvCount->xStar[this->optimalRoot->idx]);

    return rerooted;
}

void MVRooting::__compute_ST__(RootedTree *t){
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        this->mvCount->ST[i->data->idx] = this->mvCount->ST[t->idx] + (this->myTree->n - 2*i->data->n)*i->data->edge_length;
        this->__compute_ST__(i->data);
    } 
}

void MVRooting::__compute_var__(RootedTree *t){
    for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
        double alpha, beta, a, b, x_star;

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
        if (this->mvCount->minVar[i->data->idx] < this->optimalVarScore){
            this->optimalVarScore = this->mvCount->minVar[i->data->idx]; 
            this->optimalRoot = i->data;
        }
        this->__compute_var__(i->data);
    }
}

