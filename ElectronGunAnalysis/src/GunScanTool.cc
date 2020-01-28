#include "../interface/GunScanTool.h"

#include "../../CommonTools/interface/NanoUVCommon.h"

#include <fstream>

#include "TF1.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TPaveText.h"



GunScanTool::GunScanTool( float gunEnergy, float APDhv, const std::string& data ) {

  gunEnergy_ = gunEnergy;
  APDhv_ = APDhv;
  data_ = data;
  currentMethod_ = "max";

  firstN_fit_ = 12;
  lastN_fit_ = 12;
  baselineFunc_ = "pol3";

}



// ------ SETTERS

void GunScanTool::set_gunEnergy( float gunEnergy ) {

  gunEnergy_ = gunEnergy;

}


void GunScanTool::set_APDhv( float APDhv ) {

  APDhv_ = APDhv;

}


void GunScanTool::set_currentMethod( const std::string& currentMethod ) {

  currentMethod_ = currentMethod;

}

void GunScanTool::set_data( const std::string& data ) {

  data_ = data;

}


void GunScanTool::set_firstN_fit( int firstN_fit ) {

  firstN_fit_ = firstN_fit;

}


void GunScanTool::set_lastN_fit( int lastN_fit ) {

  lastN_fit_ = lastN_fit;

}


void GunScanTool::set_baselineFunc( const std::string& baselineFunc ) {

  baselineFunc_ = baselineFunc;

}



// ------ GETTERS

float GunScanTool::gunEnergy() const {

  return gunEnergy_;

}


float GunScanTool::APDhv() const {

  return APDhv_;

}


std::string GunScanTool::currentMethod() const {

  return currentMethod_;

}


std::string GunScanTool::data() const {

  return data_;

}


int GunScanTool::firstN_fit() const {

  return firstN_fit_;

}


int GunScanTool::lastN_fit() const {

  return lastN_fit_;

}


std::string GunScanTool::baselineFunc() const {

  return baselineFunc_;

}



// ---- OTHER FUNCTIONS


TGraph* GunScanTool::getScanFromFile( const std::string& fileName ) {

  TGraph* graph = new TGraph(0);
  graph->SetName( Form( "gr_%s", fileName.c_str()) );

  std::string fullFileName( Form("data/%s/%s", data_.c_str(), fileName.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Opening: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x_gun*201/gunEnergy_, -i_apd*1E9 ); // make positive and convert to nA

  }

  return graph;

}




TGraph* GunScanTool::getCorrectedGraph( TGraph* graph, TF1* baseline ) {

  TGraph* graph_corr = new TGraph(0);
  graph_corr->SetName( Form( "%s_corr", graph->GetName() ) );

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    graph_corr->SetPoint( iPoint, x, y-baseline->Eval(x) );

  }

  return graph_corr;

}



float GunScanTool::getYmax( TGraph* graph ) {

  float yMax = 0.;

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double this_x, this_y;
    graph->GetPoint( iPoint, this_x, this_y );
    if( this_y>=yMax ) yMax = this_y;

  } // for points

  return yMax;

}



float GunScanTool::getCurrentFromScan( TGraph* graph ) {

  float current = 0.;

  if( currentMethod_ == "max" ) {

    current = getYmax( graph );


  } else if( currentMethod_ == "firstMax" ) {

    for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

      double this_x, this_y;
      graph->GetPoint( iPoint, this_x, this_y );
      if( this_x >= -6. ) continue;
      if( this_y>=current ) current = this_y;

    } // for points

  } else if( currentMethod_ == "integral" ) {

    for( int i=0; i<graph->GetN(); ++i ) {

      double x, y;
      graph->GetPoint( i, x, y );
      current += y;

    } // for points
  
  } else {

    std::cout << "[GunScanTool::getCurrentFromScan] WARNING!! Current method '" << currentMethod_ << "' not recognized! Aborting." << std::endl;
    exit(1);

  } // if current method

  return current;

}


TF1* GunScanTool::fitDrift( TGraph* graph ) {

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



void GunScanTool::addPointToGraph( TGraphErrors* graph, const std::string& fileName, float iGunBefore, float iGunAfter ) {

  TGraph* gr_scan = this->getScanFromFile( fileName.c_str() );

  TF1* baseline = fitDrift( gr_scan );

  TGraph* gr_scan_corr = getCorrectedGraph( gr_scan, baseline );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  double xFirst, xLast, yFirst, yLast;
  gr_scan->GetPoint( 0, xFirst, yFirst );
  gr_scan->GetPoint( gr_scan->GetN()-1, xLast, yLast );

  double xMin, xMax;
  if(xFirst<xLast) {
    xMin = xFirst;
    xMax = xLast;
  } else {
    xMin = xLast;
    xMax = xFirst;
  }
  

  double yMax = getYmax( gr_scan );
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

  float gunCurrent = 0.5*( iGunBefore + iGunAfter );
  if( gunCurrent < 1. )
    label_settings->AddText( Form("I_{gun} = %.0f fA", gunCurrent*1000.) );
  else
    label_settings->AddText( Form("I_{gun} = %.1f pA", gunCurrent) );
  label_settings->AddText( Form( "V_{APD} = %.0f V", APDhv_ ) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");
  
  baseline->SetLineWidth(2);
  baseline->SetLineColor(46);
  baseline->SetLineStyle(2);
  baseline->Draw("L same");

  gr_scan->SetMarkerStyle(20);
  gr_scan->SetMarkerSize(1.1);
  gr_scan->SetMarkerColor(kGray+3);
  gr_scan->SetLineColor(kGray+3);
  gr_scan->Draw("P same");


  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scan_%s.pdf", data_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scan_%s.eps", data_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));

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
  gr_scan_corr->SetMarkerSize(1.6);
  gr_scan_corr->SetMarkerColor(kGray+3);
  gr_scan_corr->SetLineColor(kGray+3);
  gr_scan_corr->Draw("P same");

  label->Draw("same");  
  gPad->RedrawAxis();

  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.pdf", data_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/%.0feV/%.0fV/scanCorr_%s.eps", data_.c_str(), gunEnergy(), APDhv(), fileName.c_str()));


  float iAPD = getCurrentFromScan( gr_scan_corr )*1000; // convert to pA

  std::cout << "--> for E(gun) = " << this->gunEnergy() << " eV, V(APD) = " << this->APDhv() << " V, I(gun) = " << gunCurrent << " pA, the APD current was: " << iAPD/1000. << " nA" << std::endl;

  int thisPoint = graph->GetN();
  graph->SetPoint     ( thisPoint, gunCurrent, iAPD );
  graph->SetPointError( thisPoint, fabs( iGunAfter-iGunBefore ), 0.02*iAPD ); // error on APD current = 2% (?)

  delete c1;
  delete h2_axes;

}
