#include "templated_stack.h"


/*
template <class dataType>
TemplatedStack<dataType>::TemplatedStack(){
    this->stack = NULL;
}

template <class dataType>
TemplatedStack<dataType>::~TemplatedStack<dataType>(){
    TemplatedLinkedList<dataType> *curr = this->stack;
    while (curr != NULL){
        TemplatedLinkedList<dataType> *tobeDel;
        curr = curr->next;
        delete tobeDel;
    }
}
*/

template <class dataType>
bool TemplatedStack<dataType>::push(dataType newData){
    TemplatedLinkedList<dataType> *newNode = new TemplatedLinkedList<dataType>;
    newNode->data = newData;
    newNode->next = this->stack;
    this->stack = newNode;

    return true;
}

template <class dataType>
dataType TemplatedStack<dataType>::pop(){
    dataType* p = this->stack;
    if (this->stack != NULL)
        this->stack = this->stack->next;
    return *p;    
}
