#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>

#include "TH2D.h"
#include "TLegend.h"




void drawThreeRatios( const std::string& name, const std::string& label,
                      const std::string& l1, BearScan s_cnt1, BearScan s_ac1, 
                      const std::string& l2, BearScan s_cnt2, BearScan s_ac2,
                      const std::string& l3, BearScan s_cnt3, BearScan s_ac3 );



int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();


  drawThreeRatios( "VB_PolP_thetaA"    , "VB, E = 21 eV, Pol P", "E_{k} = 3.3 eV", BearScan(99) , BearScan(94) , "E_{k} = 10 eV", BearScan(98) , BearScan(95) , "E_{k} = 13.5 eV", BearScan(97) , BearScan(96 ) );
  drawThreeRatios( "VB_PolP_thetaA_ext", "VB, E = 21 eV, Pol P", "E_{k} = 3.3 eV", BearScan(170), BearScan(165), "E_{k} = 10 eV", BearScan(166), BearScan(163), "E_{k} = 13.5 eV", BearScan(169), BearScan(164) );

  drawThreeRatios( "VB_PolP_phiA"      , "VB, E = 21 eV, Pol P", "E_{k} = 3.3 eV", BearScan::mergeScans(111,112), BearScan::mergeScans(118,119), "E_{k} = 10 eV", BearScan::mergeScans(106,107) , BearScan::mergeScans(114,115) , "E_{k} = 13.5 eV", BearScan::mergeScans(109,110) , BearScan::mergeScans(116,117) );

  drawThreeRatios( "C1S_PolP_thetaA"   , "C1S, Pol P"          , "h#nu = 310 eV" , BearScan(136), BearScan(138), "h#nu = 340 eV", BearScan(55) , BearScan(56) , "h#nu = 400 eV"  , BearScan(62) , BearScan(64 ) );


  return 0;

}



void drawThreeRatios( const std::string& name, const std::string& label,
                      const std::string& l1, BearScan s_cnt1, BearScan s_ac1, 
                      const std::string& l2, BearScan s_cnt2, BearScan s_ac2,
                      const std::string& l3, BearScan s_cnt3, BearScan s_ac3 ) {


  TGraphErrors* gr_ratio1 = BearScan::getRatio( &s_cnt1, &s_ac1 );
  TGraphErrors* gr_ratio2 = BearScan::getRatio( &s_cnt2, &s_ac2 );
  TGraphErrors* gr_ratio3 = BearScan::getRatio( &s_cnt3, &s_ac3 );

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
  h2_axes->SetXTitle( s_cnt1.getXtitle().c_str() );
  h2_axes->Draw("same");

  TLine* lineOne = new TLine( xMin, 1., xMax, 1. );
  lineOne->Draw("same");

  TLegend* legend = new TLegend( 0.2, 0.65, 0.55, 0.9, label.c_str() );
  legend->SetFillColor(0);
  legend->SetTextSize( 0.038 );
  legend->AddEntry( gr_ratio1, l1.c_str(), "PL" );
  legend->AddEntry( gr_ratio2, l2.c_str(), "PL" );
  legend->AddEntry( gr_ratio3, l3.c_str(), "PL" );
  legend->Draw("same");

  gr_ratio1->Draw("PLSame");
  gr_ratio2->Draw("PLSame");
  gr_ratio3->Draw("PLSame");

  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/ratioAll_%s.pdf", name.c_str()) );

  delete h2_axes;
  delete c1;

}


