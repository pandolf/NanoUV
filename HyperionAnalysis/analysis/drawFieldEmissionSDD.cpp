#include "NanoUVCommon.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"




TH1D* fillFromTree( TTree* tree, const std::string& var, float k, int nBins, float xMin, float xMax );


int main() {

  NanoUVCommon::setStyle();

  TFile* file_pON  = TFile::Open( "data/HyperionSDD/Run_SDD_G100_HVcnt2000_20k_Measurements_Only_12_2_2020.root"  );
  TFile* file_pOFF = TFile::Open( "data/HyperionSDD/Run_SDD_G100_HVcnt2000_pressureOFF_20k_Measurements_Only_12_2_2020.root" );

  TTree* tree_pON  = (TTree*)file_pON ->Get("tree");
  TTree* tree_pOFF = (TTree*)file_pOFF->Get("tree");

  // 1 Volt = 6.39369 keV (for G = 100) 
  float volt_to_kev = 6.39369;

  int nBins = 200;
  float xMin = 0.;
  float xMax = 4.5;

  TH1D* h1_pON  = fillFromTree( tree_pON , "pON" , volt_to_kev, nBins, xMin, xMax );
  TH1D* h1_pOFF = fillFromTree( tree_pOFF, "pOFF", volt_to_kev, nBins, xMin, xMax );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., h1_pOFF->GetMaximum()/h1_pOFF->Integral()*1.2 );
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

  c1->SaveAs( "test.pdf" );

  return 0;

}
  


TH1D* fillFromTree( TTree* tree, const std::string& name, float k, int nBins, float xMin, float xMax ) {

  TH1D* h1 = new TH1D( name.c_str(), "", nBins, xMin, xMax );

  float var;
  tree->SetBranchAddress( "vamp", &var );

  int nEntries = tree->GetEntries();

  for( unsigned iEntry = 0; iEntry<nEntries; ++iEntry ) {

    tree->GetEntry( iEntry );

    h1->Fill( var*k );

  } // for entries

  return h1;

}
