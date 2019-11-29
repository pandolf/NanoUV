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



TGraph* getScanFromFile( const std::string& fileName );
TF1* fitDrift( TGraph* graph, int firstN=10, int lastN=5 );
TGraph* getCorrectedGraph( TGraph* graph, TF1* baseline );
double getYmax( TGraph* graph );



int main( int argc, char* argv[] ) {

  std::string fileName;

  if( argc == 1 ) {

    std::cout << "USAGE: ./drawAPDGunScan [scanFileName]" << std::endl;
    exit(1);

  } else {

    fileName = std::string(argv[1]);

  }

  NanoUVCommon::setStyle();

  TGraph* gr_scan = getScanFromFile( fileName );

  TF1* baseline = fitDrift( gr_scan );

  TGraph* gr_scan_corr = getCorrectedGraph( gr_scan, baseline );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  double xMin, xMax, yMin, y;
  gr_scan->GetPoint( 0, xMin, y );
  gr_scan->GetPoint( gr_scan->GetN()-1, xMax, yMin );

  // find max by hand
  double yMax = getYmax( gr_scan );

  yMin *= 0.98;
  yMax *= 1.10;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes->SetXTitle("Gun position"); 
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

  c1->SaveAs("testscan.pdf");
  c1->SaveAs("testscan.eps");

  c1->Clear();

  TH2D* h2_axes_corr = new TH2D( "axes_corr", "", 10, xMin, xMax, 10, -2., yMax-yMin );
  h2_axes_corr->SetXTitle("Gun position");
  h2_axes_corr->SetYTitle("Corrected APD Current [nA]");
  h2_axes_corr->Draw();

  TLine* line_zero = new TLine( xMin, 0., xMax, 0. );
  line_zero->SetLineWidth(2);
  line_zero->SetLineColor(46);
  line_zero->SetLineStyle(2);
  line_zero->Draw("L same");

  gr_scan_corr->SetMarkerStyle(20);
  gr_scan_corr->SetMarkerSize(1.6);
  gr_scan_corr->SetMarkerColor(kGray+3);
  gr_scan_corr->SetLineColor(kGray+3);
  gr_scan_corr->Draw("P same");

  label->Draw("same");  
  gPad->RedrawAxis();

  c1->SaveAs("testscanCorr.pdf");
  c1->SaveAs("testscanCorr.eps");

  double i_max = getYmax( gr_scan_corr );
  std::cout << "For this configuration the max APD current was: " << i_max << " nA" << std::endl;


  delete c1;
  delete h2_axes;

  return 0;

}


TGraph* getScanFromFile( const std::string& fileName ) {

  TGraph* graph = new TGraph(0);
  graph->SetName( Form( "gr_%s", fileName.c_str()) );

  std::string fullFileName( Form("data/2019_11_28/%s", fileName.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Opening: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x_gun, -i_apd*1E9 ); // make positive and convert to nA

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

  double xMin, xMax, yMin, yMax;
  graph->GetPoint( 0, xMin, yMin );
  graph->GetPoint( nPoints-1, xMax, yMax );

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
