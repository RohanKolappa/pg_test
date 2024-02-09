/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        configuration.c
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


#include "function.h"
#include "platform.h"
#include "videocap.h"
#include "configuration.h"
#include "Board.h"

///////////////////////////////////////////////////////////////////////////////
// confiuration dumper
///////////////////////////////////////////////////////////////////////////////

void DumpVideoConfig(TCFGVIDEOEX* video_config)
{
	DumpSystemConfig(&video_config->syscfg);
	DumpMiscConfig(&video_config->misccfg);
	DumpStreamConfig(&video_config->strcfg);
	DumpResolutionConfig(&video_config->rescfg);
	DumpBitrateConfig(&video_config->ctlcfg);
	DumpFrameRateConfig(&video_config->fpscfg);
}

void DumpFormatExtension(TCFG_FORMAT_EXTENSION* extension)
{
	DumpStreamConfig(&extension->_stream);
	DumpResolutionConfig(&extension->_resolution);
	DumpBitrateConfig(&extension->_bitrate);
	DumpFrameRateConfig(&extension->_framerate);
}

void DumpAudioConfig(AUDIO_CONFIG* audio_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("--------- Audio config -----------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Format          : %d", audio_config->Format));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SampleRate      : %d", audio_config->SampleRate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Channels        : %d", audio_config->Channels));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SampleBits      : %d", audio_config->SampleBits));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("BlockAlign      : %d", audio_config->BlockAlign));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("AvgBytesPerSec  : %d", audio_config->AvgBytesPerSec));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SamplesPerBlock : %d", audio_config->SamplesPerBlock));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ExtSize         : %d", audio_config->ExtSize));
}

void DumpSystemConfig(TCFGSYSTEM* system_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- system setting -------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",system_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",system_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("tv_standard         : %d",system_config->tv_standard));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("framerate           : %d",system_config->framerate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_h            : %d",system_config->sensor_h));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_v            : %d",system_config->sensor_v));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("format              : %d",system_config->format));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("pformat             : %d",system_config->pformat));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_656_mode     : %d",system_config->sensor_656_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("valid_enable        : %d",system_config->valid_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("valid_polar         : %d",system_config->valid_polar));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("href_polar          : %d",system_config->href_polar));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("vref_polar          : %d",system_config->vref_polar));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("field_id_polar      : %d",system_config->field_id_polar));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_bit_width    : %d",system_config->sensor_bit_width));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("hv_resync_enable    : %d",system_config->hv_resync_enable));
}

void DumpStreamConfig(TCFGSTREAM*	stream_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- stream setting -------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",stream_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",stream_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("flags		        : %d",stream_config->header.flags));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("compress_mode       : %d",stream_config->compress_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sequence            : %d",stream_config->sequence));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("gop_mode            : %d",stream_config->gop_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("gop_size            : %d",stream_config->gop_size));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("deinterlace_mode    : %d",stream_config->deinterlace_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("search_range        : %d",stream_config->search_range));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("gop_head_enable     : %d",stream_config->gop_head_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("seq_head_enable     : %d",stream_config->seq_head_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("aspect_ratio        : %d",stream_config->aspect_ratio));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DVD_compliant       : %d",stream_config->DVD_compliant));
}

void DumpMiscConfig(TCFGMISC* misc_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- misc setting -------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("iip_enable          : %d",misc_config->iip_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("vbi_enable          : %d",misc_config->vbi_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("four_channel_enable : %d",misc_config->four_channel_enable));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_filter_mode       : %d",misc_config->h_filter_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_filter_mode       : %d",misc_config->v_filter_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nAX          : %d",misc_config->filter_nAX));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nBX          : %d",misc_config->filter_nBX));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nCX          : %d",misc_config->filter_nCX));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nAY          : %d",misc_config->filter_nAY));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nBY          : %d",misc_config->filter_nBY));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("filter_nCY          : %d",misc_config->filter_nCY));
}

void DumpResolutionConfig(TCFGRESOLUTION* resolution_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- resolution setting ---"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",resolution_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",resolution_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("tv standards        : %d",resolution_config->tv_standard));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("width               : %d",resolution_config->width));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("height              : %d",resolution_config->height));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_sub_window        : %d",resolution_config->h_sub_window));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_sub_window        : %d",resolution_config->v_sub_window));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_sub_offset        : %d",resolution_config->h_sub_offset));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_sub_offset        : %d",resolution_config->v_sub_offset));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_scale_enb         : %d",resolution_config->h_scale_enb));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_scale_enb         : %d",resolution_config->v_scale_enb));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sub_sample          : %d",resolution_config->sub_sample));
}

void DumpBitrateConfig(TCFGBRCTRL* bitrate_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- bitrate setting ------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",bitrate_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",bitrate_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("target_bitrate      : %d",bitrate_config->target_bitrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("peak_bitrate        : %d",bitrate_config->peak_bitrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("vbv_buffer          : %d",bitrate_config->vbv_buffer));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("converge_speed      : %d",bitrate_config->converge_speed));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("lambda              : %d",bitrate_config->lambda));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Q                   : %d",bitrate_config->Q));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("IQ                  : %d",bitrate_config->IQ));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("PQ                  : %d",bitrate_config->PQ));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("BQ                  : %d",bitrate_config->BQ));
}

void DumpFrameRateConfig(TCFGFRAMERATE* framerate_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- framerate setting ------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("frame rate          : %d",framerate_config->frame_rate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("tv standards        : %d",framerate_config->tv_standard));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("drop_frame          : %d",framerate_config->drop_frame));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ivtc_enable         : %d",framerate_config->ivtc_enable));
}

