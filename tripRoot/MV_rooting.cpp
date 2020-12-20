#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "MVRooting.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <inTree> <outTree>" << std::endl;
  std::cout << std::endl;
  std::cout << "Where <inTree> is a newick tree. The tree will be" << std::endl
            << "(re)rooted to minimize root-to-tip variance and "  << std::endl
            << "output to <outTree> file"                          << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    usage(argv[0]);
    return 0;
  }

  char *inTreeFile = argv[1];
  char *outTreeFile = argv[2];
  
  NewickParser parser;

  
  ofstream fout;
  fout.open(outTreeFile);

  ifstream fin;
  fin.open(inTreeFile);
  
  unsigned int i = 1;

  while (1){
    std::cout << "Processing tree " << i << std::endl;
    string treeStr;
    std::getline(fin,treeStr);
 
    if (fin.eof())
        break;
    
    RootedTreeFactory *tFactory = new RootedTreeFactory();
    RootedTree *myTree = tFactory->getRootedTree();
    myTree->factory = tFactory;
    myTree->read_newick_str(treeStr);    
    myTree->countChildren();
    myTree->count_nodes();
    myTree->set_all_idx(0);
    
    MVRooting mvRoot;
    mvRoot.initialize(myTree);
    RootedTree *rerooted = mvRoot.root_tree();
    rerooted->write_newick(fout);
    fout << endl;

    delete tFactory;
    i++;
  }
  fin.close();  
  fout.close();    

  return 0;
}
