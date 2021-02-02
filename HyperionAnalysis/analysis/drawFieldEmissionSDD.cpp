#include "NanoUVCommon.h"
#include "SDD.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"



std::string outdir = "plots/FieldEmissCNT";

std::string varName = "vamp";



int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );


  std::map< int, std::string > files;
  files[1750] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1750_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
  files[1800] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1800_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
  files[1850] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1850_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
  files[1900] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1900_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
  
  //files[1800] = "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1650] = "data/HyperionSDD/Run_SDD_G100_HVcnt1650_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1700] = "data/HyperionSDD/Run_SDD_G100_HVcnt1700_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1750] = "data/HyperionSDD/Run_SDD_G100_HVcnt1750_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1850] = "data/HyperionSDD/Run_SDD_G100_HVcnt1850_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1900] = "data/HyperionSDD/Run_SDD_G100_HVcnt1900_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1950] = "data/HyperionSDD/Run_SDD_G100_HVcnt1950_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[2000] = "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float xMax_keV = 2.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax_keV, 10, 0., 0.2 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();


  TCanvas* c1_response = new TCanvas( "c1_response", "", 600, 600 );
  c1_response->cd();

  float xMax_response = 0.8;

  TH2D* h2_axes_response = new TH2D( "axes_response", "", 10, 0., xMax_response, 10, 0., 0.1 );
  h2_axes_response->SetXTitle( "Energy / #DeltaV" );
  h2_axes_response->SetYTitle( "Normalized to Unity" );
  h2_axes_response->Draw();


  std::vector<int> colors;
  colors.push_back( 46 );
  colors.push_back( 38 );
  colors.push_back( 29 );
  colors.push_back( kGray+3 );
  colors.push_back( 15 );
  colors.push_back( 42 );
  colors.push_back( kPink+1 );
  colors.push_back( 30 );

  TLegend* legend = new TLegend( 0.7, 0.6, 0.9, 0.9, "-#DeltaV(CNT-SDD)" );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );


  TGraphErrors* gr_peak_vs_HV = new TGraphErrors(0);

  int i=0;

  for( unsigned iHV=1750; iHV<=1900; iHV+=50 ) {

    std::string fileName = files[iHV];
    TFile* file = TFile::Open(fileName.c_str());
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1          = SDD::fillFromTree( tree, Form("h1_%d"         , iHV), varName, 300, 100, 0., xMax_keV );
    TH1D* h1_response = SDD::fillFromTree( tree, Form("h1_response_%d", iHV), varName, 300, 100, 0., xMax_response, 1000./((float)iHV) );

    c1->cd();
    h1->SetLineWidth(3);
    h1->SetLineColor(colors[i]);
    h1->DrawNormalized("same");

    TF1* f1 = new TF1( Form("f1_%d", iHV), "gaus", 0.4, 0.8 );
    f1->SetParameter(1, h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin()));
    h1->Fit(f1, "0R");
    gr_peak_vs_HV->SetPoint     ( i, iHV, f1->GetParameter(1) );
    gr_peak_vs_HV->SetPointError( i, 2. , f1->GetParError (1) );

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

  c1_response->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_response->SaveAs( Form("%s/%s_vsHV_response.pdf", outdir.c_str(), varName.c_str()) );


  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();

  float xMin_gr = 1701.;
  float xMax_gr = 1999.;

  TH2D* h2_axes_3 = new TH2D("axes3", "", 10, xMin_gr, xMax_gr, 10, 0.3, 0.8);
  h2_axes_3->SetXTitle( "-#DeltaV(CNT-SDD) [V]" );
  h2_axes_3->SetYTitle( "E_{peak} [keV]" );
  h2_axes_3->Draw();

  TF1* line = new TF1( "line", "[0] + [1]*x", xMin_gr, xMax_gr );
  line->SetLineColor(46);
  line->SetLineWidth(2);
  gr_peak_vs_HV->Fit( line, "XR" );

  TPaveText* label_fit = new TPaveText( 0.55, 0.2, 0.9, 0.5, "brNDC" );
  label_fit->SetTextSize(0.035);
  label_fit->SetTextColor(46);
  label_fit->SetFillColor(0);
  label_fit->AddText( "f(x) = m*x + q" );
  label_fit->AddText( Form("m = %.2f#pm%.2f [keV/kV]", line->GetParameter(1)*1000., line->GetParError(1)*1000. ));
  label_fit->AddText( Form("q = %.2f#pm%.2f keV", line->GetParameter(0), line->GetParError(0) ));
  label_fit->AddText( Form("#chi^2 / NDF = %.2f / %d", line->GetChisquare(), line->GetNumberFreeParameters()) );
  label_fit->Draw("same");
  

  gr_peak_vs_HV->SetMarkerStyle(20);
  gr_peak_vs_HV->SetMarkerSize(1.6);
  gr_peak_vs_HV->SetMarkerColor(kGray+3);
  gr_peak_vs_HV->Draw("psame");

  gPad->RedrawAxis();

  c2->SaveAs( Form("%s/Epeak_vsHV.pdf", outdir.c_str()) );


  delete c1;
  delete h2_axes;
  delete c1_response;
  delete h2_axes_response;
  delete c2;
  delete h2_axes_3;


}
