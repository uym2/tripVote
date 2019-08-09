#include "counting_array.h"


CountingArray::CountingArray(unsigned int degree){
    this->degree = degree;
    this->n_i = new INTTYPE_REST[degree];
    this->n_i_circ = new INTTYPE_REST[degree];
    this->n_parent_ii = new INTTYPE_REST[degree];
    this->n_i_arrow_circ = new INTTYPE_REST[degree];

    for (int i = 0; i < degree; i++){
        this->n_i[i] = 0;
        this->n_i_circ[i] = 0;
        this->n_parent_ii[i] = 0;
        this->n_i_arrow_circ[i] = 0;
    }

    // for n_i_j, create lower triangular matrices; for easy indexing, create an empty pointer for index 0 and ignore it
    this->n_i_j = new INTTYPE_REST*[degree];
    this->n_i_j[0] = NULL;

    for (int i = 1; i < degree; i++){
        this->n_i_j[i] = new INTTYPE_REST[i];

        for (int j = 0; j < i; j++){
            this->n_i_j[i][j] = 0;
        }
    }

    // for n_i_arrow_j, we need to maintain a full matrix because it is not symmetric
    this->n_i_arrow_j = new INTTYPE_REST*[degree];
    
    for (int i = 0; i < degree; i++){
        this->n_i_arrow_j[i] = new INTTYPE_REST[i];

        for (int j = 0; j < degree; j++){
            if (j == i)
                this->n_i_arrow_j[i][j] = -1; // insist that the value is only valid if i != j
            else    
                this->n_i_arrow_j[i][j] = 0;
        }
    }
}

CountingArray::~CountingArray(){
    delete [] this->n_i;
    delete [] this->n_i_circ;
    delete [] this->n_parent_ii;
    delete [] this->n_i_arrow_circ;

    delete [] this->n_arrow_j[0];

    for (int i = 1; i < this->degree; i++){
        delete [] this->n_i_j[i];
        delete [] this->n_i_arrow_j[i];
    }
    delete [] this->n_i_j;
    delete [] this->n_i_arrow_j;  
}

INTTYPE_REST CountingArray::get_n_ij(unsigned int i, unsigned int j){
    unsigned int i_star, j_star;
    if ( i < 0 || i >= this->degree || j < 0 || j >= this->degree || i == j){
        return -1; // invalid case!
    }
    if (i > j){
        i_star = i;
        j_star = j;
    } else {
        i_star = j;
        j_star = i;
    }

    return this->n_i_j[i_star][j_star];
}
INTTYPE_REST CountingArray::get_n_arrow_ij(unsigned int i, unsigned int j){
    if ( i >= this->degree || j >= this->degree || i == j){
        return -1; // invalid case!
    }
    
    return this->n_i_arrow_j[i][j];
}
        
bool CountingArray::set_n_ij(unsigned int i, unsigned int j, INTTYPE v){
    if ( i < 0 || i >= this->degree || j < 0 || j >= this->degree || i == j){
        return false; // invalid case!
    }
    
    unsigned int i_star, j_star;
    if (i > j){
        i_star = i;
        j_star = j;
    } else {
        i_star = j;
        j_star = i;
    }

    this->n_i_j[i_star][j_star] = v;
    return true; 
}

bool CountingArray::set_n_arrow_ij(unsigned int i, unsigned int j, INTTYPE v){
    if ( i < 0 || i >= this->degree || j < 0 || j >= this->degree || i == j){
        return false; // invalid case!
    }
    
    this->n_i_arrow_j[i][j] = v;
    return true; 
}
bool CountingArray::set_n_i(unsigned int i, INTTYPE v){
    if (i >= this->degree)
        return false;
    this->n_i[i] = v;
    return true;
}
bool CountingArray::set_n_i_circ(unsigned int i, INTTYPE v){
    if (i >= this->degree)
        return false;
    this->n_i_circ[i] = v;
    return true;
}
bool CountingArray::set_n_parent_ii(unsigned int i, INTTYPE v){
    if (i >= this->degree)
        return false;
    this->n_parent_ii[i] = v;
    return true;
}
bool CountingArray::set_n_i_arrow_circ(unsigned int i, INTTYPE v){
    if (i >= this->degree)
        return false;
    this->n_i_arrow_circ[i] = v;
    return true;
}

INTTYPE_REST CountingArray::get_n_i(unsigned int i)
{
    if ( i >= this->degree)
        return -1;
    return this->n_i[i];    
}

INTTYPE_REST CountingArray::get_n_i_circ(unsigned int i)
{
    if ( i >= this->degree)
        return -1;
    return this->n_i_circ[i];    
}

INTTYPE_REST CountingArray::get_n_parent_ii(unsigned int i)
{
    if ( i >= this->degree)
        return -1;
    return this->n_parent_ii[i];    
}

INTTYPE_REST get_n_i_arrow_circ(unsigned int i){
{
    if ( i >= this->degree)
        return -1;
    return this->n_i_arrow_circ[i];    
}
