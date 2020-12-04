#include "NanoUVCommon.h"
#include "SDD.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphErrors.h"

#include <iostream>


int main() {

  NanoUVCommon::setStyle();

  std::string led = "SP5605_SN030";

  std::map< float, std::string> files;
  files[1]    = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_1kHz_Measurements_Only_12_4_2020.root";
  //files[1.5]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_1p5kHz_Measurements_Only_12_4_2020.root";
  files[2]    = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_2kHz_Measurements_Only_12_4_2020.root";
  files[5]    = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_5kHz_Measurements_Only_12_4_2020.root";
  files[10]   = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_10kHz_Measurements_Only_12_4_2020.root";
  files[20]   = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_20kHz_Measurements_Only_12_4_2020.root";
  files[50]   = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_50kHz_Measurements_Only_12_4_2020.root";
  files[100]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_100kHz_Measurements_Only_12_4_2020.root";
  //files[200]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_200kHz_Measurements_Only_12_4_2020.root";
  //files[500]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_500kHz_Measurements_Only_12_4_2020.root";
  //files[1000] = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_1MHz_Measurements_Only_12_4_2020.root";
  //files[300]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_300kHz_Measurements_Only_12_4_2020.root";
  //files[400]  = "data/HyperionSDD/Run_SDD_G30_LEDUV_A10_400kHz_Measurements_Only_12_4_2020.root";

  std::vector<int> colors;
  colors.push_back( 15 );
  colors.push_back( 46 );
  colors.push_back( 38 );
  colors.push_back( 28 );
  colors.push_back( 42 );
  colors.push_back( kGray+3 );
  colors.push_back( kPink+1 );
  colors.push_back( 30 );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float xMax = 0.4;

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., 0.5 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.2, 0.6, 0.5, 0.9, "LED frequency" );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  
  TGraphErrors* gr_nPhPerShot_vs_freq = new TGraphErrors(0);
  TGraphErrors* gr_nPhPerSec_vs_freq = new TGraphErrors(0);

  int i=0;

  for( std::map< float, std::string>::iterator it=files.begin(); it!=files.end(); ++it ) { // map is automatically sorted by key

    TFile* file = TFile::Open( it->second.c_str() );
    TTree* tree = (TTree*)file->Get("tree");
    TH1D* h1 = SDD::fillFromTree( tree, Form("h1_%.0fkHz", it->first), "vamp", 30, 100, 0., xMax );

    h1->SetLineWidth(2);
    h1->SetLineColor(colors[i]);

    h1->DrawNormalized("same");

    legend->AddEntry( h1, Form("%.0f kHz", it->first), "L" );

    gr_nPhPerShot_vs_freq->SetPoint     ( i, it->first, h1->GetMean()*1000./5. );
    gr_nPhPerShot_vs_freq->SetPointError( i, 5., h1->GetRMS()*1000./5. );

    gr_nPhPerSec_vs_freq->SetPoint     ( i, it->first, it->first*h1->GetMean()*1000./5./1000000. ); // in MHz
    gr_nPhPerSec_vs_freq->SetPointError( i, 5., it->first*h1->GetRMS()*1000./5./1000000. );

    i++;

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/%s/LED_vs_frequency.pdf", led.c_str()) );


  c1->Clear();

  TH2D* h2_axes_2 = new TH2D( "axes_2", "", 10, 0., 180., 10, 0., 100000. );
  h2_axes_2->SetYTitle( "Number of photons per bunch" );
  h2_axes_2->SetXTitle( "LED repetition frequency [kHz]" );
  h2_axes_2->Draw();

  gr_nPhPerShot_vs_freq->SetMarkerStyle(20);
  gr_nPhPerShot_vs_freq->SetMarkerSize(1.6);
  gr_nPhPerShot_vs_freq->SetMarkerColor(kGray+3);
  gr_nPhPerShot_vs_freq->SetLineColor(kGray+3);
  gr_nPhPerShot_vs_freq->Draw("Psame");

  c1->SaveAs( Form("plots/%s/nPhotonsPerShot_vs_frequency.pdf", led.c_str()) );
  

  c1->Clear();

  TH2D* h2_axes_3 = new TH2D( "axes_3", "", 10, 0., 180., 10, 0., 10000. );
  h2_axes_3->SetYTitle( "Number of photons hitting SDD [MHz]" );
  h2_axes_3->SetXTitle( "LED repetition frequency [kHz]" );
  h2_axes_3->Draw();

  gr_nPhPerSec_vs_freq->SetMarkerStyle(20);
  gr_nPhPerSec_vs_freq->SetMarkerSize(1.6);
  gr_nPhPerSec_vs_freq->SetMarkerColor(kGray+3);
  gr_nPhPerSec_vs_freq->SetLineColor(kGray+3);
  gr_nPhPerSec_vs_freq->Draw("Psame");

  c1->SaveAs( Form("plots/%s/nPhotonsPerSec_vs_frequency.pdf", led.c_str()) );
  

  return 0;

}





















