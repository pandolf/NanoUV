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
