#include "rooted_tree_lb.h"
#include "TripletDistanceCalculator.h"
#include "test_util.hpp"
#include "newick_parser.h"

#include <cassert>

void test_down_root() {
  UnrootedTree *ut1 = NULL;
  UnrootedTree *ut2 = NULL;
  RootedTree *rt1 = NULL;
  RootedTree *rt2 = NULL;
  RootedTree *rt3 = NULL;

  NewickParser parser;

  ut1 = parser.parseFile("../trees/tree_ab-c.new");
  ut2 = parser.parseFile("../trees/tree_ab-c.new");
  
  rt1 = ut1->convertToRootedTree(NULL);
  rt2 = ut2->convertToRootedTree(rt1->factory);
  
  rt1->set_all_idx(0);

  TripletDistanceCalculator tripletCalc;
  assert_equal(tripletCalc.calculateTripletDistance(rt1,rt2), (INTTYPE_REST) 0);
  assert_equal(tripletCalc.calculateTripletDistance(rt2,rt1), (INTTYPE_REST) 0);

  rt3 = rt1->down_root(rt1->children[0].data->children[0].data);
  assert_equal(tripletCalc.calculateTripletDistance(rt1,rt3), (INTTYPE_REST) 1);
  assert_equal(tripletCalc.calculateTripletDistance(rt3,rt1), (INTTYPE_REST) 1);
}

void test_triplet_distance() {
  TripletDistanceCalculator tripletCalc;
  assert_equal(tripletCalc.calculateTripletDistance("../trees/tree_ab-c.new", "../trees/tree_ac-b.new"), (INTTYPE_REST) 1);
  assert_equal(tripletCalc.calculateTripletDistance("../trees/tree_ac-b.new", "../trees/tree_ab-c.new"), (INTTYPE_REST) 1);

  assert_equal(tripletCalc.calculateTripletDistance("../trees/test_tree1.new", "../trees/test_tree2.new"), (INTTYPE_REST) 26);
  assert_equal(tripletCalc.calculateTripletDistance("../trees/test_tree2.new", "../trees/test_tree1.new"), (INTTYPE_REST) 26);

  assert_equal(tripletCalc.calculateTripletDistance("../trees/test_tree3.new", "../trees/test_tree4.new"), (INTTYPE_REST) 187793);
  assert_equal(tripletCalc.calculateTripletDistance("../trees/test_tree4.new", "../trees/test_tree3.new"), (INTTYPE_REST) 187793);
}

void test_all_pairs_triplet_distance() {
  TripletDistanceCalculator tripletCalc;
  
  const std::vector<std::vector<double> > &result = tripletCalc.calculateAllPairsTripletDistance("../trees/two_trees.new");
  
  assert_equal(result[1][0], (double) 26);
  assert_equal(result[0][0], (double) 0);
  assert_equal(result[2][1], (double) 26);
}


int main(int argc, char **argv) {
  test_triplet_distance();
  test_all_pairs_triplet_distance();
  test_down_root();

  return 0;
}
