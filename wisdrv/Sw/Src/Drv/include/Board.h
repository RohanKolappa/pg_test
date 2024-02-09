/*****************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
******************************************************************************
*
*   FILE: 
*       Board.h (Windows platform only)
*
*   DESCRIPTION:
*		Interface of the board module to the driver module:
*		Each board module must export InitBoardModule function, through this
*		function the access function table are passed into the board module
*		and the board dispatch table are passed back to the driver module.
*		Multiple boards can be supported in one board module; each board should
*		implement different dispatch table define below.
*		IBOARD_BASIC: (mandetory) provide basic board information and functionalities
*		IBOARD_VIDEO: (mandetory) provide sensor and video information and functionalities
*		IBOARD_AUDIO: (optional) provide audio information and functionalities
*		IBOARD_TUNER: (optional) provide tv tuner information and functionalities
*
*   AUTHOR:
*		Jie Sun		<jsun@wischip.com>
*		Jun Ding	<jding@wischip.com>
*
*    <SOURCE CONTROL TAGS TBD>
*
*****************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

/*****************************************************************************
*   Platform specific definitions
*****************************************************************************/

#if defined(_WINDOWS)

	#include <wdm.h>

	#ifdef BOARDMODULE
		#define BOARD_API __declspec(dllexport) __stdcall
	#else
		#define BOARD_API __declspec(dllimport) __stdcall
	#endif

#elif defined(_LINUX)

	#define BOARD_API

#endif

#include "AccessFunc.h"
#include "Struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef  void* BOARD_CONTEXT;
typedef  void* AVSTREAM_COOKIE;

	
/*****************************************************************************
*
*   enumeration structures definition
*
*****************************************************************************/

/*****************************************************************************
*   video properties - WIS Tech reserve the right to add more properties
*****************************************************************************/

typedef enum
{
	PROP_VIDEO_SOURCE,

    PROP_VIDEO_BRIGHTNESS,   
    PROP_VIDEO_BRIGHTNESS_AUTO,

    PROP_VIDEO_CONTRAST,
    PROP_VIDEO_CONTRAST_AUTO,
    
	PROP_VIDEO_HUE,
	PROP_VIDEO_HUE_AUTO,

    PROP_VIDEO_SATURATION,
    PROP_VIDEO_SATURATION_AUTO,

    PROP_VIDEO_SHARPNESS,
    PROP_VIDEO_SHARPNESS_AUTO,

    PROP_VIDEO_GAMMA,
    PROP_VIDEO_GAMMA_AUTO,

    PROP_VIDEO_WHITEBALANCE,
    PROP_VIDEO_WHITEBALANCE_AUTO,

    PROP_VIDEO_BACKLIGHT_COMPENSATION,
    PROP_VIDEO_BACKLIGHT_COMPENSATION_AUTO,

    PROP_VIDEO_COLORENABLE,

	PROP_VIDEO_MACROVISION = 0x1000,
	PROP_VIDEO_HLOCK,

	PROP_VIDEO_TVSTANDARD,

} VIDEO_PROPERTY_INDEX;

/*****************************************************************************
*   sensor capabilities - WIS Tech reserve the right to add more capabilities
*****************************************************************************/

typedef enum
{
	CAP_VIDEO_SOURCE						= 0x00000001,

    CAP_VIDEO_BRIGHTNESS					= 0x00000004,   
    CAP_VIDEO_BRIGHTNESS_AUTO				= 0x00000008,

    CAP_VIDEO_CONTRAST						= 0x00000010,
    CAP_VIDEO_CONTRAST_AUTO					= 0x00000020,
    
	CAP_VIDEO_HUE							= 0x00000040,
	CAP_VIDEO_HUE_AUTO						= 0x00000080,

    CAP_VIDEO_SATURATION					= 0x00000100,
    CAP_VIDEO_SATURATION_AUTO				= 0x00000200,

    CAP_VIDEO_SHARPNESS						= 0x00000400,
    CAP_VIDEO_SHARPNESS_AUTO				= 0x00000800,

    CAP_VIDEO_GAMMA							= 0x00001000,
    CAP_VIDEO_GAMMA_AUTO					= 0x00002000,

    CAP_VIDEO_WHITEBALANCE					= 0x00004000,
    CAP_VIDEO_WHITEBALANCE_AUTO				= 0x00008000,

    CAP_VIDEO_BACKLIGHT_COMPENSATION		= 0x00010000,
    CAP_VIDEO_BACKLIGHT_COMPENSATION_AUTO	= 0x00020000,

    CAP_VIDEO_COLORENABLE					= 0x00040000,
} SENSOR_CAPABILITIES;


