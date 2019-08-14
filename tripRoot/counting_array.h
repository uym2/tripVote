#ifndef COUNTING_ARRAY_H
#define COUNTING_ARRAY_H

#include "int_stuff.h"

class CountingArray
{
    private:
        unsigned int degree;
        INTTYPE_REST *n_i, *n_i_circ, *n_parent_ii, *n_i_arrow_circ;
        INTTYPE_REST **n_i_j, **n_i_arrow_j;

	public:
		void initialize(unsigned int degree);
        CountingArray();
        ~CountingArray();
        INTTYPE_REST get_n_ij(unsigned int i, unsigned int j);
        INTTYPE_REST get_n_arrow_ij(unsigned int i, unsigned int j);
        INTTYPE_REST get_n_i(unsigned int i);
        INTTYPE_REST get_n_i_circ(unsigned int i);
        INTTYPE_REST get_n_parent_ii(unsigned int i);
        INTTYPE_REST get_n_i_arrow_circ(unsigned int i);
        bool set_n_ij(unsigned int i, unsigned int j, INTTYPE_REST v);
        bool set_n_arrow_ij(unsigned int i, unsigned int j, INTTYPE_REST v);
        bool set_n_i(unsigned int i, INTTYPE_REST v);
        bool set_n_i_circ(unsigned int i, INTTYPE_REST v);
        bool set_n_parent_ii(unsigned int i, INTTYPE_REST v);
        bool set_n_i_arrow_circ(unsigned int i, INTTYPE_REST v);
};  

#endif
