#include "NanoUVCommon.h"
#include "SDD.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"



std::string outdir = "plots/FieldEmissCNT";



void comparePressureGauge( const std::string& file_pON, const std::string& file_pOFF );
void compareTriggerThreshold( const std::string& fileName1, const std::string& fileName2, const std::string& fileName3 );
void drawFieldEmissionVsHV( const std::string& varName );


int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );

  comparePressureGauge( "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L30_20k_Measurements_Only_12_2_2020.root",
                        "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L30_pressureOFF_20k_Measurements_Only_12_2_2020.root" );

  compareTriggerThreshold( "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig750_pressureOFF_20k_Measurements_Only_12_3_2020.root",
                           "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig800_pressureOFF_20k_Measurements_Only_12_3_2020.root",
                           "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig850_pressureOFF_20k_Measurements_Only_12_3_2020.root" );

  drawFieldEmissionVsHV( "vamp" );
  drawFieldEmissionVsHV( "vcharge" );

  return 0;

}



void comparePressureGauge( const std::string& fileName_pON, const std::string& fileName_pOFF ) {

  TFile* file_pON  = TFile::Open( fileName_pON.c_str()  );
  TFile* file_pOFF = TFile::Open( fileName_pOFF.c_str() );

  TTree* tree_pON  = (TTree*)file_pON ->Get("tree");
  TTree* tree_pOFF = (TTree*)file_pOFF->Get("tree");

  TH1D* h1_pON  = SDD::fillFromTree( tree_pON , "pON" , "vamp", 100 );
  TH1D* h1_pOFF = SDD::fillFromTree( tree_pOFF, "pOFF", "vamp", 100 );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, h1_pON->GetXaxis()->GetXmin(), h1_pON->GetXaxis()->GetXmax(), 10, 0., h1_pOFF->GetMaximum()/h1_pOFF->Integral()*1.2 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();

  h1_pON->SetLineColor(46);
  h1_pON->SetLineWidth(3);
  h1_pON->DrawNormalized("same");

  h1_pOFF->SetLineColor(38);
  h1_pOFF->SetLineWidth(3);
  h1_pOFF->DrawNormalized("same");

  TLegend* legend = new TLegend( 0.55, 0.65, 0.9, 0.9, "B/A vacuum gauge:" );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( h1_pON , "ON" , "L" );
  legend->AddEntry( h1_pOFF, "OFF", "L" );
  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/pressure.pdf", outdir.c_str()) );

  delete c1;
  delete h2_axes;

}
  




void compareTriggerThreshold( const std::string& fileName1, const std::string& fileName2, const std::string& fileName3 ) {

  TFile* file1 = TFile::Open( fileName1.c_str() );
  TFile* file2 = TFile::Open( fileName2.c_str() );
  TFile* file3 = TFile::Open( fileName3.c_str() );

  TTree* tree1 = (TTree*)file1->Get("tree");
  TTree* tree2 = (TTree*)file2->Get("tree");
  TTree* tree3 = (TTree*)file3->Get("tree");

  TH1D* h1_1 = SDD::fillFromTree( tree1, "h1_1", "vamp" );
  TH1D* h1_2 = SDD::fillFromTree( tree2, "h1_2", "vamp" );
  TH1D* h1_3 = SDD::fillFromTree( tree3, "h1_3", "vamp" );

  h1_1->SetLineColor(46);
  h1_1->SetLineWidth(3);

  h1_2->SetLineColor(29);
  h1_2->SetLineWidth(3);

  h1_3->SetLineColor(38);
  h1_3->SetLineWidth(3);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  h1_3->Draw();
  h1_1->Draw("same");
  h1_2->Draw("same");
  
  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/trigger.pdf", outdir.c_str()) );

  delete c1;

}



