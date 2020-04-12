#ifndef EXODimensions_h
#define EXODimensions_h 1

#include "EXOUtilities/SystemOfUnits.hh"
#include <cstddef>


// June 2014: The drift size is now the more fundamental volume.  Define the
// drift size, and wrap the salt and world volumes around it by defining
// additional thickness in each dimension.

// The drift dimensions are approximate, but realistic!
// Width of 394" (~10.01*m) derives from drift layout doc by M. Swift:
//     https://confluence.slac.stanford.edu/pages/viewpage.action?pageId=67503445

// Measurements by D. Auty in 02/2012 indicate salt movement, but are close nonetheless:
//     https://confluence.slac.stanford.edu/display/exo/Position+of+veto+panels

// Drift length is arbitrarily shorter than real 142' (~43.28*m) of Mod 1's
// "block" of the drift, per layout drawing (above).  Additional length should
// not matter.

const double DRIFTSIZEX = (10.0/2)*CLHEP::m;  // drift width  (~9.70*m total, per D. Auty)
const double DRIFTSIZEY = ( 5.5/2)*CLHEP::m;  // drift height (~5.64*m total, per D. Auty; back has crept since measurement, however)
const double DRIFTSIZEZ = (20.0/2)*CLHEP::m;  // drift length (might be able to shorten, without affecting physics)

// Define salt to have 3m thickness on all sides of the drift, except for the
// top, which has 5m.  These salt thicknesses are arbitrary, but the overburden
// will be refined after a study of shower size vs. depth of primary particle
// generation.  Recall that these dimensions are all half-lengths, despite
// confusing (historical) names.

const double SALTSIZEX = (DRIFTSIZEX + 3.*CLHEP::m);   // Old: (16.0/2)*m;
const double SALTSIZEY = (DRIFTSIZEY + 4.*CLHEP::m);   // Old: (13.5/2)*m; 4*m = avg of 5*m above and 3*m below.
const double SALTSIZEZ = (DRIFTSIZEZ + 3.*CLHEP::m);   // Old: (25.0/2)*m;

// The world has been sized for a nominal 3m of empty space on all sides of the
// salt, although this gap is modified by the shift of salt, drift, and clean
// room volumes incurred in keeping the TPC cathode at the world origin, which
// is critical.

const double WORLDSIZEX = (SALTSIZEX + 3.*CLHEP::m);  // Old: (24.0/2)*m;
const double WORLDSIZEY = (SALTSIZEY + 3.*CLHEP::m);  // Old: (17.5/2)*m;
const double WORLDSIZEZ = (SALTSIZEZ + 3.*CLHEP::m);  // Old: (29.0/2)*m;

const double DETECTORXPOSITION =  2086.*CLHEP::cm;

const double OUTERLEADWALLTHICKNESS    = 20.0*CLHEP::cm;
const double OUTERLEADWALLHEIGHT1      = 86.4*CLHEP::cm;
const double OUTERLEADWALLHEIGHT2      = 92.4*CLHEP::cm;
const double OUTERLEADWALLLENGTH       = 108.0*CLHEP::cm;
const double INNER_OUTER_WALL_DISTANCE = 25.4*CLHEP::cm;

//For the following:
//  INNERRAD and OUTERRAD are the respective radii of the cylinder or
//  polyhedra.
//  HEIGHT is the half length of the tube.
//  The wall thickness of the tube is OUTERRAD-INNERRAD and is uniform 
//  throughout the object.

//Dimensions of HFE are defined such that the space between CuCry2 and LXe
// vessel is filled. Dimensions of Xenon are such that they fill the volume within the LXe vessel.

const double LEADTHICKNESSFRONTANDREAR = 25.0*CLHEP::cm;
const double LEADFRONTSHAVEOFF         = 2.4*CLHEP::cm; // acount for shaving and holes

