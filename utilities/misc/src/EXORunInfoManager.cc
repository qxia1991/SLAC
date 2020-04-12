//______________________________________________________________________________
//
// EXORunInfoManager provides the ability to access information about a run.
//
// The main function that will be called is EXORunInfoManager::GetRunInfo,
// which return an EXORunInfo object given a run number
//
// An additional function is GetDataSet, which returns a data set for a given
// quality.  For example:
//
//  const EXODataSet& ds = 
//    EXORunInfoManager::GetDataSetWithQuality("Data/Processed/masked", "GOLDEN");
//
// will return a data set with this particular label.  The input is case
// sensitive!
//
// A python script to output all the masked files in the golden run list would
// look like:
//
//  ds = ROOT.EXORunInfoManager.GetDataSetWithQuality("Data/Processed/masked", "GOLDEN")
//  for run in ds:
//      for afile in run.GetRunFiles():
//          print afile.GetFileLocation()
//
//
// It's also possible to send arbitrary filters to GetDataSet, see the website:
//
// https://confluence.slac.stanford.edu/display/exo/EXO+Datacatalog+REST+API
//
// for more information as to what filters can be passed.
//
// It's possible to access all data sets in the Data Catalog, for example:
//
//  ds = ROOT.EXORunInfoManager.GetDataSet("MC/Phase2/P2_APDFrame_Th232")
//  for run in ds:
//      for afile in run.GetRunFiles():
//          afile.Print() 
// 
// Prints out information for the Phase2 MC data set for the P2_APDFrame_Th232
// simulation.
//
// ** Data-Run Information **
//
// Data runs have special data that are stored, in particular things like run
// comments and exposure time, etc.  These are available by using the
// GetDataRunInfo function which is called for a single function.
//
// For example:
//
//   EXORunInfoManager::GetDataRunInfo(4001).Print()
//
// outputs:
//
//  Data Run Information for run: 4001
//    Key: endTime, Value: Mon, 23 Jul 2012 15:40:55
//    Key: exposure, Value: 4851086728000
//    Key: fullTypeName, Value: Data-Source calibration
//    Key: quality, Value: GOOD
//    Key: run, Value: 4001
//    Key: startTime, Value: Mon, 23 Jul 2012 14:20:01
//
// One can also grab multiple runs using: EXORunInfoManager::GetDataRunsInfo(4000, 4010)
//
// In python, one can loop over this:
//
//   tmp = ROOT.EXORunInfoManager.GetDataRunsInfo(4000, 4010)
//   for i in tmp: i.Print()
// 
//______________________________________________________________________________

#include "EXOUtilities/EXORunInfoManager.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOXmlParser.hh"
#include "TString.h"
#include <sstream>
#include <iostream>
#ifdef HAVE_CURL_CURL_H
#include <curl/curl.h>
#endif
#ifdef HAVE_ROOT_XML
#include "TDOMParser.h"
#include "TXMLNode.h"
#endif

#define CHECK_OR_RETURN(obj, ret, msg)              \
    if (!obj) {                                     \
      LogEXOMsg(std::string("XML Parse error: ") +  \
        msg, EEError);                              \
      return ret;                                   \
    }


EXORunInfoManager::EXORunInfoManager(const std::string& baseServer) :
  fBaseServer(baseServer)
{
#ifdef HAVE_CURL_CURL_H
  curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
}
//______________________________________________________________________________
EXORunInfoManager::~EXORunInfoManager()
{
#ifdef HAVE_CURL_CURL_H
  curl_global_cleanup();
#endif
}
//______________________________________________________________________________
EXORunInfoManager& EXORunInfoManager::GetRunMgr()
{
  // Returns singleton Run Info Manager
  static EXORunInfoManager gfRunMgr("exo-data.slac.stanford.edu/ExoDatacat");
  return gfRunMgr;
}

