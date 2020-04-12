//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOMiscUtil                                                          //
//                                                                      //
// Utilities for EXOAnalysis.  Also provides access to constants/names. //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "EXOUtilities/EXOMiscUtil.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOTransferFunction.hh"
#include "EXOUtilities/EXOControlRecordList.hh"

#include <sstream>
#include <iostream>
#include <cstdio>
#include "TChain.h"
#include "TROOT.h"
#include "TObject.h"
#include "TMessage.h"
#include "TClass.h"
#include "TSystem.h"
#include "TTimer.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TTimeStamp.h"
#include "TString.h"
#include "TBase64.h"
#include "Math/SMatrix.h"
#include "Math/SVector.h"
#include "TRandom3.h"
#include "TStreamerInfo.h"
#include <wordexp.h>
using std::string;
using std::cout;
using std::endl;
using std::vector;

/*
#ifdef WIN32
// For Sleep( ).   Not happy about this, but can't find anything else 
// that works.  All that's needed is WinBase.h, but compile fails 
// when that's all that is included.
#include <Windows.h>
#else
#include <unistd.h>
#endif
*/

/** @file Util.cxx 
@author J. Bogart
*/

//______________________________________________________________________________
int EXOMiscUtil::expandEnvVar(std::string* toExpand, 
                              const std::string& openDel,
                              const std::string& closeDel) {
  unsigned opLen = openDel.size();
  unsigned clLen = closeDel.size();
  int nSuccess = 0;
        
  int envStart = toExpand->find(openDel.c_str());
  while (envStart != -1) {
    int envEnd = toExpand->find(closeDel.c_str());
            
    // add  characters to account for opening delimiter
    int afterBracket = envStart + opLen;
            
    if (!( (envStart==-1) || (envEnd==-1) ))
    {
      std::string envVariable = 
        toExpand->substr(afterBracket,(envEnd-afterBracket));
      const char * path = ::getenv(envVariable.c_str());
      if (path) {
        toExpand->replace(envStart,(envEnd+clLen), path);
        if (nSuccess > -1) nSuccess++;
      }
      else {
        std::cerr << "EXOMiscUtil::expandEnvVar unable to translate " 
                  << envVariable << std::endl;
        throw EXOExceptUntranslatable(envVariable);
      }
    }
    envStart = toExpand->find(openDel.c_str());
  }
  return nSuccess;
}
    
//______________________________________________________________________________
int EXOMiscUtil::expandEnvVarOS(std::string* toExpand) {
#ifdef WIN32 
  return expandEnvVar(toExpand, "%", "%");
#else
  return expandEnvVar(toExpand, "${", "}" );
#endif
}

//______________________________________________________________________________
int EXOMiscUtil::expandEnvVarList(const std::vector<std::string>& toExpand,
                                  std::vector<std::string> &result,
                                  const std::string &delimiters) {
  int num=0;
  try {
    std::vector<std::string>::const_iterator listIt, listIt2;
    // iterate over the elements in the vector of strings
    for (listIt = toExpand.begin(); listIt != toExpand.end(); listIt++) {
      std::string tempStr = *listIt;
      num = expandEnvVar(&tempStr);
      std::vector<std::string> tempList;
      // find all the individual strings
      stringTokenize(tempStr, delimiters, tempList);
      // Save all the strings
      for (listIt2 = tempList.begin(); listIt2 != tempList.end(); listIt2++) 
        result.push_back(*listIt2);
    }
  } catch(std::exception& e) {
    throw(e);
  } catch(...) {
    throw;
  }
  return num;
}

//______________________________________________________________________________
const char* EXOMiscUtil::itoa(int val, std::string &outStr) {
  // Purpose and Method:  Provide a standard routine to convert integers
  //    into std::string.  The method used depends upon the availability of
  //    the stringstream classes.  The stringstream classes are the
  //    standard, but some compilers do yet support them. 
  //    [Oct. 24, 2005. Compiler upgrades some time ago eliminated this
  //    problem, so no longer need the #ifdef

  static char outCharPtr[20];

  std::ostringstream locStream;
  locStream.str("");

  locStream << val;

  outStr = locStream.str();
  strcpy(outCharPtr, outStr.c_str());
  return outCharPtr;
}

const char* EXOMiscUtil::utoa(unsigned int val, std::string &outStr, int base) {
  static char outCharPtr[20];

  std::ostringstream locStream;
  locStream.str("");
  if (base == 16) {
    locStream.fill('0');
    locStream.width(8);
    locStream.setf(std::ios::hex);
    locStream.unsetf(std::ios::dec);
  }
  locStream << val;

  outStr = locStream.str();
  strcpy(outCharPtr, outStr.c_str());
  return outCharPtr;
}
    
//______________________________________________________________________________
int EXOMiscUtil::atoi(const std::string& inStr) {
  // Purpose and Method:  Provide a standard routine to convert std::strings
  //    into integers.  The method used depends upon the availability of
  //    the stringstream classes.  The stringstream classes are the
  //    standard, but some compilers do yet support them.
  //    The method used is determined by the DEFECT_NO_STRINGSTREAM
  //    macro, defined in the facilities requirements file.
  // Output: returns an integer
  //    if string cannot be converted to an integer, returns zero

  int val;

  std::istringstream locStream(inStr);

  locStream >> val;
  if (!locStream) {return 0;}
  return val;
}  

//______________________________________________________________________________
double EXOMiscUtil::stringToDouble(const std::string& inStr) {
  // Extra test is because OSX Leopard converts 3. to a double (3)
  // and a string (.) while everyone else just gives a double (3.)
  double val;
  char  junk[3];
  int nItem = sscanf(inStr.c_str(), "%lg %1s", &val, junk);
  if ((nItem != 1) && (nItem != 2)) {
    throw EXOExceptWrongType(inStr, "double");
  }
  if (nItem == 2) {
    if (junk[0]!='.'){
      throw EXOExceptWrongType(inStr, "double");
    }
  }
  return val;
}


//______________________________________________________________________________
int EXOMiscUtil::stringToInt(const std::string& inStr) {
  int  val;
  char junk[3];
  int nItem = sscanf(inStr.c_str(), "%d %1s", &val, junk);
  if (nItem != 1) {
    throw EXOExceptWrongType(inStr, "int");
  }
  return val;
}

//______________________________________________________________________________
unsigned int EXOMiscUtil::stringToUnsigned(const std::string& inStr) {
  unsigned int  val;
  char junk[3];
  // %u format blithely wraps negative numbers around to positive ones, 
  // so check for that first
  if (inStr.find(std::string("-")) < inStr.size()) {
    throw EXOExceptWrongType(inStr, "unsigned int");
  }
  int nItem;
  if (inStr.find(std::string("0x")) < inStr.size()) {
    nItem = sscanf(inStr.c_str(), "%x %1s", &val, junk);
  }
  else {
    nItem = sscanf(inStr.c_str(), "%u %1s", &val, junk);
  }
  if (nItem != 1) {
    throw EXOExceptWrongType(inStr, "unsigned int");
  }
  return val;
}

//______________________________________________________________________________
unsigned long long EXOMiscUtil::stringToUll(const std::string& inStr) {
  unsigned long long  val;
  char junk[3];
  // %u format blithely wraps negative numbers around to positive ones, 
  // so check for that first
  if (inStr.find(std::string("-")) < inStr.size()) {
    throw EXOExceptWrongType(inStr, "unsigned long long");
  }
  int nItem;
  if (inStr.find(std::string("0x")) < inStr.size()) {
    nItem = sscanf(inStr.c_str(), "%llx %1s", &val, junk);
  } 
  else {
    nItem = sscanf(inStr.c_str(), "%llu %1s", &val, junk);
  }
  if (nItem != 1) {
    throw EXOExceptWrongType(inStr, "unsigned long long");
  }
  return val;
}


//______________________________________________________________________________
void EXOMiscUtil::stringTokenize(std::string input, const std::string& delimiters,
                                 std::vector<std::string>& tokens, bool clear) {
  if (clear) tokens.clear();

  std::string::size_type j;
  while ( (j = input.find_first_of(delimiters)) != std::string::npos ) {
    if (j != 0) tokens.push_back(input.substr(0, j));
    input = input.substr(j+1);
  }
  tokens.push_back(input);
  if (tokens.back() == "") tokens.pop_back();
}


//______________________________________________________________________________
void EXOMiscUtil::keyValueTokenize(std::string input, 
                                   const std::string& delimiters,
                                   std::map<std::string,std::string>& tokens, 
                                   const std::string& pairDelimiter,
                                   bool clear) {
  if (clear) tokens.clear();

  std::vector<std::string> strvec;
  stringTokenize(input,delimiters,strvec,true);
  unsigned advance = pairDelimiter.size();

  std::vector<std::string>::const_iterator input_itr = strvec.begin();
  while(input_itr!=strvec.end())
  {
    std::string current = *input_itr++;
    std::string::size_type j = current.find(pairDelimiter);
    std::string key   = current.substr(0, j);
    std::string value = current.substr(j+advance);
    tokens[key] = value;
  } 
}
  
//______________________________________________________________________________
std::string EXOMiscUtil::basename(const std::string& path) {
  std::vector<std::string> names;
  stringTokenize(path, "\\/", names);
  return *(names.end() - 1);
}


//______________________________________________________________________________
unsigned EXOMiscUtil::trimLeading(std::string* toTrim) {
  std::string::size_type first_char = toTrim->find_first_not_of(" \r\n\t\f");
  if (first_char == std::string::npos)
    return 0;
  *toTrim = toTrim->substr(first_char, std::string::npos);
  return first_char;
}


//______________________________________________________________________________
unsigned EXOMiscUtil::trimTrailing(std::string* toTrim) {
  static const char blank=' ';
  static const char LF=0xA;    // new line
  static const char FF=0xC;    // form feed
  static const char CR=0xD;    // carriage return

  unsigned orig = toTrim->size();
  unsigned last = orig - 1;
  bool notDone = true;
  unsigned nTrimmed = 0;


  while (notDone) {
    char lastChar = (*toTrim)[last];
    switch (lastChar) {
    case blank:
    case LF:
    case FF:
    case CR:
      last--;
      nTrimmed++;
      break;
    default:
      notDone=false;
      break;
    }
  }
  if (nTrimmed)  toTrim->resize(orig - nTrimmed);

  return nTrimmed;
}


//______________________________________________________________________________
unsigned EXOMiscUtil::trim(std::string* toTrim) {
  return EXOMiscUtil::trimTrailing(toTrim)
    + EXOMiscUtil::trimLeading(toTrim);
}

//______________________________________________________________________________
void EXOMiscUtil::gsleep(unsigned milli) {
  /*
  // System routines available vary with OS
  #ifdef WIN32 
  Sleep(milli);
  #else
  */
  unsigned sec = milli/((unsigned) 1000);
  unsigned micro = 1000 * (milli - 1000*sec);
  if (sec) sleep(sec);
  usleep(micro);
  /* #endif */
}

