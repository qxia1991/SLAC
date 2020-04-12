#ifndef EXOMuonData_hh
#define EXOMuonData_hh

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EXOMuon Data                                                         //
//                                                                      //
// Encapsulation class for Muon Data in EXO.                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef EXOMuonTrack_hh
#include "EXOMuonTrack.hh"
#endif
#ifndef EXOTClonesArray_hh
#include "EXOTClonesArray.hh"
#endif
#include <cstddef> //for size_t

class EXOMuonData : public TObject
{

  public:
   
    //Int_t     fRunNumber;
    //Int_t     fEventNumber;

    Double_t fMonteCarloTheta;
    Double_t fMonteCarloPhi;
   
    // Default constructor
    EXOMuonData(); 
    EXOMuonData( const EXOMuonData& );
    EXOMuonData& operator=( const EXOMuonData& ); 
    virtual ~EXOMuonData(); 
    
    //*********************************************
    const EXOTClonesArray* GetMuonTrackArray() const;
    EXOMuonTrack* GetMuonTrack(size_t i); 
    const EXOMuonTrack* GetMuonTrack(size_t i) const; 
    EXOMuonTrack* GetNewMuonTrack();
    size_t GetNumMuonTracks() const;

    //*********************************************
    Bool_t ContainsMuon() const;
    Bool_t ContainsPossibleMuon() const;


    //*********************************************
    void Remove(EXOMuonTrack* track);

    void ResetForReconstruction();

    virtual void Clear(Option_t* option = "");

    bool operator==(const EXOMuonData& ed) const;

  protected:

    // InitializeArrays is called to properly initialize the 
    // TClonesArrays 
    void InitializeArrays();
    void InitializeMuonTracks();
    EXOTClonesArray* GetMuonTrackArray();

    // Reconstructed u-wire signal info
    TClonesArray     *fMuonTracks;              //->reconstructed muon tracks for this event

  ClassDef(EXOMuonData, 1)
};

inline EXOTClonesArray* EXOMuonData::GetMuonTrackArray() 
{ 
  // Get array of muon tracks. Normally access to the array should be
  // through the access functions GetMuonTrack and  
  // GetNumMuonTracks
  return static_cast<EXOTClonesArray*>(fMuonTracks); 
}

inline const EXOTClonesArray* EXOMuonData::GetMuonTrackArray() const
{ 
  return static_cast<const EXOTClonesArray*>(fMuonTracks); 
}

inline EXOMuonTrack* EXOMuonData::GetMuonTrack(size_t i) 
{ 
  // Get muon track object at index i.
  return static_cast<EXOMuonTrack*>(GetMuonTrackArray()->At(i)); 
}

inline const EXOMuonTrack* EXOMuonData::GetMuonTrack(size_t i) const
{ 
  // Get muon track object at index i.
  return static_cast<const EXOMuonTrack*>(GetMuonTrackArray()->At(i)); 
}

inline EXOMuonTrack* EXOMuonData::GetNewMuonTrack()
{ 
  // Get a new reconstructed muon track.
  return static_cast<EXOMuonTrack*>(
    GetMuonTrackArray()->GetNewOrCleanedObject(GetNumMuonTracks()));
}

inline size_t EXOMuonData::GetNumMuonTracks() const 
{ 
  // Get Number of muon tracks in the event.
  return (fMuonTracks) ? fMuonTracks->GetEntriesFast() : 0;
}

inline void EXOMuonData::Remove(EXOMuonTrack* track)
{
  // Remove muon track from the event
  GetMuonTrackArray()->RemoveAndCompress(track);
}

inline void EXOMuonData::ResetForReconstruction()
{
  // Clears the event data, removing muon tracks.
  //
  // We must call Delete because we need to make sure destructors are called if
  // any objects exist in these arrays because they were likely created in
  // another process and this messes up TRef(Array)s.  This is a no-op in 
  // data that has empty arrays which is the case when EXOEventData is new
  // or has had Clear("C") called on it. 
  GetMuonTrackArray()->Delete();
}
#endif

