#ifndef EXOChannelMap_h
#define EXOChannelMap_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <set>

class EXOChannelMap 
{
  public:

    EXOChannelMap();
    virtual ~EXOChannelMap();
    
    // DAQcard runs from 0 to 17 (for 6 cards per section)
    // module address runs from 0 to 23 (3 crates, 8 slots per crate)
    // card channel runs from 0 to 15 (16 channels per card)
    // physical channel is the offline channel number (should be 0 to 225)
    
    int get_physical_channel( int DAQcard, int cardchannel ) const;
    void get_card_and_channel( int physical_channel, int &card, int &channel ) const;
    int get_module_address( int DAQcard ) const;
    int get_card_number( int module_address ) const;
    bool good_channel( int physical_channel ) const;
    bool good_channel( int DAQcard, int cardchannel ) const;
    int daq_card_map( int card_address) const;
    bool channel_suppressed_by_daq(int physical_channel) const;    
    
    int   get_card_number_array (int i) const;
    
    void print_map() const;
    // Version 2 reads the channel map from the database (3/24/11, SS).
    // Version 1 reflects the channel map as of March 2010.
    // Version 0 is a dummy channel map which was only intended
    // as a temporary placeholder. No other versions are implemented.
    enum EChannelMapType { kDummyMap = 0,
                           kDefaultChannelMap,
                           kDBChannelMap,
                           kNumMaps
                         };

    EChannelMapType get_version() const { return fVersion; }

  protected:
  
    friend class EXOChannelMapManager;
    void set_map( int * in_map );
    void set_version( EChannelMapType val ) { fVersion = val; } 
    void set_good_channel_array( bool *in_gca );
    void ResetToDummy();
    
    int  *map;
    int  *module_address_array;
    int  *card_number_array;
    bool *good_channel_array;
    std::set<int> fSuppressedByDaq; // set of physical channels which were suppressed by the DAQ
    EChannelMapType fVersion;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


