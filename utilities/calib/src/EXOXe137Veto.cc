/////////////////////////////////////////////////////////////
//
// This class is designed to perform the Xe137 veto cuts
//
/////////////////////////////////////////////////////////////

#include "EXOCalibUtilities/EXOXe137Veto.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/SystemOfUnits.hh"
#include <sstream>
#include <cmath>

std::vector<std::string> EXOXe137Veto::fColumnNames = EXOXe137Veto::GetColumnNames();
bool EXOXe137Veto::fUseDatabase = true;
double EXOXe137Veto::fMinEnergy = 0;
double EXOXe137Veto::fMaxEnergy = 1e16;
bool EXOXe137Veto::fRequire2DCluster = true;
double EXOXe137Veto::fTimeWindow = 0;
double EXOXe137Veto::fSpatialExtension = 0;
bool EXOXe137Veto::fUseZ137XeVetoCut = true;
bool EXOXe137Veto::fUseXY137XeVetoCut = true;
bool EXOXe137Veto::fIgnoreH1Line = true;
TRandom3 EXOXe137Veto::fRandomGenerator;
std::vector<Xe137VetoTrigger> EXOXe137Veto::fUserXe137VetoTriggerList;

std::vector<std::string> EXOXe137Veto::GetColumnNames()
{
  // specify the column names in the DB
  
  std::vector<std::string> names;
  names.push_back("RunNumber");
  names.push_back("EventNumber");
  names.push_back("EventTime");
  names.push_back("Multiplicity");
  names.push_back("NumCC");
  names.push_back("NumSC");
  names.push_back("Energy");
  names.push_back("VetoMultiplicity");
  names.push_back("IsMissingPosition");
  names.push_back("MinZ");
  names.push_back("MaxZ");
  names.push_back("MinY");
  names.push_back("MaxY");
  names.push_back("MinX");
  names.push_back("MaxX");
  names.push_back("MinXSkip");
  names.push_back("MinYSkip");

  std::sort(names.begin(),names.end());
  return names;
}

EXOXe137Veto::EXOXe137Veto() : EXOCalibBase()
{
  fDBXe137VetoTriggerList.clear();
  //fUserXe137VetoTriggerList.clear();
  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;  
}

size_t EXOXe137Veto::GetXe137VetoTriggerListSize() const
{
  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;  
  return fXe137VetoTriggerList->size();
}

const Xe137VetoTrigger* EXOXe137Veto::GetXe137VetoTrigger(size_t pos) const
{
  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;  
  if(pos > fXe137VetoTriggerList->size())
    return NULL;
  return &fXe137VetoTriggerList->at(pos);
}


void EXOXe137Veto::SetEnergyWindow(double e_min, double e_max)
{
  // Set the energy window allowed for trigger events
  
  fMinEnergy = e_min; fMaxEnergy = e_max;
}

void EXOXe137Veto::IgnoreH1Line(bool ignore)
{
  // Set to ignore trigger events within the H1 line (2.1 - 2.3 MeV)
  fIgnoreH1Line = ignore;
}

void EXOXe137Veto::SetXe137VetoCuts(double time_window, double spatial_extend, bool useZ, bool useXY)
{
  // Set options for veto cuts
  
  fTimeWindow = time_window;
  SetXe137VetoSpatialCuts(spatial_extend,useZ,useXY);
}

void EXOXe137Veto::SetXe137VetoSpatialCuts(double spatial_extend, bool useZ, bool useXY)
{
  // Set options for spatial veto cuts

  // if spatial_extend is strictly negative, then it veto the TPC in which the trigger event was found
  // by comparing the signal of the Z coordinate
  
  fSpatialExtension = spatial_extend;
  fUseZ137XeVetoCut = useZ;
  fUseXY137XeVetoCut = (fSpatialExtension<0) ? false : useXY;
}

void EXOXe137Veto::Require2DCluster(bool req)
{
  fRequire2DCluster = req;
}

