#include <cstdlib> // for exit
#include "rooted_tree.h"
#include "hdt.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

/**************/
/* uym2 added */

void RootedTree::__set_label__(stack<RootedTree*> &stk, string &label, bool &wait_for_int_lab){
        if (label != ""){
            if (!wait_for_int_lab){
                RootedTree *p = this->factory->getRootedTree(label);
                //cout << "Added one tree to the factory" << endl;
                stk.push(p);
            } else {
                stk.top()->name = label;
            }
        }
        label = "";
        wait_for_int_lab = false;
}

bool RootedTree::read_newick_str(string str){
        this->maxDegree = 0;
		char c;
		bool wait_for_int_lab = false;
        bool is_at_root = true;
		stack<RootedTree*> stk;

		string label = "";
        string::iterator it = str.begin();

        while (it != str.end()){
			c = *it;
            if (c == '('){
                if (is_at_root){
                    stk.push(this);
                    is_at_root = false;
                }
                else{
				    stk.push(NULL);
                }
				label = "";
                ++it;

			} else if (c == ',') {
				this->__set_label__(stk,label,wait_for_int_lab);
                ++it;

            } else if (c == ')'){
				this->__set_label__(stk,label,wait_for_int_lab);
				
				RootedTree *q = stk.top();
			    stk.pop();
                unsigned int numChildren = 0;
                TemplatedLinkedList<RootedTree*> *children = NULL;

				while (q != NULL && q != this){
                    numChildren++;
                    TemplatedLinkedList<RootedTree*> *newItem = factory->getTemplatedLinkedList();
                    newItem->data = q;
                    newItem->next = children;
                    children = newItem;
                    q = stk.top();
					stk.pop();
				}

                if (q == NULL){
                    q = this->factory->getRootedTree();
                }
                
                q->children = children;
                q->numChildren = numChildren;  
                
                for (TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
                    i->data->parent = q;
				
                if (numChildren > this->maxDegree)
                    this->maxDegree = numChildren;  
                
                stk.push(q);
				label = "";
				wait_for_int_lab = true;
			    ++it;

            } else if (c == ';'){
				this->__set_label__(stk,label,wait_for_int_lab);
				stk.pop();
				break;

			} else if (c == ':'){
				this->__set_label__(stk,label,wait_for_int_lab);
                string se = "";
                ++it;
                c = *it;
                while (c != ',' && c != ')' && c != ';'){
				    se += c;
                    ++it;
                    c = *it;
                }
                double e = strtod(se.c_str(),NULL);
				stk.top()->edge_length = e;
				wait_for_int_lab = false;
			}
			else {
				label += c;
                ++it;
			}
		}
		return true;
}

bool RootedTree::read_newick_file(string treeFile){
        this->maxDegree = 0;
        ifstream fin;
        fin.open(treeFile.c_str());
		char c;
		bool wait_for_int_lab = false;
        bool is_at_root = true;
		stack<RootedTree*> stk;

		string label = "";
		while (!fin.eof()){
			fin >> c;
            if (c == '('){
                if (is_at_root){
                    stk.push(this);
                    is_at_root = false;
                }
                else{
				    stk.push(NULL);
                }
				label = "";
			}
			
            else if (c == ',') {
				this->__set_label__(stk,label,wait_for_int_lab);
			
            } else if (c == ')'){
				this->__set_label__(stk,label,wait_for_int_lab);
				
				RootedTree *q = stk.top();
			    stk.pop();
                unsigned int numChildren = 0;
                TemplatedLinkedList<RootedTree*> *children = NULL;

				while (q != NULL && q != this){
                    numChildren++;
                    TemplatedLinkedList<RootedTree*> *newItem = factory->getTemplatedLinkedList();
                    newItem->data = q;
                    newItem->next = children;
                    children = newItem;
                    q = stk.top();
					stk.pop();
				}

                if (q == NULL){
                    q = this->factory->getRootedTree();
                }
                
                q->children = children;
                q->numChildren = numChildren;

                for (TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
                    i->data->parent = q;

                if (numChildren > this->maxDegree)
                    this->maxDegree = numChildren;  
				
                stk.push(q);
				label = "";
				wait_for_int_lab = true;
			
            } else if (c == ';'){
				stk.pop();
				break;
			} else if (c == ':'){
				this->__set_label__(stk,label,wait_for_int_lab);
				double e;
				fin >> e;
				stk.top()->edge_length = e;
				wait_for_int_lab = false;
			}
			else {
				label += c;
			}
		}
        fin.close();
		return true;
	}
	
void RootedTree::mark_active(TripletCounter *tripCount){
    if (this->isLeaf())
        return;
    bool is_active = false;    
    for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next){
        RootedTree* curr = i->data;
        curr->mark_active(tripCount);
        is_active = is_active || tripCount->isActive[curr->idx];
    }
    tripCount->isActive[this->idx] = is_active;
}

bool RootedTree::remove_child(RootedTree *child){
    if (child->parent != this)
        return false;

    if (children == NULL)
        return false;

    if (children->data == child){
        TemplatedLinkedList<RootedTree*> *temp = children;
        children = children->next;
        temp->next = NULL;
        temp->data = NULL;
        child->parent = NULL;
        numChildren--;
        return true;
    }

    for(TemplatedLinkedList<RootedTree*> *i = children; i->next != NULL; i = i->next){
        if (child == i->next->data){
            TemplatedLinkedList<RootedTree*> *temp = i->next;
            i->next = i->next->next;
            child->parent = NULL;
            numChildren--;
            return true;
        }         
    }
    return false;
}

RootedTree* RootedTree::reroot_at_edge(RootedTree* node, double x){
    RootedTree* v = node;
    if (v == this) // v is the root
        return this;
    
    /*for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next){
        if (v == i->data)
            return this; // v is one of the root's children
    }*/
        
    RootedTree *u = v->parent;    
    RootedTree *w = u->parent;
    double ev = v->edge_length;
    double eu = u->edge_length;


    //RootedTreeFactory *tFactory = new RootedTreeFactory();
    RootedTreeFactory *tFactory = node->factory;
    RootedTree *newRoot = tFactory->getRootedTree();
    newRoot->factory = tFactory;

    //RootedTree *newRoot = this->factory->getRootedTree();
    //newRoot->factory = this->factory;
    u->remove_child(v);
    newRoot->children = NULL;
    newRoot->addChild(v);
    newRoot->addChild(u);
    v->edge_length = ev-x;
    u->edge_length = x;
  
   
    while(w != NULL){
        v = w;
        w = w->parent;
        ev = v->edge_length;;
        u->addChild(v);
        v->edge_length = eu;

        u = v;
        eu = ev;
    }
    
    if (u->numChildren == 1){
        // suppress unifurcation
        // u has a single child; it is v = u->children->data
        v = u->children->data;
        double e = u->edge_length + v->edge_length;
        w = u->parent;
        w->remove_child(u);
        v->edge_length = e;
        w->addChild(v);
    }
    
    return newRoot;
}

void RootedTree::write_newick(ofstream &fout){
    this->__write_newick__(fout);
    fout << ";";
}

void RootedTree::__write_newick__(ofstream &fout){
    if (this->isLeaf()){
        fout << this->name;
        if (this->edge_length >= 0)
            fout << ":" << this->edge_length;
    }
    else {
        fout << "(";
        // first child
        RootedTree *firstChild = this->children->data;
        firstChild->__write_newick__(fout);
        // the remaining children
        for (TemplatedLinkedList<RootedTree*> *i = children->next; i != NULL; i = i->next){
            fout << ",";
            i->data->__write_newick__(fout);
        }
        fout << ")" << this->name;
        if (this->edge_length >= 0)
            fout << ":" << this->edge_length;
    }
}

string RootedTree::toString(){
    string myString = "";
    this->__to_string__(myString);
    myString += ";";

    return myString;
}

void RootedTree::__to_string__(string &myString){
    if (this->isLeaf()){
        myString += this->name;
        if (this->edge_length >= 0){
            myString += ":";
            std::ostringstream convertor;
            convertor << this->edge_length;
            myString += convertor.str(); //std::to_string(this->edge_length);
        }
    }
    else {
        myString += "(";
        // first child
        RootedTree *firstChild = this->children->data;
        firstChild->__to_string__(myString);
        // the remaining children
        for (TemplatedLinkedList<RootedTree*> *i = children->next; i != NULL; i = i->next){
            myString += ",";
            i->data->__to_string__(myString);
        }
        myString += ")";
        myString += this->name;
        if (this->edge_length >= 0){
            myString += ":";
            std::ostringstream convertor;
            convertor << this->edge_length;
            myString += convertor.str(); //std::to_string(this->edge_length);
        }
    }
}

void RootedTree::print_leaves(){
    if (this->isLeaf())
        std::cout << this->name << " ";
    else {
        for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next) {
            i->data->print_leaves();
        }
    }       
}

