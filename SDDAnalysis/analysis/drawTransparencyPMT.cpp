#include <iostream>

#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "interface/NanoUVCommon.h"



void drawTransparency(  const std::string& fileName1, const std::string& fileName2, const std::string& name, const std::string& legendName );




int main() {


  NanoUVCommon::setStyle();

  drawTransparency( "Run_HV700_A10_Measurements_Only_9_1_2020", "Run_HV700_A10_LiF_Measurements_Only_9_1_2020", "LiF", "LiF window (2 mm)" ) ;
  drawTransparency( "Run_HV700_A10_Measurements_Only_9_2_2020", "Run_HV700_A10_Fused_Measurements_Only_9_2_2020", "Fused", "fused silica (500 #mum)" ) ;

  return 0;

}




void drawTransparency(  const std::string& fileName1, const std::string& fileName2, const std::string& name, const std::string& legendName ) {

  TFile* file     = TFile::Open( Form("data/PMT_LEDUV/%s.root", fileName1.c_str()));
  TFile* file_LiF = TFile::Open( Form("data/PMT_LEDUV/%s.root", fileName2.c_str()));

  TTree* tree     = (TTree*)file    ->Get("tree");
  TTree* tree_LiF = (TTree*)file_LiF->Get("tree");

  std::string varName = "vcharge";
  //std::string varName = "vamp";

  float xMin = (varName=="vcharge") ? -249. : -1.2;
  float xMax = (varName=="vcharge") ? -151. : -1.0;

  TH1D* h1_vcharge     = new TH1D( Form("h_%s"    , name.c_str()), "", 200, xMin, xMax);
  TH1D* h1_vcharge_LiF = new TH1D( Form("h_%s_LiF", name.c_str()), "", 200, xMin, xMax);

  tree    ->Project( h1_vcharge    ->GetName(), varName.c_str() );
  tree_LiF->Project( h1_vcharge_LiF->GetName(), varName.c_str() );


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, h1_vcharge->GetMean()*1.07, xMax, 10, 0., 1.3*h1_vcharge->GetMaximum() );
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
  legend->AddEntry( h1_vcharge_LiF, Form("+ %s", legendName.c_str()), "L" );
  legend->Draw("same");

  TPaveText* label_led = new TPaveText( 0.2, 0.79, 0.44, 0.89, "brNDC" );
  label_led->SetFillColor(0);
  label_led->SetTextColor(kGray+1);
  label_led->SetTextSize(0.035);
  label_led->SetTextAlign( 11 );
  label_led->AddText( "CAEN UV LED Driver SP5605" );
  label_led->AddText( "#lambda = 248 nm (E = 5 eV)");
  label_led->Draw("same");

  float transparency = h1_vcharge_LiF->GetMean()/h1_vcharge->GetMean();

  TPaveText* label_LiF = new TPaveText( 0.6, 0.3, 0.78, 0.4, "brNDC" );
  label_LiF->SetFillColor(0);
  label_LiF->SetTextSize(0.04);
  label_LiF->AddText( Form("T = %.1f%%", 100.*transparency) );
  label_LiF->Draw("same");

  std::cout << std::endl;
  std::cout << Form("-> Transparency of %s: %.1f%%", legendName.c_str(), 100.*transparency) << std::endl;

  gPad->RedrawAxis();

  c1->SaveAs( Form("PMT_%s.pdf", name.c_str()) );

  delete c1;
  delete h2_axes;
  delete h1_vcharge;
  delete h1_vcharge_LiF;

} 
