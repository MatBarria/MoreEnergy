#include "Binning.h"
#include "TStopwatch.h"
#include <iostream>
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"

void SetErrorXNull(TGraphErrors* g, int Ntarget) {

  double* errors_Y = g->GetEY();
  for(int point = 0 ; point < Ntarget ; point++) {
    g->SetPointError(point,0,errors_Y[point]);
  }

}

void SetXShift(TGraphErrors* g, double shift, int Ntarget) {

  double* content_Y = g->GetY();
  double* content_X = g->GetX();

  for(int point = 0; point < Ntarget ; point++) {
    g->SetPoint(point, content_X[point] + shift , content_Y[point]);
  }

}

TLegend* GenTLegendTarget(TGraphErrors* g[N_STARGETS][N_PION], float pos = 0.15) {

  TLegend* legend = new TLegend(pos, 0.75, 0.3, 0.92, "", "NDC");
  legend->AddEntry(g[0][0],"C","lpf");
  legend->AddEntry(g[1][0],"Fe","lpf");
  legend->AddEntry(g[2][0],"Pb","lpf");
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->SetTextFont(62);
  legend->SetTextSize(0.04);
  return legend;

}

TLegend* GenTLegendNPion(TGraphErrors* g[N_STARGETS][N_PION], float pos = 0.22) {

  TLegend* legend = new TLegend(pos, 0.75, 0.3, 0.92, "", "NDC");
  //legend->AddEntry((TObject*)0, "Final state", "");
  legend->AddEntry(g[2][0],"1 #pi +","lpf");
  legend->AddEntry(g[2][1],"2 #pi +","lpf");
  legend->AddEntry(g[2][2],"3 #pi +","lpf");
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->SetTextFont(62);
  legend->SetTextSize(0.04);
  return legend;

}

