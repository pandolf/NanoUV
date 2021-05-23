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

  TGraphErrors* gr_I0() const;
  TGraphErrors* gr_Idrain() const;
  TGraphErrors* gr_scan() const;

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

  static TGraphErrors* getRatio( BearScan* s_num, BearScan* s_denom );
  static BearScan mergeScans( int scanNumber1, int scanNumber2 );
  void expandGraphs( BearScan* s2 );
  static void expandGraph( TGraphErrors* gr1, TGraphErrors* gr2 );



 private:

  int number_;
  int firstRegion_;
  int lastRegion_;
  std::string type_;

  TGraphErrors* gr_I0_;
  TGraphErrors* gr_Idrain_;
  TGraphErrors* gr_scan_;

};


#endif