//______________________________________________________________________________
std::string EXOMiscUtil::SerializeTObject( TObject* object,
                                           bool useBase64,
                                           bool includeStreamerInfo)
{
   // Takes a TObject and serializes it into a string This is a utility function
   // to allow saving of ROOT objects directly in text files, or for example, in
   // DBs.

   if ( !object ) return "";
   TBufferFile s_buff( TBuffer::kWrite );

   if (includeStreamerInfo && !useBase64) {
       std::cerr << "Cannot include streamerInfo when not using base64 " << std::endl;
       return "";
   }

   // Use TBufferFile::WriteObjectAny for error checking
   if ( s_buff.WriteObjectAny( object, object->IsA() ) != 1 ) {
       std::cerr << "could not serialize object of type " <<
                     object->IsA()->GetName() << std::endl;
      return "";
   }

   // Hack back, to remove extra words in TMessage
   char* buf = s_buff.Buffer();
   size_t len = s_buff.Length();
   if ( ! useBase64) return std::string ( buf, len );
   std::string retString = std::string("EXOTBase64") +
     std::string(TBase64::Encode( buf, len ));
   if (!includeStreamerInfo) return retString;

#if ROOT_VERSION_CODE  >= ROOT_VERSION(5,34,0)
   // We use TMessage because it tracks the schema evolution, but we always try
   // to write with TBufferFile 
   TMessage* msg = new TMessage( TBuffer::kWrite );
   msg->EnableSchemaEvolution(true);
   msg->WriteObject(object);

   // Now remove the duplicate entries
   TList* buf_list = msg->GetStreamerInfos();
   std::set<Int_t> sent_list;
   TIter next_it(buf_list);
   TStreamerInfo* si;
   TList new_list;
   while( (si = dynamic_cast<TStreamerInfo*>(next_it()) ) ) {
     if (sent_list.find(si->GetNumber()) != sent_list.end()) continue;
     sent_list.insert(si->GetNumber());
     new_list.Add(si);
   }
   // delete to ensure nothing funny happens with ROOT globals
   delete msg;

   // Now we have a list that has only single entries
   if (new_list.GetSize() > 0) {
     return SerializeTObject(&new_list, true, false) + "," + retString;
   }
#else
   std::cerr << "ROOT version too old to include StreamerInfo" << std::endl;
#endif
   return retString;
}

//______________________________________________________________________________
TObject* EXOMiscUtil::UnserializeString( const std::string& bufferstr )
{
  // Takes a string and attempts to unserialize it, returning a
  // TObject.  Returns NULL if the Unserialization fails.

   // copy since std::string::data is a const void.
   std::string prefix = "EXOTBase64";
   size_t offset = 0;
   if ( bufferstr.compare( 0, prefix.size(), prefix ) == 0 ) {
     offset = prefix.size();
     // Now check if we need to load the streamer information
     size_t actual_obj = bufferstr.find("," + prefix, offset);
     if (actual_obj != std::string::npos) {
       // Load it
       TList* infoList =
         dynamic_cast<TList*>( UnserializeString(bufferstr.substr(0, actual_obj)) );
       if (!infoList) {
         std::cerr<< "Couldn't unserialize TStreamerInfo!" << std::endl;
         return 0;
       }
       TIter next(infoList);
       TStreamerInfo* sInfo;
       while ( (sInfo = dynamic_cast<TStreamerInfo*>(next())) ) {
         sInfo->BuildCheck();
       }
       offset = actual_obj + 1 + prefix.size();
     }
   }

   TString tempbuf;
   if ( offset != 0 ) {
     // We must first get rid of the base64 encoding
     tempbuf = TBase64::Decode( bufferstr.c_str() + offset );
   } else {
     tempbuf = bufferstr;
   }

   // First cd into the gROOT directory to make sure this object gets owned
   // *there*
   TDirectory* tmpDirectory = gDirectory; 
   gROOT->cd();
   TBufferFile buff( TBuffer::kRead, tempbuf.Length(), 
     const_cast<char*>(tempbuf.Data()), false );
   TObject* retObj = buff.ReadObject( 0 );
   tmpDirectory->cd();

   return retObj;
}

//______________________________________________________________________________
int EXOMiscUtil::delta_compression( int *data, int nsample, unsigned short *qdata, int maxqdata, int nbit  )
{
/* delta compression algorithm.  data is an array of 12-bit data stored
   in 32-bits.  This algorithm will not otherwise work! */
  int i;
  int nqele;
  unsigned short afewbits;
  unsigned short word;
  int index, delta[5];
  int full_compress;
  int compress_factor;
  int delta_ul, delta_ll;
  unsigned short encodemask;

  if ( maxqdata < nsample ) {
    printf("error 1\n");
    return -1;
  }

  if ( nsample < 1 ) return 0;
  if ( maxqdata < 1 ) return 0;

  if ( nbit == 3 ) {
    compress_factor = 5;
    delta_ul = 4;
    delta_ll = -3;
    encodemask = 0x0007;
  }
  else if ( nbit == 5 ) {
    compress_factor = 3;
    delta_ul = 16;
    delta_ll = -15;
    encodemask = 0x001F;
  }
  else {
    printf("nbit should be 3 or 5\n");
    return -1;
  }

  // First sample is always absolute

  nqele = 0;
  qdata[0] = data[0];
  qdata[0] = qdata[0] | 0x8000;
  nqele++;

  // look at the rest of the data in sets of compress_factor

  for ( i = 0; i < compress_factor; i++ ) delta[i] = 0;

  index = 1;
  while ( nsample - index >= compress_factor ) {

    if ( nqele >= maxqdata ) {
      printf("error 2\n");
      return -1;
    }

    full_compress = 1;
    for ( i = 0; i < compress_factor; i++ ) {
      delta[i] = data[index+i] - data[index+i-1];
      if ( delta[i] > delta_ul || delta[i] < delta_ll ) full_compress = 0;
    }
    
    if ( full_compress == 0 ) {

      // See if we can pack the next two into one word

      if ( delta[0] >= -63 && delta[0] <= 64 &&
	   delta[1] >= -63 && delta[1] <= 64 ) {

	// OK, use 7 bit deltas for these two  
	
	word = 0;
	afewbits = delta[0] + 63;
	afewbits = afewbits << 7;
	word = word | afewbits;
	afewbits = delta[1] + 63;
	word = word | afewbits;
	word = word | 0xC000; // Set bits 14 and 15 to one for this case
	qdata[nqele] = word;

	nqele++;
	index += 2;
	continue;

      }

      else {

	// in this case write out the full 12 bits for the first data sample

	word = data[index];
	word = word & 0x0FFF;
	word = word | 0x8000; // Make sure only bit 15 is set to one for this case
	qdata[nqele] = word;

	nqele++;
	index++;
	continue;

      }

    } // full compression didn't work
    
    // Write these data points into a single 16 bit word
    
    word = 0;
    afewbits = 0;
    for ( i = 0; i < compress_factor; i++ ) {
      afewbits = delta[i] - delta_ll;
      afewbits = afewbits  & encodemask;
      afewbits = afewbits << nbit*(compress_factor-1-i);
      word = word | afewbits;
    }
    
    // Make sure that bit 15 is zero
    
    word = word & 0x7FFF;
    
    // Copy the deltas into qdata
    
    qdata[nqele] = word;
    nqele++;
    index += compress_factor;
  
  }

  // Write the remaining values as absolute

  while ( nsample - index > 0 ) {
    if ( nqele >= maxqdata ) return -1;
    qdata[nqele] = data[index];
    qdata[nqele] = qdata[nqele] | 0x8000;
    nqele++;
    index++;
    continue;
  }

  return nqele;

}

//______________________________________________________________________________
int EXOMiscUtil::delta_uncompression( unsigned short *qdata, int qlength, int *data, int maxdata, int nbit )
{

  int i;
  int nqele;
  unsigned short afewbits;
  unsigned short word;
  int delta;
  int nsample;
  unsigned short encodemask[5];
  unsigned short partial_compression, unpack_sevenbit;
  int compress_factor;
  int delta_ll;

  compress_factor = 0;
  delta_ll = 0;
  if ( nbit == 3 ) {
    compress_factor = 5;
    encodemask[0] = 0x7000;
    encodemask[1] = 0x0E00;
    encodemask[2] = 0x01C0;
    encodemask[3] = 0x0038;
    encodemask[4] = 0x0007;
    delta_ll = -3;
  }
  else if ( nbit == 5 ) {
    compress_factor = 3;
    encodemask[0] = 0x7C00;
    encodemask[1] = 0x03E0;
    encodemask[2] = 0x001F;
    delta_ll = -15;
  }    
  else {
    printf("nbit should be 3 or 5\n");
    return -1;
  }

  if ( qlength <= 0 ) return 0;
  if ( maxdata <= 0 ) return 0;

  // First word is always absolute

  nsample = 0;
  nqele = 0;
  word = qdata[nqele] & 0x7FFF;
  data[nsample] = word;
  nsample = 1;
  nqele = 1;

  partial_compression = 0;
  unpack_sevenbit = 0;

  while ( nqele < qlength ) {

    if ( nsample >= maxdata ) {
      printf("error 1, nsample = %d, maxdata = %d, nqele = %d, qlength = %d\n", nsample, maxdata, nqele, qlength );
      return -1;
    }

    // See if this qdata is not fully compressed

    partial_compression = qdata[nqele] & 0x8000;

    if ( partial_compression > 0 ) {

      // See if two words are packed into this word

      unpack_sevenbit = qdata[nqele] & 0x4000;

      if ( unpack_sevenbit > 0 ) {

	// Unpack the two data samples stored here as 7 bit deltas

	word = qdata[nqele];
	word = word & 0x3F80;
	word = word >> 7;
	data[nsample] = data[nsample-1] + word - 63;
	nsample++;

	word = qdata[nqele];
	word = word & 0x007F;
	data[nsample] = data[nsample-1] + word - 63;
	nsample++;

	nqele++;
	continue;

      }

      else {

	// In this case we just have one 12 bit sample stored here

	word = qdata[nqele] & 0x7FFF;
	data[nsample] = word;
	nqele++;
	nsample++;
	continue;

      }

    }

    // If full compression was used, unpack the values from this word

    for ( i = 0; i < compress_factor; i++ ) {
      word = qdata[nqele];
      afewbits = word & encodemask[i];
      afewbits = afewbits >> nbit*(compress_factor-1-i);
      delta = afewbits;
      data[nsample] = data[nsample-1] + delta + delta_ll;
      nsample++;
    }
    nqele++;

  }

  return nsample;

}

//______________________________________________________________________________
void EXOMiscUtil::RC_shaper( Double_t *signal_in, Double_t *signal_out, Int_t n, 
		Double_t tratio )

{

  // Given an input signal, calculates the output signal for an RC shaper.
  // n = number of samples
  // tratio = sample_time/RC_time

  Double_t w = exp( -1.0*tratio );
  Double_t d1, d0;

  signal_out[0] = signal_in[0];
  d0 = 0.0;
  for ( Int_t i = 1; i < n; i++ ) {
    d1 = signal_in[i] - signal_in[i-1] + d0*w;
    signal_out[i] = signal_in[i]*(1.0-w) + signal_in[i-1]*w - 
      d0*w*w;
    d0 = d1;
  }

}

//______________________________________________________________________________
void EXOMiscUtil::CR_shaper( Double_t *signal_in, Double_t *signal_out, Int_t n, 
		Double_t tratio, Double_t baseline )

{

  // Given an input signal, calculates the output signal for a CR shaper.
  // n = number of samples
  // tratio = sample_time/CR_time

  Double_t w = exp( -1.0*tratio );

  signal_out[0] = signal_in[0];
  for ( Int_t i = 1; i < n; i++ ) {
    signal_out[i] = baseline + signal_in[i] - signal_in[i-1] +
      (signal_out[i-1] - baseline)*w;
  }

}

// Numerical recipes real fourier transform method

