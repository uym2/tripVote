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
   
    bool get_tripScore(RootedTree *myTree, TripletCounter* myCount, double M, string &myScore){
        double currScore = double(myCount->tripScore[myTree->idx])/M;
        myScore += myTree->name +  ":" + to_string(currScore) + ",";
        
        for(TemplatedLinkedList<RootedTree*> *i = myTree->children; i != NULL; i = i->next)
        {
            RootedTree *t = i->data; 
            get_tripScore(t, myCount, M, myScore);
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
	PyObject* result;
        
	if (tripRoot.find_optimal_root()){
		
		TripletCounter* myCount = tripRoot.tripCount;
		double M = double(myTree->n)*(myTree->n-1)*(myTree->n-2)/6;
		
		string myScore;
		get_tripScore(myTree, myCount, M, myScore);
		myScore.pop_back(); // remove the last comma which is redundant */
		
		result = PyUnicode_FromString(myScore.c_str());

	} else {
		result = PyUnicode_FromString("");
	}	
	delete factory;
	return result;   
    }
   
   
    bool get_mvScore(RootedTree *myTree, MinVarCounter* myCount, double n, string &myScore){
        double mv = myCount->minVar[myTree->idx];
        double x_p = myCount->xStar[myTree->idx]; // distance to the parent
        double x_c = myTree->edge_length - x_p; // distance to the child
        double n_p, n_c;
        n_c = myTree->n;
        n_p = n - n_c;
        double avg = (myCount->ST[myTree->idx]+n_c*x_c-n_p*x_p)/n;
        myScore += myTree->name +  ":" + to_string(mv) + ":" + to_string(avg) + ",";
        
        for(TemplatedLinkedList<RootedTree*> *i = myTree->children; i != NULL; i = i->next)
        {
            RootedTree *t = i->data; 
            get_mvScore(t, myCount, n, myScore);
        } 
        return true;
    }
     
    static PyObject *method_MVScore(PyObject *self, PyObject *args) {
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
	
        PyObject* result;
            
        mvRoot.compute_score();
            
        MinVarCounter* myCount = mvRoot.mvCount;
        double n = double(myTree->n);
        
        string myScore;
        get_mvScore(myTree, myCount, n, myScore);
        myScore.pop_back(); // remove the last comma which is redundant */
        
        result = PyUnicode_FromString(myScore.c_str());

        delete factory;
        return result;   
    }
   
   
    
    static PyMethodDef tripRootMethods[] = {
        {"MVroot", method_MVRoot, METH_VARARGS, "MinVar rooting"},
        {"tripRootScore", method_tripRootScore, METH_VARARGS, "Triplet rooting"},
        {"minVarScore", method_MVScore, METH_VARARGS, "MinVar rooting score"},
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
