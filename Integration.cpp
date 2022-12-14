// This code integrate the histograms generate by AccCorrection.cpp
// To obtain the means of Pt2
// It can be compile with
// g++ -Wall -fPIC -I./include `root-config --cflags` Integration.cpp -o ./bin/Integration `root-config --glibs` ./include/Integration.h

#include "Integration.h"

int main(){

  TStopwatch t;

  TString inputDirectory  = "/home/matias/proyecto/MoreEnergy/Data/";
  TString outputDirectory = "/home/matias/proyecto/MoreEnergy/Data/";

  std::cout << "PhiPQ integration" << std::endl;
  CallPhiIntegration(   inputDirectory , outputDirectory);
  std::cout << "Nu and Zh integration" << std::endl;
  CallNuZhIntegration(  inputDirectory , outputDirectory);
  std::cout << "Q2 and Nu integration" << std::endl;
  CallQ2NuIntegration(  inputDirectory , outputDirectory);
  std::cout << "Q2 and Zh integration" << std::endl;
  CallQ2ZhIntegration(  inputDirectory , outputDirectory);
  std::cout << "Q2, Nu and Zh integration" << std::endl;
  CallQ2NuZhIntegration(inputDirectory , outputDirectory);
  std::cout << "Mean Pt2" << std::endl;
  CallMeanPt2(inputDirectory, outputDirectory);

  t.Print();

}
