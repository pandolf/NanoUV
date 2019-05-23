#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "../../CommonTools/interface/NanoUVCommon.h"



//struct NanoUVIVScan {
//
//  float eGun;
//  float iGun;
//
//  std::vector< std::pair<float,float> > iv;
//
//};



std::pair<float,float> getGunEnergyCurrent( std::string word );
std::vector<TGraphErrors*> getScansFromFile( const std::string& fileName );
float getCurrentAtRefVolt( TGraphErrors* scan, float ref );
TGraphErrors* getGain( TGraphErrors* scan, TGraphErrors* darkScan, float ref=40. );




int main() {

  NanoUVCommon::setStyle();

  std::vector<int> colors = NanoUVCommon::colors();

  std::vector<TGraphErrors*> scans = getScansFromFile("data/2019_05_17/iv_scan.txt");


  std::vector<TGraphErrors*> goodScans;
  goodScans.push_back( scans[0] );
  goodScans.push_back( scans[2] );
  goodScans.push_back( scans[4] );
  goodScans.push_back( scans[7] );

  std::vector<TGraphErrors*> darkScans = getScansFromFile("data/2019_05_17/iv_scan_dark.txt");
  TGraphErrors* darkScan = darkScans[0];

  TCanvas* c0 = new TCanvas( "c0", "", 600, 600 );
  c0->SetLogy();
  c0->cd();

  TH2D* h2_axes0 = new TH2D( "axes0", "", 10, 10., 420., 10, 0.01, 60. );
  h2_axes0->SetXTitle( "APD Voltage [V]" );
  h2_axes0->SetYTitle( "APD Current [#muA]" );
  h2_axes0->Draw();


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->SetLogy();
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 10., 420., 10, 0.5, 1000. );
  h2_axes->SetXTitle( "APD Voltage [V]" );
  h2_axes->SetYTitle( "Gain" );
  h2_axes->Draw();


  TCanvas* c1_rel = new TCanvas( "c1_rel", "", 600, 600 );
  c1_rel->SetLogy();
  c1_rel->cd();

  TH2D* h2_axes_rel = new TH2D( "axes_rel", "", 10, 10., 420., 10, 0.5, 1000. );
  h2_axes_rel->SetXTitle( "APD Voltage [V]" );
  h2_axes_rel->SetYTitle( "Gain (Normalized to 500 eV / 38 nA)" );
  h2_axes_rel->Draw();


  TLegend* legend = new TLegend( 0.2, 0.51, 0.55, 0.76 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  
  for( unsigned i=0; i<goodScans.size(); ++i ) {

    std::pair<float,float> gun = getGunEnergyCurrent( goodScans[i]->GetTitle() );

    goodScans[i]->SetMarkerColor( colors[i] );
    goodScans[i]->SetLineColor( colors[i] );
    goodScans[i]->SetMarkerStyle( 20+i );
    goodScans[i]->SetMarkerSize( 1.3 );
    
    c0->cd();
    goodScans[i]->Draw("P same");


    TGraphErrors* gain = getGain( goodScans[i], darkScan );  
    gain->SetMarkerColor( colors[i] );
    gain->SetLineColor( colors[i] );
    gain->SetMarkerStyle( 20+i );
    gain->SetMarkerSize( 1.3 );

    c1->cd();
    gain->Draw("P same");


    c1_rel->cd();
   
    TGraphErrors* gain_rel = new TGraphErrors(0);
    for( unsigned iPoint=0; iPoint<gain->GetN(); ++iPoint ) {
      double x,y;
      gain->GetPoint( iPoint, x, y );
      gain_rel->SetPoint ( iPoint, x, y*(500./gun.first)*(38./gun.second) );
      gain_rel->SetPointError( iPoint, gain->GetErrorX(iPoint), gain->GetErrorY(iPoint) );
    }
    gain_rel->SetMarkerColor( colors[i] );
    gain_rel->SetLineColor( colors[i] );
    gain_rel->SetMarkerStyle( 20+i );
    gain_rel->SetMarkerSize( 1.3 );
    gain_rel->Draw("P same");

    legend->AddEntry( gain, Form("E = %.0f eV (I = %.1f nA)", gun.first, gun.second ), "P" );

  }

  TPaveText* label = NanoUVCommon::getNanoUVLabel(2);

  c0->cd();

  legend->Draw("same");
  gPad->RedrawAxis();
  label->Draw("same");

  c0->SaveAs( "iv.pdf" );

  c1->cd();

  legend->Draw("same");
  gPad->RedrawAxis();
  label->Draw("same");

  c1->SaveAs( "gain.pdf" );

  c1_rel->cd();

  legend->Draw("same");
  gPad->RedrawAxis();
  label->Draw("same");

  c1_rel->SaveAs( "gain_rel.pdf" );



  std::vector<TGraphErrors*> scans_2 = getScansFromFile("data/2019_05_17/iv_scan_2.txt");

  TCanvas* c1_2 = new TCanvas( "c1_2", "", 600, 600 );
  c1_2->SetLogy();
  c1_2->cd();

  TH2D* h2_axes_2 = new TH2D( "axes_2", "", 10, 10., 420., 10, 0.5, 1000. );
  h2_axes_2->SetXTitle( "APD Voltage [V]" );
  h2_axes_2->SetYTitle( "Gain" );
  h2_axes_2->Draw();

  TLegend* legend_2 = new TLegend( 0.2, 0.51, 0.55, 0.76 );
  legend_2->SetFillColor(0);
  legend_2->SetTextSize(0.035);
  
  for( unsigned i=0; i<scans_2.size(); ++i ) {

    std::pair<float,float> gun = getGunEnergyCurrent( scans_2[i]->GetTitle() );

    TGraphErrors* gain = getGain( scans_2[i], darkScan );  
    gain->SetMarkerColor( colors[i] );
    gain->SetLineColor( colors[i] );
    gain->SetMarkerStyle( 20+i );
    gain->SetMarkerSize( 1.3 );

    gain->Draw("P same");

    legend->AddEntry( gain, Form("E = %.0f eV (I = %.1f nA)", gun.first, gun.second ), "P" );

  }

  legend_2->Draw("same");
  gPad->RedrawAxis();
  label->Draw("same");

  c1_2->SaveAs( "gain_2.pdf" );



  return 0;

}



