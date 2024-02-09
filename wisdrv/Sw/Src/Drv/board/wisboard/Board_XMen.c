/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Board_XMen.c
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
// XMen

#include "typedef.h"
#include "debug.h"
#include "Board.h"
#include "platform.h"

/////////////////////////////////////////////////////////////////////
// common data
/////////////////////////////////////////////////////////////////////

#define OVT7640_WRITE_DEVICEADDR 0x42
#define OVT7640_READ_DEVICEADDR	 0x43
#define OVT7640_BRIGHTNESSADDR	 0x06
#define OVT7640_HUEADDR			 0x04
#define OVT7640_SATURATIONADDR	 0x03

static TCFGSYSTEM SystemCfgs[] = 
{
	{
		{
			"Omnivision 7640 NTSC",		// name of configuration for system setting
			"Omnivision 7640 NTSC",		
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_None,				// tv_standard
		30000,							// framerate, will devided by 1001, for example 30000 represents 29.97
		640,							// sensor_h, sensor valid pixels in horizontal
		480,							// sensor_v, sensor valid pixels in vertical
		0,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		0,								// sensor_656_mode, 1: input format is 656 mode
		0,								// valid_enable
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		1,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},
};

static UINT8 DeviceAddr = 0x42;
static INIT_REGISTER InitRegs[]=
{
	{ 0x12, 0x80 },
	{ 0x03, 0xa4 },
	{ 0x04, 0x2f },
	{ 0x05, 0x3e },
	{ 0x06, 0x60 },
	{ 0x11, 0x00 },
	{ 0x12, 0x54 },
	{ 0x13, 0xa3 },
	{ 0x14, 0x04 },
	{ 0x15, 0x00 },
	{ 0x20, 0xd0 },
	{ 0x23, 0xde },
	{ 0x24, 0x42 },
	{ 0x25, 0x80 },
	{ 0x26, 0xa2 },
	{ 0x27, 0xe2 },
	{ 0x28, 0x20 },
	{ 0x2a, 0x90 },
	{ 0x2b, 0x5e },
	{ 0x2d, 0x01 },
	{ 0x2f, 0x94 },
	{ 0x30, 0x40 },
	{ 0x31, 0xa0 },
	{ 0x60, 0x06 },
	{ 0x61, 0xe0 },
	{ 0x62, 0x88 },
	{ 0x63, 0x11 },
	{ 0x64, 0x89 },
	{ 0x65, 0x00 },
	{ 0x67, 0x94 },
	{ 0x68, 0x7a },
	{ 0x69, 0x08 },
	{ 0x6c, 0x11 },
	{ 0x6d, 0x33 },
	{ 0x6e, 0x22 },
	{ 0x6f, 0x00 },
	{ 0x74, 0x20 },
	{ 0x75, 0x86 },
	{ 0x77, 0xa6 },
};

STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param);

STATIC SINT32 SetBrightness(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	UINT8  register_value;

	if ( value > 100 || value < 0 ) return E_ARG;

	register_value = (UINT8) (value * 255 / 100); 
	rt = g_pAF->I2C_WriteRegister(Cookie, OVT7640_WRITE_DEVICEADDR, 8, OVT7640_BRIGHTNESSADDR, register_value, IM_SCCB);

	return rt;
}

/*
STATIC SINT32 GetBrightness(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32* value, PVOID param)
{
	UINT8  register_value;

	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, OVT7640_READ_DEVICEADDR, 8, OVT7640_BRIGHTNESSADDR, &register_value, IM_SCCB);

	*value = (UINT32)(value) * 100 / 255; 

	return rt;
}
*/

STATIC SINT32 SetHue(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	UINT8  register_value;

	if ( value > 50 || value < -50 ) return E_ARG;

	value += 50;
	register_value = (UINT8) (value * 0x1F / 100); // 0 - 4 bit is hue setting
	register_value |= 0x20;						   // enable hue	
	rt = g_pAF->I2C_WriteRegister(Cookie, OVT7640_WRITE_DEVICEADDR, 8, OVT7640_HUEADDR, register_value, IM_SCCB);

	return rt;
}

/*
STATIC SINT32	GetHue(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32* value, PVOID param)
{
	UINT8  register_value;

	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, OVT7640_READ_DEVICEADDR, 8, OVT7640_HUEADDR, &register_value, IM_SCCB);

	*value = (UINT32)(register_value & 0x1F) * 100 / 0x1F - 50; 

	return rt;
}
*/

STATIC SINT32	SetSaturation(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	UINT8  register_value;

	if ( value > 100 || value < 0 ) return E_ARG;

	register_value = (UINT8) (value * 255 / 100); 
	rt = g_pAF->I2C_WriteRegister(Cookie, OVT7640_WRITE_DEVICEADDR, 8, OVT7640_SATURATIONADDR, register_value, IM_SCCB);

	return rt;
}

/*
STATIC SINT32	GetSaturation(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, SINT32* value, PVOID param)
{
	UINT8  register_value;

	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, OVT7640_READ_DEVICEADDR, 8, OVT7640_SATURATIONADDR, &register_value, IM_SCCB);

	*value = (UINT32)(value) * 100 / 255; 

	return rt;
}
*/


/////////////////////////////////////////////////////////////////////
// function interface
/////////////////////////////////////////////////////////////////////


////////////////////
// basic interface

STATIC SINT32 InitBoard(AVSTREAM_COOKIE Cookie,BOARD_CONTEXT* pBoardContext,BOARD_INFO *pBoardInfo)
{
	strcpy(pBoardInfo->Name, "XMen");
	pBoardInfo->RevisionNum = 0x0200;
	pBoardInfo->BusMode = UM_USB_ONCHIP_MODE1;
	pBoardInfo->Capability = BOARD_CAP_VIDEO;

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
	SensorSetting->Brightness = 38;
	SensorSetting->BrightnessAuto = 0;
	SensorSetting->Contrast = 50;
	SensorSetting->ContrastAuto = 0;
	SensorSetting->Hue = 0;
	SensorSetting->HueAuto = 0;
	SensorSetting->Saturation = 64;
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

	if ( g_pAF == NULL ) return E_UNKNOWN;

	for(i=0;i<reg_num;i++)
	{
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, InitRegs[i].Addr, (UINT8)InitRegs[i].Value, IM_I2C);
		if( rt != 0 ) return E_UNKNOWN;
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
	return SUCCESS;
}

STATIC SINT32 OnStopCapture(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
{
	return SUCCESS;
}


// return SUCCESS(==0) or errno
STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param)
{
	SINT32 rt = SUCCESS;
	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("SetVideoProperty %d %d",Index, value));
	switch(Index)
	{
		case PROP_VIDEO_BRIGHTNESS:
			rt = SetBrightness(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_HUE:
			rt = SetHue(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_SATURATION:
			rt = SetSaturation(pContext,Cookie,value, param);
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

BOARD_INTERFACE BI_XMen = 
{
	sizeof(BOARD_INTERFACE),
	0x0eb1,					// Vender ID of WIS Technologies Inc.
	0x7007,					// Product ID of GO7007SB chip
	0x0200,					// revision number of XMen
	&IBoardBasic,
	&IBoardVideo,
	NULL,
	NULL,
	NULL
};

/****************************** end of Board_XMen.c **************************/
