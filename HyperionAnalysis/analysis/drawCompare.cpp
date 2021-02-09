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

std::string varName = "vcharge";


int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );


  std::map< std::string, std::string > files;
  files["WC 8ch" ] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1800_L32_h17_trig0p6_pressureOFF_WaveCatcher8ch_2_Measurements_Only_2_9_2021.root";
  files["WC 64ch"] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1800_L32_h17_trig0p6_pressureOFF_WaveCatcher24ch_Measurements_Only_2_9_2021.root";
  
  std::vector<int> colors;
  colors.push_back(46);
  colors.push_back(38);
  colors.push_back(29);
  colors.push_back(kGray+3);

  TLegend* legend = new TLegend( 0.6, 0.65, 0.9, 0.9, "#DeltaV = -1800 V" );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);

  float xMin_plot = 0.;
  float xMax_plot = 3.;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin_plot, xMax_plot, 10, 0., 0.08 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();

  TCanvas* c1_log = new TCanvas( "c1_log", "", 600, 600 );
  c1_log->cd();
  c1_log->SetLogy();

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, xMin_plot, xMax_plot, 10, 0.00003, 0.099 );
  h2_axes_log->SetXTitle( "Energy [keV]" );
  h2_axes_log->SetYTitle( "Normalized to Unity" );
  h2_axes_log->Draw();

  int i=0;

  for( std::map< std::string, std::string >::const_iterator ifile=files.begin(); ifile!=files.end(); ++ifile ) {

    std::cout << "-> Opening file: " << ifile->second.c_str() << std::endl;
    TFile* file = TFile::Open( ifile->second.c_str() );
    TTree* tree = (TTree*)file->Get("tree");

    TH1D* h1 = SDD::fillFromTree( tree, Form("h%s", ifile->first.c_str()), varName.c_str(), 300 );

    h1->SetLineColor(colors[i]);
    h1->SetLineWidth(3);

    c1->cd();
    h1->DrawNormalized("same");

    c1_log->cd();
    h1->DrawNormalized("same");

    legend->AddEntry( h1, Form("%s", ifile->first.c_str()), "L" ); 

    i++;

  } // for L


  c1->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/compareWC_%s.pdf", outdir.c_str(), varName.c_str()) );
  c1->SaveAs( Form("%s/compareWC_%s.eps", outdir.c_str(), varName.c_str()) );

  c1_log->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_log->SaveAs( Form("%s/compareWC_%s_log.pdf", outdir.c_str(), varName.c_str()) );
  c1_log->SaveAs( Form("%s/compareWC_%s_log.eps", outdir.c_str(), varName.c_str()) );


  return 0;

}



