#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH2D.h"

#include "../interface/NanoUVCommon.h"





int main() {

  NanoUVCommon::setStyle();

  std::vector<float> energies;
  //energies.push_back(90.); 
  energies.push_back(500.); 
  energies.push_back(900.); 


  TGraphErrors* graph = new TGraphErrors(0);

  for( unsigned i=0; i<energies.size(); ++i ) {

    TFile* fitResultsFile = TFile::Open( Form("plots/APDscans/2020_02/%.0feV/350V/fitResults.root", energies[i]) );

    TF1* fitFunction = (TF1*) fitResultsFile->Get("lineScan");

    graph->SetPoint     ( i, energies[i], fitFunction->GetParameter(1) );
    graph->SetPointError( i,        0.05, fitFunction->GetParError (1) );

  } // for energies


  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.5);
  graph->SetMarkerColor(kGray+3);
  graph->SetLineColor(kGray+3);


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 1100., 10, 0., 500. );
  h2_axes->SetXTitle( "Electron energy [eV]" );
  h2_axes->SetYTitle( "G" );
  h2_axes->Draw();

  graph->Draw("P same");

  NanoUVCommon::addNanoUVLabel( c1, 2 );
  c1->SaveAs("gainVsEnergy.pdf");  

  return 0;

} 
