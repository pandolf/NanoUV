#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "interface/NanoUVCommon.h"




int main() {


  NanoUVCommon::setStyle();

  TFile* file     = TFile::Open("data/PMT_LEDUV/Run_HV700_A10_Measurements_Only_9_1_2020.root");
  TFile* file_LiF = TFile::Open("data/PMT_LEDUV/Run_HV700_A10_LiF_Measurements_Only_9_1_2020.root");

  TTree* tree     = (TTree*)file    ->Get("tree");
  TTree* tree_LiF = (TTree*)file_LiF->Get("tree");

  std::string varName = "vcharge";
  //std::string varName = "vamp";

  float xMin = (varName=="vcharge") ? -209. : -1.2;
  float xMax = (varName=="vcharge") ? -151. : -1.0;

  TH1D* h1_vcharge     = new TH1D("h"    , "", 100, xMin, xMax);
  TH1D* h1_vcharge_LiF = new TH1D("h_LiF", "", 100, xMin, xMax);

  tree    ->Project( "h"    , varName.c_str() );
  tree_LiF->Project( "h_LiF", varName.c_str() );


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 1.3*h1_vcharge->GetMaximum() );
  if( varName == "vcharge" )
    h2_axes->SetXTitle( "PMT Charge [au]");
  else
    h2_axes->SetXTitle( "PMT Amplitude [au]");
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw("");

  h1_vcharge->SetLineColor(38);
  h1_vcharge->SetLineWidth(3);

  h1_vcharge_LiF->SetLineColor(46);
  h1_vcharge_LiF->SetLineWidth(3);

  h1_vcharge->Draw("same");
  h1_vcharge_LiF->Draw("same");

  TLegend* legend = new TLegend( 0.5, 0.5, 0.88, 0.64 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( h1_vcharge, "Hamamatsu R7378A", "L" );
  legend->AddEntry( h1_vcharge_LiF, "+ LiF window (2 mm)", "L" );
  legend->Draw("same");

  TPaveText* label_led = new TPaveText( 0.2, 0.79, 0.44, 0.89, "brNDC" );
  label_led->SetFillColor(0);
  label_led->SetTextColor(kGray+1);
  label_led->SetTextSize(0.035);
  label_led->SetTextAlign( 11 );
  label_led->AddText( "CAEN UV LED Driver SP5605" );
  label_led->AddText( "#lambda = 248 nm (E = 5 eV)");
  label_led->Draw("same");

  TPaveText* label_LiF = new TPaveText( 0.6, 0.3, 0.78, 0.4, "brNDC" );
  label_LiF->SetFillColor(0);
  label_LiF->SetTextSize(0.04);
  label_LiF->AddText( Form("T(LiF) = %.1f%%", 100.*h1_vcharge_LiF->GetMean()/h1_vcharge->GetMean()) );
  label_LiF->Draw("same");


  gPad->RedrawAxis();

  c1->SaveAs( "PMT_LiF.pdf" );

  return 0;

} 