unsigned int RootedTree::set_all_idx(unsigned int startIdx){
    unsigned int currIdx = startIdx;
    this->set_idx(currIdx); 
    currIdx++;
    for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
    {
        RootedTree *t = i->data; 
        currIdx = t->set_all_idx(currIdx);
    }
    return currIdx;
}

void RootedTree::count_nodes(){
    if (this->isLeaf())
        this->nodeCounts = 1;
    else{
        unsigned int count = 1;
        for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
        {
            RootedTree *t = i->data;
            t->count_nodes(); 
            count += t->nodeCounts;
        }
        this->nodeCounts = count;
    }
}

unsigned int RootedTree::count_leaves(){
    if (this->isLeaf())
        return 1;
    else{
        unsigned int count = 0;
        for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
        {
            RootedTree *t = i->data;
            count += t->count_leaves(); 
        }
	return count;
    }
}

RootedTree* RootedTree::search_idx(unsigned int idx){
    if(this->idx == idx)
        return this;
    
    for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
    {
        RootedTree *t = i->data; 
        RootedTree *result = t->search_idx(idx);
        if (result)
            return result;
    }
    return NULL;
}

RootedTree* RootedTree::search_name(string name){
    if(this->name == name)
        return this;
    
    for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
    {
        RootedTree *t = i->data; 
        RootedTree *result = t->search_name(name);
        if (result)
            return result;
    }
    return NULL;
}

