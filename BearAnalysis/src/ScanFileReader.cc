#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>



ScanFileReader::ScanFileReader( int scanNumber ) {

  std::cout << "[ScanFileReader] Starting scan N." << scanNumber << std::endl;

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
  
  std::string fileName(Form("../data/file(1)%s.txt", suffix.c_str()));

  readFile(fileName);

}


void ScanFileReader::readFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  if( !(ifs.good()) ) {
    std::cout << "[ScanFileReader::readFile] ERROR! File '" << fileName << "' doesn't exist. Exiting." << std::endl;
    exit(1);
  }

     
  std::string line;

  while( getline(ifs,line) ) {

    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    std::string thisLine = line;

    std::string delimiter = " ";
    size_t pos = 0;
    std::vector<std::string> words;
    std::string word;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      word = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());
      words.push_back(word);
    }

    if( words.size() < 2 ) continue;

    if( words[0]=="SCAN" && words[1]=="TYPE:" ) {

      if( thisLine=="SCAN TYPE: Kinetic Energy Electron analyzer " ) scanType_ = "energyScan";
      if( thisLine=="SCAN TYPE: THETAA" ) scanType_ = "thetaScan";

      std::cout << "[ScanFileReader::readFile] Recognized scantype: " << scanType_ << std::endl;

    } // if scanType line

  } // while getLine

}
