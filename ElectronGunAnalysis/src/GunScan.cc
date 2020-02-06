#include "../interface/GunScan.h"

#include "../../CommonTools/interface/NanoUVCommon.h"

#include <fstream>

#include "TF1.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"





GunScan::GunScan( float gunEnergy, float APDhv, const std::string& dataDir, const std::string& scanName, float iGunBefore, float iGunAfter ) {

  gunEnergy_ = gunEnergy;
  APDhv_ = APDhv;
  dataDir_ = dataDir;
  scanName_ = scanName;
  iGunBefore_ = iGunBefore;
  iGunAfter_ = (iGunAfter>=0.) ? iGunAfter : iGunBefore;

  currentMethod_ = "max";
  firstN_fit_ = 12;
  lastN_fit_ = 12;
  baselineFunc_ = "pol3";

  loadScan();

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



// ---- OTHER FUNCTIONS


float GunScan::gunCurrent() const {

  return 0.5*(iGunBefore_+iGunAfter_);

}


float GunScan::gunCurrentError() const {

  return fabs(iGunBefore_-iGunAfter_);

}


void GunScan::loadScan() {

  if( graph_ != 0 ) delete graph_;
  graph_ = new TGraph(0);
  graph_->SetName( Form( "gr_%s", scanName_.c_str()) );

  std::string fullFileName( Form("data/%s/%s", dataDir_.c_str(), scanName_.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Loading scan from file: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph_->GetN();
    graph_->SetPoint( iPoint, x_gun*201/gunEnergy_, -i_apd*1E9 ); // make positive and convert to nA

  }

}




TGraph* GunScan::getCorrectedGraph( TF1* baseline ) {

  TGraph* graph_corr = new TGraph(0);
  graph_corr->SetName( Form( "%s_corr", graph_->GetName() ) );

  for( int iPoint=0; iPoint<graph_->GetN(); ++iPoint ) {

    double x, y;
    graph_->GetPoint( iPoint, x, y );

    graph_corr->SetPoint( iPoint, x, y-baseline->Eval(x) );

  }

  return graph_corr;

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

  float current = 0.;

  if( currentMethod_ == "max" ) {

    current = getYmax( graph_ );


  } else if( currentMethod_ == "firstMax" ) {

    for( int iPoint=0; iPoint<graph_->GetN(); ++iPoint ) {

      double this_x, this_y;
      graph_->GetPoint( iPoint, this_x, this_y );
      if( this_x >= -6. ) continue;
      if( this_y>=current ) current = this_y;

    } // for points

  } else if( currentMethod_ == "integral" ) {

    float step = getStep(graph_);

    for( int i=0; i<graph_->GetN(); ++i ) {

      double x, y;
      graph_->GetPoint( i, x, y );
      current += y*step;

    } // for points

  } else {

    std::cout << "[GunScan::getCurrentFromScan] WARNING!! Current method '" << currentMethod_ << "' not recognized! Aborting." << std::endl;
    exit(1);

  } // if current method

  return current;

}



float GunScan::getBaseline( float x0 ) {

  TF1* baseline = this->fitDrift( graph_ );

  return baseline->Eval(x0);

}


TF1* GunScan::fitDrift( TGraph* graph ) {

  TGraph* fit_points = new TGraph(0);

  int nPoints = graph->GetN();

  for( int i=0; i<nPoints; ++i ) {

    if( (i<=firstN_fit_) || (i>=(nPoints-lastN_fit_)) ) {

      double x, y;
      graph->GetPoint( i, x, y );

      fit_points->SetPoint( fit_points->GetN(), x, y );

    } // if

  } // for

  double xFirst, xLast, yFirst, yLast;
  graph->GetPoint( 0, xFirst, yFirst );
  graph->GetPoint( nPoints-1, xLast, yLast );

  double xMin, xMax;
  if(xFirst<xLast) {
    xMin = xFirst;
    xMax = xLast;
  } else {
    xMin = xLast;
    xMax = xFirst;
  }

  TF1* baseline = new TF1( Form("line_%s", graph->GetName()), baselineFunc_.c_str(), xMin, xMax );
  //TF1* baseline = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x + [2]*x*x + [3]*x*x*x", xMin, xMax );

  fit_points->Fit( baseline, "QR" );
 
  return baseline;

}



void GunScan::addPointToGraph( TGraphErrors* graph, const std::string& fileName ) {

  TF1* baseline = fitDrift( graph_ );

  TGraph* gr_scan_corr = getCorrectedGraph( baseline );


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


  TH2D* h2_axes = new TH2D( Form("axes_%s", fileName.c_str()), "", 10, xMin, xMax, 10, y0 - 0.5*yDiff, y0 + 2.5*yDiff );
  h2_axes->SetXTitle("Gun Position [mm]"); 
  h2_axes->SetYTitle("APD Current [nA]");
  h2_axes->Draw();

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();

  TPaveText* label_settings = new TPaveText( 0.21, 0.6, 0.48, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetTextColor( 46 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy_) );

  if( this->gunCurrent() < 1. )
    label_settings->AddText( Form("I_{gun} = %.0f fA", this->gunCurrent()*1000.) );
  else
    label_settings->AddText( Form("I_{gun} = %.1f pA", this->gunCurrent()) );
  label_settings->AddText( Form( "V_{APD} = %.0f V", APDhv_ ) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");
  
  baseline->SetLineWidth(2);
  baseline->SetLineColor(46);
  baseline->SetLineStyle(2);
  baseline->Draw("L same");

  graph_->SetMarkerStyle(20);
  graph_->SetMarkerSize(1.1);
  graph_->SetMarkerColor(kGray+3);
  graph_->SetLineColor(kGray+3);
  graph_->Draw("P same");


  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scan_%s.pdf", dataDir_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scan_%s.eps", dataDir_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));

  c1->Clear();

  double yMax_corr = getYmax( gr_scan_corr );

  //TH2D* h2_axes_corr = new TH2D( Form("axes_corr_%s", fileName.c_str()), "", 10, xMin, xMax, 10, -1.5, 13. );
  TH2D* h2_axes_corr = new TH2D( Form("axes_corr_%s", fileName.c_str()), "", 10, xMin, xMax, 10, -0.5*yMax_corr, 2.5*yMax_corr );
  h2_axes_corr->SetXTitle("Gun Position [mm]");
  h2_axes_corr->SetYTitle("Corrected APD Current [nA]");
  h2_axes_corr->Draw();

  TLine* line_zero = new TLine( xMin, 0., xMax, 0. );
  line_zero->SetLineWidth(2);
  line_zero->SetLineColor(46);
  line_zero->SetLineStyle(2);
  line_zero->Draw("L same");

  label_settings->Draw("same");

  gr_scan_corr->SetMarkerStyle(20);
  gr_scan_corr->SetMarkerSize(1.1);
  gr_scan_corr->SetMarkerColor(kGray+3);
  gr_scan_corr->SetLineColor(kGray+3);
  gr_scan_corr->Draw("P same");

  label->Draw("same");  
  gPad->RedrawAxis();

  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.pdf", dataDir_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.eps", dataDir_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));


  float iAPD = this->getCurrentFromScan()*1000; // convert to pA

  std::cout << "--> for E(gun) = " << this->gunEnergy() << " eV, V(APD) = " << this->APDhv() << " V, I(gun) = " << this->gunCurrent() << " pA, the APD current was: " << iAPD/1000. << " nA" << std::endl;

  int thisPoint = graph->GetN();
  graph->SetPoint     ( thisPoint, this->gunCurrent(), iAPD );
  graph->SetPointError( thisPoint, this->gunCurrentError(), 0.02*iAPD ); // error on APD current = 2% (?)

  delete c1;
  delete h2_axes;

}
