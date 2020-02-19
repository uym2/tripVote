#include "TripletRooting.h"
#include "hdt.h"
#include "hdt_factory.h"

bool TripletRooting::pairing(){
    myRef->pairAltWorld(myTree,true,tripCount);
    
    if (myTree->isError()) {
        std::cerr << "The two trees do not have the same set of leaves." << std::endl;
        std::cerr << "Aborting." << std::endl;
        return false;
    }

    myTree->mark_active(tripCount);
    return true;
}

bool TripletRooting::find_optimal_root(){
    countChildren(myTree);
    if (this->compute_tripScore()){
        this->optimalRoot = optimaltripRoots->data;
        return true;
    } else{
        return false;
    }
}

bool TripletRooting::compute_tripScore(){
    if (!this->pairing()){
        cerr << "Error: could not pair the two trees. Aborting!" << endl;
        return false;
    }

    // construct HDT for myRef
    //std::cout << "Degree: " << myTree->maxDegree + 1 << std::endl;
    hdt = HDT::constructHDT(myRef, myTree->maxDegree + 1, dummyHDTFactory);
    
    count(myTree);

    unsigned int r = myTree->idx;

    this->optimaltripRoots = NULL;
    this->optimalTripScore = -1;

    this->tripCount->tripScore[r] = this->compute_root_tripScore();
    
    this->__downroot__(myTree,this->tripCount->tripScore[r],false);

    return true;
}

void TripletRooting::__downroot__(RootedTree *t, INTTYPE_REST parent_score, bool parent_active){
    unsigned int u = t->idx;
    
    for(TemplatedLinkedList<RootedTree*> *current = t->children; current != NULL; current = current->next) {
        unsigned int v = current->data->idx;
        //if (!tripCount->isActive[v]){
        //    continue;
        //}
        
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
            TemplatedLinkedList<RootedTree*> *newItem = new TemplatedLinkedList<RootedTree*>;
            newItem->data =  current->data;
            if (current_score == this->optimalTripScore && t != this->myTree){
                newItem->next = this->optimaltripRoots;
                this->optimaltripRoots = newItem;
                this->ambiguity += 1;
            }
            else if (current_score > this->optimalTripScore){
                this->optimalTripScore = current_score;
                newItem->next = NULL;
                this->optimaltripRoots = newItem;
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
}

bool TripletRooting::initialize(RootedTree *ref, RootedTree *tree){
    this->myRef = ref;
    this->myTree = tree;
    this->hdt = NULL;
    unsigned int N = this->myTree->nodeCounts;
    tripCount = new TripletCounter(N);
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    return true;
}

TripletRooting::~TripletRooting(){
    delete [] tripCount;
    while (this->optimaltripRoots != NULL){
        TemplatedLinkedList<RootedTree*> *curr = this->optimaltripRoots;
        this->optimaltripRoots = this->optimaltripRoots->next;
        delete curr;
    }
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


/*
void TripletRooting::updateCounters(unsigned int nodeIdx, unsigned int color){
    if (color == 0) {
        // update tI
        this->tripCount->tI[nodeIdx] = this->hdt->getResolvedTriplets(0) + this->hdt->getUnresolvedTriplets(0);
    }
    else { 
        // update tO
        this->tripCount->tO[nodeIdx] = this->hdt->getResolvedTriplets(color) + this->hdt->getUnresolvedTriplets(color);
    }
    // update tR
    this->tripCount->tR[nodeIdx] = this->hdt->getResolvedTriplets_root();
}*/

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

/*    
#ifdef doExtractAndContract
    delete hdt->factory;
#endif
*/    
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

/*  
#ifdef doExtractAndContract
  // Extract
  RootedTree** extractedVersions = new RootedTree*[v->numChildren - 1];
  c = 0;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    if (current->data->isLeaf() || current->data->n <= 2) {
      extractedVersions[c] = NULL;
    } else {
      current->data->markHDTAlternative();
      RootedTree *extractedT2 = hdt->extractAndGoBack(myTree->factory);
      extractedVersions[c] = extractedT2->contract();
      delete extractedT2->factory;
    }
    c++; // Weee :)
  }
#endif
*/

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
/*
#ifdef doExtractAndContract
    // Notice no recoloring here... It's not neccesary as it is extracted and contracted away,
    // and will actually cause an error if called with firstChild->colorSubtree(0) as myRef is linked
    // to a non-existing hdt (as we just deleted it) (we could wait with deleting it, but as we don't need the coloring why bother)
    delete hdt->factory;
#else */
    firstChild->colorSubtree(0);
/*
#endif */
  } else {
/*      
#ifdef doExtractAndContract
    bool hdtTooBig = firstChild->n * CONTRACT_MAX_EXTRA_SIZE < hdt->leafCount();
    if (hdtTooBig) {
      HDT *newHDT;
      
      firstChild->markHDTAlternative();
      RootedTree *extractedT2 = hdt->extractAndGoBack(myTree->factory);
      RootedTree *contractedT2 = extractedT2->contract();
      delete extractedT2->factory;
      newHDT = HDT::constructHDT(contractedT2, myTree->maxDegree, dummyHDTFactory, true);
      delete contractedT2->factory;
      delete hdt->factory;
      hdt = newHDT;
    }
#endif */
    count(firstChild);
    // HDT is deleted in recursive call!
  }
  
  // Color 1 and recurse
  c = 0;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
/*
#ifdef doExtractAndContract
      hdt = HDT::constructHDT(extractedVersions[c], myTree->maxDegree, dummyHDTFactory, true);
      delete extractedVersions[c]->factory;
#endif */
      
      current->data->colorSubtree(1);
      hdt->updateCounters();  
      tripCount->tR[current->data->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);
      
      count(current->data);
    c++; // Weee :)
    // HDT is deleted on recursive calls!
  }
/*  
#ifdef doExtractAndContract
  delete[] extractedVersions;
#endif */
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
