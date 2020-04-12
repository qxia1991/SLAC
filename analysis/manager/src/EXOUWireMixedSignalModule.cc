//______________________________________________________________________________
#include "EXOAnalysisManager/EXOUWireMixedSignalModule.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOUtilities/EXOTalkToManager.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOCalibUtilities/EXOUWireGains.hh"
#include "EXOCalibUtilities/EXOElectronicsShapers.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOReconstruction/EXOSignalModelManager.hh"
#include "EXOReconstruction/EXOSignalFitter.hh"
#include "EXOReconstruction/EXOUWireSignalModelBuilder.hh"
#include "EXOReconstruction/EXOUWireIndSignalModelBuilder.hh"
#include "EXOReconstruction/EXOReconProcessList.hh"
#include "EXOReconstruction/EXOSignal.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include <iostream>
using namespace std;

IMPLEMENT_EXO_ANALYSIS_MODULE( EXOUWireMixedSignalModule, "umixedsignal" )

EXOUWireMixedSignalModule::EXOUWireMixedSignalModule():
    fElectronicsDatabaseFlavor("measured_times"),
    fUWireDatabaseFlavor("vanilla")
{

  //  cout << "constructing EXOUWireMixedSignalModule with name " << NAME << endl;

}

int EXOUWireMixedSignalModule::Initialize()
{

  return 0;
}

EXOAnalysisModule::EventStatus EXOUWireMixedSignalModule::BeginOfRun(EXOEventData *ED)
{

  cout << "At BeginOfRun for " << GetName() << ", run number = " << ED->fRunNumber 
       << " status = " << status << endl;

  return kOk;
}

double EXOUWireMixedSignalModule::CombineFit(double* x, double* p){
	
	func_coll->FixParameter(0,1);
	func_coll->SetParameter(1,p[0]);
	func_coll->SetParameter(2,p[1]);
	func_ind->FixParameter(0,1);
	func_ind->SetParameter(1,p[2]);
	func_ind->SetParameter(2,p[3]);
	
	double j = func_coll->Eval(x[0]) + func_ind->Eval(x[0]);
	return j;
}

