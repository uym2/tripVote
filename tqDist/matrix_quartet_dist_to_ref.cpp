#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "QuartetDistanceCalculator.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " [-v] <treeList> <outFile>" << std::endl 
	    << std::endl;
  // std::cout << "Where <treeList> point to a file in Newick format each contains a set of tree. "                           << std::endl
  // std::cout << "The quartet scores between each tree and all the trees in <treeList> will be printed to <outFile>."      << std::endl;
  
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }

  if(argc < 3) {
    std::cerr << "Error: Not enough parameters!" << std::endl;
    usage(argv[0]);
    return -1;
  }

  char *myTreeFile = argv[argc-2];
  char *outputFile = argv[argc-1];

  NewickParser parser;

  ofstream fout;
  fout.open(outputFile);

  ifstream fin;
  fin.open(myTreeFile);

  // read tree list
  string treeStr;
  std::vector<std::string> treeList;
  while (std::getline(fin, treeStr)) {
    treeList.push_back(treeStr);
  }
  fin.close();

  UnrootedTree *uRef;
  UnrootedTree *uTre; 
  QuartetDistanceCalculator quartetCalc;
  INTTYPE_N4 dist;
  INTTYPE_N4 totalNoQuartets;
  double dist_norm;
  
  int numTree = treeList.size();

  // output the weights in the symmetric weight matrix
  for (int i = 0; i < numTree; i++) {
    uRef = parser.parseStr(treeList[i]);
    for (int j = 0; j <= i; j++) {
      uTre = parser.parseStr(treeList[j]);
      dist = quartetCalc.calculateQuartetDistance(uRef,uTre);
      totalNoQuartets = quartetCalc.get_totalNoQuartets();
      if (totalNoQuartets == 0) dist_norm = 1;
      else dist_norm = double(dist) / double(totalNoQuartets);
      fout << dist_norm << endl;
      // cout << "i = " << i << " "<< "j = " << j << " "<< "dist_norm is " << dist_norm << endl;
      // cout << dist_norm << endl;
      delete uTre;
    }
    delete uRef;
  }
  
  fout.close();    

  return 0;
}
