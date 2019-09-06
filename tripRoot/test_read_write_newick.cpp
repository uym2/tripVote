#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TripletRooting.h"
#include <fstream>

#include <cassert>

int main(int argc, char **argv) {
  NewickParser parser;
  //UnrootedTree *ut1 = parser.parseFile(argv[1]);
  //RootedTree *rt1 = ut1->convertToRootedTree(NULL);
  ifstream fin;
  fin.open(argv[1]);
  RootedTree *rt1 = new RootedTree;
  //rt1->initialize("");
  rt1->factory = new RootedTreeFactory();
  rt1->read_newick(fin);
  fin.close();
  ofstream fout;
  fout.open(argv[2]);
  rt1->write_newick(fout);
  fout.close();
  return 0;
}
