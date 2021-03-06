#include "../interface/GunScan.h"

#include "../../CommonTools/interface/NanoUVCommon.h"

#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"





GunScan::GunScan( float gunEnergy, float APDhv, const std::string& dataDir, const std::string& scanName, float iGunBefore, float iGunAfter, const std::string& baselineFunc ) {

  gunEnergy_ = gunEnergy;
  APDhv_ = APDhv;
  dataDir_ = dataDir;
  
  TString scanName_tstr(scanName);
  if( scanName_tstr.EndsWith(".dat") ) scanName_ = scanName.substr(0, scanName.length()-4);
  else scanName_ = scanName;

  iGunBefore_ = iGunBefore;
  iGunAfter_ = (iGunAfter>=0.) ? iGunAfter : iGunBefore;

  currentMethod_ = "average";
  currentEvalPoint_ = -6.;
  firstN_fit_ = 18;
  lastN_fit_ = 16;
  baselineFunc_ = baselineFunc;

  gr_currentSyst_ = 0;

  loadScan();
  correctGraph();

  loadCurrentSyst();

  system( Form("mkdir -p %s", this->outdir().c_str()) );

}



// ------ SETTERS

void GunScan::set_gunEnergy( float gunEnergy ) {

  gunEnergy_ = gunEnergy;

}


void GunScan::set_APDhv( float APDhv ) {

  APDhv_ = APDhv;

}


void GunScan::set_currentMethod( const std::string& currentMethod ) {

  currentMethod_ = currentMethod;

}


void GunScan::set_currentEvalPoint( float currentEvalPoint ) {

  currentEvalPoint_ = currentEvalPoint;

}


void GunScan::set_dataDir( const std::string& dataDir ) {

  dataDir_ = dataDir;

}


void GunScan::set_scanName( const std::string& scanName ) {

  scanName_ = scanName;

}


void GunScan::set_iGunBefore( float iGunBefore ) {

  iGunBefore_ = iGunBefore;

}


void GunScan::set_iGunAfter( float iGunAfter ) {

  iGunAfter_ = iGunAfter;

}


void GunScan::set_firstN_fit( int firstN_fit ) {

  firstN_fit_ = firstN_fit;

}


void GunScan::set_lastN_fit( int lastN_fit ) {

  lastN_fit_ = lastN_fit;

}


void GunScan::set_baselineFunc( const std::string& baselineFunc ) {

  baselineFunc_ = baselineFunc;

}



// ------ GETTERS

float GunScan::gunEnergy() const {

  return gunEnergy_;

}


float GunScan::APDhv() const {

  return APDhv_;

}


std::string GunScan::currentMethod() const {

  return currentMethod_;

}


float GunScan::currentEvalPoint() const {

  return currentEvalPoint_;

}


std::string GunScan::dataDir() const {

  return dataDir_;

}


std::string GunScan::scanName() const {

  return scanName_;

}


float GunScan::iGunBefore() const {

  return iGunBefore_;

}


float GunScan::iGunAfter() const {

  return iGunAfter_;

}


int GunScan::firstN_fit() const {

  return firstN_fit_;

}


int GunScan::lastN_fit() const {

  return lastN_fit_;

}


std::string GunScan::baselineFunc() const {

  return baselineFunc_;

}


TGraph* GunScan::graph() const {

  return graph_;

}


TGraph* GunScan::graph_corr() const {

  return graph_corr_;

}


TF1* GunScan::baseline() const {

  return baseline_;

}



// ---- OTHER FUNCTIONS


std::string GunScan::outdir() const {

  return std::string(Form("plots/APDscans/%s/%.0feV/%.0fV", dataDir_.c_str(), gunEnergy_, APDhv_) );

}


float GunScan::gunCurrent() const {

  return 0.5*(iGunBefore_+iGunAfter_);

}


float GunScan::gunCurrentError() const {

  float diffCurrent = fabs(iGunBefore_-iGunAfter_)/2.;
  float accuracy = 0.01*this->gunCurrent() + 0.003;
  //float accuracy = 0.005*this->gunCurrent();

  float error = (diffCurrent>accuracy) ? diffCurrent : accuracy;

  return error;

}


std::string GunScan::gunCurrentText() const {

  std::string gunCurrentText;
  if( this->gunCurrent() < 1. )
    gunCurrentText = std::string(Form("I_{gun} = %.0f fA", this->gunCurrent()*1000.) );
  else
    gunCurrentText = std::string(Form("I_{gun} = %.1f pA", this->gunCurrent()) );

  return gunCurrentText;

}


