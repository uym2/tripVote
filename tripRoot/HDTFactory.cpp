#include "hdt_factory.h"
#include "rooted_tree.h"
#include "counting_linked_list.h"

CountingLinkedList CountingLinkedList::dummyLL = CountingLinkedList(true);

#define HDTFactorySize 30

HDTFactory::HDTFactory(int numD, HDTFactory *copyMemAllocFrom)
{
	this->numD = numD;

	if (copyMemAllocFrom == NULL)
	{
		memHDT = new MemoryAllocator<HDT>(HDTFactorySize+1);
		memCLL = new MemoryAllocator<CountingLinkedList>(HDTFactorySize+1); // obsolete; to be deleted
		memCLLNO = new MemoryAllocator<CountingLinkedListNumOnly>(HDTFactorySize+1); // obsolete; to be deleted
        memARR = new MemoryAllocator<TemplatedLinkedList<CountingArray*> >(HDTFactorySize+1); // uym2 added
		memTLL = new MemoryAllocator<TemplatedLinkedList<HDT*> >(HDTFactorySize+1);
	}
	else
	{
		memHDT = copyMemAllocFrom->memHDT;
		memCLL = copyMemAllocFrom->memCLL; // obsolete; to be deleted
		memCLLNO = copyMemAllocFrom->memCLLNO; // obsolete; to be deleted
		memTLL = copyMemAllocFrom->memTLL;
        memARR = copyMemAllocFrom->memARR;
	}
	memHDT->numUses++;
	memCLL->numUses++; // obsolete; to be deleted
	memCLLNO->numUses++; // obsolete; to be deleted
	memTLL->numUses++;
    memARR->numUses++;

	createdHDTs = memHDT->getMemory();
	createdHDTs->left = NULL;
	currentHDT = createdHDTs;
	hdtLocation = 1;


    /* obsolete; to be deleted */
	createdLL = memCLL->getMemory();
	createdLL->initialize();
	currentLL = createdLL;
	llLocation = 1;

	createdLLNO = memCLLNO->getMemory();
	createdLLNO->initialize();
	currentLLNO = createdLLNO;
	llnoLocation = 1;
    /***************************/
	
    /*       uym2 added         */
    createdARR = memARR->getMemory();
    createdARR->initialize();
    currentARR = createdARR;
    arrLocation = 1;
    /***************************/
    
    
    createdTLL = memTLL->getMemory();
	createdTLL->initialize();
	currentTLL = createdTLL;
	currentLocationTLL = 1;
}

HDTFactory::~HDTFactory()
{
    {
		HDT *current = createdHDTs;
		while (current != NULL)
		{
			HDT *next = current->left;
			memHDT->releaseMemory(current);
			current = next;
		}
	}

    /* obsolete; to be deleted */
	{
		CountingLinkedList *current = createdLL;
		while (current != NULL)
		{
			CountingLinkedList *next = current->next;
			memCLL->releaseMemory(current);
			current = next;
		}
	}

	{
		CountingLinkedListNumOnly *current = createdLLNO;
		while (current != NULL)
		{
			CountingLinkedListNumOnly *next = current->next;
			memCLLNO->releaseMemory(current);
			current = next;
		}
	}
    /***************************/
	
    /*******  uym2 added *******/
    {
		TemplatedLinkedList<CountingArray*> *current = createdARR;
		while (current != NULL)
		{
			TemplatedLinkedList<CountingArray*> *next = current->next;
			memARR->releaseMemory(current);
			current = next;
		}
	}
    /***************************/

    {
		TemplatedLinkedList<HDT*> *current = createdTLL;
		while (current != NULL)
		{
			TemplatedLinkedList<HDT*> *next = current->next;
			memTLL->releaseMemory(current);
			current = next;
		}
	}
	
	memHDT->numUses--;
	if (memHDT->numUses == 0) delete memHDT;
	memCLL->numUses--;
	if (memCLL->numUses == 0) delete memCLL; // obsolete; to be deleted
	memCLLNO->numUses--;
	if (memCLLNO->numUses == 0) delete memCLLNO; // obsolete; to be deleted
	memTLL->numUses--;
	if (memTLL->numUses == 0) delete memTLL;
	memARR->numUses--;
	if (memARR->numUses == 0) delete memTLL;

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
	if (hdtLocation > HDTFactorySize)
	{
		currentHDT->left = memHDT->getMemory();;
		currentHDT = currentHDT->left;
		currentHDT->left = NULL;
		hdtLocation = 1;
	}

	HDT *returnMe = &currentHDT[hdtLocation];
	returnMe->initialize(getARR(), type, numD, link, doLink);
	returnMe->factory = this;
	hdtLocation++;
	return returnMe;
}

CountingLinkedList* HDTFactory::getLL()
{
	if (llLocation > HDTFactorySize)
	{
		currentLL->next = memCLL->getMemory();;
		currentLL = currentLL->next;
		currentLL->initialize();
		llLocation = 1;
	}

	CountingLinkedList *returnMe = &currentLL[llLocation];
	returnMe->initialize();
	llLocation++;
	return returnMe;
}

CountingLinkedListNumOnly* HDTFactory::getLLNO()
{
	if (llnoLocation > HDTFactorySize)
	{
		currentLLNO->next = memCLLNO->getMemory();
		currentLLNO = currentLLNO->next;
		currentLLNO->initialize();
		llnoLocation = 1;
	}

	CountingLinkedListNumOnly *returnMe = &currentLLNO[llnoLocation];
	returnMe->initialize();
	llnoLocation++;
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

CountingArray* HDTFactory::getARR()
{
	if (arrLocation > HDTFactorySize)
	{
		currentARR->next = memARR->getMemory();
		currentARR = currentARR->next;
		currentARR->initialize();
		arrLocation = 1;
	}

	TemplatedLinkedList<CountingArray*> *returnMe = &currentARR[arrLocation];
    returnMe->data = new CountingArray;
	returnMe->data->initialize(this->numD); 	
    arrLocation++;
	return returnMe->data;
}

long long HDTFactory::getSizeInRam()
{
	long long resultHDT = 0;
	{
		HDT *current = createdHDTs;
		while (current != NULL)
		{
			resultHDT++;
			current = current->left;
		}
	}

	long long resultLL = 0;
	{
		CountingLinkedList *current = createdLL;
		while (current != NULL)
		{
			resultLL++;
			current = current->next;
		}
	}

	long long resultLLNO = 0;
	{
		CountingLinkedListNumOnly *current = createdLLNO;
		while (current != NULL)
		{
			resultLLNO++;
			current = current->next;
		}
	}

	long long resultTLL = 0;
	{
		TemplatedLinkedList<HDT*> *current = createdTLL;
		while (current != NULL)
		{
			resultTLL++;
			current = current->next;
		}
	}
	
    long long resultARR = 0;
	{
		TemplatedLinkedList<CountingArray*> *current = createdARR;
		while (current != NULL)
		{
			resultARR++;
			current = current->next;
		}
	}

	return resultHDT * (HDTFactorySize+1) * sizeof(HDT) +
		resultLL * (HDTFactorySize+1) * sizeof(CountingLinkedList) +
		resultLLNO * (HDTFactorySize+1) * sizeof(CountingLinkedListNumOnly) +
		resultTLL * (HDTFactorySize+1) * sizeof(TemplatedLinkedList<HDT*>) +
		resultARR * (HDTFactorySize+1) * sizeof(TemplatedLinkedList<CountingArray*>);
}
