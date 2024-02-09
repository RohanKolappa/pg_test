/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Board_XMenII.c
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
// XMenII

#include "typedef.h"
#include "debug.h"
#include "Board.h"
#include "platform.h"

/////////////////////////////////////////////////////////////////////
// common data
/////////////////////////////////////////////////////////////////////

#define IIP_BASE_ADDR			0x4100

#define IIP_SATURATION_ADDR		IIP_BASE_ADDR+0x36
#define IIP_HUE_ADDR			IIP_BASE_ADDR+0x37
#define SPI_BRIGHTNESS_L_ADDR	0x83
#define SPI_BRIGHTNESS_H_ADDR	0x84

static TCFGSYSTEM SystemCfgs[] = 
{
	{
		{
			"Sharp LZ24BP CCD",			// name of configuration for system setting
			"Sharp LZ24BP CCD",		
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_None,				// tv_standard
		30000,							// framerate, will devided by 1001, for example 30000 represents 29.97
		640,							// sensor_h, sensor valid pixels in horizontal
		480,							// sensor_v, sensor valid pixels in vertical
		2,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		0,								// sensor_656_mode, 1: input format is 656 mode
		0,								// valid_enable
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		1,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},
};

typedef struct _INIT_REGISTER_SPI
{
	UINT8  OpCode;
	UINT16 Addr;
	UINT16 Value;
} INIT_REGISTER_SPI;

static INIT_REGISTER_SPI InitRegs[]=
{
	{ 0xe8, 0x00, 0x20 },
	{ 0xe8, 0x00, 0x00 },
	{ 0xd4, 0x00, 0x2b },
	{ 0x08, 0x00, 0x08 },
	{ 0xd7, 0x00, 0x20 },
	{ 0x94, 0x00, 0x20 },
	{ 0xd8, 0x00, 0x20 },
	{ 0x24, 0x00, 0x20 },
	{ 0x64, 0x00, 0x20 },
	{ 0xa4, 0x00, 0x00 },
	{ 0x37, 0x00, 0x00 },
	{ 0xb7, 0x00, 0x24 },
	{ 0x77, 0x00, 0x2c },
	{ 0xf7, 0x00, 0x04 },
	{ 0x0f, 0x00, 0x1c },
	{ 0x8f, 0x00, 0x20 },
	{ 0x4f, 0x00, 0x0c },
	{ 0xcf, 0x00, 0x0c },
	{ 0x2f, 0x00, 0x20 },
	{ 0xaf, 0x00, 0x31 },
	{ 0x6f, 0x00, 0x00 },
	{ 0xef, 0x00, 0x0c },
	{ 0x1f, 0x00, 0x0c },
	{ 0xdf, 0x00, 0x0c },
	{ 0x3f, 0x00, 0x0c },
	{ 0xbf, 0x00, 0x00 },
	{ 0x7f, 0x00, 0x00 },
	{ 0xff, 0x00, 0x00 },
	{ 0x00, 0x02, 0x37 },
	{ 0x80, 0x02, 0x00 },
	{ 0x40, 0x02, 0x00 },
	{ 0xc0, 0x02, 0x24 },
	{ 0x60, 0x02, 0x0c },
	{ 0xe0, 0x02, 0x0c },
	{ 0x10, 0x02, 0x20 },
	{ 0x90, 0x02, 0x00 },
	{ 0x3a, 0x02, 0x00 },
	{ 0xba, 0x02, 0x31 },
	{ 0x7a, 0x02, 0x00 },
	{ 0xfa, 0x02, 0x38 },
	{ 0x06, 0x02, 0x20 },
	{ 0x86, 0x02, 0x0c },
	{ 0x46, 0x02, 0x0c },
	{ 0xc6, 0x02, 0x20 },
	{ 0x31, 0x02, 0x00 },
	{ 0xb1, 0x02, 0x00 },
	{ 0x49, 0x02, 0x0c },
	{ 0xc9, 0x02, 0x00 },
	{ 0x19, 0x02, 0x06 },
	{ 0x99, 0x02, 0x00 },
	{ 0x79, 0x02, 0x06 },
	{ 0xf9, 0x02, 0x00 },
	{ 0xe1, 0x02, 0x00 },
	{ 0x11, 0x02, 0x14 },
	{ 0x71, 0x02, 0x14 },
	{ 0x29, 0x02, 0x14 },
	{ 0x59, 0x02, 0x14 },
	{ 0x91, 0x02, 0x20 },
	{ 0xf1, 0x02, 0x00 },
	{ 0xa9, 0x02, 0x20 },
	{ 0xd9, 0x02, 0x00 },
	{ 0x61, 0x02, 0x00 },
	{ 0x4d, 0x02, 0x00 },
	{ 0xcd, 0x02, 0x00 },
	{ 0x1d, 0x02, 0x0c },
	{ 0x9d, 0x02, 0x00 },
	{ 0x7d, 0x02, 0x00 },
	{ 0xfd, 0x02, 0x00 },
	{ 0x23, 0x02, 0x08 },
	{ 0xa3, 0x02, 0x10 },
	{ 0x75, 0x02, 0x14 },
	{ 0x2d, 0x02, 0x14 },
	{ 0x5d, 0x02, 0x20 },
	{ 0x03, 0x02, 0x20 },
	{ 0xf5, 0x02, 0x20 },
	{ 0xad, 0x02, 0x00 },
	{ 0xdd, 0x02, 0x20 },
	{ 0x83, 0x02, 0x00 },
	{ 0x35, 0x02, 0x00 },
	{ 0x21, 0x02, 0x0c },
	{ 0xa1, 0x02, 0x0c },
	{ 0x55, 0x02, 0x0c },
	{ 0xd5, 0x02, 0x0c },
	{ 0x02, 0x01, 0x00 },
	{ 0x36, 0x02, 0x00 },
	{ 0xb6, 0x02, 0x12 },
	{ 0x76, 0x02, 0x00 },
	{ 0xf6, 0x02, 0x20 },
	{ 0x0e, 0x02, 0x32 },
	{ 0x8e, 0x02, 0x00 },
	{ 0x4e, 0x02, 0x00 },
	{ 0x8a, 0x01, 0x20 },
	{ 0x4a, 0x01, 0x25 },
	{ 0xca, 0x01, 0x38 },
	{ 0x2a, 0x01, 0x39 },
	{ 0xaa, 0x01, 0x04 },
	{ 0x6a, 0x01, 0x00 },
	{ 0xea, 0x01, 0x0e },
	{ 0x1a, 0x01, 0x02 },
	{ 0x9a, 0x01, 0x0a },
	{ 0x5a, 0x01, 0x12 },
	{ 0xa6, 0x01, 0x12 },
	{ 0x66, 0x01, 0x00 },
	{ 0xe6, 0x01, 0x00 },
	{ 0x16, 0x01, 0x00 },
	{ 0x96, 0x01, 0x00 },
	{ 0x56, 0x01, 0x20 },
	{ 0xf6, 0x01, 0x0f },
	{ 0x0e, 0x01, 0x3f },
	{ 0x44, 0x00, 0x20 },
	{ 0xc4, 0x00, 0x20 },
	{ 0xa5, 0x00, 0x00 },
	{ 0x65, 0x00, 0x00 },
	{ 0xe5, 0x00, 0x31 },
	{ 0x15, 0x00, 0x00 },
	{ 0x95, 0x00, 0x35 },
	{ 0x55, 0x00, 0x20 },
	{ 0xbd, 0x00, 0x20 },
	{ 0x7d, 0x00, 0x35 },
	{ 0xfd, 0x00, 0x20 },
	{ 0x03, 0x00, 0x37 },
	{ 0x83, 0x00, 0x20 },
	{ 0x8e, 0x01, 0x20 },
	{ 0x4e, 0x01, 0x04 },
	{ 0xce, 0x01, 0x20 },
	{ 0x2e, 0x01, 0x3e },
	{ 0xae, 0x01, 0x20 },
	{ 0x6e, 0x01, 0x04 },
	{ 0xee, 0x01, 0x20 },
	{ 0x1e, 0x01, 0x3e },
	{ 0x9e, 0x01, 0x20 },
	{ 0x5e, 0x01, 0x28 },
	{ 0xde, 0x01, 0x10 },
	{ 0x60, 0x00, 0x24 },
	{ 0x87, 0x00, 0x20 },
	{ 0x47, 0x00, 0x20 },
	{ 0x17, 0x00, 0x20 },
	{ 0xe0, 0x00, 0x01 },
	{ 0x10, 0x00, 0x03 },
	{ 0x90, 0x00, 0x04 },
	{ 0x50, 0x00, 0x01 },
	{ 0x40, 0x00, 0x03 },
	{ 0xc0, 0x00, 0x18 },
	{ 0x97, 0x00, 0x01 },
	{ 0x57, 0x00, 0x37 },
	{ 0xb1, 0x00, 0x20 },
	{ 0x71, 0x00, 0x14 },
	{ 0xf1, 0x00, 0x00 },
	{ 0x09, 0x00, 0x1e },
	{ 0x89, 0x00, 0x00 },
	{ 0x20, 0x00, 0x00 },
	{ 0xa0, 0x00, 0x10 },
	{ 0x78, 0x00, 0x20 },
	{ 0x18, 0x00, 0x20 },
	{ 0xd8, 0x00, 0x00 },
	{ 0x04, 0x00, 0x00 },
};

STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param);

