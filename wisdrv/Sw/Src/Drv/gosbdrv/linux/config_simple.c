/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        config_simple.c
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
#include "config_simple.h"
#include "Board.h"
#include "common.h"

///////////////////////////////////////////////////////////////////////////////
// confiuration dumper
///////////////////////////////////////////////////////////////////////////////

void DumpAudioConfig(AUDIO_CONFIG* audio_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("--------- Audio config -----------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Format          : %d", (UINT32)audio_config->Format));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SampleRate      : %d", (UINT32)audio_config->SampleRate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Channels        : %d", (UINT32)audio_config->Channels));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SampleBits      : %d", (UINT32)audio_config->SampleBits));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("BlockAlign      : %d", audio_config->BlockAlign));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("AvgBytesPerSec  : %d", (UINT32)audio_config->AvgBytesPerSec));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("SamplesPerBlock : %d", audio_config->SamplesPerBlock));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ExtSize         : %d", audio_config->ExtSize));
}

void DumpSystemConfig(TCFGSYSTEM* system_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- system setting -------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",system_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",system_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("tv_standard         : %d",system_config->tv_standard));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("framerate           : %d",(UINT32)system_config->framerate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_h            : %d",(UINT32)system_config->sensor_h));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sensor_v            : %d",(UINT32)system_config->sensor_v));
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
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("compress_mode       : %d",stream_config->compress_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sequence            : %d",stream_config->sequence));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("gop_mode            : %d",stream_config->gop_mode));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("gop_size            : %d",(UINT32)stream_config->gop_size));
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
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("width               : %d",(UINT32)resolution_config->width));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("height              : %d",(UINT32)resolution_config->height));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_sub_window        : %d",resolution_config->h_sub_window));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_sub_window        : %d",resolution_config->v_sub_window));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_sub_offset        : %d",(UINT32)resolution_config->h_sub_offset));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_sub_offset        : %d",(UINT32)resolution_config->v_sub_offset));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("h_scale_enb         : %d",resolution_config->h_scale_enb));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_scale_enb         : %d",resolution_config->v_scale_enb));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("sub_sample          : %d",resolution_config->sub_sample));
}

void DumpBitrateConfig(TCFGBRCTRL* bitrate_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- bitrate setting ------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("name                : %s",bitrate_config->header.name));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("desc                : %s",bitrate_config->header.desc));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("target_bitrate      : %d",(UINT32)bitrate_config->target_bitrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("peak_bitrate        : %d",(UINT32)bitrate_config->peak_bitrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("vbv_buffer          : %d",(UINT32)bitrate_config->vbv_buffer));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("converge_speed      : %d",bitrate_config->converge_speed));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("lambda              : %d",bitrate_config->lambda));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("Q                   : %d",(UINT32)bitrate_config->Q));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("IQ                  : %d",bitrate_config->IQ));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("PQ                  : %d",bitrate_config->PQ));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("BQ                  : %d",bitrate_config->BQ));
}

void DumpFrameRateConfig(TCFGFRAMERATE* framerate_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("-------- framerate setting ------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("frame rate          : %d",(UINT32)framerate_config->frame_rate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("tv standards        : %d",framerate_config->tv_standard));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("drop_frame          : %d",(UINT32)framerate_config->drop_frame));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ivtc_enable         : %d",framerate_config->ivtc_enable));
}

void DumpFormatExtension(TCFG_FORMAT_EXTENSION* extension)
{
	DumpStreamConfig(&extension->_stream);
	DumpResolutionConfig(&extension->_resolution);
	DumpBitrateConfig(&extension->_bitrate);
	DumpFrameRateConfig(&extension->_framerate);
}

void DumpVideoConfig(TCFGVIDEOEX* video_config)
{
	DumpSystemConfig(&video_config->syscfg);
	DumpMiscConfig(&video_config->misccfg);
	DumpStreamConfig(&video_config->strcfg);
	DumpResolutionConfig(&video_config->rescfg);
	DumpBitrateConfig(&video_config->ctlcfg);
	DumpFrameRateConfig(&video_config->fpscfg);
}

void Config_Initialize(PDEVICE_EXTENSION_COMMON pdxc)
{	
	pdxc->config_manager._video_config_valid = 0;
	pdxc->config_manager._audio_config_valid = 0;

	pdxc->BoardInterface->pIBoardVideo->
			GetDefaultSensorParameters(
				pdxc->BoardContext,
				pdxc,
				&(pdxc->config_manager._sensor_config));
		
	pdxc->config_manager._num_of_system_configs = MAX_SYSTEM_CONFIG;
	
	pdxc->BoardInterface->pIBoardVideo->
			GetSensorSettings(
				pdxc->BoardContext,
				pdxc,
				&(pdxc->config_manager._num_of_system_configs),
				pdxc->config_manager._system_configs);
		
	pdxc->config_manager._active_system_config = 0;
	pdxc->config_manager._video_config.syscfg = 
		pdxc->config_manager._system_configs[pdxc->config_manager._active_system_config];	

	DumpSystemConfig(&(pdxc->config_manager._video_config.syscfg));
}

unsigned char Config_IsVideoConfigValid(PCONFIG_MANAGER manager)
{
	return manager->_video_config_valid;
}

unsigned char Config_IsAudioConfigValid(PCONFIG_MANAGER manager)
{
	return manager->_audio_config_valid;
}

TCFGVIDEOEX* Config_GetCurrentVideoConfig(PCONFIG_MANAGER manager) 
{ 
	return &manager->_video_config; 
}

AUDIO_CONFIG* Config_GetCurrentAudioConfig(PCONFIG_MANAGER manager) 
{ 
	return &manager->_audio_config; 
}

SENSOR_PARAMETERS* Config_GetCurrentSensorConfig(PCONFIG_MANAGER manager) 
{ 
	return &manager->_sensor_config; 
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

/*
unsigned __int64 Config_GetVideoAvgTimePerFrame(PCONFIG_MANAGER manager) // in the unit of 100ns
{
	unsigned long framerate = Config_GetVideoTargetFrameRate(manager); 
	return (ULONGLONG)(10010000000) / framerate;
}
*/

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
