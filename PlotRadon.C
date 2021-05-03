// Functions for plotting radon diffusion with different options.
// Author: David Gallacher, Sept 2020
// Eg for use: in root terminal:
// .L PlotRadon.C
// loadFile(***path***);
//

#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"

//Globals
TFile *fileIn;
string sanding_path;

//Prototypes
void loadFile(string path);
TH1D FinalProfile();

//main function
void PlotRadon(){
  //load the file
  string cold_path = "/Users/david/Documents/Physics/DEAP/Radon_Diffusion/Rn_Diff/data/rootfiles/FlowguidesNew_Full_0_2945_RnDiff.root";
  string hot_path = "/Users/david/Documents/Physics/DEAP/Radon_Diffusion/Rn_Diff/data/rootfiles/FlowguidesNew_Full_HotSecondAnneal_0_2945_RnDiff.root";
  sanding_path = "/Users/david/Documents/Physics/DEAP/Radon_Diffusion/Rn_Diff/data/rootfiles/FlowguidesNew_OnlySanding_2921_2945_RnDiff.root";

  loadFile(hot_path);
  TH1D ColdAnneal = FinalProfile();
  loadFile(cold_path);
  TH1D HotAnneal = FinalProfile();
  ColdAnneal.SetLineColor(kBlue);
  HotAnneal.SetLineColor(kRed);

  TH1D *hDiff = (TH1D*) HotAnneal.Clone();
  hDiff->Add(&ColdAnneal,-1);
  hDiff->SetLineColor(kMagenta);

  TCanvas *c1 = new TCanvas();
  gStyle->SetOptStat(000000);

  c1->Divide(2,1);
  c1->cd(1);
  gPad->SetLogy();
  gPad->SetGrid();
  HotAnneal.Draw();
  ColdAnneal.Draw("same");

  cout << "Hot Surface Activity = " <<HotAnneal.GetBinContent(1)<< " uBq/kg"<<endl;
  cout << "Cold Surface Activity = " <<HotAnneal.GetBinContent(1)<< " uBq/kg"<<endl;


  TLegend *leg = new TLegend(0.5,0.7,0.9,0.9);
  leg->AddEntry("HotAnneal","12 Bq/m3 90C Second Anneal","lpf");
  leg->AddEntry("ColdAnneal","Radon-free air 90C Second Anneal","lpf");
  leg->Draw("same");

  c1->cd(2);
  gPad->SetGrid();
  hDiff->Draw();
  hDiff->GetYaxis()->SetTitle("Hot-Cold [units]");


}

//Load radon file from path
void loadFile(string path){
  fileIn = new TFile(path.c_str(),"OPEN");
  cout << "File "<< path << " loaded" <<endl;
}


TH1D* FinalProfile(){

  //Sitting in room air until after first anneal
  TH1D *hA1 = (TH1D*) fileIn->Get("hdecay2895");

  //Second anneal in room air
  TH1D *hA2 = (TH1D*) fileIn->Get("hdecay2899");

  //Sanding and coating in Carleton glovebox
  loadFile(sanding_path);
  TH1D *hSanding = (TH1D*) fileIn->Get("hdecay2944");


  hA1->SetFillStyle(0);
  hA1->SetLineColor(1);

  hA2->SetFillStyle(0);
  hA2->SetLineColor(2);

  hSanding->SetFillStyle(0);
  hSanding->SetLineColor(4);

  // Summed histogram
  TH1D *hSum = (TH1D*) hA2->Clone();
  hSum->SetLineColor(kMagenta);

  int nbins = hA1->GetNbinsX();
  cout << nbins<<endl;

  double aQueens[125]={0};
  // Loop over bins and add to summed histogram after 1 mm cut
  int nstart = hSanding->FindBin(0.1);
  cout << nstart <<endl;

  int p = 0;
  for(int i=nstart;i<nbins;i++){
    aQueens[p] = hSum->GetBinContent(i);
    //cout << aQueens[i]<<endl;
    p++;
  }

  for(int i=1;i<=nbins;i++){
      int j = i-1;
      double sum = hSanding->GetBinContent(i) + aQueens[j];
      hSum->SetBinContent(i,sum);
  }

  return hSum;
}

//"/Users/david/Documents/Physics/DEAP/Radon_Diffusion/Rn_Diff/data/rootfiles/FlowguidesNew_Full_0_2945_RnDiff.root"
