#ifndef EXOMatrix_hh
#define EXOMatrix_hh

#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include "TObject.h"

template <typename Type>
class EXOMatrix : public TObject
{
  public:
    EXOMatrix(size_t nrows = 0, size_t ncolumns = 0, const Type& initval = Type());
    std::vector<Type>& operator[](size_t row);
    const std::vector<Type>& operator[](size_t row) const;
    size_t GetNrows() const {return fData.size();}
    size_t GetNcols() const;
    void AddRow(const Type& initval = Type());
    void RemoveRow(size_t row);
    void AddColumn(const Type& initval = Type());
    void RemoveColumn(size_t column);
    void AddRowAndColumn(const Type& initval = Type());
    void RemoveRowAndColumn(size_t pos);
    bool IsSquare() const {return GetNrows() == GetNcols();}
    Type Max(size_t& row, size_t& column) const;
    Type Min(size_t& row, size_t& column) const;
    virtual void Print(Option_t* opt = "") const;

  protected:
    std::vector< std::vector<Type> > fData;

    ClassDef(EXOMatrix,1)
};

template <typename Type>
EXOMatrix<Type>::EXOMatrix(size_t nrows, size_t ncolumns, const Type& initval)
{
  fData.assign(nrows,std::vector<Type>(ncolumns,initval));
}

template <typename Type>
std::vector<Type>& EXOMatrix<Type>::operator[](size_t row)
{
  return fData[row];
}

template <typename Type>
const std::vector<Type>& EXOMatrix<Type>::operator[](size_t row) const
{
  return fData[row];
}

template <typename Type>
void EXOMatrix<Type>::AddRow(const Type& initval)
{
  fData.push_back(std::vector<Type>(GetNcols(),initval));
}

template <typename Type>
void EXOMatrix<Type>::RemoveRow(size_t row)
{
  assert(row < fData.size());
  fData.erase(fData.begin()+row);
}

template <typename Type>
void EXOMatrix<Type>::AddColumn(const Type& initval)
{
  size_t nrows = GetNrows();
  if(nrows == 0){
    fData.push_back(std::vector<Type>(1,initval));
    return;
  }
  for(size_t i=0; i<nrows; i++){
    fData[i].push_back(initval);
  }
}

template <typename Type>
void EXOMatrix<Type>::RemoveColumn(size_t column)
{
  for(size_t i=0; i<fData.size(); i++){
    assert(fData[i].size() > column);
    fData[i].erase(fData[i].begin()+column);
  }
}

template <typename Type>
void EXOMatrix<Type>::AddRowAndColumn(const Type& initval)
{
  AddRow(initval);
  AddColumn(initval);
}

template <typename Type>
void EXOMatrix<Type>::RemoveRowAndColumn(size_t pos)
{
  RemoveRow(pos);
  RemoveColumn(pos);
}

template <typename Type>
Type EXOMatrix<Type>::Max(size_t& row, size_t& column) const
{
  Type ret = std::numeric_limits<Type>::min();
  for(size_t i=0; i<GetNrows(); i++){
    for(size_t j=0; j<GetNcols(); j++){
      if(fData[i][j] > ret){
        ret = fData[i][j];
        row = i;
        column = j;
      }
    }
  }
  return ret;
}

template <typename Type>
Type EXOMatrix<Type>::Min(size_t& row, size_t& column) const
{
  Type ret = std::numeric_limits<Type>::max();
  for(size_t i=0; i<GetNrows(); i++){
    for(size_t j=0; j<GetNcols(); j++){
      if(fData[i][j] < ret){
        ret = fData[i][j];
        row = i;
        column = j;
      }
    }
  }
  return ret;
}

template <typename Type>
void EXOMatrix<Type>::Print(Option_t* opt) const
{
  size_t nrows = GetNrows();
  size_t ncols = GetNcols();
  size_t i,j;
  int digits = 10;
  std::string emptystring = "";
  for(int i=0; i<digits; i++){
    emptystring += " ";
  }
  std::cout << emptystring;
  for(size_t j=0; j<ncols; j++){
    std::cout << std::setfill(' ') << std::setw(digits+1) << j;
  }
  std::cout << std::endl;
  for(size_t i=0; i<nrows; i++){
    std::cout << std::setfill(' ') << std::setw(digits+1) << i;
    for(size_t j=0; j<ncols; j++){
      std::cout << std::setfill(' ') << std::setw(digits+1) << fData[i][j];
    }
    std::cout << std::endl;
  }
}

template <typename Type>
size_t EXOMatrix<Type>::GetNcols() const
{
  if(fData.size()){
    return fData[0].size();
  }
  return 0;
}

#endif