TGraphErrors* TH1TOTGraph(TH1 *h1) {

  if (!h1) std::cout << "TH1TOTGraph: histogram not found !" << std::endl;

  TGraphErrors* g1= new TGraphErrors();
  Double_t x, y, ex, ey;
  for (Int_t i = 1; i <= h1->GetNbinsX(); i++) {
    y = h1->GetBinContent(i);
    ey = h1->GetBinError(i);
    x = h1->GetBinCenter(i);
    // ex=h1->GetBinWidth(i);
    ex = 0.5*h1->GetBinWidth(i);
    g1->SetPoint(i-1, x, y);
    g1->SetPointError(i-1, ex, ey);
 }
 return g1;

}
// Calculate the Pt broadening and plot it in funcion of the size of the target
void PtBroadeningFullIntegrated(TString inputDirectory, TString plotDirectory) {

  TFile* inputFile = new TFile(inputDirectory + "meanPt2.root", "READ");

  TGraphErrors* g[N_STARGETS][N_PION];

  for(int nPion = 1; nPion <= N_PION; nPion++) {

    TH1F* histSolid[N_STARGETS]; TH1F* histLiquid[N_STARGETS]; TH1F* histBroadening[N_STARGETS];

    histSolid[0]  = (TH1F*) inputFile->Get(Form("meanPt2_C_%i",   nPion));
    histSolid[1]  = (TH1F*) inputFile->Get(Form("meanPt2_Fe_%i",  nPion));
    histSolid[2]  = (TH1F*) inputFile->Get(Form("meanPt2_Pb_%i",  nPion));

    histLiquid[0] = (TH1F*) inputFile->Get(Form("meanPt2_DC_%i",  nPion));
    histLiquid[1] = (TH1F*) inputFile->Get(Form("meanPt2_DFe_%i", nPion));
    histLiquid[2] = (TH1F*) inputFile->Get(Form("meanPt2_DPb_%i", nPion));

    for(int i = 0 ; i < N_PION ; i++) {
      //Calculate the Broadening (subtract of the means)
      histBroadening[i] = new TH1F(Form("histBroadening_%i", i), "", 1, Zh_MIN, Zh_MAX);
      histBroadening[i]->Add(histSolid[i], histLiquid[i], 1, -1);
    }

    // Set the points in TGraphErrors
    g[0][nPion-1] = new TGraphErrors();
    g[0][nPion-1]->SetPoint(1, TMath::Power(12.01,1./3.), histBroadening[0]->GetBinContent(1));
    g[0][nPion-1]->SetPointError(1, 0, histBroadening[0]->GetBinError(1));

    g[1][nPion-1] = new TGraphErrors();
    g[1][nPion-1]->SetPoint(1, TMath::Power(55.845,1./3.), histBroadening[1]->GetBinContent(1));
    g[1][nPion-1]->SetPointError(1, 0,histBroadening[1]->GetBinError(1));

    g[2][nPion-1] = new TGraphErrors();
    g[2][nPion-1]->SetPoint(1, TMath::Power(207.2,1./3.), histBroadening[2]->GetBinContent(1));
    g[2][nPion-1]->SetPointError(1, 0, histBroadening[2]->GetBinError(1));

    //Set a color for each target
    g[0][nPion-1]->SetMarkerColor(kRed);
    g[0][nPion-1]->SetLineColor(kRed);
    g[1][nPion-1]->SetMarkerColor(kBlue);
    g[1][nPion-1]->SetLineColor(kBlue);
    g[2][nPion-1]->SetMarkerColor(kBlack);
    g[2][nPion-1]->SetLineColor(kBlack);

    SetErrorXNull(g[0][nPion-1], N_STARGETS);
    SetErrorXNull(g[1][nPion-1], N_STARGETS);
    SetErrorXNull(g[2][nPion-1], N_STARGETS);

    for(int i = 0; i < N_STARGETS; i++) {
      delete histSolid[i];
      delete histLiquid[i];
      delete histBroadening[i];
    }
  } // End number pion event loop
  inputFile->Close();

  TFile* outputFile = new TFile(inputDirectory + "Pt_broad_FullIntegrated.root", "RECREATE");

  outputFile->cd();

  for(int i = 0; i < N_STARGETS; i++){
    for(int j = 0; j < N_PION; j++) {
      if(i == 0) { g[i][j]->Write(Form("PtBroad_FullIntegrated_C_%i" , j)); };
      if(i == 1) { g[i][j]->Write(Form("PtBroad_FullIntegrated_Fe_%i", j)); };
      if(i == 2) { g[i][j]->Write(Form("PtBroad_FullIntegrated_Pb_%i", j)); };
    }
  }

  outputFile->Close();
  gROOT->cd();


  for(int k = 0; k < N_STARGETS; k++) {
    g[k][0]->SetMarkerStyle(4);
    g[k][0]->SetMarkerSize(.53);
    g[k][1]->SetMarkerStyle(8);
    g[k][1]->SetMarkerSize(.6);
    g[k][2]->SetMarkerStyle(27);
    g[k][2]->SetMarkerSize(1.2);
    SetXShift(g[k][0], -0.055, N_STARGETS);
    SetXShift(g[k][2], 0.055,  N_STARGETS);
  }

  TCanvas* c = new TCanvas("c", "", 800, 600);
  c->Draw();

  // Create the legends
  TLegend* legendTarget = GenTLegendTarget(g);
  TLegend* legendNPion  = GenTLegendNPion(g);

  TPad* p = new TPad("p", "p", 0, 0, 1, 1);
  p->SetLeftMargin(0.13);
  p->SetTopMargin(0.06);
  p->SetGridx(1);
  p->SetGridy(1);
  p->Draw();
  p->cd();

  TMultiGraph* mg = new TMultiGraph();

  for(int i = 0; i < N_STARGETS; i++) {
    for(int j = 0; j < N_PION; j++) {
        mg->Add(g[i][j]);
    }
  }

  mg->Draw("APE");

  gStyle->SetTitleFont(62,"XY");
  gStyle->SetTitleSize(0.04,"XY");

  // mg->GetYaxis()->SetRangeUser(-0.004,0.042);
  mg->GetYaxis()->SetRangeUser(0,0.075);
  mg->GetXaxis()->SetRangeUser(2, 6.3);
  mg->GetXaxis()->SetTitle("A^{1/3}");
  mg->GetXaxis()->CenterTitle();
  mg->GetXaxis()->SetTitleOffset(1.1);

  mg->GetYaxis()->SetTitle("#Delta P^{2}_{T} [GeV^{2}]");
  mg->GetYaxis()->CenterTitle();
  mg->GetYaxis()->SetTitleOffset(1.3);

  mg->Draw("APE");
  legendTarget->Draw();
  legendNPion->Draw();

  c->Print(plotDirectory + "Pt_broad_FullIntegrated.pdf");
  delete c;
}


