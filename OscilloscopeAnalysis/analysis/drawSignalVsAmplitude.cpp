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
bool invertPolarity = false;


TGraphErrors* getSignalVsAmplitude( const std::string& prodName );
void divideGraph( TGraphErrors* graph, float factor );


int main( int argc, char* argv[] ) {

  std::vector<std::string> prodNames;

  std::string prodName = "May1_v2";
  if( argc > 1 ) {
    prodName = (std::string)(argv[1]);
  }
  prodNames.push_back(prodName);

  if( argc > 2 ) {
    prodNames.push_back((std::string)(argv[2]));
  }


  NanoUVCommon::setStyle();

  std::vector<TGraphErrors*> graphs;
  for( unsigned i=0; i<prodNames.size(); ++i ) graphs.push_back( getSignalVsAmplitude( prodNames[i] ) );

  if( graphs.size()==2 ) {

    TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
    c2->cd();

    double xmax0, ymax0;
    graphs[0]->GetPoint(0, xmax0, ymax0 );
    double xmax1, ymax1;
    graphs[1]->GetPoint(0, xmax1, ymax1 );
    float yMax = (ymax0>ymax1) ? ymax0 : ymax1;

    float xMin = 0.;
    float xMax = 10.9;

    TH2D* h2_axes = new TH2D( "axes2", "", 10, xMin, xMax, 10, 0., yMax*1.1 );
    h2_axes->SetYTitle("Signal / Noise" );
    h2_axes->SetXTitle("LED Amplitude");
    h2_axes->Draw();

    TLine* lineOne = new TLine( xMin, 1., xMax, 1. );
    lineOne->SetLineColor(46);
    lineOne->SetLineStyle(2);
    lineOne->SetLineWidth(2);
    lineOne->Draw("same");

    graphs[0]->SetMarkerStyle(24);
    graphs[0]->SetMarkerColor(kGray+3);
    graphs[0]->SetLineColor(kGray+3);

    graphs[1]->SetMarkerStyle(20);
    graphs[1]->SetMarkerColor(kGray+3);
    graphs[1]->SetLineColor(kGray+3);

    TLegend* legend = new TLegend( 0.5, 0.7, 0.85, 0.81 );
    legend->SetFillColor(0);
    legend->SetTextSize(0.035);
    legend->SetTextFont(42);
    legend->AddEntry( graphs[1], "S11625-30N", "P" );
    legend->AddEntry( graphs[0], "S8664-55", "P" );
    legend->Draw("same");

    graphs[0]->Draw("PLsame");
    graphs[1]->Draw("PLsame");


    TPaveText* label_led = new TPaveText( 0.23, 0.52, 0.6, 0.59, "brNDC" );
    label_led->SetFillColor(0);
    label_led->SetTextSize(0.035);
    label_led->SetTextAlign(11);
    label_led->SetTextFont(62);
    label_led->AddText("LED pulse (1 ns)");
    label_led->Draw("same");

    TPaveText* label_gamma = new TPaveText( 0.23, 0.42, 0.6, 0.52, "brNDC" );
    label_gamma->SetFillColor(0);
    label_gamma->SetTextSize(0.035);
    label_gamma->SetTextAlign(11);
    label_gamma->SetTextFont(42);
    label_gamma->AddText("380 < #lambda < 420 nm");
    label_gamma->AddText("2.9 < E_{#gamma} < 3.3 eV");
    label_gamma->Draw("same");

    NanoUVCommon::addNanoUVLabel(c2, 2);

    c2->SaveAs(Form("sigalVsAmplitude_%s_vs_%s.pdf", prodNames[0].c_str(), prodNames[1].c_str()));

    delete c2;
    delete h2_axes;

  }

  return 0;

}



