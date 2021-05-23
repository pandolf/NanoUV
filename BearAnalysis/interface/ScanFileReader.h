#ifndef ScanFileReader_h
#define ScanFileReader_h


#include "../interface/Scan.h"

#include "TGraphErrors.h"
#include <fstream>



class ScanFileReader {

 public:

  ScanFileReader( int scanNumber, int firstRegion=-1, int lastRegion=-1 );



 private:

  Scan* scan_;


};


#endif
