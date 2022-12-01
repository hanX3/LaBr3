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
    return -1;
  }

  if(argc!=2){
    std::cout << "need parameter " << std::endl;
    std::cout << "like: ./raw2root 3" << std::endl;
    std::cout << "means analysis run 3" << std::endl;
    return -1;
  }

  int run_num = atoi(argv[1]);

  //
  char raw_name[1024];
  char root_name[1024];

  //
  FILE *raw_file;
  TFile *fi;
  TTree *tr;

  short header;
  short board;
  short channel;
  unsigned long time_stamp;
  unsigned short energy_ch;
  double energy_keV;
  unsigned short energy_short;
  uint32_t energy_short_flag;
  char waveform_code;
  unsigned int size;
  short data[10000];
  short dt[10000];
  long long n;

  //
  for(int i=0;i<10000;i++){
    dt[i] = i;
  }
  // for(;;;){ // loop for diff channel
    // sprintf(raw_name, "%s/run_%d/%s%d.BIN", FILE_PATH, run_num, FILE_HEAD, run_num);
  // }

  sprintf(raw_name, "%s/run_%d/%s%d.BIN", FILE_PATH, run_num, FILE_HEAD, run_num);
  std::cout << "analysis " << raw_name << std::endl;

  if((raw_file=fopen(raw_name,"rb")) == NULL){
    std::cout << "can not open " << raw_name << std::endl;
    return -1;
  }

  // flag
  const static unsigned int mask_energy_ch = 0x00000001;
  const static unsigned int mask_energy_keV = 0x00000002;
  const static unsigned int mask_energy_short_ch = 0x00000004;
  const static unsigned int mask_waveform = 0x00000008;
  bool flag_energy_ch = 0;
  bool flag_energy_keV = 0;
  bool flag_energy_short_ch = 0;
  bool flag_waveform = 0;

  fread(&header, 2, 1, raw_file);
  if((header&mask_energy_ch)==mask_energy_ch) flag_energy_ch = 1;
  if((header&mask_energy_keV)==mask_energy_keV) flag_energy_keV = 1;
  if((header&mask_energy_short_ch)==mask_energy_short_ch) flag_energy_short_ch = 1;
  if((header&mask_waveform)==mask_waveform) flag_waveform = 1;

  // std::cout << "flag_energy_ch " << flag_energy_ch << std::endl;
  // std::cout << "flag_energy_keV " << flag_energy_keV << std::endl;
  // std::cout << "flag_energy_short_ch " << flag_energy_short_ch << std::endl;
  // std::cout << "flag_waveform " << flag_waveform << std::endl;


  sprintf(root_name,"run%04d.root",run_num);
  fi = new TFile(root_name, "recreate");
  tr = new TTree("tr", "DT5730_DPP_PSD");
  tr->Branch("board", &board, "board/S");
  tr->Branch("channel", &channel, "channel/S");
  tr->Branch("time_stamp", &time_stamp, "time_stamp/l");
  if(flag_energy_ch){
    tr->Branch("energy_ch", &energy_ch, "energy_ch/s");
  }
  if(flag_energy_keV){
    tr->Branch("energy_keV", &energy_keV, "energy_keV/l");
  }
  if(flag_energy_short_ch){
    tr->Branch("energy_short", &energy_short, "energy_short/s");
  }
  if(flag_waveform){
    tr->Branch("size", &size, "size/i");
    tr->Branch("data", data, "data[size]/S");
    tr->Branch("dt", dt, "dt[size]/S");
  }
  tr->Branch("n", &n, "n/L");

  //
  n = 0;
  for(int i=0;i<10000;i++)  dt[i] = i;
  while(!feof(raw_file)){
    if(n%10000==0){
      std::cout << n << std::endl;
    }

    fread(&board, 2, 1, raw_file);
    fread(&channel, 2, 1, raw_file);
    fread(&time_stamp, 8, 1, raw_file);

    // std::cout << "board " << std::hex << board << std::endl;
    // std::cout << "channel " << channel << std::endl;
    // std::cout << "time_stamp " << time_stamp << std::endl;

    if(flag_energy_ch){
      fread(&energy_ch, 2, 1, raw_file);
      // std::cout << "energy_ch " << energy_ch << std::endl;
    }

    if(flag_energy_keV){
      fread(&energy_keV, 8, 1, raw_file);
      // std::cout << "energy_keV " << energy_keV << std::endl;
    }

    if(flag_energy_short_ch){
      fread(&energy_short, 2, 1, raw_file);
      fread(&energy_short_flag, 4, 1, raw_file);
      // std::cout << "energy_short " << energy_short << std::endl;
      // std::cout << "energy_short_flag " << energy_short_flag << std::endl;
    }

    if(flag_waveform){
      fread(&waveform_code, 1, 1, raw_file);
      fread(&size, 4, 1, raw_file);
      fread(data, 2, size, raw_file);
      // std::cout << "waveform_code " << std::dec << waveform_code << std::endl;
      // std::cout << "size " << size << std::endl;
    }

    tr->Fill();

    n++;
    if(feof(raw_file)) break;
  }

  fclose(raw_file);
  fi->cd();
  tr->Write();
  fi->Close();

  // test
  // short header;
  // fread(&header, 2, 1, raw_file);
  // fread(&board, 2, 1, raw_file);
  // fread(&channel, 2, 1, raw_file);
  // fread(&time_stamp, 8, 1, raw_file);


  // std::cout << "header " << std::hex << header << std::endl;
  // std::cout << "board " << std::hex << board << std::endl;
  // std::cout << "channel " << channel << std::endl;
  // std::cout << "time_stamp " << time_stamp << std::endl;

  // std::cout << "mask_energy_ch " << (header&mask_energy_ch) << std::endl; 
  // std::cout << "mask_energy_keV " << (header&mask_energy_keV) << std::endl;
  // std::cout << "mask_energy_short_ch " << (header&mask_energy_short_ch) << std::endl;
  // std::cout << "mask_waveform " << (header&mask_waveform) << std::endl;


  return 0;
}