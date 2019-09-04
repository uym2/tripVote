#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting_OG.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << "<refTree> <newTree>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <newTree> point to two files each containing one"         << std::endl
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


  if(argc < 4) {
    std::cerr << "Error: Not enough parameters!" << std::endl;
    usage(argv[0]);
    return -1;
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
    UnrootedTree *uRef = NULL;
    UnrootedTree *uTre = NULL;
    RootedTree *rRef = NULL;
    RootedTree *rTre = NULL;

    // read new tree to reroot it
    string treeStr;
    std::getline(fin,treeStr);
 
    if (fin.eof())
        break;
  
    std::cout << "Processing tree " << i << std::endl;
    
    // read reference tree
    uRef = parser.parseFile(refTreeFile);
    rRef = uRef->convertToRootedTree(NULL);
    
    uTre = parser.parseStr(treeStr);
    rTre = uTre->convertToRootedTree(rRef->factory);

    TripletRootingOG tripRoot(rRef,rTre,"###");
    tripRoot.find_optimal_root();
    std::cout << "Optimal triplet score: " << tripRoot.optimalTripScore << endl;
    
    //std::cout << "Rooting at sub tree: " << endl;
    //tripRoot.optimalRoot->print_leaves();
    //std::cout << endl;

    RootedTree *rerooted = rTre->reroot_at_edge(tripRoot.optimalRoot);
    rerooted->write_newick(fout);
    fout << endl;

    delete rRef->factory;

    i++;
  }
  fin.close();  
  fout.close();    

  return 0;
}
