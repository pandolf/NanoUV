#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"



ScanFileReader::ScanFileReader( int scanNumber, int firstRegion, int lastRegion ) {

  scan_ = new Scan( scanNumber, firstRegion, lastRegion );

}


