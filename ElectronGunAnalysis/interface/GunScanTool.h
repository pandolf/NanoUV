#ifndef GunScanTool_h
#define GunScanTool_h

#include <string>

#include "TGraphErrors.h"


class GunScanTool {

 public:

  GunScanTool( float gunEnergy, float APDhv, const std::string& data = "2020_01" );
  ~GunScanTool() {};

  void set_gunEnergy( float gunEnergy );
  void set_APDhv( float APDhv );
  void set_currentMethod( const std::string& currentMethod );
  void set_data( const std::string& data );
  void set_firstN_fit( int firstN_fit );
  void set_lastN_fit( int lastN_fit );
  void set_baselineFunc( const std::string& baselineFunc );

  float gunEnergy() const;
  float APDhv() const;
  std::string currentMethod() const;
  std::string data() const;
  int firstN_fit() const;
  int lastN_fit() const;
  std::string baselineFunc() const;


  TGraph* getScanFromFile( const std::string& fileName );
  TGraph* getCorrectedGraph( TGraph* graph, TF1* baseline );

  float getCurrentFromScan( TGraph* graph );

  void addPointToGraph( TGraphErrors* graph, const std::string& fileName, float iGunBefore, float iGunAfter );

  // handy:
  float getXmax( TGraph* graph );
  float getYmax( TGraph* graph );

 private:

  TF1* fitDrift( TGraph* graph );

  float gunEnergy_;
  float APDhv_;

  std::string currentMethod_;

  std::string data_;

  int firstN_fit_;
  int lastN_fit_;
  std::string baselineFunc_;

};



#endif
