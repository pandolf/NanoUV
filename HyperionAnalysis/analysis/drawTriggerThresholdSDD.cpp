#include <iostream>

#include "NanoUVCommon.h"
#include "SDD.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"



std::string outdir = "plots/FieldEmissCNT";

void drawTrigThresh( const std::string& outdir, float hv, const std::vector<float> triggers );



int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );

  std::vector<float> triggers;
  triggers.push_back(0.5);
  triggers.push_back(0.6);
  triggers.push_back(0.7);
  triggers.push_back(0.8);

  drawTrigThresh( outdir, 1750, triggers );
  drawTrigThresh( outdir, 1800, triggers );
  drawTrigThresh( outdir, 1850, triggers );
  drawTrigThresh( outdir, 1900, triggers );

  return 0;

}



void drawTrigThresh( const std::string& outdir, float hv, const std::vector<float> triggers ) {


  std::vector<int> colors;
  colors.push_back(46);
  colors.push_back(38);
  colors.push_back(29);
  colors.push_back(kGray+3);

  TLegend* legend = new TLegend( 0.6, 0.6, 0.9, 0.9, "Trigger threshold:" );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  float xMin_plot = 0.;
  float xMax_plot = 1.5;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin_plot, xMax_plot, 10, 0., 0.2 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();

  TCanvas* c1_log = new TCanvas( "c1_log", "", 600, 600 );
  c1_log->cd();
  c1_log->SetLogy();

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, xMin_plot, xMax_plot, 10, 0.00001, 0.8 );
  h2_axes_log->SetXTitle( "Energy [keV]" );
  h2_axes_log->SetYTitle( "Normalized to Unity" );
  h2_axes_log->Draw();


  for( unsigned i=0; i<triggers.size(); ++i ) {

    std::string trig_str( Form("%.1f", triggers[i]) );
    TString trig_tstr(trig_str);
    trig_tstr.ReplaceAll( ".", "p" );
    trig_str = trig_tstr;

    TFile* file = TFile::Open( Form("data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt%.0f_L32_h17_trig%s_pressureOFF_Measurements_Only_2_2_2021.root", hv, trig_str.c_str()) );
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1 = SDD::fillFromTree( tree, Form("h%d", i), "vamp", 300 );

    h1->SetLineColor(colors[i]);
    h1->SetLineWidth(3);

    c1->cd();
    h1->DrawNormalized("same");

    c1_log->cd();
    h1->DrawNormalized("same");

    legend->AddEntry( h1, Form("-%.1f V", triggers[i]), "L" );


  } // for triggers


  TPaveText* label = new TPaveText( 0.7, 0.2, 0.9, 0.3, "brNDC" );
  label->SetTextSize(0.035);
  label->SetFillColor(0);
  label->AddText( Form("#DeltaV = -%.0f V", hv) );
  label->AddText( Form("d = 3 mm" ) );

  c1->cd();
  legend->Draw("same");
  label->Draw("same");
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/trigger_hv%.0f.pdf", outdir.c_str(), hv) );

  c1_log->cd();
  legend->Draw("same");
  label->Draw("same");
  gPad->RedrawAxis();
  c1_log->SaveAs( Form("%s/trigger_hv%.0f_log.pdf", outdir.c_str(), hv) );

  

  delete c1;

}



