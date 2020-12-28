#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"

void HDT::handleCTransform()
{    
	// Triplets (sum, n_i_circ has been set to 0, i.e. n_circ_square is also 0!; n_circ is unchanged)
	n_circ_square = 0;
    n_parent_zero_circ = countingVars.get_n_i(0) * (n_circ - countingVars.get_n_i(0));
    n_parent_circ_square = 0; //n_circ*n_circ; // will update in the loop 

    for (int i = 0; i < this->degree; i++){
        // n_parent_circ_square
        INTTYPE_REST n_i = countingVars.get_n_i(i);
        n_parent_circ_square += n_i*(n_circ - n_i);
        // n_i_circ
        countingVars.set_n_i_circ(i,0);
        // n_parent_ii
        countingVars.set_n_parent_ii(i,Util::binom2(n_i));
        // n_i is unchanged by this transform!
        // n_i_arrow_circ is not defined for G
        countingVars.set_n_i_arrow_circ(i,0);
    }

    n_parent_circ_square /= 2;

    // n_i_j
    for (int i = 1; i < degree; i++){
        for (int j = 0; j < i; j++){
            countingVars.set_n_ij(i,j,0);
        }
    }

    // n_i_arrow_j are not defined for G
    for (int i = 0; i < degree; i++){
        for (int j = 0; j < degree; j++){
            if (j != i)
                countingVars.set_n_arrow_ij(i,j,0);
        }
    }
}
