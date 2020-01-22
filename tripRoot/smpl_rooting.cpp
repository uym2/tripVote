#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting.h"
#include "MVRooting.h"
#include "subtree_sampler.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

TripletCounter* oneVote(RootedTree *myTree, RootedTree *refTree){
    TripletRooting tripRoot;
    tripRoot.initialize(refTree,myTree);
    if (tripRoot.find_optimal_root())
        return tripRoot.tripCount;
    else    
        return NULL;
}

RootedTree* rootFromSamples(RootedTree *myTree, unsigned int n_smpl){
    myTree->set_all_idx(0);
    myTree->count_nodes();
    
    ofstream fout;
    fout.open("smpl_trees.txt");

    INTTYPE_REST *allCounts = NULL;
    SubtreeSampler sampler(myTree);
    unsigned int k = sampler.N/2;
    unsigned int N = 0;
    
    for (int i=0; i < n_smpl; i++){
        cout << "Generating sample " << i+1 << endl;

        RootedTree* tree = sampler.sample_subtree(k);  
        
        MVRooting mvRoot;
        mvRoot.initialize(tree);
        RootedTree* refTree = mvRoot.root_tree(); 
        refTree->write_newick(fout);
        fout << endl;
        
        TripletRooting tripRoot;
        tripRoot.initialize(refTree,myTree);
        tripRoot.find_optimal_root();        
        TripletCounter* oneCount = tripRoot.tripCount;
        
        // add oneCount to allCounts
        if (allCounts != NULL){
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
    fout.close();

    unsigned int best_node_idx = -1;    
    INTTYPE_REST best_vote = -1;

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
  std::cout << "Usage: " << programName << " <inTree> <outTree> <n_samples> " << std::endl 
	    << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 4) {
    usage(argv[0]);
    return 0;
  }

  unsigned int n_smpl = std::stoi(argv[3]);
  char *myTreeFile = argv[1];
  char *outputTree = argv[2];
  
  NewickParser parser;


  RootedTreeFactory *tFactory = new RootedTreeFactory();
  RootedTree *myTree = tFactory->getRootedTree();
  myTree->factory = tFactory;
  myTree->read_newick_file(myTreeFile);

  RootedTree *bestRoot = rootFromSamples(myTree, n_smpl);    
  RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
  
  ofstream fout;
  fout.open(outputTree);  
  rerooted->write_newick(fout);
  fout.close();    

  return 0;
}
