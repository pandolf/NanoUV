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


  float APDhv;

  if( argc==1 ) {

    std::cout << "USAGE: ./checkFitSyst [APDhv]" << std::endl;
    exit(1);

  } else {

    APDhv = atof(argv[1]);

  }

  NanoUVCommon::setStyle();

  std::vector< GunScan* > scans  = GunScan::loadScans( "data/baselines/scans.txt", 900, APDhv );

  TH1D* h1_systTot  = new TH1D( "systTot" , "", 50, -0.25, 0.25 );
  TH1D* h1_systMean = new TH1D( "systMean", "", 50, -0.25, 0.25 );
  TH1D* h1_systRMS  = new TH1D( "systRMS" , "", 50, 0., 0.5 );

  TGraphErrors* gr_syst = new TGraphErrors(0);

  // will need also later (after loop)
  TPaveText* label_settings = new TPaveText( 0.21, 0.7, 0.48, 0.77, "brNDC" );
  label_settings->SetTextSize( 0.035 );
  label_settings->SetTextColor( 46 );
  label_settings->SetFillColor(0);
  label_settings->AddText( Form( "V_{APD} = %.0f V", APDhv ) );
  label_settings->SetTextAlign(11);
    

  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

    if( scans[iScan]->APDhv() != APDhv ) continue;

    scans[iScan]->correctGraph();

    TGraph* thisGraph = scans[iScan]->graph();
    TF1* thisBaseline = scans[iScan]->baseline();

    TGraph* gr_fit   = new TGraph(0);
    TGraph* gr_nofit = new TGraph(0);
    gr_fit  ->SetName( Form("fit_%s"  , thisGraph->GetName()) );
    gr_nofit->SetName( Form("nofit_%s", thisGraph->GetName()) );

    TH1D* h1_thisSyst = new TH1D( Form("syst%d", iScan), "", 50, -2.5, 2.5 );

    for( int iPoint=0; iPoint<thisGraph->GetN(); ++iPoint ) {

      bool isFit=false;
      if( iPoint<= scans[iScan]->firstN_fit() ) isFit=true;
      if( thisGraph->GetN()-iPoint-1 <= scans[iScan]->lastN_fit() ) isFit=true;

      double thisX, thisY;
      thisGraph->GetPoint(iPoint, thisX, thisY);

      if( isFit ) {

        gr_fit->SetPoint( gr_fit->GetN(), thisX, thisY );
  
      } else { // in extrap region, so compute also syst

        gr_nofit->SetPoint( gr_nofit->GetN(), thisX, thisY );

        h1_thisSyst->Fill( (thisBaseline->Eval(thisX)-thisY) );
        h1_systTot ->Fill( (thisBaseline->Eval(thisX)-thisY) );

      } // if isFit

    } // for points

    h1_systMean->Fill( h1_thisSyst->GetMean() );
    h1_systRMS ->Fill( h1_thisSyst->GetRMS () );

    int thisPoint = gr_syst->GetN();
    gr_syst->SetPoint     ( thisPoint, thisPoint+0.5, h1_thisSyst->GetMean() );
    gr_syst->SetPointError( thisPoint,            0., h1_thisSyst->GetRMS()  );

    TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
    c1->cd();

    double xFirst, xLast, yFirst, yLast;
    thisGraph->GetPoint( 0, xFirst, yFirst );
    thisGraph->GetPoint( thisGraph->GetN()-1, xLast, yLast );

    double xMin, xMax;
    if(xFirst<xLast) {
      xMin = xFirst;
      xMax = xLast;
    } else {
      xMin = xLast;
      xMax = xFirst;
    }
    
    double yMax = GunScan::getYmax( thisGraph );
    double yDiff = (yLast<yFirst) ? fabs(yMax - yLast) : fabs(yMax - yFirst);
    double y0 = (yLast<yFirst) ? yLast : yFirst;


    TH2D* h2_axes = new TH2D( Form("axes_%s", thisGraph->GetName()), "", 10, xMin, xMax, 10, y0 - 0.5*yDiff, y0 + 2.5*yDiff );
    h2_axes->SetXTitle("Gun Position [mm]"); 
    h2_axes->SetYTitle("APD Current [nA]");
    h2_axes->Draw();

    TPaveText* label = NanoUVCommon::getNanoUVLabel(2);
    label->Draw("same");  
    gPad->RedrawAxis();

    label_settings->Draw("same");

    thisBaseline->SetLineWidth(2);
    thisBaseline->SetLineColor(46);
    thisBaseline->SetLineStyle(2);
    thisBaseline->Draw("L same");

    gr_fit->SetMarkerStyle(24);
    gr_fit->SetMarkerSize(1.1);
    gr_fit->SetMarkerColor(kGray+3);
    gr_fit->SetLineColor(kGray+3);
    gr_fit->Draw("P same");

    gr_nofit->SetMarkerStyle(20);
    gr_nofit->SetMarkerSize(1.1);
    gr_nofit->SetMarkerColor(kGray+3);
    gr_nofit->SetLineColor(kGray+3);
    gr_nofit->Draw("P same");

    TLegend* legend = new TLegend( 0.5, 0.75, 0.9, 0.9 );
    legend->SetTextSize(0.035);
    legend->SetFillColor(0);
    legend->AddEntry( gr_fit, "Fitted points", "P" );
    legend->AddEntry( gr_nofit, "Extrapolation points", "P" );
    legend->Draw("same");

    c1->SaveAs(Form("%s/baseline_%s.pdf", scans[iScan]->outdir().c_str(), scans[iScan]->scanName().c_str()));
    c1->SaveAs(Form("%s/baseline_%s.eps", scans[iScan]->outdir().c_str(), scans[iScan]->scanName().c_str()));

    delete c1;
    delete h2_axes;

  } // for scans


  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();

  float yMin = (APDhv==380) ? -0.2 : -0.02;
  float yMax = (APDhv==380) ?  0.2 :  0.02;

  TH2D* h2_axes2 = new TH2D( "axes2", "", 10, 0., gr_syst->GetN(), 10, yMin, yMax );
  h2_axes2->SetXTitle( "Baseline scan number" );
  h2_axes2->SetYTitle( "Fit - true [nA]" );
  h2_axes2->Draw();

  TLine* line_zero = new TLine( 0., 0., gr_syst->GetN(), 0. );
  line_zero->Draw("same");
 
  gr_syst->SetMarkerStyle(20);
  gr_syst->SetMarkerSize(1.3);
  gr_syst->SetMarkerColor(46);
  gr_syst->SetLineColor(kGray+3);
  gr_syst->Draw("Psame");

  NanoUVCommon::addNanoUVLabel( c2, 2 );

  label_settings->Draw("same");

  c2->SaveAs( Form("systFit_APDhv%.0f.pdf", APDhv) );


//TFile* file = TFile::Open( "testSyst.root", "recreate" );
//file->cd();

//h1_systTot ->Write(); 
//h1_systMean->Write(); 
//h1_systRMS ->Write(); 

//file->Close();

  return 0;

}    
