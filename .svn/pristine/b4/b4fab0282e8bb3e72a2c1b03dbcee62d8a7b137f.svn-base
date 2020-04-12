#include "EXOReconstruction/EXODefineCrossProductProcessList.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXOCorrelationCollection.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "TFile.h"
#include "TObject.h"
#include "TH1D.h"
#include <sstream>
#include <vector>

using namespace std;

EXODefineCrossProductProcessList::EXODefineCrossProductProcessList()
: fFilename("Correlations.root"),
  fNSigma(5.0),
  fNBins(32),
  fCollectionName("fCollection"),
  fNeedUpdate(true)
{

}

void EXODefineCrossProductProcessList::SetFile(const std::string filename)
{
  if(filename != fFilename){
    fFilename = filename;
    fNeedUpdate = true;
  }
}

void EXODefineCrossProductProcessList::SetNSigma(double nsigma)
{
  if(nsigma != fNSigma){
    fNSigma = nsigma;
    fNeedUpdate = true;
  }
}

void EXODefineCrossProductProcessList::SetNBins(int nbins)
{
  if(nbins != fNBins){
    fNBins = nbins;
    fNeedUpdate = true;
  }
}

void EXODefineCrossProductProcessList::SetCollectionName(std::string val)
{
  if(val != fCollectionName){
    fCollectionName = val;
    fNeedUpdate = true;
  }
}

EXOReconProcessList EXODefineCrossProductProcessList::GetProcessList(const EXOReconProcessList& inputList) const
{
  if(fNeedUpdate){
    UpdateFilters();
    fNeedUpdate = false;
  }
  fWFMap.clear();
  EXOReconProcessList retList;
  if(inputList.GetSize() == 0){
    return retList;
  }
  
  inputList.ResetIterator();
  const EXOReconProcessList::WaveformWithType* wfWithType = NULL;
  while((wfWithType = inputList.GetNextWaveformAndType()) != NULL){
    const EXOWaveform* wf = wfWithType->fWf;

    // We only care about doing this for APD channels.
    if(wf->GetChannelType() != EXOMiscUtil::kAPDGang) continue;
    // Furthermore, we only want to do this for signals that behave like APD signals.
    if(wfWithType->fType != EXOReconUtil::kAPD) continue;

    int channel = wf->fChannel;
    EXODoubleWaveform doubleWf = wf->Convert<Double_t>();
    FilterMap::const_iterator filter = fFilterMap.find(channel);
    if(filter == fFilterMap.end()){
      stringstream str;
      str << "Filter for channel " << channel << " was not created";
      LogEXOMsg(str.str(),EEAlert);
    }
    filter->second.Transform(&doubleWf);
    EXOWaveform& filteredWf = fWFMap[channel];
    filteredWf.MakeSimilarTo(*wf);
    filteredWf.fChannel = wf->fChannel;
    filteredWf.Zero();
    filteredWf.SetData(doubleWf.GetData(),doubleWf.GetLength());
    retList.Add(filteredWf,EXOReconUtil::kAPD);
    ProcessVerbosity(*wf,filteredWf);
  }
  return retList;
}

void EXODefineCrossProductProcessList::ProcessVerbosity(const EXOWaveform& raw, const EXOWaveform& filtered) const
{
  static int histDumper = 0;
  if(!fVerbose.ShouldPlotForChannel(raw.fChannel)){
    return;
  }

  TH1D rawHist;
  TH1D filteredHist;
  raw.LoadIntoHist(rawHist);
  filtered.LoadIntoHist(filteredHist);
  filteredHist.SetLineColor(kRed);
  vector<TObject*> histVector;
  histVector.push_back(&rawHist);
  histVector.push_back(&filteredHist);

  if(fVerbose.ShouldPlotToScreenForChannel(raw.fChannel)){
    EXOMiscUtil::DisplayInProgram(histVector,"Drawing raw and cross-product-filtered waveform");
  }
  if(fVerbose.ShouldPlotToFileForChannel(raw.fChannel)){
    stringstream filename;
    filename << "CPFilter-" << histDumper << ".png";
    EXOMiscUtil::DumpHistToFile(histVector,filename.str(),"Drawing raw and cross-product-filtered waveform to file "+filename.str());
    histDumper++;
  }
}

void EXODefineCrossProductProcessList::UpdateFilters() const
{
  TFile file(fFilename.c_str(),"READ");
  if(file.IsZombie()){
    LogEXOMsg("Could not open file \"" + fFilename + "\". You must explicitely specify a filename via the corresponding command",EEAlert);
    return;
  }
  EXOCorrelationCollection* correlations = static_cast<EXOCorrelationCollection*>(file.Get(fCollectionName.c_str()));
  if(not correlations){
    LogEXOMsg("Could not find collection with name \"" + fCollectionName + "\" in file " + fFilename,EEAlert);
    return;
  }
  const int minchannel = NCHANNEL_PER_WIREPLANE*NWIREPLANE;
  const int maxchannel = minchannel + NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE;
  //cout << "minchannel = " << minchannel << endl;
  //cout << "maxchannel = " << maxchannel-1 << endl;
  if(minchannel < correlations->GetMinChannel() || correlations->GetMaxChannel() >= maxchannel){
    LogEXOMsg("Correlation collection does not contain all channels",EEError);
    return;
  }

  for(int channel = minchannel; channel < maxchannel; channel++){
    EXOWaveformFT spectrum;
    spectrum.MakeSimilarTo(correlations->GetCorrelation(minchannel,minchannel+1));
    spectrum.Zero();
    for(int otherchan = minchannel; otherchan < maxchannel; otherchan++){
      if(otherchan == channel){
        continue;
      }
      spectrum += correlations->GetCorrelation(channel,otherchan);
    }
    spectrum /= double(NAPDPLANE*NUMBER_APD_CHANNELS_PER_PLANE-1);
    /*
    cout << "Spectrum length = " << spectrum.GetLength() << endl;
    cout << "fNBins = " << fNBins << endl;
    cout << "fNSigma = " << fNSigma << endl;
    */
    fFilterMap[channel].SetSpectrum(spectrum,fNSigma,fNBins);
    //cout << "created filter for channel " << channel << endl;
  }
}

void EXODefineCrossProductProcessList::SetupTalkTo(const std::string& prefix, EXOTalkToManager* talkTo)
{
  talkTo->CreateCommand(prefix + "/file",
                        "Set the input file that contains the correlation collection",
                        this,
                        fFilename,
                        &EXODefineCrossProductProcessList::SetFile );

  talkTo->CreateCommand(prefix + "/collectionName",
                        "Set the name of the EXOCorrelationCollection object contained in the input file",
                        this,
                        fCollectionName,
                        &EXODefineCrossProductProcessList::SetCollectionName );

  talkTo->CreateCommand(prefix + "/binsPerBand",
                        "Set the number of bins per frequency band",
                        this,
                        fNBins,
                        &EXODefineCrossProductProcessList::SetNBins );

  talkTo->CreateCommand(prefix + "/numSigma",
                        "Set the number of sigma above mean for the filter",
                        this,
                        fNSigma,
                        &EXODefineCrossProductProcessList::SetNSigma );
}
