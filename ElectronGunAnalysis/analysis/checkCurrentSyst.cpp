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




float getSyst( float iNominal, GunScan* scan, const std::string& method );
//TGraphErrors* subtractMin( TGraphErrors* graph );


int main( int argc, char* argv[] ) {


  float APDhv;
  float gunEnergy;

  if( argc==1 ) {

    std::cout << "USAGE: ./checkCurrentSyst [gunEnergy] [APDhv=350 V]" << std::endl;
    exit(1);

  } else {

    gunEnergy = atof(argv[1]);
    if( argc>2 )
      APDhv = atof(argv[2]);
    else
      APDhv = 350.;

  }


  NanoUVCommon::setStyle();

  std::vector< GunScan* > scans;
  if( gunEnergy==90. )
    scans  = GunScan::loadScans( "data/2020_07/scans.txt", gunEnergy, APDhv );
  else
    scans  = GunScan::loadScans( "data/2020_02/scans.txt", gunEnergy, APDhv );

  TGraphErrors* gr_syst     = new TGraphErrors(0);
  TGraphErrors* gr_systCorr = new TGraphErrors(0);


  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

    if( scans[iScan]->APDhv() != APDhv ) continue;

    float iapd_nominal = scans[iScan]->getCurrentFromScan();

    float syst     = getSyst( iapd_nominal    , scans[iScan], "max" );
    float systCorr = getSyst( iapd_nominal*1.1, scans[iScan], "max" );

    gr_syst    ->SetPoint( gr_syst    ->GetN(), scans[iScan]->gunCurrent(), syst     );
    gr_systCorr->SetPoint( gr_systCorr->GetN(), scans[iScan]->gunCurrent(), systCorr );

  } // for scans


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->SetLogx();
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.05, 80., 10, 0., 0.4 );
  h2_axes->SetXTitle( "I_{gun} [pA]" );
  h2_axes->SetYTitle( "(Alternative - Nominal) / Nominal" );
  h2_axes->Draw();

  TPaveText* label_settings = new TPaveText( 0.72, 0.65, 0.92, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  //label_settings->SetTextColor( 46 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form("E_{e} = %.0f eV", gunEnergy) );
  label_settings->AddText( Form("V_{apd} = %.0f V", APDhv) );
  label_settings->SetTextAlign(11);
  label_settings->Draw("same");

  TLegend* legend = new TLegend( 0.28, 0.75, 0.65, 0.88);
  legend->SetTextSize(0.04);
  legend->SetFillColor(0);
  legend->AddEntry( gr_syst    , "Raw", "P" );
  legend->AddEntry( gr_systCorr, "Nominal #times 1.1", "P" );
  legend->Draw("same");

  gr_syst->SetMarkerStyle(20);
  gr_syst->SetMarkerSize(1.6);
  gr_syst->SetMarkerColor(kGray+3);
  gr_syst->SetLineColor(kGray+3);
  gr_syst->Draw("Psame");

  gr_systCorr->SetMarkerStyle(24);
  gr_systCorr->SetMarkerSize(1.6);
  gr_systCorr->SetMarkerColor(kGray+3);
  gr_systCorr->SetLineColor(kGray+3);
  gr_systCorr->Draw("Psame");


  NanoUVCommon::addNanoUVLabel( c1, 1 );

  label_settings->Draw("same");

  c1->SaveAs( Form("systCurrent_E%.0feV_APDhv%.0fV.pdf", gunEnergy, APDhv) );

  delete c1;
  delete h2_axes;


  std::string outfileName(Form("systCurrent_E%.0feV_APDhv%.0fV.root", gunEnergy, APDhv));
  TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
  outfile->cd();
  gr_systCorr->SetName("syst");
  gr_systCorr->Write();
  outfile->Close();

  std::cout << "-> Saved syst in file: " << outfileName << std::endl;

  return 0;

}    


float getSyst( float iNominal, GunScan* scan, const std::string& method ) {

  scan->set_currentMethod(method);
  float iapd_syst = scan->getCurrentFromScan();

  //std::cout << std::endl;
  //std::cout << "---- Gun current: " << scans[iScan]->gunCurrent() << std::endl;
  //std::cout << "Nominal I(apd): " << iapd_nominal << std::endl;
  //std::cout << "Syst   I(apd): " << iapd_syst << std::endl;

  //return (iNominal-iapd_syst)/iNominal;
  return fabs(iapd_syst-iNominal)/iNominal;

}



//TGraphErrors* subtractMin( TGraphErrors* graph ) {
//
//  TGraphErrors* newGraph = new TGraphErrors(0);
//
//  float yMin = 10.;
//
//  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {
//
//    double x, y;
//    graph->GetPoint( iPoint, x, y );
//
//    if( y<yMin ) yMin = y;
//
//  }
//
//  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {
//
//    double x, y;
//    graph->GetPoint( iPoint, x, y );
//
//    newGraph->SetPoint( newGraph->GetN(), x, y-yMin );
//
//  }
//
//  return newGraph;
//
//}
