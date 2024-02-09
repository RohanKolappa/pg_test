/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Board_TVTuner.c
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*		Jie Sun:	Feb 05, 2004	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/
// TV Tuner

#include "typedef.h"
#include "debug.h"
#include "Board.h"
#include "platform.h"

/////////////////////////////////////////////////////////////////////
// common data
/////////////////////////////////////////////////////////////////////

#define SAA7113_ADDR_ANALOGCONTROL1		0x02
#define SAA7113_ADDR_LUMINANCECONTROL	0x09
#define SAA7113_ADDR_BRIGHTNESS			0x0a
#define SAA7113_ADDR_CONTRAST			0x0b
#define SAA7113_ADDR_HUE				0x0d
#define SAA7113_ADDR_SATURATION			0x0c

#define SAA7113_ADDR_STATUS_BYTE		0x1f

STATIC TCFGSYSTEM SystemCfgs[] = 
{
	{
		{
			"SAA7113 NTSC | PAL60",	// name of configuration for system setting
			"SAA7113",						// sensor configuration file name
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_NTSC_Mask,				// tv_standard
		30000,							// framerate, will devided by 1001, for example 30000 represents 29.97
		720,							// sensor_h, sensor valid pixels in horizontal
		480,							// sensor_v, sensor valid pixels in vertical
		1,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		1,								// sensor_656_mode, 1: input format is 656 mode
		0,								// valid_enable
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},

	{
		{
			"SAA7113 PAL",					// name of configuration for system setting
			"SAA7113",						// sensor configuration file name
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_PAL_Mask,				// tv_standard
		25025,							// framerate, will devided by 1001, for example 30000 represents 29.97
		720,							// sensor_h, sensor valid pixels in horizontal
		576,							// sensor_v, sensor valid pixels in vertical
		1,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		1,								// sensor_656_mode, 1: input format is 656 mode
		0,								// valid_enable
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},
};


STATIC UINT8 DeviceAddr = 0x4A;
STATIC INIT_REGISTER InitRegs[]=
{
	{ 0x00, 0x20 },
	{ 0x01, 0x08 },
	{ 0x02, /*0xc9*/0xc0 },
	{ 0x03, 0x33 },
	{ 0x04, 0x00 },
	{ 0x05, 0x00 },
	{ 0x06, 0xe9 },
	{ 0x07, 0x0d },
	{ 0x08, 0xd8 },
	{ 0x09, /*0x81*/0x40 },
	{ 0x0a, 0x80 },
	{ 0x0b, 0x47 },
	{ 0x0c, 0x40 },
	{ 0x0d, 0x00 },
	{ 0x0e, 0x01 },
	{ 0x0f, 0x2a },
	{ 0x10, 0x00 },
	{ 0x11, 0x0c },
	{ 0x12, 0xa7 },
	{ 0x13, 0x00 },
	{ 0x14, 0x00 },
	{ 0x15, 0x00 },
	{ 0x16, 0x00 },
	{ 0x17, 0x00 },
	{ 0x18, 0x00 },
	{ 0x19, 0x00 },
	{ 0x1a, 0x00 },
	{ 0x1b, 0x00 },
	{ 0x1c, 0x00 },
	{ 0x1d, 0x00 },
	{ 0x1e, 0x00 },
	{ 0x1f, 0xc8 },
	{ 0x40, 0x00 },
	{ 0x41, 0xff },
	{ 0x42, 0xff },
	{ 0x43, 0xff },
	{ 0x44, 0xff },
	{ 0x45, 0xff },
	{ 0x46, 0xff },
	{ 0x47, 0xff },
	{ 0x48, 0xff },
	{ 0x49, 0xff },
	{ 0x4a, 0xff },
	{ 0x4b, 0xff },
	{ 0x4c, 0xff },
	{ 0x4d, 0xff },
	{ 0x4e, 0xff },
	{ 0x4f, 0xff },
	{ 0x50, 0xff },
	{ 0x51, 0xff },
	{ 0x52, 0xff },
	{ 0x53, 0xff },
	{ 0x54, 0xff },
	{ 0x55, 0xff },
	{ 0x56, 0xff },
	{ 0x57, 0xff },
	{ 0x58, 0x00 },
	{ 0x59, 0x54 },
	{ 0x5a, 0x07 },
	{ 0x5b, 0x83 },
	{ 0x5c, 0x00 },
	{ 0x5d, 0x00 },
	{ 0x5e, 0x00 },
	{ 0x5f, 0x00 },
	{ 0x60, 0x00 },
	{ 0x61, 0x00 },
	{ 0x63, 0x00 },
};

typedef struct
{
	VIDEO_SOURCE	CurVideoSource;		// VS_SVIDEO or VS_COMPOSITE only for this board.
	TV_STANDARD		CurTVStandard;		// NTSC or PAL or SECAM
	TVTunerSettings TunerSettings;
} TVTuner_Context_t;

STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param);

STATIC SINT32 SetVideoSource(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;

	if( value == VS_SVIDEO ) // s-video
	{
		ctx->CurVideoSource = VS_SVIDEO;
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_ANALOGCONTROL1, 0xC9, IM_I2C);
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_LUMINANCECONTROL, 0x80, IM_I2C);
	}
	else if ( value == VS_COMPOSITE || value == VS_TVTUNER ) // composite or tv tuner
	{
		ctx->CurVideoSource = value;
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_ANALOGCONTROL1, 0xC0, IM_I2C);
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_LUMINANCECONTROL, 0x40, IM_I2C);
	}
	else
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Error:Unknown Video Source %d!!!", value));
		return E_ARG;
	}

 	return SUCCESS;
}

