#include "hdt_factory.h"
#include "rooted_tree.h"
#include "counting_linked_list.h"

CountingLinkedList CountingLinkedList::dummyLL = CountingLinkedList(true);

#define HDTFactorySize 30

HDTFactory::HDTFactory(int numD)
{
	this->numD = numD;

    memTLL = new MemoryAllocator<TemplatedLinkedList<HDT*> >(HDTFactorySize+1);
	memTLL->numUses++;

	currentHDT = NULL;
    
    createdTLL = memTLL->getMemory();
	createdTLL->initialize();
	currentTLL = createdTLL;
	currentLocationTLL = 1;
}

HDTFactory::~HDTFactory()
{
    for (vector<HDT*>::iterator it = createdHDTs.begin(); it != createdHDTs.end(); ++it) {
        delete *it;
    }
    
    TemplatedLinkedList<HDT*> *current = createdTLL;
    while (current != NULL)
    {
        TemplatedLinkedList<HDT*> *next = current->next;
        memTLL->releaseMemory(current);
        current = next;
    }
	
	memTLL->numUses--;
	if (memTLL->numUses == 0) delete memTLL;
}

void HDTFactory::deleteTemplatedLinkedList()
{
	TemplatedLinkedList<HDT*> *current = createdTLL;
	while (current != NULL)
	{
		TemplatedLinkedList<HDT*> *next = current->next;
		memTLL->releaseMemory(current);
		current = next;
	}
	createdTLL = currentTLL = NULL;
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
	if (currentLocationTLL > HDTFactorySize)
	{
		currentTLL->next = memTLL->getMemory();
		currentTLL = currentTLL->next;
		currentTLL->initialize();
		currentLocationTLL = 1;
	}

	TemplatedLinkedList<HDT*> *returnMe = &currentTLL[currentLocationTLL];
	returnMe->initialize();
	currentLocationTLL++;
	return returnMe;
}