void SetAxisMultiGraph(TMultiGraph* mg, char var[]) {

  if(var[0] == 'Q') {
    // mg->GetYaxis()->SetRangeUser(0.,0.048);
    mg->GetYaxis()->SetRangeUser(0.,0.08);
    mg->GetXaxis()->SetRangeUser(.9,4.1);
    mg->GetXaxis()->SetTitle("Q^{2}[GeV^{2}]");
    mg->GetXaxis()->CenterTitle();
    mg->GetXaxis()->SetTitleOffset(1.1);
    return;
  }
  if(var[0] == 'N') {
    // mg->GetYaxis()->SetRangeUser(0.,0.049);
    mg->GetYaxis()->SetRangeUser(0.,0.08);
    mg->GetXaxis()->SetRangeUser(2.1,4.3);
    mg->GetXaxis()->SetTitle("#nu[GeV]");
    mg->GetXaxis()->CenterTitle();
    mg->GetXaxis()->SetTitleOffset(1.1);
    return;
  }
  if(var[0] == 'Z') {
    // mg->GetYaxis()->SetRangeUser(0.,0.072);
    mg->GetYaxis()->SetRangeUser(0.,0.1);
    mg->GetXaxis()->SetRangeUser(Zh_MIN, 1);
    // mg->GetXaxis()->SetTitle("Zh_{sum}");
    mg->GetXaxis()->SetTitle("Zh");
    mg->GetXaxis()->CenterTitle();
    mg->GetXaxis()->SetTitleOffset(1.1);
    return;
  }

}

void SetNBins(char var[] ,int &nBins) {

  if(var[0] == 'Q') {
    nBins = N_Q2;
    return;
  }
  if(var[0] == 'N') {
    nBins = N_Nu;
    return;
  }
  if(var[0] == 'Z') {
    nBins = N_Zh;
    return;
  }

}

