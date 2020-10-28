#include "NanoUVCommon.h"

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"




int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc<2 ) {

    std::cout << "USAGE: ./drawLED [ledType= SP5605_SN030, SP5601_SN134, SP5601_SN243]" << std::endl;
    exit(1);

  }

  float adc2kev = 8.12501; // from drawFe55
  float energy;

  std::string ledType(argv[1]);

  std::vector< std::pair<float, float> > ag; 


  if( ledType=="SP5605" ) ledType = "SP5605_SN030";

  if( ledType=="SP5605_SN030" ) {

    ag.push_back( std::pair<float,float>(10., 4.) );
    ag.push_back( std::pair<float,float>( 9., 4.) );
    ag.push_back( std::pair<float,float>( 8., 4.) );
    ag.push_back( std::pair<float,float>( 7., 4.) );
    ag.push_back( std::pair<float,float>( 6., 4.) );
    //ag.push_back( std::pair<float,float>( 6., 12.) );
    ag.push_back( std::pair<float,float>( 5.,12.) );
    ag.push_back( std::pair<float,float>( 4.,40.) );

    energy = 0.005;

  } else if( ledType=="SP5601_SN134" ) {

    ag.push_back( std::pair<float,float>(10., 12.) );
    ag.push_back( std::pair<float,float>( 9., 40.) );
    ag.push_back( std::pair<float,float>( 8.,120.) );
    ag.push_back( std::pair<float,float>( 7.,120.) );
 
    energy = 0.0031;

  } else if( ledType=="SP5601_SN243" ) {

    ag.push_back( std::pair<float,float>(10.,  12.) );
    ag.push_back( std::pair<float,float>( 9.,  12.) );
    //ag.push_back( std::pair<float,float>( 9.,  40.) );
    ag.push_back( std::pair<float,float>( 8.,  40.) );
    //ag.push_back( std::pair<float,float>( 8., 120.) );
    ag.push_back( std::pair<float,float>( 7., 120.) );

    energy = 0.0031;

  } else {

    std::cout << "Unkown LED type. Exiting." << std::endl;
    exit(1);

  }

  std::string outdir( Form("plots/%s", ledType.c_str()) );
  system( Form("mkdir -p %s", outdir.c_str()) );

  TGraphErrors* gr_kev_vs_ampl      = new TGraphErrors(0);
  TGraphErrors* gr_nPhotons_vs_ampl = new TGraphErrors(0);


  for( unsigned i=0; i<ag.size(); ++i ) {

    float a = ag[i].first;
    float g = ag[i].second;

    TChain* chain = new TChain("tree");

    //TFile* file = TFile::Open( Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020.root", ledType.c_str(), a, g) );

    //std::cout <<  Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020.root/tree", ledType.c_str(), a, g) << std::endl;
    //chain->Add( Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020.root/tree"  , ledType.c_str(), a, g) );
    //chain->Add( Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020_2.root/tree", ledType.c_str(), a, g) );

    std::cout <<  Form("data/LED_%s/Run_A%.0f_G%.0f_Data_7_23_2020_Ascii.root/tree", ledType.c_str(), a, g) << std::endl;
    chain->Add( Form("data/LED_%s/Run_A%.0f_G%.0f_Data_7_23_2020_Ascii.root/tree"  , ledType.c_str(), a, g) );
    chain->Add( Form("data/LED_%s/Run_A%.0f_G%.0f_Data_7_23_2020_2_Ascii.root/tree", ledType.c_str(), a, g) );



//  TTree* tree = (TTree*)file->Get("tree");

//  if( tree!=0 ) {

//    chain->Add( Form("%s/tree", file->GetName()) );
//    std::cout << "-> Added: " << file->GetName() << "/tree" << std::endl;

//    TFile* file_2 = TFile::Open( Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020_2.root", ledType.c_str(), a, g) );

//    if( !file_2->IsZombie() ) {

//      TTree* tree_2 = (TTree*)file_2->Get("tree");

//      if( tree_2!=0 ) {

//        chain->Add( Form("%s/tree", file_2->GetName()) );
//        std::cout << "-> Added: " << file_2->GetName() << "/tree" << std::endl;

//        TFile* file_3 = TFile::Open( Form("data/LED_%s/Run_A%.0f_G%.0f_Measurements_Only_7_23_2020_3.root", ledType.c_str(), a, g) );
//  
//        if( !file_3->IsZombie() ) {
//  
//          TTree* tree_3 = (TTree*)file_3->Get("tree");
//  
//          if( tree_3!=0 ) {
//  
//            chain->Add( Form("%s/tree", file_3->GetName()) );
//            std::cout << "-> Added: " << file_3->GetName() << "/tree" << std::endl;

//          } // if tree_3

//        } // if file_3

//      } // if tree_2

//    } // if file_2

//  } // if tree

    
    TH1D* h1_vamp = new TH1D( Form("vamp_%s_a%.0f_g%.0f", ledType.c_str(), a, g), "", 2000, 0., 2.5 );
    chain->Project( h1_vamp->GetName(), "vamp", "vamp>0. && delta>-0.05" );

    TF1* f1_gaus0 = new TF1( Form("gaus0_%s_a%.0f_g%.0f", ledType.c_str(), a, g), "gaus", h1_vamp->GetMean()-0.5*h1_vamp->GetRMS(), h1_vamp->GetMean()+h1_vamp->GetRMS() );
    h1_vamp->Fit( f1_gaus0, "R0" );

    TF1* f1_gaus = new TF1( Form("gaus_%s_a%.0f_g%.0f", ledType.c_str(), a, g), "gaus", f1_gaus0->GetParameter(1)-1.5*f1_gaus0->GetParameter(2), f1_gaus0->GetParameter(1)+2.5*f1_gaus0->GetParameter(2) );
    h1_vamp->Fit( f1_gaus, "R" );

    TCanvas* c1 = new TCanvas( Form("c1_%s_a%.0f_g%.0f", ledType.c_str(), a, g), "", 600, 600 );
    c1->cd();
 
    TH2D* h2_axes = new TH2D( Form("axes_%s_a%.0f_g%.0f", ledType.c_str(), a, g), "", 10, f1_gaus->GetParameter(1)-7.*f1_gaus->GetParameter(2), f1_gaus->GetParameter(1)+7.*f1_gaus->GetParameter(2), 10, 0., 1.2*h1_vamp->GetMaximum() );
    h2_axes->Draw();
    h1_vamp->Draw("same");

    c1->SaveAs( Form( "%s/vamp_a%.0f_g%.0f.pdf", outdir.c_str(), a, g) );

    float k = (adc2kev)*40./g; // Fe55 was taken at gain = 40
    gr_kev_vs_ampl->SetPoint     ( i,  a, f1_gaus->GetParameter(1)*k );
    gr_kev_vs_ampl->SetPointError( i, 0., f1_gaus->GetParameter(2)*k );

    gr_nPhotons_vs_ampl->SetPoint     ( i,  a, f1_gaus->GetParameter(1)*k/energy );
    gr_nPhotons_vs_ampl->SetPointError( i, 0., f1_gaus->GetParameter(2)*k/energy );

  } // for ag



  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();

  TH2D* h2_axes_2 = new TH2D( "axes_2", "", 10, 3.5, 10.5, 10, 0., 19. );
  h2_axes_2->SetYTitle( "SDD amplitude [keV]" );
  h2_axes_2->SetXTitle( "LED amplitude" );
  h2_axes_2->Draw();

  gr_kev_vs_ampl->SetMarkerStyle(20);
  gr_kev_vs_ampl->SetMarkerColor(kGray+3);
  gr_kev_vs_ampl->SetLineColor(kGray+3);
  gr_kev_vs_ampl->SetMarkerSize(1.5);

  gr_kev_vs_ampl->Draw("P same");

  c2->SaveAs( Form("%s/kev_vs_ampl.pdf", outdir.c_str()) );

  c2->Clear();

  TH2D* h2_axes_3 = new TH2D( "axes_3", "", 10, 3.5, 10.5, 10, 0., 40000. );
  h2_axes_3->SetYTitle( "Number of photons" );
  h2_axes_3->SetXTitle( "LED amplitude" );
  h2_axes_3->Draw();

  gr_nPhotons_vs_ampl->SetMarkerStyle(20);
  gr_nPhotons_vs_ampl->SetMarkerColor(kGray+3);
  gr_nPhotons_vs_ampl->SetLineColor(kGray+3);
  gr_nPhotons_vs_ampl->SetMarkerSize(1.5);

  gr_nPhotons_vs_ampl->Draw("P same");

  c2->SaveAs( Form("%s/nPhotons_vs_ampl.pdf", outdir.c_str()) );

  return 0;

}
