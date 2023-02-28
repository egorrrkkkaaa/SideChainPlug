#define PLUG_MFR "Koshelev"
#define PLUG_NAME "SideChainPlug"

#define PLUG_CLASS_NAME SideChainPlug

#define BUNDLE_MFR "Koshelev"
#define BUNDLE_NAME "SideChainPlug"

#define PLUG_ENTRY IPlugSideChain_Entry
#define PLUG_VIEW_ENTRY IPlugSideChain_ViewEntry

#define PLUG_ENTRY_STR "IPlugSideChain_Entry"
#define PLUG_VIEW_ENTRY_STR "IPlugSideChain_ViewEntry"

#define VIEW_CLASS IPlugSideChain_View
#define VIEW_CLASS_STR "IPlugSideChain_View"

#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"



#define PLUG_UNIQUE_ID 'Ipsc'

#define PLUG_MFR_ID 'Acme'


#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[1] = {'SCN1'};
#endif

#define PLUG_MFR_PT "AcmeInc\nAcmeInc\nAcme"
#define PLUG_NAME_PT "IPlugSideChain\nIPSC"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 0


#ifdef RTAS_API
// RTAS can only have a mono sc input
// at the moment this is required instead of "2-2 3-2"
#define PLUG_CHANNEL_IO "3-2"
#define PLUG_SC_CHANS 1

#else 
#define PLUG_CHANNEL_IO "2-2 4-2"
#define PLUG_SC_CHANS 2
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0


#define BG_ID 100
#define KNOB_ID 101

#define KNOB_FN "resources/img/knob.png"
#define BG_FN "resources/img/back.png"


#define GUI_WIDTH 300
#define GUI_HEIGHT 300

#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx"