const double LEADTHICKNESSSIDES        = 24.7*CLHEP::cm;
const double LEADOUTERRAD              = 108.101786*CLHEP::cm;
const double LEADINNERRAD              = LEADOUTERRAD - LEADTHICKNESSSIDES;
const double LEADHEIGHT                = 91.23035*CLHEP::cm;
const int LEADSIDES                    = 12;
const double CUCRY1LIPEXTEND           = 4.5*CLHEP::cm;  // how far the lip at outer cryo door extends
const double CUCRY1LIPDEPTH            = 5.3*CLHEP::cm;  // how thick the lip at outer cryo door is
const double LEADLIPINNERRAD           = LEADINNERRAD + CUCRY1LIPEXTEND;

const double AIR_GAP_THICKNESS_FRONT   = 8.4082*CLHEP::cm;  // 
const double AIR_GAP_THICKNESS_REAR    = 0.9525*CLHEP::cm;  // used in the position of the Outer Cryo

const double CUCRY1THICKNESS           = 2.7*CLHEP::cm;
const double CUCRY1OUTERRAD            = 83.101786*CLHEP::cm;
const double CUCRY1INNERRAD            = CUCRY1OUTERRAD - CUCRY1THICKNESS;
const double CUCRY1HEIGHT              = 86.55*CLHEP::cm;
const double CUCRY1LIPOUTERRAD         = 87.601786*CLHEP::cm;

const double CUCRY1DOORRAD            = 58.75*CLHEP::cm;
const double CUCRY1DOORTHICKNESS      = 2.54*CLHEP::cm;
const double CUCRY1HOLERAD            = 52.9*CLHEP::cm;

const double CUCRY1DISKRAD            = 40.0*CLHEP::cm;
const double CUCRY1DISKTHICKNESS      = 2.7*CLHEP::cm;

const double VACUUM_GAP_THICKNESS_FRONT = 10.06*CLHEP::cm; // 
const double VACUUM_GAP_THICKNESS_REAR  = 8.24*CLHEP::cm;  // used in the position of the Inner Cryo

const double CUCRY2THICKNESS           = 2.7*CLHEP::cm;
const double CUCRY2OUTERRAD            = 74.59281*CLHEP::cm;
const double CUCRY2INNERRAD            = CUCRY2OUTERRAD - CUCRY2THICKNESS;
const double CUCRY2HEIGHT              = 74.7*CLHEP::cm;

const double CUCRY2DOORRAD            = 51.5*CLHEP::cm;
const double CUCRY2DOORTHICKNESS      = 2.7*CLHEP::cm;
const double CUCRY2HOLERAD            = 41.8*CLHEP::cm;

// heat exchangers!
const double HEATEXTHICKNESS           = 1.8*CLHEP::cm; // actually 1.3 cm, but have 1 cm overlap!
const double HEATEXHEIGHT              = 66.72*CLHEP::cm; // would be 73.8cm by the CAD, but we reduce it to account for channel volume
const double HEATEXWIDTH               = 16.4*CLHEP::cm; // from CAD
const double HEATEXOFFSET              = CUCRY2OUTERRAD + HEATEXTHICKNESS - 1.0*CLHEP::cm;


const double LXEVESSELOUTERRAD         = 22.875*CLHEP::cm;
const double LXEVESSELTHICKNESS        = 1.37*CLHEP::mm;
const double LXEVESSELINNERRAD         = LXEVESSELOUTERRAD - LXEVESSELTHICKNESS;
const double LXEVESSELHEIGHT           = 19.0*CLHEP::cm;
const double INACTIVE_XENON_RADIUS     = LXEVESSELINNERRAD;
const double INACTIVE_XENON_UPPER_LIMIT= LXEVESSELHEIGHT-LXEVESSELTHICKNESS;
const double INACTIVE_XENON_LOWER_LIMIT= -1.0*INACTIVE_XENON_UPPER_LIMIT;
const double SALTTHICKNESS             = 100.*CLHEP::cm;
const double CATHODE_ANODE_y_DISTANCE= 19.223657*CLHEP::cm;
const double CATHODE_ANODE_x_DISTANCE= 19.84065*CLHEP::cm;
const double CATHODE_ANODE_DISTANCE  = 19.53215*CLHEP::cm; //midway_between_x_y_planes ;
const double CATHODE_RADIUS          = 16.764*CLHEP::cm;
const double CATHODE_RING_LENGTH     = 1.55956*CLHEP::cm; // actually it's 1.956CLHEP::cm but using this number becuase it allows the ring to fit between the teflon and the cathode, in reality teflon is two unconnected sheets, cathode ring juts out a little in the center, beyond the teflon radius. ;
const double CATHODE_RING_THICKNESS  = 0.508*CLHEP::cm/2.0; // doesn't include the two arms which anchor it in place ;

