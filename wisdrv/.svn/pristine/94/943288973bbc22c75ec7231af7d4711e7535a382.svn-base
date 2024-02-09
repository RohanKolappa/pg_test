/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Board_Twister.c
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
// Twister

#include "typedef.h"
#include "debug.h"
#include "Board.h"
#include "platform.h"

/////////////////////////////////////////////////////////////////////
// common data
/////////////////////////////////////////////////////////////////////

static TCFGSYSTEM SystemCfgs[] = 
{
	{
		{
			"Data Pump",				// name of configuration for system setting
			"Data Pump",				// sensor configuration file name
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_NTSC_Mask,				// tv_standard
		30000,							// framerate, will devided by 1001, for example 30000 represents 29.97
		720,							// sensor_h, sensor valid pixels in horizontal
		480,							// sensor_v, sensor valid pixels in vertical
		0,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		0,								// sensor_656_mode, 1: input format is 656 mode
		0,								// valid_enable
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},
};


/////////////////////////////////////////////////////////////////////
// function interface
/////////////////////////////////////////////////////////////////////

////////////////////
// basic interface

STATIC SINT32 InitBoard(AVSTREAM_COOKIE Cookie,BOARD_CONTEXT* pBoardContext,BOARD_INFO *pBoardInfo)
{
	strcpy(pBoardInfo->Name, "Twister");
	pBoardInfo->RevisionNum = 0x0206;
	pBoardInfo->BusMode = UM_USB_EZUSB;
	pBoardInfo->Capability = BOARD_CAP_VIDEO|BOARD_CAP_AUDIO;

	return SUCCESS;
}
STATIC SINT32 UnloadBoard(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

////////////////////
// video interface

// The maximum number of system setting is in *Num, the value is 16
STATIC SINT32 GetSensorSettings(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 *Num, TCFGSYSTEM *syscfg)
{
	*Num = sizeof(SystemCfgs)/sizeof(SystemCfgs[0]);
	PICopyMem(syscfg, SystemCfgs, (*Num)*sizeof(TCFGSYSTEM));
	return SUCCESS;
}

STATIC VOID GetDefaultSensorParameters(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SENSOR_PARAMETERS* SensorSetting)
{
	SensorSetting->Brightness = 50;
	SensorSetting->BrightnessAuto = 0;
	SensorSetting->Contrast = 50;
	SensorSetting->ContrastAuto = 0;
	SensorSetting->Hue = 0;
	SensorSetting->HueAuto = 0;
	SensorSetting->Saturation = 50;
	SensorSetting->SaturationAuto = 0;
	SensorSetting->Sharpness = 50;
	SensorSetting->SharpnessAuto = 0;
	SensorSetting->Gamma = 50;
	SensorSetting->GammaAuto = 0;
	SensorSetting->WhiteBalance = 50;
	SensorSetting->WhiteBalanceAuto = 0;
	SensorSetting->BacklightComp = 50;
	SensorSetting->BacklightCompAuto = 0;
	SensorSetting->ColorEnable = 0;
}

STATIC UINT32 GetSensorCapabilities(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return 0;
}

STATIC _VIDEO_CAPABILITIES* GetVideoCapabilities(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return NULL;
}

STATIC SINT32 InitializeSensor(
		BOARD_CONTEXT pContext,
		AVSTREAM_COOKIE Cookie, 
		TV_STANDARD TVStandard,
		VIDEO_SOURCE VideoSource,
		SENSOR_PARAMETERS SensorSetting)
{
	return SUCCESS;
}

STATIC SINT32 UninitializeSensor(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

STATIC SINT32 OnStartCapture(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, TCFGVIDEOEX *video_config, SENSOR_PARAMETERS SensorSetting)
{
	return SUCCESS;
}

STATIC SINT32 OnStopCapture(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

// return SUCCESS(==0) or errno(>0)

STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param)
{
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("SetVideoProperty %d %d",Index, value));
	return SUCCESS;
}

STATIC SINT32 GetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, VIDEO_PROPERTY_INDEX Index, UINT32 *value, PVOID param)
{
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("GetVideoProperty %d",Index));
	switch(Index)
	{
		case PROP_VIDEO_MACROVISION:
			*value = 0;
			break;

		case PROP_VIDEO_HLOCK:
			*value = 1;
			break;

		default:
			break;
	}
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("GetVideoProperty return %d",*value));

	return SUCCESS;
}

////////////////////
// audio interface


STATIC SINT32 GetAudioCapabilities(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return 
		CAP_AUDIO_FORMAT_PCM |
//		CAP_AUDIO_FORMAT_ADPCM_MS |
//		CAP_AUDIO_FORMAT_ADPCM_IMA |
//		CAP_AUDIO_FORMAT_ALAW |
//		CAP_AUDIO_FORMAT_ULAW |
        CAP_AUDIO_I2S_MASTER_MODE |
//		CAP_AUDIO_SAMPLERATE_8K |
//		CAP_AUDIO_SAMPLERATE_11025 |
//		CAP_AUDIO_SAMPLERATE_16K |
//		CAP_AUDIO_SAMPLERATE_22050 |
//		CAP_AUDIO_SAMPLERATE_32K |
		CAP_AUDIO_SAMPLERATE_44100 |
		CAP_AUDIO_SAMPLERATE_48K |
		CAP_AUDIO_CHANNEL_MONO |
		CAP_AUDIO_CHANNEL_STEREO |
		CAP_AUDIO_SAMPLE_4BIT |
//		CAP_AUDIO_SAMPLE_8BIT |
		CAP_AUDIO_SAMPLE_16BIT;
}


