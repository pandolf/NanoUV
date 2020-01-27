#include "../interface/GunScanTool.h"



GunScanTool::GunScanTool( float gunEnergy, float APDhv, const std::string& data ) {

  gunEnergy_ = gunEnergy;
  APDhv_ = APDhv;
  data_ = data;
  currentMethod_ = "max";

}


void GunScanTool::set_gunEnergy( float gunEnergy ) {

  gunEnergy_ = gunEnergy;

}


void GunScanTool::set_APDhv( float APDhv ) {

  APDhv_ = APDhv;

}


void GunScanTool::set_currentMethod( const std::string& currentMethod ) {

  currentMethod_ = currentMethod;

}

void GunScanTool::set_data( const std::string& data ) {

  data_ = data;

}


float gunEnergy() const {

  return gunEnergy_;

}


float APDhv() const {

  return APDhv_;

}


std::string currentMethod() const {

  return currentMethod_;

}


std::string data() const {

  return data_;

}


