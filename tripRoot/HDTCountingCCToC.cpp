#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_array.h"

void HDT::handleCCToC()
{
	// Recurse (if not I child) (we really _should_ have both children!)
	if (!left->up2date) left->updateCounters();
	if (!right->up2date) right->updateCounters();

    // reset counters for this node
    this->resetCounters();

	// NOTE: We generate this with c2 (the higher one) being the left child and c1 (the lower one) being the right child
	HDT *c1 = this->right;
	HDT *c2 = this->left;

	// n_i is just the sum of the 2 children, thus n_circ is also just the sum of the 2 children
	n_circ = c1->n_circ + c2->n_circ;

    // n_parent_zero_circ 
    n_parent_zero_circ = c1->n_parent_zero_circ + c2->n_parent_zero_circ;

    //n_parent_circ_square
    n_parent_circ_square = c1->n_parent_circ_square + c2->n_parent_circ_square;

    // compute n_i,n_parent_ii, n_i_circ, n_i_arrow_circ, and n_circ_square
    n_circ_square = 0; // Initialize sum to 0 so we can increment it in the loop
    CountingArray *ourCount = &this->countingVars;
    CountingArray *c1Count = &c1->countingVars;
    CountingArray *c2Count = &c2->countingVars;

    for (int i = 0; i < this->degree; i++){
        ourCount->set_n_i(i,c1Count->get_n_i(i) + c2Count->get_n_i(i));
        ourCount->set_n_parent_ii(i,c1Count->get_n_parent_ii(i) + c2Count->get_n_parent_ii(i));
        ourCount->set_n_i_circ(i,c1Count->get_n_i_circ(i) + c2Count->get_n_i_circ(i));
		ourCount->set_n_i_arrow_circ(i, c1Count->get_n_i_arrow_circ(i) + c2Count->get_n_i_arrow_circ(i) +
                            			c1Count->get_n_i(i) * (c2->n_circ - c2Count->get_n_i(i)));
        n_circ_square += ourCount->get_n_i_circ(i);
    }

    n_circ_square /= 2;


    // compute n_i_j
    //for (int j = 0; j < this->degree-1; j++){
    //    for (int i = j+1; i < this->degree; i++){
    for (int i = 1; i < this->degree; i++){
        for (int j = 0; j < i; j++) { 
            ourCount->set_n_ij(i,j, c1Count->get_n_ij(i,j) + c2Count->get_n_ij(i,j));
        }
    }

    // compute n_i_arrow_j
    for (unsigned int i = 0; i < this->degree; i++){
        for (unsigned int j = 0; j < this->degree; j++){
            if (i != j){
                ourCount->set_n_arrow_ij(i,j, c1Count->get_n_arrow_ij(i,j) + c2Count->get_n_arrow_ij(i,j) +
                                              c1Count->get_n_i(i) * c2Count->get_n_i(j));
            }
        }
    }

    /* Computing tripResolved, tripUnresolved and tripResolved_root */

	// Fetch the childrens "result counting data"
    tripResolved_root = c1->tripResolved_root + c2->tripResolved_root + c1Count->get_n_i(0) * c2Count->get_n_i_arrow_circ(0); 

    for (int i = 0; i < this->degree; i++){
        tripResolved[i] = c1->tripResolved[i] + c2->tripResolved[i];
	    tripUnresolved[i] = c1->tripUnresolved[i] + c2->tripUnresolved[i];
        if (i != 0)
            tripResolved_root += (c1->n_circ - c1Count->get_n_i(0)) * c2Count->get_n_arrow_ij(i,0);
    }
    
    tripResolved_root += Util::binom2(c1Count->get_n_i(0)) * (c2->n_circ - c2Count->get_n_i(0)) + Util::binom2(c1->n_circ - c1Count->get_n_i(0)) * c2Count->get_n_i(0);
    tripResolved_root += (c1->n_circ - c1Count->get_n_i(0)) * c2Count->get_n_parent_ii(0) + c1Count->get_n_i(0) * (c2->n_parent_circ_square - c2->n_parent_zero_circ);

    
    for (int j = 0; j < this->degree; j++){
        for (int i = 0; i < this->degree; i++){
            if (i != j){ // leave j out
                tripResolved[j] += c1Count->get_n_i(i) * ( c2Count->get_n_i_arrow_circ(i) - c2Count->get_n_arrow_ij(i,j) );
                tripResolved[j] += Util::binom2(c1Count->get_n_i(i)) * ( c2->n_circ - c2Count->get_n_i(j) - c2Count->get_n_i(i) );
                tripResolved[j] += ( c1->n_circ - c1Count->get_n_i(j) - c1Count->get_n_i(i) ) * c2Count->get_n_parent_ii(i);
                tripUnresolved[j] += c1Count->get_n_i(i) * ( c2->n_circ_square - c2Count->get_n_i_circ(j) - c2Count->get_n_i_circ(i) + c2Count->get_n_ij(i,j) );
            }
        }
    }
    
}
