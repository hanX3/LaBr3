#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <string>
#include <cstring>

#include "TFile.h"
#include "TTree.h"

#include "set.h"

//
int main(int argc, char *argv[])
{
  // check
  if(sizeof(char)!=1 || sizeof(short)!=2 || sizeof(int)!=4 || sizeof(long)!=8){
    std::cout << "sizeof(char) = " << sizeof(char) << std::endl;
    std::cout << "sizeof(short) = " << sizeof(short) << std::endl;
    std::cout << "sizeof(int) = " << sizeof(int) << std::endl;
    std::cout << "sizeof(long) = " << sizeof(long) << std::endl;
    std::cout << "The current compiler is not suitable for running the program！" << std::endl;
    return 0;
  }

  //
  char file_name[256];

  TFile *fi;
  TTree *tr;

  short ch;
  unsigned int time_tag;
  unsigned short charge_short;
  unsigned short charge_long;
  uint32_t Format;
  uint32_t Extras;
  unsigned long time_stamp;
  short ft;
  short size;
  short sample[10000];
  short wave_data[10000];
}