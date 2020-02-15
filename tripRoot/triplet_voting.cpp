#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting.h"
#include "newick_parser.h"
#include "MVRooting.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif


RootedTree* rootFromVotes(RootedTree *myTree, char *refTreeFile, bool size_scaling = true){
    myTree->set_all_idx(0);
    myTree->count_nodes();

    double *allCounts = NULL;
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
        refTree->countChildren();           
        
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
        
        double M;
        if (size_scaling)
            M = refTree->n*(refTree->n-1)*(refTree->n-2)/6;
        else
            M = 1;     
        cout << M << endl;
    
        // add oneCount to allCounts
        if (allCounts != NULL){
            if (oneCount->N != N)
                cout << "Size mismatch!" << endl;
            for (int i=0; i<N; i++)
                allCounts[i] += oneCount->tripScore[i]/M;
        } else {
            N = oneCount->N;
            allCounts = new double[N];
            for (int i=0; i<N; i++){
                allCounts[i] = oneCount->tripScore[i]/M;
            }
        }
    }

    fin.close();

    unsigned int best_node_idx = -1;    
    double best_vote = -1;

    for (int i=0; i<N; i++){
        if (allCounts[i] > best_vote){
            best_vote = allCounts[i];
            best_node_idx = i;
        }
    }
    MVRooting mvRoot;
    mvRoot.initialize(myTree);
    mvRoot.compute_score();
    unsigned int Ambiguity = 0;
    double min_MV = mvRoot.mvCount->minVar[best_node_idx];

    for (int i = 0; i<N; i++){
        if (allCounts[i] == best_vote){
            Ambiguity++;
            if (mvRoot.mvCount->minVar[i] < min_MV){
                min_MV = mvRoot.mvCount->minVar[i];
                best_node_idx = i;
            }
        }
    }

    cout << "Best vote score: " << best_vote << endl;
    cout << "Ambiguity: " << Ambiguity << endl;       
    cout << "Optimal root index: " << best_node_idx << endl;

    cout << "MV score at original root: " << mvRoot.mvCount->minVar[myTree->idx] << endl;
    cout << "MV score at best voted root: " << mvRoot.mvCount->minVar[best_node_idx] << endl;

    cout << "Vote score at original root: " << allCounts[myTree->idx] << endl;
    cout << "Vote score at best voted root: " << allCounts[best_node_idx] << endl;


    return myTree->search_idx(best_node_idx);
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
  char *inTreeFile = argv[1];
  char *outputTree = argv[2];
  
  NewickParser parser;
  unsigned int i = 1;

  ifstream fin;
  fin.open(inTreeFile);

  ofstream fout;
  fout.open(outputTree);

  while (1){
      std::cout << "Rooting tree " << i << std::endl;
      string treeStr;
      std::getline(fin,treeStr);

      if (fin.eof())
          break;

      RootedTreeFactory *tFactory = new RootedTreeFactory();
      RootedTree *myTree = tFactory->getRootedTree();
      myTree->factory = tFactory;
      myTree->read_newick_str(treeStr);

      RootedTree *bestRoot = rootFromVotes(myTree, refTreeFile);    
      RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
      
      //ofstream fout;
      //fout.open(outputTree);  
      rerooted->write_newick(fout);
      fout << endl;
      //fout.close(); 
      delete tFactory;   
      i++;
  }  
  fin.close();
  fout.close();
  
  return 0;
}
