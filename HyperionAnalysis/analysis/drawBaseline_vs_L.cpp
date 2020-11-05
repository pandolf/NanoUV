#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"



int main() {

  NanoUVCommon::setStyle();

  HyperionData hd_vacuum( "data/HyperionAPD/baseline_vs_L_vacuum.dat" );
  TGraphErrors* gr_vacuum = hd_vacuum.getGraphFromColumns("baseline_vs_L_vacuum", 1);

  HyperionData hd_air( "data/HyperionAPD/baseline_vs_L_air.dat" );
  TGraphErrors* gr_air = hd_air.getGraphFromColumns("baseline_vs_L_air", 1, 2);

  gr_vacuum->SetMarkerStyle(20);
  gr_vacuum->SetMarkerSize(1.5);
  gr_vacuum->SetMarkerColor(46);
  gr_vacuum->SetLineColor  (46);

  gr_air->SetMarkerStyle(20);
  gr_air->SetMarkerSize(1.5);
  gr_air->SetMarkerColor(kGray+3);
  gr_air->SetLineColor  (kGray+3);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 25.5, 37.5, 10, 60., 125. );
  h2_axes->SetXTitle( "L [mm]" );
  h2_axes->SetYTitle( "Baseline Current [nA]" );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( gr_vacuum, Form("P < %.1fE-5 mbar", hd_vacuum.p_before()/1E-5), "P" );
  legend->AddEntry( gr_air   , Form("P = 1 atm"), "P" );
  legend->Draw("same");

  gr_vacuum->Draw("Psame");
  gr_air->Draw("Psame");

  c1->SaveAs("baseline_vs_L.pdf");

  return 0;

}  
