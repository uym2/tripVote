#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TripletRooting.h"
#include <fstream>

#include <cassert>

int main(int argc, char **argv) {
  /*ifstream fin;
  fin.open(argv[1]);
  string treeStr;
  std::getline(fin,treeStr);
  fin.close();*/
  
  RootedTree *rt1 = new RootedTree;
  rt1->initialize("");
  rt1->factory = new RootedTreeFactory();
  rt1->read_newick_file(argv[1]);

  ofstream fout;
  fout.open(argv[2]);
  rt1->write_newick(fout);
  fout.close();
  return 0;
}
