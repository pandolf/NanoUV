#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"



bool SAVEFULLSHAPE = false;



int main( int argc, char* argv[] ) {

  if( argc != 2 ) {

    std::cout << "USAGE: ./asciiToTree [filename]" << std::endl;
    exit(1);

  }

  std::string fileName(argv[1]);

  if( fileName.rfind("./data/",0)==0 )
    fileName.erase(0, 7);

  if( fileName.rfind("data/",0)==0 )
    fileName.erase(0, 5);

  std::string fullFileName( Form("data/%s", fileName.c_str()) );
  
  std::ifstream fs(fullFileName.c_str());
  if( !fs.good() ) {
    std::cout << "-> No file called '" << fileName << "' found in 'data/'. Exiting." << std::endl;
    exit(1);
  }

  std::cout << "-> Opened ascii data file: " << fileName << std::endl;

  size_t pos = 0;
  std::string outfileName;
  if((pos = fullFileName.find(".")) != std::string::npos) {
    std::string prefix = fullFileName.substr(0, pos);
    if( SAVEFULLSHAPE ) prefix = prefix + "_SHAPE";
    outfileName = prefix + ".root";
  }


  TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
  TTree* tree = new TTree( "tree", "" );


  int ev=-1;
  float base     ;
  float vamp     ;
  float vcharge  ;
  float letime   ;
  float tetime   ;
  float ratecount;
  float delta    ; // last sample minus first sample
  float pshape   [1024];

  tree->Branch( "ev"       , &ev      , "ev/I"            );
  tree->Branch( "base"     , &base     , "base/F"     );
  tree->Branch( "vamp"     , &vamp     , "vamp/F"     );
  tree->Branch( "vcharge"  , &vcharge  , "vcharge/F"  );
  tree->Branch( "letime"   , &letime   , "letime/F"   );
  tree->Branch( "tetime"   , &tetime   , "tetime/F"   );
  tree->Branch( "ratecount", &ratecount, "ratecount/F");
  tree->Branch( "delta"    , &delta    , "delta/F"    );
  if( SAVEFULLSHAPE )
    tree->Branch( "pshape"   ,  pshape   , "pshape[1024]/F");

  

  std::string line;
  bool wasReadingEvent = false;
  bool readyForPulseShape = false;


  if( fs.good() ) {

    std::cout << "-> Starting parsing file." << std::endl;

    while( getline(fs,line) ) {

      std::string delimiter = " ";
      size_t pos = 0;
      std::vector<std::string> words;
      std::string word;
      while ((pos = line.find(delimiter)) != std::string::npos) {
        word = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        words.push_back(word);
      }

      if( words[0]=="===" && words[1]=="EVENT" && wasReadingEvent ) {

        if( ev % 1000 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;

        tree->Fill();
 
        wasReadingEvent = false;

      } else if( words[0]=="===" && words[1]=="CH:" ) {

        wasReadingEvent = true;
        readyForPulseShape = true;

        base      = atof(words[8].c_str());
        vamp      = atof(words[11].c_str());
        vcharge   = atof(words[14].c_str());
        letime    = atof(words[17].c_str());
        tetime    = atof(words[20].c_str());
        ratecount = atof(words[23].c_str());

      } else if( readyForPulseShape ) {
  
        for( unsigned i=0; i<words.size(); ++i ) 
          pshape[i] = atof(words[i].c_str());

        delta = pshape[1023] - pshape[0];

        readyForPulseShape = false;
   
      }

      if( words[0]=="===" && words[1]=="EVENT" && wasReadingEvent==false) {
        ev = atoi(words[2].c_str());
      }

    } // while get lines

  } // if file is good

  if( wasReadingEvent )
    {
      std::cout << "   ... analyzing event: " << ev << std::endl;
      tree->Fill();
    }

  fs.close();

  tree->Write();
  outfile->Close();

  std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;

  return 0;

}

