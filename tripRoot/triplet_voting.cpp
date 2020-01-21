#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif


RootedTree* rootFromVotes(RootedTree *myTree, char *refTreeFile){
    myTree->set_all_idx(0);
    myTree->count_nodes();

    INTTYPE_REST *allCounts = NULL;
    unsigned int N = 0;
    
    ifstream fin;
    fin.open(refTreeFile);

    unsigned int tr = 1;

    while(1){
        string treeStr;
        std::getline(fin,treeStr);
        if (fin.eof())
            break;
        
        cout << "Processing tree " << tr << endl;
        tr++;
        
        RootedTreeFactory *rFactory = new RootedTreeFactory();
        RootedTree *refTree = rFactory->getRootedTree();
        refTree->factory = rFactory;
        refTree->read_newick_str(treeStr); 
           
        
        TripletRooting tripRoot;
        tripRoot.initialize(refTree,myTree);
        if (!tripRoot.find_optimal_root()){
            cout << "Failed to find optimal root of this sample!" << endl;
            continue;
        }
        TripletCounter* oneCount = tripRoot.tripCount;
        
        //for (int i = 0; i<oneCount->N; i++)
        //    cout << oneCount->tripScore[i] << " ";
        //cout << endl;

        // add oneCount to allCounts
        if (allCounts != NULL){
            if (oneCount->N != N)
                cout << "Size mismatch!" << endl;
            for (int i=0; i<N; i++)
                allCounts[i] += oneCount->tripScore[i];
        } else {
            N = oneCount->N;
            allCounts = new INTTYPE_REST[N];
            for (int i=0; i<N; i++){
                allCounts[i] = oneCount->tripScore[i];
            }
        }
    }

    fin.close();

    unsigned int best_node_idx = -1;    
    INTTYPE_REST best_vote = -1;

    for (int i=0; i<N; i++){
        if (allCounts[i] > best_vote){
            best_vote = allCounts[i];
            best_node_idx = i;
        }
    }

    unsigned int Ambiguity = 0;
    for (int i = 0; i<N; i++){
        //cout << allCounts[i] << " ";
        if (allCounts[i] == best_vote)
            Ambiguity++;
    }
    cout << endl;

    cout << "Best vote score: " << best_vote << endl;
    cout << "Ambiguity: " << Ambiguity << endl;       

    if (allCounts != NULL)
        delete [] allCounts;
    
    RootedTree* opt_root = myTree->search_idx(best_node_idx);

    return opt_root;
}

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <inTree> <outTree> <refTrees> " << std::endl 
	    << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 4) {
    usage(argv[0]);
    return 0;
  }

  char *refTreeFile = argv[3];
  char *myTreeFile = argv[1];
  char *outputTree = argv[2];
  
  NewickParser parser;


  RootedTreeFactory *tFactory = new RootedTreeFactory();
  RootedTree *myTree = tFactory->getRootedTree();
  myTree->factory = tFactory;
  myTree->read_newick_file(myTreeFile);

  RootedTree *bestRoot = rootFromVotes(myTree, refTreeFile);    
  RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
  
  ofstream fout;
  fout.open(outputTree);  
  rerooted->write_newick(fout);
  fout.close();    

  return 0;
}
