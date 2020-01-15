#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "../../CommonTools/interface/NanoUVCommon.h"



float get_iAPD( const std::string& fileName, float gunEnergy, float gunCurrent );
TGraph* getScanFromFile( const std::string& fileName, float gunEnergy );
TF1* fitDrift( TGraph* graph, int firstN=7, int lastN=5 );
TGraph* getCorrectedGraph( TGraph* graph, TF1* baseline );
double getYmax( TGraph* graph );
float getIntegral( TGraph* graph );


std::string data = "2019_11_28";


int main( int argc, char* argv[] ) {

  float gunEnergy;

  if( argc == 1 ) {

    std::cout << "USAGE: ./drawAPDGunScan [gunEnergy]" << std::endl;
    exit(1);

  } else {

    gunEnergy = atof(argv[1]);

  }

  NanoUVCommon::setStyle();

  std::string outdir( Form("plots/APDscans/%s/", data.c_str() ) );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TGraphErrors* gr_iapd_vs_igun = new TGraphErrors(0);

  float xMax;
  float yMax;

  if( gunEnergy == 500. ) {

    gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN()  , 0.75 , 1000.*get_iAPD("Ek_500_750fA_dfh_APD__28nov19_07_M_.dat", 500, 0.75) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 10. );

    gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 3.6 , 1000.*get_iAPD("Ek_500_3.6pA_dfh_APD__28nov19_06_M_.dat", 500, 3.6) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 10. );

    gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 26. , 1000.*get_iAPD("Ek_500_26pA_dfh_APD__28nov19_05_M_.dat" , 500, 26.) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 10. );

    xMax = 30.;
    yMax = 13000.;

  } else if( gunEnergy == 900. ) {

    gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 0.45, 1000.*get_iAPD("Ek_900_450fA_dfv.txt", 900, 0.45) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 1000. );

    gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 3.3 , 1000.*get_iAPD("Ek_900_3.3pA_dfv_28nov19_01_M_.dat", 900, 3.3) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 10. );

    gr_iapd_vs_igun->SetPoint( gr_iapd_vs_igun->GetN(), 15. , 1000.*get_iAPD("Ek_900_15pA_dfv.txt" , 900, 15) );
    gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.001, 1000. );

    xMax = 20.;
    yMax = 75000.;

  }


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "Gun Current [pA]" );
  h2_axes->SetYTitle( "APD Current [pA]" );
  h2_axes->Draw();

  TF1* f1_line = new TF1( "lineScan", "[0] + [1]*x", 0., xMax );
  gr_iapd_vs_igun->Fit( f1_line, "QR0" );

  f1_line->SetLineWidth(2);
  f1_line->SetLineColor(46);
  f1_line->SetLineStyle(2);
  f1_line->Draw("L same");

  gr_iapd_vs_igun->SetMarkerStyle(20);
  gr_iapd_vs_igun->SetMarkerSize(1.6);
  gr_iapd_vs_igun->SetMarkerColor(kGray+3);
  gr_iapd_vs_igun->SetLineColor(kGray+3);
  gr_iapd_vs_igun->Draw("P same");


  TPaveText* fitResults = new TPaveText( 0.6, 0.23, 0.9, 0.44, "brNDC" );
  fitResults->SetTextSize( 0.035 );
  fitResults->SetFillColor(0);
  fitResults->SetTextColor(46);
  fitResults->AddText( "f(x) = q + m*x" );
  fitResults->AddText( Form("q = %.1f #pm %.1f pA", f1_line->GetParameter(0), f1_line->GetParError(0) ) );
  fitResults->AddText( Form("m = %.1f #pm %.1f", f1_line->GetParameter(1), f1_line->GetParError(1) ) );
  fitResults->Draw("same");

  TPaveText* label_settings = new TPaveText( 0.23, 0.6, 0.5, 0.72, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy) );
  label_settings->AddText( "I_{APD} = 380 V" );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/APDscans/%s/iapd_vs_igun_E%.0f.pdf", data.c_str(), gunEnergy) );
  c1->SaveAs( Form("plots/APDscans/%s/iapd_vs_igun_E%.0f.eps", data.c_str(), gunEnergy) );
  
  return 0;

}


