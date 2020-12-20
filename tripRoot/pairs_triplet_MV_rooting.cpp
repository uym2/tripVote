#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TrplMVRooting.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <refTree> <inTree> <outTree>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <inTree> are two trees in Newick format"                                    << std::endl
	    << "In both trees all leaves should be labeled and the"                                                 << std::endl
	    << "two trees must have the same leaf set. Output: <outTree> is"                                        << std::endl                  
        << "a rerooted version of <inTree> that has the minimum triplet distance to <refTree>"                  << std::endl;
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

  char *refTreeFile = argv[argc-3];
  char *myTreeFile = argv[argc-2];
  char *outputTree = argv[argc-1];

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
      ofstream fout;
      fout.open(outputTree);
      for (int i = 0; i <ref_nwk.size(); i++) {
          RootedTreeFactory *rFactory = new RootedTreeFactory();
          RootedTreeFactory *tFactory = new RootedTreeFactory();
          RootedTree *rRef = rFactory->getRootedTree();
          RootedTree *rTre = tFactory->getRootedTree();
          rRef->factory = rFactory;
          rTre->factory = tFactory;
          
          rRef->read_newick_str(ref_nwk[i]);
          rTre->read_newick_str(tre_nwk[i]);
            
          TrplMVRooting tripRoot;
          tripRoot.initialize(rRef,rTre);
          tripRoot.find_optimal_root();
          std::cout << "Optimal triplet score: " << tripRoot.optimalTripScore << endl;
          std::cout << "Ambiguity: " << tripRoot.ambiguity << endl;
          std::cout << "MV score at the selected root: " << tripRoot.mvCount->minVar[tripRoot.optimalRoot->idx] << endl;
            
          double x =  tripRoot.mvCount->xStar[tripRoot.optimalRoot->idx];
          RootedTree *rerooted = rTre->reroot_at_edge(tripRoot.optimalRoot,x);
          rerooted->write_newick(fout);
          fout << endl;

          delete rFactory;
          delete tFactory;
      }
      fout.close();
  }
  return 0;}
