// Main diffusion calculations
// This is almost all based on work by Bei Cai and Marcin Kuzniak, refer to DEAP-STR-###
// Restructuring and compiling by David Gallacher, Sept 2020

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TObject.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TTimeStamp.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

//#include <stdio.h>
//#include <cstdio>

#include "Diffuse.hh"
#include "Util.hh"

using namespace std;

//Prototypes
void CalculateRadonDiffusion(string file_out_name, int startDay, int endDay);
void oneStep();
void start();
void loop(int nhours);
void Parameters(int startDay, int endDay);


int main(int argc, char **argv){

    char oF[256]; // output file name, creates output_file_name_startDay_endDay_Rn_Diff.root and output_file_name_startDay_endDay_Rn_Diff.txt
    int sDay;
    int eDay;
    if(argc==4){
      sprintf(oF,argv[1]);
      sDay = atoi(argv[2]);
      eDay = atoi(argv[3]);
    } else {
        cout <<"./Radonify output_file_name start_day end_day"<< endl;
        cout << "Creates output_file_name_startDay_endDay_Rn_Diff.root and output_file_name_startDay_endDay_Rn_Diff.txt" <<endl;
     exit(0);
    }

    string fname = oF;
    CalculateRadonDiffusion(fname,sDay,eDay);

  return 0;
}


void CalculateRadonDiffusion(string file_out_name, int startDay,int endDay){

  string fname_root = file_out_name+"_"+to_string(startDay)+"_"+to_string(endDay)+"_RnDiff.root";
  string fname_txt = file_out_name+"_"+to_string(startDay)+"_"+to_string(endDay)+"_RnDiff.txt";

  ofstream ofs;
  ofs.open(fname_txt.c_str());

  start();
  ofs  << "Check Stability using NR in C eqn 19.2.6"<<endl;
  ofs  << "This number must be less than 1: "<< 2*D*timeStep/stepSq << " with D= "<< D<<" timeStep= "<< timeStep <<" step = "<<step<<endl;

  TFile* fout = new TFile(fname_root.c_str(),"RECREATE");
  Parameters(startDay,endDay);

  int i;
  // int j;
  char hname[100];
  // Double_t decays;
  // Double_t activity;
  // Double_t activitykg;

  Double_t density = 1.19e-3; // density of acrylic in kg/cm3
  Double_t livetimePb = 22.3*365.24*24*3600/log(2); // in seconds
  Double_t PbBqkgToppt = 3.54e-4;
  Double_t conversion = PbBqkgToppt/density/livetimePb; // converted Bq/kg into ppt // should be uBq/kg into ppt?

  for(i=startDay;i<endDay;i++){//
    dayNumber = i;
    loop(24); // Always loop for 24 hours to get a full day
    sprintf(hname, "h%d",i);
    h->Write(hname);
    sprintf(hname, "hdecay%.2d",i);
    if(i%5==0)cout << i << "  " << currentTemperature<< "  " << solubility << "  " << D<< "  " << currentActivity<< "  " << 2*D*timeStep/stepSq<< "  " << h->GetBinContent(1)<< "  " << hdecay->GetBinContent(1)*conversion/1.1e-8*0.01<< "  " << hdecay->GetBinContent(hdecay->FindBin(0.05))*conversion/1.1e-8*0.01<< "  " << hdecay->GetBinContent(hdecay->FindBin(0.1))*conversion/1.1e-8*0.01<<endl;
    ofs << i << "  " << currentTemperature<< "  " << solubility << "  " << D<< "  " << currentActivity<< "  " << 2*D*timeStep/stepSq<< "  " << h->GetBinContent(1)<< "  " << hdecay->GetBinContent(1)*conversion/1.1e-8*0.01<< "  " << hdecay->GetBinContent(hdecay->FindBin(0.05))*conversion/1.1e-8*0.01<< "  " << hdecay->GetBinContent(hdecay->FindBin(0.1))*conversion/1.1e-8*0.01<<endl;
    hdecay->Write(hname);
  }
  fout->Close();
  ofs.close();
}

