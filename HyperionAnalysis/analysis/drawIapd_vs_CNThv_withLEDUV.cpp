#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TFile.h"
#include "TF1.h"


int main() {

  NanoUVCommon::setStyle();

  HyperionData hd("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_withLEDUV_L33_CNT50um_fusedITO.dat");

  TGraphErrors* gr_ledON  = hd.getGraphFromColumns( "gr_ledON" , 3, 4, 1 ); 
  TGraphErrors* gr_ledOFF = hd.getGraphFromColumns( "gr_ledOFF", 1, 2, 1 ); 

  gr_ledON->SetMarkerStyle(20);
  gr_ledON->SetMarkerSize(1.5);
  gr_ledON->SetMarkerColor(46);
  gr_ledON->SetLineColor(46);

  gr_ledOFF->SetMarkerStyle(20);
  gr_ledOFF->SetMarkerSize(1.5);
  gr_ledOFF->SetMarkerColor(38);
  gr_ledOFF->SetLineColor(38);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );

  float xMin = -1400.;
  float xMax = 99.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 98., 102. );
  h2_axes->SetXTitle( "#DeltaV [V]" );
  h2_axes->SetYTitle( "I_{apd} [nA]");
  h2_axes->Draw();

  gr_ledOFF->Draw("Psame");
  gr_ledON ->Draw("Psame");

  TLegend* legend = new TLegend( 0.6, 0.73, 0.9, 0.87 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_ledOFF, "UV LED: OFF", "P" );
  legend->AddEntry( gr_ledON , "UV LED: ON ", "P" );
  legend->Draw("psame");

  gPad->RedrawAxis();

  c1->SaveAs( "plots/CNT50um_fusedITO/Iapd_vs_CNThv_withLEDUV_L33.pdf" );


  TGraphErrors* gr_diff = new TGraphErrors(0);

  for( unsigned iPoint=0; iPoint<gr_ledON->GetN(); ++iPoint ) {

    double xON, yON;
    gr_ledON->GetPoint( iPoint, xON, yON );
    double err_yON = gr_ledON->GetErrorY( iPoint );

    double xOFF, yOFF;
    gr_ledOFF->GetPoint( iPoint, xOFF, yOFF );
    double err_yOFF = gr_ledOFF->GetErrorY( iPoint );

    gr_diff->SetPoint     ( iPoint, xON, yON-yOFF );
    gr_diff->SetPointError( iPoint, 1., sqrt( err_yON*err_yON + err_yOFF*err_yOFF ) );

  }
  
  gr_diff->SetMarkerStyle(20);
  gr_diff->SetMarkerSize(1.5);
  gr_diff->SetMarkerColor(kGray+3);
  gr_diff->SetLineColor(kGray+3);

  c1->Clear();

  
  TH2D* h2_axes2 = new TH2D( "axes2", "", 10, xMin, xMax, 10, 0., 1. );
  h2_axes2->SetXTitle( "#DeltaV [V]" );
  h2_axes2->SetYTitle( "I_{apd}(ON) - I_{apd}(OFF) [nA]");
  h2_axes2->Draw();

  gr_diff->Draw("Psame");

  gPad->RedrawAxis();

  c1->SaveAs( "plots/CNT50um_fusedITO/Iapd_vs_CNThv_withLEDUV_L33_diff.pdf" );

  delete c1;
  delete h2_axes;
  delete h2_axes2;

  return 0;

}

