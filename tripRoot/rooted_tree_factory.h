#ifndef ROOTED_TREE_FACTORY_H
#define ROOTED_TREE_FACTORY_H
#include "rooted_tree.h"
#include <vector>

class RootedTreeFactory
{
	private:
		vector<RootedTree*> createdRT;
		RootedTree *currentRT;

		vector<TemplatedLinkedList<RootedTree*>*> createdTLL;
		TemplatedLinkedList<RootedTree*> *currentTLL;

		int currentLocationRT, currentLocationTLL;

	public:
		RootedTreeFactory();
		~RootedTreeFactory();
		RootedTree* getRootedTree(string name = "");
		TemplatedLinkedList<RootedTree*>* getTemplatedLinkedList();
};

#endif
