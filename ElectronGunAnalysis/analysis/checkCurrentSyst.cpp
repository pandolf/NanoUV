#include "../../CommonTools/interface/NanoUVCommon.h"
#include "../../ElectronGunAnalysis/interface/GunScan.h"

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
#include "TStyle.h"




int main( int argc, char* argv[] ) {


  float APDhv;

  if( argc==1 ) {

    std::cout << "USAGE: ./checkCurrentSyst [APDhv]" << std::endl;
    exit(1);

  } else {

    APDhv = atof(argv[1]);

  }

  float gunEnergy = 900.;

  NanoUVCommon::setStyle();

  std::vector< GunScan* > scans  = GunScan::loadScans( "data/2020_02/scans.txt", 900, APDhv );

  TH1D* h1_syst = new TH1D( "syst", "", 50, -0.8, 0.8 );

  TGraphErrors* gr_syst = new TGraphErrors(0);


  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

    if( scans[iScan]->APDhv() != APDhv ) continue;

    float iapd_nominal = scans[iScan]->getCurrentFromScan();

    scans[iScan]->set_currentMethod("integral");
    float iapd_syst = scans[iScan]->getCurrentFromScan();

    std::cout << std::endl;
    std::cout << "---- Gun current: " << scans[iScan]->gunCurrent() << std::endl;
    std::cout << "Nominal I(apd): " << iapd_nominal << std::endl;
    std::cout << "Syst   I(apd): " << iapd_syst << std::endl;

    float syst = fabs(iapd_nominal-iapd_syst)/iapd_nominal;
    h1_syst ->Fill( syst );

    gr_syst->SetPoint( gr_syst->GetN(), scans[iScan]->gunCurrent(), syst );

  } // for scans


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 20., 10, 0., 1. );
  h2_axes->SetXTitle( "I_{gun} [pA]" );
  h2_axes->SetYTitle( "I_{apd} [pA]" );
  h2_axes->Draw();

  TPaveText* label_settings = new TPaveText( 0.23, 0.65, 0.5, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{e} = %.0f eV", gunEnergy) );
  label_settings->AddText( Form("V_{apd} = %.0f V", APDhv) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");

  gr_syst->SetMarkerStyle(20);
  gr_syst->SetMarkerSize(1.3);
  gr_syst->SetMarkerColor(kGray+3);
  gr_syst->SetLineColor(kGray+3);
  gr_syst->Draw("Psame");

  NanoUVCommon::addNanoUVLabel( c1, 2 );

  label_settings->Draw("same");

  c1->SaveAs( Form("systCurrent_E%.0feV_APDhv%.0f.pdf", gunEnergy, APDhv) );

  delete c1;
  delete h2_axes;


  std::cout << "-> The systematic uncertainty is: " << h1_syst->GetMean() << " percent" << std::endl;

  return 0;

}    
