#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>



ScanFileReader::ScanfileReader( int scanNumber ) {

  gr_mirrorCurrent_ = new TGraphErrors(0);
  gr_drainCurrent_ = new TGraphErrors(0);
  gr_scan_ = new TGraphErrors(0);

  gr_mirrorCurrent_->SetName( Form("mirrorCurrent_%d", scanNumber) );
  gr_drainCurrent_->SetName( Form("drainCurrent_%d", scanNumber) );
  gr_scan_->SetName( Form("scan_%d", scanNumber) );


  std::string suffix(Form("%d", scanNumber));

  if( scanNumber < 1000 && scanNumber > 99 ) // three digits, add one zero
    suffix = "0" + suffix;
  else if( scanNumber < 100 && scanNumber > 9 ) // two digits, add two zeros
    suffix = "00" + suffix;
  else if( scanNumber < 10 ) // one digit, add three zeros
    suffix = "000" + suffix;
  
  std::string fileName(Form("file(1)%s.txt");

  readFile(fileName);

}


void ScanFileReader::readFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  if( !(ifs.good()) ) {
    std::cout << "[ScanFileReader::readFile] ERROR! File '" << fileName << "' doesn't exist. Exiting." << std::endl;
    exit(1);
  }

     
  std::string line

  while( getline(ifs,line) ) {

    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

    std::string delimiter = " ";
    size_t pos = 0;
    std::vector<std::string> words;
    std::string word;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      word = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());
      words.push_back(word);
    }

    if( words[0]=="SCAN" && words[1]=="TYPE:" ) {

      if( line=="SCAN TYPE: Kinetic Energy Electron analyzer " ) scanType_ = "energyScan";
      if( line=="SCAN TYPE: THETAA " ) scanType_ = "energyScan";

      std::cout << "[ScanFileReader::readFile] Recognized scantype: " << scanType_ << std::endl;

    } // if scanType line

  } // while getLine

}