//Create histograms
void start(){
  nbins = TMath::Nint(thickness/step); // Convert to int for histogram binning
  h = new TH1D("h","Concentration", nbins, 0, nbins*step);
  thickness = nbins*step; // Back to double
  h->SetBinContent(1,0.476595*solubility); // scale initial concentration to 1Bq/m3
  hd = new TH1D("hd","dC", nbins, 0, nbins*step);
  hdd = new TH1D("hdd","d2C/dx2", nbins, 0, nbins*step);
  hdt = new TH1D("hdt","dC/dt", nbins, 0, nbins*step);
  hdecay = new TH1D("hdecay","Radon Decays", nbins, 0, nbins*step);
  hdecay->SetXTitle("Depth of acrylic, cm");
  hdecay->SetYTitle("Pb activity, uBq/kg");
  hpbact = new TH2D("hpbact", "Activity of Lead", 100,0, 100, nbins, 0, nbins*step);
  hpbact->SetXTitle("Exposure Time [days]");
  hpbact->SetYTitle("Depth [cm]");
  char title[100];
  sprintf(title,"Activity #mu Bq/kg");
  hpbact->SetZTitle(title);
  cout <<"Check Stability using NR in C eqn 19.2.6"<<endl;
  cout << "This number must be less than 1: "<< 2*D*timeStep/stepSq << " with D= "<< D<<" timeStep= "<< timeStep <<" step = "<<step<<endl;
}

// Loops over one day. Should always be called with 24 as the argument.
// Allows updating of daily histograms.
void loop(Int_t nhours) {
  Int_t nseconds = 3600*nhours;
  Int_t nsteps = nseconds/timeStep;
  int i;

  Double_t currentTimeSeconds, currentTimeHours;
  for( i=0 ; i<nsteps ; i++ ) {
    if( !constantTemperature ) {
      currentTimeSeconds = i*timeStep;
      // dayNumber set in doIt
      currentTimeHours = 24*dayNumber + currentTimeSeconds/3600;
      currentTemperature = TempProfile(currentTimeHours);
      currentActivity = AofTime(currentTimeHours);
      solubility = SofT(currentTemperature);
      D = DofT(currentTemperature);
    }
    //printf("step # %d, about to take a step, day %d, total %d steps  \n",i,dayNumber,nsteps);
    oneStep();
  }//End loop over steps
}// End loop

void oneStep(){

  int i;
  Float_t c3, c2, c1;
  Double_t ndecays;
  Float_t deltaC;

  // Do decays in first bin
  // Will need to reset this value as solubility changes with temperature.
  // For true time-dependent analysis require this be A[Bq/m^3] * 0.476595 * S(T)
  if( constantTemperature ) {
    c2 = h->GetBinContent(1);  // Should always be 0.476595*solubility
  }
  else {
    // current activity refers to the RRA inside the vessel
    h->SetBinContent(1, currentActivity*0.476595*solubility); // scale initial concentration to 0.476595 atoms/cm3=1Bq/m3
    //where cA is the RAD7 reading in Bq, thus bin content is concentration of Rn at air-acrylic boundary
    c2 = h->GetBinContent(1);
  }
  ndecays = lambda*c2*timeStep;
  hdecay->SetBinContent(1, hdecay->GetBinContent(1) + ndecays); // in atoms/cm^3

  for( i=2 ; i<nbins ; i++ ) {
    c3 = h->GetBinContent(i+1);
    c2 = h->GetBinContent(i);
    c1 = h->GetBinContent(i-1);
    hdd->SetBinContent(i, D*(c3-2*c2+c1)/stepSq);
    ndecays = lambda*c2*timeStep;
    hdecay->SetBinContent(i, hdecay->GetBinContent(i) + ndecays);
    deltaC = (hdd->GetBinContent(i) - lambda*c2)*timeStep;
    hd->SetBinContent(i, deltaC);
  }// update histograms
  h->Add(hd);//add hd to the existing h
}// End of one step


//Ntuple to hold parameters
void Parameters(int startDay=0,int endDay=1) {
  if( ntparameters == NULL ) {
    ntparameters = new TNtuple("ntparameters","Parameters","h:degC:A:D:S");
  }
  ntparameters->SetMarkerStyle(6);

  int i;
  Double_t TT;
  Double_t AA;
  Double_t DD;
  Double_t SS;
  // one entry per day, time in hours since start
  for(i=startDay;i<24*endDay;i++){
    TT = TempProfile(i);
    AA = AofTime(i);
    DD = DofT(TT);
    SS = SofT(TT);
    ntparameters->Fill(i, TT, AA, DD, SS);
  }
  ntparameters->Write();
}
