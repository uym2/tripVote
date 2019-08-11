#ifndef TRIPLET_COUNTER_H
#define TRIPLET_COUNTER_H

#include "int_stuff.h"
#include "counting_linked_list_num_only.h"
#include "hdt_factory.h"

class TripletCounter
{
    public:
        unsigned int N; // this is the number of nodes in the tree it represents
        int *tI, *tO, *tR, *tA; //triplets inside, outside, as the root, and accumulated of the inside
	
		TripletCounter(unsigned int N);
        ~TripletCounter();
};  

#endif