RootedTree* RootedTree::copyTree(RootedTreeFactory *factory){
    // create a deep copy of tree t
	//if (factory == NULL) factory = new RootedTreeFactory(NULL);
	if (factory == NULL) factory = this->factory;

    string tree_str = this->toString();
    RootedTree *t_new = factory->getRootedTree();
    t_new->factory = factory;
	t_new->read_newick_str(tree_str);

    return t_new;
}

RootedTree* RootedTree::down_root(RootedTree *u) {
    
    // we assume that u is a grandchild of the root
	//RootedTreeFactory *factory = new RootedTreeFactory(NULL);
	RootedTreeFactory *factory = new RootedTreeFactory();
    RootedTree *v = factory->getRootedTree();
    RootedTree *v1 = factory->getRootedTree();
    RootedTree *v2 = u->copyTree(factory);
    
    
    // make a copy for each sister of u and make them the children of v1
    for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
    {
        if (i->data != u->parent){
            v1->addChild(i->data->copyTree(factory));
        }
    }
    
    // make a copy for each child of the root except for u->parent and make them the children of v1
    for(TemplatedLinkedList<RootedTree*> *i = u->parent->children; i != NULL; i = i->next)
    {
        if (i->data != u) { 
            v1->addChild(i->data->copyTree(factory));
        }
    }
    v->addChild(v1);
    v->addChild(v2);
    
    return v;
}

/*************/



void RootedTree::initialize(string name)
{
	parent = altWorldSelf = NULL;
	children = NULL;
	level = maxDegree = 0;
	numZeroes = numChildren = 0;

	// Soda13 counting stuff set to default to -1 so we can print if they're not -1...
	n = -1;

	// Color 1 is standard...
	color = 1;
	this->name = name;
    this->factory = NULL;
    this->edge_length = -1;
}

bool RootedTree::isLeaf()
{
	return numChildren == 0;
}

bool RootedTree::isRoot()
{
    return this->parent == NULL;
}

void RootedTree::compute_d2root(){
    if (this->isRoot())
        this->d2root = 0;
    for(TemplatedLinkedList<RootedTree*> *i = this->children; i != NULL; i = i->next){
        RootedTree* t = i->data;
        t->d2root = this->d2root + t->edge_length;
        t->compute_d2root();
    }
}

void RootedTree::addChild(RootedTree *t)
{
    // uym2 added: if t has a parent, remove it from the parent's children first
    if (t->parent != NULL){
        t->parent->remove_child(t);
    }

	numChildren++;
	t->parent = this;
	TemplatedLinkedList<RootedTree*> *newItem = factory->getTemplatedLinkedList();
	newItem->data = t;
	newItem->next = children;
	children = newItem;
}

RootedTree* RootedTree::getParent()
{
	return parent;
}

void RootedTree::toDot()
{
	cout << "digraph g {" << endl;
	cout << "node[shape=circle];" << endl;
	toDotImpl();
	cout << "}" << endl;
}

vector<RootedTree*>* RootedTree::getList()
{
	vector<RootedTree*>* list = new vector<RootedTree*>();
	getListImpl(list);
	return list;
}