// convert user set audio config to chip audio chip

STATIC void GetAudioChipConfig(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, TCFGAUDIO *pAudioChipConfig, AUDIO_CONFIG *audio_config)
{
	if(audio_config->SampleRate == 48000)
		pAudioChipConfig->main_divider = 2;
	else if(audio_config->SampleRate == 44100)
		pAudioChipConfig->main_divider = 1;

	if(audio_config->SampleRate == 48000)
		pAudioChipConfig->BCLK_divider = 8;
	else if(audio_config->SampleRate == 44100)
		pAudioChipConfig->BCLK_divider = 4;

	pAudioChipConfig->OKI_mode = 0;
	pAudioChipConfig->OKI_short_frame_mode = 0;
	pAudioChipConfig->clock_generator_enable = 1;
	pAudioChipConfig->I2S_master_mode = 1;
	pAudioChipConfig->I2S_mode = 1;
	pAudioChipConfig->invert_phase_BCLK = 0;
	pAudioChipConfig->OKI_shoftframe_mode = 0;
	pAudioChipConfig->word_length = (UINT16)audio_config->SampleBits;
	pAudioChipConfig->auto_SOF_generation = 1;
	
	if(audio_config->Channels == 1)
		pAudioChipConfig->left_channel_only = 1;
	else
		pAudioChipConfig->left_channel_only = 0;
	
	pAudioChipConfig->higher_8_bit_combine = 0;
	pAudioChipConfig->AC97_enable = 0;
	pAudioChipConfig->signature_bit_selection = 0;
	pAudioChipConfig->buffer_config = 0;
	pAudioChipConfig->sample_mapping_reg1 = 0;
	pAudioChipConfig->sample_mapping_reg2 = 0;
	pAudioChipConfig->sample_mapping_reg3 = 0;
	pAudioChipConfig->sample_mapping_reg4 = 0;
	pAudioChipConfig->sample_mapping_reg5 = 0;
	pAudioChipConfig->sample_mapping_reg6 = 0;
	pAudioChipConfig->sample_mapping_reg7 = 0;

    if ((audio_config->Format != AUDIO_FORMAT_ADPCM_MS) && (audio_config->Format != AUDIO_FORMAT_ADPCM_IMA))
    {
        pAudioChipConfig->adpcm_enable = 0;
        pAudioChipConfig->adpcm_mode = 0;
        pAudioChipConfig->bytes_per_block = 256;
    }
    else
    {
        PrintMsg(DEBUG_ALWAYS_DISPLAY, ("ADPCM enable"));
        pAudioChipConfig->adpcm_enable = 1;
        pAudioChipConfig->adpcm_mode = 0;
        pAudioChipConfig->bytes_per_block = 1010;
        if (audio_config->Format == AUDIO_FORMAT_ADPCM_MS)
            pAudioChipConfig->adpcm_mode = 1;

    }

	pAudioChipConfig->bpredictor = 0;
	pAudioChipConfig->idelta_left = 16;
	pAudioChipConfig->idelta_right = 16;
	pAudioChipConfig->icoef1_left = 256;
	pAudioChipConfig->icoef1_right = 256;
	pAudioChipConfig->icoef2_left = 0;
	pAudioChipConfig->icoef2_right = 0;
}

STATIC SINT32 InitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, AUDIO_CONFIG *pAudioConfig)
{
	return SUCCESS;
}

STATIC SINT32 UninitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

//////////////////////////////////////////////////////

STATIC IBOARD_BASIC IBoardBasic = 
{
	InitBoard,
	UnloadBoard
};

STATIC IBOARD_VIDEO IBoardVideo = 
{
	GetSensorCapabilities,
	GetVideoCapabilities,
	GetSensorSettings,
	GetDefaultSensorParameters,
	InitializeSensor,
	UninitializeSensor, 
	OnStartCapture,
	OnStopCapture,
	SetVideoProperty,
	GetVideoProperty,
};

STATIC IBOARD_AUDIO IBoardAudio = 
{
	GetAudioCapabilities,
	GetAudioChipConfig,
	InitializeAudio,
	UninitializeAudio,
};

BOARD_INTERFACE BI_Twister = 
{
	sizeof(BOARD_INTERFACE),
	0x0eb1,					// Vender ID of WIS Technologies Inc.
	0x7007,					// Product ID of GO7007SB chip
	0x0206,					// revision number of Twister
	&IBoardBasic,
	&IBoardVideo,
	&IBoardAudio,
	NULL,
	NULL
};

/****************************** end of Board_Twister.c ***********************/
