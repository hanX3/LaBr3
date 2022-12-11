#ifndef ALIGNMENT_H_
#define ALIGNMENT_H_

#include "set.h"

#include "TBenchmark.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include <vector>
#include <iostream>

//
class Alignment
{
public:
  Alignment() = default;
  Alignment(const std::string &filename_in, const std::string &filename_out);
  ~Alignment();

public:
  void Process();


private:
  void GetTimestampVector(TTree *tr, std::vector<Long64_t> &vec);

private:
  TBenchmark *benchmark;

  TFile *file_in;
  TTree *tr[MAX_CHANNELS];

  Short_t channel;
  Long64_t timestamp;

private:
  int ii; // the first channel  

private:
  TFile *file_out;
  TH1D *h1[MAX_CHANNELS-1];//channel vs. channel

};

#endif
