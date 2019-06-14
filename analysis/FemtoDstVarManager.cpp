#include <iostream>
using std::cout;
using std::endl;

#include <TMath.h>
#include <TLorentzVector.h>
#include "FemtoDstVarManager.h"

void FemtoDstVarManager::FillEventInfo(StFemtoEvent *event, double *values) {
  values [kCentrality] = 77.5 - event->cent16() * 5.;
  TVector3 pVtx = event->primaryVertex();
  values [kVx] = pVtx.X();
  values [kVy] = pVtx.Y();
  values [kVz] = pVtx.Z();
  // Going through the inner BBC rings
  for (int iTile=0; iTile<16; iTile++)
  {
    // Get BBC phi angle
    values [kBBCeastPhi + iTile] = BBC_GetPhi(0,iTile);
    values [kBBCwestPhi + iTile] = BBC_GetPhi(1,iTile);
    // Get energy deposition from BBC iTile module
    values [kBBCeastADC + iTile] = event->bbcAdcEast(iTile);
    values [kBBCwestADC + iTile] = event->bbcAdcWest(iTile);
  }
}

void FemtoDstVarManager::FillTrackInfo(StFemtoTrack *track, TVector3 pVtx, double *values) {
  values [kPt] = track->pt();
  values [kEta] = track->eta();
  values [kPhi] = track->phi();
  values [kDca] = track->gDCA(pVtx).Mag();
  values [kPid] = GetPid(track);
}

int FemtoDstVarManager::GetPid(StFemtoTrack *const &femtoTrack) {
  int pid = -1;
  //TPC-only
  if (!femtoTrack->isTofTrack())
  {
    //pion id
    if (femtoTrack->ptot() >= 0.2 && femtoTrack->ptot() < 0.6 && TMath::Abs(femtoTrack->nSigmaPion()) < 2)
    {
      pid = 211;
    }
    // kaon id
    if (femtoTrack->ptot() >= 0.2 && femtoTrack->ptot() < 0.5 && TMath::Abs(femtoTrack->nSigmaKaon()) < 2)
    {
      pid = 321;
    }
    // proton id
    if (femtoTrack->ptot() >= 0.4 && femtoTrack->ptot() < 0.9 && TMath::Abs(femtoTrack->nSigmaProton()) < 2)
    {
      pid = 2212;
    }
  }
  if (isGoodPID(femtoTrack))
  {
  // pion id - asymmetry cut
    if (femtoTrack->ptot() >= 0.2 && femtoTrack->ptot() < 3.4 && TMath::Abs(femtoTrack->nSigmaPion()) < 3 
      && femtoTrack->massSqr() >= -0.15 && femtoTrack->massSqr() < 0.1)
    {
      pid = 211;
    }
    // kaon id - asymmetry cut
    if (femtoTrack->pt() >= 0.2 && femtoTrack->ptot() < 3.4 && TMath::Abs(femtoTrack->nSigmaKaon()) < 3 
      && femtoTrack->massSqr() >= 0.2 && femtoTrack->massSqr() < 0.32)
    {
      pid = 321;
    }
    // proton id
    if (femtoTrack->ptot() >= 0.4 && femtoTrack->ptot() < 3.4 && TMath::Abs(femtoTrack->nSigmaProton()) < 3 
    && femtoTrack->massSqr() >= 0.75 && femtoTrack->massSqr() < 1.2)
    {
      pid = 2212;
    }
  }
  return pid;
}


Bool_t FemtoDstVarManager::isGoodPID(StFemtoTrack *const &track)
{
  float cutMass2Min = -10;
  if (!track->isTofTrack()) return false;
  if (track->massSqr() < cutMass2Min) return false;
  //NhitsDedx cut applied in StFemtoDst?
  // ToFYLocal cut applied in StFemtoDst?
  return true;
}

//--------------------------------------------------------------------------------------------------------------------------//
//this function simply connects the gain values read in to the BBC azimuthal distribution
//since tiles 7 and 9 (+ 13 and 15) share a gain value it is ambiguous how to assign the geometry here
//I prefer assigning the angle between the tiles thus "greying out" the adcs. 
//Others have assigned all of the adc to one (exclusive) or the the other. 
Float_t FemtoDstVarManager::BBC_GetPhi(const Int_t eastWest, const Int_t tileId)
{
  //float GetPhiInBBC(int eastWest, int bbcN) { //tileId=0 to 23
  const float Pi = TMath::Pi();
  const float Phi_div = Pi / 6;
  float bbc_phi = Phi_div;
  switch(tileId) {
    case 0: bbc_phi = 3.*Phi_div;
  break;
    case 1: bbc_phi = Phi_div;
  break;
    case 2: bbc_phi = -1.*Phi_div;
  break;
    case 3: bbc_phi = -3.*Phi_div;
  break;
    case 4: bbc_phi = -5.*Phi_div;
  break;
    case 5: bbc_phi = 5.*Phi_div;
  break;
    //case 6: bbc_phi= (mRndm.Rndm() > 0.5) ? 2.*Phi_div:4.*Phi_div;	//tiles 7 and 9 are gained together we randomly assign the gain to one XOR the other
    case 6: bbc_phi = 3.*Phi_div;
  break;
    case 7: bbc_phi = 3.*Phi_div;
  break;
    case 8: bbc_phi = Phi_div;
  break;
    case 9: bbc_phi = 0.;
  break;
    case 10: bbc_phi = -1.*Phi_div;
  break;
    //case 11: bbc_phi = (mRndm.Rndm() > 0.5) ? -2.*Phi_div:-4.*Phi_div;	//tiles 13 and 15 are gained together
    case 11: bbc_phi = -3.*Phi_div;
  break;
    case 12: bbc_phi = -3.*Phi_div;
  break;
    case 13: bbc_phi = -5.*Phi_div;
  break;
    case 14: bbc_phi = Pi;
  break;
    case 15: bbc_phi = 5.*Phi_div;
  break;
  }

  //if we're looking at the east BBC we need to flip around x in the STAR coordinates, 
  //a line parallel to the beam would go through tile 1 on the W BBC and tile 3 on the 
  if(0 == eastWest){
    if (bbc_phi > -0.001){ //this is not a >= since we are talking about finite adcs -- not to important
      bbc_phi = Pi - bbc_phi;
    }
    else {
      bbc_phi= -Pi - bbc_phi;
    }
  }

  if(bbc_phi < 0.0) bbc_phi += 2.*Pi;
  if(bbc_phi > 2.*Pi) bbc_phi -= 2.*Pi;

  return bbc_phi;
}