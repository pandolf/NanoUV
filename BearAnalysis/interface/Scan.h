#ifndef Scan_h
#define Scan_h

#include <string>


class Scan {

 public:

  Scan( int number, int firstRegion=-1, int lastRegion=-1 );

  void setType( const std::string& type );

  int number() const;
  int firstRegion() const;
  int lastRegion() const;
  std::string type() const;

  std::string getXtitle() const;
  std::string getYtitle() const;

  int x_index() const;

  std::string scanNumberText() const;
  void recognizeScanType( const std::string& line );


 private:

  int number_;
  int firstRegion_;
  int lastRegion_;
  std::string type_;

};


#endif
