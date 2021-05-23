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

  static void addNanoUVLabel( TCanvas* c1, int quadrant=4, const std::string& text="NanoUV" );
  static void addNanoUVLabel( TCanvas* c1, float xmin, float ymin, float xmax, float ymax, const std::string& text="NanoUV" );

  static TPaveText* getNanoUVLabel( int quadrant, const std::string& text="NanoUV" );
  static TPaveText* getNanoUVLabel( float xmin, float ymin, float xmax, float ymax, const std::string& text="NanoUV" );

  static float integrateSignal( TGraph* graph, bool invertPolarity=false );
  static float ampMaxSignal( TGraph* graph, bool invertPolarity=false );

  static float getPedestal( TGraph* graph, int nPoints );
  static void removeTimeOffset( TGraph* graph );
  static void plotWaveformGraph( TGraph* graph, const std::string& saveName );
  static TGraph* getGraphFromFile( const std::string& fileName );

  static void findGraphRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax );

  static std::string scientific( float x, int decimals = 1 );

};

#endif
