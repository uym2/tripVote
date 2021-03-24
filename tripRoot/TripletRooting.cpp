#include "TripletRooting.h"
#include "hdt.h"
#include "hdt_factory.h"

bool TripletRooting::pairing(){
    if (!myRef->pairAltWorld(myTree,true,tripCount)) {    
    //if (myTree->isError()) {
        //std::cerr << "TripletRooting::pairing: Failed to pair the two trees. Aborting!" << std::endl;
        return false;
    }

    if (myRef->count_leaves() < 3){
    	//std::cerr << "TripletRooting::pairing: Reference tree after pairing has less than 3 leaves. Aborting!" << std::endl;
	return false;
    }

    myTree->mark_active(tripCount);
    return true;
}

bool TripletRooting::find_optimal_root(){
    if (this->compute_tripScore()){
        //this->optimalRoot = optimaltripRoots->data;
        this->optimalRoot = optimaltripRoots[0];
        return true;
    } else{
        return false;
    }
}

bool TripletRooting::compute_tripScore(){
    if (!this->pairing()){
        //cerr << "TripletRooting::compute_tripScore: Could not pair the two trees. Aborting!" << endl;
        return false;
    }
    // construct HDT for myRef
    hdt = HDT::constructHDT(myRef, myTree->maxDegree + 1, dummyHDTFactory);

    count(myTree);

    unsigned int r = myTree->idx;

    //this->optimaltripRoots = NULL;
    this->optimalTripScore = -1;

    this->tripCount->tripScore[r] = this->compute_root_tripScore();
    
    this->__downroot__(myTree,this->tripCount->tripScore[r],false);

    return true;
}

void TripletRooting::__downroot__(RootedTree *t, INTTYPE_REST parent_score, bool parent_active){
    unsigned int u = t->idx;
    
    for(TemplatedLinkedList<RootedTree*> *current = t->children; current != NULL; current = current->next) {
        unsigned int v = current->data->idx;
        
        INTTYPE_REST current_score = parent_score - tripCount->tI[u] + tripCount->tO[v] + tripCount->tR[v];
        this->tripCount->tripScore[v] = current_score;
        
        bool sister_active = false;
        for(TemplatedLinkedList<RootedTree*> *sis = t->children; sis != NULL; sis = sis->next) {
            if (sis != current){
                unsigned int v1 = sis->data->idx;
                if (tripCount->isActive[v1]){
                    sister_active = true;
                    break;
                }
            }
        }
        if (parent_active || sister_active){
            if (current_score == this->optimalTripScore && t != this->myTree){
                /*TemplatedLinkedList<RootedTree*> *newItem = new TemplatedLinkedList<RootedTree*>;
                newItem->data =  current->data;
                newItem->next = this->optimaltripRoots;
                this->optimaltripRoots = newItem; */
                this->optimaltripRoots.push_back(current->data);
                this->ambiguity += 1;
            }
            else if (current_score > this->optimalTripScore){
                /*TemplatedLinkedList<RootedTree*> *newItem = new TemplatedLinkedList<RootedTree*>;
                newItem->data =  current->data;
                this->optimalTripScore = current_score;
                newItem->next = NULL;
                this->optimaltripRoots = newItem;*/
                this->optimalTripScore = current_score;
                this->optimaltripRoots.push_back(current->data);
                this->ambiguity = 1;
            }
        }
        INTTYPE_REST v_parent_score = current_score - tripCount->tR[v]; 
        bool v_parent_active = parent_active | sister_active;
        this->__downroot__(current->data,v_parent_score,v_parent_active);
    }   
}

TripletRooting::TripletRooting(){
    this->myRef = this->myTree = NULL;
    this->hdt = NULL;
    this->tripCount = NULL;
    this->dummyHDTFactory = NULL;
    this->ambiguity = 0;
    this->factory = NULL;
    //this->optimaltripRoots = NULL;
}

bool TripletRooting::initialize(RootedTree *ref, RootedTree *tree){
    this->hdt = NULL;
    this->factory = new RootedTreeFactory();
    this->myRef = ref->copyTree(this->factory);
    this->myTree = tree->copyTree(this->factory);
    
    myTree->set_all_idx(0);
    myTree->count_nodes();
    myTree->countChildren();           
    
    unsigned int N = this->myTree->nodeCounts;
    tripCount = new TripletCounter(N);
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    return true;
}