STATIC SINT32 SetTVStandard(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TV_STANDARD value)
{
	SINT32 rt = SUCCESS;
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;

	if ( value == TVStandard_Default )
		return SUCCESS;
	else if ( value == TVStandard_None )
		return E_ARG;
	else if ( value & TVStandard_NTSC_Mask )
	{
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x0E, 0x01, IM_I2C);
		if( rt != 0 ) return E_UNKNOWN;
	}
	else if ( value & TVStandard_PAL_Mask )
	{
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x0E, 0x01, IM_I2C);
		if( rt != 0 ) return E_UNKNOWN;
	}
	else if ( value & TVStandard_SECAM_Mask )
	{
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x0E, 0x50, IM_I2C);
		if( rt != 0 ) return E_UNKNOWN;
	}

	ctx->CurTVStandard = value;
 	return SUCCESS;
}

STATIC SINT32 SetBrightness(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 100 || value < 0 ) return E_ARG;

	value1 = value * 255 / 100; 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_BRIGHTNESS, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 SetContrast(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 100 || value < 0 ) return E_ARG;

	value1 = value * 127 / 100; 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_CONTRAST, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 SetHue(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 50 || value < -50 ) return E_ARG;

	if(value>=0)
		value1 = value * 127 / 50; 
	else
		value1 = 255 - (-value * 127 / 50); 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_HUE, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 SetSaturation(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 100 || value < 0 ) return E_ARG;

	value1 = value * 127 / 100; 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_SATURATION, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 GetHLockStatus(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, UINT32* value)
{
	UINT8 ret_value;
	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_STATUS_BYTE, &ret_value, IM_I2C);

	if( rt != SUCCESS )
		return E_UNKNOWN;

	*value = (ret_value & 0x40) ? 0 : 1;
	return SUCCESS;
}

STATIC SINT32 GetMacrovision(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, UINT32* value)
{
	UINT8 ret_value;
	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, DeviceAddr, 8, SAA7113_ADDR_STATUS_BYTE, &ret_value, IM_I2C);

	if(rt != SUCCESS)
		return E_UNKNOWN;

	*value = (ret_value>>1) & 0x0001;	// bit 1
	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// function interface
/////////////////////////////////////////////////////////////////////

////////////////////
// basic interface

STATIC SINT32 InitBoard(AVSTREAM_COOKIE Cookie,BOARD_CONTEXT* pBoardContext,BOARD_INFO *pBoardInfo)
{
	TVTuner_Context_t* ctx = PIAllocMem(sizeof(TVTuner_Context_t));
	if ( ctx == NULL ) return E_MEM;
	*pBoardContext = ctx;
	
    ctx->CurVideoSource = VS_TVTUNER; //default is tv tuner input
	ctx->CurTVStandard = TVStandard_NTSC_M; //default is NTSC_M
	ctx->TunerSettings.TunerMode = TVTUNER_TV;
	ctx->TunerSettings.VideoStandard = 0;
	ctx->TunerSettings.Frequency = 0;
	ctx->TunerSettings.Country = 1;
	ctx->TunerSettings.Channel = 4;
	ctx->TunerSettings.TunerInput = 0;
	ctx->TunerSettings.Busy = 0;
	ctx->TunerSettings.TVAudioMode = TVAUDIO_MODE_MONO|TVAUDIO_MODE_LANG_A;

	strcpy(pBoardInfo->Name, "TV Tuner Board");
	pBoardInfo->RevisionNum = 0x0208;
	pBoardInfo->BusMode = UM_USB_EZUSB;
	pBoardInfo->Capability = BOARD_CAP_VIDEO|BOARD_CAP_AUDIO|BOARD_CAP_XBAR|BOARD_CAP_TVTUNER;

	return SUCCESS;
}