EXOAnalysisModule::EventStatus EXOUWireMixedSignalModule::ProcessEvent(EXOEventData *ED)
{

  EXOWaveformData* wfd = ED->GetWaveformData();
  wfd->Decompress();
  //const EXOTClonesArray* uwa = ED->GetUWireSignalArray();
  //const EXOTClonesArray* uwia = ED->GetUWireInductionSignalArray();
  
  //string fElectronicsDatabaseFlavor = "measured_times";
  //string fUWireDatabaseFlavor = "vanilla";


  //Shaping times database
  const EXOElectronicsShapers* electronicsShapers = GetCalibrationFor(
      EXOElectronicsShapers,
      EXOElectronicsShapersHandler,
      fElectronicsDatabaseFlavor,
      ED->fEventHeader);

  //U-wire gains database
  //const EXOUWireGains* uwireGains = GetCalibrationFor(
      //EXOUWireGains,
      //EXOUWireGainsHandler,
      //fUWireDatabaseFlavor,
      //ED->fEventHeader);
	  
	//cout << "Event Number " << ED->fEventNumber << endl;
  int countermodulus = 0;
  //Loop over relevant fIsInduction tags
  size_t nuw = ED->GetNumUWireSignals();
  for(size_t i=0; i<nuw; i++){
    EXOUWireSignal* usig = ED->GetUWireSignal(i);
	//Get the induction tag channel 
    int channel = usig->fChannel;
	//only events tagged by the Induction module
	if(usig->fIsInduction == false){
		continue;
		}
	//these will be used as parameter guess in the fit
	double indE = 0.0;
	double indT = 0.0;
    for(size_t j=0; j<ED->GetNumUWireInductionSignals(); j++){
		EXOUWireInductionSignal* uisig = ED->GetUWireInductionSignal(j);
		int indchannel = uisig->fChannel;
		if(-1*(indchannel+500) == channel){
			indE = uisig->fMagnitude;
			indT = uisig->fTime;
			}
		}
	//If theses didn't change then it didn't find a UWireInductionSignal to match so move on
	if(indE == 0.0 && indT == 0.0){
		continue;
		}
	//use the largest neighboring uwire time for the initial guess
	double nE = 0.0;
	double nT = 0.0;
	for(size_t k=0; k<nuw; k++){
		EXOUWireSignal* nusig = ED->GetUWireSignal(k);
		int nchannel = nusig->fChannel;
		if((abs(nchannel - channel) ==1) && nusig->fRawEnergy > nE){
			nE = nusig->fRawEnergy;
			nT = nusig->fTime;
		}
	}
	//Get transfer function and gains for the channel then use them to get the collection and induction TF1's
    const EXOTransferFunction& tf1 = electronicsShapers->GetTransferFunctionForChannel(channel);
    //Double_t gain = uwireGains->GetGainOnChannel(channel) / 300.0;
	//cout << "gain = " << gain << endl;
	//cout << "RawEnergy = " << usig->fRawEnergy << ", CorEnergy = " << usig->fCorrectedEnergy << endl;
	
	//make the model manager and register an UandAPDExtractor
	EXOSignalModelManager fSignalModelManager;
	EXOSignalFitter fUandAPDExtractor;
	fSignalModelManager.AddRegisteredObject(&fUandAPDExtractor);
	
	//Build the Models for the coll and induct
	fSignalModelManager.BuildSignalModelForChannelOrTag(channel, EXOUWireSignalModelBuilder(tf1));
	fSignalModelManager.BuildSignalModelForChannelOrTag(-1*(500+channel), EXOUWireIndSignalModelBuilder(tf1));
	
	//Get the models
	const EXOSignalModel* sig_model_coll = fSignalModelManager.GetSignalModelForChannelOrTag(channel);
	const EXOSignalModel* sig_model_ind  = fSignalModelManager.GetSignalModelForChannelOrTag(-1*(500+channel));
	
	//Make the functions of the models
	//TF1& func_coll1  = sig_model_coll->GetFunction(0,2048e3,1);
	func_coll  = &(sig_model_coll->GetFunction(0,2048e3,1));
	//TF1& func_ind  = sig_model_ind->GetFunction(0,2048e3,1);
	func_ind  = &(sig_model_ind->GetFunction(0,2048e3,1));
	//cout << "coll name " << func_coll->GetName() << endl;
	//cout << "in name " << func_ind->GetName() << endl;
	
	//Set parameters for the two functions based on usig and induction signal;
	func_coll->FixParameter(0,1);
	func_coll->SetParameter(1,usig->fRawEnergy/5.61);
	//cout << "value 1 " << usig->fRawEnergy/gain << endl;
	func_coll->SetParameter(2,nT); //usig->fTime);
	//cout << "Time " << usig->fTime << endl;
	func_ind->FixParameter(0,1);
	func_ind->SetParameter(1, indE);
	func_ind->SetParameter(2,indT);
	
	//make a new function with the method to sum the collection and induction models
	function_combined = new TF1("function_combined",this,&EXOUWireMixedSignalModule::CombineFit,0,2048e3,4);
	//set the parameters of the new combined function
	function_combined->SetParameter(0,usig->fRawEnergy/5.61);
	function_combined->SetParameter(1,nT); //usig->fTime);
	function_combined->SetParameter(2,indE);
	function_combined->SetParameter(3,indT);
	
	//To use for diagnostic. Makes a plot of the functions before fits.
	//EXOMiscUtil::GetApplication();
	//static TCanvas c2; // = new TCanvas("c1","c1",600,600);
	//TCanvas& c1 = EXOMiscUtil::GetDebugCanvas();
	func_coll->SetLineColor(2);
	//func_coll->Draw();
	func_ind->SetLineColor(3);
	//func_ind->Draw("SAME");
	function_combined->SetLineColor(1);
	//function_combined->Draw("SAME");
	//c2.Update();
	//cin.get();
	
	//Make the waveform histogram relevant to IsInduction tagged usig
	EXOWaveform* wf = wfd->GetWaveformWithChannelToEdit(channel);
	EXODoubleWaveform dwf = EXODoubleWaveform(*wf);
	double baseline = usig->fBaseline; //(dwf.Sum(0,500))/500.0;
	double rms = usig->fBaselineError;
	//cout << "baseline " << baseline << endl;
	dwf -= baseline;
	TH1D* histdwf = dwf.GimmeHist();
	
	//Make New Hist with x values to be in nanoseconds for fitting 
	TH1D* histdwfnano = new TH1D("HistNano","",1,0,1);
	histdwfnano->Reset();
	histdwfnano->SetMaximum(-1111);
	histdwfnano->SetMinimum(-1111);
	histdwfnano->SetTitle(histdwf->GetName());
	double bin_width = 1000; //histdwf.GetSamplingPeriod()/CLHEP::microsecond;
	
	histdwfnano->SetBins(histdwf->GetNbinsX(), - 0.5*bin_width, histdwf->GetNbinsX()*bin_width - 0.5*bin_width);
	histdwfnano->SetXTitle("t [#ns]");
	histdwfnano->SetYTitle("ADC Units");
	histdwfnano->SetOption("L");
	
	for(int iSample = 0; iSample < histdwf->GetNbinsX(); iSample++) {
		histdwfnano->SetBinContent(iSample+1, histdwf->GetBinContent(iSample+1));
		histdwfnano->SetBinError(iSample+1,rms);
	}
	
	//fit the hist - baseline
	TFitResultPtr fitresult = histdwfnano->Fit(function_combined->GetName(),"S","",usig->fTime-100e3,usig->fTime+300e3);
	//TFitResultPtr fitresultind = histdwfnano->Fit(func_ind->GetName(),"S+");
	//TFitResultPtr fitresultcoll = histdwfnano->Fit(func_coll->GetName(),"S+");
	//cin.get();
	
	//Diagnostic. Makes plot of fits and waits for user input when running this module.
	//EXOMiscUtil::DisplayInProgram(*histdwfnano,"histdwfnano");
	//cout << "bin content = " << histdwfnano->GetBinContent(1) << " binerror = " << histdwfnano->GetBinError(1) << endl;
	//cout << "bin content = " << histdwfnano->GetBinContent(1000) << " binerror = " << histdwfnano->GetBinError(1000) << endl;
	//cout << "bin content = " << histdwfnano->GetBinContent(1125) << " binerror = " << histdwfnano->GetBinError(1125) << endl;
	
	//Access results to be saved
	// Double_t fitchi2 = fitresult->Chi2();
	// Double_t coll_par = fitresult->Parameter(0);
	// Double_t coll_err = fitresult->ParError(0);
	// Double_t coll_parT = fitresult->Parameter(1);
	// Double_t coll_errT = fitresult->ParError(1);
	// Double_t ind_par = fitresult->Parameter(2);
	// Double_t ind_err = fitresult->ParError(2);
	// Double_t ind_parT = fitresult->Parameter(3);
	// Double_t ind_errT = fitresult->ParError(3);
	//cout << "fit result " << fitchi2 << endl;
	//cin.get();
	
	//Save results to usig
	usig->fMixSigFitChi2 = fitresult->Chi2();								// Chi Squared result of the mixed signal fit
	usig->fMixSigColPar = fitresult->Parameter(0);								//Mixed Signal Collection Parameter
	//cout << "fitResult * gain = " << fitresult->Parameter(0) * gain << endl;
	usig->fMixSigColErr = fitresult->ParError(0);									//Mixed Signal Collection Error
	usig->fMixSigColTime = fitresult->Parameter(1);								//Mixed Signal Collection Time Par
	usig->fMixSigColErrTime = fitresult->ParError(1);						//Mixed Signal Collection Time Error
	usig->fMixSigIndPar = fitresult->Parameter(2);								//Mixed Signal Induction Parameter
	usig->fMixSigIndErr = fitresult->ParError(2);									//Mixed Signal Induction Error
	usig->fMixSigIndTime = fitresult->Parameter(3);								//Mixed Signal Induction Time Par
	usig->fMixSigIndErrTime = fitresult->ParError(3);
	//This is the value that will be corrected by later modules
	usig->fRawEnergy = fitresult->Parameter(0)*5.61;
	
	}//for loop


  return kOk;
}//process event

