#include "rooted_tree_lb.h"

int RootedTreeLb::set_all_labels(unsigned int startIdx){
    if this->isleaf() { this->set_label(startIdx); return 0; }
    rcode = 0
    for(TemplatedLinkedList<RootedTreeLb*> *i = children; i != NULL; i = i->next)
    {
        RootedTreeLb *t = i->data; 
        rcode += t->set_all_labels(startIdx+1);
    }
    return rcode;
}

RootedTreeLb::RootedTreeLb(RootedTree *t){
    if t->isleaf() { *this = *t };
    else {
        for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
            this->children.addchild(new RootedTreeLb(*i->data));
        }
    }
    this->set_all_labels(0);
}    

RootedTreeLb::RootedTreeLb(RootedTreeLb *t){
    if t->isleaf() { *this = *t };
    else {
        for(TemplatedLinkedList<RootedTreeLb*> *i = t->children; i != NULL; i = i->next){
            this->children.addchild(new RootedTreeLb(*i->data));
        }
    }
}

int RootedTreeLb::down_root(RootedTreeLb *u) {
    // we assume that u is a grandchild of the root
    RootedTreeLb *v = new RootedTreeLb();
    RootedTreeLb *v1 = new RootedTreeLb();
    RootedTreeLb *v2 = new RootedTreeLb(*u);
    
    // make a copy for each sister of u and make them the children of v1
    for (TemplatedLinkedList<RootedTree*> *i = u->children, i != NULL; i = i->next)
    {
        v1.addchild(i->data);
    }

    // make a copy for each child of the root except for u->parent and make them the children of v1
    for (TemplatedLinkedList<RootedTree*> *i = this->children, i != NULL; i = i->next)
    {
        if *i != *u->parent {
            v1.addchild(i->data);
        }
    }
    
    v->addchild(v1);
    v->addchild(v2);

    return v;
}
