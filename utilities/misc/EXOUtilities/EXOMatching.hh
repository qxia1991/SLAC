#ifndef EXOMatching_hh
#define EXOMatching_hh

#include "EXOUtilities/EXOMatrix.hh"


class EXOMatching
{
  public:
    EXOMatching();
    EXOMatching(int N, int cost);
    int hungarian(const EXOMatrix<int>& CostMatrix, int maxN, bool minimize=false);
    int GetMatchedY(int x) const;
    int GetMatchedX(int y) const;
    int GetN() const {return fN;}
    int GetCost(int x, int y) const;
    EXOMatrix<int> generate_random(int n, unsigned seed) const;
    void Print() const;

  protected:
    void init();
    void update_labels();
    void add_to_tree(int x, int prevx);
    void augment();

    int fN;
    int fMaxMatch;
    EXOMatrix<int> fCostMatrix;
    int fMaxElement;
    bool fMinimize;
    std::vector<int> fLX, fLY, fXY, fYX, fSlack, fSlackX, fPrev;
    std::vector<bool> fS, fT;
};

#endif
