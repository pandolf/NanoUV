#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TH2D.h"
#include "TLine.h"
#include "TLegend.h"

#include "NanoUVCommon.h"


bool use_ampMax = true;



std::map<int, float> getVoltGainMap( const std::string& fileName );


int main( int argc, char* argv[] ) {


  std::string prodName = "May3";
  if( argc > 1 ) {
    prodName = (std::string)(argv[1]);
  }


  NanoUVCommon::setStyle();


  std::string datadir = "data/LED_driver/" + prodName;
  std::string prefix = "C1211215up2000";
  std::string plotsdir = "plots";
  system( Form("mkdir -p %s", plotsdir.c_str()) );

  std::vector<int> volts;
  volts.push_back(300);
  volts.push_back(350);
  volts.push_back(380);
  volts.push_back(390);
  volts.push_back(395);
  volts.push_back(400);
  volts.push_back(405);
  volts.push_back(410);
  volts.push_back(415);
  volts.push_back(420);
  volts.push_back(425);
  volts.push_back(430);
  volts.push_back(435);
  volts.push_back(440);
  volts.push_back(450);


  std::map<int, float> voltGainMap = getVoltGainMap( "gain_APD3.txt" );

  int nFiles = 50;

  TFile* file_signal = TFile::Open( Form("signalHistos_%s.root", prodName.c_str()), "recreate" );

  TGraphErrors* gr_signal_vs_gain = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_gain_sigmaUp = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_gain_sigmaDn = new TGraphErrors(0);

  for( unsigned i=0; i<volts.size(); ++i ) {

    TH1D* h1_signal = new TH1D( Form("signal_%d", volts[i]), "", 1000, -10000., 1000. );

    for( unsigned iFile=0; iFile<nFiles; ++iFile ) {

      std::string additionalZero = (iFile<10) ? "0" : "";
      std::string thisFileName( Form( "%s/%d/%s%s%d.txt", datadir.c_str(), volts[i], prefix.c_str(), additionalZero.c_str(), iFile ) );
      TGraph* thisGraph = NanoUVCommon::getGraphFromFile( thisFileName.c_str() );
      NanoUVCommon::plotWaveformGraph( thisGraph, Form("%s.pdf", thisFileName.c_str()) ); 
      float thisSignal = (use_ampMax) ? NanoUVCommon::ampMaxSignal( thisGraph ) : NanoUVCommon::integrateSignal( thisGraph )/1000.;
      h1_signal->Fill( thisSignal );
      delete thisGraph;

    } // for files

    file_signal->cd();
    h1_signal->Write();

    float xValue = voltGainMap[volts[i]];

    int iPoint = gr_signal_vs_gain->GetN();
    gr_signal_vs_gain        ->SetPoint( iPoint, xValue, h1_signal->GetMean() );
    gr_signal_vs_gain_sigmaUp->SetPoint( iPoint, xValue, h1_signal->GetMean()+h1_signal->GetRMS() );
    gr_signal_vs_gain_sigmaDn->SetPoint( iPoint, xValue, h1_signal->GetMean()-h1_signal->GetRMS() );
    gr_signal_vs_gain->SetPointError( iPoint, 0., h1_signal->GetMeanError() );

  } // for volts


  file_signal->cd();
  gr_signal_vs_gain->Write();
  file_signal->Close();

  TCanvas* c1 = new TCanvas( "c1c1", "", 600, 600 );
  c1->cd();

  float xMin = 0.;
  float xMax = 153.;
  float yMin = (use_ampMax) ? -120. : -4.;
  float yMax = (use_ampMax) ? 0. : 0.5;

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax );
  if( use_ampMax ) 
    h2_axes->SetYTitle("AmpMax Signal [a.u.]"); 
  else
    h2_axes->SetYTitle("Integrated Signal [a.u.]"); 
  h2_axes->SetXTitle("APD Gain");
  h2_axes->Draw();


  TLegend* legend = new TLegend( 0.6, 0.3, 0.88, 0.4 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->AddEntry( gr_signal_vs_gain_sigmaDn, "68% band", "L" );
  legend->Draw("same");


  gr_signal_vs_gain_sigmaUp->SetLineColor(38);
  gr_signal_vs_gain_sigmaUp->SetLineWidth(2);
  gr_signal_vs_gain_sigmaUp->SetLineStyle(2);
  gr_signal_vs_gain_sigmaUp->Draw("L same");

  gr_signal_vs_gain_sigmaDn->SetLineColor(38);
  gr_signal_vs_gain_sigmaDn->SetLineWidth(2);
  gr_signal_vs_gain_sigmaDn->SetLineStyle(2);
  gr_signal_vs_gain_sigmaDn->Draw("L same");

  gr_signal_vs_gain->SetMarkerStyle(20);
  gr_signal_vs_gain->SetMarkerSize(1.6);
  gr_signal_vs_gain->SetMarkerColor(46);
  gr_signal_vs_gain->Draw("PL same");


  TPaveText* label_led = new TPaveText( 0.23, 0.55, 0.6, 0.62, "brNDC" );
  label_led->SetFillColor(0);
  label_led->SetTextSize(0.035);
  label_led->SetTextAlign(11);
  label_led->SetTextFont(62);
  label_led->AddText("LED pulse (1 ns)");
  label_led->Draw("same");

  TPaveText* label_gamma = new TPaveText( 0.23, 0.45, 0.6, 0.55, "brNDC" );
  label_gamma->SetFillColor(0);
  label_gamma->SetTextSize(0.035);
  label_gamma->SetTextAlign(11);
  label_gamma->SetTextFont(42);
  label_gamma->AddText("380 < #lambda < 420 nm");
  label_gamma->AddText("2.9 < E_{#gamma} < 3.3 eV");
  label_gamma->Draw("same");

  NanoUVCommon::addNanoUVLabel(c1, 3);

  c1->SaveAs(Form("sigVsGain_%s.pdf", prodName.c_str()));

  return 0;

}


std::map<int, float> getVoltGainMap( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  std::map<int, float> voltGainMap;

  while( ifs.good() ) {

    int volt;
    float gain;
    ifs >> volt >> gain;

    voltGainMap[volt] = gain;

  }

  return voltGainMap;

}

