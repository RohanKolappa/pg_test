/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Board_MatrixII.c
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

// Matrix II 

#include "typedef.h"
#include "debug.h"
#include "Board.h"
#include "platform.h"
 
/////////////////////////////////////////////////////////////////////
// common data
/////////////////////////////////////////////////////////////////////

#define SAA7115_ADDR_ANALOGCONTROL1		0x02
#define SAA7115_ADDR_LUMINANCECONTROL	0x09
#define SAA7115_ADDR_BRIGHTNESS			0x0a
#define SAA7115_ADDR_CONTRAST			0x0b
#define SAA7115_ADDR_HUE				0x0d
#define SAA7115_ADDR_SATURATION			0x0c

#define SAA7115_ADDR_STATUS_BYTE1		0x1e
#define SAA7115_ADDR_STATUS_BYTE2		0x1f

static TCFGSYSTEM SystemCfgs[] = 
{
	{
		{
			"SAA7115 NTSC | PAL60",	// name of configuration for system setting
			"SAA7115",						// sensor configuration file name
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
		1,								// valid_enable       //bache add for scaling
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},

	{
		{
			"SAA7115 PAL",					// name of configuration for system setting
			"SAA7115",						// sensor configuration file name
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
		1,								// valid_enable      //bache add for scaling
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},

	{
		{
			"SAA7115 SECAM",					// name of configuration for system setting
			"SAA7115 SECAM",						// sensor configuration file name
			0,
			sizeof(TCFGSYSTEM),
		},
		TVStandard_SECAM_Mask,				// tv_standard
		25025,							// framerate, will devided by 1001, for example 30000 represents 29.97
		720,							// sensor_h, sensor valid pixels in horizontal
		576,							// sensor_v, sensor valid pixels in vertical
		1,								// format, 0 : YUV progressive, 1: YUV interlace, 2 : RGB Bayer
		0,								// pformat ,'MultiMedia.h': EPixelMode, valid only if sensor_format si RGB bayer
		1,								// sensor_656_mode, 1: input format is 656 mode
		1,								// valid_enable     //bache add for scaling
		0,								// valid_polar
		0,								// href_polar, h reference polar mode, only valid in non-656 mode
		0,								// vref_polar, v reference polar mode
		0,								// field_id_polar
		0,								// sensor_bit_width, 0: 8bits, 1: 10bits
		0,								// hv_resync_enable
	},
};

static UINT8 DeviceAddr = 0x40;
static INIT_REGISTER InitRegs[]=
{
	{ 0x01, 0x08 },
	{ 0x02, 0xc9 },
	{ 0x03, 0x20 },
	{ 0x04, 0x80 },
	{ 0x05, 0x80 },
	{ 0x06, 0xeb },
	{ 0x07, 0xe0 },
	{ 0x08, 0xf0 },	//always toggle FID
	{ 0x09, 0x80 },
	{ 0x0a, 0x80 },
	{ 0x0b, 0x40 },
	{ 0x0c, 0x40 },
	{ 0x0d, 0x00 },
	{ 0x0e, 0x03 },
	{ 0x0f, 0x2a },
	{ 0x10, 0x0e },
	{ 0x11, 0x00 },
	{ 0x12, 0x8d },
	{ 0x13, 0x00 },
	{ 0x14, 0x00 },
	{ 0x15, 0x11 },
	{ 0x16, 0x01 },
	{ 0x17, 0xda },
	{ 0x18, 0x40 },
	{ 0x19, 0x80 },
	{ 0x1a, 0x00 },
	{ 0x1b, 0x42 },
	{ 0x1c, 0xa9 },
	{ 0x30, 0x66 },
	{ 0x31, 0x90 },
	{ 0x32, 0x01 },
	{ 0x34, 0x00 },
	{ 0x35, 0x00 },
	{ 0x36, 0x20 },
	{ 0x38, 0x03 },
	{ 0x39, 0x20 },
	{ 0x3a, 0x88 },
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
	{ 0x54, 0xf4 /*0xff*/ },
	{ 0x55, 0xff },
	{ 0x56, 0xff },
	{ 0x57, 0xff },
	{ 0x58, 0x40 },
	{ 0x59, 0x47 },
	{ 0x5a, 0x06 /*0x03*/ },
	{ 0x5b, 0x83 },
	{ 0x5d, 0x06 },
	{ 0x5e, 0x00 },
	{ 0x80, 0x30 },		//window defined scaler operation, task A and B enabled
	{ 0x81, 0x03 },		//use scaler datapath generated V
	{ 0x83, 0x00 },
	{ 0x84, 0x00 },
	{ 0x85, 0x00 },
	{ 0x86, 0x45 },
	{ 0x87, 0x31 },
	{ 0x88, 0xc0 },
	{ 0x90, 0x02 },	 //task A process top field
	{ 0x91, 0x08 },	
	{ 0x92, 0x09 },
	{ 0x93, 0x80 },
	{ 0x94, 0x02 /*0x04*/ },
	{ 0x95, 0x00 },
	{ 0x96, 0xd0 },
	{ 0x97, 0x02 },
	{ 0x98, 0x10 },
	{ 0x99, 0x00 },
	{ 0x9a, 0xf2 },
	{ 0x9b, 0x00 },
	{ 0x9c, 0xd0 },
	{ 0x9d, 0x02 },
	{ 0x9e, 0xf2 },
	{ 0x9f, 0x00 },
	{ 0xa0, 0x01 },
	{ 0xa1, 0x00 },
	{ 0xa2, 0x00 },
	{ 0xa4, 0x80 },
	{ 0xa5, 0x40 },
	{ 0xa6, 0x40 },
	{ 0xa8, 0x00 },
	{ 0xa9, 0x04 },
	{ 0xaa, 0x00 },
	{ 0xac, 0x00 },
	{ 0xad, 0x02 },
	{ 0xae, 0x00 },
	{ 0xb0, 0x00 },
	{ 0xb1, 0x04 },
	{ 0xb2, 0x00 },
	{ 0xb3, 0x04 },
	{ 0xb4, 0x00 },
	{ 0xb8, 0x00 },
	{ 0xbc, 0x00 },
	{ 0xc0, 0x03 },		//task B process bottom field
	{ 0xc1, 0x08 },
	{ 0xc2, 0x09 },
	{ 0xc3, 0x80 },
	{ 0xc4, 0x02 },
	{ 0xc5, 0x00 },
	{ 0xc6, 0xd0 },
	{ 0xc7, 0x02 },
	{ 0xc8, 0x10 },
	{ 0xc9, 0x00 },
	{ 0xca, 0xf2 },
	{ 0xcb, 0x00 },
	{ 0xcc, 0xd0 },
	{ 0xcd, 0x02 },
	{ 0xce, 0xf2 },
	{ 0xcf, 0x00 },
	{ 0xd0, 0x01 },
	{ 0xd1, 0x00 },
	{ 0xd2, 0x00 },
	{ 0xd4, 0x80 },
	{ 0xd5, 0x40 },
	{ 0xd6, 0x40 },
	{ 0xd8, 0x00 },
	{ 0xd9, 0x04 },
	{ 0xda, 0x00 },
	{ 0xdc, 0x00 },
	{ 0xdd, 0x02 },
	{ 0xde, 0x00 },
	{ 0xe0, 0x00 },
	{ 0xe1, 0x04 },
	{ 0xe2, 0x00 },
	{ 0xe3, 0x04 },
	{ 0xe4, 0x00 },
	{ 0xe8, 0x00 },
	{ 0x88, 0xf0 },
};

typedef struct
{
	VIDEO_SOURCE	CurVideoSource;		// VS_SVIDEO or VS_COMPOSITE only for this board.
	TV_STANDARD		CurTVStandard;		// NTSC or PAL or SECAM
} MatrixII_Context_t;

STATIC SINT32 SetVideoProperty(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,VIDEO_PROPERTY_INDEX Index, UINT32 value, PVOID param);

STATIC SINT32 SetVideoSource(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value)
{
	SINT32 rt = SUCCESS;
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;

	if( value == VS_SVIDEO )	// s-video
	{
		ctx->CurVideoSource = VS_SVIDEO;
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_ANALOGCONTROL1, 0xC9, IM_I2C);
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_LUMINANCECONTROL, 0x80, IM_I2C);
	}
	else if ( value == VS_COMPOSITE ) // composite
	{
		ctx->CurVideoSource = VS_COMPOSITE;
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_ANALOGCONTROL1, 0xC0, IM_I2C);
		rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_LUMINANCECONTROL, 0x40, IM_I2C);
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
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;

	if ( value == TVStandard_Default )
		return SUCCESS;
	else if ( value == TVStandard_None )
		return E_ARG;
	else if ( value & TVStandard_NTSC_Mask )
	{
		rt  = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xC0, IM_I2C); //disable scaler
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x98, 0x10, IM_I2C); //task A Vertical start =0x10
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9A, 0xf2, IM_I2C); //Veritcal input window length = 0x0f2
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9B, 0x00, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9E, 0xf2, IM_I2C); //Veritcal output window length = 0x0f2
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9F, 0x00, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xC8, 0x10, IM_I2C); //task B Vertical start =0x10
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCA, 0xf2, IM_I2C); //Veritcal input window length = 0x0f2
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCB, 0x00, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCE, 0xf2, IM_I2C); //Veritcal output window length = 0x0f2
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCF, 0x00, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xf0, IM_I2C); //enable scaler
		if( rt != 0 ) return E_UNKNOWN;
	}
	else if ( value & TVStandard_PAL_Mask )
	{
		rt  = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xC0, IM_I2C); //disable scaler
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x98, 0x14, IM_I2C); //task A Vertical start =0x14
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9A, 0x20, IM_I2C); //Veritcal input window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9B, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9E, 0x20, IM_I2C); //Veritcal output window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9F, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xC8, 0x14, IM_I2C); //task B Vertical start =0x14
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCA, 0x20, IM_I2C); //Veritcal input window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCB, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCE, 0x20, IM_I2C); //Veritcal output window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCF, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xf0, IM_I2C); //enable scaler
		if( rt != 0 ) return E_UNKNOWN;
	}
	else if ( value & TVStandard_SECAM_Mask )
	{
		rt  = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xC0, IM_I2C); //disable scaler
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x98, 0x14, IM_I2C); //task A Vertical start =0x14
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9A, 0x20, IM_I2C); //Veritcal input window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9B, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9E, 0x20, IM_I2C); //Veritcal output window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9F, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xC8, 0x14, IM_I2C); //task B Vertical start =0x14
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCA, 0x20, IM_I2C); //Veritcal input window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCB, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCE, 0x20, IM_I2C); //Veritcal output window length = 0x120
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xCF, 0x01, IM_I2C);
		rt |= g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x88, 0xf0, IM_I2C); //enable scaler
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
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_BRIGHTNESS, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 SetContrast(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 100 || value < 0 ) return E_ARG;

	value1 = value * 127 / 100; 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_CONTRAST, (UINT8)value1, IM_I2C);

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
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_HUE, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 SetSaturation(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 value, PVOID param)
{
	SINT32 rt, value1;

	if ( value > 100 || value < 0 ) return E_ARG;

	value1 = value * 127 / 100; 
	rt = g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_SATURATION, (UINT8)value1, IM_I2C);

	return rt;
}

