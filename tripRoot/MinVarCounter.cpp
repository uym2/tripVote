#include "MinVarCounter.h"

MinVarCounter::MinVarCounter(unsigned int N){
    var = new float [N];
    minVar = new float [N];
    xStar = new float [N];
    SI = new float [N];
    ST = new float [N];
}

MinVarCounter::~MinVarCounter(){
    delete [] var;
    delete [] minVar;
    delete [] xStar;
    delete [] SI;
    delete [] ST;
}
