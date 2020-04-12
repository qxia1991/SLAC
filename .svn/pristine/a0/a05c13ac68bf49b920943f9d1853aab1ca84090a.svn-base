#ifndef EXOSmearMCIonizationEnergy_hh
#define EXOSmearMCIonizationEnergy_hh

class EXOMonteCarloData;

class EXOSmearMCIonizationEnergy
{
  public:
    EXOSmearMCIonizationEnergy();
    void SetLXeEnergyResolution(double Resolution);
    double GetLXeEnergyResolution() const;
    void ApplySmear(EXOMonteCarloData& MonteCarloData);

  private:
    double fLXeEnergyResolution;
};
#endif
