#include "MinVarCounter.h"

MinVarCounter::MinVarCounter(unsigned int N){
    var = new double [N];
    minVar = new double [N];
    xStar = new double [N];
    SI = new double [N];
    ST = new double [N];
}

MinVarCounter::~MinVarCounter(){
    delete [] var;
    delete [] minVar;
    delete [] xStar;
    delete [] SI;
    delete [] ST;
}