float get_iAPD( const std::string& fileName, float gunEnergy, float gunCurrent ) {

  TGraph* gr_scan = getScanFromFile( fileName, gunEnergy );

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
  
  double yMin = 101.;

  // find max by hand
  double yMax = getYmax( gr_scan );


  TH2D* h2_axes = new TH2D( Form("axes_%s", fileName.c_str()), "", 10, xMin, xMax, 10, yMin, yMin + 2.*(yMax-yMin) );
  h2_axes->SetXTitle("Gun Position [mm]"); 
  h2_axes->SetYTitle("APD Current [nA]");
  h2_axes->Draw();

  baseline->SetLineWidth(2);
  baseline->SetLineColor(46);
  baseline->SetLineStyle(2);
  baseline->Draw("L same");

  gr_scan->SetMarkerStyle(20);
  gr_scan->SetMarkerSize(1.6);
  gr_scan->SetMarkerColor(kGray+3);
  gr_scan->SetLineColor(kGray+3);
  gr_scan->Draw("P same");

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();

  TPaveText* label_settings = new TPaveText( 0.21, 0.6, 0.48, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetTextColor( 46 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy) );
  if( gunCurrent < 1. )
    label_settings->AddText( Form("I_{gun} = %.0f fA", gunCurrent*1000.) );
  else
    label_settings->AddText( Form("I_{gun} = %.1f pA", gunCurrent) );
  label_settings->AddText( "I_{APD} = 380 V" );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");
  

  c1->SaveAs(Form("plots/APDscans/%s/scan_%s.pdf", data.c_str(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/scan_%s.eps", data.c_str(), fileName.c_str()));

  c1->Clear();

  double yMax_corr = getYmax( gr_scan_corr );

  //TH2D* h2_axes_corr = new TH2D( Form("axes_corr_%s", fileName.c_str()), "", 10, xMin, xMax, 10, -1.5, 13. );
  TH2D* h2_axes_corr = new TH2D( Form("axes_corr_%s", fileName.c_str()), "", 10, xMin, xMax, 10, -0.2*yMax_corr, 1.2*yMax_corr );
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

  c1->SaveAs(Form("plots/APDscans/%s/scanCorr_%s.pdf", data.c_str(), fileName.c_str()));
  c1->SaveAs(Form("plots/APDscans/%s/scanCorr_%s.eps", data.c_str(), fileName.c_str()));


  float iAPD = getYmax( gr_scan_corr ); 
  //float iAPD = getIntegral( gr_scan_corr );

  std::cout << "For this configuration the APD current was: " << iAPD << " nA" << std::endl;

  delete c1;
  delete h2_axes;

  return iAPD;

}



float getIntegral( TGraph* graph ) {

  float integral = 0.;

  for( unsigned i=0; i<graph->GetN(); ++i ) {

    double x, y;
    graph->GetPoint( i, x, y );
    integral += y;

  } // for points

  return integral;

}



TGraph* getScanFromFile( const std::string& fileName, float gunEnergy ) {

  TGraph* graph = new TGraph(0);
  graph->SetName( Form( "gr_%s", fileName.c_str()) );

  std::string fullFileName( Form("data/2019_11_28/%s", fileName.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Opening: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x_gun*201/gunEnergy, -i_apd*1E9 ); // make positive and convert to nA

  }

  return graph;

}


TF1* fitDrift( TGraph* graph, int first_n, int last_n ) {

  TGraph* fit_points = new TGraph(0);

  int nPoints = graph->GetN();

  for( unsigned i=0; i<nPoints; ++i ) {

    if( (i<=first_n) || (i>=(nPoints-last_n)) ) {

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

  TF1* baseline = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x + [2]*x*x + [3]*x*x*x", xMin, xMax );

  fit_points->Fit( baseline, "QR" );
 
  return baseline;

}


TGraph* getCorrectedGraph( TGraph* graph, TF1* baseline ) {

  TGraph* graph_corr = new TGraph(0);
  graph_corr->SetName( Form( "%s_corr", graph->GetName() ) );

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    graph_corr->SetPoint( iPoint, x, y-baseline->Eval(x) );

  }

  return graph_corr;

}


double getYmax( TGraph* graph ) {

  double yMax;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {
    double this_x, this_y;
    graph->GetPoint( iPoint, this_x, this_y );
    if( this_y>=yMax ) yMax = this_y;
  }

  return yMax;

}
