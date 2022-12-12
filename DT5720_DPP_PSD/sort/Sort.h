#ifndef SORT_H_
#define SORT_H_

#include "set.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h> //memset()
#include <stdlib.h> //malloc()

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TBenchmark.h"

void GetCaliPar(Double_t p[MAX_CHANNELS][3]);
void GetTSOffset(Double_t p[MAX_CHANNELS]);

typedef struct {
  Short_t board;
  Short_t channel;
  Double_t energy;
  Long64_t timestamp;
} Data;

//
class Sort
{
public:
  Sort() = default;
  Sort(const std::string &filename_in, const std::string &filename_out);
  ~Sort();

public:
  void Process();

private:
  Double_t par[MAX_CHANNELS][3];
  Double_t ts_offset[MAX_CHANNELS];

public:
  Data d;
  UInt_t size;
  Short_t data[MAX_SAMPLES];
  Short_t dt[MAX_SAMPLES];
  Long64_t n;

private:
  TBenchmark *benchmark;
  TFile *file_in;
  TFile *file_out;
  TTree *tr_out;
};

#endif
