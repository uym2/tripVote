#include "rooted_tree_lb.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TripletRooting.h"

#include <cassert>

void test(const char* rFile, const char* tFile, INTTYPE_REST correct_value) {
  UnrootedTree *ut1 = NULL;
  UnrootedTree *ut2 = NULL;
  RootedTree *rt1 = NULL;
  RootedTree *rt2 = NULL;
  RootedTree *rt3 = NULL;

  NewickParser parser;

  ut1 = parser.parseFile(rFile);
  ut2 = parser.parseFile(tFile);
  
  rt1 = ut1->convertToRootedTree(NULL);
  rt2 = ut2->convertToRootedTree(rt1->factory);

  TripletRooting trplRooting(rt1,rt2);
  trplRooting.find_optimal_root(); 
  assert_equal(trplRooting.optimalTripScore,correct_value); 
}

int main(int argc, char **argv) {
  test("../trees/tree_ab-c.new","../trees/tree_ab-c.new", (INTTYPE_REST) 1); 
  test("../trees/test_tree1.new","../trees/test_tree2.new", (INTTYPE_REST) 13);  
  test("../trees/test_tree3.new","../trees/test_tree4.new", (INTTYPE_REST) 97005);  
  test("../trees/taxonomy.tre","../trees/astral.MR.tre", (INTTYPE_REST) 125355891855);  
  return 0;
}
