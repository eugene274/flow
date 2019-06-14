#include <utility>

//
// Created by Lukas Kreis on 13.11.17.
//

#include "CorrelationTask.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TTreeReaderValue.h"

using namespace std;

CorrelationTask::CorrelationTask(std::string filelist, std::string treename) :
    in_tree_(this->MakeChain(std::move(filelist), std::move(treename))),
    reader_(new TTreeReader(in_tree_.get())) {}

void CorrelationTask::Configure(Qn::CorrelationManager &a) {
  auto scalar = [](const std::vector<Qn::QVector> &a) -> double {
    return a[0].x(2) * a[1].x(2) + a[0].y(2) * a[1].y(2);
  };

  auto XY = [](const std::vector<Qn::QVector> &a) {
    return a[0].x(1) * a[1].y(1);
  };
  auto YX = [](const std::vector<Qn::QVector> &a) {
    return a[0].y(1) * a[1].x(1);
  };
  auto XX = [](const std::vector<Qn::QVector> &a) {
    return a[0].x(1) * a[1].x(1);
  };
  auto YY = [](const std::vector<Qn::QVector> &a) {
    return a[0].y(1) * a[1].y(1);
  };

  a.SetOutputFile("corr.root");
//  a.AddEventVariable({"Centrality", {0, 5, 10, 15, 25, 35, 45, 60, 80, 100.}});
  a.AddEventVariable({"Centrality", 20, 0, 100});
  a.ConfigureResampling(Qn::Sampler::Method::NONE, 100); // BOOTSTRAP, SUBSAMPLING

  a.AddQVectors("BBCwest, BBCeast, TPCeast, TPCwest");
  a.AddQVectors("pion_east_pT, pion_west_pT, pion_east_eta, pion_west_eta");
  
  if (isSimulation) {
    a.AddQVectors("mc_pion_pT, mc_pion_eta, psi");
  }
  
  vector <string> Q_vectors =
    {"BBCwest", "BBCeast", "TPCeast", "TPCwest"};
  
  vector <string> u_vectors = 
    {"pion_west_pT", "pion_east_pT", "pion_west_eta", "pion_east_eta"};

  const vector <string> u_vectors_MC = 
    {};

  /**
   * Correlations of all detectors vs PsiRP
   */
  string sComponents [] = {"XX", "YY", "XY", "YX"};
  vector<std::function<double(std::vector<Qn::QVector> &)>> fComponents;
  fComponents.push_back ([](const std::vector<Qn::QVector> &a) {return 2 * a[0].x(1) * a[1].x(1);});
  
  if (isSimulation) {
      u_vectors.insert (u_vectors.end(), u_vectors_MC.begin(), u_vectors_MC.end());
  }
  
  for (int iQ1 = 0; iQ1 < Q_vectors.size(); iQ1++)
  {
    auto Q1 = Q_vectors.at (iQ1);
    
    for (int iQ2 = iQ1 + 1; iQ2 < Q_vectors.size(); iQ2++)
    {
      auto Q2 = Q_vectors.at (iQ2);
      a.AddCorrelation(Q1 + "_" + Q2 + "_XX", Q1 + ", " + Q2, XX);
      a.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      a.AddCorrelation(Q1 + "_" + Q2 + "_YY", Q1 + ", " + Q2, YY);
      a.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      a.AddCorrelation(Q1 + "_" + Q2 + "_XY", Q1 + ", " + Q2, XY);
      a.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      a.AddCorrelation(Q1 + "_" + Q2 + "_YX", Q1 + ", " + Q2, YX);
      a.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    }
    
    for (const auto &u : u_vectors) {
      a.AddCorrelation(u + "_" + Q1 + "_XX", u + ", " + Q1, XX);
      a.SetRefQinCorrelation(u + "_" + Q1 + "_XX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      a.AddCorrelation(u + "_" + Q1 + "_YY", u + ", " + Q1, YY);
      a.SetRefQinCorrelation(u + "_" + Q1 + "_YY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      a.AddCorrelation(u + "_" + Q1 + "_XY", u + ", " + Q1, XY);
      a.SetRefQinCorrelation(u + "_" + Q1 + "_XY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      a.AddCorrelation(u + "_" + Q1 + "_YX", u + ", " + Q1, YX);
      a.SetRefQinCorrelation(u + "_" + Q1 + "_YX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
    }
  }
}

void CorrelationTask::Run() {
  int nevents = in_tree_->GetEntries();
  Qn::CorrelationManager a(reader_, nevents);
  in_tree_->LoadTree(0); // prevents weird TTree errors
  Configure(a);
  int events = 1;
  reader_->SetEntry(0); //  first entry needed to access configuration of DataContainers in the input file
  a.Initialize();
  in_tree_->LoadTree(0); // prevents weird TTree errors
  std::cout << "Processing..." << std::endl;
  int eventsteps = nevents / 100;
  while (reader_->Next()) {
    events++;
    a.Process();
    if (eventsteps > 1 && events % eventsteps == 0) {
      float progress = events / (float) nevents;
      ProgressBar(progress);
    }
  }
  a.Finalize();
  std::cout << std::endl << "number of analyzed events: " << events << std::endl;
}

std::unique_ptr<TChain> CorrelationTask::MakeChain(std::string filename, std::string treename) {
  std::unique_ptr<TChain> chain(new TChain(treename.data()));
  std::ifstream in;
  in.open(filename);
  std::string line;
  std::cout << "files in TChain:" << "\n";
  while ((in >> line).good()) {
    if (!line.empty()) {
      chain->AddFile(line.data());
      std::cout << line << std::endl;
    }
  }
  return chain;
}

void CorrelationTask::ProgressBar(float progress) {
  int barWidth = 70;
  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}