std::vector<TGraphErrors*> getScansFromFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  std::string line;

  std::vector<TGraphErrors*> theScans;

  if( ifs.good() ) {

    std::cout << "-> Starting to parse file: " << fileName << std::endl;

    while( getline(ifs,line) ) {

      std::string delimiter = "\t";
      size_t pos = 0;
      std::vector<std::string> words;
      std::string word;
      while ((pos = line.find(delimiter)) != std::string::npos) {
        word = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        words.push_back(word);
      }
      words.push_back(line); // last bit

      if( theScans.size()==0 ) {
        for( unsigned i=0; i<words.size(); ++i ) {
          TGraphErrors* newScan = new TGraphErrors(0);
          newScan->SetName( Form("scan_%d", i) );
          theScans.push_back(newScan);
        }
      }

      if( words[0]=="gun:" ) {

        for( unsigned i=1; i<words.size(); ++i ) { // first column is common to all
     
          theScans[i-1]->SetTitle( words[i].c_str() );
     
        }

      } // if "gun:"

      else {

        float x = atof(words[0].c_str());

        for( unsigned i=1; i<words.size(); ++i ) { // first column is common to all

          float y = atof(words[i].c_str());
          int iPoint = theScans[i-1]->GetN();
          theScans[i-1]->SetPoint( iPoint, x, y );
          float erry = 0.;
          if( (y==7.) || (y==8.) || (y==9.) || (y==10.) || (y==11.) ) erry = 3.;
          else if( y<=9.1 ) erry = 0.005;
          else erry = 0.5;
          theScans[i-1]->SetPointError( iPoint, 1., erry );

        }

      } // else

    } // while get line

  } // if file good

  return theScans;

}




std::pair<float,float> getGunEnergyCurrent( std::string word ) {

  std::string delimiter = ",";
  size_t pos = 0;
  std::vector<std::string> newwords;
  std::string newword;
  while ((pos = word.find(delimiter)) != std::string::npos) {
    newword = word.substr(0, pos);
    word.erase(0, pos + delimiter.length());
    newwords.push_back(newword);
  }
  newwords.push_back(word);

  std::pair<float,float> energy_current;
  energy_current.first = atof(newwords[0].c_str());
  energy_current.second = (newwords.size()>1) ? atof(newwords[1].c_str()) : -1;

  return energy_current;

}



TGraphErrors* getGain( TGraphErrors* scan, TGraphErrors* darkScan, float ref ) {

  TGraphErrors* gr_gain = new TGraphErrors(0);
  gr_gain->SetName( Form("gain_%s", scan->GetName() ));

  // x = volt (in V)
  // y = i (current, in microA)

  float i_open_ref = getCurrentAtRefVolt( scan    , ref );
  float i_dark_ref = getCurrentAtRefVolt( darkScan, ref );

  for( unsigned i=0; i<scan->GetN(); ++i ) {

    double volt, i_open;
    scan->GetPoint( i, volt, i_open );
    double volt_err = 1.;
    double i_err = scan->GetErrorY( i );
    float i_dark = getCurrentAtRefVolt( darkScan, volt );

    float gain = ( i_open - i_dark ) / ( i_open_ref - i_dark_ref );
    float gain_err = i_err/i_open_ref; // consider error on i_ref zero
    //float gain_err = sqrt( i_err*i_err/(i_open_ref*i_open_ref) + i_open*i_open*0.01*0.01/(i_open_ref*i_open_ref*i_open_ref*i_open_ref) );

    float r = 10000.;  // 10 kOhm
    float volt_corr = volt - r*i_open*1E-6; // put current in Amperes

    //gr_gain->SetPoint( i, volt, gain );
    gr_gain->SetPoint( i, volt_corr, gain );
    gr_gain->SetPointError( i, volt_err, gain_err );

  } // for iPoints

  return gr_gain;

}



float getCurrentAtRefVolt( TGraphErrors* scan, float ref ) {

  float refcurr = 0.;

  for( unsigned i=0; i<scan->GetN(); ++i ) {

    double volt, curr;
    scan->GetPoint( i, volt, curr );
    if( volt==ref ) {
      refcurr = curr;
      break;
    }

  } // for

  return refcurr;

}
