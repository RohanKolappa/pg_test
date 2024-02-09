/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        videocap.c
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

#include "videocap.h"

///////////////////////////////////////////////////////////////////////////////
// stream settings
///////////////////////////////////////////////////////////////////////////////

#define STREAM_DIVX_MPEG4			0
#define STREAM_MICROSOFT_MPEG4		1
#define STREAM_MPEG2				2
#define STREAM_MPEG1				3
#define STREAM_H263					4
#define STREAM_MJPG					5
#define STREAM_FCC_MPEG4			6

TCFGSTREAM GO7007SB_VideoStreams[] = 
{
	{
		{	// TCFG_HEADER
			"DIVX MPEG4",			// name
			"DIVX MPEG4",			// desc
			FLAGS_STREAM_MPEG4_MANDETORY,
			sizeof(TCFGSTREAM)
		},

		MPEG4,							// compress_mode
		IPB,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		DIVX_MPEG4,						// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		128,							// search_range
		1,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"MICROSOFT MPEG4",			// name
			"MICROSOFT MPEG4",			// desc
			FLAGS_STREAM_MPEG4_MANDETORY,
			sizeof(TCFGSTREAM)
		},

		MPEG4,							// compress_mode
		IPONLY,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		MICROSOFT_MPEG4,				// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		64,								// search_range
		0,								// gop_head_enable
		0,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"MPEG2",			// name
			"MPEG2",			// desc
			FLAGS_STREAM_COMPRESS_MODE,
			sizeof(TCFGSTREAM)
		},

		MPEG2,							// compress_mode
		IPB,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		0,								// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		128,							// search_range
		1,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"MPEG1",			// name
			"MPEG1",			// desc
			FLAGS_STREAM_COMPRESS_MODE,
			sizeof(TCFGSTREAM)
		},

		MPEG1,							// compress_mode
		IPB,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		0,								// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		128,							// search_range
		1,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"H263",			// name
			"H263",			// desc
			FLAGS_STREAM_COMPRESS_MODE,
			sizeof(TCFGSTREAM)
		},

		H263,							// compress_mode
		IPONLY,								// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		0,								// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		64,								// search_range
		1,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"MJPEG",			// name
			"MJPEG",			// desc
			FLAGS_STREAM_COMPRESS_MODE,
			sizeof(TCFGSTREAM)
		},

		MOTIONJPEG,						// compress_mode
		IONLY,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		0,								// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		64,								// search_range
		1,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"RCC MPEG4",			// name
			"RCC MPEG4",			// desc
			FLAGS_STREAM_MPEG4_MANDETORY,
			sizeof(TCFGSTREAM)
		},

		MPEG4,							// compress_mode
		IPONLY,							// sequence		

		0,								// gop_mode			
		30,								// gop_size			

		RCC_MPEG4,						// mpeg4_mode
		0,								// DVD_compliant
		1,								// deinterlace_mode

		128,							// search_range
		0,								// gop_head_enable
		1,								// seq_head_enable
		1,								// aspect_ratio

		0								// reserved;
	},
};

///////////////////////////////////////////////////////////////////////////////
// frame rate settings
///////////////////////////////////////////////////////////////////////////////

#define FRAMERATE_NTSC_30			0
#define FRAMERATE_NTSC_IVTC			1
#define FRAMERATE_NTSC_15			2
#define FRAMERATE_NTSC_10			3
#define FRAMERATE_NTSC_5			4
#define FRAMERATE_PAL_25			5
#define FRAMERATE_PAL_12_5			6
#define FRAMERATE_PAL_8_3			7
#define FRAMERATE_NONE_30			8
#define FRAMERATE_SECAM_25			9

