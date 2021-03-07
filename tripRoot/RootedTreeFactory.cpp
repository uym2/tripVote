#include "rooted_tree_factory.h"
#include "rooted_tree.h"

RootedTreeFactory::RootedTreeFactory()
{
    currentRT = NULL;
    currentTLL = NULL;
}

RootedTreeFactory::~RootedTreeFactory()
{   
    for (vector<RootedTree*>::iterator it = createdRT.begin(); it != createdRT.end(); ++it) {
        delete *it;
        *it = NULL;
    }
    
    for (vector<TemplatedLinkedList<RootedTree*>* >::iterator it = createdTLL.begin(); it != createdTLL.end(); ++it) {
        delete *it;
        *it = NULL;
    }
}

RootedTree* RootedTreeFactory::getRootedTree(string name)
{
    RootedTree *returnMe = new RootedTree;
	returnMe->initialize(name);
	returnMe->factory = this;
    this->currentRT = returnMe;
	this->createdRT.push_back(returnMe);
    return returnMe;
}

TemplatedLinkedList<RootedTree*>* RootedTreeFactory::getTemplatedLinkedList()
{
	TemplatedLinkedList<RootedTree*> *returnMe =  new TemplatedLinkedList<RootedTree*>; 
	returnMe->initialize();
	this->currentTLL = returnMe;
    this->createdTLL.push_back(returnMe);
	return returnMe;
}