bool RootedTree::prune_subtree(RootedTree* u){
    RootedTree* w = u->parent;
    if (w == NULL){
        cerr << "Are you trying to remove the root?" << endl;
        return false;
    }    
    if (!w->remove_child(u)){
        cerr << "Could not remove leaf!" << endl;
        return false;
    }
    
    if (w->numChildren < 1){
        cerr << "The tree has unifurcation?" << endl;
        return false;
    }
    else if (w->numChildren == 1){
        // supress unifurcation
        //cerr << "Supress unifurcation after removing leaf ..." << endl;
        RootedTree *v = w->children->data;
        double l = v->edge_length;
        w->remove_child(v);
        
        if (w->parent == NULL) { // the leaf we are trying to remove is a child of the root
            // w will replace v after the following commands
            w->name = v->name;
        
            while(v->children != NULL){
                RootedTree *t = v->children->data;
                double l1 = t->edge_length;
                w->addChild(t);
                t->edge_length = l+l1;
            }
        } else {
            RootedTree *p = w->parent;
            double l1 = w->edge_length;
            p->remove_child(w);
            p->addChild(v);
            v->edge_length = l+l1;
        }
    } 
    return true;
}

bool RootedTree::pairAltWorld(RootedTree *t, bool do_pruning, TripletCounter *tripCount)
{
	//bool error = false;
	vector<RootedTree*>* l = t->getList();
	map<string, RootedTree*> altWorldLeaves;

	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
		altWorldLeaves[leaf->name] = leaf;
	}

	delete l;
	l = getList();
	map<string, RootedTree*>::iterator altWorldEnd = altWorldLeaves.end();
	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
		map<string, RootedTree*>::iterator j = altWorldLeaves.find(leaf->name);
		if (j == altWorldEnd)
		{
			// This leaf wasn't found in the input tree!
            if (do_pruning){
                // prune the leaf out from the first tree then continue
                //cerr << leaf->name << " didn't exist in the second tree. Pruning it out from the first tree..." << endl;
                if (this->prune_subtree(leaf)){
		    cerr << "Removed leaf " << leaf->name << endl;	
                    continue;
                }
                else {
                    cerr << "Could not remove leaf. Aborting!" << " Leaf label: " << leaf->name << endl;
                    //error = true;
                    delete l;
                    return false;
                }
            } else {
			    cerr << "Leaves don't agree! Aborting! (" << leaf->name << " didn't exist in the second tree)" << endl;
			    //error = true;
			    delete l;
			    return false;
            }
		}
				
		// If we got this far, we found the match! Setup bidirectional pointers!
		leaf->altWorldSelf = j->second;
		j->second->altWorldSelf = leaf;
        
		// Delete result
		altWorldLeaves.erase(j);
	}


	// Is there results left in altWorldLeaves? If so it had more leaves than we do...
	if (altWorldLeaves.size() > 0)
	{
        if (tripCount != NULL){
            for(map<string,RootedTree*>::iterator i = altWorldLeaves.begin(); i != altWorldLeaves.end(); i++){
                //cerr << i->first << " didn't exist in the first tree. It will be ignored in the second tree..." << endl;
                tripCount->isActive[i->second->idx] = false;      
             }
        } else {
            cerr << "Leaves don't agree! Aborting! (" << altWorldLeaves.begin()->first << " didn't exist in the first tree)";
            if (altWorldLeaves.size() > 1)
                cerr << " (and " << (altWorldLeaves.size() - 1) << " other leaves missing from first tree!)";
            cerr << endl;
            //error = true;
            delete l;
            return false;
        }
	}
	delete l;
    return true;
}

void RootedTree::colorSubtree(int c)
{
	color = c;
	if (altWorldSelf != NULL)
	{
		altWorldSelf->color = c;
		if (altWorldSelf->hdtLink != NULL)
		{
			altWorldSelf->hdtLink->mark();
		}
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		i->data->colorSubtree(c);
	}
}

void RootedTree::markHDTAlternative()
{
	if (altWorldSelf != NULL)
	{
		if (altWorldSelf->hdtLink != NULL)
		{
			altWorldSelf->hdtLink->markAlternative();
		}
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		i->data->markHDTAlternative();
	}
}

bool RootedTree::isError()
{
	return this->error;
}

void RootedTree::toDotImpl()
{
	cout << "n" << this << "[label=\"";
	if (isLeaf() && numZeroes > 0) cout << "0's: " << numZeroes;
	else cout << name;
	
	cout << "\"];" << endl;

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		RootedTree *t = i->data;
		t->toDotImpl();
		cout << "n" << this << " -> n" << t << ";" << endl;
	}
}

