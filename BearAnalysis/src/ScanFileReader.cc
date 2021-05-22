#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "TFile.h"



ScanFileReader::ScanFileReader( int scanNumber ) {

  std::cout << "[ScanFileReader] Starting scan N." << scanNumber << std::endl;

  gr_mirror_ = new TGraphErrors(0);
  gr_drain_ = new TGraphErrors(0);
  gr_scan_ = new TGraphErrors(0);

  gr_mirror_->SetName( Form("mirror") );
  gr_drain_ ->SetName( Form("drain") );
  gr_scan_  ->SetName( Form("scan") );


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
  int foundLineLines = 0;
  bool readyToRead = false;

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


    if( words.size()>1 && words[0]=="SCAN" && words[1]=="TYPE:" ) {

      if( thisLine=="SCAN TYPE: Kinetic Energy Electron analyzer " ) scanType_ = "energyScan";
      if( thisLine=="SCAN TYPE: THETAA" ) scanType_ = "thetaScan";

      std::cout << "[ScanFileReader::readFile] Recognized scantype: " << scanType_ << std::endl;

    } else if( thisLine=="________________________________________________________________________________" ) {

      foundLineLines += 1;

      if( foundLineLines==2 ) 
        readyToRead = true;


    } else if( readyToRead ) {

      std::string delimiter = "\t";  // numbers are spaced with tabs
      size_t pos = 0;
      std::vector<std::string> numbers;
      std::string number;
      while ((pos = thisLine.find(delimiter)) != std::string::npos) {
        number = thisLine.substr(0, pos);
        thisLine.erase(0, pos + delimiter.length());
        numbers.push_back(number);
      }


      if( numbers.size() > 5 ) {

        float x = atof( numbers[3].c_str() );
        float y_mirror = atof( numbers[5].c_str() );
        float y_drain  = atof( numbers[4].c_str() );
        float y_scan   = atof( numbers[6].c_str() );

        gr_mirror_->SetPoint( gr_mirror_->GetN(), x, y_mirror );
        gr_drain_ ->SetPoint( gr_drain_ ->GetN(), x, y_drain  );
        gr_scan_  ->SetPoint( gr_scan_  ->GetN(), x, y_scan   );

      } // if numbers size

    } // if readyToRead

  } // while getLine

  TFile* file = TFile::Open( "test.root", "recreate" );
  file->cd();
  gr_mirror_->Write();
  gr_drain_ ->Write();
  gr_scan_  ->Write();
  file->Close();
  

}

