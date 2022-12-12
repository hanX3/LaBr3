#ifndef BUILD_H_
#define BUILD_H_

#include "set.h"

#include "TBenchmark.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <stdlib.h>
#include <vector>
#include <iostream>

//
typedef struct {
  Short_t board;
  Short_t channel;
  Double_t energy;
  Long64_t timestamp;
} Data;

typedef struct {
  UShort_t m; // multi
  Short_t board[MAX_MULTI];
  Short_t channel[MAX_MULTI];
  Double_t energy[MAX_MULTI];
  Long64_t timestamp[MAX_MULTI];
} Event;

//
class Build
{
public:
  Build() = default;
  Build(const std::string &filename_in, const std::string &filename_out);
  ~Build();

public:
  void Process();

private:
  Data da;
  Event ev;

private:
  TBenchmark *benchmark;

  TFile *file_in;
  TTree *tr_in;

  TFile *file_out;
  TTree *tr_out;
};


#endif
