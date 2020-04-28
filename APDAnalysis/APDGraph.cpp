#include "NanoUVCommon.h"

#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>



TGraphErrors* getGraphFromFile( const std::string& fileName, float& Egun, float& Igun );
TGraphErrors* getGain( TGraphErrors* gopen, TGraphErrors* gdark, float Igun );
std::string getDarkFile( const std::string& file_open );


int main( int argc, char* argv[] ) {

std::string fileOpen;

if( argc==1 ) {

  std::cout << "USAGE: ./APDGraph [fileName]" << std::endl;
  exit(1);

} else {

  fileOpen = std::string(argv[1]);
}

std::string fileDark = getDarkFile(fileOpen);

NanoUVCommon::setStyle();


TCanvas* c1 = new TCanvas("c1", "", 500, 500);
c1->cd();
c1->SetLogy();

TH2D* h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 1.001, 2000000. );
h2_axes->SetXTitle( "APD HV [V]" );
h2_axes->SetYTitle( "APD Current [nA]" );
//h2_axes->GetYaxis()->SetNoExponent();
//h2_axes->GetYaxis()->SetMoreLogLabels();
h2_axes->Draw();

NanoUVCommon::addNanoUVLabel( c1, 2 );

float gunEnergy, gunCurrent;
TGraphErrors* graphOpen = getGraphFromFile(fileOpen, gunEnergy, gunCurrent);
TGraphErrors* graphDark = getGraphFromFile(fileDark, gunEnergy, gunCurrent);

TLegend* legend = new TLegend( 0.15, 0.6, 0.6, 0.8 );
legend->SetTextSize(0.035);
legend->SetFillStyle(0);
legend->SetBorderSize(0);
if( gunEnergy != 0 && gunCurrent != 0 ){
    legend->AddEntry((TObject*)0, Form("E gun = %.0f eV", gunEnergy), "");
    legend->AddEntry((TObject*)0, Form("I gun = %.1f pA", gunCurrent), "");
    }
legend->AddEntry(graphOpen, "I APD", "P");
legend->AddEntry(graphDark, "I dark", "P");
legend->Draw("same");

graphOpen->SetLineColor(46);
graphOpen->SetMarkerColor(46);
graphOpen->SetMarkerSize(0.8);
graphOpen->SetMarkerStyle(20);
graphOpen->Draw("P same");

graphDark->SetLineColor(38);
graphDark->SetMarkerColor(38);
graphDark->SetMarkerSize(0.8);
graphDark->SetMarkerStyle(20);
graphDark->Draw("P same");

if( fileOpen == "IV_light.txt" ) {
     c1->SaveAs("IV_light.pdf");} 
else{c1->SaveAs(Form("IV_E%.0f_I%.1f.pdf", gunEnergy, gunCurrent));}



TCanvas* c2 = new TCanvas("c2", "", 500, 500);
c2->cd();
c2->SetLogy();

delete h2_axes;
if( fileOpen == "IV_E900_I2p9.txt" ) {
     h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 10, 100000. );}
else{h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 0.1, 4000. );}
h2_axes->SetXTitle( "APD HV [V]" );
h2_axes->SetYTitle( "APD Gain" );
//h2_axes->GetYaxis()->SetNoExponent();
//h2_axes->GetYaxis()->SetMoreLogLabels();
h2_axes->Draw();

NanoUVCommon::addNanoUVLabel( c2, 2 );

TGraphErrors* graphGain = getGain( graphOpen, graphDark, gunCurrent );

if( gunEnergy != 0 && gunCurrent != 0 ){
    TLegend* legendG = new TLegend( 0.1, 0.7, 0.6, 0.8 );
    legendG->SetTextSize(0.035);
    legendG->SetFillStyle(0);
    legendG->SetBorderSize(0);
    legendG->AddEntry((TObject*)0, Form("E gun = %.0f eV", gunEnergy), "");
    legendG->AddEntry((TObject*)0, Form("I gun = %.1f pA", gunCurrent), "");
    //legendG->AddEntry(graphGain, "Gain", "P");
    legendG->Draw("same");
    }

graphGain->SetLineColor(46);
graphGain->SetMarkerColor(46);
graphGain->SetMarkerSize(0.8);
graphGain->SetMarkerStyle(20);
graphGain->Draw("P same");

if( fileOpen == "IV_light.txt" ) {
     c2->SaveAs("Gain_light.pdf");} 
else{c2->SaveAs(Form("Gain_E%.0f_I%.1f.pdf", gunEnergy, gunCurrent));}  



