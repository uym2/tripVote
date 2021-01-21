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
        //double dtrpl = tripletCalc.calculateTripletDistance(rt1, rt2);
        //double total = tripletCalc.get_totalNoTriplets();
        //PyObject* result = PyFloat_FromDouble(dtrpl/total);
        tripletCalc.calculateTripletDistance(rt1, rt1);
        double total = tripletCalc.get_resolvedTriplets();
        tripletCalc.calculateTripletDistance(rt1, rt2);
        double dtrpl = (total-tripletCalc.get_resolvedTriplets())/total;
        PyObject* result = PyFloat_FromDouble(dtrpl);

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
        //double dquart = quartetCalc.calculateQuartetDistance(ut1, ut2);
        //double total = quartetCalc.get_totalNoQuartets();
        //PyObject* result = PyFloat_FromDouble(dquart/total);
        
        quartetCalc.calculateQuartetDistance(ut1, ut1);
        double total = quartetCalc.get_resolvedQuartetsAgree() + quartetCalc.get_resolvedQuartetsAgreeDiag() + quartetCalc.get_resolvedQuartetsAgreeUpper(); 
        quartetCalc.calculateQuartetDistance(ut1, ut2);
        double agree = quartetCalc.get_resolvedQuartetsAgree() + quartetCalc.get_resolvedQuartetsAgreeDiag() + quartetCalc.get_resolvedQuartetsAgreeUpper(); 
        double dquart = (total-agree)/total;
        PyObject* result = PyFloat_FromDouble(dquart);
        
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
