#include <iostream>

#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "interface/NanoUVCommon.h"



void drawTransparency(  const std::string& fileName1, const std::string& fileName2, const std::string& name, const std::string& legendName );
float getGainFromFileName( const std::string& fileName );




int main() {


  NanoUVCommon::setStyle();

  drawTransparency( "Run_HV700_A10_Measurements_Only_9_1_2020"   , "Run_HV700_A10_LiF_Measurements_Only_9_1_2020"      , "LiF"  , "LiF window (2 mm)"       ) ;
  drawTransparency( "Run_HV700_A10_Measurements_Only_9_23_2020"   , "Run_HV700_A10_Fused500um_Measurements_Only_9_23_2020"    , "Fused", "fused silica (500 #mum)" ) ;
//  drawTransparency( "Run_HV700_A10_Measurements_Only_9_2_2020"   , "Run_HV700_A10_Fused_Measurements_Only_9_2_2020"    , "Fused", "fused silica (500 #mum)" ) ;
  drawTransparency( "Run_HV700_A10_Measurements_Only_9_21_2020"  , "Run_HV700_A10_Fused2mm_Measurements_Only_9_21_2020", "Fused2mm", "fused silica (2 mm)"  ) ;
  drawTransparency( "Run_HV700_A10_Measurements_Only_9_21_2020_2", "Run_HV700_A10_Fused4mm_Measurements_Only_9_21_2020", "Fused4mm", "fused silica (4 mm)"  ) ;
  drawTransparency( "Run_HV700_A10_G10_Measurements_Only_10_5_2020", "Run_HV700_A10_G300_Fused2mm_Fused500um_CNT50um_Measurements_Only_10_5_2020", "Fused2p5mm_CNT", "fused silica (2.5 mm) + CNT (50 #mum)"  ) ;
  drawTransparency( "Run_HV700_A7_G30_Measurements_Only_10_6_2020_2", "Run_HV700_A7_G10_Measurements_Only_10_6_2020_2", "test_gain", "test gain"  ) ;

  return 0;

}




void drawTransparency(  const std::string& fileName1, const std::string& fileName2, const std::string& name, const std::string& legendName ) {

  std::cout << std::endl;

  float gain1 = getGainFromFileName(fileName1);
  float gain2 = getGainFromFileName(fileName2);
  
  TFile* file     = TFile::Open( Form("data/PMT_LEDUV/%s.root", fileName1.c_str()));
  TFile* file_LiF = TFile::Open( Form("data/PMT_LEDUV/%s.root", fileName2.c_str()));

  TTree* tree     = (TTree*)file    ->Get("tree");
  TTree* tree_LiF = (TTree*)file_LiF->Get("tree");

  //std::string varName = "vcharge";
  std::string varName = "vamp";

  float var;
  tree->SetBranchAddress( varName.c_str(), &var );
  tree->GetEntry(0);

  TH1D* h1_vcharge_tmp     = new TH1D( Form("h_%s_tmp"    , name.c_str()), "", 200, 0., 20000.);
  TH1D* h1_vcharge_LiF_tmp = new TH1D( Form("h_%s_LiF_tmp", name.c_str()), "", 200, 0., 20000.);

  // first project just to understand variable ranges
  tree    ->Project( h1_vcharge_tmp    ->GetName(), Form("-%s", varName.c_str())); // minus sign because variable is negative!
  tree_LiF->Project( h1_vcharge_LiF_tmp->GetName(), Form("-%f*%s", gain1/gain2, varName.c_str()));

std::cout << h1_vcharge_tmp->GetMean() << std::endl;
std::cout << h1_vcharge_LiF_tmp->GetMean() << std::endl;


  float xMin = h1_vcharge_LiF_tmp->GetMean()*0.5;
  float xMax = h1_vcharge_tmp->GetMean()*1.1;

  TH1D* h1_vcharge     = new TH1D( Form("h_%s"    , name.c_str()), "", 200, xMin, xMax);
  TH1D* h1_vcharge_LiF = new TH1D( Form("h_%s_LiF", name.c_str()), "", 200, xMin, xMax);

  tree    ->Project( h1_vcharge    ->GetName(), Form("-%s", varName.c_str())); // minus sign because variable is negative!
  tree_LiF->Project( h1_vcharge_LiF->GetName(), Form("-%f*%s", gain1/gain2, varName.c_str()));


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, xMin, xMax, 10, 0., 1.3*h1_vcharge_LiF->GetMaximum() );
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
  std::cout << Form("-> Transparency of %s: %.1f%%  (1-T=%.1f%%)", legendName.c_str(), 100.*transparency, 100.*(1.-transparency)) << std::endl;

  gPad->RedrawAxis();

  c1->SaveAs( Form("PMT_%s.pdf", name.c_str()) );

  delete c1;
  delete h2_axes;
  delete h1_vcharge;
  delete h1_vcharge_LiF;

} 



float getGainFromFileName( const std::string& fileName ) {

  float gain = 1.;

  TString fileName_tstr(fileName);

  if     ( fileName_tstr.Contains("_G10_"  ) ) gain = 10.;
  else if( fileName_tstr.Contains("_G30_"  ) ) gain = 30.;
  else if( fileName_tstr.Contains("_G100_" ) ) gain = 100.;
  else if( fileName_tstr.Contains("_G300_" ) ) gain = 300.;
  else if( fileName_tstr.Contains("_G1000_") ) gain = 1000.;
  else if( fileName_tstr.Contains("_G3000_") ) gain = 3000.;

  if( gain!= 1. ) 
    std::cout << "-> Setting gain G = " << gain << " for file: " << fileName << std::endl;

  return gain;

}
