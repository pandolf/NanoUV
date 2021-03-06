#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "../../CommonTools/interface/NanoUVCommon.h"



void drawGraph( const std::string& name, TGraphErrors* graph, const std::string& axisName );

int main() {

  NanoUVCommon::setStyle();

  TGraphErrors* graph = new TGraphErrors(0);
 
  // numbers taken from data/2019_05_17/current_scan.txt 
  graph->SetPoint( graph->GetN(), 3.5, 0.16*1000 );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.05, 0.01*1000. );

  graph->SetPoint( graph->GetN(), 9.9, 0.48*1000. );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.05, 0.01*1000. );

  graph->SetPoint( graph->GetN(), 14.5, 0.71*1000. );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.1, 0.01*1000. );

  graph->SetPoint( graph->GetN(), 21.9, 1.06*1000. );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.1, 0.01*1000. );

  graph->SetPoint( graph->GetN(), 29.3, 1.43*1000. );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.1, 0.01*1000. );

  graph->SetPoint( graph->GetN(), 38, 1.82*1000. );  // in nA
  graph->SetPointError( graph->GetN()-1, 0.1, 0.01*1000. );


  TGraphErrors* graph_noGain = new TGraphErrors(0);

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    graph_noGain->SetPoint( iPoint, x, y/55. ); // gain @ 355 V is 55, then put in nA
    graph_noGain->SetPointError( iPoint, graph->GetErrorX(iPoint), graph->GetErrorY(iPoint)/55. );

  } 

  drawGraph( "currentScan", graph, "I(APD) [nA]" );
  drawGraph( "currentScan_noGain", graph_noGain, "I(APD) / Gain [nA]" );

  return 0;

}




void drawGraph( const std::string& name, TGraphErrors* graph, const std::string& axisName ) {

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  float xMin = 0.;
  float xMax = 40.;
  float yMax = (name=="currentScan") ? 2000. : 40.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "I(gun) [nA]" );
  h2_axes->SetYTitle( axisName.c_str() );
  h2_axes->Draw();


  graph->SetMarkerColor( kGray+3 );
  graph->SetLineColor  ( kGray+3 );
  graph->SetMarkerStyle( 20 );
  graph->SetMarkerSize( 1.3 );

  TF1* f1 = new TF1("line", "[0] + [1]*x", xMin, xMax );
  f1->SetLineColor( 46 );
  f1->SetLineWidth( 2 );
  f1->SetParameter( 0, 0. );
  f1->SetParameter( 1, 4.82322e-2 );

  graph->Fit( f1, "R" );

  TPaveText* labelGun = new TPaveText( 0.22, 0.6, 0.45, 0.7, "brNDC" );
  labelGun->SetFillColor(0);
  labelGun->SetTextSize(0.035);
  labelGun->AddText( "E(gun) = 500 eV" );
  labelGun->AddText( "V(APD) = 355 V" );
  labelGun->Draw("same");

  TPaveText* labelFit = new TPaveText( 0.5, 0.17, 0.9, 0.42, "brNDC" );
  labelFit->SetFillColor(0);
  labelFit->SetTextSize(0.035);
  labelFit->SetTextAlign(31);
  labelFit->AddText( Form("Linear Fit (mx + q)"));
  labelFit->AddText( Form("#chi^{2}/NDF = %.1f/%d", f1->GetChisquare(), f1->GetNDF()) );
  //labelFit->AddText( Form("m = (%.1f #pm %.1f)#times10^{-3}", f1->GetParameter(1)*1000., f1->GetParError(1)*1000.) );
  //labelFit->AddText( Form("q = %.3f #pm %.3f #muA", f1->GetParameter(0), f1->GetParError(0)) );
  labelFit->AddText( Form("m = %.1f #pm %.1f", f1->GetParameter(1), f1->GetParError(1)) );
  labelFit->AddText( Form("q = %.1f #pm %.1f nA", f1->GetParameter(0), f1->GetParError(0)) );
  labelFit->Draw("same");

  graph->Draw("P same");

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  gPad->RedrawAxis();
  label->Draw("same");
  
  c1->SaveAs( Form( "%s.pdf", name.c_str()) );

  delete h2_axes;
  delete c1;
  delete f1;
  delete labelGun;
  delete labelFit;

}


