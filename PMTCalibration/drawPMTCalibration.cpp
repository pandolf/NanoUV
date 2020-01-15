#include "NanoUVCommon.h"

#include "TGraphErrors.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"

#include <fstream>
#include <vector>
#include <string>



TGraphErrors* drawHVscan( const std::string& amp );


int main() {

  NanoUVCommon::setStyle();

  std::vector<std::string> amplitudes;
  amplitudes.push_back( "5p18" );
  amplitudes.push_back( "6p0" );
  amplitudes.push_back( "8p0" );
  amplitudes.push_back( "10p0" );
  amplitudes.push_back( "3p0" );

  system( "mkdir -p plots" );

  for( unsigned i=0; i<amplitudes.size(); ++i ) {

    drawHVscan( amplitudes[i] );

  }

  return 0;

}


TGraphErrors* drawHVscan( const std::string& amp ) {

  //float ampValue = convertAmplitude( amp );

  std::string fileName( Form( "data/A%s.txt", amp.c_str()) );
  std::ifstream ifs(fileName.c_str());

  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName( Form("gr_%s", amp.c_str()) );

  while( ifs.good() ) {

    float hv, charge, chargeErr;
    ifs >> hv >> charge >> chargeErr;

    int iPoint = graph->GetN();
    graph->SetPoint     ( iPoint, hv, -charge    );
    graph->SetPointError( iPoint, 0., chargeErr );

  }

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  c1->SetLogy();
 
  TH2D* h2_axes = new TH2D( "axes", "", 10, 501., 1099., 10, 3., 500. );
  h2_axes->SetXTitle( "PMT Bias [V]" ); 
  h2_axes->SetYTitle( "PMT Charge [pC]" );
  h2_axes->GetYaxis()->SetNoExponent();
  h2_axes->GetYaxis()->SetMoreLogLabels();
  h2_axes->Draw();

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.6);
  graph->SetMarkerColor(kGray+3);
  graph->SetLineColor(kGray+3);

  graph->Draw("P same");

  NanoUVCommon::addNanoUVLabel(c1, 4);

  c1->SaveAs( Form("plots/A%s.pdf", amp.c_str()) );
  c1->SaveAs( Form("plots/A%s.eps", amp.c_str()) );

  delete h2_axes;
  delete c1;
  
  return graph;

}
