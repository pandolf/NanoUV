#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "NanoUVCommon.h"





int main() {

  NanoUVCommon::setStyle();

  TFile* file_90eV  = TFile::Open("systCurrent_E90eV_APDhv350V.root" );
  TFile* file_500eV = TFile::Open("systCurrent_E500eV_APDhv350V.root");
  TFile* file_900eV = TFile::Open("systCurrent_E900eV_APDhv350V.root");

  TGraphErrors* gr_syst90eV  = (TGraphErrors*)file_90eV ->Get("syst");
  TGraphErrors* gr_syst500eV = (TGraphErrors*)file_500eV->Get("syst");
  TGraphErrors* gr_syst900eV = (TGraphErrors*)file_900eV->Get("syst");

  gr_syst90eV ->SetMarkerStyle(20);
  gr_syst90eV ->SetMarkerSize (1.5);
  gr_syst90eV ->SetMarkerColor(38);
  gr_syst90eV ->SetLineColor  (38);

  gr_syst500eV ->SetMarkerStyle(21);
  gr_syst500eV ->SetMarkerSize (1.5);
  gr_syst500eV ->SetMarkerColor(46);
  gr_syst500eV ->SetLineColor  (46);

  gr_syst900eV ->SetMarkerStyle(22);
  gr_syst900eV ->SetMarkerSize (1.5);
  gr_syst900eV ->SetMarkerColor(29);
  gr_syst900eV ->SetLineColor  (29);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  c1->SetLogx();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.05, 190., 10, 0., 0.3 );
  h2_axes->SetXTitle( "I_{gun} [pA]" );
  h2_axes->SetYTitle( "Relative Uncertainty on I_{apd}" );
  h2_axes->Draw("same");

  gr_syst90eV ->Draw("PL same");
  gr_syst500eV->Draw("PL same");
  gr_syst900eV->Draw("PL same");

  TLegend* legend = new TLegend( 0.6, 0.6, 0.9, 0.8 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( gr_syst90eV , "E_{e} = 90 eV" , "PL" );
  legend->AddEntry( gr_syst500eV, "E_{e} = 500 eV", "PL" );
  legend->AddEntry( gr_syst900eV, "E_{e} = 900 eV", "PL" );
  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("allSystCurrent.pdf");

  return 0;

}


