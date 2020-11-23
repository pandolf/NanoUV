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
TGraphErrors* subtractBaseline( TGraphErrors* graph );


int main() {

  NanoUVCommon::setStyle();

  drawAll( "CNT50um_fusedITO" );
  drawAll( "CNT50um_fusedITO_B" );
  drawAll( "CNT50um_fused_A" );
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

  } else if( name=="CNT50um_fusedITO_B" ) {

    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L33_CNT50um_fusedITO_B.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L33p5_CNT50um_fusedITO_B.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L34_CNT50um_fusedITO_B.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L34p5_CNT50um_fusedITO_B.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L35_CNT50um_fusedITO_B.dat"  ) );

    xMin = -2100.;
    yMax = 1200.;

  } else if( name=="CNT50um_fused_A" ) {

    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L33_CNT50um_fused_A.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L33p5_CNT50um_fused_A.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L34_CNT50um_fused_A.dat"  ) );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L34p5_CNT50um_fused_A.dat") );
    v_hd.push_back( HyperionData("data/HyperionAPD/2020_11_20/Iapd_vs_CNThv_L35_CNT50um_fused_A.dat"  ) );

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


  std::string outdir(Form("plots/%s", name.c_str()));
  system( Form("mkdir -p %s", outdir.c_str()) );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  //c1->SetLogy();

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, 100., 10, 0., yMax);
  h2_axes->SetXTitle( "#DeltaV [V]" );
  h2_axes->SetYTitle( "I_{apd} [nA]" );
  h2_axes->Draw();


  TCanvas* c1_subtr = new TCanvas( "c1_subtr", "", 600, 600 );
  c1_subtr->cd();

  TH2D* h2_axes_subtr = new TH2D("axes_subtr", "", 10, xMin, 100., 10, 0., yMax);
  h2_axes_subtr->SetXTitle( "#DeltaV [V]" );
  h2_axes_subtr->SetYTitle( "I_{apd} (baseline subtracted) [nA]" );
  h2_axes_subtr->Draw();

  h2_axes_subtr->Draw();


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
    TGraphErrors* graph_subtr = subtractBaseline( graph );

    c1->cd();
    graph->SetMarkerColor( reds[i] );
    graph->SetLineColor  ( reds[i] );
    graph->Draw("PL same");

    c1_subtr->cd();
    graph_subtr->SetMarkerSize ( 1.5 );
    graph_subtr->SetMarkerStyle( 20 );
    graph_subtr->SetMarkerColor( reds[i] );
    graph_subtr->SetLineColor  ( reds[i] );
    graph_subtr->Draw("PL same");

    legend->AddEntry( graph, Form("L = %.1f mm", v_hd[i].L()), "P" );

  }

  c1->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/Iapd_vs_CNThv_%s.pdf", outdir.c_str(), name.c_str()) );

  c1_subtr->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_subtr->SaveAs( Form("%s/Iapd_vs_CNThv_%s_subtr.pdf", outdir.c_str(), name.c_str()) );




  std::vector<float> thresholds;
  if( name == "CNT50um_fusedITO" ) {
    thresholds.push_back( 25. );
    thresholds.push_back( 100. );
    thresholds.push_back( 200. );
    thresholds.push_back( 300. );
    thresholds.push_back( 400. );
  } else if( name == "CNT50um_fusedITO_B" ) {
    thresholds.push_back( 25. );
    thresholds.push_back( 100. );
    thresholds.push_back( 200. );
    thresholds.push_back( 300. );
  } else if( name == "CNT50um_fused_A" ) {
    thresholds.push_back( 25. );
    thresholds.push_back( 100. );
    thresholds.push_back( 200. );
    thresholds.push_back( 300. );
  } else if( name == "NanoFusilli_boroSiGlass" ) {
    thresholds.push_back( 100. );
    thresholds.push_back( 500. );
    thresholds.push_back( 1000. );
    thresholds.push_back( 2000. );
    thresholds.push_back( 3000. );
  }

  TGraphErrors* gr_d0_vs_thresh = new TGraphErrors(0);
  TGraphErrors* gr_E_vs_thresh  = new TGraphErrors(0);
  TGraphErrors* gr_thresh_vs_E  = new TGraphErrors(0);

  gr_d0_vs_thresh->SetName("d0_vs_thresh");
  gr_E_vs_thresh ->SetName("E_vs_thresh");
  gr_thresh_vs_E ->SetName("thresh_vs_E");


  for( unsigned ithresh=0; ithresh<thresholds.size(); ++ithresh ) {

    std::cout << "-> Starting threshold: " << thresholds[ithresh] << std::endl;

    TGraphErrors* gr_deltaV_vs_L = new TGraphErrors(0);
    gr_deltaV_vs_L->SetName( Form( "deltaV_vs_L_th%.0f", thresholds[ithresh]) );

 
    for( unsigned i=0; i<v_hd.size(); ++i ) {

      if( name=="NanoFusilli_boroSiGlass" && v_hd[i].L()<32. ) continue;

      TGraphErrors* graph = v_hd[i].getGraphFromColumns( Form("graph_%d", i), 1, 2, 1 );
      TGraphErrors* graph_subtr = subtractBaseline( graph );

      float err_deltaV = 0.;
      float deltaV = findVfixedThreshold( graph_subtr, thresholds[ithresh], err_deltaV );

      std::cout << "L: " << v_hd[i].L() << "   found deltaV: " << deltaV << std::endl;

      if( deltaV<0. ) {
        int iPoint = gr_deltaV_vs_L->GetN();
        gr_deltaV_vs_L->SetPoint     ( iPoint, -v_hd[i].L(), -deltaV );
        gr_deltaV_vs_L->SetPointError( iPoint, 0., err_deltaV );
      }

    } // for ipoints

    // find E

    TF1* f1_line = new TF1( "line", "[0] + [1]*x" );
    f1_line->SetLineColor(kGray+3);
    f1_line->SetLineWidth(2);
    gr_deltaV_vs_L->SetMarkerStyle(20);
    gr_deltaV_vs_L->SetMarkerSize(1.5);
    gr_deltaV_vs_L->SetMarkerColor(46);
    gr_deltaV_vs_L->SetLineColor(46);
    gr_deltaV_vs_L->Fit( f1_line, "X" );

    TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
    c2->cd();

    TH2D* h2_axes2 = new TH2D( "axes", "", 10, -37., -30., 10, 0., 2000. );
    h2_axes2->SetXTitle( "-L [mm]" );
    h2_axes2->SetYTitle( "#DeltaV [V]" );
    h2_axes2->Draw();

    gr_deltaV_vs_L->Draw("Psame");

    c2->SaveAs( Form("%s/deltaV_vs_L_th%.0f.pdf", outdir.c_str(), thresholds[ithresh]) );

    delete c2;
    delete h2_axes2;

    float m = f1_line->GetParameter(1);
    float q = f1_line->GetParameter(0);

    float err_m = f1_line->GetParError(1);
    float err_q = f1_line->GetParError(0);

    // L = d0-d
    float d0     = q/m;
    float err_d0 = sqrt( err_q*err_q/(m*m) + q*q*err_m*err_m/(m*m*m*m) );

    // DeltaV/d = E
    float E     = m; // in V/mm
    float err_E = err_m;

    gr_d0_vs_thresh->SetPoint     ( ithresh, thresholds[ithresh], d0 ); 
    gr_d0_vs_thresh->SetPointError( ithresh, 1., err_d0 );

    gr_E_vs_thresh->SetPoint      ( ithresh, thresholds[ithresh], E ); 
    gr_E_vs_thresh->SetPointError ( ithresh, 1., err_E );

    gr_thresh_vs_E->SetPoint      ( ithresh, E, thresholds[ithresh] );
    gr_thresh_vs_E->SetPointError ( ithresh, err_E, 1. );

  } // for ithresh



  TCanvas* c3 = new TCanvas( "c3", "", 600, 600 );
  c3->cd();

  float xMax3 = thresholds[thresholds.size()-1]*1.2;
  float yMin3 = 0.;
  float yMax3 = 1000.;

  TH2D* h2_axes3 = new TH2D( "axes3", "", 10, 0., xMax3, 10, yMin3, yMax3 );
  h2_axes3->SetXTitle( "I_{apd} Threshold (baseline subtracted) [nA]" );
  h2_axes3->SetYTitle( "E [V/mm]" );
  h2_axes3->Draw();

  TF1* f1_line2 = new TF1("line2", "[0]+[1]*x");
  f1_line2->SetLineColor(46);
  f1_line2->SetLineWidth(2);
  gr_E_vs_thresh->SetMarkerStyle(20);
  gr_E_vs_thresh->SetMarkerColor(46);
  gr_E_vs_thresh->SetLineColor  (46);
  gr_E_vs_thresh->SetMarkerSize(1.5);
  gr_E_vs_thresh->Fit( f1_line2, "X" );

  gr_E_vs_thresh->Draw("Psame");

  gPad->RedrawAxis();

  c3->SaveAs( Form("%s/E_vs_thresh.pdf", outdir.c_str()) );
 
  std::cout << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;
  std::cout << " -> " << name << " emit for E > " << f1_line2->GetParameter(0) << " +/- " << f1_line2->GetParError(0) << " V/mm" << std::endl;
  std::cout << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  c3->Clear();

  TH2D* h2_axes4 = new TH2D("axes4", "", 10, 0., xMax3, 10, 30., 45. );
  h2_axes4->SetXTitle( "I_{apd} Threshold [nA]" );
  h2_axes4->SetYTitle( "d_{0} [mm]" );
  h2_axes4->Draw();

  gr_d0_vs_thresh->SetMarkerStyle(20);
  gr_d0_vs_thresh->SetMarkerColor(46);
  gr_d0_vs_thresh->SetLineColor  (46);
  gr_d0_vs_thresh->SetMarkerSize(1.5);
  gr_d0_vs_thresh->Draw("Psame");

  gPad->RedrawAxis();

  c3->SaveAs( Form("%s/d0_vs_thresh.pdf", outdir.c_str()) );


  delete h2_axes;
  delete c1;
  delete legend;
  delete h2_axes3;
  delete h2_axes4;
  delete c1_subtr;
  delete h2_axes_subtr;
  delete c3;

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



TGraphErrors* subtractBaseline( TGraphErrors* graph ) {

  TGraphErrors* return_graph = new TGraphErrors(0);
  return_graph->SetName( Form("%s_subtr", graph->GetName()) );

  float baseline = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    if( x==0. ) {
      baseline = y;
      break;
    }    

  } // for points


  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    double x_err = graph->GetErrorX(iPoint);
    double y_err = graph->GetErrorY(iPoint);

    return_graph->SetPoint     ( iPoint, x, y-baseline );
    return_graph->SetPointError( iPoint, x_err, y_err );

  } // for points

  return return_graph;

}
