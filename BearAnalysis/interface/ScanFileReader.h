#ifndef ScanFileReader_h
#define ScanFileReader_h


#include "../interface/Scan.h"

#include "TGraphErrors.h"
#include <fstream>



class ScanFileReader {

 public:

  ScanFileReader( int scanNumber, int firstRegion=-1, int lastRegion=-1 );

  void readSingleScan();
  void readRegionsScan();

  void readFile( const std::string& fileName );
  void readFile( std::ifstream& ifs );

  void drawGraphs() const;
  void drawGraph( TGraphErrors* graph, const std::string& name, const std::string& xTitle, const std::string& yTitle ) const;

  void findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const;


 private:

  Scan* scan_;

  TGraphErrors* gr_mirror_;
  TGraphErrors* gr_drain_;
  TGraphErrors* gr_scan_;


};


#endif
