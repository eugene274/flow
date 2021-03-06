// Flow Vector Correction Framework
//
// Copyright (C) 2018  Lukas Kreis, Ilya Selyuzhenkov
// Contact: l.kreis@gsi.de; ilya.selyuzhenkov@gmail.com
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FLOW_DETECTOR_H
#define FLOW_DETECTOR_H

#include <memory>
#include <utility>

#include "CorrectionDetector.h"
#include "CorrectionCalculator.h"
#include "EventClassVariablesSet.h"
#include "DetectorConfigurationChannels.h"
#include "DetectorConfigurationTracks.h"

#include "VariableManager.h"
#include "DataContainer.h"
#include "QVector.h"
#include "DataVector.h"
#include "QAHistogram.h"
#include "VariableCutBase.h"

namespace Qn {
/**
 * Enumerator class used to determine the type of detector
 */
enum class DetectorType {
  TRACK,
  CHANNEL
};
/**
 * @class Base class of a Detector
 */
class DetectorBase {
 public:
  virtual ~DetectorBase() = default;

  virtual std::unique_ptr<DataContainerDataVector> &GetDataContainer() = 0;
  virtual std::unique_ptr<DataContainerQVector> &GetQnDataContainer() = 0;

  virtual CorrectionDetector *GenerateDetector(int globalid, int binid, EventClassVariablesSet *set) = 0;
  virtual DetectorConfiguration *CreateDetectorConfiguration(const std::string &name, EventClassVariablesSet *set) = 0;
  virtual void SetConfig(std::function<void(DetectorConfiguration *config)> conf) = 0;
  virtual void AddCut(std::unique_ptr<VariableCutBase> cut) = 0;
  virtual void AddHistogram(std::unique_ptr<QAHistoBase> base) = 0;
  virtual void InitializeCutReports() = 0;
  virtual void FillReport() = 0;
  virtual void FillData() = 0;
  virtual void ClearData() = 0;
  virtual TList *GetReportList() = 0;
  virtual void SetUpCorrectionVectorPtrs(const Qn::CorrectionCalculator &calc, std::string step) = 0;
  virtual void GetCorrectedQVectors() = 0;

};

/**
 * Detector class
 * Template parameter automatically deduced.
 * @tparam N number of harmonics activated. Harmonic max is 8.
 */
template<std::size_t N>
class Detector : public DetectorBase {
  static_assert(N <= Qn::QVector::kMaxNHarmonics, "Detector requested more harmonics than the maximum of 8.");
 public:
  Detector(std::string name, const DetectorType type, std::vector<Qn::Axis> axes, const Variable phi,
           const Variable weight, const std::vector<Variable> &vars, int const(&harmo)[N]) :
      nchannels_(phi.length()),
      name_(name),
      harmonics_(new int[N]),
      type_(type),
      phi_(phi),
      weight_(weight),
      vars_(vars),
      cuts_(new Qn::Cuts),
      int_cuts_(new Qn::Cuts),
      datavector_(new Qn::DataContainerDataVector()),
      qvector_(new Qn::DataContainerQVector()) {
    coordinates_.resize(vars.size());
    datavector_->AddAxes(axes);
    qvector_->AddAxes(axes);
    correction_ptrs_.resize(qvector_->size());
    for (unsigned int i = 0; i < N; ++i) {
      harmonics_[i] = harmo[i];
      harmonics_bits_.set(harmo[i]);
    }
  }

  /**
   * @brief Clears data before filling new event.
   */
  void ClearData() override {
    datavector_->ClearData();
    qvector_->ClearData();
  }

  /**
   * Generate the detector and configure the calibration procedure based on the preset configuration
   * @param globalid global ID used in the correction calculator. It is unique.
   * @param binid local bin ID
   * @param set the set of event variables used for correction.
   * @return configured detector.
   */
  CorrectionDetector *GenerateDetector(int globalid, int binid, EventClassVariablesSet *set) override {
    if (!configuration_) {
      throw (std::runtime_error("No Qn correction configuration found for " + name_));
    }
    std::string name;
    if (datavector_->IsIntegrated()) {
      name = name_;
    } else {
      name = name_ + std::to_string(binid);
    }
    auto detector = new CorrectionDetector(name.data(), globalid);
    auto configuration = CreateDetectorConfiguration(name, set);
    configuration_(configuration);
    normalization_ = configuration->GetQVectorNormalizationMethod();
    detector->AddDetectorConfiguration(configuration);
    return detector;
  }

  /**
   * Generate the detector configuration.
   * @param name Name of the detector
   * @param set The set of event variables used for the configuration.
   * @return The detector configuration.
   */
  DetectorConfiguration *CreateDetectorConfiguration(const std::string &name, EventClassVariablesSet *set) override {
    DetectorConfiguration *configuration = nullptr;
    if (type_==DetectorType::CHANNEL) {
      configuration =
          new DetectorConfigurationChannels(name.data(), set, nchannels_, nharmonics_, harmonics_.get());
    }
    if (type_==DetectorType::TRACK)
      configuration = new DetectorConfigurationTracks(name.data(), set, nharmonics_, harmonics_.get());
    return configuration;
  }

  /**
   * @brief Get the datacontainer associated to the detector.
   * @return A reference to the Datacontainer.
   */
  std::unique_ptr<DataContainerDataVector> &GetDataContainer() override { return datavector_; }
  /**
   * @brief Get the Qn vector data container associated to the detector.
   * @return A reference to the Datacontainer.
   */
  std::unique_ptr<DataContainerQVector> &GetQnDataContainer() override { return qvector_; }

