#ifndef SDD_h
#define SDD_h


#include "TH1D.h"
#include "TTree.h"

#include <string>


class SDD {

 public:

  static float volt2keV( int G, const std::string& var="vamp" );

  static TH1D* fillFromTree( TTree* tree, const std::string& histoName, const std::string& varName="vamp", int G=100, int nBins=200, float xMin=0., float xMax=4.5, float rescale=1. );

 private:

};


#endif
