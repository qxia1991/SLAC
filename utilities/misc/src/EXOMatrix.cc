#include "EXOUtilities/EXOMatrix.hh"

template <>
void EXOMatrix<int>::Print(Option_t* opt) const
{
  size_t nrows = GetNrows();
  size_t ncols = GetNcols();
  size_t i,j;
  int max = Max(i,j);
  if(max < 0){
    max = -max;
  }
  int digits = 1;
  while(max > 1){
    max /= 10;
    digits++;
  }
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

