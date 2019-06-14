//
// Created by Lukas Kreis on 13.11.17.
//

#ifndef CORRELATION_TASK_H
#define CORRELATION_TASK_H

#include "TChain.h"
#include "TTreeReader.h"

#include <CorrelationManager.h>

class CorrelationTask {
 public:
  CorrelationTask() = default;
  CorrelationTask(std::string filelist, std::string treename);

  void Configure(Qn::CorrelationManager &manager);
  void Run();

 private:
  std::unique_ptr<TTree> in_tree_;
  std::shared_ptr<TTreeReader> reader_;

  void ProgressBar(float progress);


  /**
   * Make TChain from file list
   * @param filename name of file containing paths to root files containing the input trees
   * @return Pointer to the TChain
   */
  std::unique_ptr<TChain> MakeChain(std::string filename, std::string treename);
  bool isSimulation{false};
};

#endif //CORRELATION_TASK_H
