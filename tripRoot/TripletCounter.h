#ifndef TRIPLET_COUNTER_H
#define TRIPLET_COUNTER_H

#include "int_stuff.h"

class TripletCounter
{
    public:
        unsigned int N; // this is the number of nodes in the tree it represents
        INTTYPE_REST *tI, *tO, *tR, *tA; //triplets inside, outside, as the root, and accumulated of the inside
        bool *isActive; // mark if the node is active - the root can be placed above it - or not
        	
		TripletCounter(unsigned int N);
        ~TripletCounter();
};  

#endif
