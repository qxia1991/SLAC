//______________________________________________________________________________
//   EXOCalibBase
// Implementation of base class for all calibration data objects Base class for
// all calibration data handlers Handlers must, at a minimum, be able to use
// the information stored in the metadata dbs about a particular calibration to
// be able to read the calibration and store it in memory.
//
// There are no requirements for derived EXOCalibBase classes.  In principle,
// they are purely data encapsulation classes.
//______________________________________________________________________________

#include "EXOCalibUtilities/EXOCalibBase.hh"

//______________________________________________________________________________
EXOCalibBase::EXOCalibBase()
{
  EXOCalibBase::clear();
}

//______________________________________________________________________________
EXOCalibBase::EXOCalibBase(const EXOTimestamp& since, const EXOTimestamp& till,
                           unsigned int serNo)
{
  EXOCalibBase::clear();
  m_validSince = since;
  m_validTill = till;
}

//______________________________________________________________________________
bool EXOCalibBase::isValid() const 
{
  return ((m_validSince != 0) && (m_validTill != 0)
          && (validSince() <= validTill())   );
}

//______________________________________________________________________________
bool EXOCalibBase::isValid (const EXOTimestamp& t) const {
  // tests fully closed time interval :: is inconsistent with our other calibration loading which are [a,b), most problematic for static loads, 
  if (!isValid()) return false;
  return validSince() <= t &&  t <= validTill(); 
}

void EXOCalibBase::PrintValidTime() {
  if (! isValid() ) 
    std::cout << "!isValid" << std::endl;
  else 
    std::cout << "isValid for [ "<< validSince().getString() << ", " << validTill().getString()  << " ]"<< std::endl;
  
}

//______________________________________________________________________________
const EXOTimestamp& EXOCalibBase::validSince() const 
{
  return m_validSince;
}

//______________________________________________________________________________
const EXOTimestamp& EXOCalibBase::validTill() const 
{
  return m_validTill;
}

//______________________________________________________________________________
void EXOCalibBase::setValidity(const EXOTimestamp& since, const EXOTimestamp& till) 
{
  setValiditySince(since);
  setValidityTill(till);
}

//______________________________________________________________________________
void EXOCalibBase::setValiditySince(const EXOTimestamp& since) 
{
  m_validSince = since;
}

//______________________________________________________________________________
void EXOCalibBase::setValidityTill(const EXOTimestamp& till) 
{
  m_validTill = till;
}

//______________________________________________________________________________
void EXOCalibBase::clear()
{
  m_serNo = 0;
  m_validSince = 0;
  m_validTill = 0;
  m_flavor = "";
  m_SourceOfData = EXOCalib::METADATASOURCEuninitialized;
}
