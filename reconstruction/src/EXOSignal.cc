#include "EXOReconstruction/EXOSignal.hh"
#include <iostream>
ClassImp(EXOSignal)

void EXOSignal::Print(Option_t* opt) const
{
  TObject::Print(opt);
  std::cout << "  Time (ns): " << fTime 
            << " +- " << fTimeError 
            << std::endl 
            << "  Magnitude: " << fMagnitude
            << " +- " << fMagnitudeError
            << std::endl 
            << "  Filtered WF Peak Magnitude: " << fFilteredWFPeakMagnitude
            << std::endl 
            << "  Filtered WF Peak Time: " << fFilteredWFPeakTime
            << std::endl; 
}
