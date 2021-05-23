#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>


int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc<2 ) {
    std::cout << "USAGE: ./drawScan [scanNumber] [firstRegion=-1] [lastRegion=-1]" << std::endl;
    exit(1);
  }

  int scanNumber( atoi(argv[1]) );
  int firstRegion = (argc>2) ? atoi(argv[2]) : -1;
  int lastRegion  = (argc>3) ? atoi(argv[3]) : -1;

  BearScan s(scanNumber, firstRegion, lastRegion);
  s.drawGraphs();

  return 0;

}