void RootedTree::getListImpl(vector<RootedTree*>* list)
{
	if (isLeaf())
	{
		list->push_back(this);
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		RootedTree *t = i->data;
		t->level = level + 1;
		t->getListImpl(list);
	}
}

void RootedTree::computeNullChildrenData()
{
	if (isLeaf()) return;

	bool allZeroes = true;
	numZeroes = 0;
	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		i->data->computeNullChildrenData();
		if (i->data->numZeroes == 0) allZeroes = false;
		else numZeroes += i->data->numZeroes;
	}
	if (!allZeroes) numZeroes = 0;
}

RootedTree* RootedTree::contract(RootedTreeFactory *factory)
{
	computeNullChildrenData();
	return contractImpl(factory);
}

RootedTree* RootedTree::contractImpl(RootedTreeFactory *factory)
{
	if (isLeaf()) return this; // reuse leaves!!

	//if (factory == NULL) factory = new RootedTreeFactory(this->factory);
	if (factory == NULL) factory = new RootedTreeFactory();

	INTTYPE_REST totalNumZeroes = 0;
	RootedTree *firstNonZeroChild = NULL;
	RootedTree *ourNewNode = NULL;
	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		RootedTree *t = i->data;
		if (t->numZeroes > 0) totalNumZeroes += t->numZeroes;
		else
		{
			if (firstNonZeroChild == NULL) firstNonZeroChild = t->contractImpl(factory);
			else
			{
				if (ourNewNode == NULL)
				{
					ourNewNode = factory->getRootedTree();
					ourNewNode->addChild(firstNonZeroChild);
				}
				ourNewNode->addChild(t->contractImpl(factory));
			}
		}
	}

	// Have we found >= 2 non-zero children?
	if (ourNewNode == NULL)
	{
		// No... We only have 1 non-zero children!
		if (firstNonZeroChild->numChildren == 2)
		{
			RootedTree *zeroChild = firstNonZeroChild->children->data;
			RootedTree *otherOne = firstNonZeroChild->children->next->data;
			if (zeroChild->numZeroes == 0)
			{
				RootedTree *tmp = otherOne;
				otherOne = zeroChild;
				zeroChild = tmp;
			}
			if (zeroChild->numZeroes != 0 && !otherOne->isLeaf())
			{
				// The 1 child has a zero child and only 2 children, the other not being a leaf, i.e. we can merge!
				zeroChild->numZeroes += totalNumZeroes;
				return firstNonZeroChild;
			}
			// if (zeroChild->numZeroes == 0) it's not a zerochild!!
		}

		// The child doesn't have a zero child, i.e. no merge...
		ourNewNode = factory->getRootedTree();
		ourNewNode->addChild(firstNonZeroChild);
	}

	// We didn't merge below --- add zero-leaf if we have any zeros...
	if (totalNumZeroes > 0)
	{
		RootedTree *zeroChild = factory->getRootedTree();
		zeroChild->numZeroes = totalNumZeroes;
		ourNewNode->addChild(zeroChild);
	}

	return ourNewNode;
}

void RootedTree::__count_children__(RootedTree *t) {
  if (t->isLeaf()) {
    t->n = 1;
    return;
  }
  
  int nSum = 0;
  for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next) {
    RootedTree *childI = i->data;
    this->__count_children__(childI);
    nSum += childI->n;
  }
  t->n = nSum;
}

void RootedTree::countChildren(){
    this->__count_children__(this);
}

bool __wayToSort__(RootedTree* i, RootedTree* j){
    return i->d2root < j->d2root;
}

vector<RootedTree*> RootedTree::sort_leaf_by_d2root(){
    this->compute_d2root();
    vector<RootedTree*> L = *this->getList(); // leafset
    sort(L.begin(),L.end(),__wayToSort__);    
    return L;
}

void __sum_d2root__(RootedTree *t, double & nleaves, double d2root, double & acc_sum){
    if (t->isLeaf()){
        nleaves += 1;
        acc_sum += d2root;
    } else {
        for (TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next){
            __sum_d2root__(i->data, nleaves, d2root + i->data->edge_length, acc_sum);
        }
    }
}

double RootedTree::mean_d2root(){
    double acc_sum = 0;
    double nleaves = 0;

    __sum_d2root__(this, nleaves, 0, acc_sum);
    return acc_sum/nleaves;
}

