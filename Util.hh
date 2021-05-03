//Utility functions for radon calculations
// See https://www.snolab.ca/deap/private/TWiki/bin/view/Main/RnDiffusionAt90C#Results_summary
// Temperature dependent diffusion coefficient

#ifndef __Util_h__
#define __Util_h__


Double_t DofT(Double_t aTCelsius) {
  Double_t TKelvin = aTCelsius + 273.15;
  Double_t answer = D_0 *TMath::Exp(-deltaE_d/(R*TKelvin));
  return answer;
}

// Temperature dependent solubility coefficient
Double_t SofT(Double_t aTCelsius) {
  Double_t TKelvin = aTCelsius + 273.15;
  Double_t answer = S_0 * TMath::Exp(-deltaH_S/(R*TKelvin));
  return answer;
}

//Option to return different profiles, defined in Diffuse.hh
Double_t GetActivity(Int_t aHours){
  if(calculateFlowguides){
    vector<Double_t> vTemp = FlowguidesNew(aHours);
    return vTemp[0];
  }
  else{
    return 0.0;
  }
}//End of get activity

//Get temperature
Double_t TempProfile(Int_t aHours){
  // LRT Profile
  if(calculateFlowguides){
    vector<Double_t> vTemp = FlowguidesNew(aHours);
    return vTemp[1];// Tempature part of profile
  }
  else{
    return 24.0;
  }
}


// Return the activity at a given time
Double_t AofTime(Int_t aHours) {
   return GetActivity(aHours);
}

#endif