STATIC SINT32 GetHLockStatus(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, UINT32* value)
{
	UINT8 ret_value;
	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_STATUS_BYTE2, &ret_value, IM_I2C);

	if(rt != SUCCESS)
		return E_UNKNOWN;

	*value = 1;//(ret_value & 0x40) ? 0 : 1;
	return SUCCESS;
}

STATIC SINT32 GetMacrovision(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie, UINT32* value)
{
	UINT8 ret_value;
	SINT32 rt = g_pAF->I2C_ReadRegister(Cookie, DeviceAddr, 8, SAA7115_ADDR_STATUS_BYTE2, &ret_value, IM_I2C);

	if(rt != SUCCESS)
		return E_UNKNOWN;

	*value = 0;//(ret_value>>1) & 0x0001;	// bit 1
	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// function interface
/////////////////////////////////////////////////////////////////////

////////////////////
// basic interface

STATIC SINT32 InitBoard(AVSTREAM_COOKIE Cookie,BOARD_CONTEXT* pBoardContext,BOARD_INFO *pBoardInfo)
{
	MatrixII_Context_t* ctx = PIAllocMem(sizeof(MatrixII_Context_t));
	if ( ctx == NULL ) return E_MEM;
	*pBoardContext = ctx;
	
    ctx->CurVideoSource = VS_SVIDEO; //default is s-video
	ctx->CurTVStandard = TVStandard_NTSC_M; //default is NTSC_M

	strcpy(pBoardInfo->Name, "Matrix II");
	pBoardInfo->RevisionNum = 0x0202;
	pBoardInfo->BusMode = UM_USB_EZUSB;
	pBoardInfo->Capability = BOARD_CAP_VIDEO|BOARD_CAP_AUDIO|BOARD_CAP_XBAR|BOARD_CAP_HWCC|BOARD_CAP_TELETEXT;

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

STATIC VOID	GetDefaultSensorParameters(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SENSOR_PARAMETERS* SensorSetting)
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
	int i, rt = SUCCESS;

	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;
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
	
	UINT8 m_HHLeng,m_HLLeng,m_HPres,m_HHLumce, m_HLLumce,m_HHChromce, m_HLChromce;
	UINT8 m_VHLeng,m_VLLeng,m_VLLumce,m_VHLumce,m_VLChromce,m_VHChromce;
	 
	SINT32 m_TempHLumce,m_TempChromce,m_HTempLength;
	SINT32 m_Vleng ,m_TempVLeng,m_TempVLumce,m_TempVChromce;
	unsigned long	m_VHalfleng,m_VlengAdjust;
    
	if (!video_config)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY, ("OnStartCapture video_config == NULL"));
		return E_ARG;
	}
 
	//begin of bache add for scaling feature 2004.4.6
    m_VlengAdjust = video_config->rescfg.height;
	if((video_config->syscfg.tv_standard&TVStandard_NTSC_Mask)!= 0)
	{
	
	   m_VlengAdjust +=4;     //for avoid iterance shadow in NTSC system, So adjust PAL input vertical length.
       m_Vleng = 480;
       m_VHalfleng = 240;
	}
	if(((video_config->syscfg.tv_standard&TVStandard_PAL_Mask)!= 0)||((video_config->syscfg.tv_standard&TVStandard_SECAM_Mask)!= 0))
	{
       m_Vleng = 576;
	   m_VHalfleng = 288;
	}
		 
    // horizontal scaling down
    if(((video_config->syscfg.tv_standard&TVStandard_NTSC_Mask)!= 0)
       ||((video_config->syscfg.tv_standard&TVStandard_PAL_Mask)!= 0)
       ||((video_config->syscfg.tv_standard&TVStandard_SECAM_Mask)!= 0))
	 {
	       video_config->rescfg.h_scale_enb = 0;
		   video_config->rescfg.h_sub_offset = 0;
	       video_config->rescfg.v_sub_offset = 0;
	       video_config->rescfg.sub_sample = 0;
		   m_HTempLength = video_config->rescfg.width;
           m_HHLeng = (m_HTempLength>>8)&0xff;
           m_HTempLength = video_config->rescfg.width;
           m_HLLeng = m_HTempLength&0xff;
           m_HTempLength = (720/video_config->rescfg.width);
           m_HPres = m_HTempLength&0xff;
           m_TempHLumce = 720/m_HPres;      
           m_TempHLumce = m_TempHLumce*1024;
           m_TempHLumce = m_TempHLumce/video_config->rescfg.width;
		   m_TempChromce = m_TempHLumce/2;
           m_HLLumce = m_TempHLumce&0xff;
		   m_HHLumce = (m_TempHLumce>>8)&0xff;
           m_HLChromce = m_TempChromce&0xff;
		   m_HHChromce = (m_TempChromce>>8)&0xff;	

           //horizontal output window length 	
	       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9c, m_HLLeng, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9d, m_HHLeng, IM_I2C);
		   //integer prescale
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xa0, m_HPres, IM_I2C);         
		   //horizontal scaling increment for luminance
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xa8, m_HLLumce, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xa9, m_HHLumce, IM_I2C);
		   //horizontal scaling increment for chrominance
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xac, m_HLChromce, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xad, m_HHChromce, IM_I2C);
		 
		   //horizontal output window length 	
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcc, m_HLLeng, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcd, m_HHLeng, IM_I2C);
		   //integer prescale
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xd0, m_HPres, IM_I2C);
		   //horizontal scaling increment for luminance
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xd8, m_HLLumce, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xd9, m_HHLumce, IM_I2C);
		   //horizontal scaling increment for chrominance
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xdc, m_HLChromce, IM_I2C);
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xdd, m_HHChromce, IM_I2C);
		  
		   //Horizontal prescaler accumulation Sequence Length
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xa1, 0x01, IM_I2C);		   
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xd1, 0x01, IM_I2C);
		   //Prescaler DC gain and FIR prefilter Control
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xa2, 0x01, IM_I2C);		   
		   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xd2, 0x01, IM_I2C);		   
	 }

   
	//vertical scaling down			  
	if(video_config->rescfg.height> m_VHalfleng)
	{
          video_config->rescfg.v_scale_enb = 0;
		  
          video_config->rescfg.h_sub_offset = 0;
          video_config->rescfg.v_sub_offset = 0;
          video_config->rescfg.sub_sample = 0;

       
          m_TempVLeng = m_VlengAdjust/2;
		  m_VHLeng= (m_TempVLeng>>8)&0xff;
		  m_TempVLeng = m_VlengAdjust/2;
          m_VLLeng= m_TempVLeng&0xff;
		  m_TempVLumce = m_Vleng*1024;
		  m_TempVLumce = m_TempVLumce/video_config->rescfg.height;
		  m_TempVChromce = m_TempVLumce;
		  m_VLLumce = m_TempVLumce&0xff;
		  m_VHLumce = (m_TempVLumce>>8)&0xff;
		  m_VLChromce = m_TempVChromce&0xff;
          m_VHChromce = (m_TempVChromce>>8)&0xff;


		  //vertical output window length
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9e, m_VLLeng, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9f, m_VHLeng, IM_I2C);		
		  // vertical scaling increment for luminance
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb0, m_VLLumce, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb1, m_VHLumce, IM_I2C);
		  // vertical scaling increment for luminance
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb2, m_VLChromce, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb3, m_VHChromce, IM_I2C);	

   		  //vertical output window length
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xce, m_VLLeng, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcf, m_VHLeng, IM_I2C);		
		  // vertical scaling increment for luminance
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe0, m_VLLumce, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe1, m_VHLumce, IM_I2C);
		  // vertical scaling increment for luminance
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe2, m_VLChromce, IM_I2C);	
		  g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe3, m_VHChromce, IM_I2C);
	}
	
	if(video_config->rescfg.height<= m_VHalfleng)
	{

	   if((video_config->syscfg.tv_standard&TVStandard_NTSC_Mask)!= 0)
	   {
	   	   switch (video_config->rescfg.height)
	       {
		     case 240:                
				   //do nothing only let's GO7007SB do 1/2 scaling down
		           //vertical output window length
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9e, 0xf2, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9f, 0x00, IM_I2C);		
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb1, 0x04, IM_I2C);
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb3, 0x04, IM_I2C);	

                   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xce, 0xf2, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcf, 0x00, IM_I2C);		
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe1, 0x04, IM_I2C);
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe3, 0x04, IM_I2C);
				   	video_config->rescfg.v_scale_enb = 1;
				break;

			 case 112:
					//Saa7115 scaling donw from 480 to 288 then GO7007 do 1/2 vertical scaling down
				    //vertical output window length 0x0096 == 150, increase vertical output window to avoid roll image
			        g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9e, 0x78, IM_I2C);	
				    g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9f, 0x00, IM_I2C);		
			         // vertical scaling increment for luminance
			        g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb0, 0x92, IM_I2C);	
			        g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb1, 0x08,IM_I2C);
			        // vertical scaling increment for luminance
			        g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb2, 0x92, IM_I2C);	
			        g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb3, 0x08, IM_I2C);
                            

				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xce, 0x78, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcf, 0x00, IM_I2C);		
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe0, 0x92, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe1, 0x08, IM_I2C);
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe2, 0x92, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe3, 0x08, IM_I2C);	
				   video_config->rescfg.v_scale_enb = 1;
			    break;
	        }	   
	   }

	   if(((video_config->syscfg.tv_standard&TVStandard_PAL_Mask)!= 0)||((video_config->syscfg.tv_standard&TVStandard_SECAM_Mask)!= 0))
	   {
		   switch(video_config->rescfg.height)
		   {
		    case 288:      
				   //do nothing only let's GO7007SB do 1/2 scaling down
		           //vertical output window length 
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xce, 0x20, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcf, 0x01, IM_I2C);		
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe1, 0x04, IM_I2C);
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe3, 0x04, IM_I2C);	 


                   //vertical output window length
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9e, 0x20, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9f, 0x01, IM_I2C);		
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb1, 0x04, IM_I2C);
			       // vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb3, 0x04, IM_I2C);
		    	   video_config->rescfg.v_scale_enb = 1;
				break;
		    case 144:
				   //Saa7115 scaling donw from 576 to 288 then GO7007 do 1/2 vertical scaling down
				   //vertical output window length 0x0096 == 150, increase vertical output window to avoid roll image
		           g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9e, 0x96, IM_I2C);  	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x9f, 0x00, IM_I2C);		
			       //vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb1, 0x08, IM_I2C);
			       //vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xb3, 0x08, IM_I2C);
                            

				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xce, 0x96, IM_I2C);	
				   g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xcf, 0x00, IM_I2C);		
			       //vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe0, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe1, 0x08, IM_I2C);
			       //vertical scaling increment for luminance
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe2, 0x00, IM_I2C);	
			       g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0xe3, 0x08, IM_I2C);	
				   video_config->rescfg.v_scale_enb = 1;
				break;    	    
		    }
	   }
	}
   	//end of bache add for scaling feature 2004.4.6
	
	if ( video_config->rescfg.v_sub_offset == 0 && video_config->syscfg.tv_standard == TVStandard_NTSC_Mask )
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
			rt = SetVideoSource(pContext,Cookie,value);
			break;

		case PROP_VIDEO_BRIGHTNESS:
			rt = SetBrightness(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_CONTRAST:
			rt = SetContrast(pContext,Cookie,value, param);
			break;

		case PROP_VIDEO_HUE:
			rt = SetHue(pContext,Cookie,value, param);
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
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;
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
	pAudioChipConfig->clock_generator_enable = 0;
	pAudioChipConfig->I2S_master_mode = 0;
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

STATIC SINT32 InitializeAudio(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,AUDIO_CONFIG *pAudioConfig)
{
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;

	if ( ctx->CurTVStandard == TVStandard_None ) return E_UNKNOWN;

	if( ctx->CurTVStandard & TVStandard_NTSC_Mask )
	{
		if(pAudioConfig->SampleRate == 48000)
		{
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x30, 0x66, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x31, 0x90, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x32, 0x01, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x34, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x35, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x36, 0x20, IM_I2C);
		}
		else if(pAudioConfig->SampleRate == 44100)
		{
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x30, 0xde, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x31, 0x6f, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x32, 0x01, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x34, 0x66, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x35, 0x66, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x36, 0x1d, IM_I2C);
		}
	}

	else if( ctx->CurTVStandard & TVStandard_PAL_Mask )
	{
		if(pAudioConfig->SampleRate == 48000)
		{
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x30, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x31, 0xe0, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x32, 0x01, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x34, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x35, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x36, 0x20, IM_I2C);
		}
		else if(pAudioConfig->SampleRate == 44100)
		{
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x30, 0x00, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x31, 0xb9, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x32, 0x01, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x34, 0x66, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x35, 0x66, IM_I2C);
			g_pAF->I2C_WriteRegister(Cookie, DeviceAddr, 8, 0x36, 0x1d, IM_I2C);
		}
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
	return XBAR_MODE_BASIC;

}

