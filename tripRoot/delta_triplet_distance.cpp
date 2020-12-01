#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <refTree> <inTree>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <inTree> are two trees in Newick format"                                    << std::endl
	    << "In both trees all leaves should be labeled and the"                                                 << std::endl
	    << "two trees must have the same leaf set. Output:"                                                     << std::endl                  
        << " + The number of leaves in each tree"                                                               << std::endl
        << " + The number of triplets in each tree"                                                             << std::endl
        << " + The triplet distance between the two trees"                                                      << std::endl
        << " + The minimum triplet distance of any rooting of <inTree> to <refTree>"                            << std::endl
        << " + The delta triplet distance"                                                                      << std::endl
        << " + The normalized delta triplet distance"                                                           << std::endl
        << "Note that the delta triplet distance is NOT symmetric."                                             << std::endl;
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }

  bool verbose = false;

  if(argc < 3) {
    std::cerr << "Error: Not enough parameters!" << std::endl;
    usage(argv[0]);
    return -1;
  }

  if(argc == 5 && strcmp(argv[1],"-v") == 0) {
    verbose = true;
  }

  char *refTreeFile = argv[argc-2];
  char *myTreeFile = argv[argc-1];

  vector<string> ref_nwk;
  vector<string> tre_nwk;

  ifstream fin;
  fin.open(refTreeFile);
  while (! fin.eof()){
    string tree;
    fin >> tree;
    if (! tree.empty())
        ref_nwk.push_back(tree);
  }
  fin.close();  
  
  fin.open(myTreeFile);
  while (! fin.eof()){
    string tree;
    fin >> tree;
    if (! tree.empty())
        tre_nwk.push_back(tree);
  }
  fin.close();  
  
  if (ref_nwk.size() != tre_nwk.size()) {
      std::cout << "Input files have different number of trees!" << std::endl;
  } else {
      for (int i = 0; i <ref_nwk.size(); i++) {
          RootedTreeFactory *rFactory = new RootedTreeFactory();
          RootedTreeFactory *tFactory = new RootedTreeFactory();
          RootedTree *rRef = rFactory->getRootedTree();
          RootedTree *rTre = tFactory->getRootedTree();
          rRef->factory = rFactory;
          rTre->factory = tFactory;
          
          rRef->read_newick_str(ref_nwk[i]);
          rTre->read_newick_str(tre_nwk[i]);

          TripletRooting tripRoot;
          tripRoot.initialize(rRef,rTre);
          tripRoot.find_optimal_root();
          rTre->countChildren();
          INTTYPE_REST n = rTre->n;
          INTTYPE_REST ntrpls = n*(n-1)*(n-2)/6;
          INTTYPE_REST dtrpl_root = ntrpls-tripRoot.compute_root_tripScore();
          INTTYPE_REST dtrpl_ideal = ntrpls-tripRoot.optimalTripScore;
          std::cout << n << " " << ntrpls << " " <<  dtrpl_root << " " << dtrpl_ideal << " " << dtrpl_root-dtrpl_ideal << " " << double(dtrpl_root-dtrpl_ideal)/ntrpls << std::endl;
          delete rFactory;
          delete tFactory;
      }
  }
  return 0;}
