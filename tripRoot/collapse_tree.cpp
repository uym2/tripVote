#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "RefTreeGen.h"
#include "newick_parser.h"
#include "rooted_tree.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <inTree> <maxDepth> <outTree>" << std::endl 
	    << std::endl;
  std::cout << "Where <inTree> point to a filee containing one tree in Newick format."            << std::endl
            << "The tree will be collasped on the branches deeper than <maxDepth>. "              << std::endl;
  std::cout << "The output will be written to <outTree>, in Newick format."                       << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    usage(argv[0]);
    return 0;
  }


  char* inTree = argv[1];
  unsigned int maxDepth = atoi(argv[2]);
  char* outTree = argv[3];
  
  NewickParser parser;
    
  UnrootedTree *uTre = NULL;
  RootedTree *rTre = NULL;
  uTre = parser.parseFile(inTree);
  rTre = uTre->convertToRootedTree(NULL);

  RefTreeGen myGen;
  RootedTree *cTree = myGen.collapse_from_depth(rTre,maxDepth); 
  
  ofstream fout;
  fout.open(outTree);
  
  cTree->write_newick(fout);
  fout.close();    

  return 0;
}
