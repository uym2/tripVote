#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"

void HDT::handleCTransform()
{
	// Triplets (sum, n_i_circ has been set to 0, i.e. n_circ_square is also 0!)
	n_circ_square = 0;

    for (int i = 0; i < this->degree; i++){
        countingVars->set_n_i_circ(i,0);
        // n_i is unchanged by this transform!
        countingVars->set_n_parent_ii(i,Util::binom2(countingVars->get_n_i(i)));
    }
}
