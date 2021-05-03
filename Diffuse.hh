//Variables and data for Diffuse.cc

#ifndef __Diffuse_h__
#define __Diffuse_h__


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

using namespace std;

Double_t thickness = 0.5; // cm PMMA
Double_t timeStep = 5; // in seconds PMMA
Double_t step = 0.004; // cm   PMMA
Double_t stepSq = step*step;
Double_t lambda = 2.1e-6;  // sec^{-1}
Int_t nbins;

Double_t R = 1.986; // cal/mol/K (ideal gas R)
Double_t D = 6.2e-10; //cm2/sec    PMMA Wojcik
Double_t solubility = 8.2;   // PMMA

/*
// CheFEM
Double_t deltaE_d = 16163.6;
Double_t D_0 = 486.95;
Double_t S_0 = 2.09527e-06;
Double_t deltaH_S = -8958.3;
*/

/*
// Marcin with O2 for S
Double_t deltaE_d = 4124.15;
Double_t D_0 = 6.72093e-07;
Double_t S_0 = 0.0230306;
Double_t deltaH_S = -3467.11;
*/

// Marcin without O2 for S
Double_t deltaE_d = 4124.15;
Double_t D_0 = 6.72093e-07;
Double_t S_0 = 4.50244;
Double_t deltaH_S = -353.797;

TH1D* h = NULL;
TH1D* hd = NULL;
TH1D* hdd = NULL;
TH1D* hdt = NULL;
TH1D* hdecay = NULL;
TH2D* hpbact = NULL;
TNtuple* ntparameters = NULL;

Bool_t constantTemperature = kFALSE;
Double_t currentTemperature;
Double_t currentActivity;
Int_t dayNumber;

//To allow for other activity and temperature profiles
Bool_t calculateFlowguides = kTRUE;

extern ofstream ofs;


vector<Double_t> FlowguidesNew(int aHours){
  // Get the activity of the air in Bq for a particular hour number
  // RPT colorado
  // TTimeStamp tStart(2012, 1, 31, 0,0,0,0,0,0);
  // Int_t hoursStart = TMath::Nint(tStart.GetSec()/3600.0);

  vector<Double_t> vecAT; // vecAT={Activity,Temp} for a given timestamp

  TTimeStamp RPTEnd(2012, 3, 1, 0,0,0,0,0,0);
  Int_t hoursRPTEnd = TMath::Nint(RPTEnd.GetSec()/3600.0);


  TTimeStamp tStart(2020, 1, 30, 0, 0, 0, 0, 0, 0); // Alberta January 2020
  Int_t hoursStart = TMath::Nint(tStart.GetSec()/3600.0) - hoursRPTEnd;
  if (aHours < hoursStart ){
    vecAT.push_back(12.0); // 12 Bq/m3
    vecAT.push_back(24.0); // 24 degC
    return vecAT;
  }

  TTimeStamp AfirstAnnealStart(2020, 2, 1, 0, 0, 0, 0, 0, 0); // Start Anneal
  Int_t AhoursFirstAnnealStart = TMath::Nint(AfirstAnnealStart.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours<AhoursFirstAnnealStart ){
    vecAT.push_back(12.0); // 12 Bq/m3
    vecAT.push_back(24.0); // 24 degC
    return vecAT;
   }

  TTimeStamp AfirstAnnealEnd(2020, 2, 2, 5, 0, 0, 0, 0, 0); // 29 hour anneal cycle
  Int_t AhoursFirstAnnealEnd = TMath::Nint(AfirstAnnealEnd.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours<=AhoursFirstAnnealEnd ){
    vecAT.push_back(12.0); // 12 Bq/m3
    vecAT.push_back(90.0); // Assume 90C the entire anneal
    return vecAT;
  }


  TTimeStamp AsecondAnnealStart(2020, 2, 5, 0, 0, 0, 0, 0, 0); // Start Second Anneal in February 2020, chosen arbitrarily
  Int_t AhoursSecondAnnealStart = TMath::Nint(AsecondAnnealStart.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours<AhoursSecondAnnealStart ) {
    vecAT.push_back(0.0); // Inside canister at vacuum
    vecAT.push_back(24.0); // Room Temp
    return vecAT;
  }

  TTimeStamp AsecondAnnealEnd(2020, 2, 6, 5, 0, 0, 0, 0, 0); // 29 hour anneal cycle
  Int_t AhoursSecondAnnealEnd = TMath::Nint(AsecondAnnealEnd.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours<=AhoursSecondAnnealEnd ){
    vecAT.push_back(12.0);// Estimated activity in Rn-free room
    vecAT.push_back(90.0);// Assume 90C for whole annealing cycle
    return vecAT;
  }

  TTimeStamp sandingQueensStart(2020, 3 ,1, 0, 0, 0, 0, 0, 0);// Sanding and delivery to Carleton starting in March,
  Int_t hoursQueensStart = TMath::Nint(sandingQueensStart.GetSec()/3600.0) - hoursRPTEnd;// In Rn-free room during interim
  if( aHours<hoursQueensStart ){
    vecAT.push_back(3e-3);
    vecAT.push_back(24.0);
    return vecAT;
  }

  TTimeStamp sandingQueensEnd(2020, 3, 6, 0, 0, 0, 0, 0, 0); // 5 days of sanding, a guess - FIXME
  Int_t hoursQueensEnd = TMath::Nint(sandingQueensEnd.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours<hoursQueensEnd ){
    vecAT.push_back(0.5e-3); // From Marcin STR: DEAP-STR-2015-001 Rev 2 activity of the purge gas
    vecAT.push_back(24.0);
    return vecAT;
  }

  TTimeStamp undergroundStart(2020, 3, 24, 7, 0, 0, 0, 0, 0);
  Int_t hoursundergroundStart = TMath::Nint(undergroundStart.GetSec()/3600.0) - hoursRPTEnd;
  if( aHours< hoursundergroundStart){
    vecAT.push_back(0.5e-3); //Inside vessel
    vecAT.push_back(24.0);
    return vecAT; // Inside vessel during shipment
  }

  // TTimeStamp undergroundEnd(2015, 8, 24, 8, 0, 0, 0, 0, 0); // Estimated 1 hour exposure to Underground air
  // Int_t hoursundergroundEnd = TMath::Nint(undergroundEnd.GetSec()/3600.0) - hoursRPTEnd;
  // if (aHours < hoursundergroundEnd) return 120.0;
  else{
    vecAT.push_back(0.0);
    vecAT.push_back(24.0);
    return vecAT;
  }

}// End vector of activities and temperatures

#endif
