#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_linked_list.h"

void HDT::resetCounters(){
    countingVars.initialize(this->degree);
    
    n_circ = 0;
    n_circ_square = 0;
    n_parent_circ_square = 0;
    n_parent_zero_circ = 0;
    
    for (int i = 0; i < degree; i++){
        tripResolved[i] = 0;
        tripUnresolved[i] = 0;
    }

    tripResolved_root = 0;                    
}

void HDT::initialize(NodeType type, int numD, RootedTree *link, bool doLink)
{
	parent = childParent = left = right = NULL;
	children = NULL;
	convertedFrom = NotConverted;
	goBackVariable = NULL;

    tripResolved = new INTTYPE_REST[numD];
    tripUnresolved = new INTTYPE_REST[numD];

    for (int i = 0; i < numD; i++){
        tripResolved[i] = 0;
        tripUnresolved[i] = 0;
    }


    tripResolved_root = 0;
    n_circ = 0;
    n_circ_square = 0;
    n_parent_circ_square = 0;
    n_parent_zero_circ = 0;

	up2date = altMarked = false;

	this->type = type;
	this->link = link;
	if (link != NULL && doLink)
	{
		link->hdtLink = this;
	}
	this->degree = numD;
	this->countingVars.initialize(numD);
}

HDT::~HDT(){
    delete [] tripResolved;
    delete [] tripUnresolved;
}

INTTYPE_REST HDT::leafCount()
{
	//if (countingVars->num == 0) return n_circ + countingVars->n_i;
	return n_circ;
}

void HDT::mark()
{
	up2date = false;

	if (parent == NULL || !parent->up2date) return;
	parent->mark();
}

void HDT::markAlternative()
{
	altMarked = true;

	if (parent == NULL || parent->altMarked) return;
	parent->markAlternative();
}

void HDT::forceLinks()
{
	if (link != NULL)
	{
		link->hdtLink = this;
	}
	if (left != NULL) left->forceLinks();
	if (right != NULL) right->forceLinks();
}

void HDT::toDot()
{
	cout << "digraph g {" << endl;
	cout << "node[shape=circle];" << endl;
	toDotImpl();
	cout << "}" << endl;
}

void HDT::updateCounters()
{
	if (this->convertedFrom == C && this->left == NULL && this->right == NULL)
	{
		handleLeaf();
	}
	else if (this->convertedFrom == C)
	{
		if (left->type == C && right->type == C)
			handleCCToC();
		else
			handleIGToC();

		handleCTransform();
	}
	else if (this->type == C)
	{
		if (left->type == C && right->type == C)
			handleCCToC();
		else
			handleIGToC();
	}
	else if (this->type == G)
	{
		handleG();
	}
	up2date = true;
}

INTTYPE_REST HDT::getResolvedTriplets(unsigned int color)
{
    if (color < 0 || color >= this->degree)
        return -1;
	return tripResolved[color];
}

INTTYPE_REST HDT::getUnresolvedTriplets(unsigned int color)
{
    if (color < 0 || color >= this->degree)
        return -1;
	return tripUnresolved[color];
}

INTTYPE_REST HDT::getResolvedTriplets_root()
{
	return tripResolved_root;
}


HDT* HDT::constructHDT(RootedTree *t, int numD, HDTFactory *factory, bool doLink)
{
	HDT *hdt = preFirstRound(t, numD, doLink, factory);
    while(hdt->children != NULL)
	{
		hdt = hdt->round(factory);
	}
	hdt->factory = factory;
	hdt->factory->deleteTemplatedLinkedList();
	return hdt;
}

#define ADD_CHILD(PARENT, NEW_CHILD) {					                \
	NEW_CHILD->childParent = PARENT;						\
	TemplatedLinkedList<HDT*> *newItem = factory->getTemplatedLinkedList();		\
	newItem->data = NEW_CHILD;							\
	newItem->next = PARENT->children;						\
	PARENT->children = newItem;							\
}

HDT* HDT::preFirstRound(RootedTree *t, int numD, bool doLink, HDTFactory *factory)
{
	if (t->isLeaf())
	{
		HDT *hdt;
		if (t->numZeroes == 0)
			hdt = factory->getHDT(G, t, doLink);
		else
		{
			hdt = factory->getHDT(G, NULL, doLink);
			hdt->numZeroes = t->numZeroes;
		}
		hdt->convertedFrom = C;
		return hdt;
	}

	// Inner node
	HDT *node = factory->getHDT(I, NULL, doLink);
	for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next)
	{
		HDT *child = preFirstRound(i->data, numD, doLink, factory);
		ADD_CHILD(node, child);
	}
	return node;
}

