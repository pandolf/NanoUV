#include "NanoUVCommon.h"

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH2D.h"
#include "TH1D.h"




int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "USAGE: ./drawFe55 [fileName]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);

  NanoUVCommon::setStyle();

  std::string outdir = "plots/Fe55";

  system(Form("mkdir -p %s", outdir.c_str()));


  TFile* file = TFile::Open( fileName.c_str() );
  TTree* tree = (TTree*)file->Get("tree");

  float vamp;
  tree->SetBranchAddress( "vamp", &vamp );


  TH1D* h1_vamp = new TH1D( "vamp", "", 1000, 0., 2. );
  TH1D* h1_energy = new TH1D( "energy", "", 1250, 5., 7.5);

  int nentries = tree->GetEntries();

  
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    h1_vamp->Fill( vamp );

  }

  float x_peak = h1_vamp->GetBinCenter( h1_vamp->GetMaximumBin() );

  TF1* f1_gaus = new TF1( "ka_gaus", "gaus", 0.968*x_peak, 1.032*x_peak );
  f1_gaus->SetParameter( 1, x_peak );
  f1_gaus->SetLineColor(46);
  f1_gaus->SetLineWidth(2);

  h1_vamp->Fit( f1_gaus, "R" );

  TCanvas* c1 = new TCanvas( "c1", "", 600., 600. );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.5*x_peak, 1.3*x_peak, 10, 0., h1_vamp->GetMaximum()*1.2 );
  h2_axes->SetXTitle("Amplitude [V]");
  h2_axes->SetYTitle("Entries");
  h2_axes->Draw();

  h1_vamp->Draw("same");

  c1->SaveAs(Form("%s/amp.pdf", outdir.c_str()) );


  float ka_peak  = f1_gaus->GetParameter(1);
  float ka_sigma = f1_gaus->GetParameter(2);

  float calibration = 5.9/ka_peak; // Volts to keV

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    h1_energy->Fill( vamp*calibration );

  }

  c1->Clear();
  c1->cd();

  h1_energy->SetXTitle("Energy [keV]");
  h1_energy->SetYTitle( Form("Entries / %.1f eV", h1_energy->GetBinWidth(1)*1000.) );
  h1_energy->Draw();

  TPaveText* label_sdd = new TPaveText( 0.6, 0.75, 0.9, 0.9, "brNDC" );
  label_sdd->SetTextSize(0.035);
  label_sdd->SetFillColor(0);
  label_sdd->AddText( "FBK SDD + MIXED Module (PoliMi)" );
  //label_sdd->Draw("same");

  TPaveText* label_kalpha = new TPaveText( 0.6, 0.65, 0.9, 0.75, "brNDC" );
  label_kalpha->SetTextSize(0.035);
  label_kalpha->SetTextColor(46);
  label_kalpha->SetFillColor(0);
  label_kalpha->AddText( "^{55}Fe Source" );
  label_kalpha->AddText( Form("K#alpha FWHM = %.1f eV", 2.355*ka_sigma*calibration*1000.) );
  label_kalpha->Draw("same");

  c1->SaveAs(Form("%s/energy.pdf", outdir.c_str()) );

  std::cout << " 1 Volt = " << calibration << " keV" << std::endl;
  std::cout << " 1 mV = " << calibration << " eV" << std::endl;

  return 0;

}
