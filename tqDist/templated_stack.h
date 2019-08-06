#ifndef TEMPLATED_STACK
#define TEMPLATED_STACK

#include "templated_linked_list.h"

template <class dataType>
class TemplatedStack
{
    private:
        TemplatedLinkedList<dataType> *stack;
    public:
        inline TemplatedStack(){
            this->stack = NULL;
        }
        inline ~TemplatedStack(){
            TemplatedLinkedList<dataType> *curr = this->stack;
            while (curr != NULL){
                TemplatedLinkedList<dataType> *tobeDel;
                curr = curr->next;
                delete tobeDel;
            }
        }
        inline bool push(dataType newData){
            TemplatedLinkedList<dataType> *newNode = new TemplatedLinkedList<dataType>;
            newNode->data = newData;
            newNode->next = this->stack;
            this->stack = newNode;

            return true;
        }
        inline bool isEmpty() { return this->stack == NULL; }
        inline dataType pop(){
            dataType p = this->stack->data;
            if (this->stack != NULL)
                this->stack = this->stack->next;
            return p;    
        }
};

#endif
