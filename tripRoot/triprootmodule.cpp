#include <Python.h>
#include <string>
#include "rooted_tree.h"
#include "rooted_tree_factory.h"
#include "MVRooting.h"

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
    
    static PyMethodDef tripRootMethods[] = {
        {"MVroot", method_MVRoot, METH_VARARGS, "MinVar rooting"},
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
