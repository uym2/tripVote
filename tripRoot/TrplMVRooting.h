#ifndef TRIPLET_MV_ROOTING_H
#define TRIPLET_MV_ROOTING_H

#include "int_stuff.h"
#include "triplet_calc.h"
#include "quartet_calc.h"
#include "MinVarCounter.h"
#include "TripletRooting.h"

#ifndef CONTRACT_MAX_EXTRA_SIZE
#define CONTRACT_MAX_EXTRA_SIZE 20000
#endif

class TrplMVRooting : public TripletRooting {
 public:
  MinVarCounter *mvCount; 
  double optimalVarScore; 
  TrplMVRooting();
  ~TrplMVRooting();
  bool initialize(RootedTree *ref, RootedTree *tree);
  bool find_optimal_root();
  double compute_root_var();
  bool compute_varScore(); // compute the MV score for each branch
 private: 
  void __compute_root_var__(RootedTree *t, double & nleaves, double d2root, double & acc_sum, double & acc_sumSquare);
  void __compute_SI__(RootedTree *t);
  void __compute_ST__(RootedTree *t);
  void __compute_var__(RootedTree *t);
};

#endif
