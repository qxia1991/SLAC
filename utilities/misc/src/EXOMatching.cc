#include "EXOUtilities/EXOMatching.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

#include "TRandom.h"
#include <algorithm>
#include <limits>
#include <iostream>

using namespace std;
ClassImp(EXOMatching)

//____________________________________________________________________
//
//  EXOMatching
//
//  This class implements the 'Hungarian Method' for solving the 
//  weighted matching problem.
//  The algorithm uses an approach from graph theory and is taken
//  from here: http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=hungarianAlgorithm
//
//  the method hungarian() does all the work. It takes a cost matrix
//  as input and by default optimizes the matching so that the sum
//  of weights of the matching is maximum.
//  Set minimize=true to minimize instead.
//  
//  After the call to hungarian() the optimum matching can be obtained
//  via GetMatchedY() and GetMatchedX()
//____________________________________________________________________

EXOMatching::EXOMatching()
: fN(0),
  fMaxMatch(0),
  fMaxElement(0),
  fMinimize(false)
{

}

EXOMatching::EXOMatching(int N, int cost)
: fN(N),
  fMaxMatch(0),
  fCostMatrix(N,N,cost),
  fMaxElement(0),
  fMinimize(false)
{
  //Initialize a matching where X[0] --> Y[0], X[1] --> Y[1], ... , X[N-1] --> Y[N-1] and the cost of each link is 'cost'
  fXY.clear();
  fYX.clear();
  for(int i=0; i<N; i++){
    fXY.push_back(i);
    fYX.push_back(i);
  }
}

int EXOMatching::GetMatchedY(int x) const
{
  //Here x,X refers to the row-index and y,Y to the column-index of the cost matrix.
  //This function returns the matching y for a given x.
  return fXY[x];
}

int EXOMatching::GetMatchedX(int y) const{
  //Here x,X refers to the row-index and y,Y to the column-index of the cost matrix.
  //This function returns the matching x for a given y.
  return fYX[y];
}

void EXOMatching::init()
{
  fMaxMatch = 0;
  fXY.assign(fN,-1);
  fYX.assign(fN,-1);
  fLX.assign(fN,0);
  fLY.assign(fN,0);
  fSlack.assign(fN,0);
  fSlackX.assign(fN,0);

  for(int x=0; x<fN; x++){
    for(int y=0; y<fN; y++){
      fLX[x] = max(fLX[x], fCostMatrix[x][y]);
    }
  }
}

void EXOMatching::update_labels()
{
  int x,y, delta = numeric_limits<int>::max();
  for(y=0; y<fN; y++){
    if(!fT[y]){
      delta = min(delta,fSlack[y]);
    }
  }
  for(x=0; x<fN; x++){
    if(fS[x]){
      fLX[x] -= delta;
    }
  }
  for(y=0; y<fN; y++){
    if(fT[y]){
      fLY[y] += delta;
    }
  }
  for(y=0; y<fN; y++){
    if(!fT[y]){
      fSlack[y] -= delta;
    }
  }
}

void EXOMatching::add_to_tree(int x, int prevx)
{
  fS[x] = true;
  fPrev[x] = prevx;
  for(int y=0; y<fN; y++){
    if(fLX[x] +fLY[y] - fCostMatrix[x][y] < fSlack[y]){
      fSlack[y] = fLX[x] + fLY[y] - fCostMatrix[x][y];
      fSlackX[y] = x;
    }
  }
}

void EXOMatching::augment()
{
  if(fMaxMatch == fN){
    return;
  }
  int x, y, root=0, wr=0, rd=0;
  vector<int> q(fN,0);

  fS.assign(fN,false);
  fT.assign(fN,false);
  fPrev.assign(fN,-1);
  for(x=0; x<fN; x++){
    if(fXY[x] == -1){
      root = x;
      q[wr++] = x;
      fPrev[x] = -2;
      fS[x] = true;
      break;
    }
  }
  for(y=0; y<fN; y++){
    fSlack[y] = fLX[root] + fLY[y] - fCostMatrix[root][y];
    fSlackX[y] = root;
  }
  while(true){
    while(rd<wr){
      x = q[rd++];
      for(y=0; y<fN; y++){
        if(fCostMatrix[x][y] == fLX[x] + fLY[y] && !fT[y]){
          if(fYX[y] == -1) break;
          fT[y] = true;
          q[wr++] = fYX[y];
          add_to_tree(fYX[y],x);
        }
      }
      if(y < fN) break;
    }
    if(y < fN) break;

    update_labels();
    wr = rd = 0;
    for(y=0; y<fN; y++){
      if(!fT[y] && fSlack[y] == 0){
        if(fYX[y] == -1){
          x = fSlackX[y];
          break;
        }
        else{
          fT[y] = true;
          if(!fS[fYX[y]]){
            q[wr++] = fYX[y];
            add_to_tree(fYX[y],fSlackX[y]);
          }
        }
      }
    }
    if(y < fN) break;
  }
  if(y < fN){
    fMaxMatch++;
    for(int cx=x, cy=y, ty; cx != -2; cx = fPrev[cx], cy = ty){
      ty = fXY[cx];
      fYX[cy] = cx;
      fXY[cx] = cy;
    }
    augment();
  }
}

int EXOMatching::hungarian(const EXOMatrix<int>& CostMatrix, int maxN, bool minimize)
{
  fXY.clear();
  fYX.clear();
  int ret = 0;
  if(CostMatrix.GetNcols() != CostMatrix.GetNrows()){
    LogEXOMsg("Need square matrix as input!",EEError);
    return ret;
  }
  if(CostMatrix.GetNcols() == 0){
    return ret;
  }

  fMinimize = minimize;

  //Create working copy of cost matrix
  fCostMatrix = CostMatrix;
  fN = fCostMatrix.GetNcols();

  if(minimize){
    size_t dummy1,dummy2;
    fMaxElement = CostMatrix.Max(dummy1,dummy2);
    for(int x=0; x<fN; x++){
      for(int y=0; y<fN; y++){
        fCostMatrix[x][y] = fMaxElement - CostMatrix[x][y];
      }
    }
  }

  init();

  augment();

  //Use original Matrix for this
  for(int x=0; x<min(fN,maxN); x++){
    ret += CostMatrix[x][fXY[x]];
  }
  return ret;
}

int EXOMatching::GetCost(int x, int y) const
{
  if(fMinimize){
    return fMaxElement - fCostMatrix[x][y];
  }
  return fCostMatrix[x][y];
}

EXOMatrix<int> EXOMatching::generate_random(int n, unsigned seed) const
{
  TRandom rand(seed);
  EXOMatrix<int> mat(n,n);
  for(int x=0; x<n; x++){
    for(int y=0; y<n; y++){
      mat[x][y] = (int)rand.Uniform(0,1e6);
    }
  }

  return mat;
}

void EXOMatching::Print() const
{
  for(size_t x=0; x<fXY.size(); x++){
    cout << "X[" << x << "] ----> Y[" << fXY[x] << "]" << endl;
  }
}
