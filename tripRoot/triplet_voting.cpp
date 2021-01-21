#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include "int_stuff.h"
#include "TripletRooting.h"
#include "newick_parser.h"
#include "MVRooting.h"
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif


string rootFromVotes(string treeStr, vector<string> refTreeStrs, vector<double> weights ,bool size_scaling = true){
    RootedTreeFactory *tFactory = new RootedTreeFactory();
    RootedTree *myTree = tFactory->getRootedTree();
    myTree->factory = tFactory;
    myTree->read_newick_str(treeStr);    

    myTree->set_all_idx(0);
    myTree->count_nodes();
    myTree->countChildren();           

    double *allCounts = NULL;
    unsigned int N = 0;

    unsigned int idx = 0;
    
    for (std::vector<string>::iterator it = refTreeStrs.begin(); it != refTreeStrs.end(); ++it) {
        //cout << "Processing reference tree " << idx+1 << endl;

        RootedTreeFactory *rFactory = new RootedTreeFactory();
        RootedTree *refTree = rFactory->getRootedTree();
        refTree->factory = rFactory;
        refTree->read_newick_str(*it); 
        refTree->countChildren();           
        
        TripletRooting tripRoot;
        tripRoot.initialize(refTree,myTree);
        
        if (!tripRoot.find_optimal_root()){
            cerr << "rootFromVotes: Failed to find optimal root of this sample!" << endl;
            delete rFactory;
            continue;
        }
        
        TripletCounter* oneCount = tripRoot.tripCount;
        
        double M;
        if (size_scaling){
            M = double(refTree->n)*(refTree->n-1)*(refTree->n-2)/6;
        }
        else
            M = 1;     
        
        double w = weights[idx];

        // add oneCount to allCounts
        if (allCounts != NULL){
            if (oneCount->N != N)
                cout << "Size mismatch!" << endl;
            for (int i=0; i<N; i++)
                allCounts[i] += w*oneCount->tripScore[i]/M;
        } else {
            N = oneCount->N;
            allCounts = new double[N];
            for (int i=0; i<N; i++){
                allCounts[i] = w*oneCount->tripScore[i]/M;
            }
        }
        delete rFactory;
        ++idx;
        //sleep(1);
    }


    unsigned int best_node_idx = -1;    
    double best_vote = -1;

    for (int i=0; i<N; i++){
        if (allCounts[i] > best_vote){
            best_vote = allCounts[i];
            best_node_idx = i;
        }
    }

    unsigned int Ambiguity = 0;
    for (int i=0; i<N; i++){
        if (abs(allCounts[i]-best_vote) < 0.0001){
            Ambiguity++;
        }
    }
    
    cout << "Best vote score: " << best_vote << endl;
    cout << "Ambiguity: " << Ambiguity << endl;       
    cout << "Optimal root index: " << best_node_idx << endl;

    //cout << "MV score at original root: " << mvRoot.mvCount->minVar[myTree->idx] << endl;
    //cout << "MV score at best voted root: " << mvRoot.mvCount->minVar[best_node_idx] << endl;

    cout << "Vote score at original root: " << allCounts[myTree->idx] << endl;
    cout << "Vote score at best voted root: " << allCounts[best_node_idx] << endl;

    RootedTree *bestRoot = myTree->search_idx(best_node_idx);
    RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
    string rerooted_str = rerooted->toString();

    delete [] allCounts;
    delete tFactory;   
    return rerooted_str;
}

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <inTree> <outTree> <refTrees> <weights> " << std::endl 
	    << std::endl;
}

int main(int argc, char** argv) {
  clock_t start = clock();
  if(argc < 4 or argc > 5) {
    usage(argv[0]);
    return 0;
  }

  char *refTreeFile = argv[3];
  char *inTreeFile = argv[1];
  char *outputTree = argv[2];
  char *weightFile = NULL;
  if (argc == 4)
      char *weightFile = argv[4];
  
  NewickParser parser;
  unsigned int i = 1;

  vector<string> outTreeStrs;
  vector<string> refTreeStrs;
  vector<double> weights;
  
  string inTreeStr, treeStr;

  ifstream fin;
  fin.open(inTreeFile);
  std::getline(fin,inTreeStr);
  fin.close();
 
  fin.open(refTreeFile);
  while (1){
      std::getline(fin,treeStr);
      
      if (fin.eof())
          break;
      
      refTreeStrs.push_back(treeStr);
  }
  fin.close();

  if (weightFile){
      fin.open(weightFile);
      string w;
      while (1){
          std::getline(fin,w);
          if (fin.eof())
              break;
          weights.push_back(exp(-10*atof(w.c_str())));
      }
      fin.close();
  } else{
      for (int i =0; i< refTreeStrs.size(); i++){
          weights.push_back(1);
      }
  }
   
  string outTreeStr = rootFromVotes(inTreeStr, refTreeStrs,weights);    
 
  ofstream fout;
  fout.open(outputTree);
  fout << outTreeStr << endl;
  fout.close();
  clock_t stop = clock();
  double duration = double(stop - start) / CLOCKS_PER_SEC;
  cout << "TripVote runtime: " << duration << std::endl;
  return 0;
}
