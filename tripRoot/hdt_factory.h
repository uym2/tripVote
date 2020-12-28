#ifndef HDT_FACTORY_H
#define HDT_FACTORY_H
#include "hdt.h"
#include "counting_array.h"
#include <vector>

class RootedTree; // forward declaration
class CountingArray; // forward declaration

using namespace std;

class HDTFactory
{
	private:
		vector<HDT*> createdHDTs;
		HDT* currentHDT;
		vector<TemplatedLinkedList<HDT*>* > createdTLL; 
		TemplatedLinkedList<HDT*> *currentTLL;

		int hdtLocation;
        int currentLocationTLL;

	public:
		int numD;
		HDTFactory(int numD);
		~HDTFactory();
		HDT* getHDT(HDT::NodeType type, RootedTree *link, bool doLink);
		TemplatedLinkedList<HDT*>* getTemplatedLinkedList();
		void deleteTemplatedLinkedList();
};

#endif
