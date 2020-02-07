#include "../../CommonTools/interface/NanoUVCommon.h"
#include "../../ElectronGunAnalysis/interface/GunScan.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"



std::string data = "2020_01";
//std::string data = "2019_11_28";

void addBaselineMeanPoint( TGraphErrors* gr_baseline_vs_APDhv, float APDhv, TGraphErrors* gr_baseline_vs_igun );
TGraphErrors* normalizeToFirstPoint( TGraphErrors* graph );



int main( int argc, char* argv[] ) {

  float gunEnergy = 900.;

  NanoUVCommon::setStyle();

  TLegend* legend = new TLegend( 0.6, 0.65, 0.9, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> colors = NanoUVCommon::colors();

  std::vector< TGraphErrors* > graphs;
  TGraphErrors* gr_baseline_vs_APDhv = new TGraphErrors(0);

  float x0 = -6.; // point at which the baseline will be evaluated (should be at APD center)


  for( float APDhv=350.; APDhv <= 380.; APDhv += 10. ) {

    std::vector< GunScan* > scans  = GunScan::loadScans( Form("data/%s/scans.dat", data.c_str()), gunEnergy, APDhv );

    TGraphErrors* gr_baseline_vs_igun  = new TGraphErrors(0);
    gr_baseline_vs_igun->SetName( Form( "V_{APD} = %.0f V", APDhv) );

    for( unsigned iScan=0; iScan<scans.size(); ++iScan )
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), scans[iScan]->gunCurrent(), scans[iScan]->getBaseline(x0) );

    gr_baseline_vs_igun->SetMarkerStyle( 20 ); 
    gr_baseline_vs_igun->SetMarkerSize( 1.4 ); 
    gr_baseline_vs_igun->SetMarkerColor( colors[graphs.size()] ); 
    gr_baseline_vs_igun->SetLineColor  ( colors[graphs.size()] ); 

    graphs.push_back( gr_baseline_vs_igun );

    legend->AddEntry( gr_baseline_vs_igun, gr_baseline_vs_igun->GetName(), "P" );

    addBaselineMeanPoint( gr_baseline_vs_APDhv, APDhv, gr_baseline_vs_igun );


  } // for APDhv


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  c1->SetLogx();


  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.01, 500., 10, 0., 260. );
  h2_axes->SetXTitle( "Gun Current [pA]" );
  h2_axes->SetYTitle( "Baseline Current (at x_{gun} = -6 mm) [pA]" );
  h2_axes->Draw();

  TPaveText* label_settings = new TPaveText( 0.23, 0.7, 0.5, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");


  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();

  legend->Draw("same");

  for( unsigned i=0; i<graphs.size(); ++i ) graphs[i]->Draw("PL same");


  std::string outdir( Form("plots/APDscans/%s/%.0feV", data.c_str(), gunEnergy) );
  system( Form("mkdir -p %s", outdir.c_str()) );
  c1->SaveAs( Form("%s/baseline_vs_igun.pdf", outdir.c_str()) );
  c1->SaveAs( Form("%s/baseline_vs_igun.eps", outdir.c_str()) );

  delete c1;
  delete h2_axes;


  std::vector< std::pair< float, float > > gunCurrents;
  gunCurrents.push_back( std::pair< float, float > (0.03, 0.04) );
  gunCurrents.push_back( std::pair< float, float > (0.1 ,  0.2) );
  gunCurrents.push_back( std::pair< float, float > (1.  ,   2.) );
  gunCurrents.push_back( std::pair< float, float > (9.  ,  11.) );
  gunCurrents.push_back( std::pair< float, float > (29. ,  30.) );
  gunCurrents.push_back( std::pair< float, float > (100., 200.) );

  std::vector< GunScan* > scans  = GunScan::loadScans( Form("data/%s/scans.dat", data.c_str()), gunEnergy);

  for( unsigned iCurrent=0; iCurrent<gunCurrents.size(); ++iCurrent ) {

    TGraphErrors* gr_iapd_vs_APDhv = new TGraphErrors(0);
    TGraphErrors* gr_baseline_vs_APDhv = new TGraphErrors(0);

    gr_iapd_vs_APDhv    ->SetName( Form("gr_iapd_vs_APDhv_i%d"    , iCurrent) );
    gr_baseline_vs_APDhv->SetName( Form("gr_baseline_vs_APDhv_i%d", iCurrent) );

    std::string gunCurrentText;

    for( unsigned iScan=0; iScan<scans.size(); ++iScan ) {

      if( scans[iScan]->gunCurrent() < gunCurrents[iCurrent].first  ) continue;
      if( scans[iScan]->gunCurrent() > gunCurrents[iCurrent].second ) continue;

      scans[iScan]->set_currentMethod("point");
      scans[iScan]->set_currentEvalPoint(-6.);
      gunCurrentText = scans[iScan]->gunCurrentText();

      gr_iapd_vs_APDhv    ->SetPoint( gr_iapd_vs_APDhv    ->GetN(), scans[iScan]->APDhv(), scans[iScan]->getCurrentFromScan() );
      gr_baseline_vs_APDhv->SetPoint( gr_baseline_vs_APDhv->GetN(), scans[iScan]->APDhv(), scans[iScan]->getBaseline(x0) );

    } // for scans

    TGraphErrors* gr_iapd_vs_APDhv_norm     = normalizeToFirstPoint( gr_iapd_vs_APDhv );
    TGraphErrors* gr_baseline_vs_APDhv_norm = normalizeToFirstPoint( gr_baseline_vs_APDhv );

    TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
    c1->cd();

    float xMin = 341.;
    float xMax = 389.;

    TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 1.2*GunScan::getYmax(gr_iapd_vs_APDhv_norm) );
    h2_axes->SetXTitle( "V_{APD} [V]" );
    h2_axes->SetYTitle( "Relative Current Intensity" );
    h2_axes->Draw();

    TLegend* legend = new TLegend( 0.25, 0.5, 0.5, 0.65 );
    legend->SetFillColor(0);
    legend->SetTextSize(0.035);
    legend->AddEntry( gr_iapd_vs_APDhv_norm, "I_{APD}", "LP" );
    legend->AddEntry( gr_baseline_vs_APDhv_norm, "Baseline", "LP" );

    legend->Draw("same");

    gr_baseline_vs_APDhv_norm->SetMarkerStyle(24);
    gr_baseline_vs_APDhv_norm->SetMarkerSize(1.6);
    gr_baseline_vs_APDhv_norm->SetMarkerColor(46);
    gr_baseline_vs_APDhv_norm->SetLineColor(46);
    gr_baseline_vs_APDhv_norm->Draw("PL same");

    gr_iapd_vs_APDhv_norm->SetMarkerStyle(20);
    gr_iapd_vs_APDhv_norm->SetMarkerSize(1.6);
    gr_iapd_vs_APDhv_norm->SetMarkerColor(46);
    gr_iapd_vs_APDhv_norm->SetLineColor(46);
    gr_iapd_vs_APDhv_norm->Draw("PL same");

    TPaveText* label_gun = new TPaveText( 0.2, 0.65, 0.5, 0.85, "brNDC" );
    //TPaveText* label_gun = new TPaveText( 0.25, 0.3, 0.5, 0.4, "brNDC" );
    //TPaveText* label_gun = new TPaveText( 0.7, 0.2, 0.9, 0.3, "brNDC" );
    label_gun->SetTextAlign(11);
    label_gun->SetFillColor(0);
    label_gun->SetTextSize(0.035);
    label_gun->AddText( Form("%s", gunCurrentText.c_str()) );
    label_gun->Draw("same");

    NanoUVCommon::addNanoUVLabel( c1, 2 );

    c1->SaveAs( Form("%s/ratios_vs_APDhv_i%d.eps", outdir.c_str(), iCurrent) );
    c1->SaveAs( Form("%s/ratios_vs_APDhv_i%d.pdf", outdir.c_str(), iCurrent) );

    delete c1;
    delete h2_axes;
    delete legend;
    //delete line_one;

  } // for iCurrent



  //c1->SetLogx();
  //c1->SetLogy();

  //float yMin = (gst.currentMethod()=="integral") ? 30. : 10.;

  //TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, 0.01, xMax_log, 10, yMin, yMax_log );
  //h2_axes_log->SetXTitle( "Gun Current [pA]" );
  //h2_axes_log->SetYTitle( "APD Current [pA]" );
  //h2_axes_log->Draw();

  //label_settings->Draw("same");

  //label->Draw("same");
  //gPad->RedrawAxis();


  //TF1* f1_lineLog = new TF1( "lineScan_log", "[0] + [1]*x", 0., xMax_log );
  //f1_lineLog->SetParameter( 0, f1_line->GetParameter(0) );
  //f1_lineLog->SetParameter( 1, f1_line->GetParameter(1) );
  //f1_lineLog->SetLineWidth(2);
  //f1_lineLog->SetLineColor(46);
  //f1_lineLog->SetLineStyle(2);
  //f1_lineLog->Draw("L same");

  //gr_baseline_vs_igun ->Draw("P same");
  //gr_baseline_vs_igun2->Draw("P same");

  //fitResults->Draw("same");

  //c1->SaveAs( Form("%s/iapd_vs_igun_log.pdf", outdir.c_str()) );
  //c1->SaveAs( Form("%s/iapd_vs_igun_log.eps", outdir.c_str()) );


  
  return 0;

}


void addBaselineMeanPoint( TGraphErrors* gr_baseline_vs_APDhv, float APDhv, TGraphErrors* gr_baseline_vs_igun ) {

  float meanBaseline = gr_baseline_vs_igun->GetMean(2);
  float rmsBaseline = gr_baseline_vs_igun->GetRMS(2);

  int thisPoint = gr_baseline_vs_APDhv->GetN();

  gr_baseline_vs_APDhv->SetPoint     ( thisPoint, APDhv, meanBaseline );
  gr_baseline_vs_APDhv->SetPointError( thisPoint,    0.,  rmsBaseline );

}



TGraphErrors* normalizeToFirstPoint( TGraphErrors* graph ) {

  double x0=999999., y0;

  // first round: find first point (the one with lowest x)
  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y);

    if( x<x0 ) {
      x0 = x;
      y0 = y;
    }

  } // for points

  TGraphErrors* graph_norm = new TGraphErrors(0);
  graph_norm->SetName( Form("%s_norm", graph->GetName()) );

  // second round: create normalized graph
  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y);

    graph_norm->SetPoint( graph_norm->GetN(), x, y/y0 );

  } // for points

  return graph_norm;

}
