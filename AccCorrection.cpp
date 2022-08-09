// This code apply the acceptance factors to the data and
// generate a PhiPQ histogram for each bin of Q2, Nu, Zh, Pt2
// For events with more than one pion uses the pmost energetic pion of the event
// For data the tuple generate by the code VecSum.cpp
// It can be compile with
// g++ -Wall -fPIC -I./include `root-config --cflags` AccCorrection.cpp -o ./bin/AccCorrection  `root-config --glibs` ./include/Acc.h
// For the target name use (C,Fe,Pb) for the solids targets and (DC,DFe,DPb) for the liquid target

#include "Acc.h"

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

  int m;
  TString fileDataName;
  // Select the data for the chosen solid target
  if(targetArr[0] == 'D') {
    m = 2;
    char solidTarget[n];
    for(int i = 0; i < n; i++){
      solidTarget[i] = targetArr[i+1];
    }
    fileDataName = Form("~/proyecto/MoreEnergy/Data/MaxE_%s.root", solidTarget);
  } else{
    m = n+1;
    fileDataName = Form("~/proyecto/MoreEnergy/Data/MaxE_%s.root", targetArr);
  }

  // Select the target of the simultion
  char simulTarget[m];
  if(targetArr[0] == 'D') {
    simulTarget[0] = 'D';
    simulTarget[1] = '\0';
  } else{
    for(int i = 0; i < m; i++){
      simulTarget[i] = targetArr[i];
    }
  }

  // Open the inputs and output files
  TFile* fileData   = new TFile(fileDataName,"READ");
  //TFile* fileSimul  = new TFile(Form("/eos/user/m/mbarrial/Data/Acc/SimulTuple_%s.root", simulTarget), "READ");
  TFile* fileSimul  = new TFile(Form("~/proyecto/MoreEnergy/Data/%s1_simul2.root", simulTarget), "READ");
  //TFile *fileOutput = new TFile(Form("/eos/user/m/mbarrial/Data/Acc/corr_data_Phi_%s.root", targetArr), "RECREATE");
  TFile* fileOutput = new TFile(Form("~/proyecto/MoreEnergy/Data/corr_data_Phi_%s.root", targetArr), "RECREATE");
  gROOT->cd();

  // Create some variables to use inside the for loops
  TString tupleDataName;
  TCut Q2Cut, NuCut, ZhCut, Pt2Cut, VCData, cutsData, cutsThrown, cutsAccept;
  // Do not take in count events of 0 generated pion in the simultion beacause the is not hadrons variables to select a bin
  TCut YCCut = "TMath::Abs(YC)<1.4";
  // Select liquid or solid target
  if(targetArr[0] == 'D') { VCData  = "VC_TM == 1.";}
  else {VCData  = "VC_TM == 2.";}
  std::cout << Form("Simul target %s, Target %s", simulTarget, targetArr) << " and " << VCData << std::endl;

  // Create all the necessary histograms
  TH1F* histAccept      = new TH1F("Accept",     "", N_Phi, -180, 180);
  TH1F* histThrown      = new TH1F("Thrown",     "", N_Phi, -180, 180);
  TH1F* histData        = new TH1F("Data",       "", N_Phi, -180, 180);
  TH1F* histAccFactors  = new TH1F("AcceFactor", "", N_Phi, -180, 180);
  TH1F* histDataCorr    = new TH1F("DataCorr",   "", N_Phi, -180, 180);

  // Store the sum of the weights A.K.A the erros (in the other histograms if save it by other methods)
  histData->Sumw2();
  histThrown->Sumw2();
  histAccept->Sumw2();

  for(int nPion = 1; nPion <= N_PION ; nPion++) { // Loops in every number of generated pions

    for(int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) { // Loops in every Q2 bin
      for(int NuCounter = 0; NuCounter < N_Nu; NuCounter++) { // Loops in every Nu bin
        for(int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) { // Loops in every Zh bin

          std::cout << "Bin selected: " << nPion << Q2Counter << NuCounter << ZhCounter << std::endl;

          // Select the cuts for each bin
          Q2Cut   = Form("Q2>%f&&Q2<%f", Q2_BINS[Q2Counter], Q2_BINS[Q2Counter+1]);
          NuCut   = Form("Nu>%f&&Nu<%f", Nu_BINS[NuCounter], Nu_BINS[NuCounter+1]);
          ZhCut   = Form("Zh>%f&&Zh<%f", Zh_BINS[ZhCounter], Zh_BINS[ZhCounter+1]);

          cutsData   = Q2Cut&&NuCut&&ZhCut&&YCCut&&VCData;
          cutsThrown = Q2Cut&&NuCut&&ZhCut;
          cutsAccept = Q2Cut&&NuCut&&ZhCut&&YCCut;

          // Open the Ntuples
          TNtuple* ntupleData   = (TNtuple*) fileData->Get(Form("ntuple_%i_pion", nPion));
          TNtuple* ntupleThrown = (TNtuple*) fileSimul->Get("ntuple_thrown");
          TNtuple* ntupleAccept = (TNtuple*) fileSimul->Get("ntuple_accept");

          // Apply the cuts to the ntuples to increces the efficiency
          ntupleData->Draw(">>listData",     cutsData);
          ntupleThrown->Draw(">>listThrown", cutsThrown);
          ntupleAccept->Draw(">>listAccept", cutsAccept);
          TEventList* evntData   = (TEventList*) gDirectory->Get("listData");
          TEventList* evntThrown = (TEventList*) gDirectory->Get("listThrown");
          TEventList* evntAccept = (TEventList*) gDirectory->Get("listAccept");
          ntupleData->SetEventList(evntData);
          ntupleThrown->SetEventList(evntThrown);
          ntupleAccept->SetEventList(evntAccept);

          for(int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) { // Loops in every Pt2 bin

            // Select the Pt2 bin
            Pt2Cut  = Form("Pt2>%f&&Pt2<%f", Pt2_BINS[Pt2Counter], Pt2_BINS[Pt2Counter+1]);

            ntupleData->Project("Data", "PhiPQ", Pt2Cut);
            if(EmptyHist(histData) == 1){ continue; } // If there isn't any event in data skip this bin
            // Generate histograms of the all dectected pion, all generated pion, and the pions that was correct dectected
            ntupleAccept->Project("Accept", "PhiPQ", Pt2Cut);
            ntupleThrown->Project("Thrown", "PhiPQ", Pt2Cut);

            AccCondition(histAccept);

            // Calculate the Acceptance factor
            histAccFactors->Divide(histAccept, histThrown, 1, 1, "B");
            // Check that the acceptance factors are smaller than one
            AccHist1(histAccFactors);
            // Apply the correction factors
            histDataCorr->Divide(histData, histAccFactors, 1, 1);

            // Save the histograms in the output file
            fileOutput->cd();

            histData->Write(Form("Data_%s_%i%i%i%i_%i",             targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, nPion));
            histDataCorr->Write(Form("DataCorr_%s_%i%i%i%i_%i",     targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, nPion));
            histAccFactors->Write(Form("AcceFactor_%s_%i%i%i%i_%i", targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, nPion));

            gROOT->cd();

            // Set the histograms values to 0
            histData->Reset();
            histThrown->Reset();
            histAccept->Reset();
            histDataCorr->Reset();
            histAccFactors->Reset();

          } // End Pt2 loop
          delete ntupleData;
          delete ntupleAccept;
          delete ntupleThrown;
          delete evntData;
          delete evntAccept;
          delete evntThrown;
        } // End Q2 loop
      } // End Nu loop
    } // End Zh loop
  } // End pion number loop
  fileOutput->Close();
  fileData->Close();
  fileSimul->Close();
  t.Print();

}
