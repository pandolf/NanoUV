#ifndef NanoUVCommon_h
#define NanoUVCommon_h

#include "TCanvas.h"
#include "TPaveText.h"
#include "TGraph.h"


#include <vector>


class NanoUVCommon {

 public:

  NanoUVCommon();
  ~NanoUVCommon();

  static void setStyle();

  static std::vector<int> colors();

  static void addNanoUVLabel( TCanvas* c1, int quadrant=4 );
  static void addNanoUVLabel( TCanvas* c1, float xmin, float ymin, float xmax, float ymax );

  static TPaveText* getNanoUVLabel( int quadrant );
  static TPaveText* getNanoUVLabel( float xmin, float ymin, float xmax, float ymax );

  static float integrateSignal( TGraph* graph );
  static float ampMaxSignal( TGraph* graph );

  static float getPedestal( TGraph* graph, int nPoints );
  static void removeTimeOffset( TGraph* graph );
  static void plotWaveformGraph( TGraph* graph, const std::string& saveName );
  static TGraph* getGraphFromFile( const std::string& fileName );

};

#endif
