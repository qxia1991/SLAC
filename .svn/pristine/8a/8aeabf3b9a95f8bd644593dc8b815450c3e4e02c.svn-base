//_________________________
//
// This class is intended to support the new module for fitting in the presence of noise;
// as such, it is rather specifically optimized.

// We save correlations between the real and imaginary components of the fourier-transformed noise.
// So, if N_i(f) is the fourier component of the noise on channel i with frequency f,
// then we want to save the expectation values:
// GetRR(f)[i][j] = < Re[N_i(f)] Re[N_j(f)] >
// GetRI(f)[i][j] = < Re[N_i(f)] Im[N_j(f)] >
// GetII(f)[i][j] = < Im[N_i(f)] Im[N_j(f)] >
//
// All three matrices are real-valued.  (In other words, x = Re[x] + i * Im[x].)
// fRR(f) and fII(f) are also symmetric.
// Since the full set of noise correlations is expected to be somewhat memory-intensive, we want a
// matrix class for fRR and fII which does not duplicate elements; ROOT fails there.
// So, SymmetricMatrix is written for this purpose.  It acts just as a container,
// with the convenience that it hides the manipulation of indices from the user.
// Since it seems aesthetically nice to have both matrix types include the same functionality,
// and these wrappers are so simple, a container class GeneralMatrix is also written to hold fRI.
//
// To create a noise correlation object:
// 1. You should first set the channel map using AddChannelToMap.
// 2. Set the number of frequency indices you will want using SetNumFrequencies.  This will
// automatically allocate memory for correlation matrices to fit the number of channels you have added.
// 3. You can access and modify entries like:
// rr = noise.GetRR(frequency)
// rr[i][j] = acorrelation
// This requires doing the conversion from channel number to index yourself, but GetIndexOfChannel
// may be helpful.
//
// ToDO: We'll be saving noise based on full 2048-length waveforms, but this class should provide
// some ability to interpolate for the benefit of shorter waveforms.

#include "EXOUtilities/EXONoiseCorrelations.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

void EXONoiseCorrelations::AddChannelToMap(UChar_t channel)
{
  // Add a single channel to the list of channels we are using.
  // We will assign the channel an index, and update the maps
  // fChannelToIndex and fIndexToChannel.
  // This function may be easier to use from pyROOT.
  // Be sure to call SetNumFrequencies after all channels are added.
  UChar_t NextIndex = UChar_t(fIndexToChannel.size());
  fChannelToIndex[channel] = NextIndex;
  fIndexToChannel.push_back(channel);

  // Ensure that fRR, fRI, fII are all kept clear
  // until a single set of allocations when SetNumFrequencies is called.
  fRR.clear();
  fRI.clear();
  fII.clear();
}

void EXONoiseCorrelations::SetNumFrequencies(unsigned short nfreq)
{
  // The number of frequencies we'd like to be able to access.
  // For a typical 2048-sample waveform, this should generally be
  // 1025 if the 0-frequency component is included, or
  // 1024 if the 0-frequency component is omitted.
  // We do not force any particular semantics on the frequency values -- that's up to the user.
  //
  // The channel list should already be initialized when this is called -- we ensure
  // here that matrices have the proper size.
  fRR.resize(nfreq);
  fII.resize(nfreq);
  fRI.resize(nfreq);

  size_t numChannels = fIndexToChannel.size();
  for(size_t i = 0; i < nfreq; i++) {
    fRR[i].SetSize(numChannels);
    fRI[i].SetSize(numChannels);
    fII[i].SetSize(numChannels);
  }
}

/*
Matrix types.

Both matrices are stored as a single vector.
The virtual method ConvertIndex translates an (i,j) pair into the one linear index.
All other methods are based on that.
*/

void EXONoiseCorrelations::Matrix::SetSize(UChar_t n)
{
  // Allocate space for a symmetric nxn matrix.
  fSize = n;
  size_t AllocSize = this->ConvertIndex(n-1, n-1) + 1;
  fData.resize(AllocSize);
}

size_t EXONoiseCorrelations::SymmetricMatrix::ConvertIndex(UChar_t i, UChar_t j) const
{
  // Internal function for converting indices to an internal index.
  UChar_t min = std::min(i, j);
  UChar_t max = std::max(i, j);
  return size_t(max)*(size_t(max)+1)/2 + size_t(min);
}

size_t EXONoiseCorrelations::GeneralMatrix::ConvertIndex(UChar_t i, UChar_t j) const
{
  // Internal function for converting indices to an internal index.
  return size_t(fSize)*size_t(i) + size_t(j);
}
