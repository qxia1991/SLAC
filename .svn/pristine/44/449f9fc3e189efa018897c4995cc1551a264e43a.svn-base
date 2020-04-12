#ifndef EXOPowerSet_hh
#define EXOPowerSet_hh

#include "EXOUtilities/EXOErrorLogger.hh"
#include <set>
#include <algorithm>

template <typename Type>
class EXOPowerSet
{
  public:
    typedef typename std::set< std::set<Type> > SetOfSets;

    EXOPowerSet(size_t minOrder = 2);
    SetOfSets FindPowerSet(std::set<Type> mySet) const;
    void SetMinOrder(size_t val){fMinOrder = val;}

  protected:
    size_t fMinOrder;
};

template <typename Type>
EXOPowerSet<Type>::EXOPowerSet(size_t minOrder)
: fMinOrder(minOrder)
{

}

template <typename Type>
std::set< std::set<Type> > EXOPowerSet<Type>::FindPowerSet(const std::set<Type> mySet) const
{
  using namespace std;
  SetOfSets ret;
  if(mySet.size() < fMinOrder){
    LogEXOMsg("order of set smaller than minimum order!",EEError);
    return ret;
  }
  
  //If mySet has order fMinOrder, we're done
  if(mySet.size() == fMinOrder){
    ret.insert(mySet);
    return ret;
  }

  for(typename set<Type>::iterator it=mySet.begin(); it!=mySet.end(); it++){
    //Original set minus the element that 'it' points to
    set<Type> setMinusElement(mySet);
    setMinusElement.erase(*it);

    //Compute power set of this smaller set and insert it into the result
    //Also insert the elements of this smaller power set plus the deleted element.
    SetOfSets powerSet = FindPowerSet(setMinusElement);
    for(typename SetOfSets::iterator it2=powerSet.begin(); it2!=powerSet.end(); it2++){
      set<Type> next = *it2;
      ret.insert(next);
      next.insert(*it);
      ret.insert(next);
    }
  }
  return ret;
}

#endif