// APD Stuff
const double APDFRAMERADIUS   = 19.303*CLHEP::cm;
const double APDFRAMETHICKNESS = 0.3175*CLHEP::cm/2.0;
const double CATHODE_APDFACE_DISTANCE = 20.44065*CLHEP::cm;
const double APDRADIUS        = 8.*CLHEP::mm;
const double APDHEIGHT        = 0.65 *CLHEP::mm;
const double APD_SPACING      = 2.218182*CLHEP::cm; //changed from 2.2
const int NUMBER_APDS_PER_PLANE = 259;
const unsigned short NUMBER_APD_CHANNELS_PER_PLANE = 37;
const unsigned short NAPDPLANE  = 2;
const double APD_ADC_BASELINE_COUNTS = 1664.0;
const int APD_ADC_FULL_SCALE_ELECTRONS = 3481600;
// Differentiation times
const double CR_TAU_APD_1 = 10.0*CLHEP::microsecond;
const double CR_TAU_APD_2 = 10.0*CLHEP::microsecond;
const double CR_TAU_APD_3 = 300.0*CLHEP::microsecond;
// Integration time
const double RC_TAU_APD_1 = 3.0*CLHEP::microsecond;
const double RC_TAU_APD_2 = 3.0*CLHEP::microsecond;
const double APD_GAIN = 200.0;
const double DEFAULT_APD_RMS_ELECTRONS_NOISE = 2000.0;

//WireSupport = Acrylic; properties
const double WIRESUPPORT_THICKNESS   = (0.60*CLHEP::cm)/2.0;
const double CATHODE_WIRESUPPORT_DISTANCE  = 19.2405678*CLHEP::cm;
const double APDFRAME_WIRESUPPORT_DISTANCE = 3.0*CLHEP::mm;
const double WIRESUPPORT_INNER_RADIUS      = 17.46377*CLHEP::cm;
// WireSupport is much thicker than origionally coded, and kind of steps to thicker
// in the middle up to 2.5 to 3.67cm. I would put it somewhere in the middle and chop
// off corners w/ boolean subtraction
const double WIRESUPPORT_OUTER_RADIUS      = WIRESUPPORT_INNER_RADIUS+3.0*CLHEP::cm;
//const double WIRESUPPORT_OUTER_RADIUS    = 38.7*CLHEP::cm/2.0; //Old way too thin value
const int NWIRESUPPORT = 2;

//Copper Wire Support Frame = Copper
const double WIRESUPPORT_RING_IR = 22.3140*CLHEP::cm;
const double WIRESUPPORT_RING_OR = 22.6185*CLHEP::cm;
const double WIRESUPPORT_RING_DEPTH = 1.905*CLHEP::cm; //Full Length, not half
const double WIRESUPPORT_RING_Z_PLACEMENT = 19.17*CLHEP::cm + WIRESUPPORT_RING_DEPTH/2.;

// The following constants are used to calculate the wire signals
const double WIRE_PITCH = 0.3*CLHEP::cm; 
const int WIRE_GANG_FACTOR = 3;
const unsigned short NCHANNEL_PER_WIREPLANE = 38;
const int NWIRE_PER_PLANE = NCHANNEL_PER_WIREPLANE*WIRE_GANG_FACTOR;
const double CHANNEL_WIDTH = WIRE_PITCH*WIRE_GANG_FACTOR;
const unsigned short NWIREPLANE = 4;
const double WIRE_DIAMETER = 0.0127*CLHEP::cm; //0.005 inches
const double WIREPLANE_RADIUS = NWIRE_PER_PLANE*WIRE_PITCH/2.0;
const double APDPLANE_UPLANE_DISTANCE = 0.6*CLHEP::cm;
const double UPLANE_VPLANE_DISTANCE = 0.6*CLHEP::cm;

