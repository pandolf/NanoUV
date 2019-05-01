#include <iostream>
#include <fstream>

#include "NanoUVCommon.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TStyle.h"




void fillGraphs( TGraph* gr_iv, TGraph* gr_gain, float volt, float idark_10V, float iopen_10V, float idark, float iopen );
void drawGraphs( const std::string& name, const std::string& yTitle, TGraph* gr0, TGraph* gr1, TGraph* gr2, TGraph* gr3 );
float findV( TGraph* gr, float gain=50. );
void setStyle();




int main() {

  setStyle();

  std::string fileName = "data_APD_IV_2019_03_22.txt";

  std::ifstream ifs(fileName.c_str());

  float volt, i0dark, i0open, i1dark, i1open, i2dark, i2open, i3dark, i3open;

  TGraph* gr_iv0   = new TGraph(0);
  TGraph* gr_gain0 = new TGraph(0);
  TGraph* gr_iv1   = new TGraph(0);
  TGraph* gr_gain1 = new TGraph(0);
  TGraph* gr_iv2   = new TGraph(0);
  TGraph* gr_gain2 = new TGraph(0);
  TGraph* gr_iv3   = new TGraph(0);
  TGraph* gr_gain3 = new TGraph(0);

  float i0dark_10V(0.), i1dark_10V(0.), i2dark_10V(0.), i3dark_10V(0.);
  float i0open_10V(0.), i1open_10V(0.), i2open_10V(0.), i3open_10V(0.);


  while( !ifs.eof() ) {

    ifs >> volt >> i0dark >> i0open >> i1dark >> i1open >> i2dark >> i2open >> i3dark >> i3open;

    if( volt==10. ) {

      i0dark_10V = i0dark;
      i1dark_10V = i1dark;
      i2dark_10V = i2dark;
      i3dark_10V = i3dark;

      i0open_10V = i0open;
      i1open_10V = i1open;
      i2open_10V = i2open;
      i3open_10V = i3open;

    }

    std::cout << "V = " << volt << std::endl;

    fillGraphs( gr_iv0, gr_gain0, volt, i0dark_10V, i0open_10V, i0dark, i0open );
    fillGraphs( gr_iv1, gr_gain1, volt, i1dark_10V, i1open_10V, i1dark, i1open );
    fillGraphs( gr_iv2, gr_gain2, volt, i2dark_10V, i2open_10V, i2dark, i2open );
    fillGraphs( gr_iv3, gr_gain3, volt, i3dark_10V, i3open_10V, i3dark, i3open );

  } // while good

  drawGraphs( "iv"  , "I [#muA]", gr_iv0  , gr_iv1  , gr_iv2  , gr_iv3   );
  drawGraphs( "gain", "Gain"     , gr_gain0, gr_gain1, gr_gain2, gr_gain3 );


  return 0;

}


void fillGraphs( TGraph* gr_iv, TGraph* gr_gain, float volt, float idark_10V, float iopen_10V, float idark, float iopen ) {

  if( iopen>0. ) {
    gr_iv  ->SetPoint( gr_iv  ->GetN(), volt, iopen );
    float gain = (iopen-idark)/(iopen_10V-idark_10V);
    gr_gain->SetPoint( gr_gain->GetN(), volt, gain );
    std::cout << "    gain: " << gain << std::endl;
  }

}


void drawGraphs( const std::string& name, const std::string& yTitle, TGraph* gr0, TGraph* gr1, TGraph* gr2, TGraph* gr3 ) {

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  c1->SetLogy();

  float yMin = (name=="iv") ? 1.   : 0.3;
  float yMax = (name=="iv") ? 500. : 250.;

  TH2D* h2_axes = new TH2D( Form("axes_%s", name.c_str()), "", 10, 10., 460., 10, yMin, yMax );  
  h2_axes->SetXTitle( "V [V]" );
  h2_axes->SetYTitle( yTitle.c_str() );
  h2_axes->GetYaxis()->SetNoExponent();
  h2_axes->GetYaxis()->SetMoreLogLabels();
  h2_axes->Draw();

  gr0->SetMarkerStyle(20);
  gr0->SetMarkerSize(1.3);
  gr0->SetMarkerColor(46);

  gr1->SetMarkerStyle(21);
  gr1->SetMarkerSize(1.3);
  gr1->SetMarkerColor(38);

  gr2->SetMarkerStyle(22);
  gr2->SetMarkerSize(1.3);
  gr2->SetMarkerColor(29);

  gr3->SetMarkerStyle(23);
  gr3->SetMarkerSize(1.3);
  gr3->SetMarkerColor(kGray+2);

  gr1->Draw("P same");
  gr0->Draw("P same");
  gr2->Draw("P same");
  gr3->Draw("P same");

  float v50_0 = (name=="gain") ? findV( gr0, 50. ) : -1.;
  float v50_1 = (name=="gain") ? findV( gr1, 50. ) : -1.;
  float v50_2 = (name=="gain") ? findV( gr2, 50. ) : -1.;
  float v50_3 = (name=="gain") ? findV( gr3, 50. ) : -1.;

  std::string suffix_0 = (v50_0>0.) ? std::string(Form(" (V_{50} = %.1f V)", v50_0)) : "";
  std::string suffix_1 = (v50_1>0.) ? std::string(Form(" (V_{50} = %.1f V)", v50_1)) : "";
  std::string suffix_2 = (v50_2>0.) ? std::string(Form(" (V_{50} = %.1f V)", v50_2)) : "";
  std::string suffix_3 = (v50_3>0.) ? std::string(Form(" (V_{50} = %.1f V)", v50_3)) : "";


  TLegend* legend = new TLegend( 0.2, 0.67, 0.5, 0.87 );
  legend->SetTextSize( 0.035 );
  legend->SetFillColor( 0 );
  legend->AddEntry( gr0, Form("APD 0%s", suffix_0.c_str()), "P" );
  legend->AddEntry( gr1, Form("APD 1%s", suffix_1.c_str()), "P" );
  legend->AddEntry( gr2, Form("APD 2%s", suffix_2.c_str()), "P" );
  legend->AddEntry( gr3, Form("APD 3%s", suffix_3.c_str()), "P" );
  legend->Draw("same");

  NanoUVCommon::addNanoUVLabel( c1 );

  c1->SaveAs( Form( "%s.pdf", name.c_str() ) );

  delete c1;
  delete h2_axes;

}



