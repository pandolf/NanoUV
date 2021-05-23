#include "../interface/Scan.h"

#include <iostream>
#include "TH1F.h"




Scan::Scan( int number, int firstRegion, int lastRegion ) {

  number_ = number;

  firstRegion_ = firstRegion;
  lastRegion_ = lastRegion;

  type_ = "";

}


void Scan::setType( const std::string& type ) {

  type_ = type;

}




int Scan::number() const {

  return number_;

}


int Scan::firstRegion() const {

  return firstRegion_;

}



int Scan::lastRegion() const {

  return lastRegion_;

}


std::string Scan::type() const {

  return type_;

}



std::string Scan::getXtitle() const {

  std::string xTitle = "";
  if( type_=="thetaScan"  ) xTitle = "Analyzer #theta [deg]";
  if( type_=="phiScan"    ) xTitle = "Analyzer #phi [deg]";
  if( type_=="energyScan" ) xTitle = "Electron kinetic energy [eV]";
  if( type_=="nexafs"     ) xTitle = "Photon energy [eV]";
  if( type_=="time"       ) xTitle = "Time [au]";

  return xTitle;

}


std::string Scan::getYtitle() const {

  std::string yTitle = "";

  if( type_=="thetaScan"  ) yTitle = "Counts";
  if( type_=="phiScan"    ) yTitle = "Counts";
  if( type_=="energyScan" ) yTitle = "Counts";
  if( type_=="nexafs"     ) yTitle = "I_{diode} [A]";
  if( type_=="time"       ) yTitle = "I_{diode} [A]";

  return yTitle;

}


int Scan::x_index() const {

  int x_index = (type_=="nexafs") ? 1 : 3;
  if( type_=="time" ) x_index = -1;

  return x_index;

}


std::string Scan::scanNumberText() const {

  std::string scanNumberText(Form("%d", number_));

  if( number_ < 1000 && number_ > 99 ) // three digits, add one zero
    scanNumberText = "0" + scanNumberText;
  else if( number_ < 100 && number_ > 9 ) // two digits, add two zeros
    scanNumberText = "00" + scanNumberText;
  else if( number_ < 10 ) // one digit, add three zeros
    scanNumberText = "000" + scanNumberText;

  return scanNumberText;

}

  
void Scan::recognizeScanType( const std::string& line ) {

  type_ = "";
  if( line=="SCAN TYPE: Kinetic Energy Electron analyzer " ) type_ = "energyScan";
  if( line=="SCAN TYPE: THETAA"                            ) type_ = "thetaScan";
  if( line=="SCAN TYPE: PHI_A"                             ) type_ = "phiScan";
  if( line=="SCAN TYPE: PHOTON ENERGY VS VAR_1 VS VAR_2"   ) type_ = "nexafs";
  if( line=="SCAN TYPE: TIME"                              ) type_ = "time";

  if( type_!="" )
    std::cout << "[Scan::recognizeScanType] Recognized scanType: " << type_ << std::endl;
  else
    std::cout << "[Scan::recognizeScanType] WARNING! No idea what type of scan this is!" << std::endl;

}
  
