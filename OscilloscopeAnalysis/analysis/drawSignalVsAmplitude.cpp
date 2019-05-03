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


//float getSignalFromFile( const std::string& fileName );
//void removeTimeOffset( TGraphErrors* graph );
//float getPedestal( TGraphErrors* graph, int nPoints );
//float integrateSignal( TGraph* graph, float pedestal );
//float ampMaxSignal( TGraph* graph, float pedestal );



int main( int argc, char* argv[] ) {

  std::string prodName = "May1_v2";
  if( argc > 1 ) {
    prodName = (std::string)(argv[1]);
  }


  NanoUVCommon::setStyle();


  std::string datadir = "data/LED_driver/" + prodName;
  std::string prefix = "C1211215up2000";
  std::string plotsdir = "plots";
  system( Form("mkdir -p %s", plotsdir.c_str()) );

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
  amplitudes.push_back(0);


  int nFiles = 50;

  float calibrationConst = (use_ampMax) ? 1. : 1000.;

  TFile* file_signal = TFile::Open( Form("signalHistos_%s.root", prodName.c_str()), "recreate" );

  TGraphErrors* gr_signal_vs_ampl = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_ampl_sigmaUp = new TGraphErrors(0);
  TGraphErrors* gr_signal_vs_ampl_sigmaDn = new TGraphErrors(0);

  for( unsigned i=0; i<amplitudes.size(); ++i ) {

    TH1D* h1_signal = new TH1D( Form("signal_%d", amplitudes[i]), "", 1000, -10000., 1000. );

    for( unsigned iFile=0; iFile<nFiles; ++iFile ) {

      std::string additionalZero = (iFile<10) ? "0" : "";
      std::string thisFileName( Form( "%s/%d/%s%s%d.txt", datadir.c_str(), amplitudes[i], prefix.c_str(), additionalZero.c_str(), iFile ) );
      TGraph* thisGraph = NanoUVCommon::getGraphFromFile( thisFileName.c_str() );
      NanoUVCommon::plotWaveformGraph( thisGraph, Form("%s.pdf", thisFileName.c_str()) ); 
      float thisSignal = NanoUVCommon::ampMaxSignal( thisGraph );
      h1_signal->Fill( thisSignal/calibrationConst );
      delete thisGraph;

    } // for files

    file_signal->cd();
    h1_signal->Write();

    float xValue = amplitudes[i]*300.; // number of photons (see LED driver data sheet)

    int iPoint = gr_signal_vs_ampl->GetN();
    gr_signal_vs_ampl        ->SetPoint( iPoint, xValue, h1_signal->GetMean() );
    gr_signal_vs_ampl_sigmaUp->SetPoint( iPoint, xValue, h1_signal->GetMean()+h1_signal->GetRMS() );
    gr_signal_vs_ampl_sigmaDn->SetPoint( iPoint, xValue, h1_signal->GetMean()-h1_signal->GetRMS() );
    gr_signal_vs_ampl->SetPointError( iPoint, 0., h1_signal->GetMeanError() );

  } // for amplitudes


  file_signal->cd();
  gr_signal_vs_ampl->Write();
  file_signal->Close();

  TCanvas* c1 = new TCanvas( "c1c1", "", 600, 600 );
  c1->cd();

  float xMin = 0.;
  float xMax = 10.*300;
  float yMin = (use_ampMax) ? -180. : -5.;
  float yMax = (use_ampMax) ? 0. : 0.5;

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax );
  if( use_ampMax ) 
    h2_axes->SetYTitle("AmpMax Signal (Gain = 150) [a.u.]"); 
  else
    h2_axes->SetYTitle("Integrated Signal (Gain = 150) [a.u.]"); 
  h2_axes->SetXTitle("Number of Photons");
  h2_axes->Draw();


  double x0, y0;
  gr_signal_vs_ampl->GetPoint( gr_signal_vs_ampl->GetN()-1, x0, y0 );
  float lineY = (use_ampMax) ? y0 : 0.;

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

  TLegend* legend = new TLegend( 0.6, 0.3, 0.88, 0.4 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->AddEntry( gr_signal_vs_ampl_sigmaDn, "68% band", "L" );
  legend->Draw("same");

  NanoUVCommon::addNanoUVLabel(c1, 3);

  c1->SaveAs(Form("sigVsAmp_%s.pdf", prodName.c_str()));

  return 0;

}



