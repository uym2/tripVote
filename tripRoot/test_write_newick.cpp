#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TripletRooting.h"
#include <fstream>

#include <cassert>

int main(int argc, char **argv) {
  NewickParser parser;
  UnrootedTree *ut1 = parser.parseFile(argv[1]);
  RootedTree *rt1 = ut1->convertToRootedTree(NULL);
  ofstream fout;
  fout.open("temp.tre");
  rt1->write_newick(fout);
  fout.close();
  return 0;
}
