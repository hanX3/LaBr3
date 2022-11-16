#include "WaveAnalysis.h"

//
WaveAnalysis::WaveAnalysis(const std::string &filename_in, const std::string &filename_out, int a, int b)
{
  benchmark = new TBenchmark;

  entry_start = a;
  entry_stop = b;

  file_in = TFile::Open(filename_in.c_str());
  if(file_in->IsZombie()){
    std::cout << "can not open " << filename_in << std::endl;
  }
  tr_in = (TTree*)file_in->Get("tree");
  tr_in->SetBranchAddress("size", &ltra);
  tr_in->SetBranchAddress("data", data);
  total_entry = tr_in->GetEntries();

  memset(&ma_par, 0, sizeof(MA_PAR));

  memset(data, 0, MAXLENGTH*sizeof(UShort_t));
  memset(data_bl, 0, MAXLENGTH*sizeof(Double_t));

  ma_par.ma_length = MALENGTH;

  file_out = new TFile(filename_out.c_str(), "recreate");
  tr_out = new TTree("tr", "wave analysis info");
  tr_out->Branch("baseline", &baseline, "baseline/D");
  tr_out->Branch("b_std_sqrt", &b_std_sqrt, "b_std_sqrt/D");
  tr_out->Branch("energy", &energy, "energy/D");  

#ifdef DEBUGDRAWOPTION
  cav1 = new TCanvas("cav1", "cav1", 0, 0, 480, 360);
  cav2 = new TCanvas("cav2", "cav2", 0, 0, 480, 360);
#endif
}

//
WaveAnalysis::~WaveAnalysis()
{

}

//
bool WaveAnalysis::GetWave(Long64_t n)
{
  ltra = 0;
  memset(data, 0, sizeof(UShort_t)*MAXLENGTH);
  tr_in->GetEntry(n);


  return true;
}

//
void WaveAnalysis::PolarityWave()
{
  memset(data_pol, 0, sizeof(Double_t)*MAXLENGTH);

  for(UInt_t i=0;i<ltra;i++){
    data_pol[i] = data[i]*POLARITY;
  }
}

//
void WaveAnalysis::BaselineWave()
{
  memset(data_bl, 0, sizeof(Double_t)*MAXLENGTH);

  for(UInt_t i=0;i<ltra;i++){
    data_bl[i] = data_pol[i]-data_pol[0];
  }

  baseline = 0.;
  for(int i=0;i<PRETRIGGER;i++){
    baseline += data_bl[i];
  }

  baseline /= (Double_t)PRETRIGGER;

  b_std_sqrt = 0.;
  for(int i=0;i<PRETRIGGER;i++){
    b_std_sqrt += (data_bl[i]-baseline)*(data_bl[i]-baseline);
  }
  b_std_sqrt = sqrt(b_std_sqrt)/PRETRIGGER;
}

//
void WaveAnalysis::MAFilterWave()
{
  energy = 0.;
  memset(data_ma, 0, sizeof(Double_t)*MAXLENGTH);

  for(UInt_t i=0;i<ltra;i++){
    data_ma[i] = MA(data_bl, i, ma_par.ma_length);
  }

  energy = *std::max_element(data_ma, data_ma+ltra);
  energy -= baseline;
}

//
void WaveAnalysis::Process()
{
  benchmark->Start("analysis");

  for(Long64_t i=entry_start;i<entry_stop;i++){
    if(i==total_entry) break;
    if(i%10000==0){
    std::cout << i << "/" << total_entry << std::endl;
  }

    ProcessEntry(i);
    tr_out->Fill();
  }

  file_out->cd();
  tr_out->Write();
  file_out->Close();

  file_in->Close();

  benchmark->Show("analysis");
}

//
bool WaveAnalysis::ProcessEntry(Long64_t n)
{
  GetWave(n);
  PolarityWave();
  BaselineWave();
  MAFilterWave();

  return true;
}

//
#ifdef DEBUGDRAWOPTION
void WaveAnalysis::DrawEntry(Long64_t n)
{
  GetWave(n);
  PolarityWave();
  BaselineWave();
  MAFilterWave();

  TGraph *g1 = new TGraph(ltra);
  for(int i=0;i<ltra;i++){
    g1->SetPoint(i, (Double_t)i, (Double_t)data_bl[i]);
  }

  cav1->cd();
  g1->Draw();

  TGraph *g2 = new TGraph(ltra);
  for(int i=0;i<ltra;i++){
    g2->SetPoint(i, (Double_t)i, (Double_t)data_ma[i]);
  }

  // cav2->cd();
  g2->Draw("same");
}
#endif

//
Double_t MA(Double_t *v, UInt_t i, UInt_t window)
{
  Double_t x = 0.;
  
  if(i==0){
    return 0;
  }

  if(i<window){
    for(UInt_t j=0;j<i;j++){
      x += v[j];
    }
  }
  else{
    for(UInt_t j=(i-window);j<i;j++){
      x += v[j];
    }
  }

  return x/(Double_t)window;
}
