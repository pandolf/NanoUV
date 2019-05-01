#ifndef NanoUVCommon_h
#define NanoUVCommon_h

#include "TCanvas.h"
#include "TPaveText.h"


#include <vector>


class NanoUVCommon {

 public:

  NanoUVCommon();
  ~NanoUVCommon();

  static void setStyle();

  static std::vector<int> colors();

  static void addNanoUVLabel( TCanvas* c1, int quadrant=4 );
  static void addNanoUVLabel( TCanvas* c1, float xmin, float ymin, float xmax, float ymax );

  static TPaveText* getNanoUVLabel( float xmin, float ymin, float xmax, float ymax );


};

#endif
