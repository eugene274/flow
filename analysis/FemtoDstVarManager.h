//
// Variable manager
//

#ifndef FEMTODSTVARMANAGER_H
#define FEMTODSTVARMANAGER_H

#include <TObject.h>
#include <TString.h>
#include <TChain.h>
#include <TH2F.h>
#include <TProfile2D.h>
#include "StFemtoEvent.h"
#include "StFemtoTrack.h"

//_____________________________________________________________________
class FemtoDstVarManager {

 public:
  enum Variables {
    kCentrality,
    kVx,
    kVy,
    kVz,
    kPt, 
    kEta, 
    kPhi,
    kDca,
    kPid,
    kBBCwestPhi, // 16
    kBBCeastPhi=kBBCwestPhi+16,  // 16
    kBBCwestADC=kBBCeastPhi+16,  // 16
    kBBCeastADC=kBBCwestADC+16,  // 16
    kNVars
  };

  FemtoDstVarManager(const FemtoDstVarManager &) = delete;
  FemtoDstVarManager(const FemtoDstVarManager &&) = delete;
  
  static void FillEventInfo(StFemtoEvent *event, double* values);
  static void FillTrackInfo(StFemtoTrack *track, TVector3 pVtx, double* values);
  static Float_t BBC_GetPhi(const Int_t eastWest, const Int_t tileId);
  static Bool_t isGoodPID (StFemtoTrack *const &femtoTrack);
  static int GetPid (StFemtoTrack *const &femtoTrack);

 private:
  
  FemtoDstVarManager() = default;
};

#endif