  /**
   * @brief Add the configuration function to the detector.
   * @param conf function which configures the detector. needs to have the specified form.
   */
  void SetConfig(std::function<void(DetectorConfiguration *config)> conf) override {
    configuration_ = conf;
  }

  /**
   * @brief Adds a cut to the detector
   * @param cut unique pointer to the cut. It is moved into the function and cannot be reused!
   */
  void AddCut(std::unique_ptr<VariableCutBase> cut) override {
    if (cut->GetVariableLength()==1) {
      int_cuts_->AddCut(std::move(cut));
    } else {
      cuts_->AddCut(std::move(cut));
    }
  }

  /**
   * @brief Fills the data into the data vectors, histograms and cut reports after the cuts have been checked.
   */
  void FillData() override {
    long i = 0;
    if (!int_cuts_->CheckCuts(0)) return;
    for (auto &histo : histograms_) {
      histo->Fill();
    }
    for (const auto &phi : phi_) {
      if (!cuts_->CheckCuts(i)) {
        ++i;
        continue;
      }
      if (vars_.empty()) {
        datavector_->CallOnElement(0, [&](std::vector<DataVector> &vector) {
          vector.emplace_back(phi, *(weight_.begin() + i));
        });
      } else {
        long icoord = 0;
        for (const auto &var : vars_) {
          coordinates_.at(icoord) = *(var.begin() + i);
          ++icoord;
        }
        try {
          datavector_->CallOnElement(coordinates_,
                                     [&](std::vector<DataVector> &vector) {
                                       vector.emplace_back(phi, *(weight_.begin() + i));
                                     });
        }
        catch (std::exception &) {}
      }
      ++i;
    }
  }

  /**
   * Initializes the histograms used for the cut report.
   * @param name name of the detector.
   */
  void InitializeCutReports() override {
    int_cuts_->CreateCutReport(name_, 1);
    cuts_->CreateCutReport(name_, static_cast<size_t>(phi_.length()));
  }

  /**
   * @brief Returns TList of QA and cut report histograms.
   * @return list of histograms. Lifetime managed by the user.
   */
  TList *GetReportList() override {
    auto list = new TList();
    int_cuts_->AddToList(list);
    cuts_->AddToList(list);
    for (auto &histo : histograms_) {
      histo->AddToList(list);
    }
    return list;
  }

  /**
   * Fill the cuts reports to the
   */
  void FillReport() override {
    int_cuts_->FillReport();
    cuts_->FillReport();
  }

  /**
   * @brief Adds a QA histogram to the detector.
   * @param histo pointer to the histogram.
   */
  void AddHistogram(std::unique_ptr<QAHistoBase> histo) override {
    histograms_.push_back(std::move(histo));
  }

  /**
   * @brief Saves the pointers to the corrected Q vectors.
   * @param calc reference to the correction calculator.
   * @param step specification which correction step is supposed to be retrieved.
   */
  void SetUpCorrectionVectorPtrs(const Qn::CorrectionCalculator &calc, std::string step) override {
    int ibin = 0;
    for (auto &bin : correction_ptrs_) {
      std::string binname;
      if (qvector_->IsIntegrated()) {
        binname = name_;
      } else {
        binname = (name_ + std::to_string(ibin));
      }
      ++ibin;
      bin = calc.GetDetectorQnVectorPtr(binname.data(), step.c_str(), step.c_str());
    }
  }

  /**
   * @brief Updates the Qvectors to the values retrieved from the correction calculator.
   * This function is called every event before the output tree is filled.
   */
  void GetCorrectedQVectors() override {
    int ibin = 0;
    for (auto &bin : *qvector_) {
      bin = QVector(normalization_, correction_ptrs_.at(ibin), harmonics_bits_);
      ++ibin;
    }
  }

 private:
  Qn::QVector::Normalization normalization_ = Qn::QVector::Normalization::NONE; /// Normalization of the Q vectors
  int nchannels_ = 0; /// number of channels in case of channel detector
  int nharmonics_ = N; /// number of harmonics
  const std::string name_; /// name of  the detector
  std::bitset<Qn::QVector::kMaxNHarmonics> harmonics_bits_; /// bitset of all activated harmonics
  std::unique_ptr<int[]> harmonics_; /// int array of all activated harmonics
  const DetectorType type_; /// type of the detector: channel or tracking detector
  const Variable phi_; /// variable holding the azimuthal angle
  const Variable weight_; /// variable holding the weight which is used for the calculation of the Q vector.
  std::vector<Variable> vars_; /// variables used for the binning of the Q vector.
  std::vector<float> coordinates_;  ///  vector holding the temporary coordinates of one track or channel.
  std::unique_ptr<Cuts> cuts_; /// per channel selection  cuts
  std::unique_ptr<Cuts> int_cuts_; /// integrated selection cuts
  std::vector<std::unique_ptr<QAHistoBase>> histograms_; /// QA histograms of the detector
  std::unique_ptr<DataContainerDataVector> datavector_; /// Container holding the data vectors of the current event.
  std::unique_ptr<DataContainerQVector> qvector_; /// Container holding the Q vectors of the current event.
  std::vector<const Qn::CorrectionQnVector *> correction_ptrs_; /// pointers to the latest corrected Q vectors
  std::function<void(DetectorConfiguration *config)> configuration_; /// correction configuration function
};
}

#endif //FLOW_DETECTOR_H
