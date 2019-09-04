#ifndef TRIPLET_COUNTER_H
#define TRIPLET_COUNTER_H

#include "int_stuff.h"

class MinVarCounter
{
    public:
        unsigned int N; // this is the number of nodes in the tree it represents
        float *minVar; // the minimum root-to-tip variance if the tree is rerooted somewhere above that node
        float *xStar; // the position on the edge where the tree should be rerooted to minimize variance
		
        MinVarCounter(unsigned int N);
        ~MinVarCounter();
};  

#endif
