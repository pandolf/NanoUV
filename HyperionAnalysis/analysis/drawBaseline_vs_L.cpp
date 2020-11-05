#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"



int main() {

  NanoUVCommon::setStyle();

  HyperionData hd( "data/HyperionAPD/baseline_vs_L.dat" );

  TGraphErrors* graph = hd.getGraphFromColumns("baseline_vs_L", 1);

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.5);
  graph->SetMarkerColor(kGray+3);
  graph->SetLineColor(kGray+3);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 25.5, 37.5, 10, 70., 115. );
  h2_axes->SetXTitle( "L [mm]" );
  h2_axes->SetYTitle( "Baseline Current [nA]" );
  h2_axes->Draw();

  graph->Draw("Psame");

  c1->SaveAs("baseline_vs_L.pdf");

  return 0;

}  
