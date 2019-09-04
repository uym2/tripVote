#include "RefTreeGen.h"

vector<string> RefTreeGen::list_leaves(RootedTree* aTree){
    vector<string> myLeaves;
    this->__list_leaves__(aTree, myLeaves);
    return myLeaves;
}

void RefTreeGen::__list_leaves__(RootedTree* aTree, vector<string> & myLeaves){
    if (aTree->isLeaf())
        myLeaves.push_back(aTree->name);
    else{
        for (TemplatedLinkedList<RootedTree*> *i = aTree->children; i != NULL; i = i->next){
            this->__list_leaves__(i->data, myLeaves);
        }

    }
}

void RefTreeGen::__collapse_tree__(RootedTree* aTree){
    vector<string> myLeaves = this->list_leaves(aTree);

    // remove all the current children
    for (TemplatedLinkedList<RootedTree*> *i = aTree->children; i != NULL; i = i->next){
        i->data->parent = NULL;
    }

    aTree->children = NULL;

    for (std::vector<string>::iterator it = myLeaves.begin(); it != myLeaves.end(); it++){
        RootedTree* newLeaf = aTree->factory->getRootedTree(*it);
        aTree->addChild(newLeaf);
    }
}

RootedTree* RefTreeGen::collapse_from_depth(RootedTree *aTree, unsigned int maxDepth){
    RootedTree* myTree = aTree;
    vector<RootedTree*> myNodes;

    this->__list_nodes_with_depth__(myTree, maxDepth, myNodes, 0);
    cout << "Found " << myNodes.size() << " nodes to be collapsed!" << endl;
    for (std::vector<RootedTree*>::iterator it = myNodes.begin(); it != myNodes.end(); it++){
        this->__collapse_tree__(*it);
        cout << "Collapsing one node ..." << endl;
    }

    return myTree;
}

void RefTreeGen::__list_nodes_with_depth__(RootedTree* aTree, unsigned int depth, vector<RootedTree*> & myNodes, unsigned int curr_depth){
    if (curr_depth == depth){
       if (!aTree->isLeaf())
           myNodes.push_back(aTree);
    }
    else{
       for (TemplatedLinkedList<RootedTree*> *i = aTree->children; i != NULL; i = i->next)
           this->__list_nodes_with_depth__(i->data, depth, myNodes, curr_depth+1); 
    }
}
