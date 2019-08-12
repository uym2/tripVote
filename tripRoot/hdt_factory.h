#ifndef HDT_FACTORY_H
#define HDT_FACTORY_H
#include "hdt.h"
#include "counting_array.h"

class RootedTree; // forward declaration
class CountingLinkedList; // forward declaration
class CountingLinkedListNumOnly; // forward declaration
class CountingArray; // forward declaration

class HDTFactory
{
	private:
		HDT *createdHDTs;
		HDT *currentHDT;
		TemplatedLinkedList<CountingArray*> *createdARR; // uym2 added
        TemplatedLinkedList<CountingArray*> *currentARR; // uym2 added
		TemplatedLinkedList<HDT*> *createdTLL; 
		TemplatedLinkedList<HDT*> *currentTLL;
        CountingLinkedList *createdLL; // obsolete; to be deleted
		CountingLinkedList *currentLL; // obsolete; to be deleted
		CountingLinkedListNumOnly *createdLLNO; // obsolete; to be deleted
		CountingLinkedListNumOnly *currentLLNO; // obsolete; to be deleted

		int hdtLocation;
        int llLocation, llnoLocation; // obsolete; to be deleted
        int arrLocation; // uym2 added
        int currentLocationTLL;
		//int numD;
		
        MemoryAllocator<HDT> *memHDT;
		MemoryAllocator<CountingLinkedList> *memCLL; // obsolete; to be deleted
		MemoryAllocator<CountingLinkedListNumOnly> *memCLLNO; // obsolete; to be deleted
		MemoryAllocator<TemplatedLinkedList<CountingArray*> > *memARR; // uym2 added
		MemoryAllocator<TemplatedLinkedList<HDT*> > *memTLL;

	public:
		int numD;
		HDTFactory(int numD, HDTFactory *copyMemAllocFrom = NULL);
		~HDTFactory();
		HDT* getHDT(HDT::NodeType type, RootedTree *link, bool doLink);
		CountingLinkedList* getLL(); // obsolete; to be deleted
		CountingLinkedListNumOnly* getLLNO(); // obsolete; to be deleted
        CountingArray* getARR(); // uym2 added
		TemplatedLinkedList<HDT*>* getTemplatedLinkedList();
		void deleteTemplatedLinkedList();
		long long getSizeInRam();
};

#endif