// Calculate the Pt broadening and plot in funcion of the Q2 bins
void PtBroadeningVarIntegrated(char var[], TString inputDirectory, TString plotDirectory) {

  int nBins;
  SetNBins(var, nBins);

  float bins[nBins+1];
  // Select the binning of the varible
  if(var[0] == 'Q') {
    for(int i = 0; i < nBins+1; i++) {
      bins[i] = Q2_BINS[i];
    }
  }
  if(var[0] == 'N') {
    for(int i = 0; i < nBins+1; i++) {
      bins[i] = Nu_BINS[i];
    }
  }
  if(var[0] == 'Z') {
    for(int i = 0; i < nBins+1; i++) {
      bins[i] = Zh_BINS[i];
    }
  }

  TFile* inputFile  = new TFile(inputDirectory + Form("meanPt2_%s_%i.root", var, nBins), "READ");

  TCanvas* c = new TCanvas("c", "", 800, 600);
  c->Draw();

  TGraphErrors* g[N_STARGETS][N_PION];

  for(int nPion = 1; nPion <= N_PION; nPion++) { // Loops in every number of pion
    TH1F* histSolid[N_STARGETS];   TH1F* histLiquid[N_STARGETS]; TH1F* histBroadening[N_STARGETS];

    //C
    histSolid[0] = (TH1F*) inputFile->Get(Form("meanPt2_C_%i",   nPion));
    histLiquid[0] = (TH1F*) inputFile->Get(Form("meanPt2_DC_%i", nPion));
    //Fe
    histSolid[1] = (TH1F*) inputFile->Get(Form("meanPt2_Fe_%i",   nPion));
    histLiquid[1] = (TH1F*) inputFile->Get(Form("meanPt2_DFe_%i", nPion));
    //Pb
    histSolid[2] = (TH1F*) inputFile->Get(Form("meanPt2_Pb_%i",   nPion));
    histLiquid[2] = (TH1F*) inputFile->Get(Form("meanPt2_DPb_%i", nPion));

    // subtract the Pt2 Means to calculate the broadening
    for(int i = 0 ; i < N_STARGETS ; i++){
      histBroadening[i] = new TH1F(Form("histBroadening_%i",i), "", nBins, bins);
      histBroadening[i]->Add(histSolid[i], histLiquid[i], 1, -1);
    }

    // Transform the histograms to TGraphErrors
    for(int i = 0 ; i < N_STARGETS ; i++){
      g[i][nPion-1] = (TGraphErrors*) TH1TOTGraph(histBroadening[i]);
      SetErrorXNull(g[i][nPion-1], nBins);
    }

    g[0][nPion-1]->SetMarkerColor(kRed);
    g[1][nPion-1]->SetMarkerColor(kBlue);
    g[0][nPion-1]->SetLineColor(kRed);
    g[1][nPion-1]->SetLineColor(kBlue);
    g[2][nPion-1]->SetMarkerColor(kBlack);
    g[2][nPion-1]->SetLineColor(kBlack);

    SetXShift(g[0][nPion-1], -0.02 + (- 0.02 + 0.01*nPion), nBins);
    SetXShift(g[1][nPion-1], 0     + (- 0.02 + 0.01*nPion), nBins);
    SetXShift(g[2][nPion-1], 0.02  + (- 0.02 + 0.01*nPion), nBins);

    for(int i = 0; i < N_STARGETS; i++){
      delete histSolid[i];
      delete histLiquid[i];
      delete histBroadening[i];
    }
  } // End number pion event loop

  inputFile->Close();

  TFile* outputFile = new TFile(inputDirectory + Form("Pt_broad_%s.root", var), "RECREATE");

  outputFile->cd();

  for(int i = 0; i < N_STARGETS; i++){
    for(int j = 0; j < N_PION; j++) {
      if(i == 0) { g[i][j]->Write(Form("PtBroad_%s_C_%i",  var, j)); };
      if(i == 1) { g[i][j]->Write(Form("PtBroad_%s_Fe_%i", var, j)); };
      if(i == 2) { g[i][j]->Write(Form("PtBroad_%s_Pb_%i", var, j)); };
    }
  }

  outputFile->Close();
  gROOT->cd();


  TPad* p = new TPad("p", "p", 0, 0, 1, 1);
  p->SetGridx(1);
  p->SetGridy(1);
  //p->SetLeftMargin(0.13);
  //p->SetTopMargin(0.06);
  p->Draw();
  p->cd();

  for(int k = 0; k < N_STARGETS; k++) {
    g[k][0]->SetMarkerStyle(4);
    g[k][0]->SetMarkerSize(.53);
    g[k][1]->SetMarkerStyle(8);
    g[k][1]->SetMarkerSize(.6);
    g[k][2]->SetMarkerStyle(27);
    g[k][2]->SetMarkerSize(1.2);
  }

  TMultiGraph* mg = new TMultiGraph();
  for(int i = 0; i < N_STARGETS; i++){
    for(int j = 0; j < N_PION; j++){
        mg->Add(g[i][j]);
    }
  }

  mg->Draw("APE0");

  //gStyle->SetTitleFont(62, "XY");
  //gStyle->SetTitleSize(0.04, "XY");

  SetAxisMultiGraph(mg, var);
  mg->GetYaxis()->SetTitle("#Delta P^{2}_{T} [GeV^{2}]");
  mg->GetYaxis()->CenterTitle();
  //mg->GetYaxis()->SetTitleOffset(1.3);

  mg->Draw("APE0");
  TLegend* legendTarget = GenTLegendTarget(g);
  TLegend* legendNPion = GenTLegendNPion(g);
  legendTarget->Draw();
  legendNPion->Draw();

  c->Print(plotDirectory + Form("Pt_broad_%s.pdf", var));
  delete c;

}

