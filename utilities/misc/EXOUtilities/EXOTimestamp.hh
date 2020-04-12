#ifndef EXOTimestamp_hh
#define EXOTimestamp_hh

#include <string>
#include <exception>

// Exception class for timestamps
class EXOBadTimeInput : public std::exception {
public:
  EXOBadTimeInput(std::string msg) : std::exception(), complaint(msg) {};
  virtual ~EXOBadTimeInput() throw() {};
  std::string complaint; 
  virtual const char* what() const throw() {
    return complaint.c_str();
  }
  
  EXOBadTimeInput(const EXOBadTimeInput &other) : std::exception() {
    complaint = other.complaint; };
}; 


/**EXOTimestamp class, valid for dates from 1970 through 2037

  Supports comparisons

  Input to constructors may be
              seconds since start of 1970, Jan. 1 with optional
                  nanosecond field
              individual fields (year, month, etc.)
              string format            

           yyyy-mm-dd hh:mm:ss
                             1969 < yyyy < 2038
                          where 0 < mm < 13
                                0 < dd < 32
                               -1 < hh < 24
                               -1 < mm < 60 
                               -1 < ss < 60 

        o  only the first three fields are required.  Omitted trailing
           fields will be interpreted as equal to 0.
        o  by default : will be used to delimit fields, but user
           may specify an alternative in most circumstances
        o  leading zeros are optional

  */
class EXOTimestamp {
public:
  /// Default constructor builds object representing current time,
  /// expressed in GMT
  EXOTimestamp();

  EXOTimestamp(const EXOTimestamp& other);

  /// Count seconds from the creation-according-to-unix, start of 1970
  /// Optional third argument is offset in seconds from GMT 
  /// (e.g., PST is +28800)
  EXOTimestamp(long int seconds, int nano = 0, int tzOffset = 0);

  /** Create a timestamp from an ascii string of standard form
      yyyy-mm-dd hh:mm:ss
      where only the first three fields are required.   
      
      If the string is invalid, object will represent unix creation time.
      If the string represents a time in a timezone other than GMT,
      tzOffset should represent time zone offset relative to GMT in 
      seconds so if local time is, for example, PST, tzOffset should
      be 28800
  */
  EXOTimestamp(const std::string& str, int tzOffset = 0);

  /// Construct absolute time with specified fields
  EXOTimestamp(int year, int month, int day, 
               int hour = 0, int minute = 0, 
               int second = 0, int nano = 0);

  /// Return string representation of time, not including nanoseconds;
  std::string getString() const;

  int         getNano() const {return m_nano;}
  long int    getClibTime() const {return m_time;}

  bool operator<(const EXOTimestamp& other) const {
    return ((m_time < other.m_time) || 
            ((m_time == other.m_time) && (m_nano < other.m_nano)));
  }

  bool operator>(const EXOTimestamp& other) const {
    return (other < (*this));
    
  }

  bool operator<=(const EXOTimestamp& other) const {
    return ( !(other < (*this)) );
  }

  bool operator>=(const EXOTimestamp& other) const {
    return ( !( (*this) < other ) );
  }


  EXOTimestamp& operator= (const EXOTimestamp& other) {
    m_time = other.m_time; m_nano = other.m_nano; 
    return *this;
  }
  
  bool operator==(const EXOTimestamp& other) const{
    return ((m_time == other.m_time) && (m_nano == other.m_nano));
  }
  
  bool operator!=(const EXOTimestamp& other) const {
    return (!(*this == other));
  }

    //             << (for debugging)
  private:
  static const int    secPerDay;
  static const double  inverseNano;
  static const int     inverseNanoInt;
  static const long int maxInt;
  
  /// Representation of nominal start of time -- Jan 1, 1970 --
  /// will change between Visual Studio V6 and V7.  Time zone
  /// also has to be discovered dynamically, but only once.
  /// Do the computation in the constructor of a private class.
  class TZOffset {
    public:
    TZOffset();          
    long int m_tzseconds;     // 0 for GMT, 28800 for PDT, etc.
    int      m_isDst;   // Not yet sure if we need it
  };
  static  TZOffset s_tz;

  /// Return "standard" binary time: count in seconds since 1 jan 1970
  static time_t toBinary(const std::string& strTime); 

  /// Assemble string rep. from count of seconds
  static void toString(time_t bin, std::string& strTime);

protected:
  /// internal binary rep of time; count seconds from Jan 1, 1970
  time_t m_time;
  
  /// Save fractional seconds separately (associated with m_time)
  int  m_nano;
};

#endif