STATIC SINT32 SetBrightness(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	SINT32 register_value, i, reverse_value = 0;

	if ( value > 100 || value < 0 ) return E_ARG;

	value = 100 - value;
	register_value = (value * 530 / 100) + 1;	// 1 ~ 531
	for (i = 0; i < 12; i++)
	{
		if (register_value & 0x1)
			reverse_value |= 0x1;
		register_value >>= 1;
		reverse_value <<= 1;
	}
	reverse_value >>= 1;

	rt = g_pAF->SPI_WriteRegister(Cookie, 8, InitRegs[SPI_BRIGHTNESS_L_ADDR].OpCode, 2,
		InitRegs[SPI_BRIGHTNESS_L_ADDR].Addr, 6, (reverse_value >> 6) & 0x3F, 275);
	if (rt != 0)
		return rt;

	rt = g_pAF->SPI_WriteRegister(Cookie, 8, InitRegs[SPI_BRIGHTNESS_H_ADDR].OpCode, 2,
		InitRegs[SPI_BRIGHTNESS_H_ADDR].Addr, 6, reverse_value & 0x3F, 275);
	if (rt != 0)
		return rt;

	return rt;
}

STATIC SINT32 SetHue(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	UINT8  register_value = 0;

	if ( value > 50 || value < -50 ) return E_ARG;

	if (value < 0)
	{
		register_value |= 0x20;
		value = -value;
	}

	register_value |= (UINT8) (value * 31 / 50);
	rt = g_pAF->WriteCBusRegFW(Cookie, IIP_HUE_ADDR, register_value);

	return rt;
}

