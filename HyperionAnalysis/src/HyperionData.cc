#include "../interface/HyperionData.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>




HyperionData::HyperionData( const std::string& fileName ) {


  // default values
  led_ = "off";
  cnt_sample_ = "none";
  p_before_ = 1000.;
  p_after_ = 1000.;


  std::cout << "-> Reading data from file: " << fileName << std::endl;

  std::ifstream ifs(fileName.c_str());

  std::string line;
  while( ifs.good() ) {

    std::getline(ifs, line);

    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

    TString line_tstr(line);
    if( line_tstr.Length()<4 ) continue;

    bool isDataLine = !(line_tstr.BeginsWith("#"));

    std::string delimiter = (isDataLine) ? "\t" : " ";
    size_t pos = 0;
    std::vector<std::string> words;
    std::string word;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      word = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());
      words.push_back(word);
    }
    words.push_back(line);


    if( !isDataLine ) {

      if( words[0] == "#p_before"   ) p_before_   = std::atof( words[1].c_str() );
      if( words[0] == "#p_after"    ) p_after_    = std::atof( words[1].c_str() );
      if( words[0] == "#led"        ) led_        = words[1];
      if( words[0] == "#led_freq"   ) led_freq_   = std::atof( words[1].c_str() ); // in kHz
      if( words[0] == "#picoamm"    ) picoamm_    = words[1];
      if( words[0] == "#L"          ) L_          = std::atof( words[1].c_str() );
      if( words[0] == "#cnt_sample" ) cnt_sample_ = words[1];

      continue; // other than setting the above, skip lines which start with #

    }

  
    // from now on it's the data table

    // first round initialize the vectors
    if( data_.size()==0 ) {
      for( int i=0; i<words.size(); ++i ) {
        std::vector<float> a;
        data_.push_back( a );
      }
    }

    // then fill the data
    for( int i=0; i<words.size(); ++i )
      data_[i].push_back( std::atof(words[i].c_str()) );


  } // while ifs


}



TGraphErrors* HyperionData::getGraphFromColumns( const std::string& name, int columnMeans, int columnErrors, int nMeasurements ) const {

  if( data_.size() == 0 ) {
    std::cout << "[HyperionData::getGraphFromColumns] Can't produce a graph, data is empty!" << std::endl;
    return 0;
  }

  if( columnMeans > (data_.size()-1) ) {
    std::cout << "[HyperionData::getGraphFromColumns] Requested column (" << columnMeans << ") is out of range!" << std::endl;
    return 0;
  }

  if( columnErrors>=0 ) {
    if( columnErrors > (data_.size()-1) ) {
      std::cout << "[HyperionData::getGraphFromColumns] Requested errors column (" << columnErrors << ") is out of range!" << std::endl;
      return 0;
    }
  }


  TGraphErrors* graph = new TGraphErrors(0);
  graph->SetName( name.c_str() );

  for( unsigned iPoint=0; iPoint<data_[0].size(); ++iPoint ) {

    graph->SetPoint( iPoint, data_[0][iPoint], data_[columnMeans][iPoint] );
    if( columnErrors>=0 ) {
      if( nMeasurements>0 ) graph->SetPointError( iPoint, 0., data_[columnErrors][iPoint]/sqrt(nMeasurements) );
      else                  graph->SetPointError( iPoint, 0., data_[columnErrors][iPoint] );
    }

  } // for iPoints

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.5);

  return graph;

}
