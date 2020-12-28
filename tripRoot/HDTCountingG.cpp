#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_linked_list.h"

void HDT::handleG()
{
	// Not a leaf, i.e. a GG->G
	HDT *g1 = this->left;
	HDT *g2 = this->right;
	
	if (!g1->up2date) g1->updateCounters();
	if (!g2->up2date) g2->updateCounters();

	// reset counters
    this->resetCounters();
    
    // n_i is just the sum of the 2 children, thus n_circ is also just the sum of the 2 children
	n_circ = g1->n_circ + g2->n_circ;

    // n_parent_zero_circ
    n_parent_zero_circ = g1->n_parent_zero_circ + g2->n_parent_zero_circ;

    // n_parent_circ_square
    n_parent_circ_square = g1->n_parent_circ_square + g2->n_parent_circ_square;

    
    // compute n_i, n_i_circ, n_parent_ii, and n_circ_square
	n_circ_square = 0; // Initialize sum to 0 so we can increment it in the loop
    CountingArray *ourCount = &this->countingVars;
    CountingArray *g1Count = &g1->countingVars;
    CountingArray *g2Count = &g2->countingVars;

    for (int i = 0; i < this->degree; i++){
        ourCount->set_n_i(i,g1Count->get_n_i(i) + g2Count->get_n_i(i));
        ourCount->set_n_parent_ii(i,g1Count->get_n_parent_ii(i) + g2Count->get_n_parent_ii(i));
        ourCount->set_n_i_circ(i, g1Count->get_n_i_circ(i) + g2Count->get_n_i_circ(i) +
                                    g1Count->get_n_i(i) * (g2->n_circ - g2Count->get_n_i(i)) + 
                                    g2Count->get_n_i(i) * (g1->n_circ - g1Count->get_n_i(i)));
        n_circ_square += ourCount->get_n_i_circ(i);
    }


	n_circ_square /= 2;

    // compute n_i_j
    //for (int j = 0; j < this->degree-1; j++){
    //    for (int i = j+1; i < this->degree; i++){
    for (int i = 1; i < this->degree; i++){
        for (int j = 0; j < i; j++){
            ourCount->set_n_ij(i,j, g1Count->get_n_ij(i,j) + g2Count->get_n_ij(i,j) +
                                    g1Count->get_n_i(i) * g2Count->get_n_i(j) + 
                                    g2Count->get_n_i(i) * g1Count->get_n_i(j));
        }
    }

    // Note: n_i_arrow_j is not defined for G

    
    /* Computing tripResolved, tripUnresolved and tripResolved_root */

	// Fetch the childrens "result counting data"
	for (int i = 0; i < this->degree; i++){
        tripResolved[i] = g1->tripResolved[i] + g2->tripResolved[i];
	    tripUnresolved[i] = g1->tripUnresolved[i] + g2->tripUnresolved[i];
    }

    
    for (int j = 0; j < this->degree; j++){
        for (int i = 0; i < this->degree; i++){
            if (i != j){ // leave j out
                tripResolved[j] += g1Count->get_n_parent_ii(i) * (g2->n_circ - g2Count->get_n_i(j) - g2Count->get_n_i(i)); 
                tripResolved[j] += g2Count->get_n_parent_ii(i) * (g1->n_circ - g1Count->get_n_i(j) - g1Count->get_n_i(i)); 
    
                tripUnresolved[j] += g1Count->get_n_i(i) * ( g2->n_circ_square - g2Count->get_n_i_circ(j) - g2Count->get_n_i_circ(i)  + g2Count->get_n_ij(i,j) );
                tripUnresolved[j] += g2Count->get_n_i(i) * ( g1->n_circ_square - g1Count->get_n_i_circ(j) - g1Count->get_n_i_circ(i)  + g1Count->get_n_ij(i,j) );
            }
        }
    }
    
    tripResolved_root = g1->tripResolved_root + g2->tripResolved_root;
    tripResolved_root += g1Count->get_n_parent_ii(0) * (g2->n_circ - g2Count->get_n_i(0)) + (g1->n_parent_circ_square - g1->n_parent_zero_circ) * g2Count->get_n_i(0);
    tripResolved_root += g2Count->get_n_parent_ii(0) * (g1->n_circ - g1Count->get_n_i(0)) + (g2->n_parent_circ_square - g2->n_parent_zero_circ) * g1Count->get_n_i(0);
}