/*****************************************************************************
*   board identification information
*****************************************************************************/

#define BOARD_NAME_MAX 64

typedef enum
{
	UM_USB_EZUSB,			// Cypress Easy USB 2.0
	UM_USB_ONCHIP_MODE1,	// On Chip USB 1.1
} BUS_MODE;

typedef enum
{
	BOARD_CAP_VIDEO		= 0x00000001,
	BOARD_CAP_AUDIO		= 0x00000002,
	BOARD_CAP_TVTUNER	= 0x00000004,
	BOARD_CAP_XBAR		= 0x00000008,
	BOARD_CAP_HWCC		= 0x00000010,
	BOARD_CAP_TELETEXT	= 0x00000020,

	BOARD_PROP_DUALFIRMWARE	= 0x00010000,

} BOARD_CAPS;

typedef struct
{
	SINT8		Name[BOARD_NAME_MAX];
	SINT32		RevisionNum;
	BUS_MODE	BusMode;
	UINT32		Capability;
} BOARD_INFO;

/*****************************************************************************
*   audio configuration -- this is strange
*****************************************************************************/


/*****************************************************************************
*   register entry
*****************************************************************************/

typedef struct _INIT_REGISTER
{
	UINT16 Addr;
	UINT16 Value;
} INIT_REGISTER;


/*****************************************************************************
*   IBOARD_BASIC (mandetory interface) :
*	1. Get board identification information
*****************************************************************************/

	typedef SINT32 (*INIT_BOARD)(
		/*in*/ AVSTREAM_COOKIE Cookie,
		/*out*/ BOARD_CONTEXT* pBoardContext,//The board can return a context pointer to AVStream framework, 
											//which will keep the pointer and pass it while calling any API in board module
		/*out*/ BOARD_INFO *pBoardInfo);

	typedef SINT32 (*UNLOAD_BOARD)(
		BOARD_CONTEXT pContext,
		AVSTREAM_COOKIE Cookie);

	typedef struct 
	{
		INIT_BOARD		InitBoard;
		UNLOAD_BOARD	UnloadBoard;
	} IBOARD_BASIC;


