#ifndef COUNTING_LINKED_LIST_H
#define COUNTING_LINKED_LIST_H

#include "int_stuff.h"
#include "counting_linked_list_num_only.h"
#include "hdt_factory.h"

class CountingLinkedList
{
	public:
		// For triplets
		INTTYPE_REST n_i, n_i_circ, n_paren_ii, n_i_arrow_circ;

		// More general stuff
		unsigned int num;
		enum NodeType {Regular, End, Dummy};
		NodeType type;
		CountingLinkedList *next, *iterator;
		static CountingLinkedList dummyLL;
		bool n_i_j_is_reset, n_j_arrow_i_is_reset, n_i_arrow_j_is_reset;

		CountingLinkedList()
		{
		}

		void initialize()
		{
			next = NULL;
		}

		CountingLinkedList(bool dummy)
		{
			initialize();

			// Triplets
			n_i = 0;
			n_i_circ = 0;
			n_paren_ii = 0;
			n_i_arrow_circ = 0;

		}
};

#endif
