#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>

#include "TH2D.h"



int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc<3 ) {
    std::cout << "USAGE: ./drawScanRatio [scanNum] [scanDenom]" << std::endl;
    exit(1);
  }

  int scanNumber1( atoi(argv[1]) );
  int scanNumber2( atoi(argv[2]) );

  BearScan* s1 = new BearScan(scanNumber1);
  BearScan* s2 = new BearScan(scanNumber2);

  s1->drawGraphs();
  s2->drawGraphs();

  TGraphErrors* gr_ratio = BearScan::getRatio( s1, s2 );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  gr_ratio->SetMarkerStyle(20);
  gr_ratio->SetMarkerSize(0.8);
  gr_ratio->SetMarkerColor(46);
  gr_ratio->SetLineColor  (46);

  float xMin, xMax, yMin, yMax;
  NanoUVCommon::findGraphRanges( gr_ratio, xMin, xMax, yMin, yMax );
  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 15. );
  h2_axes->SetYTitle( "CNT / aC" );
  h2_axes->SetXTitle( s1->getXtitle().c_str() );
  h2_axes->Draw("same");

  gr_ratio->Draw("PLSame");

  c1->SaveAs( Form("plots/ratio_%d_%d.pdf", s1->number(), s2->number()) );

  gPad->RedrawAxis();

  return 0;

}


