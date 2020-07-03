#ifndef SUBTREE_SAMPLER_H
#define SUBTREE_SAMPLER_H

#include "int_stuff.h"
#include "rooted_tree.h"
#include "TripletRooting.h"

class SubtreeSampler {
 public:
  RootedTree *myTree;
  unsigned int N;
  SubtreeSampler(RootedTree *tree);
  RootedTree* sample_subtree(unsigned int k);  // randomly sample one subtree of size k
  vector<RootedTree*> sample_by_d2root(unsigned int k, double size_proportion);
  vector<string> sample_by_splits();

 private:
};

#endif
