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



TGraphErrors* getGraphFromFile( const std::string& fileName );
TGraphErrors* getGain( TGraphErrors* gopen, TGraphErrors* gdark, float Igun );



int main( int argc, char* argv[] ) {

std::string fileOpen;
std::string fileDark("IV_dark.txt");
float gunEnergy; 
float gunCurrent; 
if( argc==1 ) {

  std::cout << "USAGE: ./APDGraph [fileName] [gunEnergy] [gunCurrentInpA]" << std::endl;
  exit(1);

} else {

  fileOpen = std::string(argv[1]);
  gunEnergy = std::stof(std::string(argv[2]));
  gunCurrent = std::stof(std::string(argv[3]));
}



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

TGraphErrors* graphOpen = getGraphFromFile(fileOpen);
TGraphErrors* graphDark = getGraphFromFile(fileDark);

TLegend* legend = new TLegend( 0.2, 0.6, 0.6, 0.8 );
legend->SetTextSize(0.025);
legend->SetFillColor(0);
legend->AddEntry((TObject*)0, Form("E gun = %f eV", gunEnergy), "");
legend->AddEntry((TObject*)0, Form("I gun = %f pA", gunCurrent), "");
legend->AddEntry(graphOpen, "I APD", "P");
legend->AddEntry(graphDark, "I dark", "P");
legend->Draw("same");

graphOpen->SetLineColor(46);
graphOpen->SetMarkerColor(46);
graphOpen->SetMarkerSize(1);
graphOpen->SetMarkerStyle(20);
graphOpen->Draw("P same");

graphDark->SetLineColor(38);
graphDark->SetMarkerColor(38);
graphDark->SetMarkerSize(1);
graphDark->SetMarkerStyle(20);
graphDark->Draw("P same");

c1->SaveAs(Form("IV_E%f_I%f.pdf", gunEnergy, gunCurrent ));



TCanvas* c2 = new TCanvas("c2", "", 500, 500);
c2->cd();
c2->SetLogy();

delete h2_axes;
h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 0.1, 4000. );
h2_axes->SetXTitle( "APD HV [V]" );
h2_axes->SetYTitle( "APD Gain" );
//h2_axes->GetYaxis()->SetNoExponent();
//h2_axes->GetYaxis()->SetMoreLogLabels();
h2_axes->Draw();

NanoUVCommon::addNanoUVLabel( c2, 2 );

TGraphErrors* graphGain = getGain( graphOpen, graphDark, gunCurrent );

TLegend* legendG = new TLegend( 0.2, 0.6, 0.6, 0.8 );
legendG->SetTextSize(0.025);
legendG->SetFillColor(0);
legendG->AddEntry((TObject*)0, Form("E gun = %f eV", gunEnergy), "");
legendG->AddEntry((TObject*)0, Form("I gun = %f pA", gunCurrent), "");
legendG->AddEntry(graphGain, "Gain", "P");
legendG->Draw("same");

graphGain->SetLineColor(46);
graphGain->SetMarkerColor(46);
graphGain->SetMarkerSize(1);
graphGain->SetMarkerStyle(20);
graphGain->Draw("P same");

c2->SaveAs(Form("Gain_E%f_I%f.pdf", gunEnergy, gunCurrent));   



delete c1;
delete c2;
delete graphOpen;
delete graphDark;
delete graphGain;
return 0;
}



TGraphErrors* getGraphFromFile( const std::string& fileName ) {

   std::ifstream ifs(fileName.c_str());
   std::cout << "Opening: " << fileName.c_str() << std::endl;

   TGraphErrors* graph = new TGraphErrors(0);
   graph->SetName(Form("gr_%s", fileName.c_str()));

   float hv, I, ErrI;

   std::string line;
   while( ifs.good() ) {
   
      std::getline(ifs, line);
      TString line_tstr(line);
      if( line_tstr.BeginsWith("#") ) continue;

      std::istringstream iss(line); 
      iss >> hv >> I >> ErrI;

      int i = graph->GetN();
      graph->SetPoint(i, hv, I);
      graph->SetPointError(i, 0., ErrI);

      }
  
   return graph;     

}

TGraphErrors* getGain( TGraphErrors* gopen, TGraphErrors* gdark, float Igun  ) {

   TGraphErrors* graphGAIN = new TGraphErrors(0);
   graphGAIN->SetName("grgain");

   //int V_0 = 1;
   //double Vopen0, Vdark0, Iopen0, Idark0;
   //gopen->GetPoint(V_0, Vopen0, Iopen0);
   //gdark->GetPoint(V_0, Vdark0, Idark0);
   
   for (unsigned i=0; i<gopen->GetN(); ++i) {
   
      int k = graphGAIN->GetN();
      double Vopen, IopenV, Vdark, IdarkV;
      float IgunNano = Igun/1000.;    //converting gun current to nA

      gopen->GetPoint(i, Vopen, IopenV);
      gdark->GetPoint(i, Vdark, IdarkV);
      graphGAIN->SetPoint(k, Vopen, (IopenV-IdarkV)/IgunNano);
   
      }


return graphGAIN;

}





