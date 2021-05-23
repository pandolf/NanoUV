#include "../interface/BearScan.h"
#include "interface/NanoUVCommon.h"

#include <iostream>

#include "TH2D.h"
#include "TLegend.h"





void drawPsiC( const std::string& name, const std::string& labelText, int cnt, int ac );


int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  drawPsiC( "VB_theta30_Ek10", "VB, E_{k} = 10 eV, #theta_{A} = -30#circ", 153, 155 );


  return 0;

}


void drawPsiC( const std::string& name, const std::string& labelText, int cnt, int ac ) {

  BearScan s_cnt(cnt);
  BearScan s_ac (ac );

  TGraphErrors* gr_cnt = s_cnt.averageSameX();
  TGraphErrors* gr_ac  = s_ac .averageSameX();

  float xMin, xMax, yMin, yMax;
  NanoUVCommon::findGraphRanges( gr_cnt, xMin, xMax, yMin, yMax );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., yMax*1.2 );
  h2_axes->SetXTitle( s_cnt.getXtitle().c_str() );
  h2_axes->SetYTitle( "Counts" );
  h2_axes->Draw();

  gr_cnt->SetMarkerStyle(20);
  gr_cnt->SetMarkerSize(0.8);
  gr_cnt->SetMarkerColor(46);
  gr_cnt->SetLineColor(46);

  gr_ac->SetMarkerStyle(20);
  gr_ac->SetMarkerSize(0.8);
  gr_ac->SetMarkerColor(38);
  gr_ac->SetLineColor(38);

  TLegend* legend = new TLegend( 0.2, 0.4, 0.55, 0.6, labelText.c_str() );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  legend->AddEntry( gr_cnt, "CNT", "PL" );
  legend->AddEntry( gr_ac , "aC", "PL" );
  legend->Draw("same");

  gr_cnt->Draw("PLsame");
  gr_ac->Draw("PLsame");

  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/psiC_%s.pdf", name.c_str()) );

  c1->Clear();

  TH2D* h2_axes_ratio = new TH2D( "axes_ratio", "", 10, xMin, xMax, 10, 0., 8. );
  h2_axes_ratio->SetXTitle( s_cnt.getXtitle().c_str() );
  h2_axes_ratio->SetYTitle( "CNT / aC" );
  h2_axes_ratio->Draw();

  TGraphErrors* gr_ratio = NanoUVCommon::getGraphRatio( gr_cnt, gr_ac );
  gr_ratio->SetMarkerStyle(20);
  gr_ratio->SetMarkerSize(0.8);
  gr_ratio->SetMarkerColor(46);
  gr_ratio->SetLineColor(46);

  gr_ratio->Draw("PLsame");

  TPaveText* label = new TPaveText( 0.2, 0.8, 0.55, 0.9, "brNDC" );
  label->SetTextSize(0.035);
  label->SetFillColor(0);
  label->AddText( labelText.c_str() );
  label->Draw("same");

  c1->SaveAs( Form("plots/psiC_ratio_%s.pdf", name.c_str()) );

  delete c1;
  delete h2_axes;
  delete h2_axes_ratio;

}