/*****************************************************************************
*   IBOARD_VIDEO (mandetory interface) :
*	1. Get sensor capabilities
*	2. Get video capabilities
*	3. Get sensor setting
*	4. Get default sensor parameters
*	5. Initialize sensor callback
*	6. Uninitialize sensor callback
*	7. Start sensor callback
*	8. Stop sensor callback
*	9. Get sensor properties
*	10. Set sensor properties
*****************************************************************************/

	/*****************************************************************************
	*   GET_SENSOR_CAPABILITIES:
	*		return bitmask of SENSOR_CAPABILITIES enumeration.
	*****************************************************************************/
	typedef UINT32 (*GET_SENSOR_CAPABILITIES)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	/*****************************************************************************
	*   GET_VIDEO_CAPABILITIES:
	*		return _VIDEO_CAPABILITIES structures. if board module decide to use
	*		the default capabilities of our chip, return NULL.
	*****************************************************************************/
	typedef _VIDEO_CAPABILITIES* (*GET_VIDEO_CAPABILITIES)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	/*****************************************************************************
	*   GET_SENSOR_SETTINGS:
	*		retrieve supported sensor setting from board module.
	*		caller should allocate the buffer which the board module will fill in.
	*		return zero if success.
	*****************************************************************************/
	typedef SINT32 (*GET_SENSOR_SETTINGS)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					SINT32 *Num, 
					TCFGSYSTEM *syscfg);

	/*****************************************************************************
	*   GET_DEFAULT_SENSOR_PARAMETERS:
	*		retrieve the default sensor parameters for brightness, hue etc.
	*****************************************************************************/
	typedef VOID   (*GET_DEFAULT_SENSOR_PARAMETERS)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					SENSOR_PARAMETERS* SensorSetting);

	/*****************************************************************************
	*   INITIALIZE_SENSOR:
	*		This function is called when the device is connected
	*		SettingIndex indicate which sensor setting is used in this session
	*		SensorSetting is current sensor parameters
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*INITIALIZE_SENSOR)(
					BOARD_CONTEXT pContext, 
					AVSTREAM_COOKIE Cookie,
					TV_STANDARD TVStandard, 
					VIDEO_SOURCE VideoSource, 
					SENSOR_PARAMETERS SensorSetting);

	/*****************************************************************************
	*   UNINITIALIZE_SENSOR:
	*		This function is called when the device is disconnected
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*UNINITIALIZE_SENSOR)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	/*****************************************************************************
	*   START_SENSOR:
	*		This function is called when start capturing
	*		video_config indicate the current video config
	*		SensorSetting is current sensor parameters
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*ON_START_CAPTURE)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie, 
					TCFGVIDEOEX *video_config, 
					SENSOR_PARAMETERS SensorSetting);

	/*****************************************************************************
	*   STOP_SENSOR:
	*		This function is called when stop capturing
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*ON_STOP_CAPTURE)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	/*****************************************************************************
	*   SET_VIDEO_PROPERTY:
	*		This function allow driver module set all properties in board module
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*SET_VIDEO_PROPERTY)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					VIDEO_PROPERTY_INDEX Index, 
					UINT32 value, 
					PVOID param);

	/*****************************************************************************
	*   GET_VIDEO_PROPERTY:
	*		This function allow driver module retrieve all properties in board module
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*GET_VIDEO_PROPERTY)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					VIDEO_PROPERTY_INDEX Index, 
					UINT32* value, 
					PVOID param);

	typedef struct
	{
		GET_SENSOR_CAPABILITIES	GetSensorCapabilities;
		GET_VIDEO_CAPABILITIES	GetVideoCapabilities;
		GET_SENSOR_SETTINGS		GetSensorSettings;
		GET_DEFAULT_SENSOR_PARAMETERS GetDefaultSensorParameters;
		INITIALIZE_SENSOR		InitializeSensor;
		UNINITIALIZE_SENSOR		UninitializeSensor;
		ON_START_CAPTURE		OnStartCapture;
		ON_STOP_CAPTURE			OnStopCapture;
		SET_VIDEO_PROPERTY		SetVideoProperty;
		GET_VIDEO_PROPERTY		GetVideoProperty;
	} IBOARD_VIDEO;

/*****************************************************************************
*   IBOARD_AUDIO (optional interface) :
*	1. Get audio capabilities
*	2. Get audio chip config
*	3. Initialize audio callback
*	4. Uninitialize audio callback
*****************************************************************************/

	/*****************************************************************************
	*   GET_AUDIO_CAPABILITIES:
	*		return bitmask of AUDIO_CAPS enumeration.
	*****************************************************************************/
	typedef SINT32 (*GET_AUDIO_CAPABILITIES)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	/*****************************************************************************
	*   GET_AUDIO_CHIP_CONFIG:
	*		convert user set audio config to chip audio chip
	*****************************************************************************/
	typedef void (*GET_AUDIO_CHIP_CONFIG)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					TCFGAUDIO *pAudioChipConfig, 
					AUDIO_CONFIG *audio_config);

	/*****************************************************************************
	*   INITIALIZE_AUDIO:
	*		this function is called when start capturing
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*INITIALIZE_AUDIO)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie,
					AUDIO_CONFIG *pAudioConfig);

	/*****************************************************************************
	*   UNINITIALIZE_AUDIO:
	*		this function is called when stop capturing
	*		return zero if success
	*****************************************************************************/
	typedef SINT32 (*UNINITIALIZE_AUDIO)(
					BOARD_CONTEXT pContext,
					AVSTREAM_COOKIE Cookie);

	typedef struct
	{
		GET_AUDIO_CAPABILITIES	GetAudioCapabilities;
		GET_AUDIO_CHIP_CONFIG	GetAudioChipConfig;
		INITIALIZE_AUDIO		InitializeAudio;
		UNINITIALIZE_AUDIO		UninitializeAudio;
	} IBOARD_AUDIO;