TripletRooting::~TripletRooting(){
    delete this->tripCount;
    delete this->factory;
    /*while (this->optimaltripRoots != NULL){
        TemplatedLinkedList<RootedTree*> *curr = this->optimaltripRoots;
        this->optimaltripRoots = this->optimaltripRoots->next;
        delete curr;
    }*/
    delete dummyHDTFactory;
}

void TripletRooting::update_tI(unsigned int nodeIdx, bool count_unresolved){
        this->tripCount->tI[nodeIdx] = this->hdt->getResolvedTriplets(0) + count_unresolved*this->hdt->getUnresolvedTriplets(0);
}

void TripletRooting::update_tO(unsigned int nodeIdx, unsigned int color, bool count_unresolved){
        this->tripCount->tO[nodeIdx] = this->hdt->getResolvedTriplets(color) + count_unresolved*this->hdt->getUnresolvedTriplets(color);
}

void TripletRooting::update_tR(unsigned int nodeIdx){
    this->tripCount->tR[nodeIdx] = this->hdt->getResolvedTriplets_root();
}



INTTYPE_REST TripletRooting::__compute_root_tripScore__(RootedTree *v){
    INTTYPE_REST acc = this->tripCount->tI[v->idx];
    for(TemplatedLinkedList<RootedTree*> *current = v->children; current != NULL; current = current->next) {
        acc += this->__compute_root_tripScore__(current->data);
    }
    return acc;    
}

INTTYPE_REST TripletRooting::compute_root_tripScore(){
    return this->__compute_root_tripScore__(this->myTree);
}

void TripletRooting::count(RootedTree *v) {
  //if (v->isLeaf() || v->n <= 2) {
  if (v->isLeaf()) {    
    hdt->updateCounters();
    tripCount->tR[v->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);

    // This will make sure the entire subtree has color 0!
    v->colorSubtree(0);
    return;
  }

  // v is not a leaf!
  // Find largest subtree
  TemplatedLinkedList<RootedTree*> *largest = v->children;
  int largestN = largest->data->n;
  TemplatedLinkedList<RootedTree*> *beforeLargest = NULL;
  TemplatedLinkedList<RootedTree*> *prev = v->children;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    if (current->data->n > largestN) {
      largest = current;
      beforeLargest = prev;
      largestN = largest->data->n;
    }
    prev = current;
  }
  if (beforeLargest != NULL) {
    beforeLargest->next = largest->next;
    largest->next = v->children;
    v->children = largest;
  }
  
  // Color i'th subtree (i > 1) with color i
  int c = 2;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    current->data->colorSubtree(c);
    c++;
  }

  // Update counters in the HDT
  hdt->updateCounters();
  update_tI(v->idx);
  
  //if (v != this->myTree) // v is not the root
  {
      // compute triplets outside of each child of v
      int c = 1;
      for(TemplatedLinkedList<RootedTree*> *current = v->children; current != NULL; current = current->next) {
          update_tO(current->data->idx,c);
          c++;
      }      
  }


  // Color to 0
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    current->data->colorSubtree(0);
  }

  // Contract and recurse on 1st child
  RootedTree *firstChild = v->children->data;
  hdt->updateCounters();  
  tripCount->tR[firstChild->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);
    

  //if (firstChild->isLeaf() || firstChild->n <= 2) {
  if (firstChild->isLeaf()) {    
    // Do "nothing" (except clean up and possibly color!)
    firstChild->colorSubtree(0);
  } else {
    count(firstChild);
    // HDT is deleted in recursive call!
  }
  
  // Color 1 and recurse
  c = 0;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
      
      current->data->colorSubtree(1);
      hdt->updateCounters();  
      tripCount->tR[current->data->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);
      
      count(current->data);
    c++; // Weee :)
    // HDT is deleted on recursive calls!
  }
}

void TripletRooting::countChildren(RootedTree *t) {
  if (t->isLeaf()) {
    t->n = 1;
    return;
  }
  
  int nSum = 0;
  for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next) {
    RootedTree *childI = i->data;
    countChildren(childI);
    nSum += childI->n;
  }
  t->n = nSum;
}
