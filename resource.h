#define PLUG_MFR "FioreMartin"
#define PLUG_NAME "Otto"

#define PLUG_CLASS_NAME Otto

#define BUNDLE_MFR "FioreMartin"
#define BUNDLE_NAME "Otto"

#define PLUG_ENTRY Otto_Entry
#define PLUG_VIEW_ENTRY Otto_ViewEntry

#define PLUG_ENTRY_STR "Otto_Entry"
#define PLUG_VIEW_ENTRY_STR "Otto_ViewEntry"

#define VIEW_CLASS Otto_View
#define VIEW_CLASS_STR "Otto_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'duw0'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'None'

// ProTools stuff
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'TXN1', 'TXN2'};
#endif
#define PLUG_MFR_PT "FioreMartin\nFioreMartin\nAcme"
#define PLUG_NAME_PT "Otto\nIPTX"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 0

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define BG_ID 101
#define BUTTON_ID  102
#define KNOB_ID 103


// Image resource locations for this plug.

#define BUTTON_FN "resources/img/button.png"
#define KNOB_FN "resources/img/knob.png"
#define BG_FN "resources/img/bg.png"
// GUI default dimensions
#define GUI_WIDTH   429
#define GUI_HEIGHT  301

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL ""
#define MFR_EMAIL ""
#define EFFECT_TYPE_VST3 "Fx"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