delete c1;
delete c2;
delete graphOpen;
delete graphDark;
delete graphGain;
return 0;
}


std::string getDarkFile( const std::string& file_open ) {

   std::string file_dark;

   if( file_open == "IV_E900_I2p9.txt" || file_open == "IV_E900_I2p9_DEFLESSO.txt" ) {
     
     file_dark = std::string("IV_dark1.txt");
   
     } else if( file_open == "IV_light.txt" ) {  
   
     file_dark = std::string("IV_darkL.txt");
   
     } else {
   
     file_dark = std::string("IV_dark1.txt"); 
   
   }

   return file_dark;
}


TGraphErrors* getGraphFromFile( const std::string& fileName, float& Egun, float& Igun ) {

   std::ifstream ifs(fileName.c_str());
   std::cout << "Opening: " << fileName.c_str() << std::endl;

   TGraphErrors* graph = new TGraphErrors(0);
   graph->SetName(Form("gr_%s", fileName.c_str()));

   float hv, I, ErrI, gunE, gunI;
   char a;

   std::string line;
   while( ifs.good() ) {
   
      std::getline(ifs, line);
      TString line_tstr(line);

      if( line_tstr.BeginsWith("#") ){ 
          continue;
      } else if( line_tstr.BeginsWith("@") ){
          std::istringstream iss(line);
          iss >> a >> gunE >> gunI;
          
          Egun = gunE;
          Igun = gunI;
      } else{   
          std::istringstream iss(line); 
          iss >> hv >> I >> ErrI;

          int i = graph->GetN();
          graph->SetPoint(i, hv, I);
          graph->SetPointError(i, 0., ErrI);

          }
 
      } 
   return graph;     

   }

TGraphErrors* getGain( TGraphErrors* gopen, TGraphErrors* gdark, float Igun  ) {

   TGraphErrors* graphGAIN = new TGraphErrors(0);
   graphGAIN->SetName("grgain");

   int V_0 = 1;
   double Vopen0, Vdark0, Iopen0, Idark0;
   gopen->GetPoint(V_0, Vopen0, Iopen0);
   gdark->GetPoint(V_0, Vdark0, Idark0);
   float Eopen0 = gopen->GetErrorY(V_0);
   float Edark0 = gdark->GetErrorY(V_0);
   float IgunNano = Igun/1000.;           //converting gun current to nA
   float ErrorGun = 0.1*IgunNano;         //Gun current error: 10% 
   //float ErrorGun = (diffCurrent>fixedPercent) ? diffCurrent : fixedPercent;

   for (unsigned i=0; i<gopen->GetN(); ++i) {
   
      int k = graphGAIN->GetN();
      double Vopen, IopenV, Vdark, IdarkV;

      gopen->GetPoint(i, Vopen, IopenV);
      gdark->GetPoint(i, Vdark, IdarkV);
      float EopenV = gopen->GetErrorY(i);
      float EdarkV = gdark->GetErrorY(i);
      
      float ErrG_light = sqrt((pow(Iopen0-Idark0,2)*(pow(EopenV,2)+pow(EdarkV,2)))+(pow(IopenV-IdarkV,2)*(pow(Eopen0,2)+pow(Edark0,2))))/pow(Iopen0-Idark0,2);
      float ErrG =  sqrt((pow(IgunNano,2)*(pow(EopenV,2)+pow(EdarkV,2)))+(pow(IopenV-IdarkV,2)*(pow(ErrorGun,2))))/pow(IgunNano,2);

      if( Igun == 0 ) { 
          graphGAIN->SetPoint(k, Vopen, (IopenV-IdarkV)/(Iopen0-Idark0));    //gain for APD illuminated with phone light
          graphGAIN->SetPointError(k, 0., ErrG_light);
          if( Vopen == 350 || Vopen == 360){
              std::cout << "gain(" <<  Vopen << ") = " << (IopenV-IdarkV)/(Iopen0-Idark0) << std::endl;
              std::cout << "error = " << ErrG_light << std::endl;
              }
          }  
      else { 
          graphGAIN->SetPoint(k, Vopen, (IopenV-IdarkV)/IgunNano);    //gain for APD illuminated with gun    
          graphGAIN->SetPointError(k, 0., ErrG);
          if( Vopen == 350 || Vopen == 360){
              std::cout << "gain(" <<  Vopen << ") = " << (IopenV-IdarkV)/IgunNano << std::endl;
              std::cout << "error = " << ErrG << std::endl;
              }
          } 
      }
      


return graphGAIN;

}





