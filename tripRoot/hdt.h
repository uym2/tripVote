#ifndef HDT_H
#define HDT_H

#include <iostream>
#include <string>
#include "int_stuff.h"
#include "util.h"
#include "rooted_tree_factory.h"
#include "counting_array.h"
class RootedTree; // forward declaration
class HDTFactory; // forward declaration
class CountingLinkedList; // forward declaration
class CountingLinkedListNumOnly; // forward declaration

#define INITIALIZE_PAREN_AND_SET_LIST(N_NAME, RESET_NAME) {	\
	if (parent->N_NAME == NULL)				\
	{							\
		parent->N_NAME = factory->getLLNO();		\
		parent->N_NAME->resetIterator();		\
		isReset = true;					\
	}							\
	else isReset = parent->RESET_NAME;			\
	theList = parent->N_NAME;				\
	parent->RESET_NAME = false;				\
}

#define NEXT_LEAST_J(LIST) {					\
    if								\
	(							\
		gotoIteratorValueForNumList(LIST, lastJPlus1) &&\
		LIST->getIteratorNum() < j			\
	)							\
		j = LIST->getIteratorNum();			\
}

using namespace std;

class HDT
{
	public:
		enum NodeType {I, C, G, NotConverted};
		void initialize(NodeType type, int numD, RootedTree *link = NULL, bool doLink = true);
		static HDT* constructHDT(RootedTree *t, int numD, HDTFactory *factory, bool doLink = true);

		void forceLinks();
		void toDot();
		void mark();
		void markAlternative();
		INTTYPE_REST leafCount();
		//RootedTree* extractAndGoBack(RootedTreeFactory *rtfactory);
		void resetCounters();
        void updateCounters();
		INTTYPE_REST getResolvedTriplets(unsigned int color);
		INTTYPE_REST getUnresolvedTriplets(unsigned int color);
        INTTYPE_REST getResolvedTriplets_root();
        ~HDT();

		HDT *left, *right;
		HDTFactory *factory;

		bool altMarked;

		// Used for extract+contract to work
		INTTYPE_REST numZeroes;


	private:
		RootedTree *goBackVariable;
		NodeType type, convertedFrom;
		RootedTree *link;
		HDT *parent, *childParent;
		TemplatedLinkedList<HDT*> *children;
		int degree;

		// Soda13 color 0+1+...+d
		CountingArray countingVars;
		INTTYPE_REST n_circ;
		INTTYPE_REST n_circ_square;
        INTTYPE_REST n_parent_circ_square;
        INTTYPE_REST n_parent_zero_circ;

		// Summing resolved/resolved and unresolved/unresolved
		INTTYPE_REST *tripResolved;
		INTTYPE_REST *tripUnresolved;
        INTTYPE_REST tripResolved_root; // the number of shared tripResolved if the other tree is to be rooted above the clade with this color


		// Marking stuff as changed or updated
		bool up2date;

		static HDT* preFirstRound(RootedTree *t, int numD, bool doLink, HDTFactory *factory);
		HDT* round(HDTFactory *factory);
		inline bool isDownwardsClosed();
		void toDotImpl();
		void handleLeaf();
		void handleCCToC();
		void handleIGToC();
		void handleCTransform();
		void handleG();
		
};

#endif
