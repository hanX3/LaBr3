#include "Sort.h"
#include <fstream>

//
Sort::Sort(const std::string &filename_in, const std::string &filename_out)
{
  benchmark = new TBenchmark;

  file_in = TFile::Open(filename_in.c_str());
  if(file_in->IsZombie()){
    std::cout << "cannot open " << filename_in << std::endl;
  }

  file_out = TFile::Open(filename_out.c_str(), "recreate");
  tr_out = new TTree("tr", "DT5730_DPP_PSD");

  tr_out->Branch("board", &d.board, "board/S");
  tr_out->Branch("channel", &d.channel, "channel/S");
  tr_out->Branch("energy", &d.energy, "energy/D");
  tr_out->Branch("timestamp", &d.timestamp, "timestamp/L");

  memset(&d, 0, sizeof(d));
  n = 0;
  
  memset(par, 0, sizeof(par)); 
  GetCaliPar(par);
  GetTSOffset(ts_offset);

#ifdef DEBUG_SORT
  for(int i=0;i<MAX_CHANNELS;i++){
    std::cout << par[i][0] << " " << par[i][1] << " " << par[i][2] << std::endl;
  }
#endif

}

//
Sort::~Sort()
{
  file_in->Close();
  file_out->Close();
}

//
void Sort::Process()
{
  benchmark->Start("sort");

  //
  Short_t channel;
  UShort_t energy;
  Long64_t timestamp;

  Long64_t nentries[MAX_CHANNELS];
  Long64_t total_entry = 0;
  TTree *tr[MAX_CHANNELS];

  for(int i=0;i<MAX_CHANNELS;i++){
    tr[i] = (TTree*)file_in->Get(TString::Format("tr_ch%02d",i).Data());
    if(!tr[i]){
#ifdef DEBUG_SORT
      std::cout << "channel " << i << " empty tree" << std::endl;
#endif
      nentries[i] = 0;
      continue;
    }

    total_entry += tr[i]->GetEntries();
    nentries[i] = tr[i]->GetEntries();

    tr[i]->SetBranchAddress("channel", &channel);
    tr[i]->SetBranchAddress("energy_ch", &energy);
    tr[i]->SetBranchAddress("timestamp", &timestamp);
    tr[i]->SetBranchAddress("size", &size);
    tr[i]->SetBranchAddress("data", data);
    tr[i]->SetBranchAddress("dt", dt);
  }

#ifdef DEBUG_SORT
  std::cout << "total_entry " << total_entry << std::endl;
#endif

  std::vector<Data> vec_da;

  Long64_t *ts;
  ts = (Long64_t*)malloc(total_entry*sizeof(Long64_t)); 
  Int_t *ts_ch;
  ts_ch = (Int_t*)malloc(total_entry*sizeof(Int_t));
  
  total_entry = 0;
  for(int i=0;i<MAX_CHANNELS;i++){
    if(!tr[i]) continue;
    for(Long64_t j=0;j<tr[i]->GetEntries();j++){
      tr[i]->GetEntry(j);
      // may add own cfd here, to do
      // ts[total_entry] = timestamp;
      ts[total_entry] = timestamp+ts_offset[i];
      total_entry++;
    }
  }

  std::cout << "start sort ..." << std::endl;
  TMath::Sort((Int_t)total_entry, (Long64_t*)ts, (Int_t*)ts_ch, kFALSE);
#ifdef DEBUG_SORT
  for(int i=0;i<10;i++){
    std::cout << "ts " << ts[i] << " ts_ch " << ts_ch[i] << std::endl;
  }
#endif

  free(ts);
  
  Long64_t min_tag[MAX_CHANNELS], max_tag[MAX_CHANNELS];
  memset(min_tag, 0, sizeof(min_tag));
  memset(max_tag, 0, sizeof(max_tag));
  for(int i=0;i<MAX_CHANNELS;i++){
    for(int j=0;j<i;j++)  min_tag[i] += nentries[j];
    for(int j=0;j<=i;j++)  max_tag[i] += nentries[j];
  }

  Data da;
  Int_t tr_ch = 0;
  Long64_t tr_entry = 0;

  memset(&da, 0, sizeof(da));
  for(Long64_t i=0;i<total_entry;i++){
    if(i%10000==0){
      std::cout << i << "/" << total_entry << std::endl;
    }
    for(int j=0;j<MAX_CHANNELS;j++){
      if(ts_ch[i]>=min_tag[j] && ts_ch[i]<max_tag[j]){
        tr_ch = j;
        tr_entry = ts_ch[i]-min_tag[j];
        break;
      }
    }
    tr[tr_ch]->GetEntry(tr_entry);
    da.channel = channel;
    da.energy = par[channel][0]+par[channel][1]*(Double_t)energy+par[channel][2]*(Double_t)(energy*energy);
    da.timestamp = timestamp+ts_offset[tr_ch];

    vec_da.push_back(da);
  }
  free(ts_ch);

#ifdef DEBUG_SORT
  std::cout << "vec_da size " << vec_da.size() << std::endl;
  for(int i=0;i<10;i++){
    std::cout << vec_da[i].channel << " " << vec_da[i].energy << " " << vec_da[i].timestamp << std::endl;
  }
#endif

  FillTree(vec_da);
  file_out->cd();
  tr_out->Write();
  file_out->Close();

  benchmark->Show("sort");
}


//
void Sort::FillTree(std::vector<Data> &v)
{
  for(Long64_t i=0;i<v.size();i++){
    d = v[i];
    tr_out->Fill();
#ifdef DEBUG_SORT
    if(i<10){
      std::cout << "board " << d.board << std::endl;
      std::cout << "channel " << d.channel << std::endl;
      std::cout << "energy " << d.energy << std::endl;
      std::cout << "timestamp " << d.timestamp << std::endl;
      std::cout << "size " << d.size << std::endl;
      std::cout << "data " << d.data[0] << std::endl;
      std::cout << "dt " << d.dt[0] << std::endl;
    }
#endif

    memset(&d, 0, sizeof(d));
    n++;
  }
}

//
void GetCaliPar(Double_t p[MAX_CHANNELS][3])
{
  std::ifstream f;
  f.open(TString::Format("%s",LABR3_CALI_DATA).Data());
  if(!f){
    std::cout << "can not open " << LABR3_CALI_DATA << std::endl;
    return;
  }

  Int_t ch;
  Double_t par0, par1, par2;
  Double_t chi2;

  while(true){
    f >> ch >> par0 >> par1 >> par2 >> chi2;
    if(!f.good()) break;

    p[ch][0] = par0;
    p[ch][1] = par1;
    p[ch][2] = par2;
  }
  f.close();
}

//
void GetTSOffset(Double_t p[MAX_CHANNELS])
{
  std::ifstream f;
  f.open(TString::Format("%s",LABR3_TS_OFFSET_DATA).Data());
  if(!f){
    std::cout << "can not open " << LABR3_TS_OFFSET_DATA << std::endl;
    return;
  }

  Int_t ch;
  Double_t offset;

  while(true){
    f >> ch >> offset;
    if(!f.good()) break;

    p[ch] = offset;
  }
  f.close();
}
