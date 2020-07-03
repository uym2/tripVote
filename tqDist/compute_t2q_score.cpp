#include "QuartetDistanceCalculator.h"
#include "TripletDistanceCalculator.h"
#include "int_stuff.h"
#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <input filename> [output filename]" << std::endl
	    << std::endl;
  std::cout << "Where:" << std::endl
	    << "\t" << "<input filename> is the name of a file containing multiple trees in" << std::endl 
	    << "\t" << "Newick format. Each tree should be on a seperate line." << std::endl
        << std::cout << "\t" << "If [output filename] is specified the output is written to the file"   << std::endl
	    << "\t" << "pointed to (if the file already exists the current content is deleted" << std::endl
	    << "\t" << "first), otherwise the output is written to stdout." << std::endl;
  std::cout << "\t" << "Each line i in the output is a number showing t2q score of tree ith."  << std::endl;
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }

  char *inFilename = argv[1];
  char *outFilename = NULL;
  if(argc > 2) 
    outFilename = argv[2];

  QuartetDistanceCalculator quartetCalc;
  std::vector<std::vector<double> > quart_mtrx = quartetCalc.calculateAllPairsQuartetDistance(inFilename,true);
  
  TripletDistanceCalculator tripletCalc;
  std::vector<std::vector<double> > trpl_mtrx = tripletCalc.calculateAllPairsTripletDistance(inFilename,true);

  unsigned int k = trpl_mtrx.size();
  cout << "Number of trees: " << k << endl;
  std::vector<double> t2q(k);  

  for (unsigned int i=0; i<k; i++){
    double s = 0;  
    for (unsigned int  j=0; j<k; j++){
        double t,q;
        if (j < i){
            t = 1-trpl_mtrx[i][j];
            q = 1-quart_mtrx[i][j];
            s += log(t/q);
        }
        else if (j > i){
            t = 1-trpl_mtrx[j][i];
            q = 1-quart_mtrx[j][i];
            s += log(t/q);
        }
    }
    t2q[i] = s;
  }

  std::ofstream fout;
  if(outFilename != NULL)
    fout.open(outFilename);
  std::ostream &out = outFilename != NULL ? fout : std::cout; // redirect to std out if no ouput filename given

  for(std::vector<double>::const_iterator it = t2q.begin(); it != t2q.end(); ++it) {
      out << (*it) << std::endl;
  }

  return 0;
}