bool EXOXe137Veto::RandXe137Vetoed(Int_t event_time, const EXOFiducialVolume& fidVol) const
{
  // Loop over vector of Xe137 veto trigger events to check if a particular time
  // (in seconds) is inside one of the vetoed times. Then, use volume set by trigger event
  // divided by 'norm'  to randomize whether the event is vetoed

  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;

  bool is_vetoed_137Xe_time = false;
  bool is_vetoed_137Xe_vol = false;
  bool is_vetoed_137Xe = false;
  
  for (std::vector<Xe137VetoTrigger>::const_iterator veto = fXe137VetoTriggerList->begin(); veto != fXe137VetoTriggerList->end() ; veto++)
  {
    // veto trigger list should be sorted by time, break loop once it reaches event time
    
    bool is_vetoed_137Xe_time = false;
    bool is_vetoed_137Xe_vol = false;

    if(!ConsiderTriggerEvent(*veto))
      continue;
    
    if(event_time < veto->fTime) //move/break on if event before veto
      break;
    if(event_time > veto->fTime + fTimeWindow) // skip if event after veto + veto time window
      continue;

    // if here, must be within veto window in time
    is_vetoed_137Xe_time = true;

    if(fUseZ137XeVetoCut || fUseXY137XeVetoCut)
    {
      double rate = 0; // this will dictate the rate to set the veto true
      if(fSpatialExtension < 0) // if vetoing volume per TPC then 1/2 chance to set veto
      {
        if((veto->fMinZ < 0 && veto->fMaxZ < 0) || (veto->fMinZ > 0 && veto->fMaxZ > 0)) // both in same TPC
          rate = 0.5;
        else
          rate = 1; // in both TPCs, always set veto (random number between 0,1)
      }
      else // otherwise veto a compact volume then chance to set veto is proportional to volume cut
      {
        // set coordinates for the vetoed volume depending on Xe137 cuts usage
        double zMinExt = (fUseZ137XeVetoCut) ? veto->fMinZ - fSpatialExtension : -fidVol.GetMaxZ();
        if(zMinExt < -fidVol.GetMaxZ()) zMinExt = -fidVol.GetMaxZ();
        double zMaxExt = (fUseZ137XeVetoCut) ? veto->fMaxZ + fSpatialExtension : fidVol.GetMaxZ();
        if(zMaxExt > fidVol.GetMaxZ()) zMaxExt = fidVol.GetMaxZ();
        
        double xyRadExt = (fUseXY137XeVetoCut) ? veto->fRadiusXY + fSpatialExtension : REFLECTORINNERRAD;
        if(xyRadExt > REFLECTORINNERRAD) xyRadExt = REFLECTORINNERRAD;

        // calculate vetoed volume in physical region, copied from EXOFiducialVolume class
        double circle = M_PI*xyRadExt*xyRadExt;
        double segment = 0.;
        if(xyRadExt > WIREPLANE_RADIUS)
        {
          double r = xyRadExt;
          double h = r - WIREPLANE_RADIUS;
          segment = r*r*acos(1.-(h/r));
          segment -= sqrt((2*r*h)-(h*h)) * (r-h);
        }
        double volume = 2. * (circle - 6*segment) * ( (zMaxExt-zMinExt) - 2.*fidVol.GetMinZ()); // 2 TPCs * circle area in physical region * z-length
        
        // now get FV and rate of veto events
        double fv = fidVol.GetVolumeInMillimeters3();
        double rate = volume / fv;
      }
      
      double rand = fRandomGenerator.Rndm();
      //std::cout << "Using rate of " << rate << " " << volume << " " << fv << " " << rand << std::endl;
      if(rand < rate)
          is_vetoed_137Xe_vol = true;
    }

    is_vetoed_137Xe = is_vetoed_137Xe_time && (!(fUseZ137XeVetoCut||fUseXY137XeVetoCut) || is_vetoed_137Xe_vol);
    
    if(is_vetoed_137Xe)
      return true;
  }
  
  return is_vetoed_137Xe;
} 

