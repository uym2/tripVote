#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"


void HDT::handleLeaf()
{
	// This is a leaf!
	// Triplets
	if (link == NULL)
	{
		countingVars->set_n_i(0,numZeros);
	}
	else
	{
        countingVars->set_n_i(this->link->color, 1);
	}
	// Sums and stuff (triplets)
    n_circ = 1;
	n_circ_square = 0;
    n_parent_zero_circ = 0;
    n_parent_circ_square = 0;
}
