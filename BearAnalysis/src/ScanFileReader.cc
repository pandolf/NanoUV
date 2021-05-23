#include "../interface/ScanFileReader.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"



ScanFileReader::ScanFileReader( int scanNumber, int firstRegion, int lastRegion ) {

  std::cout << "[ScanFileReader] Starting scan N." << scanNumber << std::endl;

  gr_mirror_ = new TGraphErrors(0);
  gr_drain_ = new TGraphErrors(0);
  gr_scan_ = new TGraphErrors(0);

  gr_mirror_->SetName( Form("mirror") );
  gr_drain_ ->SetName( Form("drain") );
  gr_scan_  ->SetName( Form("scan") );

  scan_ = new Scan( scanNumber, firstRegion, lastRegion );

  if( firstRegion==-1 && lastRegion==-1 )
    this->readSingleScan();
  else
    this->readRegionsScan();

}


void ScanFileReader::readSingleScan() {
  
  std::string fileAver(Form("../data/file(aver)%s.txt", scan_->scanNumberText().c_str()));
  std::ifstream ifsAver(fileAver.c_str());

  if( ifsAver.good() ) {

    std::cout << "[ScanFileReader::readSingleScan] Found average file: " << fileAver << std::endl;
    readFile( ifsAver );

  } else {

    std::string fileOne(Form("../data/file(1)%s.txt", scan_->scanNumberText().c_str()));
    std::cout << "[ScanFileReader::readSingleScan] Reading file: " << fileOne << std::endl;
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
  bool keithleyB = false;

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

      scan_->recognizeScanType( thisLine );

    } else if( thisLine=="Instrument 1: AMMETER KEITHLEY 6517A B" ) {

      keithleyB = true;

    } else if( thisLine=="Instrument 2: AMMETER KEITHLEY 6517A A" ) {

      keithleyB = false;

    } else if( keithleyB && thisLine=="Vsource: OFF" ) {

      std::cout << "[ScanFileReader::readFile] WARNING!!! Seems like KEITHLEY B has Vsource OFF!!! Aborting!" << std::endl;
      exit(13);

    } else if( keithleyB && thisLine=="Vsource: ON" ) {

      std::cout << "[ScanFileReader::readFile] Checked that KEITHLEY B has Vsource ON, as expected. Good." << std::endl;

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

        int x_index = scan_->x_index();
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



void ScanFileReader::readRegionsScan() {

  std::cout << std::endl;

  for( unsigned iRegion=scan_->firstRegion(); iRegion<=scan_->lastRegion(); ++iRegion ) {

    std::string fileName( Form( "../data/file(1)_Region %d__%s.txt", iRegion, scan_->scanNumberText().c_str() ) );
    std::cout << "[ScanFileReader::readRegionsScan] Adding file: " << fileName << std::endl;
    readFile( fileName );

  }

}




void ScanFileReader::drawGraphs() const {

  drawGraph( gr_mirror_, "I0"    , scan_->getXtitle(), "I_{0} [A]"       );
  drawGraph( gr_drain_ , "Idrain", scan_->getXtitle(), "I_{drain} [A]"   );
  drawGraph( gr_scan_  , "scan"  , scan_->getXtitle(), scan_->getYtitle() );

}



void ScanFileReader::drawGraph( TGraphErrors* graph, const std::string& name, const std::string& xTitle, const std::string& yTitle ) const {

  TCanvas* c1 = new TCanvas( Form("c1_%s", graph->GetName()), "", 600, 600 );
  c1->cd();

  float xMin(9999.), xMax(-9999.), yMin(9999.), yMax(-9999.);
  findPlotRanges( graph, xMin, xMax, yMin, yMax );

  if( yTitle=="Counts" ) {
    yMin = 0;
    yMax = 1.2*yMax;
  }

  graph->SetMarkerSize(0.8);
  graph->SetMarkerStyle(20);
  graph->SetMarkerColor(46);
  graph->SetLineColor(46);


  TH2D* h2_axes = new TH2D( Form("axes_%s", graph->GetName()), "", 10, xMin, xMax, 10, yMin, yMax );
  //TH2D* h2_axes = new TH2D( Form("axes_%s", graph->GetName()), "", 10, xMin, xMax, 10, yMin-0.1*fabs(yMin), yMax+0.2*fabs(yMax) );
  h2_axes->SetXTitle( xTitle.c_str() );
  h2_axes->SetYTitle( yTitle.c_str() );
  h2_axes->Draw();

  graph->Draw("PLsame");

  gPad->RedrawAxis();

  std::string regionText("");
  if( scan_->firstRegion()!=-1 && scan_->lastRegion()!=-1 ) regionText = std::string(Form("_regions_%d_%d", scan_->firstRegion(), scan_->lastRegion()));


  std::string plotFileName(Form("plots/%d%s_%s.pdf", scan_->number(), regionText.c_str(), name.c_str()) );
  
  c1->SaveAs( plotFileName.c_str() );
  system( "open ../plots/");
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


