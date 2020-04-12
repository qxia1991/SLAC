#ifndef EXORdbException_hh
#define EXORdbException_hh
#include <exception>

class EXORdbException : public std::exception {
  public:
    EXORdbException(const std::string& extraInfo = "", int code=0) : 
      std::exception(),
      m_name("RdbException"), m_extra(extraInfo), m_code(code) {}
  virtual ~EXORdbException() throw() {}
  virtual std::string getMsg() {
    std::string msg = m_name + ": " + m_extra;
    return msg;}
  virtual int getCode() const { return m_code;}
  virtual const char* what() const throw() {
    return m_extra.c_str();
  }
protected: 
  std::string m_name;
private:
  std::string m_extra;
  int         m_code;
};
#endif
