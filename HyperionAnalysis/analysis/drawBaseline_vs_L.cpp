#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"



int main() {

  NanoUVCommon::setStyle();

  HyperionData hd_vacuum( "data/HyperionAPD/2020_10_28/baseline_vs_L_vacuum.dat" );
  TGraphErrors* gr_vacuum = hd_vacuum.getGraphFromColumns("baseline_vs_L_vacuum", 1);

  HyperionData hd_vacuum2( "data/HyperionAPD/2020_10_29/baseline_vs_L_vacuum.dat" );
  TGraphErrors* gr_vacuum2 = hd_vacuum2.getGraphFromColumns("baseline_vs_L_vacuum2", 1);

  HyperionData hd_air( "data/HyperionAPD/2020_10_29/baseline_vs_L_air.dat" );
  TGraphErrors* gr_air = hd_air.getGraphFromColumns("baseline_vs_L_air", 1, 2);

  gr_vacuum->SetMarkerStyle(20);
  gr_vacuum->SetMarkerSize(1.5);
  gr_vacuum->SetMarkerColor(46);
  gr_vacuum->SetLineColor  (46);

  gr_vacuum2->SetMarkerStyle(20);
  gr_vacuum2->SetMarkerSize(1.5);
  gr_vacuum2->SetMarkerColor(38);
  gr_vacuum2->SetLineColor  (38);

  gr_air->SetMarkerStyle(20);
  gr_air->SetMarkerSize(1.5);
  gr_air->SetMarkerColor(kGray+3);
  gr_air->SetLineColor  (kGray+3);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 25.5, 37.5, 10, 0., 150. );
  //TH2D* h2_axes = new TH2D( "axes", "", 10, 25.5, 37.5, 10, 65., 145. );
  h2_axes->SetXTitle( "L [mm]" );
  h2_axes->SetYTitle( "Baseline Current [nA]" );
  h2_axes->Draw();

  //TLegend* legend = new TLegend( 0.6, 0.65, 0.9, 0.9 );
  TLegend* legend = new TLegend( 0.2, 0.2, 0.5, 0.4 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( gr_air    , Form("1 atm"), "P" );
  legend->AddEntry( gr_vacuum , Form("%.1fE-5 mbar", hd_vacuum .p_ave()/1E-5), "P" );
  legend->AddEntry( gr_vacuum2, Form("%.1fE-6 mbar", hd_vacuum2.p_ave()/1E-6), "P" );
  legend->Draw("same");

  gr_vacuum->Draw("Psame");
  gr_vacuum2->Draw("Psame");
  gr_air->Draw("Psame");

  c1->SaveAs("baseline_vs_L.pdf");

  return 0;

}  
