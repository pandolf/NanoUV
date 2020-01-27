#include "../../CommonTools/interface/NanoUVCommon.h"
#include "../../ElectronGunAnalysis/interface/GunScanTool.h"

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



std::string data = "2019_11_28";


int main( int argc, char* argv[] ) {

  float gunEnergy;
  float APDhv;

  if( argc < 3 ) {

    std::cout << "USAGE: ./drawAPDGunScan [gunEnergy] [APDhv]" << std::endl;
    exit(1);

  } else {

    gunEnergy = atof(argv[1]);
    APDhv = atof(argv[2]);

  }

  NanoUVCommon::setStyle();

  GunScanTool gst( gunEnergy, APDhv, data);

  std::string outdir( Form("plots/APDscans/%s/", data.c_str() ) );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TGraphErrors* gr_iapd_vs_igun = new TGraphErrors(0);

  float xMax;
  float yMax;

  if( gunEnergy == 500. ) {

    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_750fA_dfh_APD__28nov19_07_M_.dat", 0.8, 0.75 );
    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_3.6pA_dfh_APD__28nov19_06_M_.dat", 3.7, 3.6  );
    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_26pA_dfh_APD__28nov19_05_M_.dat" , 27.7, 26. );

//  gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN()  , 0.5*(0.8+0.75), 1000.*get_iAPD("Ek_500_750fA_dfh_APD__28nov19_07_M_.dat", 500, 0.8 /*0.75*/) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.05, 10. );

//  gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 0.5*(3.7+3.6) , 1000.*get_iAPD("Ek_500_3.6pA_dfh_APD__28nov19_06_M_.dat", 500, 3.7 /*3.6*/) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.1, 10. );

//  gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 0.5*(27.7+26.) , 1000.*get_iAPD("Ek_500_26pA_dfh_APD__28nov19_05_M_.dat" , 500, 27.7 /*26.*/) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 1.7, 10. );

    xMax = 30.;
    yMax = 13000.;

  } else if( gunEnergy == 900. ) {

//  gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 0.5*(0.44+0.45), 1000.*get_iAPD("Ek_900_450fA_dfv.txt", 900, 0.44) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 0.01, 1000. );

//  gr_iapd_vs_igun->SetPoint     ( gr_iapd_vs_igun->GetN(), 0.5*(1.5+3.3) , 1000.*get_iAPD("Ek_900_3.3pA_dfv_28nov19_01_M_.dat", 900, 1.5) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 1.8, 10. );

//  gr_iapd_vs_igun->SetPoint( gr_iapd_vs_igun->GetN(), 0.5*(15.5+17.) , 1000.*get_iAPD("Ek_900_15pA_dfv.txt" , 900, 17) );
//  gr_iapd_vs_igun->SetPointError( gr_iapd_vs_igun->GetN()-1, 1.5, 1000. );

    xMax = 20.;
    yMax = 75000.;

  }


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "Gun Current [pA]" );
  h2_axes->SetYTitle( "APD Current [pA]" );
  h2_axes->Draw();

  TF1* f1_line = new TF1( "lineScan", "[0] + [1]*x", 0., xMax );
  gr_iapd_vs_igun->Fit( f1_line, "QR0" );

  f1_line->SetLineWidth(2);
  f1_line->SetLineColor(46);
  f1_line->SetLineStyle(2);
  f1_line->Draw("L same");

  gr_iapd_vs_igun->SetMarkerStyle(20);
  gr_iapd_vs_igun->SetMarkerSize(1.6);
  gr_iapd_vs_igun->SetMarkerColor(kGray+3);
  gr_iapd_vs_igun->SetLineColor(kGray+3);
  gr_iapd_vs_igun->Draw("P same");


  TPaveText* fitResults = new TPaveText( 0.6, 0.23, 0.9, 0.44, "brNDC" );
  fitResults->SetTextSize( 0.035 );
  fitResults->SetFillColor(0);
  fitResults->SetTextColor(46);
  fitResults->AddText( "f(x) = q + m*x" );
  fitResults->AddText( Form("q = %.1f #pm %.1f pA", f1_line->GetParameter(0), f1_line->GetParError(0) ) );
  fitResults->AddText( Form("m = %.1f #pm %.1f", f1_line->GetParameter(1), f1_line->GetParError(1) ) );
  fitResults->Draw("same");

  TPaveText* label_settings = new TPaveText( 0.23, 0.6, 0.5, 0.72, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy) );
  label_settings->AddText( "I_{APD} = 380 V" );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/APDscans/%s/iapd_vs_igun_E%.0f.pdf", data.c_str(), gunEnergy) );
  c1->SaveAs( Form("plots/APDscans/%s/iapd_vs_igun_E%.0f.eps", data.c_str(), gunEnergy) );
  
  return 0;

}

