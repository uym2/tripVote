#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"


void HDT::handleLeaf()
{
	// This is a leaf!
    
    // Reset counters
    this->resetCounters();

	if (link == NULL)
	{
		countingVars.set_n_i(0,numZeroes);
	}
	else
	{
        countingVars.set_n_i(this->link->color, 1);
	}
	
    // Sums and stuff (triplets)
    n_circ = 1;
}