/*****************************************************************************
*   IBOARD_TUNER (optional interface) :
*****************************************************************************/
//{{TVTuner Defines

typedef enum
{
	TVTUNER_FLAG_TUNERMODE		 =0x1,
	TVTUNER_FLAG_VIDEOSTANDARD	 =0x2,
	TVTUNER_FLAG_FREQUENCY		 =0x4,
	TVTUNER_FLAG_COUNTRY		 =0x8,
	TVTUNER_FLAG_CHANNEL		 =0xA,
	TVTUNER_FLAG_TUNERINPUT		 =0x20,
	TVTUNER_FLAG_BUSY			 =0x40,
	TVTUNER_FLAG_TVAUDIOMODE	 =0x80
}TVTunerSettingFlag;

typedef struct
{	
	UINT32 cb;
	UINT32 Flags;
	UINT32 TunerMode;
	UINT32 VideoStandard;
	UINT32 Frequency;
	UINT32 Country;
	UINT32 Channel;
	UINT32 TunerInput;
	UINT32 Busy;
	UINT32 TVAudioMode;
}TVTunerSettings;

typedef struct
{
	UINT32 min;
	UINT32 max;
	UINT32 TuningGranularity;
	UINT32 NumberOfInputs;
	UINT32 SettlingTime;
}TVTunerFrequencyCaps;
typedef enum
{
      TVTUNER_TV =1,
	  TVTUNER_FMAUDIO =2,
	  TVTUNER_AMAUDIO =4,
	  TVTUNER_DSS    = 0x8,
	  TVTUNER_ATSC   = 0X0010,  // also used for DVB-T, DVB-C
}TVTunerMode;

typedef enum
{
	TVAUDIO_MODE_MONO =          0x0001,          // Mono
	TVAUDIO_MODE_STEREO =        0x0002,          // Stereo
	TVAUDIO_MODE_LANG_A  =       0x0010,          // Primary language
	TVAUDIO_MODE_LANG_B   =      0x0020,          // 2nd avail language
	TVAUDIO_MODE_LANG_C    =     0x0040,          // 3rd avail language
}TVAudioMode;

typedef void (*INIT_TVTUNER)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie);
typedef SINT32 (*GET_TVTUNER_FREQCAPS)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerMode mode,TVTunerFrequencyCaps* pFreqCaps);
typedef SINT32 (*GET_TV_CURRENT_SETTINGS)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerSettings* pSettings);
typedef SINT32 (*SET_TV_CURRENT_SETTINGS)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie,TVTunerSettings* pSettings);

typedef struct
{
	GET_TVTUNER_FREQCAPS		GetTvtunerFreqcaps;
	GET_TV_CURRENT_SETTINGS		GetTvCurrentSettings;
	SET_TV_CURRENT_SETTINGS		SetTvCurrentSettings;
} IBOARD_TUNER;

//TVTuner}}
/*****************************************************************************
*   IBOARD_XBAR (optional interface) :
*****************************************************************************/
//{{Xbar Defines
typedef enum
{
	XBAR_MODE_NOE   =0x0,//The borad doesn't support Xbar,like a camer
	XBAR_MODE_BASIC	= 0x00000001,//In this mode, there are three input pins: Video Composite In,Video SVideo In,Audio Line In
	XBAR_MODE_MUTE	= 0x00000002,//Set this bit, indicates the borad support hardware audio mute.
	XBAR_MODE_TVTUNER	= 0x00000004,//Set this bit, will add two more input pins:Video Tuner In,Audio Tuner In
}XBARCAPS;


