#include "../interface/ScanFileReader.h"
#include "interface/NanoUVCommon.h"

#include <iostream>


int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc<2 ) {
    std::cout << "USAGE: ./drawScan [scanNumber]" << std::endl;
    exit(1);
  }

  int scanNumber( atoi(argv[1]) );

  ScanFileReader sfr(scanNumber);

  return 0;

}
