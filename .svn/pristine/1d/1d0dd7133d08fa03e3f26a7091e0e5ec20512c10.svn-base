#include "EXOCalibUtilities/EXOCalibManager.hh"
#include "EXOCalibUtilities/EXOEnergyCalib.hh"
#include "EXOCalibUtilities/EXOEnergyBias.hh"
#include "EXOCalibUtilities/EXOEnergyRatio.hh"
#include "EXOCalibUtilities/EXOThoriumPeak.hh"

#include <cmath>
#include <iostream>

using namespace std;

set<EXOEnergyCalib*> EXOEnergyCalib::fInstances;

EXOEnergyCalib::EXOEnergyCalib(string ratioFlavor, string peakFlavor, string biasFlavor)
: fRatioFlavor(ratioFlavor),
  fPeakFlavor(peakFlavor),
  fBiasFlavor(biasFlavor),
  fThTrueEnergy(2614.7),
  fWeeklyFlavor(""),
  fAverageFlavor("")
{
  fCalibrationType = "AverageRatioGausErfcFit";
}

EXOEnergyCalib::EXOEnergyCalib(string weeklyFlavor, string averageFlavor)
: fRatioFlavor(""),
  fPeakFlavor(""),
  fBiasFlavor(""),
  fThTrueEnergy(0.),
  fWeeklyFlavor(weeklyFlavor),
  fAverageFlavor(averageFlavor)
{
  fCalibrationType = "AverageMCBasedFit";
  //fRandom = new ROOT::Math::RandomTaus();
}

EXOEnergyCalib::~EXOEnergyCalib()
{
  fInstances.erase(this);
}

EXOEnergyCalib* EXOEnergyCalib::GetInstanceForFlavor(string ratioFlavor, string peakFlavor, string biasFlavor)
{
  for(set<EXOEnergyCalib*>::iterator iter=fInstances.begin(); iter != fInstances.end(); iter++){
    if((*iter)->fRatioFlavor != ratioFlavor){
      continue;
    }
    if((*iter)->fPeakFlavor != peakFlavor){
      continue;
    }
    if((*iter)->fBiasFlavor != biasFlavor){
      continue;
    }
    return *iter;
  }
  EXOEnergyCalib* newInstance = new EXOEnergyCalib(ratioFlavor,peakFlavor,biasFlavor);
  fInstances.insert(newInstance);
  return newInstance;
}

EXOEnergyCalib* EXOEnergyCalib::GetInstanceForFlavor(string weeklyFlavor, string averageFlavor)
{
  for(set<EXOEnergyCalib*>::iterator iter=fInstances.begin(); iter != fInstances.end(); iter++){
    if((*iter)->fWeeklyFlavor != weeklyFlavor){
      continue;
    }
    if((*iter)->fAverageFlavor != averageFlavor){
      continue;
    }
    
    return *iter;
  }
  EXOEnergyCalib* newInstance = new EXOEnergyCalib(weeklyFlavor,averageFlavor);
  fInstances.insert(newInstance);
  return newInstance;
}

const char* EXOEnergyCalib::GetCalibrationType() const
{
  return fCalibrationType.c_str();
}


