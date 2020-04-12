#ifndef EXOMysqlResults_hh
#define EXOMysqlResults_hh
#include <vector>
#include <string>

typedef struct st_mysql_res MYSQL_RES;


class EXOMysqlResults {
  friend class EXOMysqlReadConnection;   

public:
  ~EXOMysqlResults();

  unsigned int getNRows() const;

  bool getRow(std::vector<std::string>& fields, unsigned int i = 0,
              bool clear = true) const;

    
  bool getRowPtrs(std::vector<std::string*>& fields, unsigned int i=0, 
                  bool clear=true) const;
  
  // The following has not been defined yet.
  //static void cleanFieldPtrs(std::vector<std::string*>& fields);

private:
  // Only EXOMysqlReadConnection calls constructor
  EXOMysqlResults(MYSQL_RES* results = 0); 

  // Disabling ability to do a deep copy.
  EXOMysqlResults(const EXOMysqlResults&);
  EXOMysqlResults& operator=(const EXOMysqlResults&);
  
  MYSQL_RES* m_myres;

};
#endif
