#include "Alignment.h"

#include <vector>
#include "math.h"

//
Alignment::Alignment(const std::string &filename_in, const std::string &filename_out)
{
  benchmark = new TBenchmark;

  file_in = TFile::Open(filename_in.c_str());
  for(int i=0;i<MAX_CHANNELS;i++){
    tr[i] = (TTree*)file_in->Get(TString::Format("tr_ch%02d",i).Data());
    if(!tr[i]){
      continue;
    }
    tr[i]->SetBranchAddress("channel", &channel);
    tr[i]->SetBranchAddress("timestamp", &timestamp);
  }

  channel = 0;
  timestamp = 0;

  file_out = TFile::Open(filename_out.c_str(), "recreate");
  
  ii = 0;
  int jj = 0; // total channel numbers
  for(int i=0;i<MAX_CHANNELS;i++){
    if(tr[i]){
      ii = i;
      jj++;
      break;
    }else continue;
  }

#ifdef DEBUG_ALIGNMENT
  std::cout << "ii " << ii << std::endl;
  std::cout << "jj " << jj << std::endl;
#endif

  for(int i=ii+1;i<MAX_CHANNELS;i++){
    if(!tr[i]) continue;
    h1[i-ii-1] = new TH1D(TString::Format("h_ch%02d_ch%02d",ii,i).Data(), TString::Format("h_ch%02d_ch%02d",ii,i).Data(), 2*ALIGNMENT_WINDOW/100, -ALIGNMENT_WINDOW/1000, ALIGNMENT_WINDOW/1000);
  }
}

//
Alignment::~Alignment()
{
  file_in->Close();
  file_out->Close();
}

//
void Alignment::GetTimestampVector(TTree *tr, std::vector<Long64_t> &vec)
{
  for(Long64_t i=0;i<tr->GetEntries();i++){
  // for(Long64_t i=0;i<10;i++){
    tr->GetEntry(i);
#ifdef DEBUG_ALIGNMENT
    if(i<10){
      std::cout << "ch " << channel << std::endl;
      std::cout << "timestamp " << timestamp << std::endl;      
    }
#endif

    vec.push_back(timestamp);
  }
}

//
void Alignment::Process()
{
  std::vector<Long64_t> v1;
  std::vector<Long64_t> v2;
  
  v1.clear();
  v2.clear();

  Long64_t ts1, ts2;
  Long64_t j = 0;
  Long64_t k = 0;

  //channel vs. channel
  std::cout << "channel vs. channel" << std::endl;
  GetTimestampVector(tr[ii], v1);
  for(int i=ii+1;i<MAX_CHANNELS;i++){
    v2.clear();
    j = 0;
    k = 0;

    if(!tr[i]) continue;
    GetTimestampVector(tr[i], v2);
    std::cout << v1.size() << " " << v2.size() << std::endl;
    while(true){
      if(j==(Long64_t)v1.size() || k==(Long64_t)v2.size())  break;
      ts1 = v1[j];
      while(true){
        if(k==(Long64_t)v2.size())  break;
        ts2 = v2[k];
#ifdef DEBUG_ALIGNMENT
        std::cout << j << " ts1 " << ts1 << std::endl;
        std::cout << k << " ts2 " << ts2 << std::endl;
        std::cout << "ts1-ts2 " << ts1-ts2 << std::endl;
#endif
        if(abs(ts1-ts2)<ALIGNMENT_WINDOW){
#ifdef DEBUG_ALIGNMENT
          std::cout << "1" << std::endl;
#endif
          h1[i-ii-1]->Fill((ts1-ts2)/1000.);
          k++;
        }
        else if((ts1-ts2)>ALIGNMENT_WINDOW){
#ifdef DEBUG_ALIGNMENT
          std::cout << "2" << std::endl;
#endif
          k++;
        }
        else{
#ifdef DEBUG_ALIGNMENT
          std::cout << "3" << std::endl;
#endif
          j++;
          break;
        }
      }
    }
  }

  file_out->cd();
  for(int i=ii+1;i<MAX_CHANNELS;i++){
    if(!tr[i]) continue;
    h1[i-ii-1]->Write();
  }
}

