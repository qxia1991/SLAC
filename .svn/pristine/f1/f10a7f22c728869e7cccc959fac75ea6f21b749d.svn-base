//______________________________________________________________________________
//                                                                        
// EXOMCPixelatedChargeDeposit 
//
// encapsulates info for a pixelated charge deposit in a MC run.
//
// WARNING:  A coordinate must be set, either using the constructor or the SetCoordinates function, BEFORE
// any of the other variables are set.  Also, once set, the coordinates must not be changed.
//
#include "EXOUtilities/EXOMCPixelatedChargeDeposit.hh"
#include "EXOUtilities/EXOMiscUtil.hh"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <iterator>

ClassImp( EXOMCPixelatedChargeDeposit )

EXOMCPixelatedChargeDeposit::EXOMCPixelatedChargeDeposit(const EXOCoordinates& Coord)
: TObject()
{
  EXOMCPixelatedChargeDeposit::Clear();
  fCoordinateKey = Coord.GetCoordinateKey(EXOMiscUtil::kUVCoordinates);
}

void EXOMCPixelatedChargeDeposit::Clear( Option_t* )
{
  // Initialize the contents of the charge deposit to show no hits have yet been registered to this object.
  fTotalEnergy = 0;
  fTotalIonizationEnergy = 0;
  fWireHitTime = 0;
  fDepositChannel = -999;
  fWeight = 1.0;
  fWireChannelsAffected.clear();
  fCoordinateKey.Clear();
  fAncestorParticleEnergy.clear();
  fAncestorParticleType.clear();
  fPCDHitChannels.clear();
  fPCDHitEnergy.clear();
  fNumPhot  = 0;
  fNumCompt = 0;
}

void EXOMCPixelatedChargeDeposit::SetCoordinates(const EXOCoordinates& Coord)
{
  // Set the coordinates of a pixel created using the default constructor.
  // WARNING:  This function must not be called if the coordinates have already been set.
  // If any of the variables in this class were set before a coordinate was defined, that was bad;
  // they will be cleared.
  assert(!fCoordinateKey.IsInitialized());

  Clear();
  fCoordinateKey = Coord.GetCoordinateKey(EXOMiscUtil::kUVCoordinates);
} 

void EXOMCPixelatedChargeDeposit::AddPCDUHit(int ch, double energy)
{
    std::vector<int>::iterator it;
    it=std::find(fPCDHitChannels.begin(), fPCDHitChannels.end(), ch);

    if(it!=fPCDHitChannels.end()){
        int ch_index = std::distance(fPCDHitChannels.begin(), it);
        //std::cout << "Found at " << ch_index << " ch " << fPCDHitChannels[ch_index] << " energy " << fPCDHitEnergy[ch_index] << std::endl;
        fPCDHitEnergy[ch_index] +=  energy;
    }
    else{
        //std::cout <<"Not Found " << ch << " " << energy <<std::endl;
        fPCDHitChannels.push_back(ch);
        fPCDHitEnergy.push_back(energy);   
    }
}

EXOCoordinates EXOMCPixelatedChargeDeposit::GetPixelCenter() const
{
  // Return the center of the pixel.
  // Use GetPixelCoordinateKey to access the raw pixel information.
  assert(fCoordinateKey.IsInitialized() and fCoordinateKey.CheckVersion());
  return fCoordinateKey.GetCenter();
}

const EXOCoordinateKey& EXOMCPixelatedChargeDeposit::GetPixelCoordinateKey() const
{
  // Return the EXOCoordinateKey of the pixel.
  // Generally it will be more convenient to use GetPixelCenter,
  // which gives the coordinates corresponding to the center of the pixel.
  // Note there is no guarantee that the EXOCoordinateKey has been initialized.
  assert(fCoordinateKey.CheckVersion());
  return fCoordinateKey;
}
