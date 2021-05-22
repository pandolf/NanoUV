#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"



ScanFileReader::ScanFileReader( int scanNumber ) {

  scanNumber_ = scanNumber;

  std::cout << "[ScanFileReader] Starting scan N." << scanNumber_ << std::endl;

  gr_mirror_ = new TGraphErrors(0);
  gr_drain_ = new TGraphErrors(0);
  gr_scan_ = new TGraphErrors(0);

  gr_mirror_->SetName( Form("mirror") );
  gr_drain_ ->SetName( Form("drain") );
  gr_scan_  ->SetName( Form("scan") );


  std::string suffix(Form("%d", scanNumber_));

  if( scanNumber_ < 1000 && scanNumber_ > 99 ) // three digits, add one zero
    suffix = "0" + suffix;
  else if( scanNumber_ < 100 && scanNumber_ > 9 ) // two digits, add two zeros
    suffix = "00" + suffix;
  else if( scanNumber_ < 10 ) // one digit, add three zeros
    suffix = "000" + suffix;
  
  std::string fileAver(Form("../data/file(aver)%s.txt", suffix.c_str()));
  std::ifstream ifsAver(fileAver.c_str());

  if( ifsAver.good() ) {

    std::cout << "[ScanFileReader::readFile] Found average, filem will use it." << std::endl;
    readFile( ifsAver );

  } else {

    std::string fileOne(Form("../data/file(1)%s.txt", suffix.c_str()));
    std::ifstream ifsOne(fileOne.c_str());

    if( ifsOne.good() ) {

      readFile( ifsOne );

    } else {

      std::cout << "[ScanFileReader::readFile] ERROR! File '" << fileOne << "' doesn't exist. Exiting." << std::endl;
      exit(1);

    } // if ifsOne

  } // if ifsAver


  system("mkdir -p plots");

}




void ScanFileReader::readFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  if( ifs.good() ) {

    readFile( ifs );

  } else {

    std::cout << "[ScanFileReader::readFile] ERROR! File '" << fileName << "' doesn't exist. Exiting." << std::endl;
    exit(1);

  }

}



void ScanFileReader::readFile( std::ifstream& ifs ) {

     
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


}



std::string ScanFileReader::getXtitle() const {

  std::string xTitle;
  if( scanType_=="thetaScan"  ) xTitle = "Analyzer #theta [deg]";
  if( scanType_=="energyScan" ) xTitle = "Electron kinetic energy [eV]";

  return xTitle;

}




void ScanFileReader::drawGraphs() const {

  drawGraph( gr_mirror_, "I0", "I_{0} [A]", this->getXtitle() );
  drawGraph( gr_drain_ , "Idrain", "I_{drain} [A]", this->getXtitle() );
  drawGraph( gr_scan_  , "scan", "Counts", this->getXtitle() );

}



void ScanFileReader::drawGraph( TGraphErrors* graph, const std::string& name, const std::string& yTitle, const std::string& xTitle ) const {

  TCanvas* c1 = new TCanvas( Form("c1_%s", graph->GetName()), "", 600, 600 );
  c1->cd();

  float xMin(9999.), xMax(-9999.), yMin(0.), yMax(0.);
  findPlotRanges( graph, xMin, xMax, yMin, yMax );

  graph->SetMarkerSize(1.5); 
  graph->SetMarkerStyle(20);
  graph->SetMarkerColor(46);
  graph->SetLineColor(46);

  TH2D* h2_axes = new TH2D( Form("axes_%s", graph->GetName()), "", 10, xMin, xMax, 10, 0.9*yMin, 1.2*yMax );
  h2_axes->SetXTitle( xTitle.c_str() );
  h2_axes->SetYTitle( yTitle.c_str() );
  h2_axes->Draw();

  graph->Draw("Psame");

  gPad->RedrawAxis();

  std::string plotFileName(Form("plots/%d_%s.pdf", scanNumber_, name.c_str()) );
  c1->SaveAs( plotFileName.c_str() );
  if( name=="scan" ) system( Form("open %s", plotFileName.c_str()) );

  delete c1;
  delete h2_axes;

}


void ScanFileReader::findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const {

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    if( x<xMin ) xMin = x;
    if( x>xMax ) xMax = x;
    //if( y<yMin ) yMin = y; // keep yMin = 0
    if( y>yMax ) yMax = y;

  } // for points

}
