#include "../interface/ScanFileReader.h"
#include "interface/NanoUVCommon.h"

#include <iostream>


int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc<4 ) {
    std::cout << "USAGE: ./drawScanRegions [scanNumber] [firstRegion] [lastRegion]" << std::endl;
    exit(1);
  }

  int scanNumber ( atoi(argv[1]) );
  int firstRegion( atoi(argv[2]) );
  int lastRegion ( atoi(argv[3]) );

  ScanFileReader sfr(scanNumber);
  sfr.readRegionsScan(firstRegion, lastRegion);
  sfr.drawGraphs( "I_{diode} [A]" );

  return 0;

}
