#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>

#include "TH2D.h"
#include "TLegend.h"



void drawThreeRatios( const std::string& name, const std::string& label,
                      const std::string& l1, int cnt1, int ac1, 
                      const std::string& l2, int cnt2, int ac2, 
                      const std::string& l3, int cnt3, int ac3 );


int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();


  drawThreeRatios( "VB_PolP_thetaA"    , "VB, E = 21 eV, Pol P", "E_{k} = 3.3 eV", 99 , 94 , "E_{k} = 10 eV", 98 , 95 , "E_{k} = 13.5 eV", 97 , 96  );
  drawThreeRatios( "VB_PolP_thetaA_ext", "VB, E = 21 eV, Pol P", "E_{k} = 3.3 eV", 170, 165, "E_{k} = 10 eV", 166, 163, "E_{k} = 13.5 eV", 169, 164 );
  drawThreeRatios( "C1S_PolP_thetaA"   , "C1S, Pol P", "h#nu = 310 eV", 136, 138, "h#nu = 340 eV", 55, 56, "h#nu = 400 eV", 62, 64 );

  return 0;

}



void drawThreeRatios( const std::string& name, const std::string& label,
                      const std::string& l1, int cnt1, int ac1, 
                      const std::string& l2, int cnt2, int ac2, 
                      const std::string& l3, int cnt3, int ac3 ) {

  BearScan* s_cnt1 = new BearScan(cnt1);
  BearScan* s_cnt2 = new BearScan(cnt2);
  BearScan* s_cnt3 = new BearScan(cnt3);

  BearScan* s_ac1  = new BearScan(ac1);
  BearScan* s_ac2  = new BearScan(ac2);
  BearScan* s_ac3  = new BearScan(ac3);

  TGraphErrors* gr_ratio1 = BearScan::getRatio( s_cnt1, s_ac1 );
  TGraphErrors* gr_ratio2 = BearScan::getRatio( s_cnt2, s_ac2 );
  TGraphErrors* gr_ratio3 = BearScan::getRatio( s_cnt3, s_ac3 );

  gr_ratio1->SetMarkerStyle(20);
  gr_ratio1->SetMarkerSize(0.8);
  gr_ratio1->SetMarkerColor(46);
  gr_ratio1->SetLineColor  (46);

  gr_ratio2->SetMarkerStyle(20);
  gr_ratio2->SetMarkerSize(0.8);
  gr_ratio2->SetMarkerColor(38);
  gr_ratio2->SetLineColor  (38);

  gr_ratio3->SetMarkerStyle(20);
  gr_ratio3->SetMarkerSize(0.8);
  gr_ratio3->SetMarkerColor(29);
  gr_ratio3->SetLineColor  (29);


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float xMin, xMax, yMin, yMax;
  NanoUVCommon::findGraphRanges( gr_ratio3, xMin, xMax, yMin, yMax );

  float yMaxPlot = (yMax>10.) ? 1.1*yMax : 10.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., yMaxPlot );
  h2_axes->SetYTitle( "CNT / aC" );
  h2_axes->SetXTitle( s_cnt1->getXtitle().c_str() );
  h2_axes->Draw("same");

  TLine* lineOne = new TLine( xMin, 1., xMax, 1. );
  lineOne->Draw("same");

  gr_ratio1->Draw("PLSame");
  gr_ratio2->Draw("PLSame");
  gr_ratio3->Draw("PLSame");

  TLegend* legend = new TLegend( 0.2, 0.65, 0.55, 0.9, label.c_str() );
  legend->SetFillColor(0);
  legend->SetTextSize( 0.038 );
  legend->AddEntry( gr_ratio1, l1.c_str(), "PL" );
  legend->AddEntry( gr_ratio2, l2.c_str(), "PL" );
  legend->AddEntry( gr_ratio3, l3.c_str(), "PL" );
  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/ratioAll_%s.pdf", name.c_str()) );

  delete h2_axes;
  delete c1;

}


