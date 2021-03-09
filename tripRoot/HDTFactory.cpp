#include "hdt_factory.h"
#include "rooted_tree.h"
#include "counting_linked_list.h"

CountingLinkedList CountingLinkedList::dummyLL = CountingLinkedList(true);

#define HDTFactorySize 30

HDTFactory::HDTFactory(int numD)
{
	this->numD = numD;

	currentHDT = NULL;
    currentTLL = NULL;    
}

HDTFactory::~HDTFactory()
{
    for (vector<HDT*>::iterator it = createdHDTs.begin(); it != createdHDTs.end(); ++it) {
        if (*it) {
            delete *it;
            *it = NULL;
        }
    }
    
    for (vector<TemplatedLinkedList<HDT*>* >::iterator it = createdTLL.begin(); it != createdTLL.end(); ++it) {
        if (*it){
            delete *it;
            *it = NULL;
        }
    }
}

void HDTFactory::deleteTemplatedLinkedList()
{
    //std::cout << "HDTFactory: deleteTemplatedLinkedList" << std::endl;
    for (vector< TemplatedLinkedList<HDT*>* >::iterator it = createdTLL.begin(); it != createdTLL.end(); ++it) {
        delete *it;
        *it = NULL;
    }
}

HDT* HDTFactory::getHDT(HDT::NodeType type, RootedTree *link, bool doLink)
{
    HDT *returnMe = new HDT;
	returnMe->initialize(type, numD, link, doLink);
	returnMe->factory = this;

    this->currentHDT = returnMe;
    this->createdHDTs.push_back(returnMe);
	return returnMe;
}

TemplatedLinkedList<HDT*>* HDTFactory::getTemplatedLinkedList()
{
	TemplatedLinkedList<HDT*> *returnMe = new TemplatedLinkedList<HDT*>;
	returnMe->initialize();

    this->currentTLL = returnMe;
    this->createdTLL.push_back(returnMe);

	return returnMe;
}
