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


int NORDERS = 10;


Double_t fPhotoEle(Double_t *x, Double_t *par)
{

   float N = par[0];
   float mu = par[1];
   float Q1 = par[2];
   float sigma = par[3];
   float offset = par[4];
   //float sigmaoffset = par[5];
   float xflex = par[5];
   float w = par[6];
   float alpha = par[7];
   float w2 = par[8];
   float alpha2 = par[9];
   //float w3 = par[10];
   //float alpha3 = par[11];

   float xx = x[0];
   double value = 0.;

   float sigmaoffset = 0.;

   for( unsigned i=1; i<=NORDERS; ++i ) {

     double sigma_n = sqrt( (double)(i)*sigma*sigma + sigmaoffset*sigmaoffset);
     value = value + N*(TMath::Poisson( i, mu ) * TMath::Gaus( xx, (double)i*Q1 + offset, sigma_n) );

   }

   //value = value + TMath::Erf(-(xx-xflex))*w*TMath::Exp(-alpha*xx);
   value = value + TMath::Erf(-(xx-xflex))*(w*TMath::Exp(-alpha*xx) + w2*TMath::Exp(-alpha2*xx));
   //value = value + TMath::Erf(-(xx-xflex))*(w*TMath::Exp(-alpha*xx) + w2*TMath::Exp(-alpha2*xx) + w3*TMath::Exp(-alpha3*xx));

   //old:
   //value = value + w*TMath::LogNormal( xx-xflex, alpha );
   //value = value + par[5] + par[6]*xx + par[7]*xx*xx;// + par[8]*xx*xx*xx;
   //value = value + TMath::Erf(-(xx-xflex))*(w*TMath::Exp(-alpha*xx) + w2*TMath::Exp(-alpha2*xx));
   //value = value + w*TMath::Erf(-(xx-xflex))*TMath::Exp(-alpha*xx);
   //value = value + (w*TMath::Exp(-alpha*xx)+w2*TMath::Exp(-alpha2*xx))*TMath::Erf(-(xx-xflex));

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

  float yMin = 20.;
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

  TF1* f1 = new TF1( "func", fPhotoEle, 0.45, 4., 10 );
  f1->SetParameter( 0, f0->GetParameter(0) ); //normalization
  f1->SetParameter( 1, 1. ); //poiss mu
  f1->SetParameter( 2, f0->GetParameter(1) ); //gauss step
  f1->SetParameter( 3, f0->GetParameter(2) ); //gauss sigma
  f1->SetParameter( 4, 0. ); //offset
  f1->SetParameter( 5, 1. ); //xflex
  f1->SetParameter( 6, 500. ); //w
  f1->SetParameter( 7, 1. ); //alpha
  f1->SetParameter( 8, 50. ); //w2
  f1->SetParameter( 9, 2. ); //alpha2

  //f1->FixParameter( 5, 0. ); //sigmaoffset
  //f1->FixParameter( 6, 0. ); //alpha
  //f1->FixParameter( 7, 0. ); //w

  f1->SetParLimits( 5, 0., 3.); //xflex
  f1->SetParLimits( 7, 0.3, 2. );

  f1->SetLineColor(46); 
  h1->Fit( f1, "R" );

//TF1* bg = new TF1( "bg", "[0]*exp(-[1]*x)", 3., 5.2 );
//h1->Fit( bg, "R");

  float ele = f1->GetParameter(2);
  float offset = f1->GetParameter(4);

  for( int i=1; i<4; ++i ) {

    TLine* line = new TLine( offset+ele*i, 0., offset+ele*i, yMax );
    line->SetLineColor(46);
    line->SetLineStyle(2);
    line->Draw("same");

    TPaveText* label_ele = new TPaveText( 0.3+ele*i, yMax/2., 0.4+ele*i, yMax/2. );
    label_ele->SetFillColor(0);
    label_ele->SetTextSize(0.033);
    label_ele->SetTextColor(46);
    label_ele->AddText( Form("%de^{-}", i) );
    label_ele->Draw("same");

  }

  c1->cd();
  gPad->RedrawAxis();
  c1->SaveAs( Form("%s/multiele.pdf", outdir.c_str()) );
  c1->SaveAs( Form("%s/multiele.eps", outdir.c_str()) );

  return 0;

}