void GunScan::loadScan() {

  //if( graph_ != 0 ) delete graph_;
  graph_ = new TGraph(0);
  if( this->baselineFunc()!="pol3" )
    graph_->SetName( Form( "gr_%s_%s", scanName_.c_str(), baselineFunc_.c_str()) );
  else
    graph_->SetName( Form( "gr_%s", scanName_.c_str()) );

  std::string fullFileName( Form("data/%s/%s.dat", dataDir_.c_str(), scanName_.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Loading scan from file: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph_->GetN();
    graph_->SetPoint( iPoint, this->convertGunPosition(x_gun), -i_apd*1E9 ); // make positive and convert to nA

  }

}




std::vector< GunScan* > GunScan::loadScans( const std::string& scansFile, float gunEnergy, float APDhv, const std::string& baselineFunc ) {

  std::cout << "-> Loading scans from file: " << scansFile << std::endl;
  if( gunEnergy>=0. ) std::cout << "  -> Will load only scans with E(gun) = " << gunEnergy << " eV" << std::endl;
  if( APDhv    >=0. ) std::cout << "  -> Will load only scans with HV(APD) = " << APDhv << " V" << std::endl;
  std::cout << std::endl;

  std::string dataDir = findDataDirFromPath( scansFile );

  std::vector< GunScan* > scans;

  std::ifstream ifs(scansFile.c_str());

  std::string line;
  while( ifs.good() ) {

    std::getline(ifs, line);
    TString line_tstr(line);
    if( line_tstr.BeginsWith("#") ) continue;
    if( line_tstr.Length()<10 ) continue;

    std::string this_scanName;
    float this_gunEnergy, this_APDhv, this_iGunBefore, this_iGunAfter;

    std::istringstream iss(line);
    iss >> this_scanName >> this_gunEnergy >> this_APDhv >> this_iGunBefore >> this_iGunAfter;

    correctCurrentZero( this_iGunBefore, scansFile );
    correctCurrentZero( this_iGunAfter , scansFile );

    bool gunEnergyOK = (gunEnergy<0.) || (gunEnergy>=0. && this_gunEnergy==gunEnergy);
    bool APDhvOK     = (APDhv    <0.) || (APDhv    >=0. && this_APDhv    ==APDhv    );
    if( gunEnergyOK && APDhvOK ) {
      GunScan* this_gunScan = new GunScan( this_gunEnergy, this_APDhv, dataDir, this_scanName, this_iGunBefore, this_iGunAfter, baselineFunc );
      scans.push_back( this_gunScan );
     }

  } // while ifs

  std::cout << std::endl << std::endl;

  return scans;

}

    
void GunScan::correctCurrentZero( float& current, const std::string& scansFile ) {
  
  // correcting for the different zeros at different dynamic ranges (measured values)

  TString scansFile_tstr(scansFile);
  if( scansFile_tstr.Contains("2020_02") ) {

    if( current < 1.3 ) current += 0.0052; 
    else                current -= 0.028; 

  } else if( scansFile_tstr.Contains("2020_07") ) {

    if( current < 1.0 ) current -= 0.002; 
    else                current -= 0.001; 

  }

}



std::string GunScan::findDataDirFromPath( const std::string& scansFile ) {

  TString scansFile_tstr(scansFile);
  std::string scansFile2(std::string(scansFile_tstr.ReplaceAll( "//", "/" )));

  std::vector<std::string> parts;
  std::string delimiter = "/";
  size_t pos = 0;
  while (( pos = scansFile2.find(delimiter)) != std::string::npos) {
    std::string part = scansFile2.substr(0, pos);
    parts.push_back(part);
    scansFile2.erase(0, pos + delimiter.length());
  }

  std::string dataDir = parts[parts.size()-1];

  //std::cout << "-> Automatically found dataDir: " << dataDir << std::endl;

  return dataDir;

}



void GunScan::loadCurrentSyst() {

  TFile* systFile = TFile::Open( Form("systCurrent_E%.0feV_APDhv%.0fV.root", this->gunEnergy(), this->APDhv()) );
  gr_currentSyst_ = (TGraphErrors*)systFile->Get("syst");

}



float GunScan::getCurrentSyst() const {

  float syst = 0.;

  if( gr_currentSyst_!=0 ) {

    for( unsigned iPoint=0; iPoint<gr_currentSyst_->GetN(); ++iPoint ) {

      double x,y;
      gr_currentSyst_->GetPoint( iPoint, x, y );

      if( x==this->gunCurrent() ) {
        syst = y;
        break;
      }

    }

  } // if graph exists

  return syst;

}
  


float GunScan::getXmax( TGraph* graph ) {

  float xMax = 0.;

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double this_x, this_y;
    graph->GetPoint( iPoint, this_x, this_y );
    if( this_x>=xMax ) xMax = this_x;

  } // for points

  return xMax;

}