///////////////////////////////////////////////////////////////////////////////
// public interface
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_Initialize(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface)
{
DWORD	num_of_streams;
	enum CONFIG_ERROR error = ERROR_NONE;
	_VIDEO_CAPABILITIES* video_caps = &manager->_video_capabilities;
	_AUDIO_CAPABILITIES* audio_caps = &manager->_audio_capabilities;
	num_of_streams	=	manager->_dwCompressionGroup; // This value is copied because the structure is filled with zero below

	PIFillMem(manager, sizeof(CONFIG_MANAGER), 0);

	manager->_dwCompressionGroup = num_of_streams;
	error = Config_LoadVideoCapabilities(manager, BoardInterface);
	if ( error != ERROR_NONE ) return error;

	error = Config_LoadAudioCapabilities(manager, BoardInterface);
	if ( error != ERROR_NONE ) return error;

	///////////////////////////////////////////////////////////////////////////////
	// load latest valid configuration
	///////////////////////////////////////////////////////////////////////////////
	error = Config_LoadConfigurations(manager);
	if ( error != ERROR_NONE ) return error;

	///////////////////////////////////////////////////////////////////////////////
	// is the lastest system configuration still valid for current sensor module?
	///////////////////////////////////////////////////////////////////////////////
	error = Config_CheckSystemCapability(manager, &manager->_video_config.syscfg);
	if ( error != ERROR_NONE )
	{
		PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("!!! invalid sensor configuration !!!"));

		BoardInterface->pIBoardVideo->GetDefaultSensorParameters(
			manager->_board_context, 
			manager->_avstream_cookie,
			&manager->_sensor_config);
		manager->_active_system_config = 0;
	}
	else
	{
		// check video configuration
		error = Config_CheckVideoConfigurationsEx(video_caps, &manager->_video_config);
		if ( error != ERROR_NONE ) PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("!!! invalid video configuration %s !!!", config_error_message[error]));
	}

	if ( error != ERROR_NONE )	// either sensor confiuration is not supported or video configuraion is not valid
	{
		///////////////////////////////////////////////////////////////////////////////
		// by default we use the first sensor configuration in the sensor module
		// we will re-enumerate other video settings and check their validity
		///////////////////////////////////////////////////////////////////////////////
		error = Config_SetSystemConfiguration(manager, manager->_active_system_config);
		PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("%s !!!", config_error_message[error]));
		if ( error != ERROR_NONE ) return error;
	}

	DumpVideoConfig(&manager->_video_config);

	///////////////////////////////////////////////////////////////////////////////
	// audio configuration
	///////////////////////////////////////////////////////////////////////////////
	if ( audio_caps->_num_of_audio_configuration == 0 ) // no audio support
	{
		manager->_audio_config_valid = 1;
		PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("!!! no audio support !!!"));
		return ERROR_NONE;
	}

	error = Config_CheckAudioConfigurations(audio_caps, &manager->_audio_config);
	if ( error != ERROR_NONE )
	{
		error = Config_CompleteAudioConfigurations(audio_caps, &manager->_audio_config);
		if ( error = ERROR_NONE ) return error;
		error = Config_CheckAudioConfigurations(audio_caps, &manager->_audio_config);
		if ( error != ERROR_NONE ) PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("!!! invalid audio configuration %s !!!", config_error_message[error]));
	}
	if ( error == ERROR_NONE ) manager->_audio_config_valid = 1;

	DumpAudioConfig(&manager->_audio_config);

	return error;
}

enum CONFIG_ERROR Config_Uninitialize(PCONFIG_MANAGER manager)
{
	enum CONFIG_ERROR error = ERROR_NONE;

	error = Config_SaveConfigurations(manager);
	if ( error != ERROR_NONE ) return error;

	if ( manager->_video_capabilities._configurations ) 
	{
		PIFreeMem(manager->_video_capabilities._configurations);
		manager->_video_capabilities._configurations = NULL;
	}

	return ERROR_NONE;
}

TV_STANDARD	Config_GetTVStandard(PCONFIG_MANAGER manager)
{
	return manager->_current_tv_standard;
}

enum CONFIG_ERROR Config_SetTVStandard(PCONFIG_MANAGER manager, TV_STANDARD tv_standard)
{
	enum CONFIG_ERROR error = ERROR_NONE;
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;
	unsigned long i;

	if ( caps->_num_of_system_configs == 0 ) 
		return ERROR_SENSOR_SETTING;

	for ( i = 0 ; i < caps->_num_of_system_configs ; i ++ )
	{
		if ( IsCompatibleWithCurrentTVStandard(tv_standard, caps->_system_configs[i].tv_standard) )
		{
			error = Config_SetSystemConfiguration(manager, i);
			manager->_current_tv_standard = tv_standard;
			if ( error == ERROR_NONE ) break;
		}
	}

	return error;
}

enum CONFIG_ERROR Config_GetTVStandardCaps(PCONFIG_MANAGER manager, unsigned long* tv_standard_caps)
{
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;
	unsigned long i;

	if ( caps->_num_of_system_configs == 0 || tv_standard_caps == NULL ) 
		return ERROR_SENSOR_SETTING;

	*tv_standard_caps = 0;
	for ( i = 0 ; i < caps->_num_of_system_configs ; i ++ )
		*tv_standard_caps = (*tv_standard_caps) | caps->_system_configs[i].tv_standard;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_SetSystemConfiguration(PCONFIG_MANAGER manager, unsigned long index)
{
	enum CONFIG_ERROR error = ERROR_NONE;
	TCFGVIDEOEX* video_config = &manager->_video_config;
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;

	unsigned long i;

	if ( index >= caps->_num_of_system_configs ) return ERROR_SENSOR_SETTING;

	///////////////////////////////////////////////////////////////////////////////
	// system settings
	///////////////////////////////////////////////////////////////////////////////
	PICopyMem(&video_config->syscfg, caps->_system_configs + index, sizeof(TCFGSYSTEM));
	manager->_active_system_config = index;
	manager->_video_config_valid = 0;

	if ( caps->_system_configs[index].tv_standard == TVStandard_NTSC_Mask )
		manager->_current_tv_standard = TVStandard_NTSC_M;
	else if ( caps->_system_configs[index].tv_standard == TVStandard_PAL_Mask )
		manager->_current_tv_standard = TVStandard_PAL_D;
	else if ( caps->_system_configs[index].tv_standard == TVStandard_SECAM_Mask )
		manager->_current_tv_standard = TVStandard_SECAM_D;
	else
		manager->_current_tv_standard = TVStandard_None;

	///////////////////////////////////////////////////////////////////////////////
	// fill a default misc configuration
	///////////////////////////////////////////////////////////////////////////////
	video_config->misccfg.header.name[0] = 0;
	video_config->misccfg.header.desc[0] = 0;
	video_config->misccfg.header.flags = 0;
	video_config->misccfg.header.size = sizeof(TCFGMISC);
	Config_SetMiscConfiguration(manager, video_config->misccfg);

	///////////////////////////////////////////////////////////////////////////////
	// try to find a valid video configuration
	///////////////////////////////////////////////////////////////////////////////
	manager->_active_stream_config = -1;
	manager->_active_framerate_config = -1;
	manager->_active_resolution_config = -1;

	error = Config_EnumVideoConfigurationsExx( caps, manager->_active_system_config );
	if ( error != ERROR_NONE ) return error;

	for ( i = 0 ; i < caps->_num_of_configurations ; i ++ )
	{
		unsigned long str_index = caps->_configurations[i].stream_index;
		unsigned long fps_index = caps->_configurations[i].framerate_index;
		unsigned long res_index = caps->_configurations[i].resolution_index;

		TCFGBRCTRL bitrate; 
		PIFillMem(&bitrate, sizeof(TCFGBRCTRL), 0);
		bitrate.header.name[0] = 0;
		bitrate.header.desc[0] = 0;
		bitrate.header.flags = FLAGS_BITRATE_MANDETORY;
		bitrate.target_bitrate = caps->_resolution_configs[res_index].max_bitrate;
		bitrate.Q = 0;

		error = Config_SetVideoConfigurationsEx( manager, 
								&caps->_stream_configs[str_index],
								&caps->_resolution_configs[res_index],
								&caps->_framerate_configs[fps_index],
								&bitrate);

		if ( error == ERROR_NONE ) break;
	}

	return error;
}

enum CONFIG_ERROR Config_SetMiscConfiguration(PCONFIG_MANAGER manager, TCFGMISC misc)
{
	TCFGVIDEOEX* video_config = &manager->_video_config;

