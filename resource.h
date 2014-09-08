#define PLUG_MFR "squeaky-clean-studios"
#define PLUG_NAME "eDrumUtil"

#define PLUG_CLASS_NAME eDrumUtil

#define BUNDLE_MFR "squeaky-clean-studios"
#define BUNDLE_NAME "eDrumUtil"

#define PLUG_ENTRY eDrumUtil_Entry
#define PLUG_VIEW_ENTRY eDrumUtil_ViewEntry

#define PLUG_ENTRY_STR "eDrumUtil_Entry"
#define PLUG_VIEW_ENTRY_STR "eDrumUtil_ViewEntry"

#define VIEW_CLASS eDrumUtil_View
#define VIEW_CLASS_STR "eDrumUtil_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00001000
#define VST3_VER_STR "1.0.0"

#define PLUG_UNIQUE_ID 'eDrm'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'sqky'

// ProTools stuff
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'PSN1', 'PSN2'};
#endif
#define PLUG_MFR_PT "squeaky-clean-studios\nsqueaky-clean-studios\nSqky\n"
#define PLUG_NAME_PT "eDrumUtil\nIPPS"
#define PLUG_TYPE_PT "Effect" // valid options "None" "EQ" "Dynamics" "PitchShift" "Reverb" "Delay" "Modulation" "Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" instrument determined by PLUG _IS _INST
#define PLUG_DOES_AUDIOSUITE 0

#if (defined(AAX_API) || defined(RTAS_API)) 
#define PLUG_CHANNEL_IO "1-1 2-2"
#else
#define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 1

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.

// GUI default dimensions
#define GUI_WIDTH   700
#define GUI_HEIGHT  300

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "squeakycleanstudios.com"
#define MFR_EMAIL "plugins@squeakycleanstudios.com"
#define EFFECT_TYPE_VST3 "Instrument|Synth"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
