#ifndef MV_COUNTER_H
#define MV_COUNTER_H

#include "int_stuff.h"

class MinVarCounter
{
    public:
        unsigned int N; // this is the number of nodes in the tree it represents
        double *var;    // the root-to-tip variance if the tree is rerooted at this node
        double *minVar; // the minimum root-to-tip variance if the tree is rerooted somewhere above that node
        double *xStar; // the position on the edge where the tree should be rerooted to minimize variance
	    double *SI;
        double *ST;
    	
        MinVarCounter(unsigned int N);
        ~MinVarCounter();
};  

#endif
