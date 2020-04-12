#ifndef EXOCalibHandlerBase_hh
#define EXOCalibHandlerBase_hh

#include <string>
#include <vector>
#include <memory>
#include "EXOCalibUtilities/EXOCalibDataTypes.hh" 
class EXOMysqlResults;
class EXOCalibBase;

class EXOCalibHandlerBase {
  friend class EXOCalibManager;
  public:
    EXOCalibHandlerBase() {}
    virtual ~EXOCalibHandlerBase() {}
  
    // All derived classes will generate this by using the macro
    // DEFINE_EXO_CALIB_HANDLER 
    virtual const std::string GetHandlerType() = 0;
  
  private:
    EXOCalibBase* read(const std::string& type, 
                       const std::string& dataIdent,
                       const std::string& formatVersion,
                       EXOCalib::METADATASOURCE msrc);
  
  protected:
    virtual EXOCalibBase* readDB(const std::string& dataIdent,
                                 const std::string& formatVersion);
  
    virtual EXOCalibBase* readDefault(const std::string& dataIdent,
                                      const std::string& formatVersion);

    typedef std::vector<std::string> StrVec;
    //////////////////////////////////////////////////////////////////////////
    // RowResults is a light wrapping of EXOMysqlResults.  This abstracts
    // EXOMysqlResults from user classes that need results from a database.
    //////////////////////////////////////////////////////////////////////////
    class RowResults {
      public:
        RowResults();
        RowResults(const StrVec& res);
        ~RowResults();
#ifndef __CINT__
        // rootcint should ignore this function of the class.
        RowResults(std::auto_ptr<EXOMysqlResults> res);
#endif
        
        unsigned int getNRows() const;
        bool getRow(StrVec& fields, unsigned int i) const;

        RowResults(const RowResults& res);
        RowResults& operator=(const RowResults& res);
      private:
        std::auto_ptr<EXOMysqlResults> fRes; //! auto destructing pointer to EXOMysqlResults 
        StrVec fTable;                       //! pointer to table 
        std::string file_base;
    };
    
    RowResults GetDBRowsWith(const std::string& table,  // name of table
                             const StrVec& getCols,     // names of fields to return
                             const StrVec& orderCols,   // sort by what fields
                             const std::string& where); // selection criteria

};

//////////////////////////////////////////////////////////////////////////////
// The following provides code to handle automatic registration of calibration
// handlers.
//////////////////////////////////////////////////////////////////////////////
// Virtual class to build registered calibration handlers
class EXOVCalibHandlerBuilder {
  public:
    EXOVCalibHandlerBuilder( const std::string& name ); 
    virtual ~EXOVCalibHandlerBuilder();

    virtual EXOCalibHandlerBase* GetNewHandler() = 0;
    virtual void DeleteCalibHandler(EXOCalibHandlerBase* toDelete);
  
};

template <class T>
class EXOCalibBuilder : public EXOVCalibHandlerBuilder {
  public:
   EXOCalibBuilder( const std::string& name) : EXOVCalibHandlerBuilder(name) {} 
   virtual EXOCalibHandlerBase* GetNewHandler() { return new T(); }
  
};

/*! The following provides the *magic* automatic registration and must
    be called at the top of implementation (.cc) files.  This essentially works
    by exporting a global variable which is constructed automatically at run-time.  
    The constructor of EXOVCalibHandlerBuilder automatically registers
    itself, which is what we want. */
#define IMPLEMENT_EXO_CALIB_HANDLER(ClassName)                                \
EXOCalibBuilder< ClassName > gEXOCalibBuilder_##ClassName( ClassName::GetHandlerName() );

#define DEFINE_EXO_CALIB_HANDLER(nickname)                                     \
    public:                                                                    \
      static const std::string GetHandlerName() { return nickname; }                 \
      const std::string GetHandlerType() { return GetHandlerName(); }

#endif
