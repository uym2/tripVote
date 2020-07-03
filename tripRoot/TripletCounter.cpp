#include "TripletCounter.h"

TripletCounter::TripletCounter(unsigned int N){
    this->N = N;
    this->tI = new INTTYPE_REST [N];
    this->tO = new INTTYPE_REST [N];
    this->tR = new INTTYPE_REST [N];
    this->tripScore = new INTTYPE_REST [N];
    this->isActive = new bool [N];

    for (int i = 0; i < N; i++){
        this->tI[i] = 0;
        this->tO[i] = 0;
        this->tR[i] = 0;
        this->tripScore[i] = 0;
        this->isActive[i] = true;
    }
}

TripletCounter::~TripletCounter(){
    delete [] this->tI;
    delete [] this->tO;
    delete [] this->tR;
    delete [] this->tripScore;
    delete [] this->isActive;
}

void TripletCounter::erase(){
    delete [] this->tI;
    delete [] this->tO;
    delete [] this->tR;
    delete [] this->tripScore;
    delete [] this->isActive;
}
