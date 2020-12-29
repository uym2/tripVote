#ifndef MV_ROOTING_H
#define MV_ROOTING_H

#include "MinVarCounter.h"

class RootedTree;

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
  void compute_score();
  void countChildren(RootedTree *t);
  double compute_root_var();

 private: 
  void __compute_root_var__(RootedTree *t, double & nleaves, double d2root, double & acc_sum, double & acc_sumSquare);
  void __compute_SI__(RootedTree *t);
  void __compute_ST__(RootedTree *t);
  void __compute_var__(RootedTree *t);
};

#endif