	PICopyMem(&video_config->misccfg, &misc, sizeof(TCFGMISC));

	///////////////////////////////////////////////////////////////////////////////////////
	// driver logic need av sync fields enabled
	///////////////////////////////////////////////////////////////////////////////////////
	video_config->misccfg.av_sync_enable = 1;

	///////////////////////////////////////////////////////////////////////////////////////
	// iip default on for RGB Bayer sensors
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->misccfg.header.flags, FLAGS_MISC_IIP_ENABLE ) == 0 )
		video_config->misccfg.iip_enable = ( video_config->syscfg.format == 2 ) ? 1 : 0; 

	if ( IsFlagsSet( video_config->misccfg.header.flags, FLAGS_MISC_VBI_ENABLE ) == 0 )
		video_config->misccfg.vbi_enable = 0;

	if ( IsFlagsSet( video_config->misccfg.header.flags, FLAGS_MISC_FOUR_CHANNEL_ENABLE ) == 0 )
		video_config->misccfg.four_channel_enable = 0;

	if ( IsFlagsSet( video_config->misccfg.header.flags, FLAGS_MISC_FILTER ) == 0 )
	{
		video_config->misccfg.h_filter_mode = GO7007SB_NOFILTER;
		video_config->misccfg.v_filter_mode = GO7007SB_NOFILTER;
		video_config->misccfg.filter_nAX = 0;
		video_config->misccfg.filter_nBX = 0;
		video_config->misccfg.filter_nCX = 0;
		video_config->misccfg.filter_nAY = 0;
		video_config->misccfg.filter_nBY = 0;	
		video_config->misccfg.filter_nCY = 0;	
	}

	return ERROR_NONE;
}

unsigned char Config_IsVideoConfigValid(PCONFIG_MANAGER manager)
{
	return manager->_video_config_valid;
}

unsigned char Config_IsAudioConfigValid(PCONFIG_MANAGER manager)
{
	return manager->_audio_config_valid;
}