EXOAnalysisModule::EventStatus EXOUWireMixedSignalModule::EndOfRun(EXOEventData *ED)
{
  //  cout << "At EndOfRun for " << name << endl;
  return kOk;
}

int EXOUWireMixedSignalModule::TalkTo(EXOTalkToManager *talktoManager)
{

  // Create some example commands

  talktoManager->CreateCommand("/umixedsignal/ElectronicsDBFlavor",
                              "Set the flavor string used to query the database for electronics shaping times",
                              this,
                              fElectronicsDatabaseFlavor,
                              &EXOUWireMixedSignalModule::SetElectronicDatabaseFlavor );

  talktoManager->CreateCommand("/umixedsignal/UWireGainDatabaseFlavor",
                              "Set the flavor string used to query the database for U-Wire gains used to scale MC WFs.",
                              this,
                              fUWireDatabaseFlavor,
                              &EXOUWireMixedSignalModule::SetUWireDatabaseFlavor);

  // example double command

  talktoManager->CreateCommand("ANewDoubleCommand","read in a double", 
           this, 0.0, &EXOUWireMixedSignalModule::SetDoubleParam);

  // example int command

  talktoManager->CreateCommand("ANewIntCommand","read in an int", 
           this, 0, &EXOUWireMixedSignalModule::SetIntParam );
  
  // example bool command

  talktoManager->CreateCommand("ANewBoolCommand","read in a bool", 
           this, false, &EXOUWireMixedSignalModule::SetBoolParam );
  
  // example string command

  talktoManager->CreateCommand("ANewStringCommand","read in a string", 
           this, "", &EXOUWireMixedSignalModule::SetStringParam );

  // example generic call-back 

  talktoManager->CreateCommand("ANewCallBackCommand","generic call-back", 
           this, &EXOUWireMixedSignalModule::CallBack );

  return 0;
}

int EXOUWireMixedSignalModule::ShutDown()
{
  //  cout << "At ShutDown for " << name << endl;
  return 0;
}

void EXOUWireMixedSignalModule::CallBack()
{
  std::cout << "EXOUWireMixedSignalModule::CallBack() called. " << std::endl;
}


