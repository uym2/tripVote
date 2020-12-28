#include "TripletDistanceCalculator.h"
#include "QuartetDistanceCalculator.h"

#include <Python.h>
#include <vector>
#include "newick_parser.h"
#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
	return TRUE;
}
#endif

#ifdef _WIN32
	__declspec(dllexport)
#endif
    static PyObject *method_TripletDistance(PyObject *self,PyObject *args) {
        const char *nw1, *nw2; // two newick strings
        /* Parse arguments */
        if(!PyArg_ParseTuple(args, "ss", &nw1, &nw2)) {
            return NULL;
        }
        
        UnrootedTree *ut1 = NULL;
        UnrootedTree *ut2 = NULL;
        RootedTree *rt1 = NULL;
        RootedTree *rt2 = NULL;

        NewickParser parser;

        ut1 = parser.parseStr(nw1);
        ut2 = parser.parseStr(nw2);
        rt1 = ut1->convertToRootedTree(NULL);
        rt2 = ut2->convertToRootedTree(rt1->factory);

        TripletDistanceCalculator tripletCalc;
        INTTYPE_REST dtrpl = tripletCalc.calculateTripletDistance(rt1, rt2);
        PyObject* result = PyLong_FromLong(dtrpl);

        if (ut1 != NULL) delete ut1;
        if (ut2 != NULL) delete ut2;
        if (rt1 != NULL) delete rt1->factory;
        if (rt2 != NULL) delete rt2->factory;
        
        return result;    
    }

#ifdef _WIN32
	__declspec(dllexport)
#endif
    static PyObject *method_QuartetDistance(PyObject *self,PyObject *args) {
        const char *nw1, *nw2;
        /* Parse arguments */
        if(!PyArg_ParseTuple(args, "ss", &nw1, &nw2)) {
            return NULL;
        }
        
        UnrootedTree *ut1 = NULL;
        UnrootedTree *ut2 = NULL;
        
        NewickParser parser;

        ut1 = parser.parseStr(nw1);
        ut2 = parser.parseStr(nw2); 
        
        QuartetDistanceCalculator quartetCalc;
        INTTYPE_N4 dquart = quartetCalc.calculateQuartetDistance(ut1, ut2);
        PyObject* result = PyLong_FromLong(dquart);
        
        if (ut1 != NULL) delete ut1;
        if (ut2 != NULL) delete ut2;
        
        return result;    
    }

    static PyMethodDef TQMethods[] = {
        {"triplet_distance", method_TripletDistance, METH_VARARGS, "Compute triplet distance"},
        {"quartet_distance", method_QuartetDistance, METH_VARARGS, "Compute quartet distance"},
        {NULL, NULL, 0, NULL}
    };


    static struct PyModuleDef tqdistmodule = {
        PyModuleDef_HEAD_INIT,
        "tqdist",
        "Python interface for the tqdist C library",
        -1,
        TQMethods
    };


    PyMODINIT_FUNC PyInit_tqdist(void) {
        return PyModule_Create(&tqdistmodule);
    }
