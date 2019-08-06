#include "TripletRooting.h"
#include "templated_stack.h"

TripletRooting::TripletRooting(RootedTree *ref, RootedTree *tr){
    this->myRef = ref;
    this->myTree = tr;
}

INTTYPE_REST TripletRooting::minTripletDistance(){
    unsigned int N = this->myTree->set_all_idx(0);
    bool *idx_marking = new bool [N];
    for (unsigned int i = 0; i<N; i++){
        idx_marking[i] = false;
    }

    // mark the root
    idx_marking[0] = true;

    // mark the children of the root
    for (TemplatedLinkedList<RootedTree*> *i = this->myTree->children; i != NULL; i = i->next){
        idx_marking[i->data->idx] = true;
    }

    TemplatedStack<RootedTree*> *stk = new TemplatedStack<RootedTree*>;
    stk->push(this->myTree);

    INTTYPE_REST min_trpl_distance = -1;

    while (!stk->isEmpty()){
        RootedTree* currTree = stk->pop();

        // compute triplet distance of currTree and refTree
        TripletDistanceCalculator tripletCalc;
        INTTYPE_REST trpl_distance = tripletCalc.calculateTripletDistance(this->myRef, currTree);
        if (min_trpl_distance < 0 || trpl_distance < min_trpl_distance)
            min_trpl_distance = trpl_distance;

        // create a downrooted version for each grandchild of curr_tree
        for (TemplatedLinkedList<RootedTree*> *i = currTree->children; i != NULL; i = i->next){
            for (TemplatedLinkedList<RootedTree*> *j = i->data->children; j != NULL; j = j->next){
                if (!idx_marking[j->data->idx]){
                    idx_marking[j->data->idx] = true; 
                    RootedTree* newTree = currTree->down_root(j->data);
                    stk->push(newTree);
                }
            }
        }
    }

    delete [] idx_marking;
    delete stk;

    return min_trpl_distance;
}
