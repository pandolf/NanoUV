#include "../interface/GunScanTool.h"

#include <fstream>

#include "TF1.h"



GunScanTool::GunScanTool( float gunEnergy, float APDhv, const std::string& data ) {

  gunEnergy_ = gunEnergy;
  APDhv_ = APDhv;
  data_ = data;
  currentMethod_ = "max";

  firstN_fit_ = 7;
  lastN_fit_ = 5;

}



// ------ SETTERS

void GunScanTool::set_gunEnergy( float gunEnergy ) {

  gunEnergy_ = gunEnergy;

}


void GunScanTool::set_APDhv( float APDhv ) {

  APDhv_ = APDhv;

}


void GunScanTool::set_currentMethod( const std::string& currentMethod ) {

  currentMethod_ = currentMethod;

}

void GunScanTool::set_data( const std::string& data ) {

  data_ = data;

}


void GunScanTool::set_firstN_fit( int firstN_fit ) {

  firstN_fit_ = firstN_fit;

}


void GunScanTool::set_lastN_fit( int lastN_fit ) {

  lastN_fit_ = lastN_fit;

}





// ------ GETTERS

float GunScanTool::gunEnergy() const {

  return gunEnergy_;

}


float GunScanTool::APDhv() const {

  return APDhv_;

}


std::string GunScanTool::currentMethod() const {

  return currentMethod_;

}


std::string GunScanTool::data() const {

  return data_;

}


int GunScanTool::firstN_fit() const {

  return firstN_fit_;

}


int GunScanTool::lastN_fit() const {

  return lastN_fit_;

}




// ---- OTHER FUNCTIONS


TGraph* GunScanTool::getScanFromFile( const std::string& fileName, float gunEnergy ) {

  TGraph* graph = new TGraph(0);
  graph->SetName( Form( "gr_%s", fileName.c_str()) );

  std::string fullFileName( Form("data/%s/%s", data_.c_str(), fileName.c_str()) );
  std::ifstream ifs( fullFileName.c_str() );

  std::cout << "-> Opening: " << fullFileName.c_str() << std::endl;

  while( ifs.good() ) {

    double x_gun, i_apd;
    ifs >> x_gun >> i_apd;

    int iPoint = graph->GetN();
    graph->SetPoint( iPoint, x_gun*201/gunEnergy_, -i_apd*1E9 ); // make positive and convert to nA

  }

  return graph;

}




TGraph* GunScanTool::getCorrectedGraph( TGraph* graph ) {

  TF1* baseline = this->fitDrift( graph );

  TGraph* graph_corr = new TGraph(0);
  graph_corr->SetName( Form( "%s_corr", graph->GetName() ) );

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    double x, y;
    graph->GetPoint( iPoint, x, y );

    graph_corr->SetPoint( iPoint, x, y-baseline->Eval(x) );

  }

  return graph_corr;

}


float GunScanTool::getCurrentFromScan( TGraph* graph ) {

  return 0.;

}


TF1* GunScanTool::fitDrift( TGraph* graph ) {

  TGraph* fit_points = new TGraph(0);

  int nPoints = graph->GetN();

  for( int i=0; i<nPoints; ++i ) {

    if( (i<=firstN_fit_) || (i>=(nPoints-lastN_fit_)) ) {

      double x, y;
      graph->GetPoint( i, x, y );

      fit_points->SetPoint( fit_points->GetN(), x, y );

    } // if

  } // for

  double xFirst, xLast, yFirst, yLast;
  graph->GetPoint( 0, xFirst, yFirst );
  graph->GetPoint( nPoints-1, xLast, yLast );

  double xMin, xMax;
  if(xFirst<xLast) {
    xMin = xFirst;
    xMax = xLast;
  } else {
    xMin = xLast;
    xMax = xFirst;
  }

  TF1* baseline = new TF1( Form("line_%s", graph->GetName()), "[0] + [1]*x + [2]*x*x + [3]*x*x*x", xMin, xMax );

  fit_points->Fit( baseline, "QR" );
 
  return baseline;

}
