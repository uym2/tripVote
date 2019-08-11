#ifndef ROOTED_TREE_LB_H
#define ROOTED_TREE_LB_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "int_stuff.h"
#include "templated_linked_list.h"
#include "rooted_tree.h"

class HDT;// Forward declaration...
class RootedTreeFactory; // Forward declaration...

using namespace std;

class RootedTreeLb : private RootedTree {
	private:
		unsigned int label;
    public:
        RootedTreeLb(RootedTreeLb *t);
        RootedTreeLb(RootedTree *t);
        int set_label (unsigned int lb) { this->label = lb; return 0; };
        int set_all_labels (unsigned int startIdx);
        unsigned int get_label() { return this->label; }   
        RootedTreeLb* down_root(RootedTreeLb *u); 
};

#endif
