#include "MinVarCounter.h"

MinVarCounter::MinVarCounter(unsigned int N){
    var = new double [N];
    minVar = new double [N];
    xStar = new double [N];
    SI = new double [N];
    ST = new double [N];
    
    for (int i = 0; i < N; i++){
        this->var[i] = 0;
        this->minVar[i] = 0;
        this->xStar[i] = 0;
        this->SI[i] = 0;
        this->ST[i] = true;
    }
}

MinVarCounter::~MinVarCounter(){
    delete [] var;
    delete [] minVar;
    delete [] xStar;
    delete [] SI;
    delete [] ST;
}