float GunScan::getYmax( TGraph* graph ) {

  float yMax = 0.;

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double this_x, this_y;
    graph->GetPoint( iPoint, this_x, this_y );
    if( this_y>=yMax ) yMax = this_y;

  } // for points

  return yMax;

}


float GunScan::getStep( TGraph* graph ) {

  double x1, x2, y1, y2;

  graph->GetPoint( 1, x1, y1 );
  graph->GetPoint( 2, x2, y2 );

  return fabs(x2-x1);

}


float GunScan::getCurrentFromScan() {

  float dummyError;

  return this->getCurrentFromScan( dummyError );

}



float GunScan::getCurrentFromScan( float& currentError ) {

  float current = 0.; // in nA
  float keithleyErr = 0.01/1000.; // Keithley uncertainty 0.01 pA, converted in nA (starting from 2020_07 we saved the full amount of digits)
  if( gunEnergy_==500. || gunEnergy_==900. ) keithleyErr = 1./1000.; // the Keithley picoammeter was unfortunately saved truncating at less uncertainty for the 500 and 900 eV runs (2020_02)
  float systErr = 1.349/1000.; //systematic on fit 1.349 pA per bin, converted in nA, from checkSystFit.cpp

  if( currentMethod_ == "max" ) {

    current = getYmax( graph_corr_ );
    currentError = keithleyErr;

  } else if( currentMethod_ == "firstMax" ) {

    for( int iPoint=0; iPoint<graph_corr_->GetN(); ++iPoint ) {

      double this_x, this_y=0.;
      graph_corr_->GetPoint( iPoint, this_x, this_y );
      if( this_y>=current ) current = this_y;
    } // for points

    currentError = keithleyErr;

  } else if( currentMethod_ == "integral" ) { // unsupported

    float step = getStep(graph_corr_);

    for( int i=0; i<graph_corr_->GetN(); ++i ) {

      double x, y;
      graph_corr_->GetPoint( i, x, y );
      current += y*step;

    } // for points

    current /= 3.; // diameter of the APD is 3mm

    currentError = keithleyErr; // this is wrong, but integral method should not be used

  } else if( currentMethod_ == "average" ) { // currently supported method 

    float step = getStep(graph_corr_);
    float Ntot = 0.; 
    float halfHeight = getYmax( graph_corr_ )/2.;
    float sumCurrent = 0.;

    for( int i=0; i<graph_corr_->GetN(); ++i ) {

      double x, y;
      graph_corr_->GetPoint( i, x, y );

      if ( y >= halfHeight ) {
        sumCurrent += y;
        Ntot += 1;
        }

      } // for points
      
      current = sumCurrent/Ntot;
      currentError = sqrt(keithleyErr*keithleyErr + systErr*systErr)/sqrt((float)Ntot);
      std::cout << "The average is computed on a length of: " << Ntot*step << " mm (" << Ntot << " points)" << std::endl;


  } else if( currentMethod_ == "average25" ) { // currently supported method 

    float step = getStep(graph_corr_);
    float Ntot = 0.; 
    float halfHeight = 0.25*getYmax( graph_corr_ );
    float sumCurrent = 0.;

    for( int i=0; i<graph_corr_->GetN(); ++i ) {

      double x, y;
      graph_corr_->GetPoint( i, x, y );

      if ( y >= halfHeight ) {
        sumCurrent += y;
        Ntot += 1;
        }

      } // for points
      
      current = sumCurrent/Ntot;
      currentError = sqrt(keithleyErr*keithleyErr + systErr*systErr)/sqrt((float)Ntot);
      std::cout << "The average is computed on a length of: " << Ntot*step << " mm (" << Ntot << " points)" << std::endl;


  } else if( currentMethod_ == "average75" ) { // currently supported method 

    float step = getStep(graph_corr_);
    float Ntot = 0.; 
    float halfHeight = 0.75*getYmax( graph_corr_ );
    float sumCurrent = 0.;

    for( int i=0; i<graph_corr_->GetN(); ++i ) {

      double x, y;
      graph_corr_->GetPoint( i, x, y );

      if ( y >= halfHeight ) {
        sumCurrent += y;
        Ntot += 1;
        }

      } // for points
      
      current = sumCurrent/Ntot;
      currentError = sqrt(keithleyErr*keithleyErr + systErr*systErr)/sqrt((float)Ntot);
      std::cout << "The average is computed on a length of: " << Ntot*step << " mm (" << Ntot << " points)" << std::endl;

  } else if( currentMethod_ == "point" ) {

    for( int i=0; i<graph_corr_->GetN()-1; ++i ) {

      double x1, y1;
      graph_corr_->GetPoint( i  , x1, y1 );

      double x2, y2;
      graph_corr_->GetPoint( i+1, x2, y2 );

      if( currentEvalPoint_ > x1 && currentEvalPoint_ < x2 ) {

        // linear interpolation
        float slope = (y2-y1)/(x2-x1);
        current = y1 + slope*(currentEvalPoint_-x1);

      } // if

    } // for points

  } else {

    std::cout << "[GunScan::getCurrentFromScan] WARNING!! Current method '" << currentMethod_ << "' not recognized! Aborting." << std::endl;
    exit(1);

  } // if current method

  return current;

}



