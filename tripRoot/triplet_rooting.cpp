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
  std::cout << "Where <refTree> and <inTree> point to two files each containing one"                            << std::endl
	    << "tree in Newick format. In both trees all leaves should be labeled and the"                          << std::endl
	    << "two trees should share some a majority of the leaf labels. <inTree> will be (re)rooted"             << std::endl
        << "such that its triplet score to <refTree> is maximized. The (re)rooted tree"                         << std::endl                
        << "will be written to <outTree>"                                                                       << std::endl;
  std::cout << "The maximum triplet score between the two trees will be printed to stdout."   << std::endl;
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
  
  NewickParser parser;

  
  ofstream fout;
  fout.open(outputTree);

  ifstream fin;
  fin.open(myTreeFile);
  
  unsigned int i = 1;

  while (1){
    RootedTreeFactory *rFactory = new RootedTreeFactory();
    RootedTreeFactory *tFactory = new RootedTreeFactory();
    RootedTree *rRef = rFactory->getRootedTree();
    RootedTree *rTre = tFactory->getRootedTree();

    //rRef->initialize();
    //rTre->initialize();

    rRef->factory = rFactory;
    rTre->factory = tFactory;

    // read new tree to reroot it
    string treeStr;
    std::getline(fin,treeStr);
 
    if (fin.eof())
        break;
  
    std::cout << "Processing tree " << i << std::endl;
    
    // read reference tree
    rRef->read_newick_file(refTreeFile);

    // read new input tree
    rTre->read_newick_str(treeStr);    

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

    i++;
  }
  fin.close();  
  fout.close();    

  return 0;
}
