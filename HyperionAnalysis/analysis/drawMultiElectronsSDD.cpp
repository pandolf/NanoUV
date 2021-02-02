#include "NanoUVCommon.h"
#include "SDD.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"



std::string outdir = "plots/FieldEmissCNT";

std::string varName = "vcharge";


int NORDERS = 2;


Double_t fPhotoEle(Double_t *x, Double_t *par)
{

   float N = par[0];
   float mu = par[1];
   float Q1 = par[2];
   float sigma = par[3];
   float offset = par[4];
   float sigmaoffset = par[5];
   float alpha = par[6];
   float w = par[7];

   float xx = x[0];
   double value = 0.;

   for( unsigned i=1; i<NORDERS; ++i ) {

     double sigma_n = sqrt( (double)(i)*sigma*sigma + sigmaoffset*sigmaoffset);
     value = value + N*(TMath::Poisson( i, mu ) * TMath::Gaus( xx, (double)i*Q1 + offset, sigma_n) );

   }

   return value;

}



int main() {

  NanoUVCommon::setStyle();

  system( Form("mkdir -p %s", outdir.c_str()) );

  float xMax_keV = 5.5;

  TFile* file = TFile::Open( "data/HyperionSDD/CNT50um_fusedITO_B/Run_SDD_G300_HVcnt1800_L32_h17_trig0p6_pressureOFF_500k_Measurements_Only_2_2_2021.root" ); 
  TTree* tree = (TTree*)file->Get("tree");
  TH1D* h1 = SDD::fillFromTree( tree, "h1", varName, 300, 100, 0., xMax_keV );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();
  c1->SetLogy();

  float yMin = 10.;
  float yMax = 200000.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax_keV, 10, yMin, yMax );
  h2_axes->SetXTitle( "Energy [keV]" );
  h2_axes->SetYTitle( "Entries" );
  h2_axes->Draw();

  h1->SetLineWidth(3);
  h1->SetLineColor(kGray+3);
  h1->Draw("same");

  TF1* f0 = new TF1( "f0", "gaus", 0.6, 1.1 );
  f0->SetParameter(1, h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin()));
  h1->Fit(f0, "0R");

  TF1* f1 = new TF1( "func", fPhotoEle, 0.4, 1.5, 8 );
  //f1->SetParameter( 0, 500000 ); //normalization
  f1->SetParameter( 1, 1. ); //poiss mu
  f1->SetParameter( 2, f0->GetParameter(1) ); //gauss step
  f1->SetParameter( 3, f0->GetParameter(2) ); //gauss sigma
  f1->SetParameter( 4, 0. ); //offset
  //f1->SetParameter( 5, 3. ); //sigmaoffset
  //f1->SetParameter( 6, 0.03 ); //alpha
  //f1->SetParameter( 7, 0.4 ); //w

  f1->FixParameter( 5, 0. ); //sigmaoffset
  f1->FixParameter( 6, 0. ); //alpha
  f1->FixParameter( 7, 0. ); //w

//f1->SetParLimits( 1, 0.5, 2.5 ); //poiss mu
//f1->SetParLimits( 2, 10., 40. ); //gauss step
//f1->SetParLimits( 3, 3., 12. ); //gauss sigma

  f1->SetLineColor(46); 
  h1->Fit( f1, "R" );


//float ele = f1->GetParameter(1);

//for( int i=1; i<5; ++i ) {

//  TLine* line = new TLine( ele*i, 0., ele*i, yMax );
//  line->SetLineColor(kGray+3);
//  line->SetLineStyle(2);
//  line->Draw("same");

//}

  c1->cd();
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/multiele.pdf", outdir.c_str()) );

  return 0;

}
