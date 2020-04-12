#ifndef EXOCalibBase_hh
#define EXOCalibBase_hh

// Base class for all calibration data classes

#include "EXOUtilities/EXOTimestamp.hh"
#include "EXOCalibUtilities/EXOCalibDataTypes.hh"
#include <iostream>

class EXOCalibBase {
  friend class EXOCalibHandlerBase; 

public:
  EXOCalibBase();
  EXOCalibBase(const EXOTimestamp& since, const EXOTimestamp& till,
               unsigned int serNo = 0);
  virtual ~EXOCalibBase() {}

  
  bool isValid() const; // check that internal timestamps at least make sense

  bool isValid(const EXOTimestamp& t) const; // Check if the data object is valid at the specified time
  const EXOTimestamp& validSince() const; // Get start of validity
  const EXOTimestamp& validTill() const;  // Get end of validity
    
  void setValidity(const EXOTimestamp& since, const EXOTimestamp& till); // Set validity range
    
  void setValiditySince(const EXOTimestamp& since); // Set start of validity
  void setValidityTill(const EXOTimestamp& till);   // Set end of validity

  void PrintValidTime(); // print [ start : end ] of validity
  
  unsigned int getSerNo() const {return m_serNo;}
  /// Get serial number of metadata row corresponding to calibration. 
  /// Can be used by clients to determine if object has been updated
  /// since last access.

  void setSerNo(unsigned int serial) {m_serNo = serial;}

  virtual void clear();

  void setFlavor(const std::string& flavor) {m_flavor = flavor;}

  std::string getFlavor() const {return m_flavor;}

  EXOCalib::METADATASOURCE getSource() const {return m_SourceOfData;}

protected:
    EXOTimestamp m_validSince; // Start of validity
    EXOTimestamp m_validTill;  // End of validity

    unsigned int m_serNo;
    std::string m_flavor;

    EXOCalib::METADATASOURCE m_SourceOfData; // Where did the data actually come from?

};
#endif
