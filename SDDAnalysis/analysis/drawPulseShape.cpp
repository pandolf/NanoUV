#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"




int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./checkPulseShape [rootFileName] [event]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  int event  (atoi(argv[2]));


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check pulse shape of event: " << event << std::endl;

  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();

  int ev;
  float pshape[1024];

  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "pshape", pshape );

  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );

  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    if( ev!=event ) continue;

    for( unsigned i=0; i<1024; ++i )  {
      std::cout << pshape[i] << std::endl;
      h1->SetBinContent( i+1, pshape[i] );
    }

  } // for entries


  h1->Draw();
  
  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  c1->SaveAs(Form("pulseShape_ev%d.pdf",event));
  //c1->SaveAs(Form("%s/pulseShape_ev%d.pdf",plotsDir.c_str(),event));

  return 0;

}
