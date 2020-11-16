#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TFile.h"
#include "TF1.h"


void drawAll( const std::string& name );
float findVfixedThreshold( TGraphErrors* graph, float threshold, float& err_deltaV );
float interpolate( float this_x, float this_y, float next_x, float next_y, float threshold );


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

  TGraphErrors* gr_deltaV_vs_L = new TGraphErrors(0);

  for( unsigned i=0; i<v_hd.size(); ++i ) {

    TGraphErrors* graph = v_hd[i].getGraphFromColumns( Form("graph_%d", i), 1, 2, 1 );
    graph->SetMarkerColor( reds[i] );
    graph->SetLineColor  ( reds[i] );
    graph->Draw("PL same");
    legend->AddEntry( graph, Form("L = %.1f mm", v_hd[i].L()), "P" );

    float err_deltaV = 0.;
    float deltaV = findVfixedThreshold( graph, 200., err_deltaV );

    std::cout << "L: " << v_hd[i].L() << "   found deltaV: " << deltaV << std::endl;

    if( deltaV<0. ) {
      gr_deltaV_vs_L->SetPoint     ( i, v_hd[i].L(), deltaV );
      gr_deltaV_vs_L->SetPointError( i, 0., err_deltaV );
    }

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("Iapd_vs_CNThv_%s.pdf", name.c_str()) );


  // find E

  TF1* f1_line = new TF1( "line", "[0] + [1]*x", 25., 36. );
  gr_deltaV_vs_L->Fit( f1_line, "R" );

  delete h2_axes;
  delete c1;
  delete legend;

}



float findVfixedThreshold( TGraphErrors* graph, float threshold, float& err_deltaV ) {

  float deltaV = 9999.;

  for( unsigned iPoint = 0; iPoint<graph->GetN()-1; ++iPoint ) {

    double this_x, this_y;
    graph->GetPoint ( iPoint, this_x, this_y );
    double err_thisy = graph->GetErrorY( iPoint );

    double next_x, next_y;
    graph->GetPoint ( iPoint+1, next_x, next_y );
    double err_nexty = graph->GetErrorY( iPoint+1 );

    if( (this_y>threshold && next_y<threshold) || (this_y<threshold && next_y>threshold) ) {

      deltaV = interpolate( this_x, this_y, next_x, next_y, threshold );
      float deltaVup = interpolate( this_x, this_y+err_thisy, next_x, next_y+err_nexty, threshold );
      float deltaVdn = interpolate( this_x, this_y-err_thisy, next_x, next_y-err_nexty, threshold );
      err_deltaV = 0.5*(abs(deltaVup-deltaV) + abs(deltaVdn-deltaV));
      break;

    }

  } // for iPoints

  return deltaV;

}
    

float interpolate( float this_x, float this_y, float next_x, float next_y, float threshold ) {

  float m = (next_y-this_y)/(next_x-this_x);
  float deltaV =  (threshold-this_y)/m + this_x;
  return deltaV;

}