TGraphErrors* getSignalVsAmplitude( const std::string& prodName ) {

  std::string datadir = "data/LED_driver/" + prodName;
  std::vector<std::string> prefixes;
  prefixes.push_back("C1211215up2000");
  prefixes.push_back("C2211215up2000");
  prefixes.push_back("C3211215up2000");
  prefixes.push_back("C4211215up2000");

  //if( prodName=="Jul17_new_v1" || prodName=="Jul17_new_v1" ) {
  //  prefix = "C2211215up2000";
  //  invertPolarity = false;
  //}
  //std::string plotsdir = "plots";
  //system( Form("mkdir -p %s", plotsdir.c_str()) );

  std::vector<int> amplitudes;
  amplitudes.push_back(10);
  amplitudes.push_back(9);
  amplitudes.push_back(8);
  amplitudes.push_back(7);
  amplitudes.push_back(6);
  amplitudes.push_back(5);
  amplitudes.push_back(4);
  amplitudes.push_back(3);
  amplitudes.push_back(2);
  amplitudes.push_back(1);
  //amplitudes.push_back(0);


  int nFiles = 50;

  float calibrationConst = (use_ampMax) ? 1. : 1000.;

  TFile* file_signal = TFile::Open( Form("signalHistos_%s.root", prodName.c_str()), "recreate" );

  TGraphErrors* gr_signal_vs_ampl = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_ampl_sigmaUp = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_ampl_sigmaDn = new TGraphErrors(0);

  gr_signal_vs_ampl        ->SetName( Form( "gr_%s"        , prodName.c_str()) );
  gr_signal_vs_ampl_sigmaUp->SetName( Form( "gr_%s_sigmaUp", prodName.c_str()) );
  gr_signal_vs_ampl_sigmaDn->SetName( Form( "gr_%s_sigmaDn", prodName.c_str()) );

  for( unsigned i=0; i<amplitudes.size(); ++i ) {

    TH1D* h1_signal = new TH1D( Form("signal_%d", amplitudes[i]), "", 1000, -1000., 10000. );

    for( unsigned iFile=0; iFile<nFiles; ++iFile ) {

      std::string additionalZero = (iFile<10) ? "0" : "";
      TGraph* thisGraph = 0;
      std::string thisFileName;

      for( unsigned iPrefix=0; iPrefix<prefixes.size(); ++iPrefix ) {
        std::string thisFileName_tmp( Form( "%s/%d/%s%s%d.txt", datadir.c_str(), amplitudes[i], prefixes[iPrefix].c_str(), additionalZero.c_str(), iFile ) );
        TGraph* thisGraph_tmp = NanoUVCommon::getGraphFromFile( thisFileName_tmp );
        if( thisGraph_tmp!=0 ) {
          thisFileName = thisFileName_tmp;
          thisGraph = thisGraph_tmp;
          break;
        } // if ifs good
      } // for prefixes
//TFile* file = TFile::Open("test.root", "recreate" );
//file->cd();
//thisGraph->SetName("graph");
//thisGraph->Write();
//file->Close();
//exit(1);
      NanoUVCommon::plotWaveformGraph( thisGraph, Form("%s.pdf", thisFileName.c_str()) ); 
      float thisSignal = (use_ampMax) ? NanoUVCommon::ampMaxSignal( thisGraph, invertPolarity ) : NanoUVCommon::integrateSignal( thisGraph, invertPolarity );
      h1_signal->Fill( thisSignal/calibrationConst );
      delete thisGraph;

    } // for files

    file_signal->cd();
    h1_signal->Write();

    float xValue = amplitudes[i]; //n*300.; // number of photons (see LED driver data sheet)

    int iPoint = gr_signal_vs_ampl->GetN();
    gr_signal_vs_ampl        ->SetPoint( iPoint, xValue, h1_signal->GetMean() );
    gr_signal_vs_ampl_sigmaUp->SetPoint( iPoint, xValue, h1_signal->GetMean()+h1_signal->GetRMS() );
    gr_signal_vs_ampl_sigmaDn->SetPoint( iPoint, xValue, h1_signal->GetMean()-h1_signal->GetRMS() );
    gr_signal_vs_ampl->SetPointError( iPoint, 0., h1_signal->GetMeanError() );

  } // for amplitudes

  // rescale:
  double x0, y0;
  gr_signal_vs_ampl->GetPoint( gr_signal_vs_ampl->GetN()-1, x0, y0 );

  divideGraph( gr_signal_vs_ampl_sigmaUp, y0 );
  divideGraph( gr_signal_vs_ampl_sigmaDn, y0 );
  divideGraph( gr_signal_vs_ampl        , y0 );


  file_signal->cd();
  gr_signal_vs_ampl->Write();
  file_signal->Close();

  TCanvas* c1 = new TCanvas( "c1c1", "", 600, 600 );
  c1->cd();

  float xMin = 0.;
  float xMax = 10.001; //*300;
  float yMin = (use_ampMax) ? 0. : -0.5;
  float yMax = (use_ampMax) ? 150./y0 : 5./y0;
  //float yMin = (use_ampMax) ? -180. : -5.;
  //float yMax = (use_ampMax) ? 0. : 0.5;

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes->SetYTitle("Signal / Noise" );
  //if( use_ampMax ) 
  //  h2_axes->SetYTitle("AmpMax Signal [a.u.]"); 
  //else
  //  h2_axes->SetYTitle("Integrated Signal [a.u.]"); 
  h2_axes->SetXTitle("LED Amplitude");
  //h2_axes->SetXTitle("Number of Photons");
  h2_axes->Draw();



  float lineY = 1.;

  TLine* line_zero = new TLine( xMin, lineY, xMax, lineY );
  line_zero->Draw("same");

  gr_signal_vs_ampl_sigmaUp->SetLineColor(38);
  gr_signal_vs_ampl_sigmaUp->SetLineWidth(2);
  gr_signal_vs_ampl_sigmaUp->SetLineStyle(2);
  gr_signal_vs_ampl_sigmaUp->Draw("L same");

  gr_signal_vs_ampl_sigmaDn->SetLineColor(38);
  gr_signal_vs_ampl_sigmaDn->SetLineWidth(2);
  gr_signal_vs_ampl_sigmaDn->SetLineStyle(2);
  gr_signal_vs_ampl_sigmaDn->Draw("L same");

  gr_signal_vs_ampl->SetMarkerStyle(20);
  gr_signal_vs_ampl->SetMarkerSize(1.6);
  gr_signal_vs_ampl->SetMarkerColor(46);
  gr_signal_vs_ampl->Draw("PL same");


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

  TLegend* legend = new TLegend( 0.6, 0.7, 0.88, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->AddEntry( gr_signal_vs_ampl_sigmaDn, "68% band", "L" );
  legend->Draw("same");

  NanoUVCommon::addNanoUVLabel(c1, 2);

  c1->SaveAs(Form("sigVsAmp_%s.pdf", prodName.c_str()));

  delete c1;
  delete h2_axes;
  delete label_gamma;
  delete legend;
  delete label_led;

  return gr_signal_vs_ampl;

}





void divideGraph( TGraphErrors* graph, float factor ) {

  int nPoints = graph->GetN();

  for( unsigned iPoint=0; iPoint<nPoints; ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    graph->SetPoint( iPoint, x, y/factor );

  } // for points

}

