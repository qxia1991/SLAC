//______________________________________________________________________________
// 4/04/11 S.S.
// EXOChannelMapManager.cc
// Handles which version of the ChannelMap to read.  Reads from the database
// unless HAVE_MYSQL is not set, in which case it reads in a hard-coded version
// current as of 03/2011.
// 5/6/11 Reverted default channel map to the hard-coded map to avoid a typo in
// the calibration database that causes Channel 202 to be ignored.  Change is
// temporary until Tony J. can fix the typo.

#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOUtilities/EXOErrorLogger.hh"
#include "EXOUtilities/EXOEventHeader.hh"
#include "EXOCalibUtilities/EXOChannelMapManager.hh"
#include "EXOCalibUtilities/EXOChannelMapCalib.hh"
#include "EXOCalibUtilities/EXOCalibManager.hh"


#include <string>
#include <iostream>
#include <cassert>

using namespace std;

//______________________________________________________________________________
const EXOChannelMap& 
  EXOChannelMapManager::GetChannelMapWithHeader(const EXOEventHeader& eh)
{
  // Version 2 reads the channel map from the database (3/24/11, SS).
  // Version 1 reflects the channel map as of March 2010.
  // Version 0 is a dummy channel map which was only intended
  // as a temporary placeholder. No other versions are implemented.

  // Use version 2 if connected to MYSQL DB
  // Otherwise use version 1

  EXOChannelMap::EChannelMapType version = EXOChannelMap::kDBChannelMap;

  // Only versions 0, 1, and 2 are currently implemented

  if ( version < 0 || version >= EXOChannelMap::kNumMaps ) {
    cout << "EXOChannelMap constructor version"
         << " not recognized" << endl;
    cout << "Using version 1 instead" << endl;
    cout << "This Channel Map may not be up-to-date" << endl;
    version = EXOChannelMap::kDefaultChannelMap;
  }

  // The channel map itself depends on the version.
  if ( fChannelMap->get_version() != version ) { 
  
    if ( version == EXOChannelMap::kDummyMap ) {
      MakeMeTheOwner();
      fChannelMap->ResetToDummy();
    }
    else if ( version == EXOChannelMap::kDefaultChannelMap ) { 
      // cout << "Using default Channel Map. Valid as of March 2011." << endl;
      create_map_for_version_1();
    }
    else if ( version == EXOChannelMap::kDBChannelMap ) {
      if(fChanMapIsOwned) delete fChannelMap;
      fChanMapIsOwned = false;
      fChannelMap = GetCalibrationFor(EXOChannelMapCalib, EXOChannelMapHandler, "vanilla", eh);
      if(fChannelMap == NULL) create_map_for_version_1(); // Failure was already logged by EXOCalibManager.
    }
  } 
  else if ( fChannelMap->get_version() == EXOChannelMap::kDBChannelMap ) {
    // All of the other versions have infinite validity, so there's no reason to re-check based on time.
    assert(not fChanMapIsOwned); // We shouldn't own it, since it's from the database.
    fChannelMap = GetCalibrationFor(EXOChannelMapCalib, EXOChannelMapHandler, "vanilla", eh);
    if(fChannelMap == NULL) create_map_for_version_1(); // Failure was already logged by EXOCalibManager.
  }

  return *fChannelMap;
}

