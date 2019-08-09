#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"

void HDT::handleIGToC()
{
	// NOTE: We generate this with left being I and right being G!

	if (!right->up2date) right->updateCounters();
	// These are just inherited from the G node
	n_circ = right->n_circ;
	n_circ_square = right->n_circ_square;
    tripResolved_root = right->tripResolved_root;

    // copy tripResolved and tripUnresolved and countingVars from the G component
    countingArray *rCV = right->countingVars;
    
    for (int i = 0; i < this->degree; i++){
        tripResolved[i] = rCV->tripResolved[i];
        tripUnresolved[i] = rCV->tripUnresolved[i];
        countingVars->set_n_i(i, rCV->get_n_i(i));
        countingVars->set_n_i_circ(i, rCV->get_n_i_circ(i));
        countingVars->set_n_parent_ii(i, rCV->get_n_parent_ii(i));
        countingVars->set_n_i_arrow_circ(i, rCV->get_n_i_arrow_circ(i));
    }

    // copy n_ij from G component
    for (int j = 0; j < this->degree-1; j++) {
        for (int i = j + 1; i < this->degree; i++){
            countingVars->set_n_ij(i,j,rCV->get_n_ij(i,j));
        }
    }
    
    // n_i_arrow_j should have gotten the correct values from constructor of countingArray
}
