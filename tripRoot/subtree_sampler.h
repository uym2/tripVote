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
  RootedTree* sample_subtree(unsigned int k);  

 private: 
};

#endif