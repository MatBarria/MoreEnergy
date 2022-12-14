// This code apply the two acceptance factors to the data and
// generate a PhiPQ histogram for each bin of Q2, Nu, Zh, Pt2
// The hadrons variables used were calculate taking the vectorial
// sum of the hadrons momentum and using it as hadron momentum for the event
// For simultion use the tuple generate by the code VecSumSimul.cpp
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
    //fileDataName = Form("/eos/user/m/mbarrial/Data/VecSum_%s.root", solidTarget);
    fileDataName = Form("~/proyecto/MoreEnergy/Data/MaxE_%s.root", solidTarget);
  } else{
    m = n+1;
    //fileDataName = Form("/eos/user/m/mbarrial/Data/VecSum_%s.root", targetArr);
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

  // Open the input and output files
  TFile* fileData   = new TFile(fileDataName,"READ");
  //TFile* fileSimul  = new TFile(Form("/eos/user/m/mbarrial/Data/Acc/SimulTuple_%s.root", simulTarget), "READ");
  TFile* fileSimul  = new TFile(Form("~/proyecto/MoreEnergy/Data/MaxEnSimul_%s.root", simulTarget), "READ");
  //TFile *fileOutput = new TFile(Form("/eos/user/m/mbarrial/Data/Acc/corr_data_Phi_%s.root", targetArr), "RECREATE");
  TFile* fileOutput = new TFile(Form("~/proyecto/MoreEnergy/Data/corr_data_Phi_%s.root", targetArr), "RECREATE");
  gROOT->cd();

  // Create some variables to use inside the for loops
  TString tupleDataName;
  TCut Q2Cut, NuCut, ZhCut, Pt2Cut, VCData, cutsData, cutsSimul, GenCut, DecCut, GenDecCut;
  // Do not take in count events of 0 generated pion in the simultion beacause the is not hadrons variables to select a bin
  TCut Gen1  = "Gen!=0";
  TCut YCCut = "TMath::Abs(YC)<1.4";
  // Select liquid or solid target
  if(targetArr[0] == 'D') { VCData  = "VC_TM == 1.";}
  else {VCData  = "VC_TM == 2.";}
  std::cout << Form("Simul target %s, Target %s", simulTarget, targetArr) << " and " << VCData << std::endl;

  // Create all the necessary histograms
  TH1F* histDetected    = new TH1F("Detected",      "", N_Phi, -180, 180);
  TH1F* histTotDetected = new TH1F("TotDetected",   "", N_Phi, -180, 180);
  TH1F* histThrown      = new TH1F("Thrown",        "", N_Phi, -180, 180);
  TH1F* histData        = new TH1F("Data",          "", N_Phi, -180, 180);
  TH1F* histFalPos      = new TH1F("FalPosFactor",  "", N_Phi, -180, 180);
  TH1F* histAccFactors  = new TH1F("AccFactor",     "", N_Phi, -180, 180);
  TH1F* histDataCorr    = new TH1F("DataCorr",      "", N_Phi, -180, 180);
  TH1F* histDataCorr2   = new TH1F("DataCorr2",     "", N_Phi, -180, 180);

  // Store the sum of the weights A.K.A the erros (in the other histograms if save it by other methods)
  histData->Sumw2();
  histThrown->Sumw2();
  histTotDetected->Sumw2();
  histDetected->Sumw2();

  for(int gen = 1; gen <= N_PION ; gen++) { // Loops in every number of generated pions

    GenCut    = Form("Gen == %f", (float)gen);
    DecCut    = Form("Dec == %f", (float)gen);
    GenDecCut = GenCut||DecCut;

    for(int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) { // Loops in every Q2 bin
      for(int NuCounter = 0; NuCounter < N_Nu; NuCounter++) { // Loops in every Nu bin
        for(int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) { // Loops in every Zh bin

          std::cout << "Bin selected: " << gen << Q2Counter << NuCounter << ZhCounter << std::endl;

          // Select the cuts for each bin
          Q2Cut   = Form("Q2>%f&&Q2<%f", Q2_BINS[Q2Counter],   Q2_BINS[Q2Counter+1]);
          NuCut   = Form("Nu>%f&&Nu<%f", Nu_BINS[NuCounter],   Nu_BINS[NuCounter+1]);
          ZhCut   = Form("Zh>%f&&Zh<%f", Zh_BINS[ZhCounter],   Zh_BINS[ZhCounter+1]);

          cutsData  = Q2Cut&&NuCut&&ZhCut&&YCCut&&VCData;
          cutsSimul = Q2Cut&&NuCut&&ZhCut&&Gen1&&GenDecCut;

          TNtuple* ntupleData  = (TNtuple*) fileData->Get(Form("ntuple_%i_pion", gen));
          TNtuple* ntupleSimul = (TNtuple*) fileSimul->Get("ntuple_sim");

          // Apply the cuts to the ntuples to increces the efficiency
          ntupleData->Draw(">>listData", cutsData);
          ntupleSimul->Draw(">>listSimul", cutsSimul);
          TEventList* evntData = (TEventList*) gDirectory->Get("listData");
          TEventList* evntSimul = (TEventList*) gDirectory->Get("listSimul");
          ntupleData->SetEventList(evntData);
          ntupleSimul->SetEventList(evntSimul);

          for(int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) { // Loops in every Pt2 bin

            // Select the Pt2 bin
            Pt2Cut  = Form("Pt2>%f&&Pt2<%f", Pt2_BINS[Pt2Counter], Pt2_BINS[Pt2Counter+1]);

            ntupleData->Project("Data", "PhiPQ", Pt2Cut);
            if(EmptyHist(histData) == 1){ continue; } // If there isn't any event in data skip this bin
            // Generate histograms of the all dectected pion, all generated pion, and the pions that was correct dectected
            ntupleSimul->Project("Detected",    "PhiPQ", Pt2Cut&&GenCut&&DecCut);
            if(EmptyHist(histDetected) == 1){ continue; } // If there isn't any event in simulation skip this bin
            ntupleSimul->Project("Thrown",      "PhiPQ", Pt2Cut&&GenCut);
            ntupleSimul->Project("TotDetected", "PhiPQ", Pt2Cut&&DecCut);

            //AccCondition(histDetected);

            // Calculate the Acceptance factor
            histAccFactors->Divide(histDetected, histThrown, 1, 1, "B");
            // Calculate a factor that reprensent how many of the detected as N pion events are truly N pions events
            histFalPos->Divide(histDetected, histTotDetected, 1, 1, "B");
            // Check that the acceptance factors are smaller than one
            AccHist1(histAccFactors);
            AccHist1(histFalPos);
            // Apply the correction factors
            histDataCorr->Divide(histData, histAccFactors, 1, 1);
            histDataCorr2->Multiply(histDataCorr, histFalPos, 1, 1);

            // Save the histograms in the output file
            fileOutput->cd();

            //histData->Write(Form("Data_%s_%i%i%i%i_%i",             targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, gen));
            histDataCorr2->Write(Form("DataCorr_%s_%i%i%i%i_%i",   targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, gen));
            //histDataCorr->Write(Form("DataCorr_%s_%i%i%i%i_%i",     targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, gen));
            //histFalPos->Write(Form("FalPosFactor_%s_%i%i%i%i_%i",   targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, gen));
            //histAccFactors->Write(Form("AccFactor_%s_%i%i%i%i_%i", targetArr, Q2Counter, NuCounter, ZhCounter, Pt2Counter, gen));

            gROOT->cd();

            // Set the histograms values to 0
            histData->Reset();
            histDataCorr2->Reset();
            histDataCorr->Reset();
            histFalPos->Reset();
            histAccFactors->Reset();
            histThrown->Reset();
            histDetected->Reset();
            histTotDetected->Reset();

          } // End Pt2 loop
          delete ntupleData;
          delete ntupleSimul;
          delete evntData;
          delete evntSimul;
        } // End Q2 loop
      } // End Nu loop
    } // End Zh loop
  } // End pion number loop
  fileOutput->Close();
  fileData->Close();
  fileSimul->Close();
  t.Print();

}