STATIC SINT32	SetSaturation(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	UINT8  register_value = 0;

	if ( value > 100 || value < 0 ) return E_ARG;

	register_value = (UINT8) (value * 127 / 100);
	rt = g_pAF->WriteCBusRegFW(Cookie, IIP_SATURATION_ADDR, register_value);

	return rt;
}

/////////////////////////////////////////////////////////////////////
// function interface
/////////////////////////////////////////////////////////////////////


////////////////////
// basic interface

STATIC SINT32 InitBoard(AVSTREAM_COOKIE Cookie,BOARD_CONTEXT* pBoardContext,BOARD_INFO *pBoardInfo)
{
	strcpy(pBoardInfo->Name, "ISpy");
	pBoardInfo->RevisionNum = 0x0207;
	pBoardInfo->BusMode = UM_USB_ONCHIP_MODE1;
	pBoardInfo->Capability = BOARD_CAP_VIDEO | BOARD_CAP_AUDIO | BOARD_PROP_DUALFIRMWARE;

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
	SensorSetting->Brightness = 75;
	SensorSetting->BrightnessAuto = 0;
	SensorSetting->Contrast = 50;
	SensorSetting->ContrastAuto = 0;
	SensorSetting->Hue = 12;
	SensorSetting->HueAuto = 0;
	SensorSetting->Saturation = 51;
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
		   CAP_VIDEO_HUE |
		   CAP_VIDEO_SATURATION;
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
	int i, rt;
	SINT32 Index[1];
	SINT32 Value[1];

	if (g_pAF == NULL)
		return E_UNKNOWN;

	Index[0] = 2;
	Value[0] = 0;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	Index[0] = 0;
	Value[0] = 0;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	Index[0] = 0;
	Value[0] = 1;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	for (i = 0; i < reg_num; i++)
	{
		rt = g_pAF->SPI_WriteRegister(Cookie, 8, InitRegs[i].OpCode, 2, InitRegs[i].Addr, 6, InitRegs[i].Value, 275);
		if (rt != 0)
			return E_UNKNOWN;
	}

	// ignore video source and tv-standard for this board
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
	SINT32 reg_num = sizeof(InitRegs)/sizeof(InitRegs[0]);
	int i, rt;
	SINT32 Index[1];
	SINT32 Value[1];

	if (g_pAF == NULL)
		return E_UNKNOWN;

	Index[0] = 2;
	Value[0] = 0;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	Index[0] = 0;
	Value[0] = 0;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	Index[0] = 0;
	Value[0] = 1;
	g_pAF->GPIO_WritePins(Cookie, 1, Index, Value, GM_ONCHIP);

	for (i = 0; i < reg_num; i++)
	{
		rt = g_pAF->SPI_WriteRegister(Cookie, 8, InitRegs[i].OpCode, 2, InitRegs[i].Addr, 6, InitRegs[i].Value, 275);
		if (rt != 0)
			return rt;
	}

	if (!video_config)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY, ("OnStartCapture video_config == NULL"));
		return E_ARG;
	}

	video_config->misccfg.iip_enable = 1;
	video_config->rescfg.h_sub_window = 1;
	video_config->rescfg.h_sub_offset = 58;
	video_config->rescfg.v_sub_window = 1;
	//video_config->rescfg.v_sub_offset = 20; Don't do it in QQVGA mode; Wiseway 2004-05-27
	/* The following code are added by wiseway for QQVGA; 2004-05-27*/
	if (video_config->rescfg.height>video_config->syscfg.sensor_v/4)	
		video_config->rescfg.v_sub_offset = 20;
	if (video_config->rescfg.width<=video_config->syscfg.sensor_h/4)
		video_config->rescfg.h_sub_offset = 24;
	/* The above code are added by wiseway for QQVGA; 2004-05-27*/

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
	switch(Index)
	{
		case PROP_VIDEO_BRIGHTNESS:
			SetBrightness(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_HUE:
			SetHue(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_SATURATION:
			SetSaturation(pContext,Cookie,value, param);
			break;

		default:
			return E_SUPPORT;
	}

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
			return E_SUPPORT;
	}
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("GetVideoProperty return %d",*value));

	return SUCCESS;
}