void drawFieldEmissionVsHV( const std::string& varName ) {

  std::map< int, std::string > files;
  files[1800] = "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1650] = "data/HyperionSDD/Run_SDD_G100_HVcnt1650_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1700] = "data/HyperionSDD/Run_SDD_G100_HVcnt1700_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1750] = "data/HyperionSDD/Run_SDD_G100_HVcnt1750_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1850] = "data/HyperionSDD/Run_SDD_G100_HVcnt1850_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1900] = "data/HyperionSDD/Run_SDD_G100_HVcnt1900_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[1950] = "data/HyperionSDD/Run_SDD_G100_HVcnt1950_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  files[2000] = "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 5.5, 10, 0., 700. );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();


  TCanvas* c1_trunc = new TCanvas( "c1_trunc", "", 600, 600 );
  c1_trunc->cd();

  float xMin_trunc = 0.6;
  float xMax_trunc = 3.;
  TH2D* h2_axes_trunc = new TH2D( "axes_trunc", "", 10, xMin_trunc, xMax_trunc, 10, 0., 0.125 );
  h2_axes_trunc->SetXTitle( "Energy [keV]" );
  h2_axes_trunc->SetYTitle( "Normalized to Unity" );
  h2_axes_trunc->Draw();



  TCanvas* c1_rescale = new TCanvas( "c1_rescale", "", 600, 600 );
  c1_rescale->cd();

  float xMin_rescale = 0.6;
  float xMax_rescale = 3.;
  TH2D* h2_axes_rescale = new TH2D( "axes_rescale", "", 10, xMin_rescale, xMax_rescale, 10, 0., 0.125 );
  h2_axes_rescale->SetXTitle( "Energy (rescaled to 1650V)" );
  h2_axes_rescale->SetYTitle( "Normalized to Unity" );
  h2_axes_rescale->Draw();


  TCanvas* c1_response = new TCanvas( "c1_response", "", 600, 600 );
  c1_response->cd();

  TH2D* h2_axes_response = new TH2D( "axes_response", "", 10, 0.2, 2.2, 10, 0., 0.125 );
  h2_axes_response->SetXTitle( "Energy / #DeltaV" );
  h2_axes_response->SetYTitle( "Normalized to Unity" );
  h2_axes_response->Draw();





  std::vector<int> colors;
  colors.push_back( 15 );
  colors.push_back( 46 );
  colors.push_back( 38 );
  colors.push_back( 28 );
  colors.push_back( 42 );
  colors.push_back( kGray+3 );
  colors.push_back( kPink+1 );
  colors.push_back( 30 );

  TLegend* legend = new TLegend( 0.7, 0.5, 0.9, 0.9, "-#DeltaV(CNT-SDD)" );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );

  int i=0;

  for( unsigned iHV=1650; iHV<=2000; iHV+=50 ) {

    std::string fileName = files[iHV];
    TFile* file = TFile::Open(fileName.c_str());
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1          = SDD::fillFromTree( tree, Form("h1_%d"         , iHV), varName, 100, 100, 0., 5.5 );
    TH1D* h1_trunc    = SDD::fillFromTree( tree, Form("h1_trunc_%d"   , iHV), varName, 100, 50, xMin_trunc, xMax_trunc );
    TH1D* h1_rescale  = SDD::fillFromTree( tree, Form("h1_rescale_%d" , iHV), varName, 100, 50, xMin_trunc, xMax_trunc, 1650./((float)iHV) );
    TH1D* h1_response = SDD::fillFromTree( tree, Form("h1_response_%d", iHV), varName, 100, 50, 0.2, 2.2, 1000./((float)iHV) );

    c1->cd();
    h1->SetLineWidth(3);
    h1->SetLineColor(colors[i]);
    h1->Draw("same");

    c1_trunc->cd();
    h1_trunc->SetLineWidth(4);
    h1_trunc->SetLineColor(colors[i]);
    h1_trunc->DrawNormalized("same");

    c1_rescale->cd();
    h1_rescale->SetLineWidth(4);
    h1_rescale->SetLineColor(colors[i]);
    h1_rescale->DrawNormalized("same");

    c1_response->cd();
    h1_response->SetLineWidth(4);
    h1_response->SetLineColor(colors[i]);
    h1_response->DrawNormalized("same");

    legend->AddEntry( h1, Form("%d V", iHV), "L" );

    i++;

  }

  c1->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/%s_vsHV.pdf", outdir.c_str(), varName.c_str()) );

  c1_trunc->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_trunc->SaveAs( Form("%s/%s_vsHV_trunc.pdf", outdir.c_str(), varName.c_str()) );

  c1_rescale->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_rescale->SaveAs( Form("%s/%s_vsHV_rescale.pdf", outdir.c_str(), varName.c_str()) );

  c1_response->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_response->SaveAs( Form("%s/%s_vsHV_response.pdf", outdir.c_str(), varName.c_str()) );

  delete c1;
  delete h2_axes;
  delete c1_trunc;
  delete h2_axes_trunc;
  delete c1_rescale;
  delete h2_axes_rescale;
  delete c1_response;
  delete h2_axes_response;

}