//______________________________________________________________________________
const EXODataRunInfo& EXORunInfoManager::GetDataRunInfo(Int_t runNumber)
{
  // Returns run information for a *data* run (which is special).  If this run
  // has not yet been requested, it will retrieve and cache the information for
  // this run.

  EXORunInfoManager& me = GetRunMgr();
  if (me.fDataRunInfoCache.find(runNumber) == me.fDataRunInfoCache.end()) {
    // Populate the run information
    DRIList alist = me.GetDataRunsInfo(runNumber, runNumber);
    if (alist.size() == 0) me.fDataRunInfoCache[runNumber] = EXODataRunInfo();
    else me.fDataRunInfoCache[runNumber] = *alist.begin();
  }
  return me.fDataRunInfoCache[runNumber];
}
//______________________________________________________________________________
const EXORunInfo& EXORunInfoManager::GetRunInfo(Int_t runNumber, const std::string& dir)
{
  // Returns run information given a certain run and directory.  If it hasn't
  // yet been requested, it will retrieve and cache the information for this
  // run.

  EXORunInfoManager& me = GetRunMgr();
  RunTag temp = make_pair(dir, runNumber);
  if (me.fRunCache.find(temp) == me.fRunCache.end()) {
    // Populate the run information
    me.PopulateRunInfo(runNumber, dir);
  }
  return me.fRunCache[temp];
}

//______________________________________________________________________________
const EXODataSet& EXORunInfoManager::GetDataSetWithQuality(
    const std::string& dir,
    const std::string& quality)
{
  // Returns data set for a given quality.  The result is cached, so that
  // subsequent calls will return almost immediately.

  std::string quality_filter = ""; 
  if (quality != "") quality_filter = "quality==\"" + quality + "\"";
  return EXORunInfoManager::GetDataSet(dir, quality_filter);
}


//______________________________________________________________________________
const EXODataSet& EXORunInfoManager::GetDataSet(const std::string& dir,
                                                const std::string& filter)
{
  // Returns data set for a given filter.  The result is cached, so that
  // subsequent calls will return almost immediately.

  EXORunInfoManager& me = GetRunMgr();
  
  DSTag temp = make_pair(dir, filter);
  DSMap::const_iterator ret_it = me.fDataSetCache.find(temp);
  if (ret_it == me.fDataSetCache.end()) {
    EXODataSet ds(dir, filter);
    RunNumList tmp = me.PopulateRunInfo(dir, filter); 
    for (RunNumList::const_iterator it = tmp.begin();
         it != tmp.end(); it++) {
      ds.fRunList.push_back(*it);
    } 
    ret_it = me.fDataSetCache.insert(make_pair(temp,ds)).first; 
  }
  return ret_it->second; 
}

//______________________________________________________________________________
void EXORunInfoManager::PrintDirectoryContents(const std::string& dir) 
{
  // Print out the directory contents of a given directory, e.g. below EXO in
  // the data catalog.

#ifdef HAVE_ROOT_XML
  EXORunInfoManager& me = EXORunInfoManager::GetRunMgr();
  std::string startingDir = "https://" + me.fBaseServer + 
                            "/rest/children/EXO/" + dir;
  // Remove trailing slash
  while (startingDir[startingDir.size()-1] == '/') {
    startingDir.erase(startingDir.size()-1);
  }
  startingDir += ".xml";

  TDOMParser tmp;
  tmp.SetValidate(false);

  std::string readback = me.DownloadURL(startingDir); 
  if (tmp.ParseBuffer(readback.c_str(), readback.size()) != 0) {
    LogEXOMsg(Form("Error at URL '%s'", startingDir.c_str()), EEError);
    return;
  }
  

  TXMLNode* node = tmp.GetXMLDocument()->GetRootNode();

  TXMLNode* child = node->GetChildren();

  std::cout << "Outputing directory contents of EXO/" << dir << " : " << std::endl;
  while (child != NULL ) {

    TXMLNode* xml = child->GetChildren();
    while (xml != NULL) {
      if (std::string("name") == xml->GetNodeName()) {
        std::cout << "  " << xml->GetText(); 
        break;
      }
      xml = xml->GetNextNode();
    }
    
    std::cout << " : " << child->GetNodeName() << std::endl;
 
    child = child->GetNextNode();
  }
#else
  LogEXOMsg("Compiled without XML", EEError);
#endif
}

