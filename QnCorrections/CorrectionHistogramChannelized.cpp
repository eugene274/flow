/// \file QnCorrectionsHistogramChannelized.cxx
/// \brief Implementation of the single multidimensional histograms with channel support

#include "TList.h"

#include "EventClassVariablesSet.h"
#include "CorrectionHistogramChannelized.h"
#include "CorrectionLog.h"
/// \cond CLASSIMP
ClassImp(Qn::CorrectionHistogramChannelized);
/// \endcond
namespace Qn {

/// Default constructor
CorrectionHistogramChannelized::CorrectionHistogramChannelized() :
    CorrectionHistogramBase() {
  fValues = NULL;
  fUsedChannel = NULL;
  fNoOfChannels = 0;
  fActualNoOfChannels = 0;
  fChannelMap = NULL;
}

/// Normal constructor
///
/// Stores the set of variables that identify the
/// different event classes passing them to its parent
/// and prepares the object for actual histogram
/// creation or attachment
///
/// \param name base for the name of the histograms
/// \param title base for the title of the histograms
/// \param ecvs the event classes variables set
/// \param nNoOfChannels the number of channels associated
CorrectionHistogramChannelized::CorrectionHistogramChannelized(const char *name,
                                                                     const char *title,
                                                                     EventClassVariablesSet &ecvs,
                                                                     Int_t nNoOfChannels) :
    CorrectionHistogramBase(name, title, ecvs) {
  fValues = NULL;
  fValues = NULL;
  fUsedChannel = NULL;
  fNoOfChannels = nNoOfChannels;
  fActualNoOfChannels = 0;
  fChannelMap = NULL;
}

/// Default destructor
/// Releases the memory taken
CorrectionHistogramChannelized::~CorrectionHistogramChannelized() {

  if (fUsedChannel!=NULL) delete[] fUsedChannel;
  if (fChannelMap!=NULL) delete[] fChannelMap;
}

/// Creates the support histogram for the channelize histogram function
///
/// Based in the event classes variables set in the parent class
/// and the channel information passed as parameters
/// the values multidimensional histogram is
/// created.
///
/// The histogram is added to the passed histogram list
///
/// The actual number of channels is stored and a mask from
/// external channel number to histogram channel number. If
/// bUsedChannel is NULL all channels
/// within fNoOfChannels are assigned to this profile.
/// \param histogramList list where the histograms have to be added
/// \param bUsedChannel array of booleans one per each channel
/// \return true if properly created
Bool_t CorrectionHistogramChannelized::CreateChannelizedHistogram(TList *histogramList, const Bool_t *bUsedChannel) {
  /* let's build the histograms names and titles */
  TString histoName = GetName();
  TString histoTitle = GetTitle();
  TString entriesHistoName = GetName();
  entriesHistoName += szEntriesHistoSuffix;
  TString entriesHistoTitle = GetTitle();
  entriesHistoTitle += szEntriesHistoSuffix;

  /* we open space for channel variable as well */
  Int_t nVariables = fEventClassVariables.GetEntriesFast();
  Double_t *minvals = new Double_t[nVariables + 1];
  Double_t *maxvals = new Double_t[nVariables + 1];
  Int_t *nbins = new Int_t[nVariables + 1];

  /* get the multidimensional structure */
  fEventClassVariables.GetMultidimensionalConfiguration(nbins, minvals, maxvals);

  /* lets consider now the channel information */
  fUsedChannel = new Bool_t[fNoOfChannels];
  fChannelMap = new Int_t[fNoOfChannels];

  fActualNoOfChannels = 0;
  for (Int_t ixChannel = 0; ixChannel < fNoOfChannels; ixChannel++) {
    if (bUsedChannel!=NULL) {
      fUsedChannel[ixChannel] = bUsedChannel[ixChannel];
    } else {
      fUsedChannel[ixChannel] = kTRUE;
    }

    if (fUsedChannel[ixChannel]) {
      fChannelMap[ixChannel] = fActualNoOfChannels;
      fActualNoOfChannels++;
    }
  }

  /* There will be a wrong external view of the channel number especially */
  /* manifested when there are holes in the channel assignment */
  /* so, lets complete the dimension information */
  /* WARNING: be aware that ROOT does not keep label information when projecting THn */
  minvals[nVariables] = -0.5;
  maxvals[nVariables] = -0.5 + fActualNoOfChannels;
  nbins[nVariables] = fActualNoOfChannels;

  /* create the values multidimensional histogram */
  fValues = new THnF((const char *) histoName, (const char *) histoTitle, nVariables + 1, nbins, minvals, maxvals);

  /* now let's set the proper binning and label on each axis */
  for (Int_t var = 0; var < nVariables; var++) {
    fValues->GetAxis(var)->Set(fEventClassVariables.At(var)->GetNBins(), fEventClassVariables.At(var)->GetBins());
    fValues->GetAxis(var)->SetTitle(fEventClassVariables.At(var)->GetVariableLabel());
  }

  /* and now the channel axis */
  fValues->GetAxis(nVariables)->SetTitle(szChannelAxisTitle);

  /* and now set the proper channel labels if needed */
  if (fActualNoOfChannels!=fNoOfChannels) {
    for (Int_t ixChannel = 0; ixChannel < fNoOfChannels; ixChannel++) {
      if (fUsedChannel[ixChannel]) {
        fValues->GetAxis(nVariables)->SetBinLabel(fChannelMap[ixChannel] + 1, Form("%d", ixChannel));
      }
    }
  }

  fValues->Sumw2();

  histogramList->Add(fValues);

  delete[] minvals;
  delete[] maxvals;
  delete[] nbins;

  return kTRUE;
}

/// Get the bin number for the current variable content and passed channel
///
/// The bin number identifies the event class the current
/// variable content points to under the passed channel.
///
/// \param variableContainer the current variables content addressed by var Id
/// \param nChannel the interested external channel number
/// \return the associated bin to the current variables content
Long64_t CorrectionHistogramChannelized::GetBin(const double *variableContainer, Int_t nChannel) {

  FillBinAxesValues(variableContainer, fChannelMap[nChannel]);
  /* store the channel number */
  return fValues->GetBin(fBinAxesValues);
}

/// Check the validity of the content of the passed bin
/// This kind of histograms cannot validate the bin content so, it
/// is always valid.
/// \param bin the bin to check its content validity
/// \return kTRUE if the content is valid kFALSE otherwise
Bool_t CorrectionHistogramChannelized::BinContentValidated(Long64_t) {

  return kTRUE;
}

/// Get the bin content for the passed bin number
///
/// The bin number identifies a desired event class whose content
/// is requested.
///
/// \param bin the interested bin number
/// \return the bin number content
Float_t CorrectionHistogramChannelized::GetBinContent(Long64_t bin) {

  return fValues->GetBinContent(bin);
}

/// Get the bin content error for the passed bin number
///
/// The bin number identifies a desired event class whose content
/// error is requested.
///
/// \param bin the interested bin number
/// \return the bin number content error
Float_t CorrectionHistogramChannelized::GetBinError(Long64_t bin) {

  return fValues->GetBinError(bin);
}

/// Fills the histogram
///
/// The involved bin is computed according to the current variables
/// content and the passed external channel number. The bin is then
/// increased by the given weight.
///
/// \param variableContainer the current variables content addressed by var Id
/// \param nChannel the interested external channel number
/// \param weight the increment in the bin content
void CorrectionHistogramChannelized::Fill(const double *variableContainer, Int_t nChannel, Float_t weight) {
  /* keep the total entries in fValues updated */
  Double_t nEntries = fValues->GetEntries();

  FillBinAxesValues(variableContainer, fChannelMap[nChannel]);
  /* and now update the bin */
  fValues->Fill(fBinAxesValues, weight);
  fValues->SetEntries(nEntries + 1);
}
}

