#ifndef ABSTRACT_DISTANCE_CALCULATOR_H
#define ABSTRACT_DISTANCE_CALCULATOR_H

#include "int_stuff.h"
#include "triplet_calc.h"
#include "quartet_calc.h"
#include "hdt.h"
#include "TripletCounter.h"

#ifndef CONTRACT_MAX_EXTRA_SIZE
#define CONTRACT_MAX_EXTRA_SIZE 20000
#endif

class TripletRooting {
 public:
  TripletRooting(RootedTree *ref, RootedTree *tree);
  ~TripletRooting();
  
  void countChildren(RootedTree *t);
  void count(RootedTree *v);
  void updateCounters(unsigned int nodeIdx, unsigned int color);
  void compute_tA(RootedTree *v);
  void find_optimal_root();
  void downroot(RootedTree *v, unsigned int &parent_score);      

  HDTFactory *dummyHDTFactory;
  RootedTree *myRef, *myTree;
  HDT *hdt;
  TripletCounter *tripCount;
  INTTYPE_REST optimalTripScore;
  RootedTree *optimalRoot; // the node in myTree where the optimal root placed above
};

#endif
