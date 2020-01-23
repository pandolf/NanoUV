#include "NanoUVCommon.h"

#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"

#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>


int main() {


NanoUVCommon::setStyle();


std::string file( "V_I_G.txt");
std::ifstream ifs(file.c_str());
std::cout << "Opening: " << file.c_str() << std::endl;


TCanvas* c1 = new TCanvas("c1", "", 500, 500);
c1->cd();
c1->SetLogy();

TH2D* h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 1.001, 2000000. );
h2_axes->SetXTitle( "APD HV [V]" );
h2_axes->SetYTitle( "APD Current [nA]" );
//h2_axes->GetYaxis()->SetNoExponent();
//h2_axes->GetYaxis()->SetMoreLogLabels();
h2_axes->Draw();

TGraphErrors* graph1 = new TGraphErrors(0);
graph1->SetName("gr1");

TGraphErrors* graph2 = new TGraphErrors(0);
graph2->SetName("gr2");

TGraphErrors* graphGAIN = new TGraphErrors(0);
graphGAIN->SetName("grgain");

float hv, openI, EopenI, darkI, EdarkI, gain;

float v_0 = 0;
int a;

while( ifs.good() ) {

   ifs >> hv >> openI >> EopenI >> darkI >> EdarkI >> gain;

   int i = graph1->GetN();
   graph1->SetPoint(i, hv, openI);
   graph1->SetPointError(i, 0., EopenI);

   int j = graph1->GetN();
   graph2->SetPoint(j, hv, darkI);
   graph2->SetPointError(j, 0., EdarkI);

   if (hv == v_0) {
      
      a = i;
      
      }

   }

TLegend* legend = new TLegend( 0.2, 0.7, 0.6, 0.9 );
legend->SetTextSize(0.035);
legend->SetFillColor(0);
legend->AddEntry(graph1, "I open", "P");
legend->AddEntry(graph2, "I dark", "P");
legend->Draw("same");

NanoUVCommon::addNanoUVLabel( c1, 4 );

graph1->SetLineColor(46);
graph1->SetMarkerColor(46);
graph1->SetMarkerSize(1);
graph1->SetMarkerStyle(20);
graph1->Draw("P same");

graph2->SetLineColor(38);
graph2->SetMarkerColor(38);
graph2->SetMarkerSize(1);
graph2->SetMarkerStyle(20);
graph2->Draw("P same");

c1->SaveAs("I-V.pdf");


TCanvas* c2 = new TCanvas("c2", "", 500, 500);
c2->cd();
c2->SetLogy();

delete h2_axes;
h2_axes = new TH2D( "axes", "", 10, -10., 400., 10, 0.5, 4000. );
h2_axes->SetXTitle( "APD HV [V]" );
h2_axes->SetYTitle( "APD Gain" );
//h2_axes->GetYaxis()->SetNoExponent();
//h2_axes->GetYaxis()->SetMoreLogLabels();
h2_axes->Draw();

double v_a1, v_a2, Iopen0, Idark0;
graph1->GetPoint(a, v_a1, Iopen0);
graph2->GetPoint(a, v_a2, Idark0);

for (unsigned i=0; i<graph1->GetN(); ++i) {

   int k = graphGAIN->GetN();
   double x1, IopenV, x2, IdarkV;

   graph1->GetPoint(i, x1, IopenV);
   graph2->GetPoint(i, x2, IdarkV);
   graphGAIN->SetPoint(k, x1, (IopenV-IdarkV)/(Iopen0-Idark0));

   }


NanoUVCommon::addNanoUVLabel( c2, 2 );

graphGAIN->SetLineColor(46);
graphGAIN->SetMarkerColor(46);
graphGAIN->SetMarkerSize(1);
graphGAIN->SetMarkerStyle(20);
graphGAIN->Draw("P same");

c2->SaveAs("Gain.pdf");   

delete c1;
delete c2;
delete graph1;
delete graph2;
delete graphGAIN;
return 0;
}

