#ifndef GunScan_h
#define GunScan_h

#include <string>

#include "TGraphErrors.h"





class GunScan {


 public:

  GunScan( float gunEnergy, float APDhv, const std::string& data, const std::string& scanName, float iGunBefore, float iGunAfter=-1. );
  ~GunScan() {};

  void set_gunEnergy( float gunEnergy );
  void set_APDhv( float APDhv );
  void set_dataDir( const std::string& dataDir );
  void set_scanName( const std::string& scanName );
  void set_iGunBefore( float iGunBefore );
  void set_iGunAfter( float iGunAfter );
  void set_currentMethod( const std::string& currentMethod );
  void set_firstN_fit( int firstN_fit );
  void set_lastN_fit( int lastN_fit );
  void set_baselineFunc( const std::string& baselineFunc );

  float gunEnergy() const;
  float APDhv() const;
  std::string dataDir() const;
  std::string scanName() const;
  float iGunBefore() const;
  float iGunAfter() const;
  std::string currentMethod() const;
  int firstN_fit() const;
  int lastN_fit() const;
  std::string baselineFunc() const;

  std::string outdir() const;
  float gunCurrent() const;
  float gunCurrentError() const;
  std::string gunCurrentText() const;

  void loadScan();
  void correctGraph();

  static std::vector<GunScan*> loadScans( const std::string& scansFileName, float gunEnergy=-1., float APDhv=-1. ); // default = all

  void correctGraph( TF1* baseline );

  float getCurrentFromScan();

  float getBaseline( float x0 ); // baseline at gun position = x0 [mm]


  void addPointToGraph( TGraphErrors* graph );

  // handy:
  static float getXmax( TGraph* graph );
  static float getYmax( TGraph* graph );
  static float getStep( TGraph* graph );


 private:

  float gunEnergy_;
  float APDhv_;
  std::string dataDir_;
  std::string scanName_;
  float iGunBefore_;
  float iGunAfter_;

  TGraph* graph_;
  TGraph* graph_corr_;
  TF1* baseline_;

  std::string currentMethod_;
  int firstN_fit_;
  int lastN_fit_;
  std::string baselineFunc_;

  void fitDrift();
  void subtractBaseline();
  static std::string findDataDirFromPath( const std::string& scansFile );

};



#endif