STATIC SINT32 UnloadBoard(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	PIFreeMem(pContext);
	return SUCCESS;
}
////////////////////
// video interface

// The maximum number of system setting is in *Num, the value is 16
STATIC SINT32 GetSensorSettings(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 *Num, TCFGSYSTEM *syscfg)
{
	*Num = sizeof(SystemCfgs)/sizeof(SystemCfgs[0]);
	PICopyMem(syscfg, SystemCfgs, (*Num)*sizeof(TCFGSYSTEM));
	return SUCCESS;
}

STATIC VOID GetDefaultSensorParameters(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SENSOR_PARAMETERS* SensorSetting)
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
	return CAP_VIDEO_BRIGHTNESS |
		   CAP_VIDEO_CONTRAST |	
		   CAP_VIDEO_HUE |
		   CAP_VIDEO_SATURATION |
		   CAP_VIDEO_SOURCE;
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
	SINT32 reg_num = sizeof(InitRegs)/sizeof(InitRegs[0]);
	SINT32 i, rt = SUCCESS;

	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;
	if ( ctx == NULL ) return E_ARG;
	if ( g_pAF == NULL ) return E_UNKNOWN;

	if ( TVStandard == TVStandard_Default )
		TVStandard = ctx->CurTVStandard;
	if ( VideoSource == VS_DEFAULT )
		VideoSource = ctx->CurVideoSource;

	for(i=0;i<reg_num;i++)
	{
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, InitRegs[i].Addr, (UINT8)InitRegs[i].Value, IM_I2C);
		if( rt != 0 ) return E_UNKNOWN;
	}

	rt = SetVideoProperty(pContext,Cookie,PROP_VIDEO_SOURCE, (UINT32) VideoSource, NULL);
	if ( rt != SUCCESS ) return rt;

	rt = SetVideoProperty(pContext,Cookie,PROP_VIDEO_TVSTANDARD, (UINT32) TVStandard, NULL);
	if ( rt != SUCCESS ) return rt;

	SetVideoProperty(pContext,Cookie,PROP_VIDEO_BRIGHTNESS, SensorSetting.Brightness, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_BRIGHTNESS_AUTO, SensorSetting.BrightnessAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_CONTRAST, SensorSetting.Contrast, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_CONTRAST_AUTO, SensorSetting.ContrastAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_HUE, SensorSetting.Hue, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_HUE_AUTO, SensorSetting.HueAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_SATURATION, SensorSetting.Saturation, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_SATURATION_AUTO, SensorSetting.SaturationAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_SHARPNESS, SensorSetting.Sharpness, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_SHARPNESS_AUTO, SensorSetting.SharpnessAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_GAMMA, SensorSetting.Gamma, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_GAMMA_AUTO, SensorSetting.GammaAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_WHITEBALANCE, SensorSetting.WhiteBalance, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_WHITEBALANCE_AUTO, SensorSetting.WhiteBalanceAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_BACKLIGHT_COMPENSATION, SensorSetting.BacklightComp, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_BACKLIGHT_COMPENSATION_AUTO, SensorSetting.BacklightCompAuto, NULL);
	SetVideoProperty(pContext,Cookie,PROP_VIDEO_COLORENABLE, SensorSetting.ColorEnable, NULL);

	return SUCCESS;
}

STATIC SINT32 UninitializeSensor(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

STATIC SINT32 OnStartCapture(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, TCFGVIDEOEX *video_config, SENSOR_PARAMETERS SensorSetting)
{
	if (!video_config)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY, ("OnStartCapture video_config == NULL"));
		return E_ARG;
	}

	if ( video_config->rescfg.v_sub_offset == 0 && video_config->syscfg.tv_standard == TVStandard_NTSC_Mask ) // we can not make this patch for PAL, because the screen will scrolling.
		video_config->rescfg.v_sub_offset = 4;

	return SUCCESS;
}

STATIC SINT32 OnStopCapture(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}

// return SUCCESS(==0) or errno
STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("SetVideoProperty %d %d",Index, value));
	switch(Index)
	{
		case PROP_VIDEO_SOURCE:
			rt = SetVideoSource(pContext,Cookie,value,param);
			break;

		case PROP_VIDEO_BRIGHTNESS:
			rt = SetBrightness(pContext,Cookie,value,param);
			break;

		case PROP_VIDEO_CONTRAST:
			rt = SetContrast(pContext,Cookie,value,param);
			break;

		case PROP_VIDEO_HUE:
			rt = SetHue(pContext,Cookie,value,param);
			break;

		case PROP_VIDEO_SATURATION:
			rt = SetSaturation(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_TVSTANDARD:
			rt = SetTVStandard(pContext, Cookie, value);
			break;

		default:
			return E_SUPPORT;
	}

	return rt;
}