float GunScan::getBaseline( float x0 ) {

  return baseline_->Eval(x0);

}



void GunScan::correctGraph() {

  fitDrift();
  subtractBaseline();

}



void GunScan::fitDrift() {

  TGraph* fit_points = new TGraph(0);

  int nPoints = graph_->GetN();

  for( int i=0; i<nPoints; ++i ) {

    if( (i<=firstN_fit_) || (i>=(nPoints-lastN_fit_)) ) {

      double x, y;
      graph_->GetPoint( i, x, y );

      fit_points->SetPoint( fit_points->GetN(), x, y );

    } // if

  } // for

  double xFirst, xLast, yFirst, yLast;
  graph_->GetPoint( 0, xFirst, yFirst );
  graph_->GetPoint( nPoints-1, xLast, yLast );

  double xMin, xMax;
  if(xFirst<xLast) {
    xMin = xFirst;
    xMax = xLast;
  } else {
    xMin = xLast;
    xMax = xFirst;
  }

  baseline_ = new TF1( Form("line_%s", graph_->GetName()), baselineFunc_.c_str(), xMin, xMax );

  fit_points->Fit( baseline_, "QR" );
 
}



void GunScan::subtractBaseline() {

  graph_corr_ = new TGraph(0);
  graph_corr_->SetName( Form( "%s_corr", graph_->GetName() ) );

std::cout << "graph_corr_ name: " << graph_corr_->GetName() << std::endl;

  for( int iPoint=0; iPoint<graph_->GetN(); ++iPoint ) {

    double x, y;
    graph_->GetPoint( iPoint, x, y );

    graph_corr_->SetPoint( iPoint, x, y-baseline_->Eval(x) );

  }

}





