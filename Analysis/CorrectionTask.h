//
// Created by Lukas Kreis on 29.06.17.
//

#ifndef FLOW_QNTestTask_H
#define FLOW_QNTestTask_H

#include <vector>
#include <array>
#include <random>
#include "QnCorrectionsProfile3DCorrelations.h"
#include "QnCorrectionsProfileCorrelationComponents.h"
#include "QnCorrectionsDetectorConfigurationChannels.h"
#include "QnCorrectionsDetectorConfigurationBase.h"
#include "QnCorrectionsDetectorConfigurationTracks.h"
#include "QnCorrectionsQnVectorRecentering.h"
#include "QnCorrectionsQnVectorTwistAndRescale.h"
#include "QnCorrectionsCutSetBit.h"
#include "QnCorrectionsCutWithin.h"
#include "QnCorrectionsInputGainEqualization.h"
#include "QnCorrectionsQnVectorAlignment.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

#include "QnCorrectionsManager.h"
#include "DataContainer.h"
#include "EventInfo.h"
#include "AliReducedEventInfo.h"
#include "AliReducedVarManager.h"
#include "AliReducedTrackInfo.h"
#include "AliReducedBaseTrack.h"
#include "CorrectionManager.h"

#define VAR AliReducedVarManager

namespace Qn {
/**
 * Qn vector analysis TestTask. It is to be configured by the user.
 * @brief TestTask for analysing qn vectors
 */
class CorrectionTask {
 public:
  CorrectionTask(std::string filelist, std::string incalib, std::string treename);
  ~CorrectionTask() = default;
  void Run();

 private:
  /**
   * Initializes TestTask;
   */
  void Initialize();
  /**
   * Processes one event;
   */
  void Process();
  /**
   * Finalizes TestTask. Called after processing is done.
   */
  void Finalize();
  /**
   * Make TChain from file list
   * @param filename name of file containing paths to root files containing the input trees
   * @return Pointer to the TChain
   */
  std::unique_ptr<TChain> MakeChain(std::string filename, std::string treename);
  void SetVariables(std::vector<VAR::Variables> vars) {
    for (auto var : vars) {
      AliReducedVarManager::SetUseVariable(var);
    }
  }
 protected:
  std::shared_ptr<TFile> out_file_;
  std::shared_ptr<TFile> in_calibration_file_;
  std::shared_ptr<TFile> out_calibration_file_;
  std::unique_ptr<TTree> in_tree_;
  TTree *out_tree_;
  TTreeReader tree_reader_;
  TTreeReaderValue<AliReducedEventInfo> event_;
  Qn::CorrectionManager manager_;
  bool write_tree_;

};
}
#endif //FLOW_QNTASK_H
