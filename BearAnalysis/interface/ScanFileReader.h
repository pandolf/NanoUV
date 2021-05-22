#ifndef ScanFileReader_h
#define ScanFileReader_h


#include "TGraphErrors.h"


class ScanFileReader {

 public:

  ScanFileReader( int scanNumber );

  void readFile( const std::string& fileName );

  std::string scanType() const;

  void drawGraphs() const;
  void drawGraph( TGraphErrors* graph, const std::string& name, const std::string& yTitle, const std::string& xTitle ) const;

  std::string getXtitle() const;
  void findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const;


 private:

  int scanNumber_;
  std::string scanType_;

  TGraphErrors* gr_mirror_;
  TGraphErrors* gr_drain_;
  TGraphErrors* gr_scan_;


};


#endif
