#ifndef ScanFileReader_h
#define ScanFileReader_h


#include "TGraphErrors.h"
#include <fstream>

class ScanFileReader {

 public:

  ScanFileReader( int scanNumber );

  void readSingleScan();
  void readFile( const std::string& fileName );
  void readFile( std::ifstream& ifs );

  void readRegionsScan( int firstRegion, int lastRegion );

  std::string scanType() const;

  void drawGraphs( const std::string& yAxisName="Counts" ) const;
  void drawGraph( TGraphErrors* graph, const std::string& name, const std::string& yTitle, const std::string& xTitle ) const;

  std::string getXtitle() const;
  void findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const;

  std::string scanNumberText() const;

 private:

  int scanNumber_;
  std::string scanType_;

  int firstRegion_;
  int lastRegion_;

  TGraphErrors* gr_mirror_;
  TGraphErrors* gr_drain_;
  TGraphErrors* gr_scan_;


};


#endif
