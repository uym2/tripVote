#ifndef TRIPLET_ROOTING_H
#define TRIPLET_ROOTING_H

#include "rooted_tree.h"
#include "TripletDistanceCalculator.h"
#include "templated_stack.h"

class TripletRooting {
 public:
    TripletRooting(RootedTree *ref, RootedTree *tr);
    INTTYPE_REST minTripletDistance();
 private:
    RootedTree *myRef,*myTree;
};

#endif
