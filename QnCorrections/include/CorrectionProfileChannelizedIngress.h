#ifndef QNCORRECTIONS_PROFILECHANNELINGRESS_H
#define QNCORRECTIONS_PROFILECHANNELINGRESS_H

/// \file QnCorrectionsProfileChannelizedIngress.h
/// \brief Ingress channelized profile for the Q vector correction framework

#include "CorrectionHistogramBase.h"
namespace Qn {
/// \class QnCorrectionsProfileChannelizedIngress
/// \brief Ingress channelized profile class for the Q vector correction histograms
///
/// Encapsulates a multidimensional profile. Each dimension
/// corresponds to one of the event classes variables so,
/// the number of dimensions matches the number of variables within
/// the set passed in the constructor. Additionally incorporates an
/// extra dimension to consider the channel number
///
/// The involved histograms can only be attached to existing ones
/// from a given list. The histograms to attach are the ones created from
/// QnCorrectionsProfileChannelized class. The difference with that class
/// is that now, the histograms are the source for applying corrections
/// and as such should be stored in a more efficient way and the
/// implicit group information be recovered.
///
/// Now the class will own histograms that must be deleted when the
/// class gets destroyed. The behavior regarding the channel handling
/// matches the one from QnCorrectionsProfileChannelized class.
///
/// The entries histogram disappears and so, GetBinContent and GetBinError
/// return in the standard way. Additionally, if applicable, the group
/// histogram is created and GetGrpBinContent and GetGrpBinError are
/// functional.
///
/// The profile as such cannot be filled. It should be considered as a
/// read only profile.
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Feb 23, 2016
class CorrectionProfileChannelizedIngress : public CorrectionHistogramBase {
 public:
  CorrectionProfileChannelizedIngress();
  CorrectionProfileChannelizedIngress(const char *name,
                                         const char *title,
                                         EventClassVariablesSet &ecvs,
                                         Int_t nNoOfChannels,
                                         Option_t *option = "");
  virtual ~CorrectionProfileChannelizedIngress();

  virtual Bool_t AttachHistograms(TList *histogramList, const Bool_t *bUsedChannel, const Int_t *nChannelGroup);
  /// wrong call for this class invoke base class behavior
  virtual Bool_t AttachHistograms(TList *histogramList) {
    return CorrectionHistogramBase::AttachHistograms(histogramList);
  }

  virtual Long64_t GetBin(const double *variableContainer, Int_t nChannel);
  virtual Long64_t GetGrpBin(const double *variableContainer, Int_t nChannel);
  /// wrong call for this class invoke base class behavior
  virtual Long64_t GetBin(const double *variableContainer) { return CorrectionHistogramBase::GetBin(variableContainer); }
  virtual Bool_t BinContentValidated(Long64_t bin);
  virtual Float_t GetBinContent(Long64_t bin);
  virtual Float_t GetGrpBinContent(Long64_t bin);
  virtual Float_t GetBinError(Long64_t bin);
  virtual Float_t GetGrpBinError(Long64_t bin);

 private:
  THnF *fValues;              //!<! the values and errors on each event class and channel
  THnF *fGroupValues;         //!<! the values and errors on each event class and group
  THnC *fValidated;           //!<! bin content validated flag
  Bool_t *fUsedChannel;       //!<! array, which of the detector channels are used for this configuration
  Int_t *fChannelGroup;       //!<! array, the group to which the channel pertains
  Int_t fNoOfChannels;        //!<! The number of channels associated to the whole detector
  Int_t fActualNoOfChannels;  //!<! The actual number of channels handled by the histogram
  Int_t *fChannelMap;         //!<! array, the map from histo to detector channel number
  Bool_t fUseGroups;          //!<! the groups structures must be used
  Bool_t *fUsedGroup;         //!<! array, which of the detector groups are used for this configuration
  Int_t fNoOfGroups;          //!<! the number of groups associated with the whole detector
  Int_t fActualNoOfGroups;    //!<! The actual number of groups handled by the histogram
  Int_t *fGroupMap;           //!<! array, the map from histo to detector channel group number


  /// \cond CLASSIMP
 ClassDef(CorrectionProfileChannelizedIngress, 2);
  /// \endcond
};
}
#endif