//______________________________________________________________________________
std::string BuildURL(const std::string& server, const std::string& runType,
                     const std::string& filter)
{
  // Build the URL to the server
  std::string retVal = "https://" + server + "/rest/datasets/EXO/" + 
    runType + ".xml?allMetadata=true&sort=nEventMin";

  if (filter != "") retVal += "&filter=" + filter;

  return retVal;
}

//______________________________________________________________________________
void Replace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

//______________________________________________________________________________
std::string& EscapeURL(std::string& url)
{
  // Escapes a URL as it should look
  Replace(url, " ", "%20");
  Replace(url, "&&", "%26%26");
  Replace(url, "\"", "%22");
  return url; 
}

//______________________________________________________________________________
EXORunInfoManager::RunNumList
   EXORunInfoManager::PopulateRunInfo(Int_t runNumber, const std::string& dir)
{
  // Populates an EXORunInfo object given a certain run number and directory.
  // It appends information for any metadata associated with it.

  std::string filter = "run==";
  std::ostringstream ss;
  ss << runNumber;
  filter += ss.str(); 
  return PopulateRunInfo(dir, filter);
}

//______________________________________________________________________________
EXORunInfoManager::RunNumList
   EXORunInfoManager::PopulateRunInfo(const std::string& runType, 
                                      const std::string& filter)
{
  // Populate the run info objects, returning a list of affected objects

  // Convenience variable
  EXORunInfoManager& me = GetRunMgr();

  RunNumList retSet;
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
  return retSet;
#else
  TDOMParser tmp;
  tmp.SetValidate(false);
  std::string url = BuildURL(fBaseServer, runType, filter);
  std::string readback = me.DownloadURL(url); 
  if (tmp.ParseBuffer(readback.c_str(), readback.size()) != 0) {
    LogEXOMsg(Form("Error at URL '%s'", url.c_str()), EEError);
    return RunNumList();
  }


  // Use xml parser to do further parsing
  EXOXmlParser xmlParse(tmp.GetXMLDocument());
  
  TXMLNode* node = xmlParse.LookUp("dataset");
  CHECK_OR_RETURN(node, RunNumList(), url)
  while (node != NULL ) {

    // Grab file system path name
    TXMLNode* xml = xmlParse.LookUp(node, "latest:master:fileSystemPath");
    CHECK_OR_RETURN(xml, RunNumList(), url)
  
    EXORunFile newFile(xml->GetText());
    
    // Now grab all the metadata
    EXORunFile::MDMap& md = newFile.fAllMD.fAllMetaData;
    xml = xmlParse.LookUp(node, "metadata");
    CHECK_OR_RETURN(xml, RunNumList(), url)
    TXMLNode* child = xml->GetChildren();
    while (child != NULL) {
      TXMLNode* key = xmlParse.LookUp(child, "key");
      CHECK_OR_RETURN(key, RunNumList(), url);
      TXMLNode* val = key->GetNextNode();
      CHECK_OR_RETURN(val, RunNumList(), url);
      EXOMetadata newMD(key->GetText(), val->GetText());
      md[key->GetText()] = newMD;
  
      child = child->GetNextNode();
    }
    xml = xmlParse.LookUp(node, "latest:master:eventCount");
    if(xml) {
      EXOMetadata newMD("eventCount", xml->GetText());
      md["eventCount"] = newMD;
    }

    // Find the run number, we assume that runMin and runMax are the same
    xml = xmlParse.LookUp(node, "latest:master:runMin");
    CHECK_OR_RETURN(xml, RunNumList(), url)
    EXOMetadata tmpMD("", xml->GetText());
    Int_t runNumber = tmpMD.AsInt();
     
    // Insert into the run cache
    EXORunInfo& ri = me.fRunCache[make_pair(runType, runNumber)];
    ri.fRunNumber = runNumber;
    ri.fRunMapFiles.insert(newFile);
    ri.fDirName = runType;
    
    node = node->GetNextNode();
    retSet.insert(runNumber);
  }
  return retSet; 
#endif
}

