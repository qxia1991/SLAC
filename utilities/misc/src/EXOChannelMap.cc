// 4/4/2011 S.S.
// Moved filling of the channel map to EXOCalibUtilities/EXOChannelMapManager.
// This allows the map to be read from the database if need be.
// What remains in this class is just the structure of the map.

#include "EXOUtilities/EXOChannelMap.hh"
#include "EXOUtilities/EXODimensions.hh"
#include "EXOUtilities/EXOErrorLogger.hh"

#include <sstream>
#include <cstdio>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EXOChannelMap::EXOChannelMap() : 
  map(0), 
  module_address_array(0), 
  card_number_array(0), 
  good_channel_array(0),
  fVersion(kDummyMap)
{
  // create memory for module_address_array (this is the same
  // for all versions of the channel map.)

  module_address_array = new int[NUM_DAQCARDS];

  for ( int i = 0; i < NUM_DAQCARDS; i++ ) {
    module_address_array[i] = (i/NUM_DAQCARDS_PER_SECTION)*(NUM_DAQSLOTS_PER_SECTION)
      +(i%NUM_DAQCARDS_PER_SECTION);
  }

  // create memory for card_number_array (this is also the same
  // for all versions of the channel map.)

  card_number_array = new int[NUM_DAQSLOTS_PER_SECTION*NUM_DAQ_SECTIONS];

  for ( int i = 0; i < NUM_DAQSLOTS_PER_SECTION*NUM_DAQ_SECTIONS; i++ ) {
    card_number_array[i] = -1;
  }

  for ( int i = 0; i < NUM_DAQCARDS; i++ ) {
    card_number_array[module_address_array[i]] = i;
    // Does this work? Isn't the number of module addresses out of range of NUM_DAQCARDS?
  }
  ResetToDummy();

}

EXOChannelMap::~EXOChannelMap()
{
  delete [] map;
  delete [] module_address_array;
  delete [] card_number_array;
  delete [] good_channel_array;
 
}

