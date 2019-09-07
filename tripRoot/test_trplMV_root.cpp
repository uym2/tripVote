#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TrplMVRooting.h"

#include <fstream>
#include <cassert>

void test(const char* rFile, const char* tFile) {
  RootedTreeFactory *factory = new RootedTreeFactory();
  RootedTree *rt1 = new RootedTree;
  RootedTree *rt2 = new RootedTree;

  rt1->factory = factory;
  rt2->factory = factory;

  ifstream fin;
  fin.open(rFile);    
  rt1->read_newick(fin);
  fin.close();
  
  fin.open(tFile);
  rt2->read_newick(fin);
  fin.close();
  
  TrplMVRooting trplRooting;
  trplRooting.initialize(rt1,rt2);
  trplRooting.compute_varScore(); 
  cout << trplRooting.optimalVarScore << endl;

  delete factory;
  delete rt1;
  delete rt2;
}

int main(int argc, char **argv) {
  test(argv[1],argv[1]); 
  return 0;
}