STATIC SINT32 GetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,VIDEO_PROPERTY_INDEX Index, UINT32 *value, PVOID param)
{
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;
	SINT32 rt = SUCCESS;
	*value = 0;

	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("GetVideoProperty %d",Index));

	switch(Index)
	{
		case PROP_VIDEO_MACROVISION:
			rt = GetMacrovision(pContext,Cookie, value);
			break;

		case PROP_VIDEO_HLOCK:
			rt = GetHLockStatus(pContext,Cookie, value);
			break;

		case PROP_VIDEO_TVSTANDARD:
			*value = ctx->CurTVStandard;
			break;

		case PROP_VIDEO_SOURCE:
			*value = ctx->CurVideoSource;
			break;

		default:
			return E_SUPPORT;
	}
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("GetVideoProperty return %d",rt));

	return rt;
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
//		CAP_AUDIO_FORMAT_MP3 |
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

STATIC void GetAudioChipConfig(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TCFGAUDIO *pAudioChipConfig, AUDIO_CONFIG *audio_config)
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
//        pAudioChipConfig->bytes_per_block = 1010;
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

STATIC SINT32 InitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,AUDIO_CONFIG *pAudioConfig)
{
	SINT32 Index[1];
	SINT32 Value[1];

	if(pAudioConfig->SampleRate == 48000)
	{
		Index[0] = 0;
		Value[0] = 1;
		g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);
	}
	else if(pAudioConfig->SampleRate == 44100)
	{
		Index[0] = 0;
		Value[0] = 0;
		g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);
	}

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

//XBAR
STATIC SINT32 GetXBarCaps(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
/*
	return value can be 
		XBAR_MODE_BASIC  or 
		XBAR_MODE_BASIC|XBAR_MODE_MUTE or 
		XBAR_MODE_BASIC|XBAR_MODE_TVTUNER or 
		XBAR_MODE_BASIC|XBAR_MODE_MUTE|XBAR_MODE_TVTUNER or
	if the return value is 0, indicates the borad doesn't support Xbar,like a camera
*/
{
	return XBAR_MODE_BASIC|XBAR_MODE_MUTE|XBAR_MODE_TVTUNER;

}

STATIC SINT32 Get_Current_Video_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
/*
	return the curent video input to route to analog video decoder

*/
{
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;
	if ( ctx->CurVideoSource == VS_SVIDEO )
		return 2; // the third ( 0 based index ) pin in cross bar.
	else if ( ctx->CurVideoSource == VS_COMPOSITE )
		return 1; // the second ( 0 based index ) pin in cross bar.
	else if ( ctx->CurVideoSource == VS_TVTUNER )
		return 0; // the first ( 0 based index ) pin in cross bar.
	else
		return E_UNKNOWN;
}

STATIC SINT32 Get_Current_Audio_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
/*
	return the curent video input to route to analog video decoder

*/
{
	return 3; // the forth ( 0 based index ) pin in cross bar
}

STATIC SINT32 Set_Current_Video_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 videoid)
/*
	videoid indicates which input pin is selected currently 
	reutrn value is 0 indicates OK 
*/
{
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;

	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Set_Current_Video_InputID %d",	videoid));

	if ( videoid == 0 )				/* 0 -- video tuner in */
		SetVideoSource(pContext,Cookie, VS_TVTUNER, NULL);		/* set to tv tuner in */
	else if ( videoid == 1 )		/* 1 -- video composite in */
		SetVideoSource(pContext,Cookie,VS_COMPOSITE, NULL);	    /* set to composite */
	else if( videoid == 2 )			/* video s-video in */
		SetVideoSource(pContext,Cookie,VS_SVIDEO, NULL);          /* set to s-video   */
	else
		return E_ARG;

	return SUCCESS;
}

STATIC SINT32 Set_Current_Audio_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 audioid)
/*
	audioid indicates which input pin is selected currently 
	reutrn value is 0, indicates OK 
*/
{
	// do not care
	return SUCCESS;
}

static IBOARD_XBAR BoardXbarInterface =
{
	GetXBarCaps,
	Get_Current_Video_InputID,
	Get_Current_Audio_InputID,
	Set_Current_Video_InputID,
	Set_Current_Audio_InputID
};


