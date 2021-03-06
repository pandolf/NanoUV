#ifndef GunScan_h
#define GunScan_h

#include <string>

#include "TGraphErrors.h"





class GunScan {


 public:

  GunScan( float gunEnergy, float APDhv, const std::string& data, const std::string& scanName, float iGunBefore, float iGunAfter=-1., const std::string& baselineFunc="pol3" );
  ~GunScan() {};

  // setters
  void set_gunEnergy( float gunEnergy );
  void set_APDhv( float APDhv );
  void set_dataDir( const std::string& dataDir );
  void set_scanName( const std::string& scanName );
  void set_iGunBefore( float iGunBefore );
  void set_iGunAfter( float iGunAfter );
  void set_currentMethod( const std::string& currentMethod );
  void set_currentEvalPoint( float currentEvalPoint );
  void set_firstN_fit( int firstN_fit );
  void set_lastN_fit( int lastN_fit );
  void set_baselineFunc( const std::string& baselineFunc );

  // getters
  float gunEnergy() const;
  float APDhv() const;
  std::string dataDir() const;
  std::string scanName() const;
  float iGunBefore() const;
  float iGunAfter() const;
  std::string currentMethod() const;
  float currentEvalPoint() const;
  int firstN_fit() const;
  int lastN_fit() const;
  std::string baselineFunc() const;

  TGraph* graph() const;
  TGraph* graph_corr() const;
  TF1* baseline() const;

  // other functions
  
  std::string outdir() const;
  float gunCurrent() const;
  float gunCurrentError() const;
  std::string gunCurrentText() const;

  void loadScan();
  void correctGraph();

  void loadCurrentSyst();
  float getCurrentSyst() const;

  static std::vector<GunScan*> loadScans( const std::string& scansFileName, float gunEnergy=-1., float APDhv=-1., const std::string& baselineFunc="pol3" ); // default = all
  static void correctCurrentZero( float& current, const std::string& scansFile );

  float convertGunPosition( float x ) const { return x*201./gunEnergy_; }

  float getCurrentFromScan();
  float getCurrentFromScan( float& currentError );

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
  float currentEvalPoint_;
  int firstN_fit_;
  int lastN_fit_;
  std::string baselineFunc_;

  TGraphErrors* gr_currentSyst_;

  void fitDrift();
  void subtractBaseline();
  static std::string findDataDirFromPath( const std::string& scansFile );

};



#endif