double EXOEnergyCalib::CalibratedChargeEnergy(double charge, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return CalibratedChargeEnergy(charge, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::CalibratedChargeEnergy(double charge, int multiplicity, long int seconds, int nano) const
{
  return CalibratedChargeEnergy(charge, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::CalibratedChargeEnergy(double charge, int multiplicity,const EXOTimestamp& time) const
{
  double energy = 0;
  if(fCalibrationType == "AverageRatioGausErfcFit")
  {
    EXOEnergyRatio* ratio = GetCalibrationFor(EXOEnergyRatio,
                                              EXOEnergyRatioHandler,
                                              fRatioFlavor,
                                              time);
    
    EXOEnergyBias* bias = GetCalibrationFor(EXOEnergyBias,
                                            EXOEnergyBiasHandler,
                                            fBiasFlavor,
                                            time);

    EXOThoriumPeak* peak = GetCalibrationFor(EXOThoriumPeak,
                                             EXOThoriumPeakHandler,
                                             fPeakFlavor,
                                             time);
  
    energy = Calibration(charge,
                       peak->GetChargePeak(multiplicity),
                       bias->GetTh2615BiasCharge(multiplicity),
                       ratio->GetChargeCalibrationParameter(0,multiplicity),
                       ratio->GetChargeCalibrationParameter(1,multiplicity),
                       ratio->GetChargeCalibrationParameter(2,multiplicity));
  }
  else if(fCalibrationType == "AverageMCBasedFit")
  {
    EXOEnergyMCBasedFit* mcbasedFitWeekly = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fWeeklyFlavor, time);
    EXOEnergyMCBasedFit* mcbasedFitAverage = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);

    energy = MCBasedFitCalibration("Ionization",charge,mcbasedFitWeekly,mcbasedFitAverage,multiplicity);    
  }

  return energy;
}

double EXOEnergyCalib::CalibratedScintEnergy(double scint, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return CalibratedScintEnergy(scint, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::CalibratedScintEnergy(double scint, int multiplicity, long int seconds, int nano) const
{
  return CalibratedScintEnergy(scint, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::CalibratedScintEnergy(double scint, int multiplicity, const EXOTimestamp& time) const
{
  double energy = 0;
  if(fCalibrationType == "AverageRatioGausErfcFit")
  {
    EXOEnergyRatio* ratio = GetCalibrationFor(EXOEnergyRatio,
                                              EXOEnergyRatioHandler,
                                              fRatioFlavor,
                                              time);

    EXOThoriumPeak* peak = GetCalibrationFor(EXOThoriumPeak,
                                             EXOThoriumPeakHandler,
                                             fPeakFlavor,
                                             time);
  
    energy = Calibration(scint,
                         peak->GetScintillationPeak(multiplicity),
                         0,
                         ratio->GetScintCalibrationParameter(0,multiplicity),
                         ratio->GetScintCalibrationParameter(1,multiplicity),
                         ratio->GetScintCalibrationParameter(2,multiplicity));
  }
  else if(fCalibrationType == "AverageMCBasedFit")
  {
    EXOEnergyMCBasedFit* mcbasedFitWeekly = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fWeeklyFlavor, time);
    EXOEnergyMCBasedFit* mcbasedFitAverage = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);

    energy = MCBasedFitCalibration("Scintillation",scint,mcbasedFitWeekly,mcbasedFitAverage,multiplicity);    
  }

  return energy;
}

double EXOEnergyCalib::CalibratedRotatedEnergy(double charge, double scint, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return CalibratedRotatedEnergy(charge, scint, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::CalibratedRotatedEnergy(double charge, double scint, int multiplicity, long int seconds, int nano) const
{
  return CalibratedRotatedEnergy(charge, scint, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::CalibratedRotatedEnergy(double charge, double scint, int multiplicity, const EXOTimestamp& time) const
{
  double energy = 0;
  if(fCalibrationType == "AverageRatioGausErfcFit")
  {
    EXOEnergyRatio* ratio = GetCalibrationFor(EXOEnergyRatio, EXOEnergyRatioHandler, fRatioFlavor, time);
    EXOEnergyBias* bias = GetCalibrationFor(EXOEnergyBias, EXOEnergyBiasHandler, fBiasFlavor, time);
    EXOThoriumPeak* peak = GetCalibrationFor(EXOThoriumPeak, EXOThoriumPeakHandler, fPeakFlavor, time);

    double angle = peak->GetRotationAngle(multiplicity);
    double rotated = charge*cos(angle) + scint*sin(angle);
    double thPeak = peak->GetRotatedPeak(multiplicity);
    double thBias = bias->GetTh2615BiasRotated(multiplicity);
    double p0 = ratio->GetRotatedCalibrationParameter(0,multiplicity);
    double p1 = ratio->GetRotatedCalibrationParameter(1,multiplicity);
    double p2 = ratio->GetRotatedCalibrationParameter(2,multiplicity);
  
    energy = Calibration(rotated, thPeak, thBias, p0, p1, p2);
  }
  else if(fCalibrationType == "AverageMCBasedFit")
  {
    EXOEnergyMCBasedFit* mcbasedFitWeekly = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fWeeklyFlavor, time);
    EXOEnergyMCBasedFit* mcbasedFitAverage = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);

    double rotated = RawRotatedEnergy(charge,scint,multiplicity,time);
    energy = MCBasedFitCalibration("Rotated",rotated,mcbasedFitWeekly,mcbasedFitAverage,multiplicity);
  }

  return energy;  
}

double EXOEnergyCalib::RawRotatedEnergy(double charge, double scint, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return RawRotatedEnergy(charge, scint, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::RawRotatedEnergy(double charge, double scint, int multiplicity, long int seconds, int nano) const
{
  return RawRotatedEnergy(charge, scint, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::RawRotatedEnergy(double charge, double scint, int multiplicity, const EXOTimestamp& time) const
{
  double angle = 0;
  if(fCalibrationType == "AverageRatioGausErfcFit")
  {
    EXOThoriumPeak* peak = GetCalibrationFor(EXOThoriumPeak,
                                             EXOThoriumPeakHandler,
                                             fPeakFlavor,
                                             time);

    angle = peak->GetRotationAngle(multiplicity);
  }
  else if(fCalibrationType == "AverageMCBasedFit")
  {
    EXOEnergyMCBasedFit* mcbasedFit = GetCalibrationFor(EXOEnergyMCBasedFit,
                                                        EXOEnergyMCBasedFitHandler,
                                                        fWeeklyFlavor,
                                                        time);
    angle = mcbasedFit->GetParByName("Angle",multiplicity);
  }

  double rotated = charge*cos(angle) + scint*sin(angle);
  return rotated;
}

double EXOEnergyCalib::Calibration(double rawEnergy, double ThRaw, double bias, double p0, double p1, double p2) const
{
  double x = rawEnergy/ThRaw;
  return (fThTrueEnergy - bias) * (p0 + x*p1 + x*x*p2);
}

double EXOEnergyCalib::MCBasedFitCalibration(const std::string& channel, double energy, EXOEnergyMCBasedFit* weekly, EXOEnergyMCBasedFit* average, int multiplicity) const
{
  int nPar = 3;
  std::vector<double> weeklyP(nPar);
  for(int i = 0; i < nPar; i++)
    weeklyP[i] = weekly->GetParByName(Form("%s_Calibration_p%d",channel.c_str(),i),multiplicity);
  double weekEnergy = weeklyP.at(0) + weeklyP.at(1)*energy + weeklyP.at(2)*energy*energy;

  std::vector<double> avgP(nPar);
  for(int i = 0; i < nPar; i++)
    avgP[i] = average->GetParByName(Form("%s_Calibration_p%d",channel.c_str(),i),multiplicity);

  return avgP.at(0) + avgP.at(1)*weekEnergy + avgP.at(2)*weekEnergy*weekEnergy;
}

double EXOEnergyCalib::Angle2D(double energy, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return Angle2D(energy, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::Angle2D(double energy, int multiplicity, long int seconds, int nano) const
{
  return Angle2D(energy, multiplicity, EXOTimestamp(seconds,nano));
}


double EXOEnergyCalib::Angle2D(double energy, int multiplicity, const EXOTimestamp& time) const
{
  EXOEnergyMCBasedFit* mcbasedFitCampaign = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);

  int nPar = 3;
  std::vector<double> corrP(nPar);
  std::vector<double> resChargeP(nPar);
  std::vector<double> resScintP(nPar);
  for(int i = 0; i < nPar; i++)
  {
    corrP[i] = mcbasedFitCampaign->GetParByName(Form("Correlation_p%d",i),multiplicity);
    resChargeP[i] = mcbasedFitCampaign->GetParByName(Form("Ionization_Resolution_p%d",i),multiplicity);
    resScintP[i] = mcbasedFitCampaign->GetParByName(Form("Scintillation_Resolution_p%d",i),multiplicity);
  }

  double corr = corrP[0]*exp(-corrP[1]*energy/1000.) + corrP[2];
  double resCharge = sqrt(resChargeP[0]*resChargeP[0]*energy + resChargeP[1]*resChargeP[1] + resChargeP[2]*resChargeP[2]*energy*energy);
  double resScint = sqrt(resScintP[0]*resScintP[0]*energy + resScintP[1]*resScintP[1] + resScintP[2]*resScintP[2]*energy*energy);
  
  return atan(-2*corr*resScint*resCharge/(resScint*resScint - resCharge*resCharge))/2;
}
  
double EXOEnergyCalib::Rotated2D(double charge, double scint, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return Rotated2D(charge, scint, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::Rotated2D(double charge, double scint, int multiplicity, long int seconds, int nano) const
{
  return Rotated2D(charge, scint, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::Rotated2D(double charge, double scint, int multiplicity, const EXOTimestamp& time) const
{
  EXOEnergyMCBasedFit* mcbasedFitCampaign = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);
  double angle = mcbasedFitCampaign->GetParByName("Angle",multiplicity);

  return (charge*cos(angle) + scint*sin(angle))/(cos(angle) + sin(angle));
}

double EXOEnergyCalib::OrthogonalSigma2D(double energy, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return OrthogonalSigma2D(energy, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::OrthogonalSigma2D(double energy, int multiplicity, long int seconds, int nano) const
{
  return OrthogonalSigma2D(energy, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::OrthogonalSigma2D(double energy, int multiplicity, const EXOTimestamp& time) const
{
  EXOEnergyMCBasedFit* mcbasedFitCampaign = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);
  double rotAngle = mcbasedFitCampaign->GetParByName("Angle",multiplicity);
  double ortAngle = rotAngle + M_PI_2; // orthogonal = + 90deg (wrt to the rotated axis)
  double ortCos = cos(ortAngle);
  double ortSin = sin(ortAngle);
  
  int nPar = 3;
  std::vector<double> corrP(nPar);
  std::vector<double> resChargeP(nPar);
  std::vector<double> resScintP(nPar);
  for(int i = 0; i < nPar; i++)
  {
    corrP[i] = mcbasedFitCampaign->GetParByName(Form("Correlation_p%d",i),multiplicity);
    resChargeP[i] = mcbasedFitCampaign->GetParByName(Form("Ionization_Resolution_p%d",i),multiplicity);
    resScintP[i] = mcbasedFitCampaign->GetParByName(Form("Scintillation_Resolution_p%d",i),multiplicity);
  }

  double corr = corrP[0]*exp(-corrP[1]*energy/1000.) + corrP[2];
  double resCharge = sqrt(resChargeP[0]*resChargeP[0]*energy + resChargeP[1]*resChargeP[1] + resChargeP[2]*resChargeP[2]*energy*energy);
  double resScint = sqrt(resScintP[0]*resScintP[0]*energy + resScintP[1]*resScintP[1] + resScintP[2]*resScintP[2]*energy*energy);

  return sqrt(pow(ortCos*resCharge,2)+pow(ortSin*resScint,2) + 2*corr*ortSin*ortCos*resCharge*resScint);
}

double EXOEnergyCalib::Distance2D(double charge, double scint, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return Distance2D(charge, scint, multiplicity, seconds, 1000*microseconds);
}

double EXOEnergyCalib::Distance2D(double charge, double scint, int multiplicity, long int seconds, int nano) const
{
  return Distance2D(charge, scint, multiplicity, EXOTimestamp(seconds,nano));
}

double EXOEnergyCalib::Distance2D(double charge, double scint, int multiplicity, const EXOTimestamp& time) const
{
  EXOEnergyMCBasedFit* mcbasedFitCampaign = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);
  double angle = mcbasedFitCampaign->GetParByName("Angle",multiplicity);
  double absDist = fabs(scint - charge)/fabs(cos(angle)+sin(angle));
  
  double rotated = Rotated2D(charge,scint,multiplicity,time);
  double unit = OrthogonalSigma2D(rotated,multiplicity,time);
  
  return absDist/unit;
}

bool EXOEnergyCalib::Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, const EXOEventHeader& header) const
{
  int moveSeconds = header.fTriggerMicroSeconds / 1000000;
  long int seconds = header.fTriggerSeconds + moveSeconds;
  int microseconds = header.fTriggerMicroSeconds - (moveSeconds * 1000000);
  return Smeared2D(charge, scint, charge_mc, scint_mc, multiplicity, seconds, 1000*microseconds);
}

bool EXOEnergyCalib::Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, long int seconds, int nano) const
{
  return Smeared2D(charge, scint, charge_mc, scint_mc, multiplicity, EXOTimestamp(seconds,nano));
}

bool EXOEnergyCalib::Smeared2D(double& charge, double& scint, double charge_mc, double scint_mc, int multiplicity, const EXOTimestamp& time) const
{
  //ROOT::Math::RandomTaus r; --> GSL random engines do not work,
  // so implementing my own 2D gaussian generator based on Cholesky decomposition of covar  

  EXOEnergyMCBasedFit* mcbasedFitCampaign = GetCalibrationFor(EXOEnergyMCBasedFit, EXOEnergyMCBasedFitHandler, fAverageFlavor, time);

  int nPar = 3;
  std::vector<double> corrP(nPar);
  std::vector<double> resChargeP(nPar);
  std::vector<double> resScintP(nPar);
  for(int i = 0; i < nPar; i++)
  {
    corrP[i] = mcbasedFitCampaign->GetParByName(Form("Correlation_p%d",i),multiplicity);
    resChargeP[i] = mcbasedFitCampaign->GetParByName(Form("Ionization_Resolution_p%d",i),multiplicity);
    resScintP[i] = mcbasedFitCampaign->GetParByName(Form("Scintillation_Resolution_p%d",i),multiplicity);
  }
  
  double corr = corrP[0]*exp(-corrP[1]*charge_mc/1000.) + corrP[2];
  double resCharge = sqrt(resChargeP[0]*resChargeP[0]*charge_mc + resChargeP[1]*resChargeP[1] + resChargeP[2]*resChargeP[2]*charge_mc*charge_mc);
  double resScint = sqrt(resScintP[0]*resScintP[0]*scint_mc + resScintP[1]*resScintP[1] + resScintP[2]*resScintP[2]*scint_mc*scint_mc);
  
  double g1 = gRandom->Gaus();
  double g2 = gRandom->Gaus();

  charge = g1 * resCharge + charge_mc;
  scint = g1 * resScint * corr + g2 * resScint * sqrt(1-corr*corr) + scint_mc;
  
  return true;
}
