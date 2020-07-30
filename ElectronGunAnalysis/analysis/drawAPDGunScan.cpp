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



std::string currentMethod = "average";


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

  std::vector< GunScan* > scans  = GunScan::loadScans( Form("data/%s/scans.txt", data.c_str()), gunEnergy, APDhv );
  std::vector< GunScan* > scans2 = GunScan::loadScans( Form("data/2019_11_28/scans.txt"), gunEnergy, APDhv );

  TGraphErrors* gr_iapd_vs_igun  = new TGraphErrors(0);
  TGraphErrors* gr_iapd_vs_igun2 = new TGraphErrors(0);


  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

    if( scans[iScan]->gunEnergy() != gunEnergy ) continue;
    if( scans[iScan]->APDhv() != APDhv ) continue;
    //if( scans[iScan]->scanName() == "25feb20_08_M_.dat") {
    //  scans[iScan]->set_firstN_fit( 5 );
    //  scans[iScan]->set_lastN_fit( 25 );
    //  scans[iScan]->set_baselineFunc( "pol5" );

    //  }

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

  float xMax = 1.2*GunScan::getXmax(gr_iapd_vs_igun);
  float xMax_log = 5.*xMax;
  float yMax = 1.2*GunScan::getYmax(gr_iapd_vs_igun);
  float yMax_log = 3.*yMax;



  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., yMax );
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

  float xFitMin, xFitMax;
   if( APDhv==350. && gunEnergy==500.) {
    xFitMin = 0.01;
    xFitMax = 25.;
  } else if( APDhv==350. && gunEnergy==900.) {
    xFitMin = 0.01;
    xFitMax = 15.;
  } else if( APDhv==380. && gunEnergy==500.) {
    xFitMin = 0.05;
    xFitMax = 100.;
  } else if( APDhv==380. && gunEnergy==900.) {
    xFitMin = 0.01;
    xFitMax = 55.;
  }else {
    xFitMin = 0.;
    xFitMax = 15.;
  }
  TF1* f1_line = new TF1( "lineScan", "[0] + [1]*x", xFitMin, xFitMax );
  gr_iapd_vs_igun->Fit( f1_line, "QR0" );

  TPaveText* fitResults = new TPaveText( 0.6, 0.2, 0.9, 0.44, "brNDC" );
  fitResults->SetTextSize( 0.035 );
  fitResults->SetFillColor(0);
  fitResults->SetTextColor(46);
  fitResults->AddText( "I_{apd} = I_{0} + G#timesI_{gun}" );
  fitResults->AddText( Form("I_{0} = %.1f #pm %.1f pA", f1_line->GetParameter(0), f1_line->GetParError(0) ) );
  fitResults->AddText( Form("G = %.1f #pm %.1f", f1_line->GetParameter(1), f1_line->GetParError(1) ) );
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
  if( gunEnergy == 500. ) yMin /=10.;
  if( gunEnergy == 90.  ) yMin /=500.;

  float xMin_log = 0.01;

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, xMin_log, xMax_log, 10, yMin, yMax_log );
  h2_axes_log->SetXTitle( "I_{gun} [pA]" );
  h2_axes_log->SetYTitle( "I_{apd} [pA]" );
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

  std::cout << "xFitMin = " << xFitMin << std::endl;
  std::cout << "xFitMax = " << xFitMax << std::endl;

  TFile* fitResultsFile = TFile::Open( Form("%s/fitResults.root", scans[0]->outdir().c_str()), "RECREATE" );
  fitResultsFile->cd();
  f1_line->Write();
  fitResultsFile->Close();

  std::cout << "-> Saved fit results in: " << fitResultsFile->GetName() << std::endl;
  
  return 0;

}

