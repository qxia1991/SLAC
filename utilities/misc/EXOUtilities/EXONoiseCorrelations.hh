#ifndef EXONoiseCorrelations_hh
#define EXONoiseCorrelations_hh

#include "Rtypes.h"
#include "TObject.h"
#include <vector>
#include <map>

class EXONoiseCorrelations : public TObject
{
 public:

  class MatrixSlice;
  class ConstMatrixSlice;
  class Matrix : public TObject {
   public:
    Matrix() : fSize(0) {}
    MatrixSlice operator[](UChar_t row) { return MatrixSlice(*this, row); }
    const ConstMatrixSlice operator[](UChar_t row) const { return ConstMatrixSlice(*this, row); }
   protected:
    void SetSize(UChar_t n);
    std::vector<Double_t> fData;
    UChar_t fSize;
    virtual size_t ConvertIndex(UChar_t i, UChar_t j) const = 0;
   friend class EXONoiseCorrelations;
   ClassDef(Matrix, 1)
  };

  class SymmetricMatrix : public Matrix {
   protected:
    virtual size_t ConvertIndex(UChar_t i, UChar_t j) const;
   ClassDef(SymmetricMatrix, 1)
  };

  class GeneralMatrix : public Matrix {
   protected:
    virtual size_t ConvertIndex(UChar_t i, UChar_t j) const;
   ClassDef(GeneralMatrix, 1)
  };

  class MatrixSlice {
   public:
    MatrixSlice(Matrix& mat, UChar_t slice) : fMatrix(mat), fRow(slice) {}
    Double_t& operator[](UChar_t col) { return fMatrix.fData[fMatrix.ConvertIndex(fRow, col)]; }
    const Double_t& operator[](UChar_t col) const { return fMatrix.fData[fMatrix.ConvertIndex(fRow, col)]; }
   protected:
    Matrix& fMatrix;
    UChar_t fRow;
  };

  class ConstMatrixSlice {
   public:
    ConstMatrixSlice(const Matrix& mat, UChar_t slice) : fMatrix(mat), fRow(slice) {}
    const Double_t& operator[](UChar_t col) const { return fMatrix.fData[fMatrix.ConvertIndex(fRow, col)]; }
   protected:
    const Matrix& fMatrix;
    UChar_t fRow;
  };

  SymmetricMatrix& GetRR(unsigned short f) { return fRR[f]; }
  SymmetricMatrix& GetII(unsigned short f) { return fII[f]; }
  GeneralMatrix& GetRI(unsigned short f) { return fRI[f]; }

  void AddChannelToMap(UChar_t channel);
  void SetNumFrequencies(unsigned short nfreq);

  UChar_t GetNumChannels() const { return fChannelToIndex.size(); }
  bool HasChannel(UChar_t channel) const { return (fChannelToIndex.count(channel) > 0); }
  UChar_t GetChannelOfIndex(UChar_t index) const { return fIndexToChannel[index]; }
  UChar_t GetIndexOfChannel(UChar_t channel) const { return fChannelToIndex.at(channel); }

 protected:
  std::vector<SymmetricMatrix> fRR;
  std::vector<SymmetricMatrix> fII;
  std::vector<GeneralMatrix> fRI;
  std::map<UChar_t, UChar_t> fChannelToIndex;
  std::vector<UChar_t> fIndexToChannel;

 ClassDef(EXONoiseCorrelations, 1)
};
#endif