bool EXOXe137Veto::IsXe137Vetoed(Int_t event_time, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const
{
  // Loop over vector of Xe137 veto trigger events to check if a particular time
  // (in seconds) is inside one of the vetoed times. Then, loop over clusters
  // Return whether to veto this event

  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;  
  
  bool is_vetoed_137Xe = false;
  
  for (std::vector<Xe137VetoTrigger>::const_iterator veto = fXe137VetoTriggerList->begin(); veto != fXe137VetoTriggerList->end() ; veto++)
  {
    // veto trigger list should be sorted by time, break loop once it reaches event time

    bool is_vetoed_137Xe_time = false;
    bool is_vetoed_137Xe_z = false;
    bool is_vetoed_137Xe_xy = false;
    
    if(!ConsiderTriggerEvent(*veto))
      continue;
    
    if(event_time < veto->fTime) //move/break on if event before veto
      break;
    if(event_time > veto->fTime + fTimeWindow) // skip if event after veto + veto time window
      continue;
    
    // if here, must be within veto window in time
    is_vetoed_137Xe_time = true;

    CheckSpatialConditions(*veto, is_vetoed_137Xe_z,is_vetoed_137Xe_xy,cluster_z,cluster_y,cluster_x);
        
    // final check between veto flags and requested
    //std::cout << "Checks: " << is_vetoed_137Xe_time << " " << is_vetoed_137Xe_z << " " << is_vetoed_137Xe_xy << std::endl;
    is_vetoed_137Xe = (is_vetoed_137Xe_time) && (!fUseZ137XeVetoCut || is_vetoed_137Xe_z) && (!fUseXY137XeVetoCut || is_vetoed_137Xe_xy);

    if(is_vetoed_137Xe)
      return true;    
    //if(!(fFOVs.GetBooleanFlag("fUseXY137XeVetoCut")) && is_vetoed_137Xe_z) 
    //is_vetoed_137Xe = true; //So veto works if only Z cut is turned on
    
  }
  
  return is_vetoed_137Xe;
}

bool EXOXe137Veto::MCXe137Vetoed(double prob, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const
{
  // Xe137 veto for MC events.
  // Relies in a probability (prob) passed by user to decided whether or not to veto (< prob passes time condition)
  // Then randomly selects a trigger to check on veto spatial conditions

  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;  

  double rand = fRandomGenerator.Rndm();
  if(rand > prob)
    return false;
  
  bool is_vetoed_137Xe = false;
  std::vector<Xe137VetoTrigger>::const_iterator veto = DrawXe137VetoTriggerTrigger();
  bool is_vetoed_137Xe_time = true;  // passing 'prob' above is like passing the time cut

  // now move to spatial conditions
  bool is_vetoed_137Xe_z = false;
  bool is_vetoed_137Xe_xy = false;
  CheckSpatialConditions(*veto,is_vetoed_137Xe_z,is_vetoed_137Xe_xy,cluster_z,cluster_y,cluster_x);

  // final check between veto flags and requested
  is_vetoed_137Xe = (is_vetoed_137Xe_time) && (!fUseZ137XeVetoCut || is_vetoed_137Xe_z) && (!fUseXY137XeVetoCut || is_vetoed_137Xe_xy);
  
  return is_vetoed_137Xe;
}

std::vector<Xe137VetoTrigger>::const_iterator EXOXe137Veto::DrawXe137VetoTriggerTrigger() const
{
  size_t nTriggers = fXe137VetoTriggerList->size();
  std::vector<Xe137VetoTrigger>::const_iterator veto;
  do
  {
    size_t drawIndex = fRandomGenerator.Integer(nTriggers);
    veto = fXe137VetoTriggerList->begin()+drawIndex;
  }
  while(not ConsiderTriggerEvent(*veto));

  return veto;  
}


bool EXOXe137Veto::ConsiderTriggerEvent(const Xe137VetoTrigger& veto) const
{  
  // check energy window
  if(veto.fEnergy < fMinEnergy || fMaxEnergy < veto.fEnergy)
    return false;

  // reject H1 line
  if(fIgnoreH1Line && (2100 < veto.fEnergy && veto.fEnergy < 2300))
    return false;

  // check whether to consider 2D clusters
  if(fRequire2DCluster)
  {
    if((veto.fMinY < -990 || veto.fMaxY > 990) || (veto.fMinX < -990 || veto.fMaxX > 990))
      return false;
  }

  return true;
}

void EXOXe137Veto::CheckSpatialConditions(const Xe137VetoTrigger& veto, bool& is_vetoed_137Xe_z, bool& is_vetoed_137Xe_xy, const std::vector<double>& cluster_z, const std::vector<double>& cluster_y, const std::vector<double>& cluster_x) const
{
  // check whether to veto event depending on spatial cut options
  
  // check the vector of z clusters
  if(fUseZ137XeVetoCut) //only do this if Z cut is applied
  {
    if(fSpatialExtension < 0) // check whether event has cluster in the TPC of Xe137 trigger
    {
      for(std::vector<double>::const_iterator z = cluster_z.begin(); z != cluster_z.end(); z++)
      {
        if((veto.fMinZ < 0 && (*z) < 0) || (veto.fMaxZ > 0 && (*z) > 0)) // do not divide in case is null
        {
          is_vetoed_137Xe_z = true;
          is_vetoed_137Xe_xy = true;
        }
      }
    }
    else // use spatial extension for volume cut
    {
      double zMinExt = veto.fMinZ - fSpatialExtension;
      double zMaxExt = veto.fMaxZ + fSpatialExtension;
      
      for(std::vector<double>::const_iterator z = cluster_z.begin(); z != cluster_z.end(); z++)
      {
        //loop over cluster z positions
        if((*z) < zMinExt || zMaxExt < (*z)) // move on if cluster outside z extension
          continue;
        
        //if here, cluster is vetoed!
        is_vetoed_137Xe_z = true;
        break; //don't go through rest of clusters, will be vetoed no matter what
      }
    }
  }
    
  if(fUseXY137XeVetoCut) //only do this if XY cut is applied
  {
    double rExt = veto.fRadiusXY + fSpatialExtension;
    double rExt2 = rExt*rExt;
    
    for(size_t i = 0; i < cluster_x.size(); i++)
    {
      double x = cluster_x.at(i);
      double y = cluster_y.at(i);
      
      double dx = x - veto.fCenterX;
      double dy = y - veto.fCenterY;
      
      //std::cout << Form("%f %f %f %f %f\n",x,y,dx,dy,rExt2);
      
      if(dx*dx + dy*dy > rExt2)//move on if cluster not in circle created by veto
        continue;
        
      is_vetoed_137Xe_xy = true;
      break;  //no point testing other clusters, will be vetoed!
    }
  }
  return;
}

Int_t EXOXe137Veto::FindTriggerTime(Int_t event_time, bool fast, const std::vector<double>* cluster_z, const std::vector<double>* cluster_y, const std::vector<double>* cluster_x) const
{
  // find the time of an event to a Xe137 trigger
  // if fast is true (default) it only finds the earliest trigger event consistent with input time
  // veto settings are specified elsewhere
  // if fast is false, then it double checks whether the trigger indeed vetoes event

  fXe137VetoTriggerList = fUseDatabase ? &fDBXe137VetoTriggerList : &fUserXe137VetoTriggerList;

  for (std::vector<Xe137VetoTrigger>::const_iterator veto = fXe137VetoTriggerList->begin(); veto != fXe137VetoTriggerList->end() ; veto++)
  {
    // veto trigger list should be sorted by time
    if(!ConsiderTriggerEvent(*veto))
      continue;

    if(event_time < veto->fTime) //move/break on if event before veto
      break;
    
    if(event_time > veto->fTime + fTimeWindow) // skip if event after veto + veto time window
      continue;

    if(fast)
      return veto->fTime;
    else
    {
      if(IsXe137Vetoed(event_time,*cluster_z,*cluster_y,*cluster_x))
        return veto->fTime;
    }
  }

  return -1;
}


std::string EXOXe137Veto::__str__() const
{
  std::stringstream sstream;
  sstream << "-------------------------EXOXe137Veto-------------------------" << std::endl;
  
  return sstream.str();
}

void EXOXe137Veto::ReadUserFile(std::string filename)
{
  // read veto trigger list from user input file

  fUseDatabase = false;

  std::cout << "Reading Xe137 trigger list from file: " << filename << std::endl;
  std::ifstream userFile(filename.c_str());

  bool first = true;
  std::vector<std::string> names;
  
  std::string line;
  while(std::getline(userFile,line))
  {
    //std::cout << line << std::endl;
    std::istringstream iline(line);

    if(first)
    {
      std::string name;
      while(iline >> name)
        names.push_back(name);
      first = false;
      continue;
    }
    
    std::map<std::string, std::string> vals;
    for(size_t n = 0; n < names.size(); n++)
    {
      std::string val;
      iline >> val;
      vals[names[n]] = val;
      //std::cout << n << " " << names[n] << " " << val << std::endl;
    }

    Xe137VetoTrigger veto;
    veto.SetAllMembers(vals);
    //veto.Print();
    
    fUserXe137VetoTriggerList.push_back(veto);
  }
  std::cout << "Total Xe137 veto triggers read " << fUserXe137VetoTriggerList.size() << std::endl;
  
}

IMPLEMENT_EXO_CALIB_HANDLER(EXOXe137VetoHandler)

EXOCalibBase* EXOXe137VetoHandler::readDefault(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  EXOXe137Veto* newData = new EXOXe137Veto();
  newData->m_SourceOfData = EXOCalib::METADATASOURCEdefault;
  return newData;
}

EXOCalibBase* EXOXe137VetoHandler::readDB(
                                        const std::string& dataIdent,
                                        const std::string& formatVersion)
{
  using namespace EXOCalib;

  // Once newData gets returned, EXOCalibManager owns it.
  EXOXe137Veto* newData = new EXOXe137Veto();

  // parse dataIdent.  It should have form
  // TABLE_NAME:COLUMN_NAME:VALUE
  unsigned delim1  = dataIdent.find(':');
  unsigned delim2  = dataIdent.rfind(':');

  // if (delim1 == delim2) { we've got a problem }
  std::string tbl = std::string(dataIdent, 0, delim1);
  std::string col = std::string(dataIdent, delim1 + 1, delim2 - delim1 - 1);
  std::string val = std::string(dataIdent, delim2 + 1);
  
  // Select which columns to read from TABLE_NAME where COLUMN_NAME=VALUE
  std::vector<std::string> selectColumns, orderColumns;
  selectColumns = EXOXe137Veto::GetColumnNames();

  std::string where(" where ");
  where += col + std::string("= '") + val + std::string("'");
  RowResults res = GetDBRowsWith(tbl, selectColumns, orderColumns, where);

  if(res.getNRows() < 1){
    LogEXOMsg("Number of rows for this entry smaller than 1",EEError);
    delete newData;
    return 0;
  }

  std::vector<std::string> fields;
  for(size_t i=0; i<res.getNRows(); i++) {
    res.getRow(fields, i);
    //for(size_t j = 0; j < fields.size(); j++)
    //  std::cout << j << " " << selectColumns[j] << " = " << fields[j] << std::endl;
    
    Xe137VetoTrigger veto;
    try
    {
      std::map<std::string, std::string> vals;
      for(size_t j = 0; j < fields.size(); j++)
      {
        //std::cout << j << " " << selectColumns[j] << " = " << fields[j] << std::endl;
        vals[selectColumns[j]] = fields[j];
      }
      //for(std::map<std::string, std::string>::iterator val = vals.begin(); val != vals.end(); val++)
      //  std::cout << val->first << " = " << val->second << std::endl;

      veto.SetAllMembers(vals);
      //veto.Print();
      
      newData->fDBXe137VetoTriggerList.push_back(veto);
    }
    catch (EXOExceptWrongType ex) {
      LogEXOMsg("Bad row in calibration data ''", EEError);
      delete newData;
      return 0;
    }
  }

  std::sort(newData->fDBXe137VetoTriggerList.begin(),newData->fDBXe137VetoTriggerList.end());
  return newData;
}

bool  EXOXe137VetoHandler::FlavorizeSQLDB( const std::string& flavor ) {
  // to be executed before use

  //  (EXOCalibManager::GetCalibManager()).PrintHandlerInfo("137Xe_veto");
  if (! fDBisBland ) return (!fDBisBland); // if already flavorized
  std::vector<std::string> forever =
    (EXOCalibManager::GetCalibManager()).findFlavorTime(GetHandlerName(),flavor,2); // 2 is mid period --> vstart was instable as we are inconsistent on half open or fully closed.
  if (forever.size() == 0 ) {
    LogEXOMsg("Empty time string for flavor should have already caused an error",EEError);
    return false;
  }
  //for if calling inside getcalibration for //fDBisBland = false ; // set to flavorful now to avoid infinite loops
  EXOXe137Veto* mergedveto = GetCalibrationFor(EXOXe137Veto,
						     EXOXe137VetoHandler,
						     flavor,
						     forever[0]);
  //  (EXOCalibManager::GetCalibManager()).PrintHandlerInfo("137Xe_veto");
  if ( forever.size() > 1 ) {
    std::cout << forever.size() - 1 << " more records to merge for Xe137Veto from DB" << std::endl;
    for ( int ix = 1 ; ix < (int) forever.size() ; ix++){
      //std::cout <<  forever[ix] << std::endl;
      EXOXe137Veto* entry = GetCalibrationFor(EXOXe137Veto,
					      EXOXe137VetoHandler,
					      flavor,
					      forever[ix]);
      //  (EXOCalibManager::GetCalibManager()).PrintHandlerInfo("137Xe_veto");
      entry->setValiditySince(EXOTimestamp(0)); // smashing vstarts should devalidize them
      entry->setValidityTill(EXOTimestamp(0)); // smashing vends should devalidize them
      // then merge lists
      mergedveto->fDBXe137VetoTriggerList.insert(mergedveto->fDBXe137VetoTriggerList.end(),
						 entry->fDBXe137VetoTriggerList.begin(),
						 entry->fDBXe137VetoTriggerList.end() );
      // should add cleaning out the handlerinfo->m_calibBase vector but only a factor of < 2
    }//end looping forever
    std::sort(mergedveto->fDBXe137VetoTriggerList.begin(),
	      mergedveto->fDBXe137VetoTriggerList.end()   ); // already if 0
  }//end if multiple records, now finalize
  mergedveto->setValidity(EXOTimestamp(1), // possibly setting to time = 0 trips isvalid ;;always valid and will be the only valid one.
			  EXOTimestamp("2022-02-02") ) ; // numeric limits was causing a fail to invalidation -> likely signing issue
  fDBisBland = false ;
  std::cout<<"flavorized 137Xe_veto super entry of size "<<mergedveto->fDBXe137VetoTriggerList.size() <<std::endl;
  //  (EXOCalibManager::GetCalibManager()).PrintHandlerInfo("137Xe_veto");
  return true;
}
