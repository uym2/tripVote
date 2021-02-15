#include <Python.h>
#include <string>
#include "rooted_tree.h"
#include "rooted_tree_factory.h"
#include "MVRooting.h"
#include "TripletRooting.h"

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
	return TRUE;
}
#endif

#ifdef _WIN32
	__declspec(dllexport)
#endif
    static PyObject *method_MVRoot(PyObject *self, PyObject *args) {
        const char *nwstr; // two newick strings
        /* Parse arguments */
        if(!PyArg_ParseTuple(args, "s", &nwstr)) {
            return NULL;
        }
        RootedTreeFactory *factory = new RootedTreeFactory();
        RootedTree *myTree = factory->getRootedTree();
        myTree->factory = factory;
        myTree->read_newick_str(nwstr);    
        myTree->countChildren();
        myTree->count_nodes();
        myTree->set_all_idx(0);
        
        MVRooting mvRoot;
        mvRoot.initialize(myTree);
        RootedTree *rerooted = mvRoot.root_tree();
       
        string outnwk = rerooted->toString();
        
        PyObject* result;
        result = PyUnicode_FromString(outnwk.c_str());

        delete factory;
         
        return result;    
    }
   
    bool get_myScore(RootedTree *myTree, TripletCounter* myCount, double M, string &myScore){
        double currScore = double(myCount->tripScore[myTree->idx])/M;
        myScore += myTree->name +  ":" + to_string(currScore) + ",";
        
        for(TemplatedLinkedList<RootedTree*> *i = myTree->children; i != NULL; i = i->next)
        {
            RootedTree *t = i->data; 
            get_myScore(t, myCount, M, myScore);
        } 
        return true;
    }
     
    static PyObject *method_tripRootScore(PyObject *self, PyObject *args) {
        const char *refnw, *treenw; // two newick strings
        /* Parse arguments */
        if(!PyArg_ParseTuple(args, "ss", &refnw, &treenw)) {
            return NULL;
        }
        RootedTreeFactory *factory = new RootedTreeFactory();
        RootedTree *refTree = factory->getRootedTree();
        refTree->factory = factory;
        refTree->read_newick_str(refnw);    
        refTree->countChildren();
        
        RootedTree *myTree = factory->getRootedTree();
        myTree->factory = factory;
        myTree->read_newick_str(treenw);    
        myTree->set_all_idx(0);
        myTree->count_nodes();
        myTree->countChildren();
    
        TripletRooting tripRoot;
        tripRoot.initialize(refTree,myTree);
        tripRoot.find_optimal_root();
        
        TripletCounter* myCount = tripRoot.tripCount;
        //double M = double(refTree->n)*(refTree->n-1)*(refTree->n-2)/6;
        double M = double(myTree->n)*(myTree->n-1)*(myTree->n-2)/6;
        
        string myScore;
        get_myScore(myTree, myCount, M, myScore);
        myScore.pop_back(); // remove the last comma which is redundant
        
        PyObject* result;
        result = PyUnicode_FromString(myScore.c_str());

        delete factory;
         
        return result;    
    }
    
    static PyMethodDef tripRootMethods[] = {
        {"MVroot", method_MVRoot, METH_VARARGS, "MinVar rooting"},
        {"tripRootScore", method_tripRootScore, METH_VARARGS, "Triplet rooting"},
        {NULL, NULL, 0, NULL}
    };


    static struct PyModuleDef triprootmodule = {
        PyModuleDef_HEAD_INIT,
        "triproot",
        "Python interface for the triplet rooting C library",
        -1,
        tripRootMethods
    };


    PyMODINIT_FUNC PyInit_triproot(void) {
        return PyModule_Create(&triprootmodule);
    }