void GunScan::addPointToGraph( TGraphErrors* graph ) {


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  double xFirst, xLast, yFirst, yLast;
  graph_->GetPoint( 0, xFirst, yFirst );
  graph_->GetPoint( graph_->GetN()-1, xLast, yLast );

  double xMin, xMax;
  if(xFirst<xLast) {
    xMin = xFirst;
    xMax = xLast;
  } else {
    xMin = xLast;
    xMax = xFirst;
  }
  

  double yMax = getYmax( graph_ );
  double yDiff = (yLast<yFirst) ? fabs(yMax - yLast) : fabs(yMax - yFirst);
  double y0 = (yLast<yFirst) ? yLast : yFirst;


  TH2D* h2_axes = new TH2D( Form("axes_%s", graph_->GetName()), "", 10, xMin, xMax, 10, y0 - 0.5*yDiff, y0 + 2.5*yDiff );
  h2_axes->SetXTitle("Beam Position [mm]"); 
  h2_axes->SetYTitle("APD Current [nA]");
  h2_axes->Draw();

  //TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  //label->Draw("same");  
  gPad->RedrawAxis();

  TPaveText* label_settings = new TPaveText( 0.21, 0.7, 0.48, 0.87, "brNDC" );
  //TPaveText* label_settings = new TPaveText( 0.21, 0.6, 0.48, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetTextColor( 46 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{e} = %.0f eV", gunEnergy_) );
  label_settings->AddText( Form("%s", this->gunCurrentText().c_str()) );

  label_settings->AddText( Form( "V_{apd} = %.0f V", APDhv_ ) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");
  
  baseline_->SetLineWidth(2);
  baseline_->SetLineColor(46);
  baseline_->SetLineStyle(2);
  baseline_->Draw("L same");

  graph_->SetMarkerStyle(20);
  graph_->SetMarkerSize(1.1);
  graph_->SetMarkerColor(kGray+3);
  graph_->SetLineColor(kGray+3);
  graph_->Draw("P same");


  if( this->baselineFunc()!="pol3" ) {
    c1->SaveAs(Form("%s/scan_%s_%s.pdf", this->outdir().c_str(), scanName_.c_str(), baselineFunc_.c_str()));
    c1->SaveAs(Form("%s/scan_%s_%s.eps", this->outdir().c_str(), scanName_.c_str(), baselineFunc_.c_str()));
  } else {
    c1->SaveAs(Form("%s/scan_%s.pdf", this->outdir().c_str(), scanName_.c_str()));
    c1->SaveAs(Form("%s/scan_%s.eps", this->outdir().c_str(), scanName_.c_str()));
  }

  c1->Clear();

  double yMax_corr = getYmax( graph_corr_ );

  TH2D* h2_axes_corr = new TH2D( Form("axes_corr_%s", graph_->GetName()), "", 10, xMin, xMax, 10, -0.5*yMax_corr, 2.5*yMax_corr );
  h2_axes_corr->SetXTitle("Beam Position [mm]");
  h2_axes_corr->SetYTitle("Corrected APD Current [nA]");
  h2_axes_corr->Draw();

  TLine* line_zero = new TLine( xMin, 0., xMax, 0. );
  line_zero->SetLineWidth(2);
  line_zero->SetLineColor(46);
  line_zero->SetLineStyle(2);
  line_zero->Draw("L same");

  label_settings->Draw("same");

  graph_corr_->SetMarkerStyle(20);
  graph_corr_->SetMarkerSize(1.1);
  graph_corr_->SetMarkerColor(kGray+3);
  graph_corr_->SetLineColor(kGray+3);
  graph_corr_->Draw("P same");

  //label->Draw("same");  
  gPad->RedrawAxis();

  //c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.pdf", dataDir_.c_str(), gunEnergy(), APDhv(), scanName_.c_str()));
  //c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.eps", dataDir_.c_str(), gunEnergy(), APDhv(), scanName_.c_str()));

  if( this->baselineFunc()!="pol3" ) {
    c1->SaveAs(Form("%s/scanCorr_%s_%s.pdf", this->outdir().c_str(), scanName_.c_str(), baselineFunc_.c_str()));
    c1->SaveAs(Form("%s/scanCorr_%s_%s.eps", this->outdir().c_str(), scanName_.c_str(), baselineFunc_.c_str()));
  } else {
    c1->SaveAs(Form("%s/scanCorr_%s.pdf", this->outdir().c_str(), scanName_.c_str()));
    c1->SaveAs(Form("%s/scanCorr_%s.eps", this->outdir().c_str(), scanName_.c_str()));
  }


  float iAPDError;
  float iAPD = this->getCurrentFromScan(iAPDError);


  iAPD      *= 1000; // convert to pA
  iAPDError *= 1000; // convert to pA

  // add current method Syst
  float currentSyst = this->getCurrentSyst()*iAPD;
  iAPDError = sqrt( iAPDError*iAPDError + currentSyst*currentSyst );

  std::cout << "--> for E(gun) = " << this->gunEnergy() << " eV, V(APD) = " << this->APDhv() << " V, I(gun) = " << this->gunCurrent() << " pA, the APD current was: " << iAPD/1000. << " nA" << std::endl;

  int thisPoint = graph->GetN();

  //float fitError = 2; // in pA... systematic on fit, from https://agenda.infn.it/event/22066/contributions/111663/attachments/71099/89094/pandolfi_2020_03_05.pdf
  //if ( this->APDhv() == 380 ) { fitError = 50; } // in pA
  
  graph->SetPoint     ( thisPoint, this->gunCurrent(), iAPD );
  graph->SetPointError( thisPoint, this->gunCurrentError(), iAPDError );

  delete c1;
  delete h2_axes;
  delete h2_axes_corr;

}
