#include "int_stuff.h"
#include "rooted_tree.h"
#include <set>
#include <string>
#include <fstream>

class OutgroupRooting {
 public:
  OutgroupRooting();
  ~OutgroupRooting();
  void initialize(RootedTree *myTree, set<string> outgroups);
  RootedTree* findRoot();  
  RootedTree* reroot();
 private: 
  RootedTree *myTree;
  set<string> myOGs;
  unsigned int N; 
  unsigned int *myCounters;  
  RootedTree* __find_one_ingroup__(RootedTree *tree, set<string> &outgroups);
  RootedTree* __find_biggest_clade__(RootedTree *tree);
};

