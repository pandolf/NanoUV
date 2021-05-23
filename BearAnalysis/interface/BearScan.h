#ifndef BearScan_h
#define BearScan_h

#include <string>
#include "TGraphErrors.h"


class BearScan {

 public:

  BearScan( int number, int firstRegion=-1, int lastRegion=-1 );

  void setType( const std::string& type );

  int number() const;
  int firstRegion() const;
  int lastRegion() const;
  std::string type() const;

  std::string getXtitle() const;
  std::string getYtitle() const;

  int x_index() const;

  std::string scanNumberText() const;

  void readSingleScan();
  void readRegionsScan();

  void readFile( const std::string& fileName );
  void readFile( std::ifstream& ifs );

  void recognizeScanType( const std::string& line );

  void drawGraphs() const;
  void drawGraph( TGraphErrors* graph, const std::string& name, const std::string& xTitle, const std::string& yTitle ) const;

  void findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const;


 private:

  int number_;
  int firstRegion_;
  int lastRegion_;
  std::string type_;

  TGraphErrors* gr_mirror_;
  TGraphErrors* gr_drain_;
  TGraphErrors* gr_scan_;

};


#endif
