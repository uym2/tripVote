#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "OutgroupRooting.h"
#include "newick_parser.h"
#include <set>
#include <string>

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout     << "Usage: " << programName << " <inTree> <outTree> <outgroups>"                     << std::endl << std::endl;
  std::cout     << "Where <inTree> point to a file containing one"                                   << std::endl
	            << "tree in Newick format; <outTree> is the name of the output file;"                << std::endl
                <<  "<outgroups> is a list of outgroup labels"                                       << std::endl 
                << "The tree will be (re)rooted at the biggest clade containing only outgroups"      << std::endl;
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }
  char *inTreeFile = argv[1];
  char *outTreeFile = argv[2];

  set<string> OGs;
  for (int i = 3; i < argc; i++){
    OGs.insert(argv[i]);
  }

  NewickParser parser;
  
  ofstream fout;
  fout.open(outTreeFile);

  ifstream fin;
  fin.open(inTreeFile);
  
  unsigned int i = 1;

  while (1){
    UnrootedTree *uTre = NULL;
    RootedTree *rTre = NULL;

    // read new tree to reroot it
    string treeStr;
    std::getline(fin,treeStr);
 
    if (fin.eof())
        break;
  
    std::cout << "Processing tree " << i << std::endl;
    
    uTre = parser.parseStr(treeStr);
    rTre = uTre->convertToRootedTree(NULL);

    OutgroupRooting ogRoot;
    ogRoot.initialize(rTre,OGs);
    RootedTree *rerooted = ogRoot.reroot();
    rerooted->write_newick(fout);
    fout << endl;

    i++;
  }
  fin.close();  
  fout.close();    

  return 0;
}