//Field rings
const double FIELDRING_OUTER_RADIUS = 18.712*CLHEP::cm; 
const double FIELDRING_INNER_RADIUS = 18.565*CLHEP::cm;
const double FIELDRING_LIP_INNER_RADIUS = 18.509*CLHEP::cm;
const double FIELDRING_LIP_WIDTH = .1143*CLHEP::cm;
const double FIELDRING_WIDTH = 0.9652*CLHEP::cm; 


const double REFLECTORINNERRAD = 18.32356*CLHEP::cm; //not including overlap
const double REFLECTORTHICKNESS = 1.524*CLHEP::mm; //1/16", not including overlap
const double REFLECTOROUTERRAD = REFLECTORINNERRAD + REFLECTORTHICKNESS;
const double REFLECTORLENGTH  = 18.40357*CLHEP::cm; //assumes that the teflon starts at the center (really starts 1.84277*CLHEP::cm from center;

const double RESISTOR_BLOCK_THICKNESS = .483*CLHEP::cm;
const double RESISTOR_BLOCK_WIDTH = 2.54*CLHEP::cm;
const double RESISTOR_BLOCK_LENGTH = 17.907*CLHEP::cm;
//const double RESISTOR_BLOCK_Z_CENTER = CATHODE_ANODE_y_DISTANCE - RESISTOR_BLOCK_LENGTH/2.;
const double RESISTOR_BLOCK_Z_CENTER = 0.48895*CLHEP::cm + RESISTOR_BLOCK_LENGTH/2.;
//const double RESISTOR_BLOCK_X_CENTER = FIELDRING_LIP_INNER_RADIUS - RESISTOR_BLOCK_THICKNESS/2. - .026*CLHEP::cm;
const double RESISTOR_BLOCK_X_CENTER = FIELDRING_OUTER_RADIUS + RESISTOR_BLOCK_THICKNESS/2;

const double ACTIVE_XENON_RADIUS = REFLECTORINNERRAD;

const double SOURCE_TUBE_INNER_RADIUS = .2413*CLHEP::cm;
const double SOURCE_TUBE_OUTER_RADIUS = .3175*CLHEP::cm;
const double SOURCE_CAPSULE_RADIUS = 0.0748*2.54*CLHEP::cm/2.0;
const double SOURCE_CAPSULE_LENGTH = 0.5810*2.54*CLHEP::cm/2.0;
//const double SOURCE_CAPSULE_SOURCE_CENTER = 0.2296*CLHEP::cm;
const double SOURCE_BEAD_RADIUS = 0.07*CLHEP::cm/2.0;


// This sample time is used to calculate the signals before shaping.
const double SAMPLE_TIME_HIGH_BANDWIDTH = 0.05*CLHEP::microsecond;

// This is the ADC sample time, and should be an integer multiple of the
// high bandwidth sample time
const unsigned int BANDWIDTH_FACTOR = 20;
const double SAMPLE_TIME = SAMPLE_TIME_HIGH_BANDWIDTH*BANDWIDTH_FACTOR; 

// Differentiation times
// Should be in order from shortest to longest
const double CR_TAU_WIRE_1 = 10.0*CLHEP::microsecond; 
const double CR_TAU_WIRE_2 = 10.0*CLHEP::microsecond; 
const double CR_TAU_WIRE_3 = 60.0*CLHEP::microsecond;

// Integration times
const double RC_TAU_WIRE_1 = 3.0*CLHEP::microsecond;
const double RC_TAU_WIRE_2 = 3.0*CLHEP::microsecond;

// ADC full scale counts
const int ADC_BITS = 4096;

const double FINE_DRIFT_STEP_TIME = SAMPLE_TIME_HIGH_BANDWIDTH;
const double COARSE_DRIFT_STEP_TIME = SAMPLE_TIME;

// This is the z location where the signal calculation switches from
// coarse stepping to fine stepping.
const double COARSE_FINE_BORDER = 2.0*CLHEP::cm;