void PtbroadeningMultiZh(TString inputDirectory, TString plotDirectory) {

  TFile* fsource  = new TFile(inputDirectory + "multiMeanPt2.root", "READ");
  TCanvas* c = new TCanvas("c","",1500,1500);
  c->Draw();
  c->Divide(3,3,0,0);

  int count = 1;
  for(int NuCounter = 0 ; NuCounter < N_Nu ; NuCounter++) { // Loops in every Nu bin
    for(int Q2Counter = 0 ; Q2Counter < N_Q2 ; Q2Counter++) { // Loops in every Q2 bin
            c->cd(count);count++;
      TGraphErrors* g[N_STARGETS][N_PION];

      for(int nPion = 1; nPion <= N_PION; nPion++) { // Loops in every number of pion

        TH1F* histSolid[3];   TH1F* histLiquid[3];  TH1F* histBroadening[3];

        histSolid[0] = (TH1F*) fsource->Get(Form("meanPt2_C_%i%i_%i", Q2Counter, NuCounter, nPion));
        histSolid[1] = (TH1F*) fsource->Get(Form("meanPt2_Fe_%i%i_%i", Q2Counter, NuCounter, nPion));
        histSolid[2] = (TH1F*) fsource->Get(Form("meanPt2_Pb_%i%i_%i", Q2Counter, NuCounter, nPion));

        histLiquid[0] = (TH1F*) fsource->Get(Form("meanPt2_DC_%i%i_%i", Q2Counter, NuCounter, nPion));
        histLiquid[1] = (TH1F*) fsource->Get(Form("meanPt2_DFe_%i%i_%i", Q2Counter, NuCounter, nPion));
        histLiquid[2] = (TH1F*) fsource->Get(Form("meanPt2_DPb_%i%i_%i", Q2Counter, NuCounter, nPion));

        for(Int_t i = 0 ; i < 3 ; i++){
          histBroadening[i] = new TH1F(Form("histBroadening_%i",i),"",N_Zh,Zh_BINS);
          histBroadening[i]->Add(histSolid[i],histLiquid[i],1,-1);
        }

        for(Int_t i = 0 ; i < 3 ; i++){
          g[i][nPion-1] = (TGraphErrors*) TH1TOTGraph(histBroadening[i]);
          SetErrorXNull(g[i][nPion-1], N_Zh);
        }
        g[0][nPion-1]->SetMarkerColor(kRed);
        g[0][nPion-1]->SetLineColor(kRed);
        g[1][nPion-1]->SetMarkerColor(kBlue);
        g[1][nPion-1]->SetLineColor(kBlue);
        g[2][nPion-1]->SetMarkerColor(kBlack);
        g[2][nPion-1]->SetLineColor(kBlack);

        SetXShift(g[0][nPion-1], -0.03 + (- 0.018 + 0.009*nPion), N_Zh);
        SetXShift(g[1][nPion-1], 0     + (- 0.018 + 0.009*nPion), N_Zh);
        SetXShift(g[2][nPion-1], 0.03  + (- 0.018 + 0.009*nPion), N_Zh);

        for(int i = 0; i < N_STARGETS; i++) {
          delete histSolid[i];
          delete histLiquid[i];
          delete histBroadening[i];
        }
      }
      TLegend* legend = new TLegend(0.15,0.75,0.3,0.92,"","NDC");
      legend->AddEntry(g[0][1],"C","lpf");
      legend->AddEntry(g[1][1],"Fe","lpf");
      legend->AddEntry(g[2][1],"Pb","lpf");
      legend->SetFillStyle(0);
      legend->SetBorderSize(0);
      legend->SetTextFont(62);
      legend->SetTextSize(0.04);

      for(int k = 0; k < 3; k++) {
        g[k][0]->SetMarkerStyle(22);
        g[k][0]->SetMarkerSize(1.2);
        g[k][1]->SetMarkerStyle(8);
        g[k][1]->SetMarkerSize(0.95);
        g[k][2]->SetMarkerStyle(27);
        g[k][2]->SetMarkerSize(1.7);
      }

      TLatex* kinematics = new TLatex();
      kinematics->SetTextFont(62);
      kinematics->SetTextSize(0.06);

      TLatex* kinematics2 = new TLatex();
      kinematics2->SetTextFont(62);
      kinematics2->SetTextSize(0.06);

      TPad* p = new TPad("p", "p", 0, 0, 1, 1);
      if(Q2Counter == 0) { p->SetLeftMargin(0.13); }
      else { p->SetLeftMargin(0.0); }
      if(NuCounter == 0) { p->SetTopMargin(0.13); }
      else { p->SetTopMargin(0.0); }
      if(Q2Counter == 2) { p->SetRightMargin(0.13); }
      else { p->SetRightMargin(0.0); }
      if(NuCounter == 2) { p->SetBottomMargin(0.13); }
      else { p->SetBottomMargin(0.0); }

      p->SetGridx(1);
      p->SetGridy(1);
      p->Draw();
      p->cd();


      TMultiGraph* mg = new TMultiGraph();
      for(int i = 0; i < N_STARGETS; i++){
        for(int j = 0; j < 2/*N_PION*/; j++){
          mg->Add(g[i][j]);
        }
      }


      mg->Draw("APE0");

      gStyle->SetTitleFont(62, "XY");
      gStyle->SetTitleSize(0.04, "XY");

      mg->GetYaxis()->SetRangeUser(0.,0.08);
      if(NuCounter == 2) {
        mg->GetXaxis()->SetRangeUser(Zh_MIN,1);
        mg->GetXaxis()->SetTitle("Zh_{SUM}");
        mg->GetXaxis()->CenterTitle();
        mg->GetXaxis()->SetTitleOffset(1.1);
      }
      if (Q2Counter == 0) {
        mg->GetYaxis()->SetTitle("#Delta P^{2}_{T} [GeV^{2}]");
        mg->GetYaxis()->CenterTitle();
        mg->GetYaxis()->SetTitleOffset(1.3);
      }

      //  mg->SetTitle(";z_{h};#Delta P^{2}_{T} [GeV^{2}]");
      mg->Draw("APE0");
      //legend->Draw();
      //kinematics->SetTextSize(1);
      if(NuCounter == 0) {
        kinematics->DrawLatexNDC(p->GetLeftMargin()+0.19,1.024*(1-p->GetTopMargin()),Form("%.2f<Q^{2}[GeV^{2}]<%.2f ", Q2_BINS[Q2Counter], Q2_BINS[Q2Counter+1]));
      }
      kinematics2->SetTextAngle(270);
      if(Q2Counter == 2) {
        kinematics2->DrawLatexNDC(1.024*(0.9-p->GetLeftMargin()),.7,Form("%.2f<#nu[GeV^{2}]<%.2f ", Nu_BINS[NuCounter], Nu_BINS[NuCounter+1]));
      }
      //delete mg;
      // for (int i = 0; i < 3; i++) {
      //   for (int j = 0; j < 3; j++) {
      //     std::cout << "PhiPQ integration4" << i << j<< std::endl;
      //     delete g[i][j];
      //   }
      // }
    } // End Nu loop
  } // End Q2 loop

  c->Print(plotDirectory + "Pt_broad_multiZh.pdf");
  fsource->Close();
  delete c;

}
