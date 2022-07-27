// This code generate a tuple with all the events of experimental data
// Saves the necesary variables, also sabes the sum on the Zh if the event has more than one pion
// For events with more than one pion saves the variables of the more energetic pion
// The code require that you have the number of the event saved in the data tupleName
// if you don't have it you can check by for the paricle has the same Q2 and Nu instead
// It can be compiled with
// g++ -Wall -fPIC -I./include `root-config --cflags` MoreEnergy.cpp -o ./bin/MoreEnergy `root-config --glibs`
// For the target name use (C,Fe,Pb)

#include <iostream>
#include <string>
#include "Binning.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TVector2.h"
#include "TStopwatch.h"
#include "TROOT.h"

int main(int argc, char* argv[]) {

  if(argc != 2) {
    std::cout << "Insert (just) the target name as a parameter" << std::endl;
    return 0;
  }

  TStopwatch t;
  std::cout << "Start" << std::endl;

  std::string target = argv[1];
  // Creating a array of chars instead of a string to use Form method
  int n = target.length();
  char targetArr[n + 1];
  strcpy(targetArr, target.c_str());

  TFile* file = new TFile(Form("~/proyecto/MoreEnergy/Data/PiPlusData_%s.root", targetArr), "READ");
  TNtuple* tuple = (TNtuple*)file->Get("ntuple_data");

  int tmpCounter = 0; // Counts how many particles there is in the event
  float tmpEvnt, evnt, Q2Evnt, NuEvnt, ZhEvnt, Pt2Evnt, PhiEvnt, YCEvnt, VCEvnt;
  float tmpZh[5], tmpPt2[5], tmpPhi[5] ;

  const char* VarList = "Q2:Nu:Zh:Pt2:PhiPQ:YC:VC_TM:ZhSum";
  // Variables to fill the tuple
  float *vars         = new Float_t[8];
  // Read the necesary variables
  tuple->SetBranchAddress("Q2",&Q2Evnt);
  tuple->SetBranchAddress("Nu",&NuEvnt);
  tuple->SetBranchAddress("Zh",&ZhEvnt);
  tuple->SetBranchAddress("Pt2",&Pt2Evnt);
  tuple->SetBranchAddress("PhiPQ",&PhiEvnt);
  tuple->SetBranchAddress("YC",&YCEvnt);
  tuple->SetBranchAddress("VC_TM",&VCEvnt);
  tuple->SetBranchAddress("NEvnt",&evnt);

  gROOT->cd();

  TNtuple* ntuplePion[5];

  for(int i = 0; i < 5; i++) {
    ntuplePion[i] = new TNtuple(Form("ntuple_%i_pion", i + 1), "", VarList);
  }

  for(int i = 0; i < tuple->GetEntries() ; i++) { // Loops in every detected paricle
    tuple->GetEntry(i);
    vars[0] = Q2Evnt;
    vars[1] = NuEvnt;
    vars[2] = ZhEvnt;
    vars[3] = Pt2Evnt;
    vars[4] = PhiEvnt;
    vars[5] = YCEvnt;
    vars[6] = VCEvnt;
    vars[7] = ZhEvnt;
    tmpZh[0]  = ZhEvnt;
    tmpPt2[0] = Pt2Evnt;
    tmpPhi[0] = PhiEvnt;
    tmpEvnt   = evnt;
    tuple->GetEntry(i + 1);
    while(tmpEvnt == evnt) { // Check all the paricles in the event
      tmpCounter++;
      tmpZh[tmpCounter]  = ZhEvnt;
      tmpPt2[tmpCounter] = Pt2Evnt;
      tmpPhi[tmpCounter] = PhiEvnt;
      if(i + 1 + tmpCounter >= tuple->GetEntries() ){ break; }
      tuple->GetEntry(i + 1 + tmpCounter);
    }
    if(tmpCounter == 0) {
      //tuple->GetEntry(i);
      ntuplePion[0]->Fill(vars);
    }else {
      vars[2] = 0;
      vars[7] = 0;
      int pos = -1;
      for(int k = 0; k <= tmpCounter; k++) {
        if(vars[2] < tmpZh[k]) {
          vars[2] = tmpZh[k];
          pos = k;
        }
        vars[7] += tmpZh[k];
      }
      // Save the Pt2 of the sum vector
      vars[2] = tmpZh[pos];
      vars[3] = tmpPt2[pos];
      vars[4] = tmpPhi[pos];
      pos = -1;
      ntuplePion[tmpCounter]->Fill(vars);
    }
    // Jump to the next event
    i += tmpCounter;
    tmpCounter = 0;
  } // End paricle loop

  // Save the tuples
  TFile* fOutput = new TFile(Form("~/proyecto/MoreEnergy/Data/MaxE_%s.root", targetArr), "RECREATE");
  fOutput->cd();

  for(int i = 0; i < N_PION + 1 ; i++) {
    ntuplePion[i]->Write();
  }

  gROOT->cd();
  fOutput->Close();
  std::cout << "Done." << std::endl;
  file->Close();
  t.Print();

}