bool EXOChannelMap::good_channel( int physical_channel ) const 
{
  if ( physical_channel < 0 || physical_channel > 225 ) {
    ostringstream Stream;
    Stream << "physical channel " << physical_channel << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    return false;
  }

  return  good_channel_array[physical_channel];
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool EXOChannelMap::good_channel(  int DAQcard, int cardchannel ) const 
{
  bool result = false;

  if ( DAQcard < 0 || DAQcard > NUM_DAQCARDS-1 ) {
    ostringstream Stream;
    Stream << "DAQcard " << DAQcard << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    return false;
  }

  if ( cardchannel < 0 || cardchannel > CHANNELS_PER_DAQCARD-1 ) {
    ostringstream Stream;
    Stream << "cardchannel " << cardchannel << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    return false;
  }

  int physical_channel = get_physical_channel(DAQcard, cardchannel);

  if ( physical_channel < 0 || physical_channel > 225 ) {
    return false;
  }
  else {
    return good_channel_array[physical_channel];
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int EXOChannelMap::get_physical_channel( int DAQcard, int cardchannel ) const
{
  if ( DAQcard < 0 || DAQcard > NUM_DAQCARDS-1 ) {
    ostringstream Stream;
    Stream << "DAQcard " << DAQcard << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    return -1;
  }

  else if ( cardchannel < 0 || cardchannel > CHANNELS_PER_DAQCARD-1 ) {
    ostringstream Stream;
    Stream << "cardchannel " << cardchannel << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    return -1;
  }

  return map[DAQcard*CHANNELS_PER_DAQCARD + cardchannel];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOChannelMap::get_card_and_channel( int physical_channel, int &card, int &channel ) const
{

  if ( physical_channel < 0 || physical_channel > 225 ) {
    ostringstream Stream;
    Stream << "physical channel " << physical_channel << " unrecognized";
    LogEXOMsg(Stream.str(), EEWarning);
    card = -1;
    channel = -1;
    return;
  }

  for ( int i = 0; i < NUM_DAQCARDS; i++ ) {
    for ( int j = 0; j < CHANNELS_PER_DAQCARD; j++ ) {
      if ( *(map + i*CHANNELS_PER_DAQCARD + j ) == physical_channel ) {
        card = i;
        channel = j;
        return;
      }
    }
  }

  ostringstream Stream;
  Stream << "physical channel " << physical_channel << " not found.";
  LogEXOMsg(Stream.str(), EEWarning);
  card = -1;
  channel = -1;
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int EXOChannelMap::get_module_address( int DAQcard ) const
{
  if ( DAQcard < 0 || DAQcard >= NUM_DAQCARDS ) {
    ostringstream Stream;
    Stream << "DAQcard " << DAQcard << " unrecognized.";
    LogEXOMsg(Stream.str(), EEWarning);
    return -1;
  }

  return module_address_array[DAQcard];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


int EXOChannelMap::get_card_number( int module_address ) const
{
  if ( module_address < 0 || 
       module_address >= NUM_DAQSLOTS_PER_SECTION*NUM_DAQ_SECTIONS ) {
    ostringstream Stream;
    Stream << "module_address = " << module_address << " unrecognized.";
    LogEXOMsg(Stream.str(), EEWarning);
    return -1;
  }

  return card_number_array[module_address];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EXOChannelMap::print_map() const
{

  printf("#################################################################\n");
  printf("DAQ channel map\n");
  printf("Version %d\n", (int)fVersion );
  printf("%d cards, %d channels per card\n\n", NUM_DAQCARDS, CHANNELS_PER_DAQCARD );
  printf("channels 0 - 37: u wires\n");
  printf("channels 38 - 75: v wires\n");
  printf("channels 76 - 113: u wires\n");
  printf("channels 114 - 151: v wires\n");
  printf("channels 152 - 188: APDs\n");
  printf("channels 189 - 225: APDs\n\n");

  for ( int i = 0; i < NUM_DAQCARDS; i++ ) {
    printf("module address = %4d:\t",module_address_array[i]);
    for ( int j = 0; j < CHANNELS_PER_DAQCARD; j++ ) {
      printf("%4d", *(map + i*CHANNELS_PER_DAQCARD + j ));
    }
    printf("\n");
  }

  printf("\n#################################################################\n");
  printf("The following offline channels are bad:\t");
  for ( int i = 0; i < 226; i++ ) {
    if ( good_channel_array[i] == false ) {
      printf("%d\t", i);      
    }
  }
  printf("\n");
  printf("\n#################################################################\n");
  
}

int EXOChannelMap::daq_card_map(int card_address) const
{
  // DAQ cards are not sequentially numbered, so let's map the 
  // actual DAQ card numbers onto the numbers 1 through 18.
  for (int i = 0; i < NUM_DAQCARDS; i++){
    if ( card_address == module_address_array[i] ){
      return i;
    }
  }
  return -1;
}

void EXOChannelMap::set_map( int * in_map )
{
  delete [] map; 
  map = in_map;
}

void EXOChannelMap::set_good_channel_array( bool * in_gca )
{
  delete [] good_channel_array;
  good_channel_array = in_gca;
}

int EXOChannelMap::get_card_number_array (int i) const
{
  return card_number_array[i];
}

void EXOChannelMap::ResetToDummy()
{
  // Create dummy map for version 0. This map is a dummy map, implemented
  // solely as a placeholder.

  // create memory for map array

  delete [] map;
  map = new int[NUM_DAQCARDS*CHANNELS_PER_DAQCARD];

  int result = 0;

  for ( int i = 0; i < NUM_DAQCARDS; i++ ) {
    for ( int j = 0; j < CHANNELS_PER_DAQCARD; j++ ) {

      if ( i >= 0 && i <=5 ) {
	// APD card
        result = 152 + i*16 + j;
	if ( result > 225 ) result = -1;
      }
      else if ( i >= 6 && i <= 8 ) {
	// u wire card
	result = (i-6)*16 + j;
	if ( result > 37 ) result = -1;
      }
      else if ( i >= 9 && i <= 11 ) {
	// u wire card
	result = 76 + (i-9)*16 + j;
	if ( result > 113 ) result = -1;
      }
      else if ( i >= 12 && i <= 14 ) {
	// v wire card
	result = 38 + (i-12)*16 + j;
	if ( result > 75 ) result = -1;
      }
      else if ( i >= 15 && i <= 17 ) {
	// v wire card
	result = 114 + (i-15)*16 + j;
	if ( result > 151 ) result = -1;
      }
      
      *(map + i*CHANNELS_PER_DAQCARD + j ) = result;
      
    }
    
  }
  
  // Fill the good channel array

  delete [] good_channel_array;
  good_channel_array = new bool[226];
  for ( int i = 0; i < 226; i++ ) good_channel_array[i] = true;

  set_version( kDummyMap ); 
  fSuppressedByDaq.clear();
}

bool EXOChannelMap::channel_suppressed_by_daq(int physical_channel) const
{
  // Returns true if the software channel (physical_channel) would otherwise have appeared in the datastream,
  // but was suppressed by the daq.
  // This happened, eg., during April-May 2012.
  return fSuppressedByDaq.find(physical_channel) != fSuppressedByDaq.end();
}
