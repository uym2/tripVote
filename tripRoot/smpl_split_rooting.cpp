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

RootedTree* rootFromSplitVotes(RootedTree *myTree){
    myTree->set_all_idx(0);
    myTree->count_nodes();
    
    ofstream fout;
    fout.open("smpl_trees.txt");

    double *allCounts = NULL;
    SubtreeSampler sampler(myTree);
    vector<string> my_subtrees = sampler.sample_by_splits();
    cout << "Number of subtrees: " << my_subtrees.size() << endl;
    
    unsigned int N = 0;
    
    for (int i=0; i < my_subtrees.size(); i++){
        cout << "Processing sample " << i+1 << endl;

        string treeStr = my_subtrees[i];  
        
        RootedTreeFactory *tFactory = new RootedTreeFactory();
        RootedTree *tree = tFactory->getRootedTree();
        tree->factory = tFactory;
        tree->read_newick_str(treeStr);

        tree->countChildren();
        cout << "Tree size: " << tree->n << endl;
        if (tree->n < 3){
            continue;
        }

        double norm = tree->n * (tree->n-1) * (tree->n-2) / 6;;
        
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
                allCounts[i] += oneCount->tripScore[i]/norm;
        } else {
            N = oneCount->N;
            allCounts = new double[N];
            for (int i=0; i<N; i++){
                allCounts[i] = oneCount->tripScore[i]/norm;
            }
        }
    }
    fout.close();

    unsigned int best_node_idx = -1;    
    double best_vote = -1;

    for (int i=0; i<N; i++){
        cout << allCounts[i] << endl;
        if (allCounts[i] > best_vote){
            best_vote = allCounts[i];
            best_node_idx = i;
        }
    }
    
    unsigned int Ambiguity = 0;
    for (int i = 0; i<N; i++){
        if (allCounts[i] == best_vote){
            cout << i << endl;
            Ambiguity++;
        }
    }

    cout << "Best vote score: " << best_vote << endl;
    cout << "Ambiguity " << Ambiguity << endl;       
    cout << "Optimal root index: " << best_node_idx << endl;

    return myTree->search_idx(best_node_idx);
}

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <inTree> <outTree>" << std::endl 
	    << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    usage(argv[0]);
    return 0;
  }

  char *myTreeFile = argv[1];
  char *outputTree = argv[2];
  
  NewickParser parser;


  RootedTreeFactory *tFactory = new RootedTreeFactory();
  RootedTree *myTree = tFactory->getRootedTree();
  myTree->factory = tFactory;
  myTree->read_newick_file(myTreeFile);

  cout << "Read input tree!" << endl;
    
  RootedTree *bestRoot = rootFromSplitVotes(myTree);    
  RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
  
  ofstream fout;
  fout.open(outputTree);  
  rerooted->write_newick(fout);
  fout.close();    

  return 0;
}
