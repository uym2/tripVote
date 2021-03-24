#ifndef ROOTED_TREE_H
#define ROOTED_TREE_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "templated_linked_list.h"
#include "TripletCounter.h"
#include <stack>

class HDT;// Forward declaration...
class RootedTreeFactory; // Forward declaration...

using namespace std;

class RootedTree
{
	public:
        unsigned int idx; // added by uym2
		unsigned int level;
		RootedTree *parent;
        double edge_length;
        double d2root;

		TemplatedLinkedList<RootedTree*> *children;
		string name;
		unsigned int numChildren;
		int maxDegree;
		RootedTree *altWorldSelf;
		HDT *hdtLink;
		int color;
		INTTYPE_REST numZeroes;

		RootedTreeFactory *factory;

		// How many leaves beneath you?
		int n;
        // How many nodes beneath you?
        unsigned int nodeCounts;
        
		void initialize(string name="");
		bool isLeaf();
		bool isRoot();
        void addChild(RootedTree *t);
		RootedTree* getParent();
		INTTYPE_REST getUnresolvedTriplets();
		INTTYPE_N4 getUnresolvedQuartets();
		void toDot();
		vector<RootedTree*>* getList();
		bool pairAltWorld(RootedTree *t, bool do_pruning=true, TripletCounter *tripCount=NULL);
		void colorSubtree(int c);
		void markHDTAlternative();
		bool isError();
		RootedTree *contract(RootedTreeFactory *factory = NULL);


        // uym2 added
        bool prune_subtree(RootedTree* leaf);
        void mark_active(TripletCounter *tripCount);
        bool read_newick_file(string treeFile);           
        bool read_newick_str(string str);
        RootedTree* search_idx(unsigned int idx);
        RootedTree* search_name(string name);
        int set_idx (unsigned int idx) { this->idx = idx; return 0; };
        unsigned int set_all_idx (unsigned int startIdx);
        RootedTree* down_root(RootedTree *u);
        void print_leaves(); 
        RootedTree* copyTree(RootedTreeFactory *factory = NULL);
        RootedTree* reroot_at_edge(RootedTree *node, double x);
        void write_newick(ofstream &fout);
        string toString();
        bool remove_child(RootedTree *child);
        void countChildren();
        void count_nodes();
        unsigned int count_leaves();
        void compute_d2root();
        vector<RootedTree*> sort_leaf_by_d2root();
        double mean_d2root(); 

    private:
		bool error;
        void __set_label__(stack<RootedTree*> &stk, string &label, bool &wait_for_int_lab);
		void toDotImpl();
		void getListImpl(vector<RootedTree*>* list);
		void computeNullChildrenData();
		RootedTree* contractImpl(RootedTreeFactory *factory);
        void __write_newick__(ofstream &fout);
        void __count_children__(RootedTree *t);
        void __to_string__(string &myString);
};

#endif
