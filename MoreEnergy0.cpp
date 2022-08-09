// This code saves the variable that we are goint to use of the simultion
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

  TFile* file = new TFile(Form("~/proyecto/MoreEnergy/Data/%s1_simul.root", targetArr), "READ");
  TNtuple* ntupleThrown = (TNtuple*) file->Get("ntuple_thrown");
  TNtuple* ntupleAccept = (TNtuple*) file->Get("ntuple_accept");

  const char* VarListThrown = "Q2:Nu:Zh:Pt2:PhiPQ:Xf:deltaZ";
  const char* VarListAccept = "Q2:Nu:Zh:Pt2:PhiPQ:YC:Xf:deltaZ";
  // Variables to fill the tuple
  float *varsAccept         = new Float_t[8];
  float *varsThrown         = new Float_t[7];
  // Read the necesary variables
  ntupleThrown->SetBranchAddress("Q2",     &varsThrown[0]);
  ntupleThrown->SetBranchAddress("Nu",     &varsThrown[1]);
  ntupleThrown->SetBranchAddress("Zh",     &varsThrown[2]);
  ntupleThrown->SetBranchAddress("Pt2",    &varsThrown[3]);
  ntupleThrown->SetBranchAddress("PhiPQ",  &varsThrown[4]);
  ntupleThrown->SetBranchAddress("Xf",     &varsThrown[5]);
  ntupleThrown->SetBranchAddress("deltaZ", &varsThrown[6]);

  gROOT->cd();

  TNtuple*  ntupleAcceptNew = new TNtuple("ntuple_accept", "", VarListAccept);
  TNtuple*  ntupleThrownNew = new TNtuple("ntuple_thrown", "", VarListThrown);


  for(int i = 0; i < ntupleThrown->GetEntries() ; i++) { // Loops in every detected paricle
    ntupleThrown->GetEntry(i);
    ntupleThrownNew->Fill(varsThrown);
  } // End paricle loop

  ntupleAccept->SetBranchAddress("Q2",&varsAccept[0]);
  ntupleAccept->SetBranchAddress("Nu",&varsAccept[1]);
  ntupleAccept->SetBranchAddress("Zh",&varsAccept[2]);
  ntupleAccept->SetBranchAddress("Pt2",&varsAccept[3]);
  ntupleAccept->SetBranchAddress("PhiPQ",&varsAccept[4]);
  ntupleAccept->SetBranchAddress("YC",&varsAccept[5]);
  ntupleAccept->SetBranchAddress("Xf",&varsAccept[6]);
  ntupleAccept->SetBranchAddress("deltaZ",&varsAccept[7]);

  for(int i = 0; i < ntupleAccept->GetEntries() ; i++) { // Loops in every detected paricle
    ntupleAccept->GetEntry(i);
    ntupleAcceptNew->Fill(varsAccept);
  } // End paricle loop
  // Save the tuples
  TFile* fOutput = new TFile(Form("~/proyecto/MoreEnergy/Data/%s1_simul2.root", targetArr), "RECREATE");
  fOutput->cd();

  ntupleThrownNew->Write();
  ntupleAcceptNew->Write();

  gROOT->cd();
  fOutput->Close();
  std::cout << "Done." << std::endl;
  file->Close();
  t.Print();

}
