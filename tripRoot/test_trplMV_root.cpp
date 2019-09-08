#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TrplMVRooting.h"

#include <fstream>
#include <cassert>

void test(const char* rFile, const char* tFile) {
  RootedTreeFactory *factory1 = new RootedTreeFactory();
  RootedTreeFactory *factory2 = new RootedTreeFactory();
  RootedTree *rt1 = new RootedTree;
  RootedTree *rt2 = new RootedTree;

  rt1->factory = factory1;
  rt2->factory = factory2;

  rt1->read_newick_file(rFile);
  rt2->read_newick_file(tFile);
  
  TrplMVRooting trplRooting;
  trplRooting.initialize(rt1,rt2);
  trplRooting.compute_varScore(); 
  cout << trplRooting.optimalVarScore << endl;

  delete factory1;
  delete factory2;
  delete rt1;
  delete rt2;
}

int main(int argc, char **argv) {
  test(argv[1],argv[1]); 
  return 0;
}
