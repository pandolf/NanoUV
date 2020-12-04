#include "../interface/SDD.h"

#include <iostream>



float SDD::volt2keV( int G, const std::string& var ) {

  float k = 1.;

  if( var=="vamp" ) { // results from drawFe55

    if     ( G == 10  ) k = 60.3202;
    else if( G == 30  ) k = 19.7706;
    else if( G == 100 ) k = 6.39378;

  } else if( var=="vcharge" ) {

    if     ( G == 10  ) k = 0.00431204;
    else if( G == 30  ) k = 0.00136046;
    else if( G == 100 ) k = 0.000433016;

  } else {

    std::cout << "[SDD::calibration] Unkown var: '" << var << "'" << std::endl;
    k = 1.;

  }

  return k;

}



TH1D* SDD::fillFromTree( TTree* tree, const std::string& histoName, const std::string& varName, int G, int nBins, float xMin, float xMax, float rescale ) {

  TH1D* h1 = new TH1D( histoName.c_str(), "", nBins, xMin, xMax );

  float var;
  tree->SetBranchAddress( varName.c_str(), &var );

  int nEntries = tree->GetEntries();

  for( unsigned iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );

    float value = var*SDD::volt2keV(G, varName)*rescale;
    if( value < 0. ) value = 0.;

    h1->Fill( value );

  } // for entries

  return h1;

}


