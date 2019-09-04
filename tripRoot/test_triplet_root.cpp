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

  TripletRooting trplRooting;
  trplRooting.initialize(rt1,rt2);
  trplRooting.find_optimal_root(); 
  assert_equal(trplRooting.optimalTripScore,correct_value); 
}

int main(int argc, char **argv) {
  test("../trees/tree_ab-c.new","../trees/tree_ab-c.new", (INTTYPE_REST) 1); 
  test("../trees/test_tree1.new","../trees/test_tree2.new", (INTTYPE_REST) 13);  
  test("../trees/test_tree3.new","../trees/test_tree4.new", (INTTYPE_REST) 97005);  
  test("../trees/taxonomy.tre","../trees/astral.MR.tre", (INTTYPE_REST) 125355891855);  
  test("../trees/1kp-taxonomy_pruned673.tre", "../trees/kp_tree673.tre", (INTTYPE_REST) 23);  
  test("../trees/1kp-taxonomy_pruned1.tre", "../trees/kp_tree1.tre", (INTTYPE_REST) 25702);  
  test("../trees/1kp-taxonomy_pruned852.tre", "../trees/kp_tree852.tre", (INTTYPE_REST) 41252);  
  test("../trees/fourLeaves_2.tre", "../trees/sixLeaves.tre", (INTTYPE_REST) 3);  
  test("../trees/fourLeaves.tre", "../trees/sixLeaves.tre", (INTTYPE_REST) 4);  
  return 0;
}