STATIC SINT32 GetAudioCapabilities(BOARD_CONTEXT pContext,AVSTREAM_COOKIE cookie)
{
	return 
		CAP_AUDIO_FORMAT_PCM |
		CAP_AUDIO_SAMPLERATE_8K |
        CAP_AUDIO_I2S_MASTER_MODE |
		CAP_AUDIO_CHANNEL_MONO |
		CAP_AUDIO_SAMPLE_16BIT;
}
// convert user set audio config to chip audio chip
STATIC void GetAudioChipConfig(BOARD_CONTEXT pContext,AVSTREAM_COOKIE cookie,TCFGAUDIO *pAudioChipConfig, AUDIO_CONFIG *audio_config)
{

	pAudioChipConfig->main_divider = 1;
	pAudioChipConfig->BCLK_divider = 48;
	pAudioChipConfig->OKI_mode = 1;
	pAudioChipConfig->OKI_short_frame_mode = 0;
	pAudioChipConfig->clock_generator_enable = 1;
	pAudioChipConfig->I2S_master_mode = 1;
	pAudioChipConfig->I2S_mode = 3;
	pAudioChipConfig->invert_phase_BCLK = 1;
	pAudioChipConfig->OKI_shoftframe_mode = 0;
	pAudioChipConfig->word_length = 14;//(UINT16)audio_config->SampleBits;
	pAudioChipConfig->auto_SOF_generation = 1;
	
	//if(audio_config->Channels == 1)
		pAudioChipConfig->left_channel_only = 1;
	//else
	//	pAudioChipConfig->left_channel_only = 0;
	
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
//        pAudioChipConfig->bytes_per_block = 1010; pacgen don't care about this
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

STATIC SINT32 InitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE cookie,AUDIO_CONFIG *pAudioConfig)
{
	return 0;
}

STATIC SINT32 UninitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE cookie)
{
	return 0;
}
//////////////////////////////////////////////////////

STATIC IBOARD_BASIC IBoardBasic = 
{
	InitBoard,
	UnloadBoard
};
STATIC IBOARD_AUDIO IBoardAudio = 
{
	GetAudioCapabilities,
	GetAudioChipConfig,
	InitializeAudio,
	UninitializeAudio,
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

BOARD_INTERFACE BI_ISpy = 
{
	sizeof(BOARD_INTERFACE),
	0x0eb1,					// Vender ID of WIS Technologies Inc.
	0x7007,					// Product ID of GO7007SB chip
	0x0207,					// revision number of XMen
	&IBoardBasic,
	&IBoardVideo,
	&IBoardAudio,                           //For PCM
	NULL,
	NULL
};

/****************************** end of Board_XMenII.c ************************/
