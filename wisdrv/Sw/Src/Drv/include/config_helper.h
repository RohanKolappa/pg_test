/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        config_helper.h
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


#ifndef __CONFIG_HELPER_H__
#define __CONFIG_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DRIVER

	#define ALLOC_MEM(Size) PIAllocMem(Size)
	#define FREE_MEM(p) PIFreeMem(p)
#else

	#define ALLOC_MEM(Size) malloc(Size)
	#define FREE_MEM(p) free(p)

#endif


///////////////////////////////////////////////////////////////////////////////
// confiuration helper
///////////////////////////////////////////////////////////////////////////////

unsigned char IsFlagsSet(unsigned long test_flags, unsigned long mandetory_flags);

unsigned char IsCompatibleWithCurrentTVStandard(TV_STANDARD tv_standard, unsigned long standards_mask);

//enum CONFIG_ERROR Config_EnumVideoConfigurations(_VIDEO_CAPABILITIES* caps, unsigned long system, unsigned long stream);
//enum CONFIG_ERROR Config_EnumVideoConfigurationsEx(_VIDEO_CAPABILITIES* caps, unsigned long system, unsigned long stream);
enum CONFIG_ERROR Config_EnumVideoConfigurationsExx(_VIDEO_CAPABILITIES* caps, unsigned long system);
enum CONFIG_ERROR Config_IsAllowedByAssociations(_VIDEO_CAPABILITIES* caps, TCFGASSOCIATION* assc);

long FindStreamConfiguration(_VIDEO_CAPABILITIES* caps, TCFGSTREAM* stream);
long FindFrameRateConfiguration(_VIDEO_CAPABILITIES* caps, TCFGFRAMERATE* framerate);
long FindResolutionConfiguration(_VIDEO_CAPABILITIES* caps, TCFGRESOLUTION* resolution);
//long FindBitRateConfiguration(_VIDEO_CAPABILITIES* caps, TCFGBRCTRL* bitrate);

unsigned long FormatGOStreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader);

unsigned long FormatMPEG1StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader);

unsigned long FormatMPEG2StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader);

unsigned long FormatMPEG4StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader);


#ifdef __cplusplus
}
#endif

#endif

/****************************** end of config_helper.h ***********************/
