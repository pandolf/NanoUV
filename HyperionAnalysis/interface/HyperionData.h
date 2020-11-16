#ifndef HyperionData_h
#define HyperionData_h


#include <string>
#include <vector>

#include "TGraphErrors.h"


class HyperionData {


 public:

  HyperionData( const std::string& fileName );

  std::string picoamm() const { return picoamm_ ; };
  std::string led()     const { return led_     ; };
  float led_freq()      const { return led_freq_; };
  float p_before()      const { return p_before_; };
  float p_after()       const { return p_after_ ; };
  float p()             const { return 0.5*(p_after_+p_before_) ; };
  float L()             const { return L_; };

  TGraphErrors* getGraphFromColumns( const std::string& name, int columnMeans, int columnErrors=-1, int nMeasurements=1000 ) const;


 private:

  std::string picoamm_; // "keithley" or "keysight"
  std::string led_; // "SP6505" (UV) or "SP6501" (visible) or "off"
  float led_freq_; // in kHz
  float p_before_; // in mbar
  float p_after_;  // in mbar
  float L_;        // in mm

  std::vector< std::vector<float> > data_; // each column is a separate vector

};


#endif
