#include "../../CommonTools/interface/NanoUVCommon.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>

#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"



#define PI 3.14159265


TGraphErrors* getNormalizedGraph( TGraphErrors* graph);


int main() {


  TGraphErrors* graph = new TGraphErrors(0);

  std::ifstream ifs("data/drainCurrent_polarAngle_cnt.dat");
  
  std::string line;
  while( ifs.good() ) {
  
    std::getline(ifs, line);
    TString line_tstr(line);
    if( line_tstr.BeginsWith("#") ) continue;
    if( line_tstr.Length()<10 ) continue;

    float theta, i_on, sigma_i_on, flux_on, i_off, sigma_i_off, flux_off;

    std::istringstream iss(line);
    iss >> theta >> i_on >> sigma_i_on >> flux_on >> i_off >> sigma_i_off >> flux_off;


    theta -= 270.;

    float angle = acos( sin(54.*PI/180.)*cos((45.+theta)*PI/180.))*180./PI;

    float delta_i = i_on-i_off;
    float sigma_delta_i = sqrt( sigma_i_on*sigma_i_on + sigma_i_off*sigma_i_off);

    float delta_flux = flux_on - flux_off;
    float sigma_delta_flux = sqrt( flux_on + flux_off ); //assuming poisson stats, so sigma_flux = sqrt(flux)

    float di_o_df = delta_i/delta_flux;
    float sigma_di_o_df = sqrt( sigma_delta_i*sigma_delta_i/(delta_flux*delta_flux) + sigma_delta_flux*sigma_delta_flux*delta_i*delta_i/(delta_flux*delta_flux*delta_flux*delta_flux)
);

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, angle, di_o_df );
    graph->SetPointError( iPoint, 0., sigma_di_o_df );

  }


  TGraphErrors* graphNorm = getNormalizedGraph( graph );


  TCanvas* c1 = new TCanvas( "c1", "c1", 600, 600 );
  c1->cd();

  float xMin = 40.;
  float xMax = 91.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., 13. );
  h2_axes->SetXTitle( "Angle with respect to normal (deg)" );
  h2_axes->SetYTitle( "#DeltaI/#Delta#Phi (normalized to first point)");
  h2_axes->Draw();

  TLine* lineOne = new TLine( xMin, 1., xMax, 1. );
  lineOne->SetLineColor(46);
  lineOne->SetLineWidth(2);
  lineOne->Draw("same");

  graphNorm->SetMarkerStyle(20);
  graphNorm->SetMarkerSize(1.6);
  graphNorm->SetMarkerColor(kGray+3);
  graphNorm->SetLineColor(kGray+3);
  graphNorm->Draw("Psame");

  c1->SaveAs("drainCurrent_polarScan.pdf");

  return 0;

}




TGraphErrors* getNormalizedGraph( TGraphErrors* graph ) {

  TGraphErrors* newgraph = new TGraphErrors(0);

  // first find yNorm
  float xMin = 1000.;
  float yNorm = -1.;
  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    if( x<xMin ) {
      xMin = x;
      yNorm = y;
    }

  }


  for( unsigned iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    newgraph->SetPoint ( iPoint, x, y/yNorm );
    newgraph->SetPointError( iPoint, 0., graph->GetErrorY( iPoint ) );

  }

  return newgraph;

} 
