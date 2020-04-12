#ifndef EXORunInfo_hh
#define EXORunInfo_hh

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#include <string>
#include <set>
#include <map>

class EXOMetadata : public TObject {
  public:
    EXOMetadata(const std::string& key = "", const std::string& val = "") :
      fKey(key), fValue(val) {}

    const std::string& GetKey() const { return fKey; }
    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const { return fValue; }

    void Print(Option_t* = "") const;

  protected:
    friend class EXORunInfoManager;
    template<typename T> 
    T GetVal() const;

    std::string fKey;
    std::string fValue;

  ClassDef(EXOMetadata, 1) // Metadata item
};

class EXOMetadataSet: public TObject {
  public:
    const EXOMetadata* FindMetaData(const std::string& dataName) const;
    void Print(Option_t* = "") const;

  private:
    friend class EXORunInfoManager;
    typedef std::map<std::string, EXOMetadata> MDMap;
    MDMap fAllMetaData;

  ClassDef(EXOMetadataSet, 1) // Set of Metadata
};

class EXODataRunInfo: public TObject {
  public:
    const EXOMetadata* FindMetaData(const std::string& dataName) const
      { return fAllMD.FindMetaData(dataName); }

    Int_t GetRunNumber() const { return fRunNumber; }

    void Print(Option_t* = "") const;

  private:
    friend class EXORunInfoManager;
    EXOMetadataSet fAllMD;
    Int_t fRunNumber;

  ClassDef(EXODataRunInfo, 1) // Run information for *data* runs
};

class EXORunFile: public TObject {
  
  public:
    EXORunFile(const std::string& file = "") : fPhysicalFileLocation(file) {}

    const EXOMetadata* FindMetaData(const std::string& dataName) const
      { return fAllMD.FindMetaData(dataName); }
    const std::string& GetFileLocation() const { return fPhysicalFileLocation; }

    void Print(Option_t* = "") const;

  private:
    friend class EXORunInfoManager;
    typedef std::map<std::string, EXOMetadata> MDMap;
    std::string fPhysicalFileLocation;
    EXOMetadataSet fAllMD;

  ClassDef(EXORunFile, 1) // Run file

};

class EXORunInfo : public TObject {

  public:
    struct RLComp {
      bool operator()(const EXORunFile& left, const EXORunFile& right) const;
    };
    typedef std::set<EXORunFile, RLComp> RunList;
    
    const RunList& GetRunFiles() const { return fRunMapFiles; }
    Int_t GetRunNumber() const { return fRunNumber; }

    const std::string& GetDirectoryName() const { return fDirName; }

    void Print(Option_t* = "") const;

  private:
    friend class EXORunInfoManager;

    RunList fRunMapFiles;
    Int_t fRunNumber;
    std::string fDirName;
    
  ClassDef( EXORunInfo, 0 ) // Run information for all types of runs 
};

#endif /* EXORunInfo_hh */
