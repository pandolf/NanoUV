#include <iostream>

#include "interface/NanoUVCommon.h"

#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"




int main() {

  NanoUVCommon::setStyle();

  TGraphErrors* gr_amp_vs_l = new TGraphErrors(0);

  int iPoint = 0;

  for( int l=38; l>26; l-- ) {

    if( l==34 ) continue;

    TFile* file = TFile::Open( Form("data/APD_LEDUV/Run_HV380_A10_Amp54dB_L%d_Measurements_Only_10_28_2020.root", l) );
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1_amp = new TH1D( Form("amp_l%d", l), "", 1000, 0., 20000.);

    tree->Project( h1_amp->GetName(), "vamp" );

    gr_amp_vs_l->SetPoint(iPoint, -l, h1_amp->GetMean());
    std::cout << "-l: " << -l << " h1_amp->GetMean(): " << h1_amp->GetMean() << std::endl;
    gr_amp_vs_l->SetPointError(iPoint, 0.05, h1_amp->GetRMS());

    iPoint+=1;

  }

  gr_amp_vs_l->SetMarkerColor(kGray+3);
  gr_amp_vs_l->SetMarkerStyle(20);
  gr_amp_vs_l->SetMarkerSize(1.5);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, -41., -25., 10, 0., 1. );
  h2_axes->SetXTitle( "-L [mm]" );
  h2_axes->SetYTitle( "APD Amplitude [V]" );
  h2_axes->Draw();

  gr_amp_vs_l->Draw("Psame");

  gPad->RedrawAxis();

  c1->SaveAs("apdAmp_vs_L.pdf");

  return 0;

}
