#ifndef TRIPLET_COUNTER_H
#define TRIPLET_COUNTER_H

#include "int_stuff.h"

class TripletCounter
{
    public:
        unsigned int N; // this is the number of nodes in the tree it represents
        INTTYPE_REST *tI, *tO, *tR; //triplets inside, outside and as the root
        INTTYPE_REST *tripScore; // triplet score of the tree if it is rooted at this node
        bool *isActive; // mark if the node is active - the root can be placed above it - or not
        void erase();
        	
		TripletCounter(unsigned int N);
        ~TripletCounter();
};  

#endif
