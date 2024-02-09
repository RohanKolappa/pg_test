/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        config_simple.h
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#ifndef __CONFIG_SIMPLE_H__
#define __CONFIG_SIMPLE_H__

#include "Struct.h"

#include "typedef.h"
#include "debug.h"
#include "Board.h"


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// configuration manager
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	// current sensor setting
	UINT32				_active_system_config;
	UINT32		       _num_of_system_configs;
	TCFGSYSTEM			_system_configs[MAX_SYSTEM_CONFIG];

	// current video setting
	TCFGVIDEOEX			_video_config;
	UINT32				_video_config_valid;

	// current audio setting
	AUDIO_CONFIG		_audio_config;
	UINT32				_audio_config_valid;

	TV_STANDARD			_current_tv_standard;
	// sensor config
	SENSOR_PARAMETERS	_sensor_config;

} CONFIG_MANAGER, *PCONFIG_MANAGER;

unsigned char Config_IsVideoConfigValid(PCONFIG_MANAGER manager); 
unsigned char Config_IsAudioConfigValid(PCONFIG_MANAGER manager); 

TCFGVIDEOEX* Config_GetCurrentVideoConfig(PCONFIG_MANAGER manager);
AUDIO_CONFIG* Config_GetCurrentAudioConfig(PCONFIG_MANAGER manager); 
SENSOR_PARAMETERS* Config_GetCurrentSensorConfig(PCONFIG_MANAGER manager); 

// video 
unsigned long Config_GetVideoSourceFrameRate(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoTargetFrameRate(PCONFIG_MANAGER manager);
UINT64 		  Config_GetVideoAvgTimePerFrame(PCONFIG_MANAGER manager);
EVideoFormat  Config_GetVideoCompressMode(PCONFIG_MANAGER manager);
ESequenceMode Config_GetVideoSequenceMode(PCONFIG_MANAGER manager);
unsigned long Config_IsVideoFieldPictureMode(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoTargetBitrate(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoMaxFrameSize(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoSourceWidth(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoSourceHeight(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoTargetWidth(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoTargetHeight(PCONFIG_MANAGER manager);
unsigned long Config_GetVideoPixelAspectRatio(PCONFIG_MANAGER manager);

unsigned long Config_IsVideoDVDCompatibleStream(PCONFIG_MANAGER manager);
unsigned long Config_IsVideoIVTCEnabled(PCONFIG_MANAGER manager);

// audio
unsigned long Config_GetAudioSampleRate(PCONFIG_MANAGER manager);
unsigned long Config_GetAudioSampleBits(PCONFIG_MANAGER manager);
unsigned long Config_GetAudioChannelCount(PCONFIG_MANAGER manager);
unsigned long Config_GetAudioExtSize(PCONFIG_MANAGER manager);

///////////////////////////////////////////////////////////////////////////////
// confiuration dumper
///////////////////////////////////////////////////////////////////////////////

void DumpVideoConfig(TCFGVIDEOEX* video_config);
void DumpAudioConfig(AUDIO_CONFIG* audio_config);
void DumpSystemConfig(TCFGSYSTEM*	system_config);
void DumpStreamConfig(TCFGSTREAM*	stream_config);
void DumpResolutionConfig(TCFGRESOLUTION* resolution_config);
void DumpBitrateConfig(TCFGBRCTRL* bitrate_config);
void DumpFrameRateConfig(TCFGFRAMERATE* framerate_config);
void DumpMiscConfig(TCFGMISC* misc_config);
void DumpFormatExtension(TCFG_FORMAT_EXTENSION* extension);

#ifdef __cplusplus
}
#endif

#endif

/****************************** end of config_simple.h ***********************/

