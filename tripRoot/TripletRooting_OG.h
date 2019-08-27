#ifndef TRIPLET_ROOTING_OG_H
#define TRIPLET_ROOTING_OG_H

#include "int_stuff.h"
#include "hdt.h"
#include "hdt_factory.h"
#include "TripletCounter.h"
#include "TripletRooting.h"
#include <string>

#ifndef CONTRACT_MAX_EXTRA_SIZE
#define CONTRACT_MAX_EXTRA_SIZE 20000
#endif

class TripletRootingOG : public TripletRooting {
 public:
  TripletRootingOG(RootedTree *ref, RootedTree *tree, string ingroupLabel);
  bool pairing(); // will override this same function in TripletRooting 

 private:    
  RootedTree* __find_ingroup_clade__(RootedTree* tree);
  string ingroupLabel;      
};

#endif