//______________________________________________________________________________
EXODataRunInfo EXORunInfoManager::PopulateDataRunInfo(TXMLNode* node) 
{
  // Populate a single run node and return it 

  EXODataRunInfo dri;
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
#else
  EXOMetadataSet& mds = dri.fAllMD;
  node = node->GetChildren();
  while (node != NULL ) {
    EXOMetadata new_md(node->GetNodeName(), node->GetText());
    if (new_md.GetKey() == "run") {
      dri.fRunNumber = new_md.AsInt();
    } else {
      mds.fAllMetaData[new_md.GetKey()] = new_md;
    }
    node = node->GetNextNode();
  }
#endif
  return dri;

}

//______________________________________________________________________________
EXORunInfoManager::DRIList EXORunInfoManager::GetDataRunsInfo(Int_t minRunNumber, Int_t maxRunNumber)
{
  // Populate the data-run info using the run function of the RESTful
  // interface.
  DRIList retList;
#ifndef HAVE_ROOT_XML
  LogEXOMsg("Compiled without XML", EEError);
#else

  EXORunInfoManager& me = GetRunMgr();

  std::ostringstream os;
  os << minRunNumber;
  std::string url = "https://" + me.fBaseServer 
                    + "/rest/runs?runMin=" + os.str() + "&runMax=";
  os.str("");
  os << maxRunNumber;
  url += os.str();

  TDOMParser tmp;
  tmp.SetValidate(false);
  std::string readback = me.DownloadURL(url); 
  if (tmp.ParseBuffer(readback.c_str(), readback.size()) != 0) {
    LogEXOMsg(Form("Error at URL '%s'", url.c_str()), EEError);
    return retList;
  }

  TXMLNode* node = tmp.GetXMLDocument()->GetRootNode();
  CHECK_OR_RETURN(node, retList, url)

  TXMLNode* therun = node->GetChildren();

  while (therun != NULL)  {
    retList.insert(me.PopulateDataRunInfo(therun));    
    therun = therun->GetNextNode();
  }
#endif

  return retList;

}

size_t download_function(char* ptr, size_t size, size_t nmemb, void *userdata)
{
  std::string* astr = static_cast<std::string*>(userdata);
  astr->append(ptr, size*nmemb);
  return size*nmemb;
}

//______________________________________________________________________________
std::string EXORunInfoManager::DownloadURL(const std::string& url)
{
  // Download and return a string of data bytes at a URL
#ifdef HAVE_CURL_CURL_H
  std::string localURL = url;
  EscapeURL(localURL);
  std::string retString;
  CURL *curl_handle = curl_easy_init();
    /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, (char *)localURL.c_str());

  curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10);

  // Want to avoid CURL SIGNALS
  curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, download_function);

  /* we pass our handle struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&retString);

  // Set xml accept header, we only download xml
  struct curl_slist *slist = curl_slist_append(NULL, "Accept: application/xml"); 
  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist); 

  CURLcode curl_ret = curl_easy_perform(curl_handle);
  
  // Get http return code
  long http_code = 0;
  curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

  curl_slist_free_all(slist);
  curl_easy_cleanup(curl_handle);

  if (http_code != 200 || curl_ret != 0) {
    if (http_code != 200) {
      LogEXOMsg(Form("Received error response %li at %s",http_code, url.c_str()), EEError);
    } else {
      LogEXOMsg("Error transferring: " + url, EEError);
    }
    retString = ""; 
  }
  return retString;
#else
  LogEXOMsg("Compiled without curl!", EEError);
  return "";
#endif
}
