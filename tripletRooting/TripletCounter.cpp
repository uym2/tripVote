#include "TripletCounter.h"

TripletCounter::TripletCounter(unsigned int N){
    this->N = N;
    this->tI = new unsigned int [N];
    this->tO = new unsigned int [N];
    this->tR = new unsigned int [N];
    this->tA = new unsigned int [N];
}

TripletCounter::TripletCounter(){
    delete [] this->tI;
    delete [] this->tO;
    delete [] this->tR;
    delete [] this->tA;
}
