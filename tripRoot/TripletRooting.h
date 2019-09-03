#ifndef TRIPLET_ROOTING_H
#define TRIPLET_ROOTING_H

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
  TripletRooting();
  bool initialize(RootedTree *ref, RootedTree *tree);
  ~TripletRooting();
  
  virtual bool pairing();
  void countChildren(RootedTree *t);
  void count(RootedTree *v);
  void update_tI(unsigned int nodeIdx, bool count_unresolved=false);
  void update_tO(unsigned int nodeIdx, unsigned int color, bool count_unresolved=false);
  void update_tR(unsigned int nodeIdx);
  void compute_tA(RootedTree *v);
  bool find_optimal_root();
  void downroot(RootedTree *v, INTTYPE_REST parent_score, bool parent_active);      

  HDTFactory *dummyHDTFactory;
  RootedTree *myRef, *myTree;
  HDT *hdt;
  TripletCounter *tripCount;
  INTTYPE_REST optimalTripScore;
  unsigned int ambiguity;
  RootedTree *optimalRoot; // the node in myTree where the optimal root placed above
};

#endif
