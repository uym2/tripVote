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
  std::cout << "Usage: " << programName << " [-v] <refTree> <newTree>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <newTree2> point to two files each containing one"        << std::endl
	    << "tree in Newick format. In both trees all leaves should be labeled and the"        << std::endl
	    << "two trees should have the same set of leaf labels. <newTree> will be (re)rooted"  << std::endl
        << "such that its triplet score to <refTree> is maximized"                            << std::endl;
  std::cout << "The maximum triplet score between the two trees will be printed to stdout."    << std::endl;
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }

  bool verbose = false;

  if(argc < 4) {
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
  
  UnrootedTree *uRef = NULL;
  UnrootedTree *uTre = NULL;
  RootedTree *rRef = NULL;
  RootedTree *rTre = NULL;

  NewickParser parser;

  uRef = parser.parseFile(refTreeFile);
  rRef = uRef->convertToRootedTree(NULL);
  
  ofstream fout;
  fout.open(outputTree);

  ifstream fin;
  fin.open(myTreeFile);
  
  unsigned int i = 1;

  while (! fin.eof()){
    std::cout << "Processing tree " << i << std::endl;
    string treeStr;
    std::getline(fin,treeStr);
    uTre = parser.parseStr(treeStr);
  
    rTre = uTre->convertToRootedTree(rRef->factory);

    TripletRooting tripRoot(rRef,rTre);
    tripRoot.find_optimal_root();
    std::cout << "Optimal triplet score: " << tripRoot.optimalTripScore << endl;
    RootedTree *rerooted = rTre->reroot_at_edge(tripRoot.optimalRoot);
    rerooted->write_newick(fout);
  }
  fin.close();  
  fout.close();  

  return 0;
}