//float getSignalFromFile( const std::string& fileName ) {
//
//  std::ifstream ifs( fileName.c_str() );
//
//  std::string line;
//  bool read = false;
//  TGraphErrors* graph = new TGraphErrors(0);
//  graph->SetName("wf");
//
//  if( ifs.good() ) {
//
//    while( getline(ifs,line) ) {
//
//      std::string delimiter = ",";
//      size_t pos = 0;
//      std::vector<std::string> words;
//      std::string word;
//      while ((pos = line.find(delimiter)) != std::string::npos) {
//        word = line.substr(0, pos);
//        line.erase(0, pos + delimiter.length());
//        words.push_back(word);
//      }
//      line.erase( line.size()-1, line.size() ); // chopping off trailing char
//      words.push_back(line); // last part
//
//      if( read ) {
//
//        double x(atof(words[0].c_str()));
//        double y(atof(words[1].c_str()));
//
//        graph->SetPoint( graph->GetN(), x*1E9, y*1E3 ); // ns and mV
//
//      }
//
//      if( words[0]=="Time" && words[1]=="Ampl" ) {
//
//        read = true;
//      }
//
//    }  // while get line
//
//  } // if ifs good
//
//  removeTimeOffset( graph );
//
//  float pedestal = getPedestal( graph, 100 );
//
//  TCanvas* c1 = new TCanvas( Form("c1_%s", fileName.c_str()), "", 600, 600 );
//  c1->cd();
//
//  double x0, y0;
//  graph->GetPoint( 0, x0, y0 );
//
//  double xmax, ymax;
//  graph->GetPoint( graph->GetN()-1, xmax, ymax );
//
//
//  TH2D* h2_axes = new TH2D( Form("axes_%s", fileName.c_str()), "", 10, x0, xmax, 10, pedestal-150., pedestal+15. );
//  h2_axes->SetXTitle( "Time [ns]" );
//  h2_axes->SetYTitle( "Amplitude [mV]" );
//  h2_axes->Draw();
//
//  TLine* line_pedestal = new TLine( x0, pedestal, xmax, pedestal );
//  line_pedestal->SetLineColor(46);
//  line_pedestal->SetLineStyle(2);
//  line_pedestal->SetLineWidth(2);
//  line_pedestal->Draw("same");
//
//  graph->Draw("L same");
//
//  gPad->RedrawAxis();
//
//  c1->SaveAs( Form("%s.pdf", fileName.c_str()) );
//  
//  float signal = NanoUVCommon::ampMaxSignal( graph, pedestal );
//  //float signal = NanoUVCommon::integrateSignal( graph, pedestal );
//
//  delete graph;
//  delete c1;
//  delete h2_axes;
//  delete line_pedestal;
//
//  return signal;
//
//}
//
//
//void removeTimeOffset( TGraphErrors* graph ) {
//
//  double x0, y0;
//  graph->GetPoint( 0, x0, y0 );
//
//  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {
//
//    double x, y;
//    graph->GetPoint( iPoint, x, y );
//    graph->SetPoint( iPoint, x-x0, y );
//
//  }
//
//}
//
//
//
//float getPedestal( TGraphErrors* graph, int nPoints ) {
//
//  float sum = 0.;
//
//  for( unsigned iPoint=0; iPoint<nPoints; ++iPoint ) {
//
//    double x, y;
//    graph->GetPoint( iPoint, x, y );
//    sum += y;
//
//  }
//
//  return sum/((float)nPoints);
//
//}