TCFGFRAMERATE GO7007SB_VideoFrameRates[] = 
{
	{
		{	// TCFG_HEADER
			"NTSC 29.97fps",			// name
			"NTSC 29.97fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_NTSC_Mask,			// tv_standard
		30000,							// frame_rate
		0,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"NTSC IVTC",			// name
			"NTSC IVTC",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_NTSC_Mask,			// tv_standard
		24000,							// frame_rate
		0,								// drop_frame
		1,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"NTSC 15fps",			// name
			"NTSC 15fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_NTSC_Mask,			// tv_standard
		15000,							// frame_rate
		1,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"NTSC 10fps",			// name
			"NTSC 10fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_NTSC_Mask,			// tv_standard
		10000,							// frame_rate
		2,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"NTSC 5fps",			// name
			"NTSC 5fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_NTSC_Mask,			// tv_standard
		5000,							// frame_rate
		5,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"PAL 25fps",			// name
			"PAL 25fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_PAL_Mask,			// tv_standard
		25025,							// frame_rate
		0,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"PAL 12.5fps",			// name
			"PAL 12.5fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_PAL_Mask,			// tv_standard
		12513,							// frame_rate
		1,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"PAL 8.3fps",			// name
			"PAL 8.3fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_PAL_Mask,			// tv_standard
		8341,							// frame_rate
		2,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"30 fps",			// name
			"30 fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_None,				// tv_standard
		30000,							// frame_rate
		0,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

	{
		{	// TCFG_HEADER
			"SECAM 25fps",			// name
			"SECAM 25fps",			// desc
			FLAGS_FRAMERATE_MANDETORY,
			sizeof(TCFGFRAMERATE)
		},
		TVStandard_SECAM_Mask,			// tv_standard
		25025,							// frame_rate
		0,								// drop_frame
		0,								// ivtc_enable
		0								// reserved;
	},

};

///////////////////////////////////////////////////////////////////////////////
// resolution settings
///////////////////////////////////////////////////////////////////////////////

#define RESOLUTION_NTSC_SD			0
#define RESOLUTION_NTSC_SIF			1
#define RESOLUTION_PAL_SD			2
#define RESOLUTION_VGA				3
#define RESOLUTION_QVGA				4
#define RESOLUTION_NTSC_VGA			5
#define RESOLUTION_NTSC_QVGA		6
#define RESOLUTION_PAL_CIF			7
#define RESOLUTION_NTSC_CIF			8
#define RESOLUTION_PAL_SVCD			9
#define RESOLUTION_NTSC_SVCD		10
#define RESOLUTION_PAL_QCIF			11
#define RESOLUTION_NTSC_QCIF		12
#define RESOLUTION_SECAM_SD			13
#define RESOLUTION_NTSC_HALFSD		14
#define RESOLUTION_PAL_HALFSD		15

TCFGRESOLUTION GO7007SB_VideoResolutions[] = 
{
	{
		{	// TCFG_HEADER
			"NTSC SD",			// name
			"NTSC SD",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		720,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC SIF",			// name
			"NTSC SIF",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		352,						// width
		240,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		8,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		2000000,					// recommended maximum bitrate
		20000					// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"PAL SD",			// name
			"PAL SD",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_PAL_Mask,		// tv_standard
		720,						// width
		576,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"VGA",			// name
			"VGA",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_None,			// tv_standard
		640,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		12000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"QVGA",			// name
			"QVGA",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_None,			// tv_standard
		320,						// width
		240,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		4000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC VGA",			// name
			"NTSC VGA",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		640,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		40,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC QVGA",			// name
			"NTSC QVGA",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		320,						// width
		240,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		2000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"PAL CIF",			// name
			"PAL CIF",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_PAL_Mask,		// tv_standard
		352,						// width
		288,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		2000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC CIF",			// name
			"NTSC CIF",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		352,						// width
		288,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		2000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC SVCD 480*480",			// name
			"NTSC SVCD 480*480",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		480,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"PAL SVCD 480*480",			// name
			"PAL SVCD 480*480",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_PAL_Mask,		// tv_standard
		480,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"PAL QCIF",			// name
			"PAL QCIF",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_PAL_Mask,		// tv_standard
		176,						// width
		144,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		1000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC QCIF",			// name
			"NTSC QCIF",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		176,						// width
		144,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		20,							// h_sub_offset
		0,							// v_sub_offset

		1,							// h_scale_enb
		1,							// v_scale_enb
		0,							// sub_sample	

		1000000,					// recommended maximum bitrate
		20000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"SECAM SD",			// name
			"SECAM SD",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_SECAM_Mask,		// tv_standard
		720,						// width
		576,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"NTSC HALF SD",			// name
			"NTSC HALF SD",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_NTSC_Mask,		// tv_standard
		352,						// width
		480,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

	{
		{	// TCFG_HEADER
			"PAL HALF SD",			// name
			"PAL HALF SD",			// desc
			FLAGS_RESOLUTION_MANDETORY,
			sizeof(TCFGRESOLUTION)
		},

		TVStandard_PAL_Mask,		// tv_standard
		352,						// width
		576,						// height

		1,							// h_sub_window
		1,							// v_sub_window
		0,							// h_sub_offset
		0,							// v_sub_offset

		0,							// h_scale_enb
		0,							// v_scale_enb
		0,							// sub_sample	

		15000000,					// recommended maximum bitrate
		300000						// recommended minimum bitrate
	},

};

TCFGASSOCIATION GO7007SB_VideoAssociations[] = 
{
	// frame rate <--> resolution
	{ TYPE_FRAMERATE_CONFIG, FRAMERATE_NTSC_IVTC, TYPE_RESOLUTION_CONFIG, RESOLUTION_NTSC_SD, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_FRAMERATE_CONFIG, FRAMERATE_NTSC_IVTC, TYPE_RESOLUTION_CONFIG, ASSOCIATION_ALL, ASSOCIATION_TYPE_DENY},

	// stream <--> resolution
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_NTSC_CIF, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_PAL_CIF, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_NTSC_QCIF, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_PAL_QCIF, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_VGA, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, RESOLUTION_NTSC_VGA, ASSOCIATION_TYPE_ALLOW},
	{ TYPE_STREAM_CONFIG, STREAM_H263, TYPE_RESOLUTION_CONFIG, ASSOCIATION_ALL, ASSOCIATION_TYPE_DENY},
	{ TYPE_STREAM_CONFIG, ASSOCIATION_ALL, TYPE_RESOLUTION_CONFIG, RESOLUTION_NTSC_CIF, ASSOCIATION_TYPE_DENY},
};

unsigned long num_of_streams = sizeof(GO7007SB_VideoStreams)/sizeof(TCFGSTREAM);
unsigned long num_of_framerates = sizeof(GO7007SB_VideoFrameRates)/sizeof(TCFGFRAMERATE);
unsigned long num_of_resolutions = sizeof(GO7007SB_VideoResolutions)/sizeof(TCFGRESOLUTION);
unsigned long num_of_associations = sizeof(GO7007SB_VideoAssociations)/sizeof(TCFGASSOCIATION);

/****************************** end of videocap.c ****************************/
