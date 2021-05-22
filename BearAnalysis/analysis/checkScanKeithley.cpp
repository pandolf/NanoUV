#include <iostream>
#include <fstream>
#include <vector>
#include <string>



int checkFile( const std::string& fileName );



int main() {


  std::ifstream ifs_allFiles("fileList.txt");
  std::string line;
  line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

  std::ofstream ofs_ok;
  std::ofstream ofs_bad;
  std::ofstream ofs_boh;

  ofs_ok .open("scans_ok.txt" );
  ofs_bad.open("scans_bad.txt");
  ofs_boh.open("scans_boh.txt");


  while( getline(ifs_allFiles,line) ) {

    int check = checkFile(line);

    if( check == 1 ) 
      ofs_ok << line << std::endl;
    else if( check==-1 )
      ofs_bad << line << std::endl;
    else if( check==0 )
      ofs_boh << line << std::endl;

  } // while lines

  ofs_ok .close();
  ofs_bad.close();
  ofs_boh.close();


  return 0;

}



int checkFile( const std::string& fileName ) {

  std::ifstream ifs(fileName.c_str());

  int returnInt = 0;
     
  std::string line;
  bool keithleyB = false;

  while( getline(ifs,line) ) {

    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    std::string thisLine = line;

    std::string delimiter = " ";
    size_t pos = 0;
    std::vector<std::string> words;
    std::string word;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      word = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());
      words.push_back(word);
    }


    if( thisLine=="Instrument 1: AMMETER KEITHLEY 6517A B" ) {

      keithleyB = true;

    } else if( thisLine=="Instrument 2: AMMETER KEITHLEY 6517A A" ) {

      break;

    } else if( keithleyB && thisLine=="Vsource: OFF" ) {

      returnInt = -1;

    } else if( keithleyB && thisLine=="Vsource: ON" ) {

      returnInt = 1;

    }

  } // while line

  return returnInt;

} // checkFile
