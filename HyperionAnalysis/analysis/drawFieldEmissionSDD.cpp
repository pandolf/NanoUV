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




std::string varName = "vcharge";



int main( int argc, char* argv[] ) {


  std::string sample = "CNTPlasmaEtched";
  if( argc>1 )
    sample = std::string(argv[1]);


  std::cout << "-> Sample: " << sample << std::endl;


  NanoUVCommon::setStyle();


  std::map< int, std::string > files;

  if( sample=="CNT50um_fusedITO_B" ) {

    files[1750] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1750_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
    files[1800] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1800_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
    files[1850] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1850_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
    files[1900] = "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1900_L32_h17_trig0p6_pressureOFF_Measurements_Only_2_2_2021.root";
  
  } else if( sample=="CNTPlasmaEtched" ) {

    files[1000] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1000_L29_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[1100] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1100_L28p5_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[1200] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1200_L28p4_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[1400] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1400_L26p5_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[1600] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1600_L25_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[1800] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1800_L23_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";
    files[2000] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt2000_L20_h17_trig0p6_pressureOFF_Measurements_Only_12_2_2021.root";

  } else if( sample=="CNTPlasmaEtched2" ) {

    files[1600] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1600_L25_h17_trig0p6_pressureOFF_2_Measurements_Only_2_12_2021.root";
    files[1700] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1700_L25_h17_trig0p6_pressureOFF_Measurements_Only_2_12_2021.root";
    files[1800] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1800_L25_h17_trig0p6_pressureOFF_Measurements_Only_2_12_2021.root";
    files[1900] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt1900_L25_h17_trig0p6_pressureOFF_Measurements_Only_2_12_2021.root";
    files[2000] = "data/HyperionSDD/CNTPlasmaEtched/Run_SDD_G300_HVcnt2000_L25_h17_trig0p6_pressureOFF_Measurements_Only_2_12_2021.root";

  }
  // old stuff:
  //files[1800] = "data/HyperionSDD/Run_SDD_G100_HVcnt1800_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1650] = "data/HyperionSDD/Run_SDD_G100_HVcnt1650_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1700] = "data/HyperionSDD/Run_SDD_G100_HVcnt1700_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1750] = "data/HyperionSDD/Run_SDD_G100_HVcnt1750_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1850] = "data/HyperionSDD/Run_SDD_G100_HVcnt1850_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1900] = "data/HyperionSDD/Run_SDD_G100_HVcnt1900_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[1950] = "data/HyperionSDD/Run_SDD_G100_HVcnt1950_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  //files[2000] = "data/HyperionSDD/Run_SDD_G100_HVcnt2000_L32_trigINT_pressureOFF_Measurements_Only_12_4_2020.root";
  

  std::string outdir(Form("plots/FieldEmissCNT/%s", sample.c_str()));
  system( Form("mkdir -p %s/fits", outdir.c_str()) );


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float xMax_keV = 4.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax_keV, 10, 0., 0.11 );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw();


  TCanvas* c1_response = new TCanvas( "c1_response", "", 600, 600 );
  c1_response->cd();

  float xMax_response = 1.2;

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
  float xMin_gr = 1500.;
  float xMax_gr = 2050.;

  for( std::map< int, std::string >::const_iterator ifile=files.begin(); ifile!=files.end(); ++ifile ) {

    int iHV = ifile->first;
    std::string fileName = ifile->second;

    TFile* file = TFile::Open(fileName.c_str());
    TTree* tree = (TTree*)file->Get("tree");

    int nBinsHisto = 100;
    float xMinHisto = 0.0001;
    TH1D* h1          = SDD::fillFromTree( tree, Form("h1_%d"         , iHV), varName, 300, nBinsHisto, xMinHisto, xMax_keV );
    TH1D* h1_response = SDD::fillFromTree( tree, Form("h1_response_%d", iHV), varName, 300, nBinsHisto, xMinHisto, xMax_response, 1000./((float)iHV) );


    float hMode = h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin());
    float hRMS = h1->GetRMS();
    TF1* f1 = new TF1( Form("f1_%d", iHV), "gaus", hMode-0.5*hRMS, hMode+0.5*hRMS );
    f1->SetParameter(1, hMode );
    h1->Fit(f1, "R0");
    gr_peak_vs_HV->SetPoint     ( i, iHV, f1->GetParameter(1) );
    gr_peak_vs_HV->SetPointError( i, 2. , f1->GetParError (1) );

    c1->cd();
    h1->SetLineWidth(3);
    h1->SetLineColor(colors[i]);
    h1->DrawNormalized("same");

    c1_response->cd();
    h1_response->SetLineWidth(3);
    h1_response->SetLineColor(colors[i]);
    h1_response->DrawNormalized("same");

    TCanvas* c1_fit = new TCanvas( "c1_fit", "", 600, 600 );
    c1_fit->cd();
    h1->Draw();
    f1->Draw("same");
    c1_fit->SaveAs( Form("%s/fits/fit_%dV.pdf", outdir.c_str(), iHV) );
    delete c1_fit;
    

    legend->AddEntry( h1, Form("%d V", iHV), "L" );

    if( iHV < xMin_gr ) xMin_gr = iHV;
    if( iHV > xMax_gr ) xMax_gr = iHV;

    i++;

  }

  xMin_gr -= 50.;
  xMax_gr += 50.;

  c1->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/%s_vsHV.pdf", outdir.c_str(), varName.c_str()) );
  c1->SaveAs( Form("%s/%s_vsHV.eps", outdir.c_str(), varName.c_str()) );

  c1_response->cd();
  legend->Draw("same");
  gPad->RedrawAxis();
  c1_response->SaveAs( Form("%s/%s_vsHV_response.pdf", outdir.c_str(), varName.c_str()) );
  c1_response->SaveAs( Form("%s/%s_vsHV_response.eps", outdir.c_str(), varName.c_str()) );


  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();


  TH2D* h2_axes_3 = new TH2D("axes3", "", 10, xMin_gr, xMax_gr, 10, 0.0, 1.4);
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
  label_fit->AddText( Form("m = %.2f #pm %.2f [keV/kV]", line->GetParameter(1)*1000., line->GetParError(1)*1000. ));
  label_fit->AddText( Form("q = %.2f #pm %.2f keV", line->GetParameter(0), line->GetParError(0) ));
  label_fit->AddText( Form("#chi^{2} / NDF = %.2f / %d", line->GetChisquare(), line->GetNumberFreeParameters()) );
  label_fit->Draw("same");
  

  gr_peak_vs_HV->SetMarkerStyle(20);
  gr_peak_vs_HV->SetMarkerSize(1.6);
  gr_peak_vs_HV->SetMarkerColor(kGray+3);
  gr_peak_vs_HV->Draw("psame");

  gPad->RedrawAxis();

  c2->SaveAs( Form("%s/%speak_vsHV.pdf", outdir.c_str(), varName.c_str()) );
  c2->SaveAs( Form("%s/%speak_vsHV.eps", outdir.c_str(), varName.c_str()) );


  delete c1;
  delete h2_axes;
  delete c1_response;
  delete h2_axes_response;
  delete c2;
  delete h2_axes_3;


}
