#include "WaveAnalysis.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>

#include "TString.h"
#include "TApplication.h"

int main(int argc, char *argv[])
{
  if(argc != 4){
    std::cout << "need parameter" << std::endl;
    std::cout << "like: ana 39 0 1000" << std::endl;
    std::cout << "means analysi file 39 entry 0 to 1000 " << std::endl;
    return -1;
  }

  int run = atoi(argv[1]);
  TString file_in = TString::Format("../raw2root_DT5730_DPP_PSD/run%04d.root", run);
  std::cout << "analysis " << file_in << std::endl;
  
  int entry_start = atoi(argv[2]);
  int entry_stop = atoi(argv[3]);
  TString file_out = TString::Format("./data_R%04d_%09d_%09d_ana.root", run, entry_start, entry_stop);

  WaveAnalysis *wa = new WaveAnalysis(file_in.Data(), file_out.Data(), entry_start, entry_stop);
  
  wa->Process();
  delete wa;

  return 0;
}