float findV( TGraph* gr, float gain ) {

  float v50 = -1.;

  for( unsigned iPoint=0; iPoint<gr->GetN()-1; ++iPoint ) {

    double thisx, thisy;
    gr->GetPoint( iPoint, thisx, thisy );
  
    double nextx, nexty;
    gr->GetPoint( iPoint+1, nextx, nexty );

    if( thisy<=gain && nexty>gain ) { // linear interpolation

      // f(x) = mx + q
      float m = (nexty-thisy)/(nextx-thisx);
      float q = thisy - m*thisx;

      v50 = (50.-q)/m;
      
      break;

    }

  } // for points

  return v50;

}




void setStyle() {

  // set the TStyle
  TStyle* style = new TStyle("DrawBaseStyle", "");
  style->SetCanvasColor(0);
  style->SetPadColor(0);
  style->SetFrameFillColor(0);
  style->SetStatColor(0);
  style->SetOptStat(0);
  style->SetOptFit(0);
  style->SetTitleFillColor(0);
  style->SetCanvasBorderMode(0);
  style->SetPadBorderMode(0);
  style->SetFrameBorderMode(0);
  style->SetPadBottomMargin(0.12);
  style->SetPadLeftMargin(0.12);
  style->cd();
  // For the canvas:
  style->SetCanvasBorderMode(0);
  style->SetCanvasColor(kWhite);
  style->SetCanvasDefH(600); //Height of canvas
  style->SetCanvasDefW(600); //Width of canvas
  style->SetCanvasDefX(0); //POsition on screen
  style->SetCanvasDefY(0);
  // For the Pad:
  style->SetPadBorderMode(0);
  style->SetPadColor(kWhite);
  style->SetPadGridX(false);
  style->SetPadGridY(false);
  style->SetGridColor(0);
  style->SetGridStyle(3);
  style->SetGridWidth(1);
  // For the frame:
  style->SetFrameBorderMode(0);
  style->SetFrameBorderSize(1);
  style->SetFrameFillColor(0);
  style->SetFrameFillStyle(0);
  style->SetFrameLineColor(1);
  style->SetFrameLineStyle(1);
  style->SetFrameLineWidth(1);
  // Margins:
  style->SetPadTopMargin(0.06);
  style->SetPadBottomMargin(0.14);//0.13);
  style->SetPadLeftMargin(0.16);//0.16);
  style->SetPadRightMargin(0.04);//0.02);
  // For the Global title:
  style->SetOptTitle(0);
  style->SetTitleFont(42);
  style->SetTitleColor(1);
  style->SetTitleTextColor(1);
  style->SetTitleFillColor(10);
  style->SetTitleFontSize(0.05);
  // For the axis titles:
  style->SetTitleColor(1, "XYZ");
  style->SetTitleFont(42, "XYZ");
  style->SetTitleSize(0.05, "XYZ");
  style->SetTitleXOffset(1.15);//0.9);
  style->SetTitleYOffset(1.5); // => 1.15 if exponents
  // For the axis labels:
  style->SetLabelColor(1, "XYZ");
  style->SetLabelFont(42, "XYZ");
  style->SetLabelOffset(0.007, "XYZ");
  style->SetLabelSize(0.045, "XYZ");
  // For the axis:
  style->SetAxisColor(1, "XYZ");
  style->SetStripDecimals(kTRUE);
  style->SetTickLength(0.03, "XYZ");
  style->SetNdivisions(510, "XYZ");
  style->SetPadTickX(1); // To get tick marks on the opposite side of the frame
  style->SetPadTickY(1);
  // for histograms:
  style->SetHistLineColor(1);
  // for the pallete
  Double_t stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red  [5] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue [5] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(5, stops, red, green, blue, 100);
  style->SetNumberContours(100);

  style->cd();

}
