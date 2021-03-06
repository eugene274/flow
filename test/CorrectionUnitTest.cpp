

#include <random>
#include "gtest/gtest.h"
#include "CorrectionManager.h"


TEST(CorrectionUnitTest, Correction) {
  using namespace Qn;
  enum values {
    kCent,
    kPhiA,
    kPhiB = kPhiA+4,
    kMultA = kPhiB+4,
    kMultB = kMultA+4,

  };
  Qn::CorrectionManager man;
  auto calibfile = new TFile("corr.root");
  auto treefile = new TFile("tree.root","RECREATE");
  treefile->cd();
  auto tree = new TTree("tree","tree");
  man.SetTree(tree);
  man.AddVariable("PhiA",kPhiA,4);
  man.AddVariable("PhiB",kPhiB,4);
  man.AddVariable("MultA",kMultA,4);
  man.AddVariable("MultB",kMultB,4);
  man.AddVariable("Cent",kCent,1);

  auto correction = [](Qn::DetectorConfiguration *config) {
    config->SetNormalization(Qn::QVector::Normalization::M);
    config->AddCorrectionOnQnVector(new Qn::Recentering());
    auto channels = new bool[4]{true, true, true, true};
    auto group = new int[4]{0, 0, 0, 0};
    config->SetChannelsScheme(channels, group);
  };
  man.AddDetector("AA",DetectorType::CHANNEL,"PhiA","MultA",{},{1});
  man.SetCorrectionSteps("AA",correction);
  man.AddDetector("BB",DetectorType::CHANNEL,"PhiB","MultB",{},{1});
  man.SetCorrectionSteps("BB",correction);
  man.AddEventVariable("Cent");
  man.AddCorrectionAxis({"Cent",100,0,100});

  man.Initialize(calibfile);

  auto caliblist = man.GetCalibrationList();
  auto calibqalist = man.GetCalibrationQAList();

  man.SetProcessName("test");

  const unsigned int nevents = 1000000;
  std::default_random_engine gen;
  std::uniform_real_distribution<double> uniform(0,100);
  std::uniform_real_distribution<double> piform(0,2*TMath::Pi());
  std::normal_distribution<double> gauss(0,20);
  constexpr std::array<double, 4> vectorX = {{1.75, -1.75, 1.75, -1.75}};
  constexpr std::array<double, 4> vectorY = {{-1.75, -1.75, 1.75, 1.75}};
  auto values = man.GetVariableContainer();
  for (unsigned int iev = 0; iev < nevents; ++iev) {
    man.Reset();
    values[kCent] = uniform(gen);
    for (unsigned int ich = 0; ich < 4; ++ich) {
      values[kPhiA+ich] = std::atan2(vectorY[ich],vectorX[ich]);
      values[kPhiB+ich] = std::atan2(vectorY[ich],vectorX[ich]);
      values[kMultA+ich] = gauss(gen);
      values[kMultB+ich] = gauss(gen);
    }
    man.ProcessEvent();
    man.FillChannelDetectors();
    man.ProcessQnVectors();
  }

  man.Reset();
  values[kCent] = 13.1;
  for (unsigned int ich = 0; ich < 4; ++ich) {
    values[kPhiA+ich] = std::atan2(vectorY[ich],vectorX[ich]);
    values[kPhiB+ich] = std::atan2(vectorY[ich],vectorX[ich]);
    values[kMultA+ich] = 1;
    values[kMultB+ich] = 1;
  }
  values[kMultA] = 100000000;
  man.ProcessEvent();
  man.FillChannelDetectors();
  man.ProcessQnVectors();

  man.Finalize();
  calibfile->Close();
  delete calibfile;
  treefile->cd();
  tree->Write();
  caliblist->Write(caliblist->GetName(),TDirectoryFile::kSingleKey);
  calibqalist->Write(calibqalist->GetName(),TDirectoryFile::kSingleKey);
  treefile->Close();
  delete treefile;
}