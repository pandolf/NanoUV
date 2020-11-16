#include "../interface/NanoUVCommon.h"

#include <iostream>
#include <fstream>
#include <math.h>
 
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TH2D.h"





std::vector<int> NanoUVCommon::colors() {

  std::vector<int> colors;
  
  colors.push_back( 29 );
  colors.push_back( 38 );
  colors.push_back( 46 );
  colors.push_back( 42 );
  //colors.push_back( kGray+1 );
  colors.push_back( kBlack );
  colors.push_back( kGreen );

  return colors;

}


//void NanoUVCommon::addNanoUVLabel( TCanvas* c1, int quadrant ) {
//
//  if( quadrant==4 )
//    NanoUVCommon::addNanoUVLabel( c1, 0.7, 0.2, 0.9, 0.25 );
//  else if( quadrant==3 )
//    NanoUVCommon::addNanoUVLabel( c1, 0.22, 0.2, 0.42, 0.25 );
//  else 
//    std::cout << "[NanoUVCommon::addNanoUVLabel] Quadrant '" << quadrant << "' not implemented yet!" << std::endl;
//
//}


void NanoUVCommon::addNanoUVLabel( TCanvas* c1, int quadrant, const std::string& text ) {

  TPaveText* label = NanoUVCommon::getNanoUVLabel( quadrant, text );

  c1->cd();

  label->Draw();

  gPad->RedrawAxis();

}



void NanoUVCommon::addNanoUVLabel( TCanvas* c1, float xmin, float ymin, float xmax, float ymax, const std::string& text ) {

  TPaveText* label = NanoUVCommon::getNanoUVLabel( xmin, ymin, xmax, ymax, text );

  c1->cd();

  label->Draw();

  gPad->RedrawAxis();

}



TPaveText* NanoUVCommon::getNanoUVLabel( int quadrant, const std::string& text ) {

  if( quadrant==4 )
    return NanoUVCommon::getNanoUVLabel( 0.7 , 0.2, 0.9 , 0.25, text );
  else if( quadrant==2 )
    return NanoUVCommon::getNanoUVLabel( 0.22, 0.83, 0.42, 0.88, text );
  else if( quadrant==3 )
    return NanoUVCommon::getNanoUVLabel( 0.22, 0.2, 0.42, 0.25, text );
  else if( quadrant==1 )
    return NanoUVCommon::getNanoUVLabel( 0.7 , 0.83, 0.9 , 0.88, text );
  else {
    std::cout << "[NanoUVCommon::addNanoUVLabel] Quadrant '" << quadrant << "' not implemented yet!" << std::endl;
    return 0;
  }

}


TPaveText* NanoUVCommon::getNanoUVLabel( float xmin, float ymin, float xmax, float ymax, const std::string& text ) {

  TPaveText* label = new TPaveText(xmin, ymin, xmax, ymax, "brNDC");
  label->SetBorderSize(0);
  label->SetFillColor(kWhite);
  if( text=="NanoUV" ) {
    label->SetTextSize(0.06);
    label->SetTextFont(72);
  } else {
    label->SetTextSize(0.04);
  }
  label->AddText( text.c_str() );

  return label;

}



float NanoUVCommon::integrateSignal( TGraph* graph, bool invertPolarity ) {

  float pedestal = NanoUVCommon::getPedestal( graph, 100 );

  float signal = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    if( invertPolarity )
      signal = signal - (y-pedestal);
    else
      signal = signal + (y-pedestal);

  }

  return signal;

}


float NanoUVCommon::ampMaxSignal( TGraph* graph, bool invertPolarity ) {

  float pedestal = NanoUVCommon::getPedestal( graph, 100 );

  float ampMax = 0.;

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    float thisPoint = (y-pedestal);
    if( invertPolarity ) thisPoint = -1.*thisPoint;
    if( thisPoint > ampMax ) ampMax = thisPoint;

  }

  return ampMax;

}



