#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletRooting.h"
#include "newick_parser.h"
#include "MVRooting.h"
#include <cmath>

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

RootedTree* copyTree(RootedTree *myTree){
    string treeStr = myTree->toString();
    RootedTreeFactory *factory = new RootedTreeFactory();
    RootedTree* newTree = factory->getRootedTree();
    newTree->factory = factory;
    newTree->read_newick_str(treeStr);
    return newTree;
}

void rootFromVotes(RootedTree *&myTree, char *refTreeFile){
    myTree->countChildren();
    myTree->count_nodes();
    myTree->set_all_idx(0);
    
    MVRooting mvRoot;
    mvRoot.initialize(myTree);
    myTree = mvRoot.root_tree();
    double var0 = mvRoot.optimalVarScore;
    double mean0 = myTree->mean_d2root();
    double CV0 = sqrt(var0)/mean0;

    cout << "CV0 = " << CV0 << endl;

    INTTYPE_REST *allCounts = NULL;
    unsigned int N = 0;
    
    myTree = copyTree(myTree);
    myTree->countChildren();
    myTree->set_all_idx(0);
    myTree->count_nodes();
    
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
        RootedTree *tree = rFactory->getRootedTree();
        tree->factory = rFactory;
        tree->read_newick_str(treeStr); 
        tree->countChildren();
        tree->count_nodes();
        tree->set_all_idx(0);        
        
        MVRooting mvRoot1;
        mvRoot1.initialize(tree);
        RootedTree *refTree = mvRoot1.root_tree(); 
        double var1 = mvRoot1.optimalVarScore;
        double mean1 = refTree->mean_d2root();
        double CV1 = sqrt(var1)/mean1;
        //double weight = log(CV0/CV1);
        double weight = 1;

        cout << "Weight: " << weight << endl;
        
        TripletRooting tripRoot;
        tripRoot.initialize(refTree,myTree);
        tripRoot.find_optimal_root();
        TripletCounter* oneCount = tripRoot.tripCount;
        
        //for (int i = 0; i<oneCount->N; i++)
        //    cout << oneCount->tripScore[i] << " ";
        //cout << endl;

        // add oneCount to allCounts
        if (allCounts != NULL){
            for (int i=0; i<N; i++)
                allCounts[i] += weight*oneCount->tripScore[i];
        } else {
            N = oneCount->N;
            allCounts = new INTTYPE_REST[N];
            for (int i=0; i<N; i++){
                allCounts[i] = weight*oneCount->tripScore[i];
            }
        }
        cout << "Finished one tree" << endl;
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
    //MVRooting mvRoot;
    mvRoot.initialize(myTree);
    mvRoot.compute_score();
    unsigned int Ambiguity = 0;
    if (best_node_idx == 0)
        Ambiguity = -2;
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
    //cout << "Optimal root index: " << best_node_idx << endl;

    cout << "MV score at MV root: " << mvRoot.mvCount->minVar[myTree->idx] << endl;
    cout << "MV score at best voted root: " << mvRoot.mvCount->minVar[best_node_idx] << endl;

    cout << "Vote score at MV root: " << allCounts[myTree->idx] << endl;
    cout << "Vote score at best voted root: " << allCounts[best_node_idx] << endl;

    RootedTree* bestRoot = myTree->search_idx(best_node_idx);
    myTree = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
    cout << myTree->toString() << endl;

    //return myTree->search_idx(best_node_idx);
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

  rootFromVotes(myTree, refTreeFile);    
  cout << "Found best root!" << endl;
  //RootedTree *rerooted = myTree->reroot_at_edge(bestRoot,bestRoot->edge_length/2);
  
  ofstream fout;
  fout.open(outputTree);  
  myTree->write_newick(fout);
  fout.close();    

  return 0;
}