typedef SINT32 (*GETXBARCAPS)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie);
/*
	return value can be 
		XBAR_MODE_BASIC  or 
		XBAR_MODE_BASIC|XBAR_MODE_MUTE or 
		XBAR_MODE_BASIC|XBAR_MODE_TVTUNER or 
		XBAR_MODE_BASIC|XBAR_MODE_MUTE|XBAR_MODE_TVTUNER or
	if the return value is 0, indicates the borad doesn't support Xbar,like a camera
*/

typedef SINT32 (*GET_CURRENT_VIDEO_INPUTID)(BOARD_CONTEXT pContext,AVSTREAM_COOKIE Cookie);
/*
	return the curent video input to route to analog video decoder

*/
typedef SINT32 (*GET_CURRENT_AUDIO_INPUTID)(
				BOARD_CONTEXT pContext,
				AVSTREAM_COOKIE Cookie);
/*
	return the curent video input to route to analog video decoder

*/
typedef SINT32 (*SET_CURRENT_VIDEO_INPUTID)(
				BOARD_CONTEXT pContext,
				AVSTREAM_COOKIE Cookie,
				SINT32 videoid);
/*
	videoid indicates which input pin is selected currently 
	reutrn value is 0 indicates OK 
*/
typedef SINT32 (*SET_CURRENT_AUDIO_INPUTID)(
				BOARD_CONTEXT pContext,
				AVSTREAM_COOKIE Cookie,
				SINT32 audioid);
/*
	audioid indicates which input pin is selected currently 
	reutrn value is 0, indicates OK 
*/

typedef struct
{
	GETXBARCAPS GetCaps;
	GET_CURRENT_VIDEO_INPUTID GetCurrentVideoInputID;
	GET_CURRENT_AUDIO_INPUTID GetCurrentAudioInputID;
	SET_CURRENT_VIDEO_INPUTID SetCurrentVideoInputID;
	SET_CURRENT_AUDIO_INPUTID SetCurrentAudioInputID;
}IBOARD_XBAR;

//Xbar}}

/*****************************************************************************
*
*   Error Code
*
*****************************************************************************/

#define SUCCESS		0
#define E_ARG		-1
#define E_MEM		-2
#define E_UNKNOWN	-3
#define E_SUPPORT	-4

/*****************************************************************************
*
*   Board interface definition
*
*****************************************************************************/

typedef struct
{
	UINT32  Size;						// sizeof(BOARD_INTERFACE)
	UINT32  BoardVendorId;				// board module version
	UINT32  BoardProductId;				// product identification
	UINT32	BoardRevisionNum;			// revision number
	IBOARD_BASIC *pIBoardBasic;			// interface pointer to IBOARD_BASIC
	IBOARD_VIDEO *pIBoardVideo;			// interface pointer to IBOARD_VIDEO
	IBOARD_AUDIO *pIBoardAudio;			// interface pointer to IBOARD_AUDIO
	IBOARD_XBAR* pIBoardXBar;			//Interface pointer to  IBOARD_XBAR
	IBOARD_TUNER *pIBoardTuner;			// interface pointer to IBOARD_TUNER
} BOARD_INTERFACE;

extern ACCESS_FUNCTIONS* g_pAF;


void BOARD_API GetBoardModuleCaps(
			/*in*/ AVSTREAM_COOKIE Cookie,					//Board module should pass the Cookie while call any service API 
															//in AVStream framework
			/*in*/	ACCESS_FUNCTIONS *pAF,					// the access function passed to board module
			/*out*/	BOARD_INTERFACE ***BoardInterfaces,		// the board interface pointer arrays returned for each board supported
			/*out*/	UINT32 *BoardNum			
			);						// how many board that the board module supports


#ifdef __cplusplus
}
#endif

#endif

/********************************** end of Board.h ***************************/
