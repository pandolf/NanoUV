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



std::string currentMethod = "integral";


int main( int argc, char* argv[] ) {

  std::string data;
  float gunEnergy;
  float APDhv;

  if( argc < 2 ) {

    std::cout << "USAGE: ./drawAPDGunScan [data] [gunEnergy(default=900 eV)] [APDhv(default=350 V)]" << std::endl;
    exit(1);

  } else {

    data = std::string(argv[1]);
    gunEnergy = (argc<3) ? 900. : atof(argv[2]);
    APDhv     = (argc<4) ? 350. : atof(argv[3]);

  }

  NanoUVCommon::setStyle();

  std::vector< GunScan* > scans  = GunScan::loadScans( Form("data/%s/scans.dat", data.c_str()), gunEnergy, APDhv );
  std::vector< GunScan* > scans2 = GunScan::loadScans( Form("data/2019_11_28/scans.dat"), gunEnergy, APDhv );

  TGraphErrors* gr_iapd_vs_igun  = new TGraphErrors(0);
  TGraphErrors* gr_iapd_vs_igun2 = new TGraphErrors(0);

  float xMax;
  float yMax;
  float xMax_log;
  float yMax_log;

//if( data == "2019_11_28" ) {

//  gst.set_APDhv(380.);
//  gst.set_firstN_fit(7);
//  gst.set_lastN_fit(5);

//  if( gunEnergy == 500. ) {
//
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_750fA_dfh_APD__28nov19_07_M_.dat", 0.8, 0.75 );
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_3.6pA_dfh_APD__28nov19_06_M_.dat", 3.7, 3.6  );
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_500_26pA_dfh_APD__28nov19_05_M_.dat" , 27.7, 26. );
//
//    xMax = 30.;
//    yMax = 13000.;
//    xMax_log = 30.;
//    yMax_log = 13000.;
//
//  } else if( gunEnergy == 900. ) {
//
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_900_450fA_dfv.txt", 0.44, 0.45 );
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_900_3.3pA_dfv_28nov19_01_M_.dat", 1.5, 3.3 );
//    gst.addPointToGraph( gr_iapd_vs_igun, "Ek_900_15pA_dfv.txt", 15.5, 17. );
//
//    xMax = 20.;
//    yMax = 75000.;
//    xMax_log = 20.;
//    yMax_log = 75000.;
//
//  }

//} else if( data == "2020_01" ) {

//    gst .set_currentMethod("integral");
//    gst2.set_currentMethod("integral");
//    gst2.set_firstN_fit(7);
//    gst2.set_lastN_fit(5);
//
//    if( gunEnergy == 900. ) {
//  
//      if( APDhv == 380. ) {
//
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_05_M_.dat", 1.21, 1.21 ); // see logbook
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_09_M_.dat", 0.378, 0.375 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_13_M_.dat", 0.106, 0.111 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_17_M_.dat", 0.039, 0.040 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_01_M_.dat", 165., 207. );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_18_M_.dat", 9.9, 10.2 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_14_M_.dat", 30.3, 29.5 );
//
//        gst2.addPointToGraph( gr_iapd_vs_igun2, "Ek_900_450fA_dfv.txt", 0.44, 0.45 );
//        gst2.addPointToGraph( gr_iapd_vs_igun2, "Ek_900_3.3pA_dfv_28nov19_01_M_.dat", 1.5, 3.3 );
//        gst2.addPointToGraph( gr_iapd_vs_igun2, "Ek_900_15pA_dfv.txt", 15.5, 17. );
//
//      } else if( APDhv == 370. ) {
//
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_06_M_.dat", 1.21, 1.21 ); 
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_10_M_.dat", 0.378, 0.375 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_14_M_.dat", 0.106, 0.111 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_18_M_.dat", 0.039, 0.040 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_03_M_.dat", 165., 207. );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_19_M_.dat", 9.9, 10.2 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_15_M_.dat", 30.3, 29.5 );
//  
//      } else if( APDhv == 360. ) {
//
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_07_M_.dat", 1.21, 1.21 ); 
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_11_M_.dat", 0.378, 0.375 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_15_M_.dat", 0.106, 0.111 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_19_M_.dat", 0.039, 0.040 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_04_M_.dat", 165., 207. );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_20_M_.dat", 9.9, 10.2 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_16_M_.dat", 30.3, 29.5 );
//  
//      } else if( APDhv == 350. ) {
//
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_08_M_.dat", 1.21, 1.21 ); 
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_12_M_.dat", 0.378, 0.375 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_16_M_.dat", 0.106, 0.111 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "22gen20_20_M_.dat", 0.039, 0.040 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_05_M_.dat", 165., 207. );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_21_M_.dat", 9.9, 10.2 );
//        gst.addPointToGraph( gr_iapd_vs_igun, "23gen20_17_M_.dat", 30.3, 29.5 );
//  
//      } // APDhv
//
//      //xMax = 2.;
//      //xMax_log = 20.;
//      //yMax = 1600.*gst.APDhv()*gst.APDhv()/(350.*350.);
//      //yMax_log = 11000.;
//
//      xMax = 1.2*gst.getXmax(gr_iapd_vs_igun);
//      xMax_log = 5.*xMax;
//      yMax = 1.2*gst.getYmax(gr_iapd_vs_igun);
//      yMax_log = 5.*yMax;
//
//    } // if gunEnergy
//
//  } // data



  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

      if( scans[iScan]->gunEnergy() != gunEnergy ) continue;
      if( scans[iScan]->APDhv() != APDhv ) continue;

      scans[iScan]->set_currentMethod(currentMethod);
      scans[iScan]->addPointToGraph( gr_iapd_vs_igun );

  }


  for( unsigned iScan=0; iScan<scans2.size(); iScan++ ) {

      if( scans2[iScan]->gunEnergy() != gunEnergy ) continue;
      if( scans2[iScan]->APDhv() != APDhv ) continue;

      scans2[iScan]->set_currentMethod(currentMethod);
      scans2[iScan]->set_firstN_fit(7);
      scans2[iScan]->set_lastN_fit(5);
      scans2[iScan]->addPointToGraph( gr_iapd_vs_igun2 );

  }

  xMax = 1.2*GunScan::getXmax(gr_iapd_vs_igun);
  xMax_log = 5.*xMax;
  yMax = 1.2*GunScan::getYmax(gr_iapd_vs_igun);
  yMax_log = 5.*yMax;



  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle( "Gun Current [pA]" );
  h2_axes->SetYTitle( "APD Current [pA]" );
  h2_axes->Draw();

  TPaveText* label_settings = new TPaveText( 0.23, 0.65, 0.5, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{gun} = %.0f eV", gunEnergy) );
  label_settings->AddText( Form("I_{APD} = %.0f V", APDhv) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");

  float xFitMin, xFitMax;
  if( APDhv==350. ) {
    xFitMin = 0.1;
    xFitMax = 15.;
  } else if( APDhv==380. && gunEnergy==500.) {
    xFitMin = 0.5;
    xFitMax = 100.;
  } else {
    xFitMin = 0.;
    xFitMax = 15.;
  }
  TF1* f1_line = new TF1( "lineScan", "[0] + [1]*x", xFitMin, xFitMax );
  //TF1* f1_line = new TF1( "lineScan", "[0] + [1]*x", 0., xMax );
  gr_iapd_vs_igun->Fit( f1_line, "QR0" );

  TPaveText* fitResults = new TPaveText( 0.6, 0.2, 0.9, 0.44, "brNDC" );
  fitResults->SetTextSize( 0.035 );
  fitResults->SetFillColor(0);
  fitResults->SetTextColor(46);
  fitResults->AddText( "f(x) = q + m*x" );
  fitResults->AddText( Form("q = %.1f #pm %.1f pA", f1_line->GetParameter(0), f1_line->GetParError(0) ) );
  fitResults->AddText( Form("m = %.1f #pm %.1f", f1_line->GetParameter(1), f1_line->GetParError(1) ) );
  fitResults->AddText( Form("#chi^{2} / NDF = %.2f / %d", f1_line->GetChisquare(), f1_line->GetNDF() ) );
  fitResults->Draw("same");


  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
  label->Draw("same");  
  gPad->RedrawAxis();


  f1_line->SetLineWidth(2);
  f1_line->SetLineColor(46);
  f1_line->SetLineStyle(2);
  f1_line->Draw("L same");

  gr_iapd_vs_igun->SetMarkerStyle(20);
  gr_iapd_vs_igun->SetMarkerSize(1.6);
  gr_iapd_vs_igun->SetMarkerColor(kGray+3);
  gr_iapd_vs_igun->SetLineColor(kGray+3);
  gr_iapd_vs_igun->Draw("P same");

  gr_iapd_vs_igun2->SetMarkerStyle(24);
  gr_iapd_vs_igun2->SetMarkerSize(1.6);
  gr_iapd_vs_igun2->SetMarkerColor(kGray+3);
  gr_iapd_vs_igun2->SetLineColor(kGray+3);
  gr_iapd_vs_igun2->Draw("P same");


  c1->SaveAs( Form("%s/iapd_vs_igun.pdf", scans[0]->outdir().c_str()) );
  c1->SaveAs( Form("%s/iapd_vs_igun.eps", scans[0]->outdir().c_str()) );

  c1->Clear();

  c1->SetLogx();
  c1->SetLogy();

  float yMin = (currentMethod=="integral") ? 30. : 10.;

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, 0.01, xMax_log, 10, yMin, yMax_log );
  h2_axes_log->SetXTitle( "Gun Current [pA]" );
  h2_axes_log->SetYTitle( "APD Current [pA]" );
  h2_axes_log->Draw();

  label_settings->Draw("same");

  label->Draw("same");
  gPad->RedrawAxis();


  TF1* f1_lineLog = new TF1( "lineScan_log", "[0] + [1]*x", 0., xMax_log );
  f1_lineLog->SetParameter( 0, f1_line->GetParameter(0) );
  f1_lineLog->SetParameter( 1, f1_line->GetParameter(1) );
  f1_lineLog->SetLineWidth(2);
  f1_lineLog->SetLineColor(46);
  f1_lineLog->SetLineStyle(2);
  f1_lineLog->Draw("L same");

  gr_iapd_vs_igun ->Draw("P same");
  gr_iapd_vs_igun2->Draw("P same");

  fitResults->Draw("same");

  c1->SaveAs( Form("%s/iapd_vs_igun_log.pdf", scans[0]->outdir().c_str()) );
  c1->SaveAs( Form("%s/iapd_vs_igun_log.eps", scans[0]->outdir().c_str()) );


  
  return 0;

}

