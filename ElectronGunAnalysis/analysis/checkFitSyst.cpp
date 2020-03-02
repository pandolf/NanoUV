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

  NanoUVCommon::setStyle();

  std::vector< GunScan* > scans  = GunScan::loadScans( "data/2020_02/scans_baseline.dat", 900, 380 );

  TH1D* h1_systTot  = new TH1D( "systTot", "", 50, -2.5, 2.5 );
  TH1D* h1_systMean = new TH1D( "systMean", "", 50, -2.5, 2.5 );
  TH1D* h1_systRMS  = new TH1D( "systRMS" , "", 50, 0., 5. );


  for( unsigned iScan=0; iScan<scans.size(); iScan++ ) {

    if( scans[iScan]->APDhv() != 380 ) continue;

    scans[iScan]->loadScan();
    scans[iScan]->correctGraph();

    TGraph* thisGraph = scans[iScan]->graph();
    TF1* thisBaseline = scans[iScan]->baseline();

    TH1D* h1_thisSyst = new TH1D( Form("syst%d", iScan), "", 50, -2.5, 2.5 );

    for( int iPoint=0; iPoint<thisGraph->GetN(); ++iPoint ) {

      if( iPoint<= scans[iScan]->firstN_fit() ) continue;
      if( thisGraph->GetN()-iPoint-1 <= scans[iScan]->lastN_fit() ) continue;

      double thisX, thisY;
      thisGraph->GetPoint(iPoint, thisX, thisY);

      h1_thisSyst->Fill( (thisBaseline->Eval(thisX)-thisY)/thisY );
      h1_systTot ->Fill( (thisBaseline->Eval(thisX)-thisY)/thisY );

    } // for points

    h1_systMean->Fill( h1_thisSyst->GetMean() );
    h1_systRMS ->Fill( h1_thisSyst->GetRMS () );

  } // for scans

  TFile* file = TFile::Open( "testSyst.root", "recreate" );
  file->cd();

  h1_systTot ->Write(); 
  h1_systMean->Write(); 
  h1_systRMS ->Write(); 

  file->Close();

  return 0;

}    
