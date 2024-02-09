/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        configuration.h
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

#ifndef __CONFIGUATION_H__
#define __CONFIGUATION_H__

#include "Struct.h"
#include "config_error.h"
#include "config_helper.h"

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
	unsigned long		_active_system_config;

	// current video setting
	TCFGVIDEOEX			_video_config;
	unsigned char		_video_config_valid;
	unsigned long		_active_stream_config;
	unsigned long		_active_framerate_config;
	unsigned long		_active_resolution_config;

	// current audio setting
	AUDIO_CONFIG		_audio_config;
	unsigned char		_audio_config_valid;

	// sensor config
	SENSOR_PARAMETERS	_sensor_config;

	// capabilities
	_VIDEO_CAPABILITIES	_video_capabilities;
	_AUDIO_CAPABILITIES	_audio_capabilities;
	
	//specifies which compression formats are included in the driver
	unsigned long		_dwCompressionGroup; 
	////////////////////////////////////////////////
	// private
	////////////////////////////////////////////////

	TCFGVIDEOEX			_temp_video_config;
	AUDIO_CONFIG		_temp_audio_config;

	TV_STANDARD			_current_tv_standard;

	BOARD_CONTEXT 		_board_context;
	AVSTREAM_COOKIE 	_avstream_cookie;

} CONFIG_MANAGER, *PCONFIG_MANAGER;

///////////////////////////////////////////////////////////////////////////////
// configuration manager public interface
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_Initialize(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface);
enum CONFIG_ERROR Config_Uninitialize(PCONFIG_MANAGER manager);

TV_STANDARD		  Config_GetTVStandard(PCONFIG_MANAGER manager);
enum CONFIG_ERROR Config_SetTVStandard(PCONFIG_MANAGER manager, TV_STANDARD tv_standard);
enum CONFIG_ERROR Config_GetTVStandardCaps(PCONFIG_MANAGER manager, unsigned long* tv_standard_caps);

enum CONFIG_ERROR Config_SetSystemConfiguration(PCONFIG_MANAGER manager, unsigned long index);
enum CONFIG_ERROR Config_SetMiscConfiguration(PCONFIG_MANAGER manager, TCFGMISC misc);
enum CONFIG_ERROR Config_SetVideoConfigurationsEx(	
	PCONFIG_MANAGER		manager,
	TCFGSTREAM*			stream,
	TCFGRESOLUTION*		resolution,
	TCFGFRAMERATE*		framerate,
	TCFGBRCTRL*			bitrate);

///////////////////////////////////////////////////////////////////////////////
// configuration manager private implementation
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_LoadVideoCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface);
enum CONFIG_ERROR Config_LoadAudioCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface);
enum CONFIG_ERROR Config_LoadSensorCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface);

enum CONFIG_ERROR Config_SaveConfigurations(PCONFIG_MANAGER manager);
enum CONFIG_ERROR Config_LoadConfigurations(PCONFIG_MANAGER manager);

unsigned char Config_IsVideoConfigValid(PCONFIG_MANAGER manager); 
unsigned char Config_IsAudioConfigValid(PCONFIG_MANAGER manager); 

///////////////////////////////////////////////////////////////////////////////
// confiuration checker
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_CompleteVideoConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config);	// fill all the configuration based on mandetory configurations
enum CONFIG_ERROR Config_CompleteAudioConfigurations(_AUDIO_CAPABILITIES* caps, AUDIO_CONFIG* audio_config);	// fill all the configuration based on mandetory configurations

enum CONFIG_ERROR Config_CheckSystemCapability(PCONFIG_MANAGER manager, TCFGSYSTEM* system);
enum CONFIG_ERROR Config_CheckMiscConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config);
enum CONFIG_ERROR Config_CheckVideoConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config);		// only check mandetory configurations
enum CONFIG_ERROR Config_CheckVideoConfigurationsEx(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config);	// full check all possible configurations
enum CONFIG_ERROR Config_CheckAudioConfigurations(_AUDIO_CAPABILITIES* caps, AUDIO_CONFIG* audio_config);

///////////////////////////////////////////////////////////////////////////////
// confiuration access functions
///////////////////////////////////////////////////////////////////////////////

// not recommended functions
TCFGVIDEOEX* Config_GetCurrentVideoConfig(PCONFIG_MANAGER manager);
AUDIO_CONFIG* Config_GetCurrentAudioConfig(PCONFIG_MANAGER manager); 
SENSOR_PARAMETERS* Config_GetCurrentSensorConfig(PCONFIG_MANAGER manager); 
_VIDEO_CAPABILITIES* Config_GetVideoCapabilities(PCONFIG_MANAGER manager);

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

/****************************** end of configuration.h ***********************/