//
//TVTuner  && TVAudio

STATIC SINT32 GetTvtunerFreqcaps(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerMode mode,TVTunerFrequencyCaps* pFreqCaps)
{
	switch(mode)
	{
		case TVTUNER_TV:
			pFreqCaps->min = 55250000L;
			pFreqCaps->max = 997250000L;
			pFreqCaps->TuningGranularity = 62500L;
			pFreqCaps->NumberOfInputs = 1;
			pFreqCaps->SettlingTime = 100;
			break;
		case TVTUNER_FMAUDIO:
			pFreqCaps->min = 88100000L;
			pFreqCaps->max = 107900000L;
			pFreqCaps->TuningGranularity = 200000L;
			pFreqCaps->NumberOfInputs = 1;
			pFreqCaps->SettlingTime = 100;
			break;
		case TVTUNER_AMAUDIO:
			pFreqCaps->min = 540000L;
			pFreqCaps->max = 1700000L;
			pFreqCaps->TuningGranularity = 1000L;
			pFreqCaps->NumberOfInputs = 1;
			pFreqCaps->SettlingTime = 100;
			break;
		default:
			break;
	}
	return 0;
}
STATIC SINT32 GetTvCurrentSettings(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerSettings* pSettings)
{
	TVTuner_Context_t* ctx = (TVTuner_Context_t*) pContext;
	*pSettings = ctx->TunerSettings;
	return 0;
}

STATIC SINT32 SetTvCurrentSettings(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerSettings* pSettings)
{
/*	UINT8 Data[16];
	SINT32 rt;
	int Band;

	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("TunerMode     %d",	pSettings->TunerMode));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("VideoStandard %d",	pSettings->VideoStandard));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Frequency     %d",	pSettings->Frequency));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Country       %d",	pSettings->Country));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Channel       %d",	pSettings->Channel));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("TunerInput    %d",	pSettings->TunerInput));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Busy          %d",	pSettings->Busy));
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("TVAudioMode   %d",	pSettings->TVAudioMode));

	if( pSettings->TunerMode != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.TunerMode =pSettings->TunerMode;
	}
	if( pSettings->VideoStandard != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.VideoStandard =pSettings->VideoStandard;
	}
	if( pSettings->Frequency != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.Frequency =pSettings->Frequency;
	}
	if( pSettings->Country != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.Country =pSettings->Country;
	}
	if( pSettings->Channel != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.Channel =pSettings->Channel;
	}
	if( pSettings->TunerInput != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.TunerInput =pSettings->TunerInput;
	}

	if( pSettings->Busy != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.Busy =pSettings->Busy;
	}
	if( pSettings->TVAudioMode != TVTUNER_NOCHANGE)
	{
		g_TVTuner_Settings.TVAudioMode =pSettings->TVAudioMode;
	}

	Data[0] = 0xce;

	if(pSettings->Frequency >= 55250000 && pSettings->Frequency <= 83250000)
		Band = 1;	// Low band
	else if(pSettings->Frequency >= 175250000 && pSettings->Frequency <= 211250000)
		Band = 2;	// Mid band
	else if(pSettings->Frequency >= 471250000 && pSettings->Frequency <= 801250000)
		Band = 3;	// High band

	if(Band==1)
		Data[1] = 0xa0;
	else if(Band==2)
		Data[1] = 0x90;
	else if(Band==3)
		Data[1] = 0x30;

	Data[2] = (UINT8)(((pSettings->Frequency/10000+4575)*16/100)>>8);
	Data[3] = (UINT8)(((pSettings->Frequency/10000+4575)*16/100) & 0xff);
*/
	/*	we don't care about addr_width and start_reg_addr
		i2c_mode should be 0x8000
	*/
//	rt = g_pAF->I2C_BurstWriteRegister(Cookie, 0xc2, 0, 0, 4, Data, 0x8000);

	return SUCCESS;
}

static IBOARD_TUNER BoardTunerInterface =
{
	GetTvtunerFreqcaps,
	GetTvCurrentSettings,
	SetTvCurrentSettings
};

BOARD_INTERFACE BI_TVTuner = 
{
	sizeof(BOARD_INTERFACE),
	0x0eb1,					// Vender ID of WIS Technologies Inc.
	0x7007,					// Product ID of GO7007SB chip
	0x0208,					// revision number of Matrix II
	&IBoardBasic,
	&IBoardVideo,
	&IBoardAudio,
	&BoardXbarInterface,
	&BoardTunerInterface
};

/****************************** end of Board_TVTuner.c ***********************/
