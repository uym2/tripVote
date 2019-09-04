#include "TripletRooting_OG.h"

TripletRootingOG::TripletRootingOG(RootedTree *ref, RootedTree *tree, string ingroupLabel){
    this->myRef = ref;
    this->myTree = tree;
    this->hdt = NULL;
    unsigned int N = this->myTree->set_all_idx(0);
    tripCount = new TripletCounter(N);
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    this->ingroupLabel = ingroupLabel;
}

RootedTree* TripletRootingOG::__find_ingroup_clade__(RootedTree* tree){
    if (tree->isLeaf()){
        if (tree->name == this->ingroupLabel)
            return tree;
        else
            return NULL;    
    } else {
        for(TemplatedLinkedList<RootedTree*> *i = tree->children; i != NULL; i = i->next){
            RootedTree* t = this->__find_ingroup_clade__(i->data); 
            if (t != NULL)
                return t;
        }
        return NULL;        
    }
}

bool TripletRootingOG::pairing(){
    RootedTree *igRoot = this->__find_ingroup_clade__(this->myRef);

	vector<RootedTree*>* l = this->myTree->getList();
	map<string, RootedTree*> altWorldLeaves;

	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
		altWorldLeaves[leaf->name] = leaf;
	}

	delete l;
	l = this->myRef->getList();
	map<string, RootedTree*>::iterator altWorldEnd = altWorldLeaves.end();
	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
        if (leaf == igRoot) // ignore the igRoot to avoid removing it from myRef
            continue;
		map<string, RootedTree*>::iterator j = altWorldLeaves.find(leaf->name);
		if (j == altWorldEnd)
		{
			// This leaf was found in myRef but not myTree; we need to prune it out
            cerr << leaf->name << " was found in reference tree but not the tree we are trying to root. Pruning it out from the reference tree..." << endl;
            if (!this->myRef->prune_subtree(leaf)){
                cerr << "Error: Could not remove leaf! Aborting" << endl;
                return false;
            }
            continue;
		}
				
		// If we got this far, we found the match! Setup bidirectional pointers!
		leaf->altWorldSelf = j->second;
		j->second->altWorldSelf = leaf;
        
		// Delete result
		altWorldLeaves.erase(j);
	}


	// Is there results left in altWorldLeaves? If so we need to fill these leaves into the reference tree below its igRoot
	if (altWorldLeaves.size() > 0)
	{
        if (igRoot == NULL){
            cerr << "Warning: found ingroup leaves but don't know where to place them! Ignoring them all in the rooting tree!"  << endl;
            for(map<string,RootedTree*>::iterator i = altWorldLeaves.begin(); i != altWorldLeaves.end(); i++)
                this->tripCount->isActive[i->second->idx] = false;     
        } else {
            // filling ingroups into the reference tree below igRoot
            for(map<string,RootedTree*>::iterator i = altWorldLeaves.begin(); i != altWorldLeaves.end(); i++) {
                RootedTree* newLeaf = myRef->factory->getRootedTree(i->second->name);
                igRoot->addChild(newLeaf);
                newLeaf->altWorldSelf = i->second;
                i->second->altWorldSelf = newLeaf;
            }               
        }
	} else {
        if (igRoot != NULL){
            cerr << "Warning: Reference tree was marked as incomplete but no ingroups found in the tree we are trying to root." << endl;
            // remove igRoot
            this->myRef->prune_subtree(igRoot);
        } 
    }
    
    this->myTree->mark_active(tripCount);
	
    delete l; 
    return true;
}
