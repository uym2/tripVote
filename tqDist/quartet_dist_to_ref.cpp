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
  std::cout << "Usage: " << programName << " [-v] <refTree> <treeList> <outFile>" << std::endl 
	    << std::endl;
  std::cout << "Where <refTree> and <treeList> point to a file in Newick format each contains"                           << std::endl
	    << "a tree and a set of tree, respectively. All leaves in all trees should be labeled"                           << std::endl
	    << "and they should all have the same set of leaf labels."                                                       << std::endl;
  std::cout << "The quartet scores between <refTree> and each of the tree in <treeList> will be printed to <outFile>."      << std::endl;
  std::cout << "If the -v option is used, the following numbers will be reported (in this"    << std::endl
	    << "order):"                                                                      << std::endl
	    << "\t - The number of leaves in the trees (should be the same for both)."        << std::endl
	    << "\t - The number of quartets in the two trees (n choose 3)."                   << std::endl
	    << "\t - The quartet distance between the two trees."                             << std::endl
	    << "\t - The normalized quartet distance between the two trees."                  << std::endl
	    << "\t - The number of resolved quartets that agree in the two trees."            << std::endl
	    << "\t - The normalized number of resolved quartets that agree in the two trees." << std::endl
	    << "\t - The number quartets that are unresolved in both trees."                  << std::endl
	    << "\t - The normalized number quartets that are unresolved in both trees."       << std::endl
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
    cout << "Processing tree " << i << endl;  
    UnrootedTree *uRef;
    UnrootedTree *uTre;
    //RootedTree *rRef = NULL;
    //RootedTree *rTre = NULL;

    // read new tree to reroot it
    string treeStr;
    std::getline(fin,treeStr);
 
    if (fin.eof())
        break;
  
    // read reference tree
    uRef = parser.parseFile(refTreeFile);
    //rRef = uRef->convertToRootedTree(NULL);
    
    uTre = parser.parseStr(treeStr);
    //rTre = uTre->convertToRootedTree(rRef->factory);

    QuartetDistanceCalculator quartetCalc;
    INTTYPE_N4 dist = quartetCalc.calculateQuartetDistance(uRef,uTre);
    INTTYPE_N4 resolvedQuartetsAgree = quartetCalc.get_resolvedQuartetsAgree();
    INTTYPE_N4 resolvedQuartetsAgreeDiag = quartetCalc.get_resolvedQuartetsAgreeDiag();
    INTTYPE_N4 resolvedQuartetsDisagree = quartetCalc.get_resolvedQuartetsDisagree();
    INTTYPE_N4 resolvedQuartetsDisagreeDiag = quartetCalc.get_resolvedQuartetsDisagreeDiag();
    INTTYPE_N4 resolvedQuartetsAgreeUpper = quartetCalc.get_resolvedQuartetsAgreeUpper();
    INTTYPE_N4 resolvedQuartetsDisagreeUpper = quartetCalc.get_resolvedQuartetsDisagreeUpper();

    INTTYPE_N4 n = quartetCalc.get_n();
    INTTYPE_N4 totalNoQuartets = quartetCalc.get_totalNoQuartets();
    double dist_norm = double(dist) / double(totalNoQuartets);
    INTTYPE_N4 resAgree = resolvedQuartetsAgree + resolvedQuartetsAgreeDiag + resolvedQuartetsAgreeUpper;
    double resAgree_norm = double(resAgree) / double(totalNoQuartets);
    INTTYPE_N4 unresolvedQuartetsAgree = quartetCalc.get_unresolvedQuartets();
    double unresolvedQuartetsAgree_norm = double(unresolvedQuartetsAgree) / double(totalNoQuartets);
    
    if (!verbose)
       cout << dist_norm << endl;
    else{
        INTTYPE_N4 resolvedQuartetsAgree = quartetCalc.get_resolvedQuartetsAgree();
        INTTYPE_N4 resolvedQuartetsAgreeDiag = quartetCalc.get_resolvedQuartetsAgreeDiag();
        INTTYPE_N4 resolvedQuartetsDisagree = quartetCalc.get_resolvedQuartetsDisagree();
        INTTYPE_N4 resolvedQuartetsDisagreeDiag = quartetCalc.get_resolvedQuartetsDisagreeDiag();
        INTTYPE_N4 resolvedQuartetsAgreeUpper = quartetCalc.get_resolvedQuartetsAgreeUpper();
        INTTYPE_N4 resolvedQuartetsDisagreeUpper = quartetCalc.get_resolvedQuartetsDisagreeUpper();

        INTTYPE_N4 n = quartetCalc.get_n();
        INTTYPE_N4 totalNoQuartets = quartetCalc.get_totalNoQuartets();
        double dist_norm = double(dist) / double(totalNoQuartets);
        INTTYPE_N4 resAgree = resolvedQuartetsAgree + resolvedQuartetsAgreeDiag + resolvedQuartetsAgreeUpper;
        double resAgree_norm = double(resAgree) / double(totalNoQuartets);
        INTTYPE_N4 unresolvedQuartetsAgree = quartetCalc.get_unresolvedQuartets();
        double unresolvedQuartetsAgree_norm = double(unresolvedQuartetsAgree) / double(totalNoQuartets);
        
        fout << n                            << "\t"
              << totalNoQuartets              << "\t"
              << dist                         << "\t"
              << dist_norm                    << "\t"
              << resAgree                     << "\t"
              << resAgree_norm                << "\t"
              << unresolvedQuartetsAgree      << "\t"
              << unresolvedQuartetsAgree_norm << std::endl;
  }
    i++;
    delete uRef;
    delete uTre;
  }
  fin.close();  
  fout.close();    

  return 0;
}
