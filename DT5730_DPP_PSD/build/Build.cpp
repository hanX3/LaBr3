#include "Build.h"

//
Build::Build(const std::string &filename_in, const std::string &filename_out)
{
  benchmark = new TBenchmark;

  file_in = TFile::Open(filename_in.c_str());
  if(file_in->IsZombie()){
    std::cout << "cannot open " << filename_in << std::endl;
  }

  tr_in = (TTree*)file_in->Get("tr");
  if(tr_in){
    tr_in->SetBranchAddress("board", &da.board);
    tr_in->SetBranchAddress("channel", &da.channel);
    tr_in->SetBranchAddress("energy", &da.energy);
    tr_in->SetBranchAddress("timestamp", &da.timestamp);

#ifdef DEBUG_BUILD
    std::cout << " entry " << tr_in->GetEntries() << std::endl;
#endif
  }
  
  memset(&da, 0, sizeof(da));
  memset(&ev, 0, sizeof(ev));

  file_out = TFile::Open(filename_out.c_str(), "recreate");
  tr_out = new TTree("tr", "DT5730_DPP_PSD");
  tr_out->Branch("m", &ev.m, "m/s");
  tr_out->Branch("board", ev.board, "board[m]/S");
  tr_out->Branch("channel", ev.channel, "channel[m]/S");
  tr_out->Branch("energy", ev.energy, "energy[m]/D");
  tr_out->Branch("timestamp", ev.timestamp, "timestamp[m]/L");
}

//
Build::~Build()
{
  file_in->Close();
}

//
void Build::Process()
{
  benchmark->Start("build"); 
  
  //
  Long64_t ts_pre;
  tr_in->GetEntry(0);
  ts_pre = da.timestamp;
  
  ev.board[ev.m] = da.board;
  ev.channel[ev.m] = da.channel;
  ev.energy[ev.m] = da.energy;
  ev.timestamp[ev.m] = da.timestamp;
  ev.m++;

  for(Long64_t i=1;i<tr_in->GetEntries();i++){
    if(ev.m > MAX_MULTI){
      std::cout << "multi too large " << ev.m << std::endl;
      break;
    }

    memset(&da, 0, sizeof(da));
    tr_in->GetEntry(i);
#ifdef DEBUG_BUILD
    if(i<10){
      std::cout << "board " << da.board << std::endl;
      std::cout << "channel " << da.channel << std::endl;
      std::cout << "energy " << da.energy << std::endl;
      std::cout << "timestamp " << da.timestamp << std::endl;

      std::cout << "delta ts " << (da.timestamp-ts_pre) << std::endl;
    }
#endif

    if((da.timestamp-ts_pre)/1000. < COINCEDENCE_WINDOWS){
      ev.board[ev.m] = da.board;
      ev.channel[ev.m] = da.channel;
      ev.energy[ev.m] = da.energy;
      ev.timestamp[ev.m] = da.timestamp;
      ev.m++;
    }
    else{
      tr_out->Fill();
      ts_pre = da.timestamp;
      memset(&ev, 0, sizeof(ev));

      ev.board[ev.m] = da.board;
      ev.channel[ev.m] = da.channel;
      ev.energy[ev.m] = da.energy;
      ev.timestamp[ev.m] = da.timestamp;
      ev.m++;
    }
  }

  file_out->cd();
  tr_out->Write();
  file_out->Close();
  benchmark->Show("build"); 
}