const double DRIFT_VELOCITY = 0.28*CLHEP::cm/CLHEP::microsecond;
const double COLLECTION_TIME = 2.940*CLHEP::microsecond; // fCollectionTime(2.940*CLHEP::microsecond),
const double COLLECTION_VELOCITY = 2.25*CLHEP::mm/CLHEP::microsecond; // #/rec/collection_drift_velocity_mm_per_ns SET, the bulk drift velocity is USEd0.00225

const double DEFAULT_WIRE_RMS_ELECTRONS_NOISE = 800.0;
const double UWIRES_ADC_BASELINE_COUNTS = 1664.0;
const double VWIRES_ADC_BASELINE_COUNTS = 1664.0;

// This defines the dimensions of charge pixels, as used by monte carlo to group geant energy deposits.
const double MCCHARGE_PIXEL_SIZE = WIRE_PITCH/20.0;
const double MCCHARGE_PIXEL_TIME = SAMPLE_TIME/2.0;

// This defines t0
const double TRIGGER_TIME = 256.0*CLHEP::microsecond;
const unsigned int TRIGGER_SAMPLE = (unsigned int)(TRIGGER_TIME/SAMPLE_TIME);

// Fiducial cut
const double FIDUCIAL_CUT = 2.0*CLHEP::cm;

// Ionization potential for gammas and betas in LXe @ 3 kV/CLHEP::cm
// from NIMA 302 177 
const double W_VALUE_LXE_EV_PER_ELECTRON = 18.7*CLHEP::eV;
const double ADC_FULL_SCALE_ELECTRONS_WIRE = 300.0*ADC_BITS;
const double ADC_FULL_SCALE_ENERGY_WIRE = ADC_FULL_SCALE_ELECTRONS_WIRE*W_VALUE_LXE_EV_PER_ELECTRON;


// Ionization quench factor for alphas compared to betas and gaCLHEP::mmas
// W_eff for alphas @ 3 kV/CLHEP::cm = 339 CLHEP::eV/electron (NIMA 307 119)
// Quench factor = 18.7/339 = 0.055
const double ALPHA_QUENCH_FACTOR = 0.055; 

// Anticorrelation parameters
const double ANTICORRELATION_PARAMETER = 0.009;
const double W_VALUE_IONIZATION = 15.6*CLHEP::eV;
const double N_EX_RATIO_EXCIMERS_TO_IONS = 0.06;
const double N_EX_RATIO_EXCIMERS_TO_IONS_FOR_IONS = 0.9;

// Parameterization of anticorrelation parameters

// Assume ANTICORRELATION_PARAMETER is field independent and 
// ANTICORRELATION_RADIUS follows the equation:
// radius = 22900(CLHEP::nm)*exp(-0.579*ln(field/(V/CLHEP::cm))) 
const double ANTICORRELATION_RADIUS_CONSTANT = 22900*CLHEP::nm;
const double ANTICORRELATION_RADIUS_EXP_TERM = -0.579;

// Total number of digitized signals
const size_t NUMBER_READOUT_CHANNELS = NCHANNEL_PER_WIREPLANE*NWIREPLANE+NUMBER_APD_CHANNELS_PER_PLANE*NAPDPLANE;

// for Xe-136 double beta decay
const double DOUBLE_BETA_DECAY_Q_VALUE = 2457.8*CLHEP::keV;
// for Xe-134 double beta decay
const double XE_134_Q_VALUE = 830.0*CLHEP::keV;
// for Xe-124 double beta+ decay
const double XE_124_Q_VALUE = 822.0*CLHEP::keV;

const double ELECTRON_MASS = 0.51099892*CLHEP::MeV;

const int NUM_DAQCARDS = 18;
const int CHANNELS_PER_DAQCARD = 16;
const int NUM_DAQSLOTS_PER_SECTION = 8;
const int NUM_DAQ_SECTIONS = 3;
const int NUM_DAQCARDS_PER_SECTION = 6;

const unsigned short NUM_VETO_CHANNELS = 29; /* number of veto channels active; there are 64 channels available,
                                               some used by the glitch counter and some inactive.
                                               The veto channels used are 0, 2, 4, ..., 56.*/

#endif
