#include "NanoUVCommon.h"

#include "TGraphErrors.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TPaveText.h"
#include "TF1.h"

#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>



TGraphErrors* drawHVscan( const std::string& amp );
float findLastPoint( TGraph* graph, float threshold );
float convertAmpToFloat( std::string amp );
void drawNPhotons( const std::vector<std::string>& amplitudes, const std::vector<TGraphErrors*>& graphs, TGraph* gr_gain, float hv );


int main() {

  NanoUVCommon::setStyle();

  std::vector<std::string> amplitudes;
  amplitudes.push_back( "5p18" );
  amplitudes.push_back( "6p00" );
  amplitudes.push_back( "8p00" );
  amplitudes.push_back( "10p00" );
  //amplitudes.push_back( "3p0" );

  system( "mkdir -p plots" );

  std::vector<TGraphErrors*> graphs;

  for( unsigned i=0; i<amplitudes.size(); ++i ) {

    graphs.push_back( drawHVscan(amplitudes[i]) );

  }

  TGraph* gr_gain = new TGraph(0);
  gr_gain->SetPoint( gr_gain->GetN(), 500., 1E4 );
  gr_gain->SetPoint( gr_gain->GetN(), 550., 2E4 );
  gr_gain->SetPoint( gr_gain->GetN(), 600., 4E4 );
  gr_gain->SetPoint( gr_gain->GetN(), 650., 7E4 );
  gr_gain->SetPoint( gr_gain->GetN(), 700., 1.3E5 );
  gr_gain->SetPoint( gr_gain->GetN(), 750., 2.1E5 );
  gr_gain->SetPoint( gr_gain->GetN(), 800., 3.6E5 );

  drawNPhotons( amplitudes, graphs, gr_gain, 650. );

  //gr_gain->SetMarkerStyle(20);
  //gr_gain->SetMarkerSize(1.6);
  //gr_gain->SetMarkerColor(kGray+3);

  //TF1* f1_gain = new TF1( "gain", "[0]*exp([1]*x)", 450., 1000. );
  //f1_gain->SetParameter(0, 0.01 );
  //f1_gain->SetLineColor(46);
  //f1_gain->SetLineWidth(3);
  //gr_gain->Fit( f1_gain, "R" );

  //TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  //c1->cd();
  //c1->SetLogy();
  //
  //TH2D* h2_axes = new TH2D("axes", "", 10, 451., 999., 10, 5E3, 1E6 );
  //h2_axes->SetYTitle( "PMT Gain" );
  //h2_axes->SetXTitle( "PMT Bias [V]" );
  //h2_axes->Draw();

  //gr_gain->Draw("p same");

  //c1->SaveAs( "plots/gain.pdf" );

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
  //c1->SetLogx();
 
  TH2D* h2_axes = new TH2D( "axes", "", 10, 501., 1099., 10, 3., 500. );
  h2_axes->SetXTitle( "PMT Bias [V]" ); 
  h2_axes->SetYTitle( "PMT Charge [pC]" );
  h2_axes->GetYaxis()->SetNoExponent();
  h2_axes->GetYaxis()->SetMoreLogLabels();
  h2_axes->Draw();

  TString amp_tstr(amp);
  amp_tstr.ReplaceAll( "p", "." );
  std::string amp2(amp_tstr);
  TPaveText* label_led = new TPaveText( 0.2, 0.74, 0.44, 0.89, "brNDC" );
  label_led->SetFillColor(0);
  label_led->SetTextColor(kGray+1);
  label_led->SetTextSize(0.035);
  label_led->SetTextAlign( 11 );
  label_led->AddText( "CAEN UV LED Driver SP5605" );
  label_led->AddText( "#lambda = 248 nm (E = 5 eV)");
  label_led->AddText( Form("Amplitude = %s", amp2.c_str()) );
  label_led->Draw("same");

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.6);
  graph->SetMarkerColor(kGray+3);
  graph->SetLineColor(kGray+3);

  //float xMaxFit = findLastPoint( graph, 80. );

  //TF1* f1_exp = new TF1( Form("f1_%s", graph->GetName()), "[0]*exp([1]*x)", 500., xMaxFit+1.);
  //f1_exp->SetParameter(0, 0.01);
  //f1_exp->SetLineColor(46);
  //f1_exp->SetLineWidth(3);
  //graph->Fit( f1_exp, "R0" );

  //f1_exp->Draw("same");

  //TF1* otherExp = new TF1( Form("otherExp_%s", graph->GetName()), "[0]*exp([1]*x)", 500., 1000. );
  //otherExp->SetParameter( 0, f1_exp->GetParameter(0) );
  //otherExp->SetParameter( 1, f1_exp->GetParameter(1) );
  //otherExp->SetLineColor(46);
  //otherExp->SetLineWidth(3);
  //otherExp->SetLineStyle(2);
  //otherExp->Draw("same");

  graph->Draw("P same");


  NanoUVCommon::addNanoUVLabel(c1, 4);


  c1->SaveAs( Form("plots/A%s.pdf", amp.c_str()) );
  c1->SaveAs( Form("plots/A%s.eps", amp.c_str()) );

  delete h2_axes;
  delete c1;
  
  return graph;

}


float findLastPoint( TGraph* graph, float threshold ) {

  float foundX = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y) ;

    if( y > threshold ) break;

    foundX = x;

  }

  return foundX;

} 



float convertAmpToFloat( std::string amp ) {

  std::string delimiter = "p";
  size_t pos = 0;
  std::vector<float> parts;
  float part;
  while ((pos = amp.find(delimiter)) != std::string::npos) {
    std::cout << amp.substr(0, pos) << std::endl;
    part = atof(amp.substr(0, pos).c_str());
    amp.erase(0, pos + delimiter.length());
    parts.push_back(part);
  }
  parts.push_back(atof(amp.c_str()));

  float decimalPart = (parts[1]>=10) ? parts[1]/100. : parts[1]/10.;
  float ampValue = parts[0] + decimalPart;

  return ampValue;
 
}



void drawNPhotons( const std::vector<std::string>& amplitudes, const std::vector<TGraphErrors*>& graphs, TGraph* gr_gain, float hv ) {

  TGraphErrors* gr_ch_vs_amp = new TGraphErrors(0);

  for( unsigned i=0; i<graphs.size(); ++i ) {

std::cout << "starting graph: " << graphs[i]->GetName() << std::endl;
    float charge, chargeErr;

    for( unsigned iPoint=0; iPoint<graphs[i]->GetN(); ++iPoint ) {

      double x, y;
      graphs[i]->GetPoint( iPoint, x, y );
std::cout << "point: " << x << " " << y << std::endl;
      if( x==hv ) {
        charge = y;
        chargeErr = graphs[i]->GetErrorY( iPoint );
      }

    } // for points

    gr_ch_vs_amp->SetPoint( gr_ch_vs_amp->GetN(), convertAmpToFloat(amplitudes[i]), charge );

  } // for graphs

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  gr_ch_vs_amp->Draw("APE*");

  c1->SaveAs( "gr650.pdf" );

}
