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

  firstRegion_ = -1;
  lastRegion_  = -1;

}


void ScanFileReader::readSingleScan() {
  
  std::string fileAver(Form("../data/file(aver)%s.txt", this->scanNumberText().c_str()));
  std::ifstream ifsAver(fileAver.c_str());

  if( ifsAver.good() ) {

    std::cout << "[ScanFileReader::readFile] Found average, filem will use it." << std::endl;
    readFile( ifsAver );

  } else {

    std::string fileOne(Form("../data/file(1)%s.txt", this->scanNumberText().c_str()));
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
      if( thisLine=="SCAN TYPE: THETAA"                            ) scanType_ = "thetaScan";
      if( thisLine=="SCAN TYPE: PHOTON ENERGY VS VAR_1 VS VAR_2"   ) scanType_ = "nexAFS";

      std::cout << "[ScanFileReader::readFile] Recognized scanType: " << scanType_ << std::endl;

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

        int x_index = (scanType_=="nexAFS") ? 1 : 3;
        float x = atof( numbers[x_index].c_str() );

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



void ScanFileReader::readRegionsScan( int firstRegion, int lastRegion ) {

  firstRegion_ = firstRegion;
  lastRegion_  = lastRegion;

  std::cout << std::endl;

  for( unsigned iRegion=firstRegion_; iRegion<=lastRegion_; ++iRegion ) {

    std::string fileName( Form( "../data/file(1)_Region %d__%s.txt", iRegion, this->scanNumberText().c_str() ) );
    std::cout << "[ScanFileReader::readRegionsScan] Adding file: " << fileName << std::endl;
    readFile( fileName );

  }

}



std::string ScanFileReader::getXtitle() const {

  std::string xTitle;
  if( scanType_=="thetaScan"  ) xTitle = "Analyzer #theta [deg]";
  if( scanType_=="energyScan" ) xTitle = "Electron kinetic energy [eV]";
  if( scanType_=="nexAFS"     ) xTitle = "Photon energy [eV]";

  return xTitle;

}




void ScanFileReader::drawGraphs( const std::string& yAxisName ) const {

  drawGraph( gr_mirror_, "I0", "I_{0} [A]", this->getXtitle() );
  drawGraph( gr_drain_ , "Idrain", "I_{drain} [A]", this->getXtitle() );
  drawGraph( gr_scan_  , "scan", yAxisName, this->getXtitle() );

}



void ScanFileReader::drawGraph( TGraphErrors* graph, const std::string& name, const std::string& yTitle, const std::string& xTitle ) const {

  TCanvas* c1 = new TCanvas( Form("c1_%s", graph->GetName()), "", 600, 600 );
  c1->cd();

  float xMin(9999.), xMax(-9999.), yMin(9999.), yMax(-9999.);
  findPlotRanges( graph, xMin, xMax, yMin, yMax );

  if( yTitle=="Counts" ) yMin = 0;

  graph->SetMarkerSize(1.3); 
  graph->SetMarkerStyle(20);
  graph->SetMarkerColor(46);
  graph->SetLineColor(46);

  TH2D* h2_axes = new TH2D( Form("axes_%s", graph->GetName()), "", 10, xMin, xMax, 10, yMin-0.1*fabs(yMin), yMax+0.2*fabs(yMax) );
  h2_axes->SetXTitle( xTitle.c_str() );
  h2_axes->SetYTitle( yTitle.c_str() );
  h2_axes->Draw();

  graph->Draw("Psame");

  gPad->RedrawAxis();

  std::string regionText("");
  if( firstRegion_!=-1 && lastRegion_!=-1 ) regionText = std::string(Form("_regions_%d_%d", firstRegion_, lastRegion_));


  std::string plotFileName(Form("plots/%d%s_%s.pdf", scanNumber_, regionText.c_str(), name.c_str()) );
  
  c1->SaveAs( plotFileName.c_str() );
  if( name=="scan" ) system( Form("open %s", plotFileName.c_str()) );

  delete c1;
  delete h2_axes;

}


void ScanFileReader::findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const {

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    if( iPoint==0 ) { // initialize
      xMin = x;
      xMax = x;
      yMin = y;
      yMax = y;
    }

    if( x<xMin ) xMin = x;
    if( x>xMax ) xMax = x;
    if( y<yMin ) yMin = y;
    if( y>yMax ) yMax = y;

  } // for points

}


std::string ScanFileReader::scanNumberText() const {

  std::string scanNumberText(Form("%d", scanNumber_));

  if( scanNumber_ < 1000 && scanNumber_ > 99 ) // three digits, add one zero
    scanNumberText = "0" + scanNumberText;
  else if( scanNumber_ < 100 && scanNumber_ > 9 ) // two digits, add two zeros
    scanNumberText = "00" + scanNumberText;
  else if( scanNumber_ < 10 ) // one digit, add three zeros
    scanNumberText = "000" + scanNumberText;

  return scanNumberText;

}

