#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TLine.h"

#include "NanoUVCommon.h"


void removeTimeOffset( TGraph* graph );
float getPedestal( TGraph* graph, int nPoints );
float integrateSignal( TGraph* graph, float pedestal );



int main() {

  NanoUVCommon::setStyle();

  std::string datadir = "data_oscill";
  std::string plotsdir = "plots";
  system( Form("mkdir -p %s", plotsdir.c_str()) );

  std::vector< std::pair< std::string, float > > files_amps;
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200000", 10.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200001", 10.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200002", 10.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200003",  9.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200004",  9.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200005",  8.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200006",  8.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200007",  7.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200008",  7.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200009",  6.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200010",  6.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200011",  5.) );
  files_amps.push_back( std::pair< std::string, float > ("C1211215up200012",  5.) );

  TGraph* gr_signal_vs_ampl = new TGraph(0);

  for( unsigned i=0; i<files_amps.size(); ++i ) {

    std::string fileName( Form("%s/%s.txt", datadir.c_str(),files_amps[i].first.c_str()) );
    std::ifstream ifs( fileName.c_str() );

    std::string line;
    bool read = false;
    TGraph* graph = new TGraph(0);
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

    float pedestal = getPedestal( graph, 20 );

    TCanvas* c1 = new TCanvas( Form("c1_%s", files_amps[i].first.c_str()), "", 600, 600 );
    c1->cd();

    double x0, y0;
    graph->GetPoint( 0, x0, y0 );

    double xmax, ymax;
    graph->GetPoint( graph->GetN()-1, xmax, ymax );


    TH2D* h2_axes = new TH2D( Form("axes_%s", files_amps[i].first.c_str()), "", 10, x0, xmax, 10, pedestal-100., pedestal+10. );
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

    c1->SaveAs( Form("%s/%s.pdf", datadir.c_str(), files_amps[i].first.c_str()) );
    
    float signal = integrateSignal( graph, pedestal );

    gr_signal_vs_ampl->SetPoint( gr_signal_vs_ampl->GetN(), files_amps[i].second, signal );

  } // for files

  TFile* file = TFile::Open( "test.root", "recreate" );
  file->cd();

  gr_signal_vs_ampl->Write();

  file->Close();

  return 0;

}


void removeTimeOffset( TGraph* graph ) {

  double x0, y0;
  graph->GetPoint( 0, x0, y0 );

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    graph->SetPoint( iPoint, x-x0, y );

  }

}



float getPedestal( TGraph* graph, int nPoints ) {

  float sum = 0.;

  for( unsigned iPoint=0; iPoint<nPoints; ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    sum += y;

  }

  return sum/((float)nPoints);

}


float integrateSignal( TGraph* graph, float pedestal ) {

  float signal = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    signal = signal + (y-pedestal);

  }

  return signal;

}

