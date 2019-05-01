#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TH2D.h"
#include "TLine.h"

#include "NanoUVCommon.h"


float getSignalFromFile( const std::string& fileName );
void removeTimeOffset( TGraphErrors* graph );
float getPedestal( TGraphErrors* graph, int nPoints );
float integrateSignal( TGraphErrors* graph, float pedestal );



int main() {

  NanoUVCommon::setStyle();


  std::string datadir = "data/LED_driver/May1/";
  std::string prefix = "C1211215up20000";
  std::string plotsdir = "plots";
  system( Form("mkdir -p %s", plotsdir.c_str()) );

  std::vector<int> amplitudes;
  amplitudes.push_back(10);
  amplitudes.push_back(9);
  amplitudes.push_back(8);
  amplitudes.push_back(7);
  amplitudes.push_back(6);
  amplitudes.push_back(5);
  amplitudes.push_back(1);


  int nFiles = 50;

  TGraphErrors* gr_signal_vs_ampl = new TGraphErrors(0);

  for( unsigned i=0; i<amplitudes.size(); ++i ) {

    TH1D* h1_signal = new TH1D( Form("signal_%d", i), "", 100, -2000., 0. );

    for( unsigned iFile=0; iFile<nFiles; ++iFile ) {

      float thisSignal = getSignalFromFile( Form( "%s/%d/%s%d.txt", datadir.c_str(), amplitudes[i], prefix.c_str(), iFile ) );
      h1_signal->Fill( thisSignal );

    } // for files

    int iPoint = gr_signal_vs_ampl->GetN();
    gr_signal_vs_ampl->SetPoint     ( iPoint, amplitudes[i], h1_signal->GetMean()      );
    gr_signal_vs_ampl->SetPointError( iPoint,            0., h1_signal->GetMeanError() );

  } // for amplitudes

  TFile* file = TFile::Open( "test.root", "recreate" );
  file->cd();
  gr_signal_vs_ampl->Write();
  file->Close();

  return 0;

}



float getSignalFromFile( const std::string& fileName ) {

  std::ifstream ifs( fileName.c_str() );

  std::string line;
  bool read = false;
  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName("wf");

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      std::string delimiter = ",";
      size_t pos = 0;
      std::vector<std::string> words;
      std::string word;
      while ((pos = line.find(delimiter)) != std::string::npos) {
        word = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        words.push_back(word);
      }
      line.erase( line.size()-1, line.size() ); // chopping off trailing char
      words.push_back(line); // last part

      if( read ) {

        double x(atof(words[0].c_str()));
        double y(atof(words[1].c_str()));

        graph->SetPoint( graph->GetN(), x*1E9, y*1E3 ); // ns and mV

      }

      if( words[0]=="Time" && words[1]=="Ampl" ) {

        read = true;
      }

    }  // while get line

  } // if ifs good

  removeTimeOffset( graph );

  float pedestal = getPedestal( graph, 100 );

  TCanvas* c1 = new TCanvas( Form("c1_%s", fileName.c_str()), "", 600, 600 );
  c1->cd();

  double x0, y0;
  graph->GetPoint( 0, x0, y0 );

  double xmax, ymax;
  graph->GetPoint( graph->GetN()-1, xmax, ymax );


  TH2D* h2_axes = new TH2D( Form("axes_%s", fileName.c_str()), "", 10, x0, xmax, 10, pedestal-100., pedestal+10. );
  h2_axes->SetXTitle( "Time [ns]" );
  h2_axes->SetYTitle( "Amplitude [mV]" );
  h2_axes->Draw();

  TLine* line_pedestal = new TLine( x0, pedestal, xmax, pedestal );
  line_pedestal->SetLineColor(46);
  line_pedestal->SetLineStyle(2);
  line_pedestal->SetLineWidth(2);
  line_pedestal->Draw("same");

  graph->Draw("L same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s.pdf", fileName.c_str()) );
  
  float signal = integrateSignal( graph, pedestal );

  delete graph;
  delete c1;
  delete h2_axes;
  delete line_pedestal;

  return signal;

}


void removeTimeOffset( TGraphErrors* graph ) {

  double x0, y0;
  graph->GetPoint( 0, x0, y0 );

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    graph->SetPoint( iPoint, x-x0, y );

  }

}



float getPedestal( TGraphErrors* graph, int nPoints ) {

  float sum = 0.;

  for( unsigned iPoint=0; iPoint<nPoints; ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    sum += y;

  }

  return sum/((float)nPoints);

}


float integrateSignal( TGraphErrors* graph, float pedestal ) {

  float signal = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    signal = signal + (y-pedestal);

  }

  return signal;

}

