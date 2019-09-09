#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletDistanceCalculator.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " [-v] <refTree> <treeList> <outFile>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <treeList> point to a file in Newick format each contains"                           << std::endl
	    << "a tree and a set of tree, respectively. All leaves in all trees should be labeled"                           << std::endl
	    << "and they should all have the same set of leaf labels."                                                       << std::endl;
  std::cout << "The triplet scores between <refTree> and each of the tree in <treeList> will be printed to <outFile>."      << std::endl;
  std::cout << "If the -v option is used, the following numbers will be reported (in this"    << std::endl
	    << "order):"                                                                      << std::endl
	    << "\t - The number of leaves in the trees (should be the same for both)."        << std::endl
	    << "\t - The number of triplets in the two trees (n choose 3)."                   << std::endl
	    << "\t - The triplet distance between the two trees."                             << std::endl
	    << "\t - The normalized triplet distance between the two trees."                  << std::endl
	    << "\t - The number of resolved triplets that agree in the two trees."            << std::endl
	    << "\t - The normalized number of resolved triplets that agree in the two trees." << std::endl
	    << "\t - The number triplets that are unresolved in both trees."                  << std::endl
	    << "\t - The normalized number triplets that are unresolved in both trees."       << std::endl
	    << std::endl;
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
  char *outputFile = argv[argc-1];
  
  NewickParser parser;

  
  ofstream fout;
  fout.open(outputFile);

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
  
    // read reference tree
    uRef = parser.parseFile(refTreeFile);
    rRef = uRef->convertToRootedTree(NULL);
    
    uTre = parser.parseStr(treeStr);
    rTre = uTre->convertToRootedTree(rRef->factory);

    TripletDistanceCalculator tripletCalc;
    INTTYPE_REST dist = tripletCalc.calculateTripletDistance(rRef,rTre);
    
    if (!verbose)
       fout << dist << endl;
    else{
    INTTYPE_REST n = tripletCalc.get_n();
    INTTYPE_REST totalNoTriplets = tripletCalc.get_totalNoTriplets();
    INTTYPE_REST resolved = tripletCalc.get_resolvedTriplets();
    INTTYPE_REST unresolved = tripletCalc.get_unresolvedTriplets();
    double dist_norm = double(dist) / double(totalNoTriplets);
    double resolved_norm = double(resolved) / double(totalNoTriplets);
    double unresolved_norm = double(unresolved) / double(totalNoTriplets);

    fout  << n               << "\t"
	      << totalNoTriplets << "\t"
	      << dist            << "\t"
	      << dist_norm       << "\t"
	      << resolved        << "\t"
	      << resolved_norm   << "\t"
	      << unresolved      << "\t"
	      << unresolved_norm << endl;
  }
  
    i++;
  }
  fin.close();  
  fout.close();    

  return 0;
}