//______________________________________________________________________________
void EXOChannelMapManager::create_map_for_version_1()
{
  // Create map for version 1. This map reflects the actual
  // hardware at the time the electronics was initially installed
  // at WIPP (February 2010)

  MakeMeTheOwner();
  EXOChannelMap& channelMap = *fChannelMap;

  // create memory for map array

  if ( NUM_DAQCARDS*CHANNELS_PER_DAQCARD != 18*16 ) {

    cout << "EXOChannelMapManager::create_map_for_version_1()::" << endl;
    cout << "\tthe number of DAQ channels appears to be wrong" << endl;
    cout << "\tusing 18 cards times 16 channels instead" << endl;

  }

  int * m_map = new int[288];

  int daqcard;

  // Create Map (explicitly)

  daqcard = 0; // module address 0x00
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 176;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 179;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 181;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 177;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 182;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 180;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 178;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 158;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 161;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 163;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 159;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 162;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 160;

  daqcard = 1; // module address 0x01
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 164;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 167;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 169;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 165;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 168;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 166;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 152;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 155;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 153;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 157;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 154;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 156;

  daqcard = 2; // module address 0x02
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 170;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 173;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 171;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 175;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 172;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 174;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 183;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 186;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 188;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 184;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 187;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 185;

  daqcard = 3; // module address 0x03
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 204;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 206;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 203;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 207;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 202;
  // offline channel 205 doesn't really exist
  // because the cable trace for this channel was
  // broken during TPC assembly. So in the hardware
  // this channel is soldered to channel 202.
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 205;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 193;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 191;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 194;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 190;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 192;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 189;

  daqcard = 4; // module address 0x04
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 212;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 210;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 213;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 209;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 211;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 208;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 222;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 224;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 221;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 225;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 223;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 220;

  daqcard = 5; // module address 0x05
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 195;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 216;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 218;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 215;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 219;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 217;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 214;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 197;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 199;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 201;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 196;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 200;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 198;

  daqcard = 6; // module address 0x08
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 15;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 16;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 17;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 18;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 0;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 9;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 10;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 11;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 12;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 13;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 14;

  daqcard = 7; // module address 0x09
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 2;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 3;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 4;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 5;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 6;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 7;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 8;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = 37;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 36;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 35;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 34;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 33;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 32;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 31;

  daqcard = 8; // module address 0x0A
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 24;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 23;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 22;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 21;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 20;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 19;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 30;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 29;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 28;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 27;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 26;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 25;

  daqcard = 9; // module address 0x0B
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 88;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 87;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 86;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 85;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 84;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 83;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 94;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 93;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 92;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 91;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 90;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 89;

  daqcard = 10; // module address 0x0C
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 82;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 81;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 80;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 79;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 78;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 77;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 76;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = 105;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 106;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 107;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 108;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 109;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 110;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 111;

  daqcard = 11; // module address 0x0D
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 99;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 100;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 101;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 102;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 103;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 104;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 112;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 113;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 95;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 96;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 97;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 98;

  daqcard = 12; // module address 0x10
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 73;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 74;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 75;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 57;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 58;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 59;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 67;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 68;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 69;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 70;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 71;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 72;

  daqcard = 13; // module address 0x11
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 60;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 61;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 62;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 63;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 64;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 65;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 66;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = 56;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 55;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 54;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 53;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 52;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 51;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 50;

  daqcard = 14; // module address 0x12
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 43;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 42;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 41;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 40;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 39;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 38;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 49;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 48;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 47;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 46;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 45;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 44;

  daqcard = 15; // module address 0x13
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 145;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 144;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 143;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 142;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 141;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 140;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 151;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 150;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 149;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 148;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 147;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 146;

  daqcard = 16; // module address 0x14
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 139;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 138;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 137;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 136;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 135;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 134;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = 133;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = 123;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 124;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 125;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 126;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 127;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 128;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 129;

  daqcard = 17; // module address 0x15
  m_map[daqcard*CHANNELS_PER_DAQCARD+0] = 117;
  m_map[daqcard*CHANNELS_PER_DAQCARD+1] = 118;
  m_map[daqcard*CHANNELS_PER_DAQCARD+2] = 119;
  m_map[daqcard*CHANNELS_PER_DAQCARD+3] = 120;
  m_map[daqcard*CHANNELS_PER_DAQCARD+4] = 121;
  m_map[daqcard*CHANNELS_PER_DAQCARD+5] = 122;
  m_map[daqcard*CHANNELS_PER_DAQCARD+6] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+7] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+8] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+9] = -1;
  m_map[daqcard*CHANNELS_PER_DAQCARD+10] = 130;
  m_map[daqcard*CHANNELS_PER_DAQCARD+11] = 131;
  m_map[daqcard*CHANNELS_PER_DAQCARD+12] = 132;
  m_map[daqcard*CHANNELS_PER_DAQCARD+13] = 114;
  m_map[daqcard*CHANNELS_PER_DAQCARD+14] = 115;
  m_map[daqcard*CHANNELS_PER_DAQCARD+15] = 116;

  // Fill the good channel array

  bool * m_good_channel_array = new bool[226];
  for ( int i = 0; i < 226; i++ ) {
    m_good_channel_array[i] = true;

    // The following three channels are considered to be bad as of 12/15/10.
    // Channels 178 and 191 are APD gangs which draw excessive leakage
    // current and have been disconnected.
    // Channel 205 is not really connected to the detector
    // because its cable was torn during TPC assembly. The APD gang which
    // would have been channel 205 has been soldered to channel 202.
    // Note: the channels listed are software channels.

    if ( i == 178 || i == 191 || i == 205 ) {
      m_good_channel_array[i] = false;
    }

    //cout << "map[" << i << "] = " << m_map[i] << ". ";

  }

  channelMap.set_map( m_map );
  channelMap.set_good_channel_array ( m_good_channel_array );
  channelMap.set_version( EXOChannelMap::kDefaultChannelMap );
  channelMap.fSuppressedByDaq.clear();
}
