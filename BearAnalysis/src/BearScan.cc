#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>
#include <fstream>
#include "TCanvas.h"
#include "TH2D.h"




BearScan::BearScan( int number, int firstRegion, int lastRegion ) {

  std::cout << "[BearScan] Creating scan N." << number << std::endl;

  number_ = number;

  firstRegion_ = firstRegion;
  lastRegion_ = lastRegion;

  type_ = "";

  gr_mirror_ = new TGraphErrors(0);
  gr_drain_ = new TGraphErrors(0);
  gr_scan_ = new TGraphErrors(0);

  gr_mirror_->SetName( Form("mirror") );
  gr_drain_ ->SetName( Form("drain") );
  gr_scan_  ->SetName( Form("scan") );

  if( firstRegion==-1 && lastRegion==-1 )
    this->readSingleScan();
  else
    this->readRegionsScan();

}


void BearScan::setType( const std::string& type ) {

  type_ = type;

}




int BearScan::number() const {

  return number_;

}


int BearScan::firstRegion() const {

  return firstRegion_;

}



int BearScan::lastRegion() const {

  return lastRegion_;

}


std::string BearScan::type() const {

  return type_;

}



std::string BearScan::getXtitle() const {

  std::string xTitle = "";
  if( type_=="thetaScan"  ) xTitle = "Analyzer #theta [deg]";
  if( type_=="phiScan"    ) xTitle = "Analyzer #phi [deg]";
  if( type_=="energyScan" ) xTitle = "Electron kinetic energy [eV]";
  if( type_=="nexafs"     ) xTitle = "Photon energy [eV]";
  if( type_=="time"       ) xTitle = "Time [au]";

  return xTitle;

}


std::string BearScan::getYtitle() const {

  std::string yTitle = "";

  if( type_=="thetaScan"  ) yTitle = "Counts";
  if( type_=="phiScan"    ) yTitle = "Counts";
  if( type_=="energyScan" ) yTitle = "Counts";
  if( type_=="nexafs"     ) yTitle = "I_{diode} [A]";
  if( type_=="time"       ) yTitle = "I_{diode} [A]";

  return yTitle;

}


int BearScan::x_index() const {

  int x_index = (type_=="nexafs") ? 1 : 3;
  if( type_=="time" ) x_index = -1;

  return x_index;

}


std::string BearScan::scanNumberText() const {

  std::string scanNumberText(Form("%d", number_));

  if( number_ < 1000 && number_ > 99 ) // three digits, add one zero
    scanNumberText = "0" + scanNumberText;
  else if( number_ < 100 && number_ > 9 ) // two digits, add two zeros
    scanNumberText = "00" + scanNumberText;
  else if( number_ < 10 ) // one digit, add three zeros
    scanNumberText = "000" + scanNumberText;

  return scanNumberText;

}

  
void BearScan::recognizeScanType( const std::string& line ) {

  type_ = "";
  if( line=="SCAN TYPE: Kinetic Energy Electron analyzer " ) type_ = "energyScan";
  if( line=="SCAN TYPE: THETAA"                            ) type_ = "thetaScan";
  if( line=="SCAN TYPE: PHI_A"                             ) type_ = "phiScan";
  if( line=="SCAN TYPE: PHOTON ENERGY VS VAR_1 VS VAR_2"   ) type_ = "nexafs";
  if( line=="SCAN TYPE: TIME"                              ) type_ = "time";

  if( type_!="" )
    std::cout << "[BearScan::recognizeScanType] Recognized scanType: " << type_ << std::endl;
  else
    std::cout << "[BearScan::recognizeScanType] WARNING! No idea what type of scan this is!" << std::endl;

}
  

void BearScan::readSingleScan() {
  
  std::string fileAver(Form("../data/file(aver)%s.txt", this->scanNumberText().c_str()));
  std::ifstream ifsAver(fileAver.c_str());

  if( ifsAver.good() ) {

    std::cout << "[BearScan::readSingleScan] Found average file: " << fileAver << std::endl;
    readFile( ifsAver );

  } else {

    std::string fileOne(Form("../data/file(1)%s.txt", this->scanNumberText().c_str()));
    std::cout << "[BearScan::readSingleScan] Reading file: " << fileOne << std::endl;
    std::ifstream ifsOne(fileOne.c_str());

    if( ifsOne.good() ) {

      readFile( ifsOne );

    } else {

      std::cout << "[BearScan::readFile] ERROR! File '" << fileOne << "' doesn't exist. Exiting." << std::endl;
      exit(1);

    } // if ifsOne

  } // if ifsAver


  system("mkdir -p plots");

}




void BearScan::readFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  if( ifs.good() ) {

    readFile( ifs );

  } else {

    std::cout << "[BearScan::readFile] ERROR! File '" << fileName << "' doesn't exist. Exiting." << std::endl;
    exit(1);

  }

}



void BearScan::readFile( std::ifstream& ifs ) {

     
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

      this->recognizeScanType( thisLine );

    } else if( thisLine=="Instrument 1: AMMETER KEITHLEY 6517A B" ) {

      keithleyB = true;

    } else if( thisLine=="Instrument 2: AMMETER KEITHLEY 6517A A" ) {

      keithleyB = false;

    } else if( keithleyB && thisLine=="Vsource: OFF" ) {

      std::cout << "[BearScan::readFile] WARNING!!! Seems like KEITHLEY B has Vsource OFF!!! Aborting!" << std::endl;
      exit(13);

    } else if( keithleyB && thisLine=="Vsource: ON" ) {

      std::cout << "[BearScan::readFile] Checked that KEITHLEY B has Vsource ON, as expected. Good." << std::endl;

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

        int x_index = this->x_index();
        float x = (x_index>=0 ) ? atof( numbers[x_index].c_str() ) : gr_scan_->GetN();

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



void BearScan::readRegionsScan() {

  std::cout << std::endl;

  for( unsigned iRegion=firstRegion_; iRegion<=lastRegion_; ++iRegion ) {

    std::string fileName( Form( "../data/file(1)_Region %d__%s.txt", iRegion, this->scanNumberText().c_str() ) );
    std::cout << "[BearScan::readRegionsScan] Adding file: " << fileName << std::endl;
    readFile( fileName );

  }

}




void BearScan::drawGraphs() const {

  drawGraph( gr_mirror_, "I0"    , this->getXtitle(), "I_{0} [A]"       );
  drawGraph( gr_drain_ , "Idrain", this->getXtitle(), "I_{drain} [A]"   );
  drawGraph( gr_scan_  , "scan"  , this->getXtitle(), this->getYtitle() );

}



void BearScan::drawGraph( TGraphErrors* graph, const std::string& name, const std::string& xTitle, const std::string& yTitle ) const {

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
  if( firstRegion_!=-1 && lastRegion_!=-1 ) regionText = std::string(Form("_regions_%d_%d", firstRegion_, lastRegion_));


  std::string plotFileName(Form("plots/%d%s_%s.pdf", number_, regionText.c_str(), name.c_str()) );
  
  c1->SaveAs( plotFileName.c_str() );
  system( "open ../plots/");
  if( name=="scan" ) system( Form("open %s", plotFileName.c_str()) );

  delete c1;
  delete h2_axes;

}


void BearScan::findPlotRanges( TGraph* graph, float& xMin, float& xMax, float& yMin, float& yMax ) const {

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


