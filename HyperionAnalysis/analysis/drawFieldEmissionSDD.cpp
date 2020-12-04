#include "NanoUVCommon.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"

// 1 Volt = 6.39369 keV (for G = 100) 
#define CALIBRATION 6.39369


std::string outdir = "plots/FieldEmissCNT";



void comparePressureGauge( const std::string& file_pON, const std::string& file_pOFF );
TH1D* fillFromTree( TTree* tree, const std::string& var, int nBins=200, float xMin=0., float xMax=4.5 );
void compareTriggerThreshold( const std::string& fileName1, const std::string& fileName2, const std::string& fileName3 );
void drawFieldEmissionVsHV();


int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );

  comparePressureGauge( "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L30_20k_Measurements_Only_12_2_2020.root",
                        "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L30_pressureOFF_20k_Measurements_Only_12_2_2020.root" );

  compareTriggerThreshold( "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig750_pressureOFF_20k_Measurements_Only_12_3_2020.root",
                           "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig800_pressureOFF_20k_Measurements_Only_12_3_2020.root",
                           "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trig850_pressureOFF_20k_Measurements_Only_12_3_2020.root" );

  drawFieldEmissionVsHV();

  return 0;

}



void comparePressureGauge( const std::string& fileName_pON, const std::string& fileName_pOFF ) {

  TFile* file_pON  = TFile::Open( fileName_pON.c_str()  );
  TFile* file_pOFF = TFile::Open( fileName_pOFF.c_str() );

  TTree* tree_pON  = (TTree*)file_pON ->Get("tree");
  TTree* tree_pOFF = (TTree*)file_pOFF->Get("tree");

  TH1D* h1_pON  = fillFromTree( tree_pON , "pON"  );
  TH1D* h1_pOFF = fillFromTree( tree_pOFF, "pOFF" );

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
  


TH1D* fillFromTree( TTree* tree, const std::string& name, int nBins, float xMin, float xMax ) {

  TH1D* h1 = new TH1D( name.c_str(), "", nBins, xMin, xMax );

  float var;
  tree->SetBranchAddress( "vamp", &var );

  int nEntries = tree->GetEntries();

  for( unsigned iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );

    h1->Fill( var*CALIBRATION );

  } // for entries

  return h1;

}



void compareTriggerThreshold( const std::string& fileName1, const std::string& fileName2, const std::string& fileName3 ) {

  TFile* file1 = TFile::Open( fileName1.c_str() );
  TFile* file2 = TFile::Open( fileName2.c_str() );
  TFile* file3 = TFile::Open( fileName3.c_str() );

  TTree* tree1 = (TTree*)file1->Get("tree");
  TTree* tree2 = (TTree*)file2->Get("tree");
  TTree* tree3 = (TTree*)file3->Get("tree");

  TH1D* h1_1 = fillFromTree( tree1, "h1_1" );
  TH1D* h1_2 = fillFromTree( tree2, "h1_2" );
  TH1D* h1_3 = fillFromTree( tree3, "h1_3" );

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



void drawFieldEmissionVsHV() {

  std::map< int, std::string > files;
  files[1800] = "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[1700] = "data/HyperionSDD/Run_SDD_G100_HVcnt1700_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[1750] = "data/HyperionSDD/Run_SDD_G100_HVcnt1750_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[1850] = "data/HyperionSDD/Run_SDD_G100_HVcnt1850_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[1900] = "data/HyperionSDD/Run_SDD_G100_HVcnt1900_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[1950] = "data/HyperionSDD/Run_SDD_G100_HVcnt1950_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  files[2000] = "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L32_trigINT_pressureOFF_20k_Measurements_Only_12_3_2020.root";
  

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 5.5, 10, 0., 700. );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  std::vector<int> colors;
  colors.push_back( kMagenta+1 );
  colors.push_back( 38 );
  colors.push_back( 28 );
  colors.push_back( 42 );
  colors.push_back( kGray+3 );
  colors.push_back( 46 );
  colors.push_back( 30 );

  TLegend* legend = new TLegend( 0.7, 0.5, 0.9, 0.9, "-#DeltaV(CNT-SDD)" );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );

  int i=0;

  for( unsigned iHV=1700; iHV<=2000; iHV+=50 ) {

    std::string fileName = files[iHV];
    TFile* file = TFile::Open(fileName.c_str());
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1 = fillFromTree( tree, Form("h1_%d", iHV), 100, 0., 5.5 );

    h1->SetLineWidth(3);
    h1->SetLineColor(colors[i++]);
    h1->Draw("same");

    legend->AddEntry( h1, Form("%d V", iHV), "L" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/vsHV.pdf", outdir.c_str()) );

  delete c1;
  delete h2_axes;

}