STATIC SINT32 Get_Current_Video_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
/*
	return the curent video input to route to analog video decoder

*/
{
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;
	if ( ctx->CurVideoSource == VS_SVIDEO )
		return 1; // the second ( 0 based index ) pin in cross bar.
	else if ( ctx->CurVideoSource == VS_COMPOSITE )
		return 0; // the first ( 0 based index ) pin in cross bar.
	else
		return E_UNKNOWN;
}

STATIC SINT32 Get_Current_Audio_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie)
/*
	return the curent video input to route to analog video decoder

*/
{
	return 2; // the third ( 0 based index ) pin in cross bar.
}

STATIC SINT32 Set_Current_Video_InputID(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,SINT32 videoid)
/*
	videoid indicates which input pin is selected currently 
	reutrn value is 0 indicates OK 
*/
{
	MatrixII_Context_t* ctx = (MatrixII_Context_t*) pContext;

	PrintMsg(DEBUG_ALWAYS_DISPLAY, ("Set_Current_Video_InputID %d",	videoid));

	if ( videoid == 0 )	                /* 0 -- video composite in */
		SetVideoSource(pContext,Cookie,VS_COMPOSITE);	        /* set to composite */
	else if( videoid == 1 )				/* video s-video in */
		SetVideoSource(pContext,Cookie,VS_SVIDEO);          /* set to s-video   */
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

BOARD_INTERFACE BI_MatrixII = 
{
	sizeof(BOARD_INTERFACE),
	0x0eb1,					// Vender ID of WIS Technologies Inc.
	0x7007,					// Product ID of GO7007SB chip
	0x0202,					// revision number of Matrix II
	&IBoardBasic,
	&IBoardVideo,
	&IBoardAudio,
	&BoardXbarInterface,
	NULL,
};

/****************************** end of Board_MatrixII.c **********************/
