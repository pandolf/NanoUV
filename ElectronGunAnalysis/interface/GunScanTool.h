#ifndef GunScanTool_h
#define GunScanTool_h

#include <string>
#include "TGraph.h"


class GunScanTool {

 public:

  GunScanTool( float gunEnergy, float APDhv, const std::string& data = "2020_01" );
  ~GunScanTool() {};

  void set_gunEnergy( float gunEnergy );
  void set_APDhv( float APDhv );
  void set_currentMethod( const std::string& currentMethod );
  void set_data( const std::string& data );

  float gunEnergy() const;
  float APDhv() const;
  std::string currentMethod() const;
  std::string data() const;

  TGraph* getScanFromFile( const std::string& fileName, float gunEnergy );
  TGraph* getCorrectedGraph( TGraph* graph );

  float getCurrentFromScan( TGraph* graph );


 private:

  TF1* fitDrift( TGraph* graph, int firstN=7, int lastN=5 );

  float gunEnergy_;
  float APDhv_;

  std::string currentMethod_;

  std::string data_;

};



#endif
