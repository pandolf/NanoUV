#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"


void drawAll( const std::string& name );


int main() {

  NanoUVCommon::setStyle();

  drawAll( "CNT50um_fusedITO" );
  drawAll( "NanoFusilli_boroSiGlass" );

  return 0;

}


void drawAll( const std::string& name ) {

  std::cout << std::endl << "-> Starting: " << name << std::endl;

  std::vector< HyperionData > v_hd;

  float xMin = -2100.;
  float yMax = 1500.;

  if( name=="CNT50um_fusedITO" ) {

    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L32_CNT50um_fusedITO.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L33_CNT50um_fusedITO.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L33p5_CNT50um_fusedITO.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L34_CNT50um_fusedITO.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L34p5_CNT50um_fusedITO.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_L35_CNT50um_fusedITO.dat"  ) );

    xMin = -2100.;
    yMax = 1200.;

  } else if (name=="NanoFusilli_boroSiGlass" ) {

    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L31p5_NanoFusilli_boroSiGlass.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L32_NanoFusilli_boroSiGlass.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L32p5_NanoFusilli_boroSiGlass.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L33_NanoFusilli_boroSiGlass.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L33p5_NanoFusilli_boroSiGlass.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_10_30/Iapd_vs_CNThv_L34_NanoFusilli_boroSiGlass.dat"  ) );

    xMin = -700.;
    yMax = 7000.;

  } else {

    std::cout << "-> Unkown name: " << name << std::endl;

  }


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  //c1->SetLogy();

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, 100., 10, 0., yMax);
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

    TGraphErrors* graph = v_hd[i].getGraphFromColumns( Form("graph_%d", i), 1, 2, 1 );
    graph->SetMarkerColor( reds[i] );
    graph->SetLineColor  ( reds[i] );
    graph->Draw("PL same");
    legend->AddEntry( graph, Form("L = %.1f mm", v_hd[i].L()), "P" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("Iapd_vs_CNThv_%s.pdf", name.c_str()) );

  delete h2_axes;
  delete c1;
  delete legend;

}