enum CONFIG_ERROR Config_SetVideoConfigurationsEx(	
	PCONFIG_MANAGER		manager,
	TCFGSTREAM*			stream,
	TCFGRESOLUTION*		resolution,
	TCFGFRAMERATE*		framerate,
	TCFGBRCTRL*			bitrate)
{
	enum CONFIG_ERROR error = ERROR_NONE;
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;
	unsigned long str_index, res_index, fps_index;
	manager->_temp_video_config = manager->_video_config;

	///////////////////////////////////////////////////////////////////////////////////////
	// check mandetory flags
	///////////////////////////////////////////////////////////////////////////////////////

	if ( IsFlagsSet( stream->header.flags, FLAGS_STREAM_COMPRESS_MODE ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	if ( IsFlagsSet( framerate->header.flags, FLAGS_FRAMERATE_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	if ( IsFlagsSet( resolution->header.flags, FLAGS_RESOLUTION_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	if ( IsFlagsSet( bitrate->header.flags, FLAGS_BITRATE_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	str_index = FindStreamConfiguration(caps, stream);
	res_index = FindResolutionConfiguration(caps, resolution);
	fps_index = FindFrameRateConfiguration(caps, framerate);

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("***************%d %d %d***********",str_index, fps_index, res_index));

	///////////////////////////////////////////////////////////////////////////////////////
	// we must support these resolution and framerate configurations
	///////////////////////////////////////////////////////////////////////////////////////

	if ( str_index == -1 || res_index == -1 || fps_index == -1 ) return ERROR_CONFIG_VIDEO_CAPABILITY;

	resolution->max_bitrate = manager->_video_capabilities._resolution_configs[res_index].max_bitrate;
	resolution->min_bitrate = manager->_video_capabilities._resolution_configs[res_index].min_bitrate;

	if ( bitrate->target_bitrate != 0 )
	{
		if ( bitrate->target_bitrate < resolution->min_bitrate || bitrate->target_bitrate > resolution->max_bitrate )
			return ERROR_CONFIG_VIDEO_CAPABILITY;
	}

	manager->_temp_video_config.strcfg = *stream;
	manager->_temp_video_config.fpscfg = *framerate;
	manager->_temp_video_config.rescfg = *resolution;
	manager->_temp_video_config.ctlcfg = *bitrate;

	// PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("base support ok"));

	///////////////////////////////////////////////////////////////////////////////////////
	// check basic video configurations : mandetory configuration
	///////////////////////////////////////////////////////////////////////////////////////
	error = Config_CheckVideoConfigurations(caps, &manager->_temp_video_config);
	if ( error != ERROR_NONE ) return error;
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("base checking ok"));

	DumpVideoConfig(&manager->_temp_video_config);

	///////////////////////////////////////////////////////////////////////////////////////
	// set full configuration by default value
	///////////////////////////////////////////////////////////////////////////////////////
	error = Config_CompleteVideoConfigurations(caps, &manager->_temp_video_config);
	if ( error != ERROR_NONE ) return error;
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("completion ok"));

	DumpVideoConfig(&manager->_temp_video_config);

	///////////////////////////////////////////////////////////////////////////////////////
	// check full configurations
	///////////////////////////////////////////////////////////////////////////////////////
	error = Config_CheckVideoConfigurationsEx(caps, &manager->_temp_video_config);
	if ( error != ERROR_NONE ) return error;
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("all checking ok"));

	manager->_video_config = manager->_temp_video_config;
	manager->_active_stream_config = str_index;
	manager->_active_framerate_config = fps_index;
	manager->_active_resolution_config = res_index;
	manager->_video_config_valid = 1;

	DumpVideoConfig(&manager->_video_config);

	return ERROR_NONE;
}


///////////////////////////////////////////////////////////////////////////////
// configuration manager private implementation
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_LoadSensorCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface)
{
	manager->_video_capabilities._num_of_system_configs = MAX_SYSTEM_CONFIG;

	BoardInterface->pIBoardVideo->GetSensorSettings(
			manager->_board_context, 
			manager->_avstream_cookie,
			&(manager->_video_capabilities._num_of_system_configs), 
									manager->_video_capabilities._system_configs);

	manager->_active_system_config = 0;

	if( manager->_video_capabilities._num_of_system_configs == 0 )
	{
		PrintMsg(DEBUG_CONFIGURATION|DEBUG_ERROR_LEVEL, ("No valid system setting!"));
		return ERROR_SENSOR_SETTING;
	}

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_LoadVideoCapabilitiesFromFile(PCONFIG_MANAGER manager)
{
	unsigned long length = sizeof(_VIDEO_CAPABILITIES);
	char default_configuration_file[512] = "video_caps.bin";
	
	FormatSysFileName(default_configuration_file);
	if ( ReadFileToBuffer(default_configuration_file, (UINT8 *)(&manager->_video_capabilities), &length) != 0 )
		return ERROR_UNKNOWN;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_LoadVideoCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface)
{
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;

	enum CONFIG_ERROR error = ERROR_NONE;
	
	////////////////////////////////////////////////////////////////////////////////////
	// use the specified video capabilities first ( in the %system_root%/video_caps.bin
	////////////////////////////////////////////////////////////////////////////////////
	/*error = Config_LoadVideoCapabilitiesFromFile(manager);
	if ( error == ERROR_NONE )
	{
		PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("use video caps in file"));
		return Config_LoadSensorCapabilities(manager);
	}
	*/
	////////////////////////////////////////////////////////////////////////////////////
	// use the default specified video capabilities if not available
	////////////////////////////////////////////////////////////////////////////////////
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("use default video caps in code"));
	error = Config_LoadSensorCapabilities(manager, BoardInterface);
	if ( error != ERROR_NONE ) return error;

	if  ( ( num_of_streams > MAX_STREAM_CONFIG )
		|| ( num_of_framerates > MAX_FRAMERATE_CONFIG )
		|| ( num_of_resolutions > MAX_RESOLUTION_CONFIG )
		|| ( num_of_associations > MAX_ASSOCIATION ) )
		return ERROR_CONFIG_TOO_MUCH_ENTRY;
	
	if((manager->_dwCompressionGroup) && (manager->_dwCompressionGroup <= num_of_streams)) { 
 	//change this to a bit map of compression formats supported
 	// and change the logic to compute the num_of_streams
 		//add check for invalid numbers here
  	
  		num_of_streams = (manager->_dwCompressionGroup) + 1; //registry value +1
	}else{ //default 
		 
 		num_of_streams = 4;//MPEG1, MPEG2 , MPEG4-Microsoft and MPEG4-DivX
	}
  


	caps->_num_of_stream_configs = num_of_streams;
	PICopyMem(&caps->_stream_configs[0], GO7007SB_VideoStreams, sizeof(TCFGSTREAM)*num_of_streams);

	caps->_num_of_resolution_configs = num_of_resolutions;
	PICopyMem(&caps->_resolution_configs[0], GO7007SB_VideoResolutions, sizeof(TCFGRESOLUTION)*num_of_resolutions);

	caps->_num_of_framerate_configs = num_of_framerates;
	PICopyMem(&caps->_framerate_configs[0], GO7007SB_VideoFrameRates, sizeof(TCFGFRAMERATE)*num_of_framerates);

	caps->_num_of_associations = num_of_associations;
	PICopyMem(&caps->_associations[0], GO7007SB_VideoAssociations, sizeof(TCFGASSOCIATION)*num_of_associations);

	caps->_num_of_configurations = 0;
	caps->_configurations = NULL;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_LoadAudioCapabilities(PCONFIG_MANAGER manager, BOARD_INTERFACE *BoardInterface)
{
	unsigned long caps = 0, index;
	unsigned long format_index, samplerate_index, samplebits_index, channel_index;
	_AUDIO_CAPABILITIES* audio_caps = &manager->_audio_capabilities;	
	if ( BoardInterface->pIBoardAudio != NULL )	
		caps = BoardInterface->pIBoardAudio->GetAudioCapabilities(
					manager->_board_context, 
					manager->_avstream_cookie);

	index = 0;
	if ( caps & CAP_AUDIO_FORMAT_PCM ) audio_caps->_audio_format_table[index++] = AUDIO_FORMAT_PCM;
    if ( caps & CAP_AUDIO_FORMAT_ADPCM_MS ) audio_caps->_audio_format_table[index++] = AUDIO_FORMAT_ADPCM_MS;
    if ( caps & CAP_AUDIO_FORMAT_ADPCM_IMA ) audio_caps->_audio_format_table[index++] = AUDIO_FORMAT_ADPCM_IMA;
	if ( caps & CAP_AUDIO_FORMAT_MP3 ) audio_caps->_audio_format_table[index++] = AUDIO_FORMAT_MP3;
	audio_caps->_num_of_format_config = index;

	index = 0;
	if ( caps & CAP_AUDIO_SAMPLERATE_48K ) audio_caps->_audio_samplerate_table[index++] = 48000;
	if ( caps & CAP_AUDIO_SAMPLERATE_44100 ) audio_caps->_audio_samplerate_table[index++] = 44100;
	if ( caps & CAP_AUDIO_SAMPLERATE_32K ) audio_caps->_audio_samplerate_table[index++] = 32000;
	if ( caps & CAP_AUDIO_SAMPLERATE_22050 ) audio_caps->_audio_samplerate_table[index++] = 22050;
	if ( caps & CAP_AUDIO_SAMPLERATE_16K ) audio_caps->_audio_samplerate_table[index++] = 16000;
	if ( caps & CAP_AUDIO_SAMPLERATE_11025 ) audio_caps->_audio_samplerate_table[index++] = 11025;
	if ( caps & CAP_AUDIO_SAMPLERATE_8K ) audio_caps->_audio_samplerate_table[index++] = 8000;
	audio_caps->_num_of_samplerate_config = index;

	index = 0;
	if ( caps & CAP_AUDIO_SAMPLE_4BIT ) audio_caps->_audio_samplebits_table[index++] = 4;
	if ( caps & CAP_AUDIO_SAMPLE_8BIT ) audio_caps->_audio_samplebits_table[index++] = 8;
	if ( caps & CAP_AUDIO_SAMPLE_16BIT ) audio_caps->_audio_samplebits_table[index++] = 16;
	audio_caps->_num_of_samplebits_config = index;

	index = 0;
	if ( caps & CAP_AUDIO_CHANNEL_STEREO ) audio_caps->_audio_channel_table[index++] = 2;
	if ( caps & CAP_AUDIO_CHANNEL_MONO ) audio_caps->_audio_channel_table[index++] = 1;
	audio_caps->_num_of_channel_config = index;

	index = 0;
	for ( format_index = 0 ; format_index < audio_caps->_num_of_format_config ; format_index ++ )
	for ( samplerate_index = 0 ; samplerate_index < audio_caps->_num_of_samplerate_config ; samplerate_index ++ )
	for ( samplebits_index = 0 ; samplebits_index < audio_caps->_num_of_samplebits_config ; samplebits_index ++ )
	for ( channel_index = 0 ; channel_index < audio_caps->_num_of_channel_config ; channel_index ++ )
	{
		audio_caps->_audio_configuration[index]._format_index = format_index;
		audio_caps->_audio_configuration[index]._channel_index = channel_index;
		audio_caps->_audio_configuration[index]._samplerate_index = samplerate_index;
		audio_caps->_audio_configuration[index]._samplebits_index = samplebits_index;
		if ( audio_caps->_audio_format_table[format_index] == AUDIO_FORMAT_MP3 )
			audio_caps->_audio_configuration[index]._extension_size = 22;
		else
			audio_caps->_audio_configuration[index]._extension_size = 0;
		audio_caps->_audio_configuration[index]._extension = NULL;
		index ++;
	}
	audio_caps->_num_of_audio_configuration = index;

	return ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////
// serialization
///////////////////////////////////////////////////////////////////////////////////////

typedef struct _DEFAULT_CHIP_CONFIG_
{
	unsigned long		_active_system_config;
	unsigned long		_active_stream_config;
	unsigned long		_active_framerate_config;
	unsigned long		_active_resolution_config;
	enum TV_STANDARD	_current_tv_standard;


	TCFGVIDEOEX			_video_config;
	AUDIO_CONFIG		_audio_config;
	SENSOR_PARAMETERS	_sensor_config;
} DEFAULT_CHIP_CONFIG;

static DEFAULT_CHIP_CONFIG chip_config;

enum CONFIG_ERROR Config_SaveConfigurations(PCONFIG_MANAGER manager)
{
	char default_configuration_file[512] = "current_settings.bin";

	if ( manager->_video_config_valid && manager->_audio_config_valid )
	{
		chip_config._video_config				= manager->_video_config;
		chip_config._audio_config				= manager->_audio_config;
		chip_config._sensor_config				= manager->_sensor_config;
		chip_config._active_system_config		= manager->_active_system_config;
		chip_config._active_stream_config		= manager->_active_stream_config;
		chip_config._active_framerate_config	= manager->_active_framerate_config;
		chip_config._active_resolution_config	= manager->_active_resolution_config;
		chip_config._current_tv_standard		= manager->_current_tv_standard;

		FormatSysFileName(default_configuration_file);
		WriteBufferToFile(default_configuration_file, (UINT8 *)(&chip_config), sizeof(DEFAULT_CHIP_CONFIG));
	}

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_LoadConfigurations(PCONFIG_MANAGER manager)
{
	unsigned long length = sizeof(DEFAULT_CHIP_CONFIG);
	char default_configuration_file[512] = "current_settings.bin";
	
	FormatSysFileName(default_configuration_file);
	if ( ReadFileToBuffer(default_configuration_file, (UINT8 *)(&chip_config), &length) != 0 )
		return ERROR_NONE;	// though file is corrupt, we may contruct one from capabilities

	manager->_video_config = chip_config._video_config;
	manager->_audio_config = chip_config._audio_config;
	manager->_sensor_config = chip_config._sensor_config;
	manager->_active_system_config = chip_config._active_system_config;
	manager->_active_stream_config = chip_config._active_stream_config;
	manager->_active_framerate_config =	chip_config._active_framerate_config;
	manager->_active_resolution_config = chip_config._active_resolution_config;
	manager->_current_tv_standard = chip_config._current_tv_standard;
	manager->_video_config_valid = 1;

	return ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////
// confiuration checker
///////////////////////////////////////////////////////////////////////////////

enum CONFIG_ERROR Config_CheckVideoConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config)
{
	///////////////////////////////////////////////////////////////////////////////////////
	// check mandetory flags
	///////////////////////////////////////////////////////////////////////////////////////

	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_COMPRESS_MODE ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	if ( ( video_config->strcfg.compress_mode == MPEG4 ) 
		&& IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_MPEG4_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;

	// PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("stream config flags ok"));

	if ( IsFlagsSet( video_config->fpscfg.header.flags, FLAGS_FRAMERATE_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;
	// PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("framerate config flags ok"));

	if ( IsFlagsSet( video_config->rescfg.header.flags, FLAGS_RESOLUTION_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;
	// PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("resolution config flags ok"));

	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_MANDETORY ) == 0 )
		return ERROR_CONFIG_INCOMPLETE;
	// PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("bitrate config flags ok"));

	///////////////////////////////////////////////////////////////////////////////////////
	// check configuration
	///////////////////////////////////////////////////////////////////////////////////////

	if ( ( video_config->rescfg.width % 16 ) != 0 || ( video_config->rescfg.height % 16 ) != 0 )
		return ERROR_CONFIG_INVALID_RESOLUTION;

	if ( ( video_config->strcfg.compress_mode == H263 ) && ( video_config->strcfg.sequence == IPB ) )
		return ERROR_CONFIG_SEQUENCE_MODE;

	if ( ( video_config->strcfg.compress_mode == MOTIONJPEG ) && ( video_config->strcfg.sequence != IONLY ) )
		return ERROR_CONFIG_SEQUENCE_MODE;

	if ( ( video_config->strcfg.compress_mode != MPEG2 && video_config->strcfg.compress_mode != MPEG1 ) && ( video_config->strcfg.DVD_compliant != 0 ) )
		return ERROR_CONFIG_DVD_COMPLIANT;

	if ( ( video_config->strcfg.DVD_compliant != 0 ) &&  ( video_config->ctlcfg.target_bitrate <= 0 ) )
			return ERROR_CONFIG_BITRATE_CONTROL;

	if ( ( video_config->strcfg.compress_mode != MPEG4 ) && ( video_config->strcfg.mpeg4_mode != 0 ) )
		return ERROR_CONFIG_MPEG4_FOURCC;

	if ( ( video_config->strcfg.compress_mode == MPEG4 ) && 
		 ( video_config->strcfg.mpeg4_mode > MAX_MPEG4_MODE 
		 || video_config->strcfg.mpeg4_mode < MIN_MPEG4_MODE ) )
		return ERROR_CONFIG_MPEG4_FOURCC;

	if ( video_config->rescfg.tv_standard != video_config->syscfg.tv_standard )
		return ERROR_CONFIG_TV_STANDARD_RES;

	if ( video_config->fpscfg.tv_standard != video_config->syscfg.tv_standard )
		return ERROR_CONFIG_TV_STANDARD_FPS;

	// PrintMsg(1, ("max:%d, min %d, this: %d", video_config->rescfg.max_bitrate, video_config->rescfg.min_bitrate, video_config->ctlcfg.target_bitrate));
	if ( ( video_config->ctlcfg.target_bitrate > 0 ) &&  
		 ( video_config->rescfg.max_bitrate < video_config->ctlcfg.target_bitrate 
		   || video_config->rescfg.min_bitrate > video_config->ctlcfg.target_bitrate  ) )
			return ERROR_CONFIG_BITRATE_CONTROL;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_CheckVideoConfigurationsEx(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config)
{
	if ( video_config->strcfg.aspect_ratio > 3 || video_config->strcfg.aspect_ratio < 1 )
		return ERROR_CONFIG_ASPECT_RATIO;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// ivtc can only be enabled when following is true
	// 1. interlaced source
	// 2. deinterlace mode == 1
	// 3. NTSC SD
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( ( video_config->fpscfg.ivtc_enable != 0 ) &&  ( ( video_config->strcfg.deinterlace_mode != 1 ) 
		  || ( video_config->rescfg.width != 720 || video_config->rescfg.height != 480 ) 
		  || ( ( video_config->syscfg.tv_standard & TVStandard_NTSC_Mask ) == 0 ) 
		  || ( video_config->syscfg.format != 1 ) ) )
		return ERROR_CONFIG_INVERSE_TELECINE;

	if ( ( video_config->strcfg.compress_mode != MPEG2 ) && ( video_config->strcfg.deinterlace_mode == 2 ) )
		return ERROR_CONFIG_INTERLACE_CODING;

	if ( ( video_config->strcfg.sequence == IPB ) && ( ( video_config->strcfg.gop_size % 3 ) != 0 ) )
		return ERROR_CONFIG_GOP_SIZE;

	if ( ( video_config->strcfg.compress_mode == MPEG4 ) && ( video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 ) && ( video_config->strcfg.sequence == IPB ) )
		return ERROR_CONFIG_SEQUENCE_MODE;

	if ( ( video_config->strcfg.compress_mode == MPEG4 ) && ( video_config->strcfg.mpeg4_mode == RCC_MPEG4 ) && ( video_config->strcfg.sequence == IPB ) )
		return ERROR_CONFIG_SEQUENCE_MODE;

	if ( video_config->strcfg.DVD_compliant != 0 && video_config->strcfg.compress_mode == MPEG2 ) 
	{
		if ( ( video_config->fpscfg.ivtc_enable != 0 ) && ( video_config->strcfg.gop_size != 12 ) )
			return ERROR_CONFIG_GOP_SIZE;

		if ( ( video_config->fpscfg.ivtc_enable == 0 ) && ( video_config->strcfg.gop_size != 15 ) )
			return ERROR_CONFIG_GOP_SIZE;
	}

	if ( video_config->syscfg.format == 1 && video_config->strcfg.deinterlace_mode == 0 )
	{
		if ( video_config->fpscfg.drop_frame != 1 ) return ERROR_CONFIG_DROP_FRAME;
	}
	else if ( video_config->fpscfg.drop_frame != 0 ) 
	{
		if ( video_config->syscfg.framerate != video_config->fpscfg.frame_rate * ( video_config->fpscfg.drop_frame + 1 ) )
			return ERROR_CONFIG_DROP_FRAME;
	}

	if ( (UINT32)(video_config->rescfg.width * ( 1 + video_config->rescfg.h_scale_enb )) > (UINT32)(video_config->syscfg.sensor_h / ( 1 + video_config->rescfg.sub_sample )) )
		return ERROR_CONFIG_FILTER_PARAMETERS;

	if ( (UINT32)(video_config->rescfg.height * ( 1 + video_config->rescfg.v_scale_enb )) > (UINT32)(video_config->syscfg.sensor_v / ( 1 + video_config->rescfg.sub_sample )) )
		return ERROR_CONFIG_FILTER_PARAMETERS;

	if ( video_config->strcfg.compress_mode == H263 && video_config->strcfg.search_range != 32 )
		return ERROR_CONFIG_SEARCH_RANGE;

	if ( video_config->strcfg.compress_mode == MPEG4 && video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 )
	{
		if ( video_config->strcfg.search_range != 64 ) return ERROR_CONFIG_SEARCH_RANGE;
		if ( video_config->strcfg.gop_head_enable != 0 || video_config->strcfg.seq_head_enable != 0 ) return ERROR_CONFIG_STREAM_HEADER;
	}

	if ( video_config->strcfg.compress_mode == MPEG4 && video_config->strcfg.mpeg4_mode == RCC_MPEG4 )
	{
		if ( video_config->strcfg.gop_head_enable != 0 ) return ERROR_CONFIG_STREAM_HEADER;
	}

	if ( video_config->strcfg.compress_mode == MPEG4 && video_config->strcfg.mpeg4_mode == DIVX_MPEG4 && video_config->strcfg.gop_mode != GOP_MODE_CLOSE )
		return ERROR_CONFIG_GOP_MODE;

	if ( video_config->strcfg.search_range != 32 &&
		 video_config->strcfg.search_range != 64 &&
		 video_config->strcfg.search_range != 128 )
		return ERROR_CONFIG_SEARCH_RANGE;

	if ( video_config->syscfg.format != 1 && video_config->strcfg.deinterlace_mode == 0 )
		return ERROR_CONFIG_DEINTERLACE_MODE;

	if ( video_config->syscfg.format == 1 && video_config->strcfg.deinterlace_mode == 0 && video_config->fpscfg.drop_frame == 0 )
		return ERROR_CONFIG_DEINTERLACE_MODE;

/*	if ( ( video_config->ctlcfg.target_bitrate == 0 ) && ( video_config->ctlcfg.Q == 0 ) )	// ?? always BRC.
			return ERROR_CONFIG_BITRATE_CONTROL;

	if ( ( video_config->ctlcfg.target_bitrate > 0 ) && ( video_config->ctlcfg.Q != 0 ) )
			return ERROR_CONFIG_BITRATE_CONTROL;
*/
	if ( video_config->ctlcfg.target_bitrate > 0x1000000)
		return ERROR_CONFIG_BITRATE_CONTROL;

	if ( video_config->ctlcfg.peak_bitrate > 0x4000000 )
		return ERROR_CONFIG_BITRATE_CONTROL;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_CheckAudioConfigurations(_AUDIO_CAPABILITIES* caps, AUDIO_CONFIG* audio_config)
{
	unsigned long index;
	for ( index = 0 ; index < caps->_num_of_audio_configuration ; index ++ )
	{
		TACFG_ENTRY* configuration = caps->_audio_configuration + index;
		if ( audio_config->Format != caps->_audio_format_table[configuration->_format_index] ) continue;
		if ( audio_config->SampleRate != caps->_audio_samplerate_table[configuration->_samplerate_index] ) continue;
		if ( audio_config->SampleBits != caps->_audio_samplebits_table[configuration->_samplebits_index] ) continue;
		if ( audio_config->Channels != caps->_audio_channel_table [configuration->_channel_index] ) continue;
		return ERROR_NONE;
	}
	return ERROR_AUDIO_SETTING;
}

enum CONFIG_ERROR Config_CompleteAudioConfigurations(_AUDIO_CAPABILITIES* caps, AUDIO_CONFIG* audio_config)
{
	TACFG_ENTRY* configuration = &caps->_audio_configuration[0];
	if ( caps->_num_of_audio_configuration == 0 ) return ERROR_AUDIO_SETTING;

	audio_config->Format = caps->_audio_format_table[configuration->_format_index];
	audio_config->SampleRate = caps->_audio_samplerate_table[configuration->_samplerate_index];
	audio_config->Channels = caps->_audio_channel_table[configuration->_channel_index];
	audio_config->SampleBits = caps->_audio_samplebits_table[configuration->_samplebits_index];
	//for ADPM SampleBits will be 4 * 2 channels /8 = 1 that is not the right Block Align and AvgBytesPerSec will be wrong too!!!
	audio_config->BlockAlign = (unsigned short)(audio_config->Channels * audio_config->SampleBits / 8);
	audio_config->AvgBytesPerSec = audio_config->SampleRate * audio_config->BlockAlign;
	audio_config->SamplesPerBlock = 0;
	audio_config->ExtSize = 0;

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_CompleteVideoConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config)
{
	enum CONFIG_ERROR error = ERROR_NONE;

	error = Config_CheckVideoConfigurations(caps, video_config);
	if ( error != ERROR_NONE ) return error;

	error = Config_CheckMiscConfigurations(caps, video_config);
	if ( error != ERROR_NONE ) return error;
	
	///////////////////////////////////////////////////////////////////////////////////////
	// only divx mpeg4 the gop mode is 1 (close gop)
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_GOP_MODE ) == 0 )
		video_config->strcfg.gop_mode = ( (video_config->strcfg.compress_mode == MPEG4) && ( video_config->strcfg.mpeg4_mode == DIVX_MPEG4 ) ) ? GOP_MODE_CLOSE : GOP_MODE_OPEN;

	///////////////////////////////////////////////////////////////////////////////////////
	// sequence mode
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_SEQUENCE_MODE ) == 0 )
	{
		switch ( video_config->strcfg.compress_mode )
		{
			case MPEG1:
			case MPEG2:
				video_config->strcfg.sequence = IPB;
				break;
			case MPEG4:
				video_config->strcfg.sequence = ( video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 
												 || video_config->strcfg.mpeg4_mode == RCC_MPEG4 ) ? IPONLY : IPB;
				break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// mpeg4 mode
	///////////////////////////////////////////////////////////////////////////////////////
	if ( video_config->strcfg.compress_mode != MPEG4 ) video_config->strcfg.mpeg4_mode = 0; // just for sure

	///////////////////////////////////////////////////////////////////////////////////////
	// dvd compliant
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_DVD_COMPLIANT ) == 0 )
	{
		if ( video_config->strcfg.compress_mode != MPEG2 )
			video_config->strcfg.DVD_compliant =  0;
		else
			video_config->strcfg.DVD_compliant = (video_config->rescfg.width == 720) ? 1: 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// deinterlace mode
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_DEINTERLACE_MODE ) == 0 )
	{
		video_config->strcfg.deinterlace_mode = 1;

		///////////////////////////////////////////////////////////////////////////////////////
		// trick here
		///////////////////////////////////////////////////////////////////////////////////////
		if ( IsFlagsSet( video_config->fpscfg.header.flags, FLAGS_FRAMERATE_DROP_FRAME ) == 0 )
		{
			if ( video_config->rescfg.height <= (UINT32)(video_config->syscfg.sensor_v / 2) && video_config->syscfg.format == 1 )
			{
				video_config->strcfg.deinterlace_mode = 0;
				video_config->fpscfg.drop_frame = 1;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// search range
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_SEARCH_RANGE ) == 0 )
	{
		if ( video_config->strcfg.compress_mode == MPEG4 && video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 )
			video_config->strcfg.search_range = 64;
		else if ( video_config->strcfg.compress_mode == H263 )
			video_config->strcfg.search_range = 32;
		else
			video_config->strcfg.search_range = 128;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// gop and sequence header 
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_GOPHEAD_ENABLE ) == 0 )
		video_config->strcfg.gop_head_enable =	( video_config->strcfg.compress_mode == MPEG4 
												&& ( video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 
													|| video_config->strcfg.mpeg4_mode == RCC_MPEG4 ) ) ? 0 : 1;
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_SEQHEAD_ENABLE ) == 0 )
		video_config->strcfg.seq_head_enable = ( video_config->strcfg.compress_mode == MPEG4 && video_config->strcfg.mpeg4_mode == MICROSOFT_MPEG4 ) ? 0 : 1;

	///////////////////////////////////////////////////////////////////////////////////////
	// aspect ratio :
	// 1. for DVD compliant MPEG2 , default is 4:3
	// 2. for other stream: default is 1:1
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_ASPECT_RATIO ) == 0 )
	{
		if ( video_config->strcfg.compress_mode == MPEG2 && video_config->strcfg.compress_mode == MPEG1 )
			video_config->strcfg.aspect_ratio = 2;
		else
			video_config->strcfg.aspect_ratio = 1;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// drop frame : 1 if and only if source is interface and deinterlace mode is 0
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->fpscfg.header.flags, FLAGS_FRAMERATE_DROP_FRAME ) == 0 )
	{
		if ( video_config->syscfg.format == 1 && video_config->strcfg.deinterlace_mode == 0 )
			video_config->fpscfg.drop_frame = 1;
		else if ( video_config->fpscfg.frame_rate != 0 )
			video_config->fpscfg.drop_frame = video_config->syscfg.framerate / video_config->fpscfg.frame_rate - 1;
		else 
			video_config->fpscfg.drop_frame = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// ivtc is enabled when the target framerate is 4/5 of source framerate
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->fpscfg.header.flags, FLAGS_FRAMERATE_IVTC_ENABLE ) == 0 )
	{
		unsigned long framerate = video_config->syscfg.framerate;
		video_config->fpscfg.ivtc_enable = ( framerate * 4 / 5 == video_config->fpscfg.frame_rate ) ? 1 : 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// default gop size : depends on ivtc
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->strcfg.header.flags, FLAGS_STREAM_GOP_SIZE ) == 0 )
	{
		if ( video_config->strcfg.compress_mode == MPEG2 && video_config->strcfg.DVD_compliant != 0 )
			video_config->strcfg.gop_size = ( video_config->fpscfg.ivtc_enable != 0 ) ? 12 : 15;
		else
			video_config->strcfg.gop_size = 30;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// resolution setting
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->rescfg.header.flags, FLAGS_RESOLUTION_SUBSAMPLE ) == 0 )
		video_config->rescfg.sub_sample = 0;	// override user input here !!

	video_config->rescfg.h_sub_window = 1;	// override user input here !!
	video_config->rescfg.v_sub_window = 1;	// override user input here !!

	if ( IsFlagsSet( video_config->rescfg.header.flags, FLAGS_RESOLUTION_SCALE_OFFSET ) == 0 )
	{
		video_config->rescfg.h_scale_enb = ( video_config->rescfg.width <= (UINT32)(video_config->syscfg.sensor_h / 2) ) ? 1 : 0;
		video_config->rescfg.v_scale_enb = ( video_config->rescfg.height <= (UINT32)(video_config->syscfg.sensor_v / 2) ) ? 1 : 0;
		video_config->rescfg.h_sub_offset = ( video_config->syscfg.sensor_h - ( 1 + video_config->rescfg.h_scale_enb ) * video_config->rescfg.width ) / 2;
		video_config->rescfg.v_sub_offset = ( video_config->syscfg.sensor_v - ( 1 + video_config->rescfg.v_scale_enb ) * video_config->rescfg.height ) / 2;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// bitrate setting
	///////////////////////////////////////////////////////////////////////////////////////
	if ( video_config->ctlcfg.target_bitrate > 0 ) 
		video_config->ctlcfg.Q = 0;

/*	if ( video_config->ctlcfg.target_bitrate == 0 && video_config->ctlcfg.Q == 0 )
		video_config->ctlcfg.Q = 4;
*/
	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_PEAK ) == 0 )
	{
		if ( video_config->strcfg.compress_mode == MPEG1 && video_config->strcfg.DVD_compliant != 0 )
			video_config->ctlcfg.peak_bitrate = 1151929;	// default for VCD
		else if ( video_config->strcfg.compress_mode == MPEG2 && video_config->strcfg.DVD_compliant != 0 )
			video_config->ctlcfg.peak_bitrate = 9800000;	// default for DVD
		else
			video_config->ctlcfg.peak_bitrate = 0;
	}

	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_VBV_BUFFER ) == 0 )
	{
		if ( video_config->strcfg.compress_mode == MPEG1 && video_config->strcfg.DVD_compliant != 0 )
			video_config->ctlcfg.vbv_buffer = 327680;	// default for VCD
		else if ( video_config->strcfg.compress_mode == MPEG2 && video_config->strcfg.DVD_compliant != 0 )
			video_config->ctlcfg.vbv_buffer = 1835008;	// default for DVD
		else
			video_config->ctlcfg.vbv_buffer = 0;
	}

	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_CONVERGE_SPEED ) == 0 )
		video_config->ctlcfg.converge_speed = ( video_config->strcfg.DVD_compliant != 0 ) ? 100 : 30;

	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_LAMBDA ) == 0 )
		video_config->ctlcfg.lambda = 100;

	if ( IsFlagsSet( video_config->ctlcfg.header.flags, FLAGS_BITRATE_IPBQ ) == 0 )
	{
		video_config->ctlcfg.IQ = 0;
		video_config->ctlcfg.PQ = 0;
		video_config->ctlcfg.BQ = 0;
	}

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_CheckMiscConfigurations(_VIDEO_CAPABILITIES* caps, TCFGVIDEOEX* video_config)
{
	///////////////////////////////////////////////////////////////////////////////////////
	// iip can be enabled only when sensor is RGB bayer mode
	///////////////////////////////////////////////////////////////////////////////////////
	if ( ( video_config->syscfg.format != 2 ) && ( video_config->misccfg.iip_enable != 0 ) )// RGB bayer
		return ERROR_CONFIG_IIP_ENABLE;

	///////////////////////////////////////////////////////////////////////////////////////
	// in low pass filter
	///////////////////////////////////////////////////////////////////////////////////////
	if ( IsFlagsSet( video_config->misccfg.header.flags, FLAGS_MISC_FILTER ) != 0 )
	{
		if ( video_config->misccfg.h_filter_mode == GO7007SB_LOWPASS && 
			video_config->misccfg.filter_nAX + video_config->misccfg.filter_nBX + video_config->misccfg.filter_nCX != 16 )
			return ERROR_CONFIG_FILTER_PARAMETERS;

		if ( video_config->misccfg.v_filter_mode == GO7007SB_LOWPASS && 
			video_config->misccfg.filter_nAY + video_config->misccfg.filter_nBY + video_config->misccfg.filter_nCY != 16 )
			return ERROR_CONFIG_FILTER_PARAMETERS;
	}

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_CheckSystemCapability(PCONFIG_MANAGER manager, TCFGSYSTEM* system)
{
	_VIDEO_CAPABILITIES* caps = &manager->_video_capabilities;
	unsigned long active_config = manager->_active_system_config;

	if ( active_config >= caps->_num_of_system_configs )
		return ERROR_SENSOR_SETTING;

	return PICmpMem(system, caps->_system_configs + active_config, sizeof(TCFGSYSTEM)) ? ERROR_NONE : ERROR_SENSOR_SETTING;
}



///////////////////////////////////////////////////////////////////////////////
// confiuration helper
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// depreciated functions
///////////////////////////////////////////////////////////////////////////////

TCFGVIDEOEX* Config_GetCurrentVideoConfig(PCONFIG_MANAGER manager) 
{ 
	ASSERT(manager->_video_config_valid);
	return &manager->_video_config; 
}

AUDIO_CONFIG* Config_GetCurrentAudioConfig(PCONFIG_MANAGER manager) 
{ 
	ASSERT(manager->_audio_config_valid);
	return &manager->_audio_config; 
}

SENSOR_PARAMETERS* Config_GetCurrentSensorConfig(PCONFIG_MANAGER manager) 
{ 
	return &manager->_sensor_config; 
}

_VIDEO_CAPABILITIES* Config_GetVideoCapabilities(PCONFIG_MANAGER manager)
{
	return &manager->_video_capabilities;
}

///////////////////////////////////////////////////////////////////////////////
// video helper functions
///////////////////////////////////////////////////////////////////////////////

unsigned long Config_GetVideoSourceFrameRate(PCONFIG_MANAGER manager) 
{ 
	return manager->_video_config.syscfg.framerate; 
}

unsigned long Config_GetVideoTargetFrameRate(PCONFIG_MANAGER manager) // multiple 1001
{
	return manager->_video_config.fpscfg.frame_rate; 
}

unsigned __int64 Config_GetVideoAvgTimePerFrame(PCONFIG_MANAGER manager) // in the unit of 100ns
{
	unsigned long framerate = Config_GetVideoTargetFrameRate(manager); 
	return (ULONGLONG)(10010000000) / framerate;
}

EVideoFormat Config_GetVideoCompressMode(PCONFIG_MANAGER manager)
{
	return manager->_video_config.strcfg.compress_mode; 
}

ESequenceMode Config_GetVideoSequenceMode(PCONFIG_MANAGER manager)
{
	return manager->_video_config.strcfg.sequence;
}

unsigned long Config_IsVideoFieldPictureMode(PCONFIG_MANAGER manager)
{
	return ( manager->_video_config.strcfg.deinterlace_mode == 2 ) ? 1 : 0;
}

unsigned long Config_GetVideoTargetBitrate(PCONFIG_MANAGER manager)
{
	return manager->_video_config.ctlcfg.target_bitrate;
}

unsigned long Config_GetVideoMaxFrameSize(PCONFIG_MANAGER manager)
{
	return manager->_video_config.rescfg.width * manager->_video_config.rescfg.height * 3 / 2;
}

unsigned long Config_GetVideoSourceWidth(PCONFIG_MANAGER manager)
{
	return manager->_video_config.syscfg.sensor_h;
}

unsigned long Config_GetVideoSourceHeight(PCONFIG_MANAGER manager)
{
	return manager->_video_config.syscfg.sensor_v;
}

unsigned long Config_GetVideoTargetWidth(PCONFIG_MANAGER manager)
{
	return manager->_video_config.rescfg.width; 
}

unsigned long Config_GetVideoTargetHeight(PCONFIG_MANAGER manager)
{
	return manager->_video_config.rescfg.height;
}

unsigned long Config_GetVideoPixelAspectRatio(PCONFIG_MANAGER manager)
{
	return manager->_video_config.strcfg.aspect_ratio;
}

unsigned long Config_IsVideoDVDCompatibleStream(PCONFIG_MANAGER manager)
{
	return manager->_video_config.strcfg.DVD_compliant;
}

unsigned long Config_IsVideoIVTCEnabled(PCONFIG_MANAGER manager)
{
	return manager->_video_config.fpscfg.ivtc_enable;
}

///////////////////////////////////////////////////////////////////////////////
// audio helper functions
///////////////////////////////////////////////////////////////////////////////

unsigned long Config_GetAudioSampleRate(PCONFIG_MANAGER manager) 
{ 
	return manager->_audio_config.SampleRate; 
}

unsigned long Config_GetAudioSampleBits(PCONFIG_MANAGER manager) 
{
	return manager->_audio_config.SampleBits; 
}

unsigned long Config_GetAudioChannelCount(PCONFIG_MANAGER manager) 
{ 
	return manager->_audio_config.Channels; 
}

unsigned long Config_GetAudioExtSize(PCONFIG_MANAGER manager)
{
	return manager->_audio_config.ExtSize;
}

/****************************** end of configuration.c ***********************/
