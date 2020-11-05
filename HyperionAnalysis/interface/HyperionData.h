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
  float p_before()      const { return p_before_; };
  float p_after()       const { return p_after_ ; };

  TGraphErrors* getGraphFromColumns( const std::string& name, int columnMeans, int columnErrors=-1 ) const;


 private:

  std::string picoamm_;
  std::string led_;
  float p_before_;
  float p_after_;

  std::vector< std::vector<float> > data_; // each column is a separate vector

};


#endif
