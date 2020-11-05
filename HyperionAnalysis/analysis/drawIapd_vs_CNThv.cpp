#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"



int main() {

  NanoUVCommon::setStyle();

  std::vector< HyperionData > v_hd;
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L32_CNT50um_fusedITO.dat"  ) );
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L33_CNT50um_fusedITO.dat"  ) );
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L33p5_CNT50um_fusedITO.dat") );
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L34_CNT50um_fusedITO.dat"  ) );
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L34p5_CNT50um_fusedITO.dat") );
  v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L35_CNT50um_fusedITO.dat"  ) );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  //c1->SetLogy();

  TH2D* h2_axes = new TH2D("axes", "", 10, -2100., 0., 10, 0., 1200.);
  h2_axes->SetXTitle( "#DeltaV [V]" );
  h2_axes->SetYTitle( "I_{apd} [nA]" );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.68, 0.6, 0.9, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> reds;
  reds.push_back( kRed-9 );
  reds.push_back( kRed-7 );
  reds.push_back( kRed );
  reds.push_back( kRed+1 );
  reds.push_back( kRed+2 );
  reds.push_back( kRed+3 );

  for( unsigned i=0; i<v_hd.size(); ++i ) {

    TGraphErrors* graph = v_hd[i].getGraphFromColumns( Form("graph_%d", i), 1, 2 );
    graph->SetMarkerColor( reds[i] );
    graph->SetLineColor  ( reds[i] );
    graph->Draw("PL same");
    legend->AddEntry( graph, Form("L = %.1f mm", v_hd[i].L()), "P" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("Iapd_vs_CNThv.pdf");

  return 0;

}
//-rw-r--r--  1 pandolf  staff  222 Nov  5 12:02 Iapd_vs_CNThv_L35_CNT50um_fusedITO_air.dat
//
//-rw-r--r--  1 pandolf  staff  239 Nov  5 12:08 Iapd_vs_CNThv_L32_noCNT_air.dat
//-rw-r--r--  1 pandolf  staff  241 Nov  5 12:06 Iapd_vs_CNThv_L33_noCNT_air.dat
//-rw-r--r--  1 pandolf  staff  358 Nov  5 12:06 Iapd_vs_CNThv_L34_noCNT_air.dat
