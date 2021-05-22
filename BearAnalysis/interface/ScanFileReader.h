#ifndef ScanFileReader_h
#define ScanFileReader_h


#include "TGraphErrors.h"


class ScanFileReader {

 public:

  ScanFileReader( int scanNumber );

  void readFile( const std::string& fileName );

  std::string scanType() const;




 private:

  std::string scanType_;

  TGraphErrors* gr_mirror_;
  TGraphErrors* gr_drain_;
  TGraphErrors* gr_scan_;


};


#endif
