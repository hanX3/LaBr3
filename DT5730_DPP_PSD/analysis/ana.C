#define ana_cxx
#include "ana.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void ana::Loop()
{
//   In a ROOT session, you can do:
//      root> .L ana.C
//      root> ana t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

void ana::TimeResolution()
{
   TH1D *h1 = new TH1D("h1", "h1", 2000, -100, 100);
   Double_t delta_t;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      
      if(m != 2)  continue;
      if(channel[0]==2 && ((energy[0]>592 && energy[0]<611) ||  (energy[0]>674 && energy[0]<693)) && 
         channel[1]==3 && ((energy[1]>1105 && energy[1]<1137) ||  (energy[1]>1253 && energy[1]<1290))){
         delta_t = (timestamp[1]-timestamp[0])/1000.;
      }
      if(channel[1]==2 && ((energy[1]>592 && energy[1]<611) ||  (energy[1]>674 && energy[1]<693)) && 
         channel[0]==3 && ((energy[0]>1105 && energy[0]<1137) ||  (energy[0]>1253 && energy[0]<1290))){
         delta_t = (timestamp[0]-timestamp[1])/1000.;
      }

      // if(channel[0]==2 && energy[0]>592 && energy[0]<611 && channel[1]==3 && energy[1]>1253 && energy[1]<1290){
      //    delta_t = (timestamp[1]-timestamp[0])/1000.;
      // }
      // if(channel[1]==2 && ((energy[1]>592 && energy[1]<611) ||  (energy[1]>674 && energy[1]<693)) && 
      //    channel[0]==3 && ((energy[0]>1105 && energy[0]<1137) ||  (energy[0]>1253 && energy[0]<1290))){
      //    delta_t = (timestamp[0]-timestamp[1])/1000.;
      // }

      // if(channel[1]==2 && energy[1]>592 && energy[1]<611 && channel[0]==3 && energy[0]>1253 && energy[0]<1290){
      //    delta_t = (timestamp[1]-timestamp[0])/1000.;
      // }
      // else continue;



      // if(channel[0]==2){
      //    delta_t = (timestamp[1]-timestamp[0])/1000.;
      // }else{
      //    delta_t = (timestamp[0]-timestamp[1])/1000.;
      // }

      h1->Fill(delta_t);
   }

   h1->Draw();
}
