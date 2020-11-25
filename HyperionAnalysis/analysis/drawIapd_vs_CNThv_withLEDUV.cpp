#include <iostream>

#include "interface/HyperionData.h"
#include "interface/NanoUVCommon.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TFile.h"
#include "TF1.h"



TGraphErrors* getDiff_vs_CNThv( HyperionData hd );
void setCoordinates( HyperionData hd, double& xMin, double& yMin, double& yMax );
void drawGraphs( HyperionData hd, double xMin, double xMax, double yMin, double yMax );
std::string getSuffix( HyperionData hd );



int main( int argc, char* argv[] ) {

  NanoUVCommon::setStyle();

  if( argc==1 ) {
    std::cout << "-> USAGE: ./drawIapd_vs_CNThv_withLEDUV [fileName]" << std::endl;
    exit(1);
  }


  std::string fileName( argv[1] );


  HyperionData hd( fileName );
  //HyperionData hd("data/HyperionAPD/2020_10_29/Iapd_vs_CNThv_withLEDUV_L33_CNT50um_fusedITO.dat");
  //HyperionData hd("data/HyperionAPD/2020_11_17/Iapd_vs_CNThv_withLEDUV_L33_CNT50um_fusedITO_A.dat");
  //HyperionData hd("data/HyperionAPD/2020_11_25/Iapd_vs_CNThv_withLEDUV_L33_CNT50um_fusedITO_B.dat");

  double xMax = 99.;
  double xMin, yMin, yMax;
  setCoordinates( hd, xMin, yMin, yMax );

  drawGraphs( hd, xMin, xMax, yMin, yMax );

  TGraphErrors* gr = getDiff_vs_CNThv( hd );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 1. );
  h2_axes->SetXTitle( "#DeltaV [V]" );
  h2_axes->SetYTitle( "I_{apd}(on) - I_{apd}(off) [nA]");
  h2_axes->Draw();

  gr->SetMarkerColor( 46 );
  gr->SetLineColor( 46 );

  gr->Draw("p same");

  //TLegend* legend = new TLegend( 0.6, 0.73, 0.9, 0.87 );
  //legend->SetTextSize(0.035);
  //legend->SetFillColor(0);
  //legend->AddEntry( gr_air, gr_air->GetName(), "P" );
  //legend->AddEntry( gr, gr->GetName(), "P" );
  ////legend->Draw("psame");

  TPaveText* label_p = new TPaveText( 0.2, 0.8, 0.55, 0.85, "brNDC" );
  label_p->SetFillColor(0);
  label_p->SetTextSize(0.035);
  label_p->AddText( Form("p = %s mbar", NanoUVCommon::scientific(hd.p()).c_str()) );
  label_p->Draw("Same");

  TPaveText* label_L = new TPaveText( 0.2, 0.75, 0.55, 0.8, "brNDC" );
  label_L->SetFillColor(0);
  label_L->SetTextSize(0.035);
  label_L->AddText( Form("L = %.1fmm", hd.L()) );
  label_L->Draw("Same");


  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/Iapd_vs_CNThv_withLEDUV_%s_diff.pdf", getSuffix(hd).c_str()) );
  
  return 0;

}




TGraphErrors* getDiff_vs_CNThv( HyperionData hd ) {


  TGraphErrors* gr_ledON  = hd.getGraphFromColumns( "gr_ledON" , 3, 4, 1 ); 
  TGraphErrors* gr_ledOFF = hd.getGraphFromColumns( "gr_ledOFF", 1, 2, 1 ); 

  TGraphErrors* gr_diff = new TGraphErrors(0);

  for( unsigned iPoint=0; iPoint<gr_ledON->GetN(); ++iPoint ) {

    double xON, yON;
    gr_ledON->GetPoint( iPoint, xON, yON );
    double err_yON = gr_ledON->GetErrorY( iPoint );

    double xOFF, yOFF;
    gr_ledOFF->GetPoint( iPoint, xOFF, yOFF );
    double err_yOFF = gr_ledOFF->GetErrorY( iPoint );

    gr_diff->SetPoint     ( iPoint, xON, yON-yOFF );
    gr_diff->SetPointError( iPoint, 1., sqrt( err_yON*err_yON + err_yOFF*err_yOFF ) );

  }
  
  gr_diff->SetMarkerStyle(20);
  gr_diff->SetMarkerSize(1.5);
  gr_diff->SetMarkerColor(kGray+3);
  gr_diff->SetLineColor(kGray+3);

  return gr_diff;

}


void drawGraphs( HyperionData hd, double xMin, double xMax, double yMin, double yMax ) {

  TGraphErrors* gr_ledON  = hd.getGraphFromColumns( "gr_ledON" , 3, 4, 1 ); 
  TGraphErrors* gr_ledOFF = hd.getGraphFromColumns( "gr_ledOFF", 1, 2, 1 ); 

  gr_ledON->SetMarkerStyle(20);
  gr_ledON->SetMarkerSize(1.5);
  gr_ledON->SetMarkerColor(46);
  gr_ledON->SetLineColor(46);

  gr_ledOFF->SetMarkerStyle(20);
  gr_ledOFF->SetMarkerSize(1.5);
  gr_ledOFF->SetMarkerColor(38);
  gr_ledOFF->SetLineColor(38);

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes->SetXTitle( "#DeltaV [V]" );
  h2_axes->SetYTitle( "I_{apd} [nA]");
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.6, 0.73, 0.9, 0.87 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( gr_ledOFF, "UV LED: OFF", "P" );
  legend->AddEntry( gr_ledON , "UV LED: ON ", "P" );
  legend->Draw("psame");

  std::string pressureText(Form( "p = %s mbar", NanoUVCommon::scientific(hd.p()).c_str() ));

  TPaveText* label_p = new TPaveText( 0.2, 0.8, 0.55, 0.85, "brNDC" );
  label_p->SetFillColor(0);
  label_p->SetTextSize(0.035);
  label_p->AddText( pressureText.c_str() );
  label_p->Draw("same");

  TPaveText* label_L = new TPaveText( 0.2, 0.75, 0.55, 0.8, "brNDC" );
  label_L->SetFillColor(0);
  label_L->SetTextSize(0.035);
  label_L->AddText( Form("L = %.1fmm", hd.L()) );
  label_L->Draw("Same");
  gPad->RedrawAxis();

  gr_ledOFF->Draw("Psame");
  gr_ledON ->Draw("Psame");

  gPad->RedrawAxis();

  c1->SaveAs( Form("plots/Iapd_vs_CNThv_withLEDUV_%s.pdf", getSuffix(hd).c_str()) );


  delete c1;
  delete h2_axes;

}



std::string getSuffix( HyperionData hd ) {

  std::string suffix = (hd.p()>100.) ? "_air" : "";

  return std::string( Form("L%.0f_%s%s", hd.L(), hd.cnt_sample().c_str(), suffix.c_str()) );

}



void setCoordinates( HyperionData hd, double& xMin, double &yMin, double &yMax ) {

  TGraphErrors* gr_ledON  = hd.getGraphFromColumns( "gr_ledON" , 3, 4, 1 ); 
  TGraphErrors* gr_ledOFF = hd.getGraphFromColumns( "gr_ledOFF", 1, 2, 1 ); 

  double x0, y0;
  gr_ledOFF->GetPoint( 0, x0, y0 );

  double x1, y1;
  gr_ledON->GetPoint( gr_ledON->GetN()-1, x1, y1 );

  xMin = x1 - 99.;
  yMin = y0*0.99;
  yMax = y1*1.02;

}

