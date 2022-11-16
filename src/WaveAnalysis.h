#ifndef WAVEANALYSIS_H_
#define WAVEANALYSIS_H_

#include "set.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TBenchmark.h"
#include "TMultiGraph.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <algorithm>

struct MA_PAR{
  UInt_t   ma_length;      // number of points for move average
};


//
Double_t MA(Double_t *v, UInt_t i, UInt_t window);


//
class WaveAnalysis
{
public:
  WaveAnalysis() = default;
  WaveAnalysis(const std::string &filename_in, const std::string &filename_out, int a, int b);
  ~WaveAnalysis();

public:
  void Process();
  bool ProcessEntry(Long64_t n);
#ifdef DEBUGDRAWOPTION
  void DrawEntry(Long64_t n);
#endif

private:
  bool GetWave(Long64_t n);

private:
  void PolarityWave();  // polarity fix
  void BaselineWave();  // baseline fix
  void MAFilterWave();  // ma filter

private:
  Int_t entry_start;
  Int_t entry_stop;

private:
  Double_t baseline;
  Double_t b_std_sqrt;
  Double_t energy;
  
  MA_PAR ma_par;

  UShort_t ltra;
  UShort_t data[MAXLENGTH];
  Double_t data_pol[MAXLENGTH];
  Double_t data_bl[MAXLENGTH];
  Double_t data_ma[MAXLENGTH];
  
private:
  TBenchmark *benchmark;

  TFile *file_in;
  TTree *tr_in;
  Long64_t total_entry;

  TFile *file_out;
  TTree *tr_out;

#ifdef DEBUGDRAWOPTION
  TCanvas *cav1;
  TCanvas *cav2;
#endif
};

#endif
