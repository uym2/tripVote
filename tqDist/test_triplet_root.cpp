#include "templated_stack.h"
#include "rooted_tree_lb.h"
#include "TripletDistanceCalculator.h"
#include "test_util.hpp"
#include "newick_parser.h"
#include "TripletRooting.h"

#include <cassert>

void test_triplet_rooting() {
  UnrootedTree *ut1 = NULL;
  UnrootedTree *ut2 = NULL;
  RootedTree *rt1 = NULL;
  RootedTree *rt2 = NULL;
  RootedTree *rt3 = NULL;

  NewickParser parser;

  ut1 = parser.parseFile("../trees/tree_ab-c.new");
  ut2 = parser.parseFile("../trees/tree_ac-b.new");
  
  rt1 = ut1->convertToRootedTree(NULL);
  rt2 = ut2->convertToRootedTree(rt1->factory);

  TripletRooting trplRooting(rt1,rt2); 
  assert_equal(trplRooting.minTripletDistance(),(INTTYPE_REST) 0); 
}

int main(int argc, char **argv) {
  test_triplet_rooting();
  return 0;
}
