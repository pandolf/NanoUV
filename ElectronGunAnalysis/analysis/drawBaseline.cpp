#include "../../CommonTools/interface/NanoUVCommon.h"
#include "../../ElectronGunAnalysis/interface/GunScanTool.h"

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




int main( int argc, char* argv[] ) {

  float gunEnergy = 900.;

  NanoUVCommon::setStyle();

  TLegend* legend = new TLegend( 0.55, 0.63, 0.9, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  std::vector<int> colors = NanoUVCommon::colors();

  std::vector< TGraphErrors* > graphs;

  for( float APDhv=350.; APDhv <= 380.; APDhv += 10. ) {

    std::string outdir( Form("plots/APDscans/%s/%.0feV/%.0fV", data.c_str(), gunEnergy, APDhv ) );
    system( Form("mkdir -p %s", outdir.c_str()) );

    TGraphErrors* gr_baseline_vs_igun  = new TGraphErrors(0);
    gr_baseline_vs_igun->SetName( Form( "V_{APD} = %.0f V", APDhv) );

    GunScanTool gst( gunEnergy, APDhv, data);
    //gst.set_currentMethod("integral");

    if( APDhv == 380. ) {

      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.0395, gst.getBaseline("22gen20_17_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.109 , gst.getBaseline("22gen20_13_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.3765, gst.getBaseline("22gen20_09_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 1.21  , gst.getBaseline("22gen20_05_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 9.96  , gst.getBaseline("23gen20_18_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 29.9  , gst.getBaseline("23gen20_14_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 186.  , gst.getBaseline("23gen20_01_M_.dat", -6.) );

    } else if( APDhv == 370. ) {

      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.0395, gst.getBaseline("22gen20_18_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.109 , gst.getBaseline("22gen20_14_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.3765, gst.getBaseline("22gen20_10_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 1.21  , gst.getBaseline("22gen20_06_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 9.96  , gst.getBaseline("23gen20_19_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 29.9  , gst.getBaseline("23gen20_15_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 186.  , gst.getBaseline("23gen20_03_M_.dat", -6.) );
    
    } else if( APDhv == 360. ) {

      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.0395, gst.getBaseline("22gen20_19_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.109 , gst.getBaseline("22gen20_15_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.3765, gst.getBaseline("22gen20_11_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 1.21  , gst.getBaseline("22gen20_07_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 9.96  , gst.getBaseline("23gen20_20_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 29.9  , gst.getBaseline("23gen20_16_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 186.  , gst.getBaseline("23gen20_04_M_.dat", -6.) );
    
    } else if( APDhv == 350. ) {

      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.0395, gst.getBaseline("22gen20_20_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.109 , gst.getBaseline("22gen20_16_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 0.3765, gst.getBaseline("22gen20_12_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 1.21  , gst.getBaseline("22gen20_08_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 9.96  , gst.getBaseline("23gen20_21_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 29.9  , gst.getBaseline("23gen20_17_M_.dat", -6.) );
      gr_baseline_vs_igun->SetPoint( gr_baseline_vs_igun->GetN(), 186.  , gst.getBaseline("23gen20_05_M_.dat", -6.) );
    
    } // APDhv

    gr_baseline_vs_igun->SetMarkerStyle( 20 ); 
    gr_baseline_vs_igun->SetMarkerSize( 1.4 ); 
    gr_baseline_vs_igun->SetMarkerColor( colors[graphs.size()] ); 
    gr_baseline_vs_igun->SetLineColor  ( colors[graphs.size()] ); 

    graphs.push_back( gr_baseline_vs_igun );

    legend->AddEntry( gr_baseline_vs_igun, gr_baseline_vs_igun->GetName(), "P" );

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

  c1->Clear();

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