TGraph* NanoUVCommon::getGraphFromFile( const std::string& fileName ) {

  std::ifstream ifs( fileName.c_str() );

  if( !ifs.good() ) return 0;

  std::string line;
  bool read = false;
  TGraph* graph = new TGraph(0);
  graph->SetName(Form("wf_%s", fileName.c_str()));

  if( ifs.good() ) {

    while( getline(ifs,line) ) {

      std::string delimiter = ",";
      size_t pos = 0;
      std::vector<std::string> words;
      std::string word;
      while ((pos = line.find(delimiter)) != std::string::npos) {
        word = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        words.push_back(word);
      }
      line.erase( line.size()-1, line.size() ); // chopping off trailing char
      words.push_back(line); // last part

      if( read ) {

        double x(atof(words[0].c_str()));
        double y(atof(words[1].c_str()));

        graph->SetPoint( graph->GetN(), x*1E9, y*1E3 ); // ns and mV

      }

      if( words[0]=="Time" && words[1]=="Ampl" ) {

        read = true;

      }

    }  // while get line

  } // if ifs good

  NanoUVCommon::removeTimeOffset( graph );


  return graph;

}


void NanoUVCommon::plotWaveformGraph( TGraph* graph, const std::string& saveName ) {

  TCanvas* c1 = new TCanvas( Form("c1_%s", graph->GetName()), "", 600, 600 );
  c1->cd();

  double x0, y0;
  graph->GetPoint( 0, x0, y0 );

  double xmax, ymax;
  graph->GetPoint( graph->GetN()-1, xmax, ymax );

  float pedestal = NanoUVCommon::getPedestal( graph, 100 );

  TH2D* h2_axes = new TH2D( Form("axes_%s", graph->GetName()), "", 10, x0, xmax, 10, pedestal-150., pedestal+15. );
  h2_axes->SetXTitle( "Time [ns]" );
  h2_axes->SetYTitle( "Amplitude [mV]" );
  h2_axes->Draw();

  TLine* line_pedestal = new TLine( x0, pedestal, xmax, pedestal );
  line_pedestal->SetLineColor(46);
  line_pedestal->SetLineStyle(2);
  line_pedestal->SetLineWidth(2);
  line_pedestal->Draw("same");

  graph->Draw("L same");

  gPad->RedrawAxis();

  c1->SaveAs( saveName.c_str() );
  
  delete c1;
  delete h2_axes;
  delete line_pedestal;

}


void NanoUVCommon::removeTimeOffset( TGraph* graph ) {

  double x0, y0;
  graph->GetPoint( 0, x0, y0 );

  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    graph->SetPoint( iPoint, x-x0, y );

  }

}




float NanoUVCommon::getPedestal( TGraph* graph, int nPoints ) {

  float sum = 0.;

  for( unsigned iPoint=0; iPoint<nPoints; ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );
    sum += y;

  }

  return sum/((float)nPoints);

}



std::string NanoUVCommon::scientific( float x, int decimals ) {

  int power = 0.;

  while( x > 10. ) {
    x /= 10.;
    power += 1;
  }

  while( x < 1. ) {
    x *= 10.;
    power -= 1;
  }

  std::string sign = (power>=0) ? "+" : "";

  std::string scient;
  if( decimals==1 )      scient = std::string(Form("%.1fE%s%d", x, sign.c_str(), power));
  else if( decimals==2 ) scient = std::string(Form("%.2fE%s%d", x, sign.c_str(), power));
  else if( decimals==3 ) scient = std::string(Form("%.3fE%s%d", x, sign.c_str(), power));
  else if( decimals==4 ) scient = std::string(Form("%.4fE%s%d", x, sign.c_str(), power));
  else if( decimals==0 ) scient = std::string(Form("%.0fE%s%d", x, sign.c_str(), power));

  return scient;

}


void NanoUVCommon::setStyle() {

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