//______________________________________________________________________________
void EXOMiscUtil::realft(float data[], unsigned long n, int isign)
{
	unsigned long i,i1,i2,i3,i4,np3;
	float c1=0.5,c2,h1r,h1i,h2r,h2i;
	double wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793/(double) (n>>1);
	if (isign == 1) {
		c2 = -0.5;
		EXOMiscUtil::four1(data,n>>1,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	np3=n+3;
	for (i=2;i<=(n>>2);i++) {
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		EXOMiscUtil::four1(data,n>>1,-1);
	}
}

// Another numerical recepies routine. This one actually does the transform.

//______________________________________________________________________________
void EXOMiscUtil::four1(float data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			std::swap(data[j],data[i]);
			std::swap(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

//______________________________________________________________________________
void EXOMiscUtil::UVToXYCoords(double u, double v, double& x, double& y, double z)
{
  // Convert U, V, to X, Y coordinates.

  y = (u+v)/sqrt(3.0);  
  if( z > 0.0 ) x = v - u;
  else x = u - v;
} 

//______________________________________________________________________________
void EXOMiscUtil::XYToUVCoords(double& u, double& v, double x, double y, double z)
{
  // Convert X, Y, to U, V coordinates.

  if ( z > 0.0 ) {                                                    
    u =  -0.5*x + 0.5*sqrt(3.0)*y;                   
    v =  0.5*x + 0.5*sqrt(3.0)*y;                    
  } else {
    u = 0.5*x + 0.5*sqrt(3.0)*y;                     
    v = -0.5*x + 0.5*sqrt(3.0)*y;                    
  }   
} 

//______________________________________________________________________________
TApplication &EXOMiscUtil::GetApplication()
{
  // Returns the current application, instantiating an application if
  // necessary.
  if (!gApplication) new TApplication("EXOAnalysis", 0, 0);
  return *gApplication;
}
//______________________________________________________________________________
TCanvas &EXOMiscUtil::GetDebugCanvas() 
{ 
  // Gets a debug canvas that is used by DisplayInProgram.
  // If gApplication is not set, this means we're running in EXOAnalysis and
  // need to instantiate a TApplication.  We need to do this *before*
  // instantiating TCanvas, otherwise ROOT doesn't load X11 libraries.
  GetApplication(); // Force application to be created. 
  static TCanvas c1; 
  return c1; 
}

//______________________________________________________________________________
void EXOMiscUtil::ShowDebugPrompt()
{
  // Set up a timer so that the window updates.
  TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
  std::cout << "******************************************************" << std::endl
            << "Type 'Enter' to continue program,"                      << std::endl 
            << "  'q' then 'Enter' to quit completely"                  << std::endl  
            << "******************************************************" << std::endl;
  timer.TurnOn();
  timer.Reset();
  char c = std::cin.get();
  if (c == 'q') {
    // User requested an exit, get out.  
    // FixME? This doesn't allow cleanup? 
    timer.TurnOff();
    gSystem->Exit(0);
  } 
  timer.TurnOff();
}

//______________________________________________________________________________
void EXOMiscUtil::CR_unshaper( Double_t *signal_in, const Double_t *signal_out, Int_t n, 
		  Double_t tratio, Double_t baseline )

{

  // Given an output signal from an CR shaper, calculates the input signal.
  // n = number of samples
  // tratio = sample_time/CR_time

  Double_t w = exp( -1.0*tratio );

  signal_in[0] = signal_out[0];
  for ( Int_t i = 1; i < n; i++ ) {
    signal_in[i] = signal_out[i] + signal_in[i-1] - signal_out[i-1]*w - 
      baseline*(1.0-w);
  }

}

//______________________________________________________________________________
// signal_in[0] should have already been set as an unknown.
// parameter 0 will be used as the correction to baseline_guess.
void EXOMiscUtil::CR_unshaper( std::vector < ROOT::Math::SVector<double,4> > &signal_in,
                               std::vector < ROOT::Math::SVector<double,4> > &signal_out, 
                               Int_t n, Double_t tratio, Double_t baseline_guess )
{

  // Given an output signal from an CR shaper, calculates the input signal.
  // n = number of samples
  // tratio = sample_time/CR_time

  Double_t w = exp( -1.0*tratio );
  ROOT::Math::SVector<double,4> baseline(baseline_guess*(1.0-w), 1.0-w, 0.0, 0.0);

  for ( Int_t i = 1; i < n; i++ ) {
    ( ( ( (signal_in[i] = signal_out[i-1]) *= (-1.0*w) ) += signal_out[i] ) += signal_in[i-1] ) -= baseline;
  }
}


//______________________________________________________________________________
void EXOMiscUtil::RC_unshaper( Double_t *signal_in, const Double_t *signal_out, Int_t n, 
		  Double_t tratio )

{

  // Given an output signal from an RC shaper, calculates the input signal.
  // n = number of samples
  // tratio = sample_time/RC_time

  for ( Int_t i = 0; i < n; i++ ) signal_in[i] = 0.0;

  Double_t w = exp( -1.0*tratio );
  if ( w >= 1.0 ) {
    std::cout << "RC_unshaper: tratio shouldn't be zero" << std::endl;
    return;
  }
  Double_t d1, d0;

  signal_in[0] = signal_out[0];
  d0 = 0.0;
  for ( Int_t i = 1; i < n-1; i++ ) {
    signal_in[i] = (signal_out[i+1] - signal_in[i-1]*w + d0*w*w)/(1.0-w);
    d1 = signal_in[i] - signal_in[i-1] + d0*w;
    d0 = d1;
  }

  // shift it forward by one bin
  for ( Int_t i = n-1; i > 0; i-- ) {
    signal_in[i] = signal_in[i-1];
  }

}

//______________________________________________________________________________
// signal_in[0] should have already been set as an unknown
void EXOMiscUtil::RC_unshaper( std::vector < ROOT::Math::SVector<double,4> > &signal_in, 
                  std::vector < ROOT::Math::SVector<double,4> > &signal_out,
                  Int_t n, Double_t tratio )
{

  // Given an output signal from an RC shaper, calculates the input signal.
  // n = number of samples
  // tratio = sample_time/RC_time

  Double_t w = exp( -1.0*tratio );

  for ( Int_t i = 1; i < n; i++ ) {
    ( ( (signal_in[i] = signal_out[i-1]) *= (-1.0*w) ) += signal_out[i]) /= (1.0-w);
  }
}

//______________________________________________________________________________
Double_t* EXOMiscUtil::unshape_with_parameters_impl ( const Int_t* shaped_waveform, Int_t& length, Int_t& increment_forward,
              const EXOTransferFunction& transfer, Double_t baseline )
{

  // This function isn't really written to be generic, so issue preconditions.
  assert(transfer.GetNumDiffStages() == 3 and transfer.GetNumIntegStages() == 2);
  Double_t diff_ratio_1 = SAMPLE_TIME / transfer.GetDiffTime(0);
  Double_t diff_ratio_2 = SAMPLE_TIME / transfer.GetDiffTime(1);
  Double_t diff_ratio_3 = SAMPLE_TIME / transfer.GetDiffTime(2);
  Double_t int_ratio_1 = SAMPLE_TIME / transfer.GetIntegTime(0);
  Double_t int_ratio_2 = SAMPLE_TIME / transfer.GetIntegTime(1);

  std::vector < ROOT::Math::SVector<double,4> > shaped_withparameters(length);
  std::vector < ROOT::Math::SVector<double,4> > unshaped_withparameters(length);
  for ( Int_t i = 0; i < length; i++ ) {
    shaped_withparameters[i][0] = (Double_t)(shaped_waveform[i]);
  }

  // Correct for the shaper in terms of unknown parameters 1,2,3 and known value 0.

  unshaped_withparameters[0][0] = baseline;
  unshaped_withparameters[0][1] = 1.0;
  unshaped_withparameters[0][2] = 1.0;
  unshaped_withparameters[0][3] = 0.0;
  CR_unshaper( unshaped_withparameters, shaped_withparameters, length, diff_ratio_3, baseline );

  for ( Int_t i = 0; i < length; i++ ) shaped_withparameters[i] = unshaped_withparameters[i];

  unshaped_withparameters[0][0] = baseline;
  unshaped_withparameters[0][1] = 1.0;
  unshaped_withparameters[0][2] = 0.0;
  unshaped_withparameters[0][3] = 1.0;
  CR_unshaper( unshaped_withparameters, shaped_withparameters, length, diff_ratio_2, baseline );

  for ( Int_t i = 0; i < length; i++ ) shaped_withparameters[i] = unshaped_withparameters[i];

  // Non-zero values at the beginning only contribute to baseline.
  unshaped_withparameters[0][0] = 0.0;
  unshaped_withparameters[0][1] = 0.0;
  unshaped_withparameters[0][2] = 0.0;
  unshaped_withparameters[0][3] = 0.0;
  CR_unshaper( unshaped_withparameters, shaped_withparameters, length, diff_ratio_1, baseline ); 

  for ( Int_t i = 0; i < length; i++ ) shaped_withparameters[i] = unshaped_withparameters[i];

  // The first unshaped value will be discarded as unreliable.
  unshaped_withparameters[0][0] = 0.0;
  unshaped_withparameters[0][1] = 0.0;
  unshaped_withparameters[0][2] = 0.0;
  unshaped_withparameters[0][3] = 0.0;
  RC_unshaper( unshaped_withparameters, shaped_withparameters, length, int_ratio_2 );

  for ( Int_t i = 0; i < length; i++ ) shaped_withparameters[i] = unshaped_withparameters[i];

  // Also, the second unshaped value will be discarded as unreliable.
  unshaped_withparameters[0][0] = 0.0;
  unshaped_withparameters[0][1] = 0.0;
  unshaped_withparameters[0][2] = 0.0;
  unshaped_withparameters[0][3] = 0.0;
  RC_unshaper( unshaped_withparameters, shaped_withparameters, length, int_ratio_1 );

  // Discard first two elements; untrustworthy, due to RC unshapers.  
  for(Int_t i=0; i<length-2; i++) {
    unshaped_withparameters[i] = unshaped_withparameters[i+2];
  }
  length -= 2;
  increment_forward = 2;

  // Compute the SSE.
  ROOT::Math::SMatrix< double,4,4,ROOT::Math::MatRepSym<double,4> > sum_of_squares;
  ROOT::Math::SVector<double,4> sum_of_values;
  double* sum_of_squares_array = sum_of_squares.Array();
  for(Int_t i=0; i<length; i++) {
    sum_of_values += unshaped_withparameters[i];
    // The following takes the square of the expression, and stores it in symmetric matrix format.
    // There would be neater ways to do this with general matrices, but they wouldn't be as fast.
    // Inside the for loop, we have to prioritize speed.
    sum_of_squares_array[0] += unshaped_withparameters[i][0]*unshaped_withparameters[i][0];
    sum_of_squares_array[1] += unshaped_withparameters[i][0]*unshaped_withparameters[i][1];
    sum_of_squares_array[2] += unshaped_withparameters[i][1]*unshaped_withparameters[i][1];
    sum_of_squares_array[3] += unshaped_withparameters[i][0]*unshaped_withparameters[i][2];
    sum_of_squares_array[4] += unshaped_withparameters[i][1]*unshaped_withparameters[i][2];
    sum_of_squares_array[5] += unshaped_withparameters[i][2]*unshaped_withparameters[i][2];
    sum_of_squares_array[6] += unshaped_withparameters[i][0]*unshaped_withparameters[i][3];
    sum_of_squares_array[7] += unshaped_withparameters[i][1]*unshaped_withparameters[i][3];
    sum_of_squares_array[8] += unshaped_withparameters[i][2]*unshaped_withparameters[i][3];
    sum_of_squares_array[9] += unshaped_withparameters[i][3]*unshaped_withparameters[i][3];
  }
  ROOT::Math::SMatrix<double,4> SSE = sum_of_squares - (ROOT::Math::TensorProd(sum_of_values,sum_of_values)/length);

  //Now minimize by separating out the variable parts from the constant parts and setting the derivative equal to 0.
  ROOT::Math::SMatrix<double,3> SSE_temp = SSE.Sub<ROOT::Math::SMatrix<double,3> >(1,1);
  SSE_temp += ROOT::Math::Transpose(SSE_temp);
  // SubRow(row, col), but SubCol(col, row) -- I know, weird.
  ROOT::Math::SVector<double,3> vector_temp = (-1.0) * (SSE.SubRow<ROOT::Math::SVector<double,3> >(0,1) + 
                                              SSE.SubCol<ROOT::Math::SVector<double,3> >(0,1));
  if( !SSE_temp.Invert() ) std::cout<<"Matrix inversion failed; please let Clayton know!  He was lazy and didn\'t feel like dealing with this possibility, so really you\'d be teaching him a lesson."<<std::endl;
  ROOT::Math::SVector<double,4> minimum;
  minimum[0] = 1.0;
  minimum.Place_at( SSE_temp*vector_temp, 1);

  // If the trace contains signals, then there will be portions where the unshaped signal should not be approximately flat.
  // Here we look for small sections where large jumps occur;
  // these will break up the trace into presumably flat segments. 
  Int_t bin_width = 2;  // Trace is separated into segments of width bin_width.
  Int_t num_bins_exclude = 2;  // num_bins_exclude segments will be excluded; the averages will differ before and after.
  Int_t pts_to_avg = 4;  // Points averaged before and after, to see how big a jump was in a bin.

  Int_t num_bins = (Int_t) (length-pts_to_avg)/bin_width; // leaves at least pts_to_avg unbinned in unshaped trace.
  if(num_bins_exclude >= num_bins) {
    std::cout<<"ERROR:  the trace passed to pattern_recognition was too short for this figure of merit."<<std::endl;
  }
  
  Double_t* drift_ofbin = new Double_t[num_bins_exclude];
  Int_t* worst_bin = new Int_t[num_bins_exclude]; 
  Int_t* worst_bin_old = new Int_t[num_bins_exclude]; 
  for(Int_t i=0; i<num_bins_exclude; i++) {
    worst_bin_old[i] = -999;
  }

  for(Int_t i=0; i<5; i++) {  // Maximum of 5 iterations -- need to see how often this happens.

    // Reset drift_ofbin and worst_bin.
    for(Int_t j=0; j<num_bins_exclude; j++) {
      drift_ofbin[j] = 0.0;
      worst_bin[j] = -999;
    }

    // Look for bins with bad drift; pretend unshaped[i<0] = 0.0 identically (should be true by the way unshaping is done).
    // Note that although signals at the beginning of the trace are excluded, signals at the end are not.
    for(Int_t j=0; j<num_bins; j++) {
      
      // What is the drift of bin j?
      Double_t drift_temp = 0.0;
      for(Int_t k=0; k<pts_to_avg; k++) {
        drift_temp += ROOT::Math::Dot(unshaped_withparameters[(j+1)*bin_width + k], minimum);
        if(j*bin_width - k - 1 >= 0) drift_temp -= ROOT::Math::Dot(unshaped_withparameters[j*bin_width - k - 1], minimum);
      }
       
      // Is it worst than the drifts currently held in drift_ofbin?  If so, keep it and resort the list.
      if(fabs(drift_temp) > drift_ofbin[num_bins_exclude-1]) {
        drift_ofbin[num_bins_exclude-1] = fabs(drift_temp);
        worst_bin[num_bins_exclude-1] = j;
        for(Int_t k=num_bins_exclude-1; k>0 and drift_ofbin[k]>drift_ofbin[k-1]; k--) {
          Double_t temp_drift = drift_ofbin[k-1];
          Int_t temp_bin_index = worst_bin[k-1];
          drift_ofbin[k-1] = drift_ofbin[k];
          worst_bin[k-1] = worst_bin[k];
          drift_ofbin[k] = temp_drift;
          worst_bin[k] = temp_bin_index;
        }
      }
    }

    // Next, it's more convenient to sort by index rather than drift distance.
    // This is O(num_bins_exclude^2), which is inefficient, but I think this number will always be small.
    for(Int_t j=num_bins_exclude-1; j>0; j--) {
      for(Int_t k=0; k<j; k++) {
        if(worst_bin[k] > worst_bin[k+1]) {
          Double_t temp_drift = drift_ofbin[k+1];
          Int_t temp_bin_index = worst_bin[k+1];
          drift_ofbin[k+1] = drift_ofbin[k];
          worst_bin[k+1] = worst_bin[k];
          drift_ofbin[k] = temp_drift;
          worst_bin[k] = temp_bin_index;
        }
      }
    }

    // Now find SSE.
    SSE = sum_of_squares;
    double* SSE_array = SSE.Array();
    for(Int_t j=0; j<num_bins_exclude; j++) {
      for(Int_t k=0; k<bin_width; k++) {
        SSE_array[0] -= unshaped_withparameters[j*bin_width + k][0]*unshaped_withparameters[j*bin_width + k][0];
        SSE_array[1] -= unshaped_withparameters[j*bin_width + k][0]*unshaped_withparameters[j*bin_width + k][1];
        SSE_array[2] -= unshaped_withparameters[j*bin_width + k][1]*unshaped_withparameters[j*bin_width + k][1];
        SSE_array[3] -= unshaped_withparameters[j*bin_width + k][0]*unshaped_withparameters[j*bin_width + k][2];
        SSE_array[4] -= unshaped_withparameters[j*bin_width + k][1]*unshaped_withparameters[j*bin_width + k][2];
        SSE_array[5] -= unshaped_withparameters[j*bin_width + k][2]*unshaped_withparameters[j*bin_width + k][2];
        SSE_array[6] -= unshaped_withparameters[j*bin_width + k][0]*unshaped_withparameters[j*bin_width + k][3];
        SSE_array[7] -= unshaped_withparameters[j*bin_width + k][1]*unshaped_withparameters[j*bin_width + k][3];
        SSE_array[8] -= unshaped_withparameters[j*bin_width + k][2]*unshaped_withparameters[j*bin_width + k][3];
        SSE_array[9] -= unshaped_withparameters[j*bin_width + k][3]*unshaped_withparameters[j*bin_width + k][3];
      }
    }
    sum_of_values = ROOT::Math::SVector<double,4>();
    for(Int_t j=0; j<bin_width*worst_bin[0]; j++) {
      sum_of_values += unshaped_withparameters[j];
    }
    if(worst_bin[0] != 0) {
      SSE -= ROOT::Math::TensorProd(sum_of_values,sum_of_values)/((Double_t) (bin_width*worst_bin[0]));
    }
    for(Int_t j=1; j<num_bins_exclude; j++) {
      sum_of_values = ROOT::Math::SVector<double,4>();
      for(Int_t k=bin_width*(worst_bin[j-1]+1); k<bin_width*worst_bin[j]; k++) {
        sum_of_values += unshaped_withparameters[k];
      }
      if(worst_bin[j]-worst_bin[j-1] > 1) {
        SSE -= ROOT::Math::TensorProd(sum_of_values,sum_of_values)/((Double_t) bin_width*(worst_bin[j]-worst_bin[j-1]-1));
      }
    }


    //Now minimize by separating out the variable parts from the constant parts and setting the derivative equal to 0.
    SSE_temp = SSE.Sub<ROOT::Math::SMatrix<double,3> >(1,1);
    SSE_temp += ROOT::Math::Transpose(SSE_temp);
    vector_temp = (-1.0) * (SSE.SubRow<ROOT::Math::SVector<double,3> >(0,1) + 
                            SSE.SubCol<ROOT::Math::SVector<double,3> >(0,1));
    if( !SSE_temp.Invert() ) std::cout<<"Matrix inversion failed; please let Clayton know!  He was lazy and didn\'t feel like dealing with this possibility, so really you\'d be teaching him a lesson."<<std::endl;
    ROOT::Math::SVector<double,4> minimum;
    minimum[0] = 1.0;
    minimum.Place_at( SSE_temp*vector_temp, 1);

    // worst_bin and worst_bin_old are both sorted; if they don't match, then another iteration should be done.
    bool should_repeat = false;
    for(Int_t j=0; j<num_bins_exclude; j++) {
      if(worst_bin[j] != worst_bin_old[j]) {
        should_repeat = true;
        break;
      }
    }

    if(!should_repeat) {
      break;
    }

    // If we're anticipating another iteration, then we need to copy worst_bin into worst_bin_old.
    for(Int_t j=0; j<num_bins_exclude; j++) {
      worst_bin_old[j] = worst_bin[j];
    }

  }

  delete [] worst_bin;
  delete [] worst_bin_old;
  delete [] drift_ofbin;

  Double_t* unshaped = new Double_t[length];
  for(Int_t i=0; i<length; i++) {
    unshaped[i] = ROOT::Math::Dot(unshaped_withparameters[i], minimum);
  }

  return unshaped;
}

//______________________________________________________________________________
const EXODoubleWaveform& EXOMiscUtil::unshaped_signal() 
{
  // Returns a normalized unshaped signal.
  // Time between samples is 0.1 microseconds

  static Double_t model[1000] = {
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0005, 0.0005, 0.0005, 0.0005,
        0.0005, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0010, 0.0010, 0.0010, 0.0010, 0.0010,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0015, 0.0015, 0.0015,
        0.0015, 0.0015, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0020, 0.0020, 0.0020, 0.0020, 0.0020,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0025, 0.0025, 0.0025, 0.0025, 0.0025,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0030, 0.0030, 0.0030, 0.0030, 0.0030,
        0.0035, 0.0035, 0.0035, 0.0035, 0.0035,
        0.0035, 0.0035, 0.0035, 0.0035, 0.0035,
        0.0035, 0.0035, 0.0035, 0.0035, 0.0035,
        0.0035, 0.0035, 0.0035, 0.0035, 0.0035,
        0.0035, 0.0035, 0.0035, 0.0035, 0.0035,
        0.0040, 0.0040, 0.0040, 0.0040, 0.0040,
        0.0040, 0.0040, 0.0040, 0.0040, 0.0040,
        0.0040, 0.0040, 0.0040, 0.0040, 0.0040,
        0.0040, 0.0040, 0.0040, 0.0040, 0.0040,
        0.0045, 0.0045, 0.0045, 0.0045, 0.0045,
        0.0045, 0.0045, 0.0045, 0.0045, 0.0045,
        0.0045, 0.0045, 0.0045, 0.0045, 0.0045,
        0.0045, 0.0045, 0.0045, 0.0050, 0.0050,
        0.0050, 0.0050, 0.0050, 0.0050, 0.0050,
        0.0050, 0.0050, 0.0050, 0.0050, 0.0050,
        0.0050, 0.0050, 0.0050, 0.0055, 0.0055,
        0.0055, 0.0055, 0.0055, 0.0055, 0.0055,
        0.0055, 0.0055, 0.0055, 0.0055, 0.0055,
        0.0055, 0.0060, 0.0060, 0.0060, 0.0060,
        0.0060, 0.0060, 0.0060, 0.0060, 0.0060,
        0.0060, 0.0060, 0.0060, 0.0065, 0.0065,
        0.0065, 0.0065, 0.0065, 0.0065, 0.0065,
        0.0065, 0.0065, 0.0065, 0.0065, 0.0070,
        0.0070, 0.0070, 0.0070, 0.0070, 0.0070,
        0.0070, 0.0070, 0.0070, 0.0075, 0.0075,
        0.0075, 0.0075, 0.0075, 0.0075, 0.0075,
        0.0075, 0.0080, 0.0080, 0.0080, 0.0080,
        0.0080, 0.0080, 0.0080, 0.0085, 0.0085,
        0.0085, 0.0085, 0.0085, 0.0085, 0.0085,
        0.0090, 0.0090, 0.0090, 0.0090, 0.0090,
        0.0090, 0.0095, 0.0095, 0.0095, 0.0095,
        0.0095, 0.0100, 0.0100, 0.0100, 0.0100,
        0.0100, 0.0105, 0.0105, 0.0105, 0.0105,
        0.0105, 0.0110, 0.0110, 0.0110, 0.0110,
        0.0115, 0.0115, 0.0115, 0.0115, 0.0120,
        0.0120, 0.0120, 0.0120, 0.0125, 0.0125,
        0.0125, 0.0130, 0.0130, 0.0130, 0.0130,
        0.0135, 0.0135, 0.0135, 0.0140, 0.0140,
        0.0145, 0.0145, 0.0145, 0.0150, 0.0150,
        0.0150, 0.0155, 0.0155, 0.0160, 0.0160,
        0.0165, 0.0165, 0.0170, 0.0170, 0.0175,
        0.0175, 0.0180, 0.0180, 0.0185, 0.0185,
        0.0190, 0.0195, 0.0195, 0.0200, 0.0200,
        0.0205, 0.0210, 0.0215, 0.0215, 0.0220,
        0.0225, 0.0230, 0.0235, 0.0235, 0.0240,
        0.0245, 0.0250, 0.0255, 0.0260, 0.0265,
        0.0270, 0.0280, 0.0285, 0.0290, 0.0295,
        0.0300, 0.0310, 0.0315, 0.0325, 0.0330,
        0.0340, 0.0345, 0.0355, 0.0365, 0.0375,
        0.0385, 0.0395, 0.0405, 0.0415, 0.0425,
        0.0435, 0.0450, 0.0460, 0.0475, 0.0485,
        0.0500, 0.0510, 0.0530, 0.0555, 0.0590,
        0.0670, 0.0805, 0.0940, 0.1086, 0.1246,
        0.1411, 0.1576, 0.1746, 0.1906, 0.2081,
        0.2266, 0.2451, 0.2641, 0.2826, 0.3037,
        0.3252, 0.3492, 0.3787, 0.4047, 0.4402,
        0.4962, 0.5778, 0.7274, 0.9400, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000,
        1.0000, 1.0000, 1.0000, 1.0000, 1.0000 };
  static EXODoubleWaveform wf(model, sizeof(model)/sizeof(model[0]));
  wf.SetSamplingPeriod(0.1*CLHEP::microsecond);
  return wf;

  /*
  Int_t nmodel = 1000;
  if ( i < 0 ) i = 0;
  if ( i >= nmodel ) i = nmodel - 1;

  return model[i];
  */
}


//______________________________________________________________________________
const EXODoubleWaveform& EXOMiscUtil::unshaped_ind_signal() 
{
  // Returns a normalized unshaped signal for induction on a U wire.
  // Time between samples is 0.1 microseconds

  static Double_t model[1000] = {0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000871,0.0001742,0.0002614,0.0003485,
0.0004356,0.0005227,0.0006099,0.0006970,0.0007841,
0.0008712,0.0011906,0.0015100,0.0018294,0.0021488,
0.0024682,0.0027876,0.0031070,0.0034264,0.0037458,
0.0040652,0.0043364,0.0046075,0.0048786,0.0051498,
0.0054209,0.0056920,0.0059632,0.0062343,0.0065054,
0.0067766,0.0063981,0.0060197,0.0056413,0.0052629,
0.0048845,0.0045060,0.0041276,0.0037492,0.0033708,
0.0029923,0.0033066,0.0036209,0.0039352,0.0042495,
0.0045638,0.0048781,0.0051924,0.0055067,0.0058210,
0.0061353,0.0067311,0.0073268,0.0079226,0.0085184,
0.0091141,0.0097099,0.0103056,0.0109014,0.0114971,
0.0120929,0.0125422,0.0129915,0.0134407,0.0138900,
0.0143393,0.0147886,0.0152379,0.0156872,0.0161364,
0.0165857,0.0172755,0.0179653,0.0186551,0.0193448,
0.0200346,0.0207244,0.0214142,0.0221039,0.0227937,
0.0234835,0.0238896,0.0242958,0.0247019,0.0251081,
0.0255142,0.0259203,0.0263265,0.0267326,0.0271388,
0.0275449,0.0278196,0.0280942,0.0283689,0.0286436,
0.0289182,0.0291929,0.0294676,0.0297422,0.0300169,
0.0302916,0.0311557,0.0320198,0.0328839,0.0337481,
0.0346122,0.0354763,0.0363404,0.0372046,0.0380687,
0.0389328,0.0397375,0.0405422,0.0413470,0.0421517,
0.0429564,0.0437611,0.0445658,0.0453705,0.0461752,
0.0469799,0.0481990,0.0494180,0.0506371,0.0518561,
0.0530751,0.0542942,0.0555132,0.0567323,0.0579513,
0.0591703,0.0600899,0.0610094,0.0619290,0.0628486,
0.0637681,0.0646877,0.0656072,0.0665268,0.0674463,
0.0683659,0.0693546,0.0703433,0.0713320,0.0723207,
0.0733094,0.0742981,0.0752868,0.0762755,0.0772642,
0.0782529,0.0798410,0.0814292,0.0830174,0.0846056,
0.0861937,0.0877819,0.0893701,0.0909583,0.0925465,
0.0941346,0.0960633,0.0979920,0.0999206,0.1018493,
0.1037779,0.1057066,0.1076352,0.1095639,0.1114926,
0.1134212,0.1155807,0.1177402,0.1198996,0.1220591,
0.1242186,0.1263781,0.1285375,0.1306970,0.1328565,
0.1350160,0.1373741,0.1397322,0.1420904,0.1444485,
0.1468067,0.1491648,0.1515230,0.1538811,0.1562392,
0.1585974,0.1652258,0.1718542,0.1784826,0.1851110,
0.1917394,0.1983678,0.2049962,0.2116247,0.2182531,
0.2248815,0.2549571,0.2850328,0.3151085,0.3451842,
0.3752598,0.4053355,0.4354112,0.4654868,0.4955625,
0.5256382,0.5730744,0.6205105,0.6679467,0.7153829,
0.7628191,0.8102553,0.8576915,0.9051276,0.9525638,
1.0000000,0.9897843,0.9795686,0.9693529,0.9591372,
0.9489215,0.9387057,0.9284900,0.9182743,0.9080586,
0.8978429,0.8203330,0.7428232,0.6653133,0.5878034,
0.5102936,0.4327837,0.3552739,0.2777640,0.2002541,
0.1227443,0.1104698,0.0981954,0.0859210,0.0736466,
0.0613721,0.0490977,0.0368233,0.0245489,0.0122744,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000,
0.0000000,0.0000000,0.0000000,0.0000000,0.0000000};

  static EXODoubleWaveform wf(model, sizeof(model)/sizeof(model[0]));
  wf.SetSamplingPeriod(0.1*CLHEP::microsecond);
  return wf;


}

Double_t EXOMiscUtil::GetDigitizerRescaling(const Int_t chan) {
  // Returns an empirical scaling factor used to correct the pulse heights 
  // generated by the digitizer to give the same amplitudes seen in data.
  // First 152 entries are for the U/V wire channels, while a single 
  // scaling is used for all APDs

  static Double_t scale_facs[153] =  {
        0.93429993,  0.92935983,  0.77029734,  0.76641352,  0.78222685,
        0.75483135,  0.77822049,  0.76937864,  0.79804876,  0.93145904,
        0.89321771,  0.89823208,  0.94012124,  0.91540861,  0.88782022,
        0.90634858,  0.94009089,  0.86907901,  0.90810409,  0.78011247,
        0.7875321 ,  0.78206254,  0.76420854,  0.7749684 ,  0.76665598,
        0.72440371,  0.77747787,  0.78305303,  0.77630831,  0.74135378,
        0.77039117,  0.77276501,  0.80630967,  0.80921787,  0.79064201,
        0.76763368,  0.77069017,  0.84818194,  0.92084741,  0.8266423 ,
        0.78358491,  0.78288151,  0.7754432 ,  0.81944252,  0.83757309,
        0.88437362,  0.84231626,  0.86403544,  0.82536927,  0.82861227,
        0.79517872,  0.86387494,  0.83457562,  0.81612236,  0.80528042,
        0.81407724,  0.77578422,  0.78627818,  0.82101626,  0.81634286,
        0.81233982,  0.84114794,  0.81292895,  0.81067761,  0.80559667,
        0.88781002,  0.84691271,  0.80434051,  0.83196971,  0.81531327,
        0.85501458,  0.88040985,  0.81293791,  0.81277087,  0.79533161,
        0.8114403 ,  0.9223394 ,  0.97781427,  0.97320273,  0.94193581,
        0.96418834,  0.95159087,  0.91673238,  0.90901798,  0.92440788,
        0.94242805,  0.91715503,  0.92875196,  0.92059652,  0.90246559,
        0.94043416,  0.9411527 ,  0.91716303,  0.88982154,  0.90091898,
        0.8074224 ,  0.77588175,  0.81401134,  0.79296705,  0.76897091,
        0.76892849,  0.74717423,  0.76578663,  0.7719438 ,  0.79154654,
        0.93128706,  0.93659099,  0.94237118,  0.93508817,  0.99517567,
        1.00598477,  0.98453785,  0.81309978,  0.79909329,  0.97543538,
        0.82263668,  0.76404248,  0.82451623,  0.82658918,  0.80652171,
        0.84536724,  0.83431259,  0.82639692,  0.84114262,  0.83623173,
        0.79752684,  0.88151075,  0.87196775,  0.85200556,  0.85683424,
        0.82953998,  0.81471833,  0.8566758 ,  0.86574048,  0.83725704,
        0.82020617,  0.82704905,  0.80176697,  0.81004789,  0.82309314,
        0.86234201,  0.86206555,  0.84956541,  0.84875279,  0.88519752,
        0.87674746,  0.83320151,  0.8623117 ,  0.86382735,  0.81825192,
        0.80638583,  0.84075413,  2.39454933};

  // return the scaling factor for the u- or v- wire channel
  // the last element in the array gives a single scaling for all APDs
  return scale_facs[ chan < 152 ? chan : 152];

}


//______________________________________________________________________________
bool EXOMiscUtil::CanWeUse_noise_sq_mag_apd(const EXOTransferFunction& transfer)
{
  // Call this function to check whether the shaping times you're using match those at which the detector noise
  // was taken; if not, you really shouldn't use EXOMiscUtil::noise_sq_mag_apd.
  // At the moment, we aren't checking that the trace length and sampling frequency are valid; just shaping times.

  if(transfer.GetNumIntegStages() != 2) return false;
  if(fabs(transfer.GetIntegTime(0) - 3.0*CLHEP::microsecond) > 0.01*CLHEP::microsecond) return false;
  if(fabs(transfer.GetIntegTime(1) - 3.0*CLHEP::microsecond) > 0.01*CLHEP::microsecond) return false;
  if(transfer.GetNumDiffStages() != 3) return false;
  // Need to have 10us, 10us, and 300 us, but they might not be in order.
  int num_10 = 0;
  int num_300 = 0;
  for(size_t i = 0; i < 3; i++) {
    if(fabs(transfer.GetDiffTime(i) - 10.0*CLHEP::microsecond) < 0.01*CLHEP::microsecond) num_10++;
    else if(fabs(transfer.GetDiffTime(i) - 300.0*CLHEP::microsecond) < 0.01*CLHEP::microsecond) num_300++;
  }
  if(num_10 != 2 or num_300 != 1) return false;

  return true;
}

//______________________________________________________________________________
const EXODoubleWaveform& EXOMiscUtil::noise_sq_mag_apd() 
{
  // Returns the square magnitude of the apd sum noise
  // power spectrum in steps of 1/(2048us)

  static Double_t fft_mag[1025] = {
 1, 5.53, 8.88, 6.8, 7.5, 11.1, 24.8, 17.3, 24.7, 32.4,
 35.2, 35.3, 38.9, 41.8, 49, 56.1, 73.6, 74.8, 80.4, 107,
 123, 158, 139, 160, 191, 182, 238, 202, 234, 290,
 442, 321, 273, 205, 233, 231, 253, 168, 152, 151,
 166, 163, 187, 153, 142, 160, 167, 157, 171, 175,
 170, 174, 193, 204, 227, 188, 264, 282, 294, 399,
 462, 412, 280, 324, 443, 530, 719, 256, 200, 172,
 160, 156, 179, 157, 171, 155, 157, 204, 319, 233,
 463, 844, 188, 128, 126, 133, 132, 143, 160, 176,
 195, 178, 142, 161, 278, 366, 536, 248, 145, 96.5,
 93.7, 85.9, 84.1, 92.4, 99.9, 85.7, 85.7, 78.9, 77.2, 76.6,
 66.5, 66.1, 74.9, 73.7, 63.1, 69.6, 69.4, 66.7, 78.6, 84.2,
 73.2, 193, 71, 86.7, 129, 149, 186, 150, 64.5, 57.7,
 54.3, 58.3, 56, 54.4, 56.8, 53.4, 48.7, 43.8, 45.2, 41.4,
 42, 41.1, 38, 38.1, 39.6, 37.5, 35.7, 35.7, 36.6, 41,
 41.1, 40.9, 33.6, 35.6, 44.7, 65.8, 68.1, 53.6, 39.3, 40,
 78.6, 124, 110, 44.5, 42.7, 47.2, 33.2, 28, 27.6, 28.2,
 24.4, 26.2, 27.3, 24.3, 21.9, 24.1, 23.9, 22.6, 26.8, 23.6,
 22.1, 39.1, 41.3, 23.7, 23.2, 20.1, 22.3, 21.8, 19.5, 19.5,
 19, 16.2, 18.9, 18.9, 24.3, 21.1, 19.7, 22.1, 21.9, 32.7,
 68.2, 738, 76.3, 90.3, 3.6e+03, 55.6, 26.4, 20.2, 17.6, 49.7,
 14.8, 16, 16, 13.4, 12.9, 14.4, 14.8, 14.1, 12.1, 12.3,
 10.6, 10.4, 10.3, 11.2, 15.3, 12.2, 12.9, 12.3, 13.7, 13,
 11.4, 11.4, 13.3, 16.1, 14.3, 11.8, 12, 10.6, 9.93, 9.99,
 12, 17.4, 45, 19.7, 11.4, 8.42, 8.31, 8.68, 7.82, 8.12,
 7.86, 7.77, 7.72, 7.71, 9.92, 8.01, 9.74, 68.4, 13, 8.99,
 8.15, 8.02, 7.56, 9.45, 9.51, 8.13, 9.54, 7.1, 6.4, 5.87,
 5.89, 5.59, 6.92, 6.31, 5.7, 5.4, 6.13, 16.2, 5.56, 6.1,
 5.02, 4.89, 4.78, 4.92, 5.26, 6.73, 5.49, 4.7, 5.29, 4.95,
 4.78, 5.66, 6.36, 7.01, 7.13, 6.93, 7.5, 5.48, 4.31, 4.25,
 4.7, 4.88, 5.59, 6.13, 3.5, 3.49, 3.45, 3.69, 3.5, 3.52,
 4.14, 4.8, 4.41, 3.6, 3.75, 4.8, 4.33, 3.74, 3.76, 4.04,
 14.5, 5.27, 6.66, 8.06, 8.54, 11.9, 300, 6.55, 3.96, 3.25,
 3.92, 5.26, 4.06, 5.67, 2.65, 2.75, 2.39, 2.53, 2.57, 2.73,
 2.53, 3, 3.69, 3.31, 2.65, 4.02, 3.4, 2.51, 2.52, 2.24,
 2.45, 2.68, 3.16, 4.82, 247, 7.97, 4.62, 3.55, 2.67, 2.3,
 2.36, 2.59, 2.96, 3.45, 2.06, 2.16, 3.75, 2.12, 1.93, 2.32,
 1.97, 1.96, 1.86, 1.89, 1.8, 3.9, 2.29, 2.39, 2.82, 1.93,
 1.75, 8.42, 2.11, 2.62, 3.57, 3.77, 4.08, 9.18, 310, 5.99,
 3.06, 2.45, 2.47, 2.33, 1.99, 1.84, 2.15, 2.21, 2.27, 2.57,
 3.43, 3.57, 4.53, 7.19, 26.6, 543, 14.3, 5.62, 8.65, 2.48,
 2.03, 2.26, 2.13, 2.19, 2.77, 2.5, 2.13, 1.94, 1.64, 1.45,
 1.71, 1.62, 1.94, 2.1, 2.23, 1.89, 2.24, 2.16, 3.2, 10.5,
 2.74, 2.53, 2.51, 2.6, 2.53, 2.72, 3.34, 2.56, 2.82, 2.81,
 2.48, 2.21, 2.39, 2.26, 2.78, 2.2, 2.11, 1.69, 1.41, 1.31,
 1.11, 1.24, 1.27, 1.17, 1.54, 1.1, 0.964, 0.995, 0.982, 0.983,
 0.89, 0.953, 0.973, 0.889, 0.87, 0.909, 2.32, 0.84, 0.941, 0.861,
 0.872, 0.907, 0.836, 0.921, 0.991, 0.908, 0.915, 0.792, 0.792, 0.907,
 1.02, 0.927, 0.987, 0.953, 1.1, 1.04, 0.843, 0.811, 0.776, 0.809,
 0.814, 0.8, 0.922, 0.777, 0.69, 0.712, 0.905, 1.05, 0.724, 0.71,
 0.729, 0.672, 0.708, 0.698, 0.832, 0.774, 0.679, 0.686, 0.712, 0.699,
 0.666, 0.829, 0.719, 0.887, 2.31, 1.49, 0.672, 0.698, 0.692, 0.69,
 0.794, 0.774, 0.755, 0.695, 0.644, 0.633, 0.696, 1.08, 0.559, 0.562,
 0.539, 0.577, 0.617, 0.516, 0.481, 0.511, 0.463, 0.45, 0.509, 0.442,
 0.434, 0.517, 0.497, 0.773, 0.551, 0.522, 0.476, 0.452, 0.473, 0.467,
 0.51, 0.729, 0.812, 0.945, 6.41, 2, 0.588, 0.571, 0.482, 0.474,
 0.483, 0.473, 0.475, 0.477, 0.542, 0.53, 0.512, 0.495, 0.467, 0.463,
 0.469, 0.399, 0.425, 0.434, 0.425, 0.457, 0.422, 0.423, 0.397, 0.41,
 0.446, 0.956, 1.68, 1.21, 0.995, 0.576, 0.671, 0.876, 0.419, 0.407,
 0.379, 0.445, 0.405, 0.386, 0.317, 0.354, 0.396, 0.384, 0.379, 0.434,
 0.47, 0.538, 0.432, 0.374, 0.389, 0.376, 0.34, 0.344, 0.316, 0.357,
 0.4, 0.741, 2.21, 1.23, 0.913, 0.635, 0.423, 0.401, 0.476, 0.435,
 0.498, 0.612, 0.521, 0.347, 0.321, 0.312, 0.328, 0.344, 0.37, 0.444,
 0.431, 1.52, 3.18, 0.534, 0.351, 0.317, 0.348, 0.329, 0.31, 0.319,
 0.613, 0.497, 1.39, 1.04, 0.832, 0.679, 0.448, 0.572, 0.766, 0.619,
 0.701, 0.874, 0.944, 0.373, 0.324, 0.352, 0.285, 0.291, 0.294, 0.28,
 0.292, 0.282, 0.51, 0.288, 0.268, 0.274, 0.282, 0.38, 0.289, 0.251,
 0.253, 0.275, 0.441, 0.456, 0.424, 0.409, 0.346, 0.607, 0.72, 0.731,
 0.808, 0.896, 0.739, 0.391, 0.311, 0.297, 0.259, 0.313, 0.308, 0.319,
 0.314, 0.312, 0.298, 0.251, 0.282, 0.322, 0.367, 0.417, 0.388, 0.358,
 0.373, 0.443, 0.505, 0.636, 0.954, 1.31, 2.47, 8.1, 297, 19.5,
 4.15, 1.9, 1.16, 0.842, 0.566, 0.476, 0.387, 0.359, 0.356, 0.35,
 0.425, 0.351, 0.318, 0.342, 0.343, 0.366, 0.358, 0.388, 0.4, 0.375,
 0.321, 0.259, 0.259, 0.273, 0.294, 0.323, 0.385, 0.471, 1.11, 0.951,
 0.572, 0.463, 0.358, 0.367, 0.39, 0.278, 0.341, 0.307, 0.309, 0.287,
 0.267, 0.268, 0.251, 0.263, 0.242, 0.285, 0.292, 0.301, 0.29, 0.263,
 0.26, 0.256, 0.327, 0.303, 0.269, 0.278, 0.285, 0.383, 0.53, 0.529,
 0.447, 0.456, 0.523, 0.43, 0.576, 0.711, 3.9, 0.484, 0.3, 0.254,
 0.235, 0.239, 0.233, 0.227, 0.255, 0.478, 2.33, 0.36, 0.518, 5.52,
 1.65, 0.408, 0.283, 0.254, 0.24, 0.273, 0.308, 0.343, 0.378, 0.411,
 0.388, 0.371, 0.429, 0.504, 0.699, 0.573, 0.535, 0.663, 1.21, 6.48,
 20.7, 1.62, 0.743, 0.492, 0.403, 0.433, 0.598, 0.505, 0.308, 0.301,
 0.297, 0.33, 0.299, 0.293, 0.286, 0.294, 0.302, 0.321, 0.352, 0.442,
 0.379, 0.467, 0.83, 4.14, 5.32, 1.52, 0.499, 0.426, 0.483, 1.75,
 2.15, 0.475, 0.345, 0.304, 0.267, 0.261, 0.299, 0.294, 0.308, 1.04,
 0.682, 0.294, 0.311, 0.267, 0.285, 0.309, 0.3, 0.392, 0.391, 0.353,
 0.419, 0.502, 0.547, 0.553, 0.56, 0.512, 0.45, 0.571, 0.702, 1.4,
 11.7, 9.51, 1.37, 0.678, 0.511, 0.425, 0.427, 0.356, 0.401, 1.28,
 0.643, 0.406, 0.341, 0.336, 0.716, 0.528, 0.407, 0.418, 0.384, 0.337,
 0.381, 0.288, 0.284, 0.266, 0.256, 0.258, 0.247, 0.271, 0.261, 0.234,
 0.249, 0.257, 0.258, 0.252, 0.267, 0.245, 0.321, 0.26, 0.255, 0.236,
 0.244, 0.218, 0.236, 0.231, 0.25, 0.245, 0.291, 0.256, 0.237, 0.23,
 0.213, 0.238, 0.237, 0.239, 0.251, 0.267, 0.235, 0.242, 0.224, 0.22,
 0.206, 0.22, 0.257, 0.23, 0.235, 0.204, 0.218, 0.217, 0.206, 0.21,
 0.195, 0.202, 0.18, 0.165, 0.191, 0.184, 0.188, 0.181, 0.174, 0.197,
 0.227, 0.19, 0.198, 0.202, 0.219, 0.221, 0.197, 0.168, 0.187, 0.188,
 0.208, 0.19, 0.177, 0.183, 0.195, 0.247, 0.206, 0.222, 0.207, 0.208,
 0.209, 0.207, 0.219, 0.193, 0.228, 0.186, 0.173, 0.199, 0.22, 0.176,
 0.174, 0.168, 0.207, 0.212, 0.228, 0.26, 0.429, 0.185, 0.181, 0.169,
 0.163, 0.172, 0.167, 0.194, 0.216, 0.195, 0.178, 0.219, 0.204, 0.202,
 0.232, 0.206, 0.232, 0.205, 0.194, 0.199, 0.206, 0.208, 0.197, 0.224,
 0.269, 0.519, 6.57, 1.38, 0.354, 0.28, 0.237, 0.216, 0.193, 0.205,
 0.193, 0.17, 0.177, 0.176, 1};
  static EXODoubleWaveform wf(fft_mag, sizeof(fft_mag)/sizeof(fft_mag[0]));
  return wf;

}

//______________________________________________________________________________
bool EXOMiscUtil::CanWeUse_noise_sq_mag_wire(const EXOTransferFunction& transfer)
{
  // Call this function to check whether the shaping times you're using match those at which the detector noise
  // was taken; if not, you really shouldn't use EXOMiscUtil::noise_sq_mag_apd.
  // At the moment, we aren't checking that the trace length and sampling frequency are valid; just shaping times.

  if(transfer.GetNumIntegStages() != 2) return false;
  if(fabs(transfer.GetIntegTime(0) - 3.0*CLHEP::microsecond) > 0.01*CLHEP::microsecond) return false;
  if(fabs(transfer.GetIntegTime(1) - 3.0*CLHEP::microsecond) > 0.01*CLHEP::microsecond) return false;
  if(transfer.GetNumDiffStages() != 3) return false;
  // Need to have 10us, 10us, and 60 us, but they might not be in order.
  int num_10 = 0;
  int num_60 = 0;
  for(size_t i = 0; i < 3; i++) {
    if(fabs(transfer.GetDiffTime(i) - 10.0*CLHEP::microsecond) < 0.01*CLHEP::microsecond) num_10++;
    else if(fabs(transfer.GetDiffTime(i) - 60.0*CLHEP::microsecond) < 0.01*CLHEP::microsecond) num_60++;
  }
  if(num_10 != 2 or num_60 != 1) return false;

  return true;
}

//______________________________________________________________________________
const EXODoubleWaveform& EXOMiscUtil::noise_sq_mag_wire() 
{
  // Returns the square magnitude of the wire noise
  // power spectrum in steps of 1/(2048us)

  static Double_t fft_mag[1025] = {
 1, 16.9, 92.7, 52.3, 56.7, 81, 122, 136, 120, 145,
 158, 167, 179, 204, 220, 241, 290, 304, 326, 362,
 386, 478, 465, 490, 445, 497, 487, 496, 576, 513,
 568, 568, 550, 540, 568, 598, 602, 519, 561, 609,
 581, 596, 621, 623, 615, 586, 521, 566, 614, 538,
 558, 558, 523, 540, 521, 557, 604, 540, 484, 485,
 555, 498, 507, 454, 472, 504, 441, 436, 453, 420,
 447, 423, 434, 360, 386, 400, 411, 381, 360, 353,
 329, 360, 329, 320, 320, 324, 293, 293, 311, 281,
 293, 300, 279, 270, 292, 283, 288, 283, 237, 241,
 236, 219, 200, 232, 214, 204, 210, 204, 209, 183,
 193, 179, 189, 180, 179, 187, 180, 162, 178, 163,
 168, 167, 157, 163, 147, 148, 147, 145, 133, 133,
 116, 120, 120, 113, 126, 117, 128, 117, 106, 117,
 95, 110, 93.7, 96.9, 96.1, 90.9, 91.8, 99.5, 88.7, 87.6,
 79.3, 81.6, 75.5, 77.7, 82.8, 78.3, 74.8, 74.4, 78.5, 76.7,
 70.3, 69.8, 66.3, 68.9, 67.8, 62.8, 66.9, 62.8, 57.3, 60.3,
 56.5, 57.3, 54.2, 55.6, 53, 49.9, 52.3, 50.1, 50.5, 51,
 50.9, 46.8, 46.4, 44.7, 46.8, 42.7, 41.4, 40.8, 40.3, 43.3,
 40.1, 39.2, 40.4, 36, 36.4, 38.8, 39.6, 38, 35.9, 41.1,
 53.4, 374, 60.8, 56.5, 1.19e+03, 44.3, 33.5, 31.7, 31.2, 49.2,
 31.8, 31.3, 27.9, 28.1, 27.7, 28, 28.7, 26.6, 25.3, 26,
 26.1, 26.1, 24.4, 24.4, 25.6, 23.7, 21.7, 21.5, 22.1, 21,
 21.6, 22.1, 20.2, 22.3, 20.6, 20, 19, 20, 17.1, 19.9,
 16.9, 18.9, 19.9, 17, 17.3, 16.2, 18.1, 18.3, 16.5, 15.5,
 15.4, 16.7, 15.5, 16.1, 16.1, 13.3, 15.5, 19.9, 15.1, 14.2,
 12.6, 14, 13.2, 14, 14, 13.7, 13.5, 13, 13.7, 12,
 12.8, 12.3, 13.1, 11.6, 12.4, 11.5, 12.9, 14.7, 9.52, 11.5,
 11, 10.3, 10.3, 11.5, 10.2, 10.3, 10.2, 10.2, 10.3, 10,
 10.9, 10, 9.86, 9.79, 10.7, 10.7, 9.69, 9.43, 9.71, 8.75,
 8.17, 8.71, 8.8, 9.09, 8.8, 8.51, 8.57, 8.66, 8.55, 8.87,
 8.42, 7.84, 7.2, 8.3, 8.04, 8.19, 8.24, 7.49, 7.5, 7.28,
 8.16, 8.56, 8.16, 7.56, 7.52, 6.94, 15.5, 7.37, 7.07, 7.09,
 6.46, 6.58, 6.71, 6.63, 6.11, 6.22, 6.43, 6.65, 6.29, 5.9,
 6.63, 6.44, 5.7, 6.45, 6.17, 6.28, 6, 6.35, 6.19, 5.82,
 6.21, 6.17, 6.15, 6.2, 20.3, 6.03, 6.03, 6.49, 6.31, 6.44,
 5.5, 5.77, 6.07, 5.63, 5.84, 5.83, 6.1, 5.68, 6.03, 5.72,
 5.55, 5.54, 5.5, 5.73, 5.01, 5.22, 5.57, 5.97, 5.41, 5.21,
 5.28, 5.8, 6.04, 5.59, 5.42, 6.17, 7.72, 17.5, 534, 11.3,
 6.94, 6.14, 5.81, 5.22, 5.67, 5.76, 6.08, 6.13, 6.06, 6.21,
 6.6, 6.15, 7.34, 7.26, 7.47, 13.4, 6.92, 7.66, 13.8, 7.17,
 7.54, 7.09, 6.89, 6.74, 5.85, 7.12, 6.12, 6.45, 6.47, 6.73,
 6.99, 7.33, 7.72, 8.71, 9.55, 10.3, 11.5, 12.9, 13.6, 16.3,
 15.1, 17.5, 16.8, 18.5, 19, 20.2, 24.6, 21.2, 23.3, 20.3,
 19.3, 15.6, 16.5, 16.5, 14.8, 12.2, 11.8, 8.88, 7.25, 6.04,
 5.44, 6.07, 4.44, 4.54, 4.17, 3.93, 3.72, 3.58, 3.07, 3.35,
 3.13, 3.35, 2.93, 2.85, 2.82, 2.93, 2.83, 2.85, 2.85, 2.92,
 2.73, 2.44, 2.73, 2.78, 2.64, 3.08, 2.92, 2.84, 2.78, 2.76,
 3.07, 2.64, 2.78, 2.74, 2.88, 2.69, 2.84, 2.82, 2.86, 2.88,
 2.71, 2.81, 2.89, 2.9, 2.8, 2.67, 2.69, 2.61, 2.67, 2.62,
 2.7, 2.39, 2.37, 2.47, 2.54, 2.44, 2.46, 2.44, 2.29, 2.51,
 2.33, 2.35, 2.35, 2.4, 2.96, 2.66, 2.18, 2.11, 2.32, 2.31,
 2.23, 2.18, 2.33, 2.35, 2.17, 2.22, 2.27, 2.23, 2.31, 2.13,
 2.41, 2.59, 2.6, 2.28, 2.14, 2.13, 2.09, 2.17, 2.14, 2.16,
 2.15, 2.28, 2.25, 2.24, 2.35, 2.13, 2.26, 2.15, 2.15, 2.32,
 2.17, 2.41, 2.44, 3.11, 15, 5.97, 2.93, 2.32, 2.25, 2.05,
 2.28, 2.45, 2.21, 2.32, 2.15, 2.26, 2.34, 2.14, 2.04, 2.29,
 2.06, 2.19, 2.16, 2.17, 1.94, 2.14, 2.17, 2.36, 2.16, 2.42,
 2.19, 2.26, 2.41, 2.45, 2.42, 2.09, 2.06, 2.06, 2.02, 2.2,
 2.17, 2.1, 2.23, 2.27, 2.05, 2.01, 2.08, 1.99, 2, 2.04,
 2.01, 1.88, 2.05, 2.09, 2, 1.87, 2.12, 2.03, 1.97, 2.13,
 2.04, 2.47, 3.3, 2.41, 2.05, 2.22, 2.02, 2.07, 2.16, 1.92,
 2.08, 2.04, 2.15, 2.07, 1.91, 2.13, 1.89, 2.31, 1.97, 1.99,
 1.95, 2.06, 2.65, 2.24, 1.94, 1.81, 2.04, 1.83, 1.89, 1.91,
 2.07, 1.9, 2.11, 2.03, 1.87, 1.94, 1.92, 1.91, 1.87, 2.02,
 1.89, 1.87, 1.9, 1.77, 1.87, 1.8, 1.83, 1.9, 1.92, 1.99,
 1.92, 1.87, 2.68, 1.88, 1.77, 1.78, 1.78, 1.7, 1.82, 1.88,
 1.79, 1.86, 1.98, 1.78, 1.88, 1.96, 1.94, 2.48, 2, 1.92,
 1.86, 2.16, 1.93, 2.08, 2.07, 2.01, 2.02, 2.12, 1.92, 1.84,
 1.9, 1.91, 1.8, 1.89, 1.86, 1.8, 1.84, 1.9, 1.85, 2.01,
 1.82, 1.89, 1.86, 1.86, 1.7, 1.8, 2.02, 2.31, 22.3, 2.92,
 2.03, 1.98, 1.85, 1.97, 1.93, 1.78, 1.7, 2.02, 1.92, 1.89,
 1.81, 1.91, 1.99, 1.94, 2.08, 1.89, 2.03, 1.87, 2.07, 1.93,
 2.03, 1.81, 1.87, 2, 2.08, 2.07, 2.16, 2.13, 1.98, 2.01,
 2.09, 1.93, 1.93, 2.03, 2.04, 2.14, 2.02, 1.96, 2.17, 1.92,
 2.1, 2.2, 2.14, 1.99, 1.95, 1.93, 1.93, 1.98, 2.08, 1.97,
 2.01, 1.95, 1.78, 2.03, 2.03, 1.93, 1.95, 1.9, 1.86, 1.85,
 1.86, 1.9, 2.16, 1.81, 1.97, 2.07, 2.57, 1.81, 1.91, 1.74,
 1.71, 1.77, 1.82, 1.92, 1.86, 1.9, 1.8, 1.8, 1.91, 1.76,
 1.93, 1.88, 1.89, 1.88, 1.93, 1.93, 2.08, 1.93, 1.97, 2.01,
 2.04, 2.08, 2.14, 2.07, 2.05, 2.07, 2.14, 2.07, 2.24, 2.42,
 2.85, 2.03, 2.29, 2.11, 2.22, 2.29, 2.45, 2.33, 2.27, 2.17,
 2.26, 2.32, 2.34, 2.15, 2.08, 2.2, 2.25, 2.17, 2.13, 2.03,
 2.12, 2.13, 2.24, 2.14, 2.28, 1.97, 2.05, 1.9, 1.92, 2.04,
 2.08, 1.93, 2.08, 2.03, 2.11, 1.92, 2.1, 1.98, 1.96, 2.06,
 2.19, 2.33, 2.07, 1.97, 2.14, 2.12, 2.26, 2.32, 2.14, 2.29,
 2.64, 3.19, 2.99, 3.28, 3.05, 2.52, 2.49, 2.5, 2.55, 2.61,
 2.86, 2.63, 2.46, 2.3, 2.44, 2.12, 2.3, 2.25, 2.17, 2.49,
 2.39, 2.42, 2.31, 2.29, 2.44, 2.48, 2.41, 2.36, 2.35, 2.34,
 2.37, 2.08, 2.24, 2.15, 1.98, 2.01, 2.02, 2.05, 2.04, 1.95,
 1.92, 1.89, 1.89, 1.88, 2.06, 2.04, 1.98, 1.89, 2.11, 1.98,
 1.96, 2.03, 2.08, 2.05, 2.31, 2.12, 1.94, 2.12, 2.12, 1.95,
 2.04, 2.04, 2.11, 2.19, 2.06, 2.05, 2.06, 1.94, 1.84, 1.96,
 1.92, 1.84, 1.97, 1.99, 1.96, 2.09, 1.82, 1.81, 1.98, 1.88,
 1.8, 1.72, 1.97, 1.73, 1.82, 1.76, 1.81, 1.73, 1.75, 1.73,
 1.72, 1.84, 1.76, 1.85, 1.8, 1.78, 1.68, 1.91, 1.83, 1.72,
 1.89, 1.65, 1.86, 1.74, 1.64, 1.89, 1.71, 1.62, 1.81, 1.64,
 1.72, 1.69, 1.91, 1.72, 1.82, 1.75, 1.66, 1.74, 1.81, 1.79,
 1.92, 1.77, 1.62, 1.83, 1.7, 1.79, 1.92, 1.67, 1.68, 1.8,
 1.7, 1.84, 1.77, 1.75, 1.9, 1.76, 1.7, 1.76, 1.64, 1.75,
 1.76, 1.73, 1.7, 1.76, 1.66, 1.68, 1.65, 1.77, 1.75, 1.84,
 1.65, 1.63, 1.79, 1.76, 1.79, 1.54, 1.73, 1.78, 1.74, 1.67,
 1.64, 1.82, 1.66, 1.77, 1};
  static EXODoubleWaveform wf(fft_mag, sizeof(fft_mag)/sizeof(fft_mag[0]));
  return wf;
}

//______________________________________________________________________________
vector<string> EXOMiscUtil::SearchForMultipleFiles(const string& filename) 
{
    // Similar to SearchPathForFile except that this looks for multiple files
    // given a glob-like expansion structure.  For example, one can send dir/*
    // and it will return all the files that satisfy the regular expressions in
    // the vector of strings.  Uses defaultPath. 

    vector<string> returnVector;
    string file;
    wordexp_t we; // lets expand it
    int i;
    if ((i=wordexp(filename.c_str(),&we,WRDE_NOCMD))==0) { // success
        for (size_t j=0; j<we.we_wordc;j++) {
          file = SearchForFile(we.we_wordv[j]); 
          if (file != "") returnVector.push_back(file);
        }
    } else {
        cout << "wordexp() failed on '" << filename 
           <<"' with error " << i << endl;;
        file = filename;
    }
    wordfree(&we);
   
    return returnVector;
}

//______________________________________________________________________________
string EXOMiscUtil::SearchForFile(const string& filename)
{
    // Search for one file, uses defaultPath.  Returns the full path or "" if
    // not found.
    TString aString(filename);
    const char* retValue = gSystem->FindFile(GetDefaultPath().c_str(), aString);
    return (retValue ? retValue : "");
}

//______________________________________________________________________________
EXOMiscUtil::EChannelType EXOMiscUtil::TypeOfChannel(int SoftwareChannelNumber)
{
  // Convert software channel number to EChannelType object.
  if(SoftwareChannelNumber <= kUWireIndOffset -3*NCHANNEL_PER_WIREPLANE) return kOtherTag;
  if(SoftwareChannelNumber <= kUWireIndOffset -2*NCHANNEL_PER_WIREPLANE) return kUWireInduction;
  if(SoftwareChannelNumber <= kUWireIndOffset -NCHANNEL_PER_WIREPLANE) return kOtherTag;
  if(SoftwareChannelNumber <= kUWireIndOffset) return kUWireInduction;
  if(SoftwareChannelNumber < -2) return kOtherTag;
  if(SoftwareChannelNumber < 0) return kAPDSumOfGangs;
  if(SoftwareChannelNumber < NCHANNEL_PER_WIREPLANE) return kUWire;
  if(SoftwareChannelNumber < 2*NCHANNEL_PER_WIREPLANE) return kVWire;
  if(SoftwareChannelNumber < 3*NCHANNEL_PER_WIREPLANE) return kUWire;
  if(SoftwareChannelNumber < 4*NCHANNEL_PER_WIREPLANE) return kVWire;
  if(SoftwareChannelNumber < (int)NUMBER_READOUT_CHANNELS) return kAPDGang;
  return kOtherTag;
}
//______________________________________________________________________________
void EXOMiscUtil::DisplayInProgram(std::vector<TObject*>& vecofhists, const std::string& message, const std::string& drawOpts)
{
  std::string opts = drawOpts;
  if(vecofhists.size() == 0) return;
  TCanvas& c1 = EXOMiscUtil::GetDebugCanvas();
  vecofhists[0]->Draw(opts.c_str());
  if(opts == "") opts = "same";
  else opts += ",same";
  for(size_t i=1; i<vecofhists.size(); i++){
	 vecofhists[i]->Draw(opts.c_str());
  }
  c1.Update();
  std::cout << message << std::endl;
  ShowDebugPrompt();
}

//______________________________________________________________________________
void EXOMiscUtil::DisplayInProgram(TObject& hist, const std::string& message, const std::string& drawOpts)
{
  TCanvas& c1 = EXOMiscUtil::GetDebugCanvas();
  hist.Draw(drawOpts.c_str());
  c1.Update();
  std::cout << message << std::endl;
  ShowDebugPrompt();
}

//______________________________________________________________________________
void EXOMiscUtil::DumpHistToFile(std::vector<TObject*>& vecofhists, const std::string& filename, const std::string& message, const std::string& drawOpts)
{
  std::string opts = drawOpts;
  if(vecofhists.size() == 0) return;
  TCanvas& c1 = EXOMiscUtil::GetDebugCanvas();
  vecofhists[0]->Draw(opts.c_str());
  if(opts == "") opts = "same";
  else opts += ",same";
  for(size_t i=1; i<vecofhists.size(); i++){
	 vecofhists[i]->Draw(opts.c_str());
  }
  c1.Update();
  c1.SaveAs(filename.c_str());
  std::cout << message << std::endl;
}

//______________________________________________________________________________
void EXOMiscUtil::DumpHistToFile(TObject& hist, const std::string& filename, const std::string& message, const std::string& drawOpts)
{
  TCanvas& c1 = EXOMiscUtil::GetDebugCanvas();
  hist.Draw(drawOpts.c_str());
  c1.Update();
  c1.SaveAs(filename.c_str());
  std::cout << message << std::endl;
}

//______________________________________________________________________________
TTimeStamp& EXOMiscUtil::GetStartTimeOfProcess()
{
  static TTimeStamp gfStartTimeOfProcess;
  return gfStartTimeOfProcess;
}

//______________________________________________________________________________
EXOMiscUtil::ETPCSide EXOMiscUtil::GetTPCSide(unsigned int channel)
{
  // Based on the channel number, return the half of the TPC on which the channel lies.
  // Invalid channels (too large) currently cause a prompt exit.
  // We don't consider negative channel numbers because summed channels don't necessarily belong to a particular plane.

  // If NAPDPLANE != 2 or NWIREPLANE != 4, none of the following is really valid -- abort.
  assert(NAPDPLANE == 2 and NWIREPLANE == 4);

  assert(channel < NUMBER_READOUT_CHANNELS);

  // Wires
  if(channel < 2*NCHANNEL_PER_WIREPLANE) return EXOMiscUtil::kNorth;
  if(channel < 4*NCHANNEL_PER_WIREPLANE) return EXOMiscUtil::kSouth;

  // APDs
  if(channel - 4*NCHANNEL_PER_WIREPLANE < NUMBER_APD_CHANNELS_PER_PLANE) return EXOMiscUtil::kNorth;
  /*if(channel - 4*NCHANNEL_PER_WIREPLANE < 2*NUMBER_APD_CHANNELS_PER_PLANE)*/ return EXOMiscUtil::kSouth;
}

//______________________________________________________________________________
bool EXOMiscUtil::OnSameDetectorHalf(int channel1, int channel2)
{
  //Negative channels would make no sense
  if(channel1 < 0 || channel2 < 0){
    return false;
  }
  return GetTPCSide(channel1) == GetTPCSide(channel2);
}

//______________________________________________________________________________
double EXOMiscUtil::GetMeanUorVPositionFromChannel(int channel)
{
  assert(channel >= 0 and channel < 4*NCHANNEL_PER_WIREPLANE);
  double position = channel%NCHANNEL_PER_WIREPLANE;
  position = (position - 0.5*NCHANNEL_PER_WIREPLANE + 0.5)*CHANNEL_WIDTH;
  return position;
}

int EXOMiscUtil::GetClosestUChannel(double uPosition, EXOMiscUtil::ETPCSide side)
{
  // Returns the closest U-channel given a uPosition.
  int channel = int(uPosition/CHANNEL_WIDTH) + NCHANNEL_PER_WIREPLANE/2;
  if(uPosition < 0.0){
    channel--; // round down, not towards zero.
  }
  if(channel < 0){
    channel = 0;
  }
  if(channel >= NCHANNEL_PER_WIREPLANE){
    channel = NCHANNEL_PER_WIREPLANE-1;
  }
  if(side == kSouth){
    channel += 2*NCHANNEL_PER_WIREPLANE;
  }
  return channel;
}

int EXOMiscUtil::GetClosestVChannel(double vPosition, EXOMiscUtil::ETPCSide side)
{
  // Returns the closest V-channel given a vPosition.
  return GetClosestUChannel(vPosition,side) + NCHANNEL_PER_WIREPLANE;
}

double EXOMiscUtil::GetGaussVar()
{
  // Call through to gRandom->Gaus().  This can be used in a TFormula.
  // It will use TRandom3, which is the good one.
  return gRandom->Gaus();
}

std::map<Int_t, EXOControlRecordList> EXOMiscUtil::ExtractControlRecords(TChain& chain,
                                                                         std::vector<TClass*> FilterRecords)
{
  // Given a TChain of files, return a map from run number to the control record list for that run.
  // Since a chain may contain multiple trees for the pieces of the run, we take
  // the longest control record list for each run; that should be the most complete.
  // Apply the filter in FilterRecords, so only records derived from classes in FilterRecords will be kept.
  // (When FilterRecords is empty, get everything; this is the default.)
  // The chain need not be ordered; the returned EXOControlRecordList will be ordered by run.
  // We do require that the files are named by the usual conventions for data files.
  // Additionally, we will get entries in the TChain; do not expect the entry number to be preserved.

  std::map<Int_t, EXOControlRecordList> listMap;
  if(FilterRecords.size() == 0) FilterRecords.push_back(EXOControlRecord::Class());

  // Get the tree offset table.  (Force it to be built first.)
  chain.GetEntries();
  Long64_t* TreeOffsets = chain.GetTreeOffset();

  // Loop through the chain, getting control record lists.
  // Since generally the chain is ordered, we start at the back; however, we do not depend on this.
  size_t ntree = chain.GetNtrees();
  while(ntree > 0) {
    ntree--;
    if(chain.LoadTree(TreeOffsets[ntree]) < 0) LogEXOMsg("Failed to load tree.", EEAlert);
    TList* userInfo = chain.GetTree()->GetUserInfo();
    EXOControlRecordList* controlList = dynamic_cast<EXOControlRecordList*>(userInfo->At(1));
    if(controlList == NULL) LogEXOMsg("Failed to get control record list.", EEAlert);
    const EXOControlRecord* lastRec = controlList->GetPreviousRecord<EXOControlRecord>();
    if(lastRec == NULL) LogEXOMsg("Run has no records.", EEAlert); // Otherwise we may surprise users.
    Int_t runNo = lastRec->GetRunNumber();
    if(runNo <= 0) LogEXOMsg("Run has an uninitialized run number.", EEAlert);

    // Should we put the control record list for this file into the map?
    std::map<Int_t, EXOControlRecordList>::iterator it = listMap.find(runNo);
    if(it == listMap.end() or it->second.size() < controlList->size()) {
      listMap[runNo] = EXOControlRecordList(*controlList, FilterRecords);
    }
  }

  return listMap;
}