HDT* HDT::round(HDTFactory *factory)
{
	// NOTE: C -> G when parent I etc is moved down!!

	// Composition 3: If we're a C we only have 1 child.
	// If that's a C, use CC->C, skip the child and go directly to that-one's child (if exists)
	if (type == C && children != NULL && children->next == NULL /*children.size() == 1*/)
	{
		HDT *child = children->data;
		if (child->type == C)
		{
			// CC->C, skip 2nd C and recurse
			HDT *newC = factory->getHDT(C, NULL, false);
			newC->left = this;
			newC->left->parent = newC;
			newC->right = child;
			newC->right->parent = newC;

			// If there's children, there's only 1. We recurse on that one and add the result to our children list.
			if (child->children != NULL)
			{
				child = child->children->data;
				child->childParent = NULL;
				child = child->round(factory);
				ADD_CHILD(newC, child);
			}
			return newC;
		}
	}

	// Recurse on non-G-children, build GG->G
	TemplatedLinkedList<HDT*> *lastG = NULL;
	int foundGs = 0;
	int downwardsOpenChildren = 0;
	TemplatedLinkedList<HDT*> *prevChild = NULL;
	for(TemplatedLinkedList<HDT*> *i = children; i != NULL; i = i->next)
	{
		// In each round we first transform all downwards closed C componenets
		// being children of I componenets into G componenets
		if (i->data->type == C && type == I && i->data->isDownwardsClosed())
		{
			// Convert to G
			i->data->type = G;
			i->data->convertedFrom = C;
		}

		// Composition 1
		if (i->data->type == G)
		{
			foundGs++;

			// We found 2 G's
			if (lastG != NULL)
			{
				// Merge the two G's by removing one and replaceing the other with the new G that points to the two old ones

				// Replace one with a new one with left, right and parent pointers set
				HDT *newG = factory->getHDT(G, NULL, false);
				newG->left = lastG->data;
				newG->left->parent = newG;
				newG->right = i->data;
				newG->right->parent = newG;
				newG->childParent = this;
				lastG->data = newG;

				// Delete the other
				prevChild->next = i->next;
				i = prevChild;

				// Reset lastG
				lastG = NULL;
			}
			else lastG = i;

			prevChild = i; //here too as we continue...

			// Don't recurse on G's
			continue;
		}
		if (!i->data->isDownwardsClosed()) downwardsOpenChildren++;

		// Recurse and save the "new child"
		i->data = i->data->round(factory);
		i->data->childParent = this;

		prevChild = i;
	}

	// Non-forking I with 1 G component: IG->C (Composition 2)
	if (type == I && downwardsOpenChildren < 2 && foundGs == 1)
	{
		HDT *newC = factory->getHDT(C, NULL, false);
		newC->left = this;
		newC->left->parent = newC;
		newC->right = lastG->data; // We've seen 1 G --- we saved that here
		newC->right->parent = newC;
		for(TemplatedLinkedList<HDT*> *i = children; i != NULL; i = i->next)
		{
			if (i->data != lastG->data)
			{
				ADD_CHILD(newC, i->data);
			}
		}

		return newC;
	}

	return this;
}

bool HDT::isDownwardsClosed()
{
	return children == NULL;
}

void HDT::toDotImpl()
{
	cout << "n" << this << "[label=\"";
	if (convertedFrom != NotConverted)
	{
		switch(convertedFrom)
		{
			case I: cout << "I"; break;
			case C: cout << "C"; break;
			case G: cout << "G"; break;
			case NotConverted: cout << "NotConverted"; break; // Shouldn't happen!
		}
		cout << " -> ";
	}
	switch(type)
	{
		case I: cout << "I"; break;
		case C: cout << "C"; break;
		case G: cout << "G"; break;
		case NotConverted: cout << "NotConverted"; break; // Shouldn't happen!
	}
	if (link != NULL) cout << "; " << link->name;
	if (type == G && convertedFrom == C && left == NULL && right == NULL) cout << "; 0's: " << numZeroes;
	cout << "\"];" << endl;

	if (left != NULL)
	{
		left->toDotImpl();
		cout << "n" << this << " -> n" << left << ";" << endl;
	}
	if (right != NULL)
	{
		right->toDotImpl();
		cout << "n" << this << " -> n" << right << ";" << endl;
	}
}
