#ifndef MV_ROOTING_H
#define MV_ROOTING_H

#include "int_stuff.h"
#include "triplet_calc.h"
#include "quartet_calc.h"
#include "MinVarCounter.h"
#include "TripletRooting.h"

#ifndef CONTRACT_MAX_EXTRA_SIZE
#define CONTRACT_MAX_EXTRA_SIZE 20000
#endif

class MVRooting {
 public:
  RootedTree *myTree;
  MinVarCounter *mvCount; 
  double optimalVarScore; 
  RootedTree *optimalRoot;

  MVRooting();
  ~MVRooting();
  bool initialize(RootedTree *tree);
  RootedTree* root_tree();
  void countChildren(RootedTree *t);
  double compute_root_var();

 private: 
  void __compute_root_var__(RootedTree *t, double & nleaves, double d2root, double & acc_sum, double & acc_sumSquare);
  void __compute_SI__(RootedTree *t);
  void __compute_ST__(RootedTree *t);
  void __compute_var__(RootedTree *t);
};

#endif
