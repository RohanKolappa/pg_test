/******************************************************************************
*
*    Copyright WIS Technologies (c) (2004)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        PacGen_7007SB.c
*
*    DESCRIPTION:
*        This file generates the GO7007SB chip initialization packages
*
*
*    AUTHOR:
*
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#ifndef _SDK_PACGEN_
	#include "s7007u.h"
	#include "config.h"
	#include "global.h"
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include "Struct.h"
	#include "platform.h"
	#include "PacGen_7007SB.h"

#if defined(_WINDOWS)
	#include <string.h>
#elif defined(_LINUX)
	#include <linux/string.h>
#endif
	
	#define malloc	PIAllocMem
	#define free	PIFreeMem
#endif

#ifndef _EXTRA_BRC_DEBUG
#define	_EXTRA_BRC_DEBUG	1
#endif

#define TOP_FIELD		1
#define BOTTOM_FIELD	2
#define FRAME_STRUCT	3

#define DupFrame_size	5120
#define MJPEG_HDR_Addr	0x1900
#define DUP_FRM_Addr	0x1900
#define DUP_MP4_Addr_P	0xaf900
#define DUP_MP4_Addr_B	0x14f900
#define P_Frame_Addr	0x1900
#define Bf_Frame_Addr	0x195C
#define Bb_Frame_Addr	0x1AF4
#define Bi_Frame_Addr	0x1C5A
#define Be_Frame_Addr	0x1DF2
#define SeqHeader_Addr	0x1DF6
#define dupPfrm_size	368		// 92DW
#define dupBffrm_size	1632	// 408DW
#define dupBbfrm_size	1432	// 358DW
#define dupBifrm_size	1632	// 408DW
#define dupBefrm_size	16		// 4DW
#define seqHdr_size		40		// 10DW

const UINT8 default_intra_quant_table[64] = {
	 8, 16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
	19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
	22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
	26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83
};

const SINT32 addrinctab[33][2]= {
  {0x01,1},  {0x03,3},  {0x02,3},  {0x03,4},
  {0x02,4},  {0x03,5},  {0x02,5},  {0x07,7},
  {0x06,7},  {0x0b,8},  {0x0a,8},  {0x09,8},
  {0x08,8},  {0x07,8},  {0x06,8},  {0x17,10},
  {0x16,10}, {0x15,10}, {0x14,10}, {0x13,10},
  {0x12,10}, {0x23,11}, {0x22,11}, {0x21,11},
  {0x20,11}, {0x1f,11}, {0x1e,11}, {0x1d,11},
  {0x1c,11}, {0x1b,11}, {0x1a,11}, {0x19,11},
  {0x18,11}
};

// Set up the standard Huffman tables (cf. JPEG standard section K.3) 
// IMPORTANT: these are only valid for 8-bit data precision! 
static const UINT8 bits_dc_luminance[17] = 
{ // 0-base  
	0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 
};

static const UINT8 val_dc_luminance[] =
{ 
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};

static const UINT8 bits_dc_chrominance[17] =
{ // 0-base  
	0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 
};

static const UINT8 val_dc_chrominance[] =
{ 
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};

static const UINT8 bits_ac_luminance[17] =
{ // 0-base  
	0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d 
};

static const UINT8 val_ac_luminance[] =
{ 
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
};

static const UINT8 bits_ac_chrominance[17] =
{ // 0-base  
	0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 
};

static const UINT8 val_ac_chrominance[] =
{ 
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
};

#define ISSTMAX 88

static const int imaStepSizeTable[ISSTMAX+1] = {
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34,
	37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
	157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
	544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1141, 1552,
	1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
	4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
	27086, 29794, 32767 };

UINT8	huff_size_dc_luminance[12];
UINT16	huff_code_dc_luminance[12];
UINT8	huff_size_dc_chrominance[12];
UINT16	huff_code_dc_chrominance[12];
UINT8	huff_size_ac_luminance[256];
UINT16	huff_code_ac_luminance[256];
UINT8	huff_size_ac_chrominance[256];
UINT16	huff_code_ac_chrominance[256];
UINT8	intra_q[64];
SINT32	dc_dct_pred[3];

typedef struct {
	SINT32 count;
	UINT32 reg;
	UINT8  *io;
	SINT32 index;
	UINT8  *codeStr;
} pMpgvlcManager;

SINT32 num[128]; 
UINT32 code[128];
pMpgvlcManager vlc;

#define	NUM_MOTION_DETECTION_REGIONS	4
#define	MAX_MACROBLOCKS_PER_PICTURE		1620 // Full D1 is 720x576
#define	NUM_MACROBLOCKS_PER_WORD		8
#define	NUM_WORDS_PER_PACKAGE			10

#define pcodeSet(code, N) ((code) << (N))

void pvlcInit(pMpgvlcManager *vlc, SINT32 out, UINT8 *codeStr) {
	if(out) {
		vlc->count = 0; vlc->reg = 0;
#ifdef _BIG_ENDIAN_ //#ifdef SPARC
		vlc->io = ((UINT8*)&(vlc->reg));
#else
		vlc->io = ((UINT8*)&(vlc->reg)) + 3;
#endif
	}
	else {
		vlc->count = 24;
		vlc->io = ((UINT8*)&(vlc->reg));
#ifdef _BIG_ENDIAN_ //#ifdef SPARC
		vlc->io[3] = codeStr[2]; vlc->io[2] = codeStr[1]; vlc->io[1] = codeStr[0]; codeStr += 3;
#else
		vlc->io[0] = codeStr[2]; vlc->io[1] = codeStr[1]; vlc->io[2] = codeStr[0]; codeStr += 3;
#endif
	}
	if(codeStr) { vlc->index = 0; vlc->codeStr = codeStr; }
}

void pvlcSave(pMpgvlcManager *vlc, SINT32 num, UINT32 code) {
	vlc->count += num; vlc->reg |= code << (32 - vlc->count);
	while(vlc->count >= 8)
	{ *vlc->codeStr ++ = *vlc->io; vlc->reg <<= 8; vlc->count -= 8; vlc->index += 8; }
}

SINT32 coding(SINT32 cnt) {
	SINT32 i;
	for(i = 0; i < cnt; i ++) pvlcSave(&vlc, num[i], code[i]);
	return vlc.index+vlc.count;
}

SINT32 align(UINT32 code) {
	if (vlc.count != 0) pvlcSave(&vlc, 8 - vlc.count, code);
	return vlc.index;
}

SINT32 nextstartcode(void) {
	SINT32 num = 8 - vlc.count;
	pvlcSave(&vlc, num, (1 << num) - 1 - (1 << (num - 1)));
	return vlc.index;
}

#define FDUP_SELF 0
//#define FDUP_PREV 1
//#define FDUP_POST 2
//#define FDUP_BIDIR 3
//#define QMIN 1
//#define QMAX 31
#define	FRMSInSGOP	(sgop + 1)
#define	LET(x, param, dft)	if((param) != FP_KEEP) x = ((param) == FP_AUTO) ? (dft) : (param);
#define	PEAKRATE_DVD	9800000
#define	VBV_DVD			(112 * 16 * 1024)
#define OSDFONT_IP_DRAM_ADDR		0x0a0100
#define	OSDFONT_IPB_DRAM_ADDR		0x140100
#define ASPECT_RATIO_EXTENEED_PAR	0xF

SINT32 vop_time_increment_mask = 0xFFFF;
SINT32 vop_time_increment_bitlength = 16;

//#define	RCLAMBDA_CFR	1.
//const REAL64 RC_COMPLEX = 2., RC_Q0 = 4., RC_Q1 = 24., RC_DQ = 100.;
const SINT16 RC_SGOP[4] = { 4, 5, 6, 8 };

const SINT16 converge_speed_table1[101] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,2,
	2,2,2,2,2,2,2,2,2,3,
	3,3,3,3,3,4,4,4,4,4,
	5,5,5,6,6,6,7,7,8,8,
	9,10,10,11,12,13,14,15,16,17,
	19,20,22,23,25,27,30,32,35,38,
	41,45,49,53,58,63,69,76,83,91,
	100
};

const SINT16 converge_speed_table3[101] = {
	3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,4,4,4,4,4,
	4,4,4,4,5,5,5,5,5,6,
	6,6,6,7,7,7,7,8,8,9,
	9,9,10,10,11,12,12,13,14,14,
	15,16,17,18,19,20,22,23,25,26,
	28,30,32,34,37,40,42,46,49,53,
	57,61,66,71,77,83,90,97,106,115,
	125,135,147,161,175,191,209,228,249,273,
	300
};

const SINT16 LAMBDA_table[4][101] = {
	{	16,16,16,16,17,17,17,18,18,18,
		19,19,19,20,20,20,21,21,22,22,
		22,23,23,24,24,25,25,25,26,26,
		27,27,28,28,29,29,30,31,31,32,
		32,33,33,34,35,35,36,37,37,38,
		39,39,40,41,42,42,43,44,45,46,
		46,47,48,49,50,51,52,53,54,55,
		56,57,58,59,60,61,62,63,64,65,
		67,68,69,70,72,73,74,76,77,78,
		80,81,83,84,86,87,89,90,92,94,
		96
	},
	{
		20,20,20,21,21,21,22,22,23,23,
		23,24,24,25,25,26,26,27,27,28,
		28,29,29,30,30,31,31,32,33,33,
		34,34,35,36,36,37,38,38,39,40,
		40,41,42,43,43,44,45,46,47,48,
		48,49,50,51,52,53,54,55,56,57,
		58,59,60,61,62,64,65,66,67,68,
		70,71,72,73,75,76,78,79,80,82,
		83,85,86,88,90,91,93,95,96,98,
		100,102,103,105,107,109,111,113,115,117,
		120
	},
	{
		24,24,24,25,25,26,26,27,27,28,
		28,29,29,30,30,31,31,32,33,33,
		34,34,35,36,36,37,38,38,39,40,
		41,41,42,43,44,44,45,46,47,48,
		49,50,50,51,52,53,54,55,56,57,
		58,59,60,62,63,64,65,66,67,69,
		70,71,72,74,75,76,78,79,81,82,
		84,85,87,88,90,92,93,95,97,98,
		100,102,104,106,108,110,112,114,116,118,
		120,122,124,127,129,131,134,136,138,141,
		144
	},
	{
		32,32,33,33,34,34,35,36,36,37,
		38,38,39,40,41,41,42,43,44,44,
		45,46,47,48,49,50,50,51,52,53,
		54,55,56,57,58,59,60,62,63,64,
		65,66,67,69,70,71,72,74,75,76,
		78,79,81,82,84,85,87,88,90,92,
		93,95,97,98,100,102,104,106,108,110,
		112,114,116,118,120,122,124,127,129,131,
		134,136,139,141,144,146,149,152,154,157,
		160,163,166,169,172,175,178,181,185,188,
		192
	}
};

#ifdef _SDK_PACGEN_

static SINT32 quantizer_scale_matrix[16] = {
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	0, 1, -1, 1, -1, 2, -1, 1, -2, 1, 1, -1, 1, -2, 1, -1
};

static SINT32 ACCoeff_threshold_matrix[16] = {
	0, 0, 1, 2, 1, 3, 2, 1, 0, 1, 2, 3, 1, 2, 0, 3
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const SINT32 DC_SCALER_Y[32]=
{	8,	8,	8,	8,	8,	10,	12,	14,
	16,	17,	18,	19,	20,	21,	22,	23,
	24,	25,	26,	27,	28,	29,	30,	31,
	32,	34,	36,	38,	40,	42,	44,	46
};

const SINT32 DC_SCALER_UV[32]=
{	8,	8,	8,	8,	8,	9,	9,	10,
	10,	11,	11,	12,	12,	13,	13,	14,
	14,	15,	15,	16,	16,	17,	17,	18,
	18,	19,	20,	21,	22,	23,	24,	25
};

SINT32 Filter_coef(SINT32 input) {
	if(input>=0) return input&0x0f;
	else return (abs(input)&0x0f)|0x10;
}

SINT32 config_checker(CHIP_CONFIG *chip_con, OSD_CONFIG *osd_con, AUDIOS_CONFIG *audio_con,
					  DEINTERLACE_CONFIG *deinterlace_con, IIP_CONFIG *iip_con, MOTION_DETECTION_CONFIG_t *md_con,
					  TCFGVIDEOEX* conf_sdk, TVIDEOCFGFIX* conf2_sdk, SINT8 *config_error,
					  SINT32 *width, SINT32 *height) {

	SINT32 error_cnt=0;
	SINT32 INPUT_WIDTH;
	SINT32 INPUT_HEIGHT;
	SINT32 index;
	strcpy(config_error,"");


	/************************* Begin configuration transfer *********************/

	{ // System level configuration
		INPUT_HEIGHT=conf_sdk->syscfg.sensor_v;
		INPUT_WIDTH=conf_sdk->syscfg.sensor_h;
		if(conf_sdk->syscfg.format==0) { // YUV progressive
			chip_con->cfg_iip_enable=0;
			chip_con->cfg_yuvj_rgb=0;
			chip_con->cfg_interlace=0;
			chip_con->cfg_deinterlace=0;
		}
		else if(conf_sdk->syscfg.format==1) { // YUV interlace
			chip_con->cfg_iip_enable=0;
			chip_con->cfg_yuvj_rgb=0;
			chip_con->cfg_interlace=1;
			chip_con->cfg_deinterlace=conf_sdk->strcfg.deinterlace_mode;
		}
		else { // RGB Bayer
			if(conf_sdk->misccfg.iip_enable) { // thru iip module
				chip_con->cfg_iip_enable=1;
				chip_con->cfg_yuvj_rgb=0;
			}
			else { // bypass iip module
				chip_con->cfg_iip_enable=0;
				chip_con->cfg_yuvj_rgb=1;
			}
			chip_con->cfg_interlace=0;
			chip_con->cfg_deinterlace=0;
		}
		{ // Video format
			if(conf_sdk->syscfg.tv_standard==TVStandard_None)
				chip_con->video_format = 0;
			else if((conf_sdk->syscfg.tv_standard&TVStandard_PAL_Mask)!=0)
				chip_con->video_format = 1;
			else if((conf_sdk->syscfg.tv_standard&TVStandard_NTSC_Mask)!=0)
				chip_con->video_format = 2;
			else if((conf_sdk->syscfg.tv_standard&TVStandard_SECAM_Mask)!=0)
				chip_con->video_format = 3;
			else
				chip_con->video_format = 0;
		}
		chip_con->cfg_bayer_pattern=conf_sdk->syscfg.pformat;
		chip_con->cfg_mode_656=conf_sdk->syscfg.sensor_656_mode;
		chip_con->cfg_sensor_desc = conf_sdk->syscfg.valid_polar +
									(conf_sdk->syscfg.href_polar<<1) +
									(conf_sdk->syscfg.vref_polar<<2) +
									(conf_sdk->syscfg.field_id_polar<<3) +
									(conf_sdk->syscfg.sensor_bit_width<<4) +
									(conf_sdk->syscfg.valid_enable<<5);
		chip_con->hv_resync_enable=conf_sdk->syscfg.hv_resync_enable;
	}

	{ // Stream level configuration
		chip_con->compress_mode=conf_sdk->strcfg.compress_mode;
		chip_con->seqMode=conf_sdk->strcfg.sequence;
		chip_con->nSubGOP=(chip_con->seqMode==3)? 3:1;
		chip_con->nGOP=conf_sdk->strcfg.gop_size / ((chip_con->seqMode==3)? 3:1);
		chip_con->cfg_four_channel=conf_sdk->misccfg.four_channel_enable;
		chip_con->is_closed_gop=conf_sdk->strcfg.gop_mode;
		chip_con->cfg_vbi_enable=conf_sdk->misccfg.vbi_enable;
		chip_con->aspect_ratio=conf_sdk->strcfg.aspect_ratio;
		chip_con->DVD_compliant=conf_sdk->strcfg.DVD_compliant;
		chip_con->search_range=conf_sdk->strcfg.search_range;
		chip_con->gop_header_enable=conf_sdk->strcfg.gop_head_enable;
		chip_con->seq_header_enable=conf_sdk->strcfg.seq_head_enable;
		chip_con->cfg_av_sync_enable=conf_sdk->misccfg.av_sync_enable;
		chip_con->myFOURCC=conf_sdk->strcfg.mpeg4_mode; //use myFOURCC to save mpeg4_mode in pacgen
		if(conf_sdk->misccfg.h_filter_mode==2) {
			chip_con->cfg_h_filter_enb=1;
			chip_con->cfg_h_filter_format=0;
		}
		else if(conf_sdk->misccfg.h_filter_mode==1) {
			chip_con->cfg_h_filter_enb=1;
			chip_con->cfg_h_filter_format=1;
		}
		else {
			chip_con->cfg_h_filter_enb=0;
			chip_con->cfg_h_filter_format=0;
		}
		if(conf_sdk->misccfg.v_filter_mode==2) {
			chip_con->cfg_v_filter_enb=1;
			chip_con->cfg_v_filter_format=0;
		}
		else if(conf_sdk->misccfg.v_filter_mode==1) {
			chip_con->cfg_v_filter_enb=1;
			chip_con->cfg_v_filter_format=1;
		}
		else {
			chip_con->cfg_v_filter_enb=0;
			chip_con->cfg_v_filter_format=0;
		}
		chip_con->cfg_filter_nAX=Filter_coef(conf_sdk->misccfg.filter_nAX);
		chip_con->cfg_filter_nBX=conf_sdk->misccfg.filter_nBX&0x1f;
		chip_con->cfg_filter_nCX=Filter_coef(conf_sdk->misccfg.filter_nCX);
		chip_con->cfg_filter_nAY=Filter_coef(conf_sdk->misccfg.filter_nAY);
		chip_con->cfg_filter_nBY=conf_sdk->misccfg.filter_nBY&0x1f;
		chip_con->cfg_filter_nCY=Filter_coef(conf_sdk->misccfg.filter_nCY);
	}

	{ // Resolution level configuration
		*width=conf_sdk->rescfg.width;
		*height=conf_sdk->rescfg.height;
		chip_con->cfg_x_sub_window=conf_sdk->rescfg.h_sub_window;
		chip_con->cfg_y_sub_window=conf_sdk->rescfg.v_sub_window;
		chip_con->cfg_x_sub_offset=conf_sdk->rescfg.h_sub_offset;
		chip_con->cfg_y_sub_offset=conf_sdk->rescfg.v_sub_offset;
		chip_con->cfg_x_scale_enb=conf_sdk->rescfg.h_scale_enb;
		chip_con->cfg_y_scale_enb=conf_sdk->rescfg.v_scale_enb;
		chip_con->cfg_x_sub_count=(1+chip_con->cfg_x_scale_enb)*(*width);
		chip_con->cfg_y_sub_count=(1+chip_con->cfg_y_scale_enb)*(*height);
		chip_con->cfg_sub_sample=conf_sdk->rescfg.sub_sample;
	}

	{ // Frame rate level configuration
		chip_con->Frame_rate=conf_sdk->fpscfg.frame_rate;
		chip_con->cfg_drop_frame=conf_sdk->fpscfg.drop_frame;
		chip_con->cfg_ivtc_enable=conf_sdk->fpscfg.ivtc_enable;
		if(chip_con->cfg_ivtc_enable==1)
			chip_con->Frame_rate = chip_con->Frame_rate*4/5;
	}

	{ // BRC level configuration
		chip_con->target_bitrate=conf_sdk->ctlcfg.target_bitrate;
		chip_con->brctrl_peak_rate=conf_sdk->ctlcfg.peak_bitrate;
		chip_con->brctrl_vbv=conf_sdk->ctlcfg.vbv_buffer;
		chip_con->brctrl_converge_speed=conf_sdk->ctlcfg.converge_speed;
		chip_con->brctrl_LAMBDA=conf_sdk->ctlcfg.lambda;
		chip_con->brctrl_Q=conf_sdk->ctlcfg.Q;
		chip_con->iniQScale=conf_sdk->ctlcfg.IQ;
		chip_con->iniPQScale=conf_sdk->ctlcfg.PQ;
		chip_con->iniBQScale=conf_sdk->ctlcfg.BQ;
		chip_con->brctrl_qmin = (conf_sdk->ctlcfg.reserved & 0x0000007F);
		chip_con->brctrl_dup_enable = (conf_sdk->ctlcfg.reserved & 0x00000080)>>7;
		chip_con->brctrl_qmax = (conf_sdk->ctlcfg.reserved & 0x00007F00)>>8;
		chip_con->brctrl_mb_enable = (conf_sdk->ctlcfg.reserved & 0x00008000)>>15;
		chip_con->brctrl_window_size = (conf_sdk->ctlcfg.reserved & 0xFFFF0000)>>17;
//		chip_con->brctrl_window_size *= 60/2*(chip_con->Frame_rate/1000); // half window_size
		chip_con->do_bitrate_control=(chip_con->brctrl_Q>0 || chip_con->target_bitrate>0)? 1:0;
	}

	{ // OSD, TH, DEINTERLACE, ELSE, AUDIO, IIP and MD configuration from conf2_sdk
		// OSD_CONFIG
		osd_con->DoOSD=conf2_sdk->osdcfg.DoOSD;
		osd_con->OSDyc0=conf2_sdk->osdcfg.OSDyc0;
		osd_con->OSDyc1=conf2_sdk->osdcfg.OSDyc1;
		osd_con->OSDuc0=conf2_sdk->osdcfg.OSDuc0;
		osd_con->OSDuc1=conf2_sdk->osdcfg.OSDuc1;
		osd_con->OSDvc0=conf2_sdk->osdcfg.OSDvc0;
		osd_con->OSDvc1=conf2_sdk->osdcfg.OSDvc1;
		// TH_CONFIG
		deinterlace_con->THedge=conf2_sdk->thcfg.THedge;
		deinterlace_con->THmotion=conf2_sdk->thcfg.THmotion;
		deinterlace_con->THblending=conf2_sdk->thcfg.THblending;
		deinterlace_con->THbigedge=conf2_sdk->thcfg.THbigedge;
		deinterlace_con->THsmalledge=conf2_sdk->thcfg.THsmalledge;
		deinterlace_con->THedgestatistics=conf2_sdk->thcfg.THedgestatistics;
		deinterlace_con->THmotionstatistics=conf2_sdk->thcfg.THmotionstatistics;
		deinterlace_con->THbigedgestatistics=conf2_sdk->thcfg.THbigedgestatistics;
		deinterlace_con->THsmalledgestatistics=conf2_sdk->thcfg.THsmalledgestatistics;
		chip_con->THACCoeffSet0=conf2_sdk->thcfg.THACCoeffSet0;
		chip_con->THACCoeffStartpoint=conf2_sdk->thcfg.THACCoeffStartpoint;
		// DEINTERLACE_CONFIG
		deinterlace_con->DeInter_Always_Motion=conf2_sdk->deintercfg.DeInter_Always_Motion;
		deinterlace_con->DeInter_Always_Blending=conf2_sdk->deintercfg.DeInter_Always_Blending;
		deinterlace_con->DeInter_Always_Weave=conf2_sdk->deintercfg.DeInter_Always_Weave;
		if(chip_con->cfg_ivtc_enable==1) {
			deinterlace_con->DeInter_Always_Blending=0;
			deinterlace_con->DeInter_Always_Weave=1;
		}
		// ELSE_CONFIG
		chip_con->HPIBufferCapacity=conf2_sdk->elsecfg.HPIBufferCapacity;
		chip_con->clock_rate=conf2_sdk->elsecfg.clockrate;
		chip_con->dramType=conf2_sdk->elsecfg.DRAM;
		chip_con->ivtc_holding_time=conf2_sdk->elsecfg.ivtc_holding_time;
		chip_con->has_audio_package=conf2_sdk->elsecfg.HasAudio;
		chip_con->v_sync_bitplane=conf2_sdk->elsecfg.v_sync_bitplane;
		// only if the last line of video signal is included, set VSYNC_ENABLE_BIT (bit[15])
		if(chip_con->v_sync_bitplane & 0x8000) {
			if(!(conf_sdk->rescfg.height==conf_sdk->syscfg.sensor_v ||
				(conf_sdk->rescfg.height==conf_sdk->syscfg.sensor_v/2 && conf_sdk->rescfg.v_scale_enb==1)))
				chip_con->v_sync_bitplane=(chip_con->v_sync_bitplane & 0x7FFF);
		}
		// only if interlace source and deinterlace!=0, set FID_DET_ENABLE_BIT (bit[14])
		if(chip_con->v_sync_bitplane & 0x4000) {
			if(!(conf_sdk->syscfg.format==1 && conf_sdk->strcfg.deinterlace_mode!=0))
				chip_con->v_sync_bitplane=(chip_con->v_sync_bitplane & 0xBFFF);
		}
		// AUDIO_CONFIG
		audio_con->main_divider = conf2_sdk->audiocfg.main_divider;
		audio_con->BCLK_divider = conf2_sdk->audiocfg.BCLK_divider;
		audio_con->OKI_mode = conf2_sdk->audiocfg.OKI_mode;
		audio_con->OKI_shortframe_mode_clock_gen = conf2_sdk->audiocfg.OKI_short_frame_mode;
		audio_con->clock_generator_enable = conf2_sdk->audiocfg.clock_generator_enable;
		audio_con->I2S_master_mode = conf2_sdk->audiocfg.I2S_master_mode;
		audio_con->I2S_mode = conf2_sdk->audiocfg.I2S_mode;
		audio_con->invert_phase_BCLK = conf2_sdk->audiocfg.invert_phase_BCLK;
		audio_con->OKI_shortframe_mode_input_config = conf2_sdk->audiocfg.OKI_shoftframe_mode;
		audio_con->word_length = conf2_sdk->audiocfg.word_length;
		audio_con->auto_SOF_generation = conf2_sdk->audiocfg.auto_SOF_generation;
		audio_con->left_channel_only = conf2_sdk->audiocfg.left_channel_only;
		audio_con->bit8_filter = conf2_sdk->audiocfg.higher_8_bit_combine;
		audio_con->AC97_enable = conf2_sdk->audiocfg.AC97_enable;
		audio_con->audio_signature_bit = conf2_sdk->audiocfg.signature_bit_selection;
		audio_con->buffer_config = conf2_sdk->audiocfg.buffer_config;
		audio_con->sample_mapping_reg1 = conf2_sdk->audiocfg.sample_mapping_reg1;
		audio_con->sample_mapping_reg2 = conf2_sdk->audiocfg.sample_mapping_reg2;
		audio_con->sample_mapping_reg3 = conf2_sdk->audiocfg.sample_mapping_reg3;
		audio_con->sample_mapping_reg4 = conf2_sdk->audiocfg.sample_mapping_reg4;
		audio_con->sample_mapping_reg5 = conf2_sdk->audiocfg.sample_mapping_reg5;
		audio_con->sample_mapping_reg6 = conf2_sdk->audiocfg.sample_mapping_reg6;
		audio_con->sample_mapping_reg7 = conf2_sdk->audiocfg.sample_mapping_reg7;
		audio_con->adpcm_enable = conf2_sdk->audiocfg.adpcm_enable;
		audio_con->adpcm_mode = conf2_sdk->audiocfg.adpcm_mode;
		audio_con->bytes_per_block = conf2_sdk->audiocfg.bytes_per_block;
		audio_con->predictor = conf2_sdk->audiocfg.bpredictor;
		audio_con->delta_left = conf2_sdk->audiocfg.idelta_left;
		audio_con->delta_right = conf2_sdk->audiocfg.idelta_right;
		audio_con->coef1_left = conf2_sdk->audiocfg.icoef1_left;
		audio_con->coef1_right = conf2_sdk->audiocfg.icoef1_right;
		audio_con->coef2_left = conf2_sdk->audiocfg.icoef2_left;
		audio_con->coef2_right = conf2_sdk->audiocfg.icoef2_right;
		// get audio SampleRate
		audio_con->reserved = conf2_sdk->audiocfg.reserved;
		// IIP CONFIG
		iip_con->Bit_Length = conf2_sdk->iipcfg.Bit_Length;
		iip_con->CCM_R_0 = conf2_sdk->iipcfg.CCM_R_0;
		iip_con->CCM_R_1 = conf2_sdk->iipcfg.CCM_R_1;
		iip_con->CCM_R_2 = conf2_sdk->iipcfg.CCM_R_2;
		iip_con->CCM_R_3 = conf2_sdk->iipcfg.CCM_R_3;
		iip_con->CCM_G_0 = conf2_sdk->iipcfg.CCM_G_0;
		iip_con->CCM_G_1 = conf2_sdk->iipcfg.CCM_G_1;
		iip_con->CCM_G_2 = conf2_sdk->iipcfg.CCM_G_2;
		iip_con->CCM_G_3 = conf2_sdk->iipcfg.CCM_G_3;
		iip_con->CCM_B_0 = conf2_sdk->iipcfg.CCM_B_0;
		iip_con->CCM_B_1 = conf2_sdk->iipcfg.CCM_B_1;
		iip_con->CCM_B_2 = conf2_sdk->iipcfg.CCM_B_2;
		iip_con->CCM_B_3 = conf2_sdk->iipcfg.CCM_B_3;
		iip_con->R_Gain = conf2_sdk->iipcfg.R_Gain;
		iip_con->G_Gain = conf2_sdk->iipcfg.G_Gain;
		iip_con->B_Gain = conf2_sdk->iipcfg.B_Gain;
		iip_con->R_Offset = conf2_sdk->iipcfg.R_Offset;
		iip_con->G_Offset = conf2_sdk->iipcfg.G_Offset;
		iip_con->B_Offset = conf2_sdk->iipcfg.B_Offset;
		iip_con->Gamma_R_P0_C = conf2_sdk->iipcfg.Gamma_R_P0_C;
		iip_con->Gamma_R_P0_Y = conf2_sdk->iipcfg.Gamma_R_P0_Y;
		iip_con->Gamma_R_P1_C = conf2_sdk->iipcfg.Gamma_R_P1_C;
		iip_con->Gamma_R_P1_Y = conf2_sdk->iipcfg.Gamma_R_P1_Y;
		iip_con->Gamma_R_P2_C = conf2_sdk->iipcfg.Gamma_R_P2_C;
		iip_con->Gamma_R_P2_Y = conf2_sdk->iipcfg.Gamma_R_P2_Y;
		iip_con->Gamma_R_P3_C = conf2_sdk->iipcfg.Gamma_R_P3_C;
		iip_con->Gamma_R_P3_Y = conf2_sdk->iipcfg.Gamma_R_P3_Y;
		iip_con->Gamma_R_P4_C = conf2_sdk->iipcfg.Gamma_R_P4_C;
		iip_con->Gamma_R_P4_Y = conf2_sdk->iipcfg.Gamma_R_P4_Y;
		iip_con->Gamma_R_P5_C = conf2_sdk->iipcfg.Gamma_R_P5_C;
		iip_con->Gamma_R_P5_Y = conf2_sdk->iipcfg.Gamma_R_P5_Y;
		iip_con->Gamma_R_P6_C = conf2_sdk->iipcfg.Gamma_R_P6_C;
		iip_con->Gamma_R_P6_Y = conf2_sdk->iipcfg.Gamma_R_P6_Y;
		iip_con->Gamma_R_P7_C = conf2_sdk->iipcfg.Gamma_R_P7_C;
		iip_con->Gamma_R_P7_Y = conf2_sdk->iipcfg.Gamma_R_P7_Y;
		iip_con->Gamma_R_P8_C = conf2_sdk->iipcfg.Gamma_R_P8_C;
		iip_con->Gamma_R_P8_Y = conf2_sdk->iipcfg.Gamma_R_P8_Y;
		iip_con->Gamma_G_P0_C = conf2_sdk->iipcfg.Gamma_G_P0_C;
		iip_con->Gamma_G_P0_Y = conf2_sdk->iipcfg.Gamma_G_P0_Y;
		iip_con->Gamma_G_P1_C = conf2_sdk->iipcfg.Gamma_G_P1_C;
		iip_con->Gamma_G_P1_Y = conf2_sdk->iipcfg.Gamma_G_P1_Y;
		iip_con->Gamma_G_P2_C = conf2_sdk->iipcfg.Gamma_G_P2_C;
		iip_con->Gamma_G_P2_Y = conf2_sdk->iipcfg.Gamma_G_P2_Y;
		iip_con->Gamma_G_P3_C = conf2_sdk->iipcfg.Gamma_G_P3_C;
		iip_con->Gamma_G_P3_Y = conf2_sdk->iipcfg.Gamma_G_P3_Y;
		iip_con->Gamma_G_P4_C = conf2_sdk->iipcfg.Gamma_G_P4_C;
		iip_con->Gamma_G_P4_Y = conf2_sdk->iipcfg.Gamma_G_P4_Y;
		iip_con->Gamma_G_P5_C = conf2_sdk->iipcfg.Gamma_G_P5_C;
		iip_con->Gamma_G_P5_Y = conf2_sdk->iipcfg.Gamma_G_P5_Y;
		iip_con->Gamma_G_P6_C = conf2_sdk->iipcfg.Gamma_G_P6_C;
		iip_con->Gamma_G_P6_Y = conf2_sdk->iipcfg.Gamma_G_P6_Y;
		iip_con->Gamma_G_P7_C = conf2_sdk->iipcfg.Gamma_G_P7_C;
		iip_con->Gamma_G_P7_Y = conf2_sdk->iipcfg.Gamma_G_P7_Y;
		iip_con->Gamma_G_P8_C = conf2_sdk->iipcfg.Gamma_G_P8_C;
		iip_con->Gamma_G_P8_Y = conf2_sdk->iipcfg.Gamma_G_P8_Y;
		iip_con->Gamma_B_P0_C = conf2_sdk->iipcfg.Gamma_B_P0_C;
		iip_con->Gamma_B_P0_Y = conf2_sdk->iipcfg.Gamma_B_P0_Y;
		iip_con->Gamma_B_P1_C = conf2_sdk->iipcfg.Gamma_B_P1_C;
		iip_con->Gamma_B_P1_Y = conf2_sdk->iipcfg.Gamma_B_P1_Y;
		iip_con->Gamma_B_P2_C = conf2_sdk->iipcfg.Gamma_B_P2_C;
		iip_con->Gamma_B_P2_Y = conf2_sdk->iipcfg.Gamma_B_P2_Y;
		iip_con->Gamma_B_P3_C = conf2_sdk->iipcfg.Gamma_B_P3_C;
		iip_con->Gamma_B_P3_Y = conf2_sdk->iipcfg.Gamma_B_P3_Y;
		iip_con->Gamma_B_P4_C = conf2_sdk->iipcfg.Gamma_B_P4_C;
		iip_con->Gamma_B_P4_Y = conf2_sdk->iipcfg.Gamma_B_P4_Y;
		iip_con->Gamma_B_P5_C = conf2_sdk->iipcfg.Gamma_B_P5_C;
		iip_con->Gamma_B_P5_Y = conf2_sdk->iipcfg.Gamma_B_P5_Y;
		iip_con->Gamma_B_P6_C = conf2_sdk->iipcfg.Gamma_B_P6_C;
		iip_con->Gamma_B_P6_Y = conf2_sdk->iipcfg.Gamma_B_P6_Y;
		iip_con->Gamma_B_P7_C = conf2_sdk->iipcfg.Gamma_B_P7_C;
		iip_con->Gamma_B_P7_Y = conf2_sdk->iipcfg.Gamma_B_P7_Y;
		iip_con->Gamma_B_P8_C = conf2_sdk->iipcfg.Gamma_B_P8_C;
		iip_con->Gamma_B_P8_Y = conf2_sdk->iipcfg.Gamma_B_P8_Y;
		iip_con->Black_P = conf2_sdk->iipcfg.Black_P;
		iip_con->White_P = conf2_sdk->iipcfg.White_P;
		iip_con->AWB_WIN_L = conf2_sdk->iipcfg.AWB_WIN_L;
		iip_con->AWB_WIN_T = conf2_sdk->iipcfg.AWB_WIN_T;
		iip_con->AWB_WIN_R = conf2_sdk->iipcfg.AWB_WIN_R;
		iip_con->AWB_WIN_B = conf2_sdk->iipcfg.AWB_WIN_B;
		iip_con->SUM_R1 = conf2_sdk->iipcfg.SUM_R1;
		iip_con->SUM_G1 = conf2_sdk->iipcfg.SUM_G1;
		iip_con->SUM_B1 = conf2_sdk->iipcfg.SUM_B1;
		iip_con->AWB_N_VLD_W1 = conf2_sdk->iipcfg.AWB_N_VLD_W1;
		iip_con->SUM_R2 = conf2_sdk->iipcfg.SUM_R2;
		iip_con->SUM_G2 = conf2_sdk->iipcfg.SUM_G2;
		iip_con->SUM_B2 = conf2_sdk->iipcfg.SUM_B2;
		iip_con->AWB_N_VLD_W2 = conf2_sdk->iipcfg.AWB_N_VLD_W2;
		iip_con->SUM_R3 = conf2_sdk->iipcfg.SUM_R3;
		iip_con->SUM_G3 = conf2_sdk->iipcfg.SUM_G3;
		iip_con->SUM_B3 = conf2_sdk->iipcfg.SUM_B3;
		iip_con->AWB_N_VLD_W3 = conf2_sdk->iipcfg.AWB_N_VLD_W3;
		iip_con->SUM_R4 = conf2_sdk->iipcfg.SUM_R4;
		iip_con->SUM_G4 = conf2_sdk->iipcfg.SUM_G4;
		iip_con->SUM_B4 = conf2_sdk->iipcfg.SUM_B4;
		iip_con->AWB_N_VLD_W4 = conf2_sdk->iipcfg.AWB_N_VLD_W4;
		iip_con->SUM_R5 = conf2_sdk->iipcfg.SUM_R5;
		iip_con->SUM_G5 = conf2_sdk->iipcfg.SUM_G5;
		iip_con->SUM_B5 = conf2_sdk->iipcfg.SUM_B5;
		iip_con->AWB_N_VLD_W5 = conf2_sdk->iipcfg.AWB_N_VLD_W5;
		iip_con->AE_WIN_L = conf2_sdk->iipcfg.AE_WIN_L;
		iip_con->AE_WIN_T = conf2_sdk->iipcfg.AE_WIN_T;
		iip_con->AE_WIN_R = conf2_sdk->iipcfg.AE_WIN_R;
		iip_con->AE_WIN_B = conf2_sdk->iipcfg.AE_WIN_B;
		iip_con->MAX_Y_CTR = conf2_sdk->iipcfg.MAX_Y_CTR;
		iip_con->MIN_Y_CTR = conf2_sdk->iipcfg.MIN_Y_CTR;
		iip_con->SUM_Y_CTR = conf2_sdk->iipcfg.SUM_Y_CTR;
		iip_con->VALID_NUM_CTR = conf2_sdk->iipcfg.VALID_NUM_CTR;
		iip_con->MAX_Y_BK = conf2_sdk->iipcfg.MAX_Y_BK;
		iip_con->MIN_Y_BK = conf2_sdk->iipcfg.MIN_Y_BK;
		iip_con->SUM_Y_BK = conf2_sdk->iipcfg.SUM_Y_BK;
		iip_con->VALID_NUM_BK = conf2_sdk->iipcfg.VALID_NUM_BK;
		iip_con->B_Strgth = conf2_sdk->iipcfg.B_Strgth;
		iip_con->B_Thrshld = conf2_sdk->iipcfg.B_Thrshld;
		iip_con->SAT = conf2_sdk->iipcfg.SAT;
		iip_con->HUE = conf2_sdk->iipcfg.HUE;
		iip_con->Y_Gain = conf2_sdk->iipcfg.Y_Gain;
		iip_con->Y_Offset = conf2_sdk->iipcfg.Y_Offset;
		iip_con->LPF_Switch = conf2_sdk->iipcfg.LPF_Switch;
		iip_con->LPF_COEF_Y_0 = conf2_sdk->iipcfg.LPF_COEF_Y_0;
		iip_con->LPF_COEF_Y_1 = conf2_sdk->iipcfg.LPF_COEF_Y_1;
		iip_con->LPF_COEF_Y_2 = conf2_sdk->iipcfg.LPF_COEF_Y_2;
		iip_con->LPF_COEF_Cr_0 = conf2_sdk->iipcfg.LPF_COEF_Cr_0;
		iip_con->LPF_COEF_Cr_1 = conf2_sdk->iipcfg.LPF_COEF_Cr_1;
		iip_con->LPF_COEF_Cr_2 = conf2_sdk->iipcfg.LPF_COEF_Cr_2;
		iip_con->LPF_COEF_Cb_0 = conf2_sdk->iipcfg.LPF_COEF_Cb_0;
		iip_con->LPF_COEF_Cb_1 = conf2_sdk->iipcfg.LPF_COEF_Cb_1;
		iip_con->LPF_COEF_Cb_2 = conf2_sdk->iipcfg.LPF_COEF_Cb_2;
		// MOTION DETECTION CONFIG
		for(index=0; index < NUM_MOTION_DETECTION_REGIONS; index++)
		{
			md_con->u32SADThresholdValues[index] = conf2_sdk->mdcfg.u32SADThresholdValues[index];
			md_con->u32MVThresholdValues[index] = conf2_sdk->mdcfg.u32MVThresholdValues[index];
			md_con->u32SensitivityValues[index] = conf2_sdk->mdcfg.u32SensitivityValues[index];
		}
		for(index=0; index < MAX_MACROBLOCKS_PER_PICTURE; index++)
		{
			md_con->u32MacroblockMap[index] = conf2_sdk->mdcfg.u32MacroblockMap[index];
		}
	}

	{ // Fixed settings
		chip_con->cfg_motion_detection_enable=conf_sdk->misccfg.reserved;
		// conf_sdk->misccfg.reserved is motion detection configuration bitmap
		if((chip_con->cfg_motion_detection_enable & 1)== 1 && chip_con->brctrl_mb_enable != 0)
			chip_con->brctrl_mb_enable = 0;

		chip_con->mp4_repeat_seqhead=conf_sdk->strcfg.reserved&0x1;
		// conf_sdk->strcfg.reserved[0] is mpeg4_repeat_seqhead flag

		chip_con->test_quant_change_in_mb=0;
		chip_con->dump_to_dram=2;
		chip_con->dram_cas=2;
		chip_con->cfg_osd_enable=0;
		chip_con->scene_change_quota=110;
		chip_con->cfg_eof_delay=1;
		chip_con->wis_proprietary_stream=0;
		chip_con->cfg_sub_mode=chip_con->wis_proprietary_stream? 0:1;
	}

	if(chip_con->do_bitrate_control==0&&chip_con->compress_mode==MPEG4) {
		if(chip_con->iniQScale<2) chip_con->iniQScale=2;
		if(chip_con->iniPQScale<2) chip_con->iniPQScale=2;
		if(chip_con->iniBQScale<2) chip_con->iniBQScale=2;
	}

	/************************* End configuration transfer *********************/


	/************************* Begin checker criteria *************************/

	if(chip_con->aspect_ratio<1||chip_con->aspect_ratio>3) {
		strcat(config_error, "Aspect Ratio can only be 1(1:1),2(4:3),3(16:9)\n");
		error_cnt++;
	}

	if(chip_con->DVD_compliant==1) {
		if(chip_con->compress_mode==MPEG1) {
			chip_con->DVD_compliant=0;
		}
		if(chip_con->compress_mode>MPEG2) {
			strcat(config_error, "DVD compliant only valid on MPEG2\n");
			error_cnt++;
		}
		if(chip_con->cfg_ivtc_enable==0&&conf_sdk->strcfg.gop_size!=15) {
			strcat(config_error, "MPEG2 DVD stream need GOP size equal to 15\n");
			error_cnt++;
		}
		if(chip_con->cfg_ivtc_enable==1&&conf_sdk->strcfg.gop_size!=12) {
			strcat(config_error, "MPEG2 DVD stream at IVTC mode need GOP size equal to 12\n");
			error_cnt++;
		}
	}

	if( ( (conf_sdk->misccfg.h_filter_mode==2) && (conf_sdk->misccfg.filter_nAX+conf_sdk->misccfg.filter_nBX+conf_sdk->misccfg.filter_nCX!=16) )||
		( (conf_sdk->misccfg.v_filter_mode==2) && (conf_sdk->misccfg.filter_nAY+conf_sdk->misccfg.filter_nBY+conf_sdk->misccfg.filter_nCY!=16) ) ){
		strcat(config_error, "VIP filter parameter sum should be 16\n");
		error_cnt++;
	}

	if(chip_con->cfg_iip_enable&&chip_con->cfg_yuvj_rgb) {
		strcat(config_error, "Video source type error\n");
		error_cnt++;
	}

	if(chip_con->cfg_ivtc_enable&&(!(chip_con->cfg_interlace==1&&chip_con->cfg_deinterlace==1))) {
		strcat(config_error, "IVTC only works on interlace = 1 and deinterlace = 1\n");
		error_cnt++;
	}

	if((chip_con->cfg_y_scale_enb&&(chip_con->cfg_y_sub_count%32))||
		(chip_con->cfg_x_scale_enb&&(chip_con->cfg_x_sub_count%32))) {
		strcat(config_error, "Scaled picture size MUST BE a multiple of 32\n");
		error_cnt++;
	}

	if((chip_con->cfg_y_sub_window&&(chip_con->cfg_y_sub_count%16))||
		(chip_con->cfg_x_sub_window&&(chip_con->cfg_x_sub_count%16))) {
		strcat(config_error, "Sub window MUST BE a multiple of 16\n");
		error_cnt++;
	}

/*	if((chip_con->cfg_y_sub_window&&
		(chip_con->cfg_y_sub_offset+chip_con->cfg_y_sub_count>INPUT_HEIGHT))||
		(chip_con->cfg_x_sub_window&&
		(chip_con->cfg_x_sub_offset+chip_con->cfg_x_sub_count>INPUT_WIDTH))) {
		strcat(config_error, "Sub window size MUST NOT EXCEED sensor size");
		error_cnt++;
	}
*/
	if(chip_con->search_range!=32&&chip_con->compress_mode==3) {
		strcat(config_error, "H.263 encoding only uses 32 as search range\n");
		error_cnt++;
	}

	if(!(chip_con->search_range==32||chip_con->search_range==64||chip_con->search_range==128)) {
		strcat(config_error, "Search range only uses 32, 64 or 128\n");
		error_cnt++;
	}

/*	if(chip_con->Frame_rate==25000) {
		strcat(config_error, "set Frame_rate as 25025 to get 25 fps\n");
		error_cnt++;
	}
*/
	if(!chip_con->cfg_interlace&&chip_con->cfg_deinterlace) {
		strcat(config_error, "deinterlace DOESN'T work on a progressive stream\n");
		error_cnt++;
	}

	if(chip_con->compress_mode!=MPEG2&&chip_con->cfg_deinterlace==2) {
		strcat(config_error, "interlace coding CAN'T work on NON-MPEG2 stream\n");
		error_cnt++;
	}

	if(chip_con->brctrl_Q<0||chip_con->target_bitrate<0) {
		strcat(config_error, "target bitrate and initial quantizer should be >= 0\n");
		error_cnt++;
	}

	if(chip_con->target_bitrate>0x1000000) {
		strcat(config_error, "target bitrate CAN'T exceed 0x1000000\n");
		error_cnt++;
	}

	if(chip_con->brctrl_peak_rate>0x4000000) {
		strcat(config_error, "peak bitrate CAN'T exceed 0x4000000\n");
		error_cnt++;
	}

	if(chip_con->compress_mode==MOTIONJPEG&&chip_con->seqMode!=IONLY) {
		strcat(config_error, "MJPEG mode CAN'T use IPONLY or IBP mode\n");
		error_cnt++;
	}

	if(chip_con->compress_mode==H263&&chip_con->seqMode==IPB) {
		strcat(config_error, "H.263 mode CAN'T use IBP mode\n");
		error_cnt++;
	}

	/************************* End checker criteria *************************/

	if(error_cnt) return 1;
	else return 0;
}

SINT32 gen_init_packages_7007SB(UINT8 *iniBuffer,
								SINT32 buflen,
								TCFGVIDEOEX *conf_sdk,
								TVIDEOCFGFIX *conf2_sdk,
								char *error_info,
								char *memory,
								SINT32 *memory_size,
								UINT8 *osd_font,
								SINT32 osd_font_size,
								UINT8 *dup_mem,
								UINT8 *seqheader_mem,
								SINT8 reset_type) {

	SINT32 width, height;
	SINT8 fgs_elbp=0;
	CHIP_CONFIG *chip_con = (CHIP_CONFIG*) malloc(sizeof(CHIP_CONFIG));
	OSD_CONFIG *osd_con = (OSD_CONFIG*) malloc(sizeof(OSD_CONFIG));
	AUDIOS_CONFIG *audio_con = (AUDIOS_CONFIG*) malloc(sizeof(AUDIOS_CONFIG));
	DEINTERLACE_CONFIG *deinterlace_con = (DEINTERLACE_CONFIG*) malloc(sizeof(DEINTERLACE_CONFIG));
	IIP_CONFIG *iip_con = (IIP_CONFIG*) malloc(sizeof(IIP_CONFIG));
	MOTION_DETECTION_CONFIG_t *md_con = (MOTION_DETECTION_CONFIG_t *) malloc(sizeof(MOTION_DETECTION_CONFIG_t));
	TInitBuffer tBuff[32];
	SINT32 rc;
	SINT32 seqheader_len = 0;
	TPackage *package_buf = (TPackage*) malloc(2048*sizeof(TPackage));
	SINT32 package_number = 0;
	SINT32 *pack_no = &package_number;
#ifdef _BIG_ENDIAN_
	UINT16 *package_pointer16 = (UINT16*) package_buf;
#endif
	{ // Get and check those configurations from sdk interface
		if(config_checker(chip_con, osd_con, audio_con, deinterlace_con, iip_con,
			md_con, conf_sdk, conf2_sdk, error_info, &width, &height)) {
			free(md_con);
			free(package_buf);
			free(iip_con);
			free(deinterlace_con);
			free(audio_con);
			free(osd_con);
			free(chip_con);
			return 2;
		}
	}
	rc = get_init_buffer(tBuff, iniBuffer, buflen);
	if(rc != SUCCESS) {
		free(md_con);
		free(package_buf);
		free(iip_con);
		free(deinterlace_con);
		free(audio_con);
		free(osd_con);
		free(chip_con);
		return 1;
	}
	memset(package_buf, 0, 2048 * sizeof(TPackage));
	generator(package_buf, pack_no, tBuff, chip_con, osd_con, iip_con, md_con, deinterlace_con,
		audio_con, width, height, fgs_elbp, 0, osd_font, osd_font_size, reset_type);
	//SJ - save seq header for later
	if(seqheader_mem != NULL) {
	  memset(seqheader_mem, 0, seqHdr_size);
	  seqhead_to_package(SEQUENCE_TO_BUFFER,NULL,NULL,seqheader_mem,&seqheader_len,chip_con,width,height);
	}
	if(dup_mem!=NULL) blank_Pframe(width, height, chip_con, dup_mem);
	free_init_buffer(tBuff);
	*memory_size=(*pack_no) * sizeof(TPackage);
#ifdef _BIG_ENDIAN_
	for(rc=0;rc<((*pack_no) * sizeof(TPackage) / 2);rc++)
		package_pointer16[rc] = ((package_pointer16[rc]&0x00ff)<<8)+((package_pointer16[rc]&0xff00)>>8);
#endif
	memcpy(memory, package_buf, *memory_size);
	free(md_con);
	free(package_buf);
	free(iip_con);
	free(deinterlace_con);
	free(audio_con);
	free(osd_con);
	free(chip_con);
	return 0;
}

#else

void gen_init_packages(void *packBuffer, SINT32 *pack_no, UINT8 *iniBuffer, SINT32 buflen,
					   SINT32 width, SINT32 height, SINT8 fgs_elbp,
					   CHIP_CONFIG chip_con, OSD_CONFIG osd_con, IIP_CONFIG iip_con, MOTION_DETECTION_CONFIG_t md_con,
					   DEINTERLACE_CONFIG deinterlace_con, AUDIOS_CONFIG audio_con) {
	TInitBuffer tBuff[32];
	TPackage *package_buf;
	FILE *fp;
	SINT32 rc = get_init_buffer(tBuff, iniBuffer, buflen);
	if (rc != SUCCESS) return;
	package_buf = (TPackage *)packBuffer;
	*pack_no = 0;
	memset(package_buf, 0, 2048 * sizeof(TPackage));
	generator(package_buf, pack_no, tBuff, &chip_con, &osd_con, &iip_con, &md_con, &deinterlace_con,
		&audio_con, width, height, fgs_elbp, 1, NULL, 0, 0);
	free_init_buffer(tBuff);
	fp = fopen("init_pack2.bin", "wb");
	fwrite(package_buf, *pack_no * sizeof(TPackage), 1, fp);
	fclose(fp);
}

#endif

void Setup(APRIL *april, SINT32 ttarget_rate, SINT32 ppeak_rate,
			SINT32 vvbv, SINT32 cconverge_speed, SINT32 LLAMBDA,
			SINT32 QQ, SINT32 ffps, SINT32 ff_area, SINT32 ssgop_mode,
			SINT32 qmin, SINT32 qmax, SINT32 window_size) {
	SINT32	i;
	SINT16	temp16=0;
	SINT32	temp32=0;
//	SINT16	mb;
//	SINT32	sgop_expt;				// expected 32b DWORD count of a SubGOP
//	SINT32	afrm_peak;				// 32b DWORD count of peak-rate in a SubGOP
//	SINT32	vbvAlert=0;				// 32b DWORD count of VBV alert watermark
//	SINT16	QD[4];					// quantizers for duplicate frame control
//	SINT16	dq=0;						// maximum allowed change to previous Q
//	SINT32	MinLmt, MaxLmt;			// limits for calculating bitrate difference
//	SINT32	Complex[4];				// averaged complexities from short to long term
//	SINT16	sgop_q;					// equivalent quantizer of last SubGOP
//	SINT32	sgop_used;				// 32b DWORD count of last SubGOP
//	SINT32	totl_used;				// 32b DWORD count of current session
//	SINT32	totl_expt;				// expected 32b DWORD count up to current
//	SINT16	FrameIdx;				// frame index in a SubGOP
//	SINT32	vbv_lvl;				// 32b DWORD count of VBV buffer current usage
//	SINT16	Q;						// base Q
//	SINT16	PQ;						// Q of next P
//	SINT16	ModeQ[4];				// picture modes of next SubGOP
//	SINT16	sgop_coef;				// see RC_SGOP
//	SINT32	sgop_next;				// adjusted expection for next SubGOP
//	SINT32	sumq;					// sum of quantizers of all the macroblocks in I/P picture
//#ifdef	_EXTRA_BRC_DEBUG
//	SINT32	sum_used;
//	SINT32	sum_expt;
//	SINT32	sum_sgop;
//#endif
	SINT32	fps=ffps;						// frames per second
	SINT16	sgop=(SINT16) ssgop_mode;		// see ESGOPMode
	SINT32	target_rate=0;
	SINT32	peak_rate=0;
	SINT32	converge_speed=0;
	SINT32	LAMBDA=0;
	SINT32	q=0;

//	Complex[0] = Complex[1] = Complex[2] = Complex[3] = (SINT32) (RC_COMPLEX * ff_area * RC_SGOP[sgop] / 32);
	temp32=(SINT32) (2 * ff_area * RC_SGOP[sgop] / 32);
	april->COMPLEX0_ADDR = temp32;
	april->COMPLEX1_ADDR = temp32;
	april->COMPLEX2_ADDR = temp32;
	april->COMPLEX3_ADDR = temp32;

	LET(target_rate, ttarget_rate, 0)
	LET(peak_rate, ppeak_rate, target_rate > 0 ? target_rate * 4 : PEAKRATE_DVD)

	LET(april->VBV_ALERT_ADDR, vvbv, VBV_DVD)
//	LET(converge_speed, cconverge_speed, 1)
	LET(converge_speed, cconverge_speed, 100)
//	LET(LAMBDA, LLAMBDA, RCLAMBDA_CFR)
	LET(LAMBDA, LLAMBDA, 100)
	LET(q, QQ, 0)

//	mb = (SINT16) (ff_area/256);
	april->MB_ADDR = (SINT16) (ff_area/256);

	if(peak_rate == 0)
		peak_rate = 0x4000000;

//	vbvAlert = vbvAlert == 0 ? 0x10000000 : (SINT32)(0.75 * vbvAlert / 32);
	april->VBV_ALERT_ADDR = april->VBV_ALERT_ADDR==0? 0x10000000:(SINT32)((april->VBV_ALERT_ADDR*3)/(4*32));
	april->MIN_LMT_ADDR = 0; //april->VBV_ALERT_ADDR / 3;
	april->MAX_LMT_ADDR = april->VBV_ALERT_ADDR; //april->MIN_LMT_ADDR * 2;

//	dq = (SINT16)(pow(RC_DQ, converge_speed * converge_speed) * FRMSInSGOP);
	if(FRMSInSGOP==1)
		april->DQ_ADDR = converge_speed_table1[converge_speed];
	else if(FRMSInSGOP==3)
		april->DQ_ADDR = converge_speed_table3[converge_speed];

	for(i = 0; i < sgop; i ++) {
//		QD[i] = (SINT16)(pow(RC_Q0, 1 - LAMBDA) * pow(RC_Q1, LAMBDA) * RC_SGOP[i]);
//		QD[i] = LAMBDA_table[i][LAMBDA];
		temp16 = LAMBDA_table[i][LAMBDA];
		switch(i) {
		case 0: april->QD0_ADDR = temp16; break;
		case 1: april->QD1_ADDR = temp16; break;
		case 2: april->QD2_ADDR = temp16; break;
		case 3: april->QD3_ADDR = temp16; break;
		}
//		LAMBDA = (LAMBDA + 1) / 2;
		LAMBDA = (LAMBDA + 100) / 2;
	}
//	for(i = sgop; i < 3; i ++) QD[i] = 32767;
	for(i = sgop; i < 3; i ++) {
		switch(i) {
		case 0: april->QD0_ADDR = 32767; break;
		case 1: april->QD1_ADDR = 32767; break;
		case 2: april->QD2_ADDR = 32767; break;
		case 3: april->QD3_ADDR = 32767; break;
		}
	}

//	QD[3] = LAMBDA > 0.9999 ? 32767 : (SINT16)(pow(RC_Q0, 1 - LAMBDA) * pow(RC_Q1, LAMBDA) * RC_SGOP[3]);
	april->QD3_ADDR = LAMBDA > 99 ? 32767 : LAMBDA_table[3][LAMBDA];
//	sgop_expt = (SINT32)floor(target_rate / 32. * FRMSInSGOP / fps);
//	afrm_peak = (SINT32)floor(peak_rate / 32. / fps);
	april->SGOP_EXPT_ADDR = (SINT32) (target_rate / 32 * FRMSInSGOP * 1001 / fps);
	april->SGOP_PEAK_ADDR = (SINT32) (peak_rate / 32 * 1001 / fps);
	april->TOTAL_BITS_USED_ADDR = (SINT32) (target_rate / 32 * 1000 / fps * window_size);
	april->TOTAL_EXPT_ADDR = (SINT32) (target_rate / 32 * 1000 / fps * window_size);
	april->SGOP_COEF_ADDR = RC_SGOP[0];
	april->SGOP_NEXT_ADDR = april->SGOP_EXPT_ADDR;
	april->SUM_Q_ADDR = 0;

	if(q > 0) {
//		Complex[0] = Complex[1] = Complex[2] = Complex[3] =	(SINT32)(sgop_expt * q * 4);
//		Complex[0] = Complex[1] = Complex[2] = Complex[3] =	(SINT32)(tmpAPRIL->SGOP_EXPT_ADDR * q);
		temp32 = (SINT32)(april->SGOP_EXPT_ADDR * q);
		april->COMPLEX0_ADDR = temp32;
		april->COMPLEX1_ADDR = temp32;
		april->COMPLEX2_ADDR = temp32;
		april->COMPLEX3_ADDR = temp32;			
	}
	else {
//		q = 0.25 * Complex[0] / sgop_expt;
		q = april->COMPLEX0_ADDR / ((april->SGOP_EXPT_ADDR==0)? 1:april->SGOP_EXPT_ADDR);
	}

	april->Q_ADDR = (SINT16) CLP(q, 4*qmin, 4*qmax);
	april->FRAME_INDEX_ADDR = 0;
	april->SGOP_Q_ADDR = 0;
	april->SGOP_USED_ADDR = 0;
	april->VBV_LEVEL_ADDR = 0;
	april->MODE0_Q_ADDR = FDUP_SELF;
	april->MODE1_Q_ADDR = FDUP_SELF;
	april->MODE2_Q_ADDR = FDUP_SELF;
	april->MODE3_Q_ADDR = FDUP_SELF;
	april->PQ_ADDR = 0;
	april->SGOP_ADDR = sgop;
	april->P_MODE_OFFSET = 0;
	april->RC_SGOP0_ADDR = (SINT32) RC_SGOP[0];
	april->RC_SGOP1_ADDR = (SINT32) RC_SGOP[1];
	april->RC_SGOP2_ADDR = (SINT32) RC_SGOP[2];
	april->RC_SGOP3_ADDR = (SINT32) RC_SGOP[3];
#ifdef	_EXTRA_BRC_DEBUG
	april->SUM_USED_ADDR = 1;
	april->SUM_EXPT_ADDR = 1;
	april->SUM_SGOP_ADDR = 0;
#endif
}

SINT32 get_init_buffer(TInitBuffer *tBuff, UINT8 *iniBuffer, SINT32 buflen) {
	SINT8 *comments;
	UINT8 *pBuffer;
	SINT32 i;
	SINT32 actlen;
#ifdef _BIG_ENDIAN_
#ifdef _little_endian_init_buffer_
	SINT32 j;
#endif
#endif

	for(i = 0; i < 32; i++) tBuff[i].buf = NULL;
	actlen = 0;
	i = 0;
	pBuffer = iniBuffer + 16;
	actlen += 16;
	comments = (SINT8 *)iniBuffer;
	comments[12]=0;
	if (strcmp(comments, "WIS7007SBINI") != 0) 
		return ERR_MISMCH;

	while(actlen < buflen - 1) {
/*#ifdef SPARC	
		tBuff[i].tag = ((UINT16)pBuffer[0] << 8) + pBuffer[1];
		pBuffer += sizeof(UINT16);
		tBuff[i].len = ((UINT32)pBuffer[0] << 24) + ((UINT32)pBuffer[1] << 16) + ((UINT32)pBuffer[2] << 8) + pBuffer[3];
#else
		tBuff[i].tag = *((UINT16 *)pBuffer);
		pBuffer += sizeof(UINT16);
		tBuff[i].len = *((UINT32 *)pBuffer);
#endif*/
#ifdef _BIG_ENDIAN_
		tBuff[i].tag = ((UINT16)pBuffer[0] << 8) + pBuffer[1];
		pBuffer += sizeof(UINT16);
		tBuff[i].len = ((UINT32)pBuffer[0] << 24) + ((UINT32)pBuffer[1] << 16) +
						((UINT32)pBuffer[2] << 8) + pBuffer[3];
#ifdef _little_endian_init_buffer_ 
		tBuff[i].tag = ((tBuff[i].tag&0x00ff) << 8) + ((tBuff[i].tag&0xff00) >> 8);
		tBuff[i].len = ((tBuff[i].len&0x000000ff) << 24) +
						((tBuff[i].len&0x0000ff00) << 8) +
						((tBuff[i].len&0x00ff0000) >> 8) +
						((tBuff[i].len&0xff000000) >> 24);
#endif
#else
		tBuff[i].tag = ((UINT16)pBuffer[1] << 8) + pBuffer[0];
		pBuffer += sizeof(UINT16);
		tBuff[i].len = ((UINT32)pBuffer[3] << 24) + ((UINT32)pBuffer[2] << 16) +
						((UINT32)pBuffer[1] << 8) + pBuffer[0];
#endif
		pBuffer += sizeof(UINT32);
		tBuff[i].buf = (UINT16 *)malloc(tBuff[i].len * sizeof(UINT16) + 1024);
		memcpy(tBuff[i].buf, pBuffer, tBuff[i].len * sizeof(UINT16));
#ifdef _BIG_ENDIAN_
#ifdef _little_endian_init_buffer_
		for(j=0;j<tBuff[i].len;j++)
			tBuff[i].buf[j] = ((tBuff[i].buf[j]&0x00ff)<<8)+((tBuff[i].buf[j]&0xff00)>>8);
#endif
#endif
		pBuffer += tBuff[i].len * sizeof(UINT16);
		actlen += sizeof(UINT16) + sizeof(UINT32) + sizeof(UINT16) * tBuff[i].len;
		i++;
	}
	return SUCCESS;
}

SINT32 find_init_buffer(TInitBuffer *tBuff, UINT16 tag) {
	SINT32 i;
	for (i = 0; i < 32; i++)
		if(tBuff[i].tag == tag) return i;
	return i;
}

void free_init_buffer(TInitBuffer *tBuff) {
	SINT32 i;
	for (i = 0; i < 32; i++)
		if (tBuff[i].buf != NULL) free(tBuff[i].buf);
}

void	gen_dqvmec_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff) {
	SINT32 i;
	
	i = find_init_buffer(tBuff, TAG_DQVMEC_REG);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_icxy_ivlc_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff) {
	SINT32 i;
	
	i = find_init_buffer(tBuff, TAG_ICX_SRAM);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);

	i = find_init_buffer(tBuff, TAG_ICY_SRAM);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);

	i = find_init_buffer(tBuff, TAG_IVLC_SRAM);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_dma_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff) {
	SINT32 i;
	
	i = find_init_buffer(tBuff, TAG_DMA_SRAM);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_mpegx_reg_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode)
{
	SINT32 i;

	i = 0;
	if (compress_mode & GO) return;

	if (compress_mode == MPEG1)
		i = find_init_buffer(tBuff, TAG_MPEG1_REG);
	else if (compress_mode == MPEG2)
		i = find_init_buffer(tBuff, TAG_MPEG2_REG);
	else if (compress_mode == H263)
		i = find_init_buffer(tBuff, TAG_H263_REG);
	else if (compress_mode == MPEG4)
		i = find_init_buffer(tBuff, TAG_MPEG4_REG);
	else if (compress_mode == MOTIONJPEG)
		i = find_init_buffer(tBuff, TAG_MJPEG_REG);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_mpegx_tbl_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode)
{
	SINT32 i;

	i = 0;
	if (compress_mode & GO) return;

	if (compress_mode == MPEG1 || compress_mode == MPEG2)
		i = find_init_buffer(tBuff, TAG_T1212_SRAM);
	else if (compress_mode == MPEG4 || compress_mode == H263)
		i = find_init_buffer(tBuff, TAG_T3412_SRAM);
	else if (compress_mode == MOTIONJPEG)
		i = find_init_buffer(tBuff, TAG_TMJ12_SRAM);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);

	if (compress_mode == MPEG1 || compress_mode == MPEG2)
		i = find_init_buffer(tBuff, TAG_T123_SRAM);
	else if (compress_mode == MPEG4 || compress_mode == H263)
		i = find_init_buffer(tBuff, TAG_T343_SRAM);
	else if (compress_mode == MOTIONJPEG)
		i = find_init_buffer(tBuff, TAG_TMJ3_SRAM);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_vlcbook_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff)
{
	SINT32 i;

	i = find_init_buffer(tBuff, TAG_VLCBOOK_DRAM);
	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_sch_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode, SINT32 clock_rate)
{
	SINT32 packcnt, len, iCount, iLine;
	SINT32 i, j, k, m;
	
	m = find_init_buffer(tBuff, TAG_SCH_SRAM);

	if (clock_rate >= 85)
		tBuff[m].buf[2 * (SDRAM_REFRESH_SCH_LINE + 2)] &= 0xF0FF;
	if (clock_rate >= 72) {
		for (i = 3; i <= 7; i++)
			tBuff[m].buf[2 * (SDRAM_REFRESH_SCH_LINE + i)] &= 0xF0FF;
	}
	if (clock_rate >= 24) {
		for (i = 8; i <= 10; i++)
			tBuff[m].buf[2 * (SDRAM_REFRESH_SCH_LINE + i)] &= 0xF0FF;
	}
	if (clock_rate >= 20) {
		for (i = 10; i <= 15; i++)
			tBuff[m].buf[2 * (SDRAM_REFRESH_SCH_LINE + i)] &= 0xF0FF;
	}
	if (clock_rate >= 12) {
		for (i = 16; i < 23; i++)
			tBuff[m].buf[2 * (SDRAM_REFRESH_SCH_LINE + i)] &= 0xF0FF;
	}

	j = *pack_no;
	len = tBuff[m].len / 2;
	packcnt = (len + 2) / 3 ;
	for (i = 0; i < packcnt; i++)
	{
		iCount = (i < packcnt - 1) ? 3 : (len - 3 * (packcnt - 1));
		ex_package[j].init_buff[0] = 0x2000 + iCount * 8;
		ex_package[j].init_buff[1] = 0x0000;

		for (k = 0; k < iCount; k++)
		{
			iLine = i * 3 + k;
			ex_package[j].init_buff[2 + 8 * k] = 0xe402;
			ex_package[j].init_buff[3 + 8 * k] = tBuff[m].buf[iLine * 2];
			ex_package[j].init_buff[4 + 8 * k] = 0xe401;
			ex_package[j].init_buff[5 + 8 * k] = 0x8001 + (iLine << 8);
			ex_package[j].init_buff[6 + 8 * k] = 0xe402;
			ex_package[j].init_buff[7 + 8 * k] = tBuff[m].buf[iLine * 2 + 1];
			ex_package[j].init_buff[8 + 8 * k] = 0xe401;
			ex_package[j].init_buff[9 + 8 * k] = 0x0001 + (iLine << 8);
		}
		ex_package[j].init_buff[30] = 0x0001;
		ex_package[j].init_buff[31] = 0x6400 + i;
		j++;
	}

	*pack_no = j;
}

void	gen_qdeq_tbl_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode, 
						  SINT8 default_iq, SINT8 default_niq, UINT8 *intra_quant_matrix, UINT8 *inter_quant_matrix)
{
	SINT32 i;
	SINT32 mode;

	mode = compress_mode & (GO - 1);
	i = 0;
	if (mode == MPEG1 || mode == MPEG2 || mode == MOTIONJPEG)
		i = find_init_buffer(tBuff, TAG_QTBL12_DRAM);
	else if (mode == MPEG4)
		i = find_init_buffer(tBuff, TAG_QTBLM4_DRAM);
	else if (mode == H263 || mode == H261)
		i = find_init_buffer(tBuff, TAG_QTBLH3_DRAM);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_osdfont_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 seqMode)
{
	SINT32 i;

	if (seqMode == IPB)
		i = find_init_buffer(tBuff, TAG_OSDFONTIPB_DRAM);
	else
		i = find_init_buffer(tBuff, TAG_OSDFONTIP_DRAM);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_osdfont_pack2(TPackage *ex_package,
						  SINT32 *pack_no,
						  UINT8 *osd_font,
						  SINT32 osd_font_size,
						  SINT32 seqMode)
{
	SINT32 i,k;
	SINT32 j = *pack_no;
	SINT32 iCount = 28;
	UINT16 start_device_mem = 0x3e00;
	SINT32 start_osdfont_addr = (seqMode==IPB)? OSDFONT_IPB_DRAM_ADDR:OSDFONT_IP_DRAM_ADDR;

	for (i = 0; i < osd_font_size; i+=2*iCount)
	{
		if (osd_font_size - i < 2 * 28)
		{
			iCount = (osd_font_size - i) / 2;
			ex_package[j].init_buff[0] = (UINT16) (0x4000 + iCount);
			ex_package[j].init_buff[1] = (UINT16) (0x8000 + start_device_mem);
//			memcpy(&(ex_package[j].init_buff[2]), &initBuff[i], iCount * sizeof(UINT16));
			for(k=0;k<iCount;k++)
				ex_package[j].init_buff[2+k] = (((UINT16) osd_font[i+2*k+1]) << 8) + ((UINT16) osd_font[i+2*k]);
			ex_package[j].init_buff[31] = (UINT16) (start_osdfont_addr >> 8);
			iCount = 28;
			j++;
		}
		else if (0x4000 - start_device_mem > 28) 
		{
			iCount = 28;
			ex_package[j].init_buff[0] = (UINT16) (0x1000 + iCount);
			ex_package[j].init_buff[1] = (UINT16) (0x8000 + start_device_mem);
//			memcpy(&(ex_package[j].init_buff[2]), &initBuff[i], iCount * sizeof(UINT16));
			for(k=0;k<iCount;k++)
				ex_package[j].init_buff[2+k] = (((UINT16) osd_font[i+2*k+1]) << 8) + ((UINT16) osd_font[i+2*k]);
			start_device_mem += (UINT16) iCount;
			j++;
		}
		else
		{
			iCount = (SINT32) (0x4000 - start_device_mem);
			ex_package[j].init_buff[0] = (UINT16) (0x4000 + iCount);
			ex_package[j].init_buff[1] = (UINT16) (0x8000 + start_device_mem);
//			memcpy(&(ex_package[j].init_buff[2]), &initBuff[i], iCount * sizeof(UINT16));
			for(k=0;k<iCount;k++)
				ex_package[j].init_buff[2+k] = (((UINT16) osd_font[i+2*k+1]) << 8) + ((UINT16) osd_font[i+2*k]);
			ex_package[j].init_buff[31] = (start_osdfont_addr >> 8);
			start_osdfont_addr += 0x0100;
			start_device_mem = 0x3e00; 
			j++;
		}
	}
	*pack_no = j;
}

void	gen_mjpeghdr_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff)
{
	SINT32 i;

	i = find_init_buffer(tBuff, TAG_MJPEGHDR_DRAM);

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_addio_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff)
{
	SINT32 i;

	i = find_init_buffer(tBuff, TAG_ADDIO_REG);
	if (i >= 32) return;

	memcpy(&ex_package[*pack_no], tBuff[i].buf, tBuff[i].len * sizeof(UINT16));
	*pack_no = *pack_no + tBuff[i].len * sizeof(UINT16) / sizeof(TPackage);
}

void	gen_gpio_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff,
					  SINT8 fgs_elbp, CHIP_CONFIG *chip_con) {

	SINT8 init_i_qscale = (SINT8)chip_con->iniQScale;
	SINT8 init_p_qscale = (SINT8)chip_con->iniPQScale;
	SINT8 init_b_qscale = (SINT8)chip_con->iniBQScale;
	SINT8 compress_mode = (SINT8)chip_con->compress_mode;
	UINT8 search_range = (UINT8)chip_con->search_range;
	UINT16 *gpioBuffer;
	UINT32 len;
	UINT32 i, j, iCount;
	UINT16 start_device_mem;
	SINT8 mode;

	iCount = 28;
	start_device_mem = 0x3e00;
	mode = compress_mode & (GO - 1);
	i = find_init_buffer(tBuff, TAG_GPIO_SRAM);
	len = tBuff[i].len;
	gpioBuffer = tBuff[i].buf;

	gpioBuffer[0x53] = (init_i_qscale << 8) + (init_i_qscale >> fgs_elbp);
	gpioBuffer[0x57] = (init_p_qscale << 8) + (init_p_qscale >> fgs_elbp);
	gpioBuffer[0x5b] = (init_b_qscale << 8) + (init_b_qscale >> fgs_elbp);
	if (mode == H263 || mode == H261 || search_range == 32)
	{
		gpioBuffer[0x5f] = 0x211f;		// 2 1 31
	}
	else if (search_range == 64)
	{
		gpioBuffer[0x5f] = 0x323f;		// 3 2 63
	}
	else	// search_range = 128
	{
		gpioBuffer[0x5f] = 0x547f;		// 5 4 127
	}

	j = len;
	if (mode != MPEG4)
	{
		for (i = 1; i < 32; i++)
		{
			gpioBuffer[j++] = 0x0000;
			gpioBuffer[j++] = 0x0000;
			gpioBuffer[j++] = 0x0202;
			gpioBuffer[j++] = 0x2000;
		}
	}
	else
	{
		for (i = 1; i < 32; i++)
		{
			gpioBuffer[j++] = (0x4000 * DC_SCALER_Y [i]) & 0xffff;
			gpioBuffer[j++] = (0x4000 * DC_SCALER_UV[i]) & 0xffff;
			gpioBuffer[j++] = (((0x4000 * DC_SCALER_Y[i]) >> 16) << 8) + ((0x4000 * DC_SCALER_UV[i]) >> 16);
			gpioBuffer[j++] = 0x10000 / DC_SCALER_UV[i];
		}
	}
	j = *pack_no;
	for (i = 0; i < 256; i += 28)
	{
		iCount = ((256 - i > 28) ? 28 : (256 - i));
		ex_package[j].init_buff[0] = 0x1000 + iCount;
		ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
		memcpy(&(ex_package[j].init_buff[2]), &gpioBuffer[i], iCount * sizeof(UINT16));
		start_device_mem += (UINT16)iCount;
		j++;
	}
	*pack_no = j;
}

void MJPEG_BuildHuffCodes(UINT8 *huff_size, UINT16 *huff_code, const UINT8 *bits_table, const UINT8 *val_table)
{
    SINT32 i, j, k, nb, code, sym;
    code = 0;
    k = 0;
    for(i = 1; i <= 16; i++) {
        nb = bits_table[i];
        for(j = 0; j < nb; j++) {
            sym = val_table[k++];
            huff_size[sym] = (UINT8)i;
            huff_code[sym] = (UINT16)code;
            code++;
        }
        code <<= 1;
    }
}

void MJPEG_Init(void) {
    MJPEG_BuildHuffCodes(huff_size_dc_luminance,
                        huff_code_dc_luminance,
                        bits_dc_luminance,
                        val_dc_luminance);
    MJPEG_BuildHuffCodes(huff_size_dc_chrominance,
                        huff_code_dc_chrominance,
                        bits_dc_chrominance,
                        val_dc_chrominance);
    MJPEG_BuildHuffCodes(huff_size_ac_luminance,
                        huff_code_ac_luminance,
                        bits_ac_luminance,
                        val_ac_luminance);
    MJPEG_BuildHuffCodes(huff_size_ac_chrominance,
                        huff_code_ac_chrominance,
                        bits_ac_chrominance,
                        val_ac_chrominance);
}

SINT32 MJPEG_StrHead(void) {
	SINT32 i = 0;
	if(0) for (i = 0; i < 64; i++) { } //intra_q[i] = strInfo->intraq[i];
	else for (i = 0; i < 64; i++) intra_q[i] = default_intra_quant_table[i];
	MJPEG_Init();
	return coding(0);
}

SINT32 MJPEG_PutQuantHead(SINT32 Q, UINT8 *intra_q) {
    SINT32 i = 0, j;
    // quant matrixes 
	num[i] = 16; code[i ++] = 0xFFDB;				// DQT = 0xDB
	num[i] = 16; code[i ++] = 2 + 1 * (1 + 64);
	num[i] =  8; code[i ++] = pcodeSet(0, 4) + 0;	// 8 bit precision , // table 0 
	num[i] =  8; code[i ++] = intra_q[0];
    for(j = 1; j < 64; j++) {
		num [i]		= 8;
		code[i++]	= (intra_q[j] * Q) >> 3;
    }
	return coding(i);
}

SINT32 MJPEG_PutHuffTable(SINT32 table_class, SINT32 table_id,
						  const UINT8 *bits_table, const UINT8 *value_table) {
    SINT32 n = 0, i = 0, j, cc, k = 0;
	num[i] = 8; code[i ++] = pcodeSet(table_class, 4) + table_id;
    for(j = 1; j <= 16; j++) {
        n += bits_table[j];
		num[i] = 8; 
		code[i++] = bits_table[j];
    }
	coding(i); i = 0;
	while(n > 0) {
		cc = (n > 128) ? 128 : n;
		for(j = 0; j < cc; j++) {
			num[i] = 8; code[i++] = value_table[k++];
		}
		coding(i); i = 0;
		n -= 128;
	}
    return coding(i);
}

SINT32 MJPEG_PutHuffHead(void) {
    SINT32 size = 418, i = 0;
    // huffman table 
	num[i] = 16; code[i ++] = 0xFFC4;		// DHT = 0xC4
	num[i] = 16; code[i ++] = size;
	coding(i); i = 0;
    MJPEG_PutHuffTable(0, 0, bits_dc_luminance, val_dc_luminance);
    MJPEG_PutHuffTable(0, 1, bits_dc_chrominance, val_dc_chrominance);
    MJPEG_PutHuffTable(1, 0, bits_ac_luminance, val_ac_luminance);
    MJPEG_PutHuffTable(1, 1, bits_ac_chrominance, val_ac_chrominance);
	return coding(i);
}

SINT32 MJPEG_FrmHead(SINT32 fq, SINT32 cols, SINT32 rows) {
	SINT32 i = 0;

	dc_dct_pred[0] = dc_dct_pred[1] = dc_dct_pred[2] = 0;

	num[i] = 16; code[i ++] = 0xFFD8;				//SOI = 0xD8
	coding(i); i = 0;

    MJPEG_PutQuantHead(fq, intra_q);

	num[i] = 16; code[i ++] = 0xFFC0;				// SOF0 = 0xC0
	num[i] = 24; code[i ++] = pcodeSet(17, 8) + 8;	// 8 bits/component 

	num[i] = 16; code[i ++] = rows << 4;
	num[i] = 16; code[i ++] = cols << 4;
	num[i] =  8; code[i ++] = 3;					// 3 components 

    // Y component 
	num[i] = 24; code[i ++] = pcodeSet(1, 16) +		// component number 
							  pcodeSet(2, 12) +		// H factor 
							  pcodeSet(2,  8) +		// V factor 
							  0;					// select matrix 
    
    // Cb component 
	num[i] = 24; code[i ++] = pcodeSet(2, 16) +		// component number 
							  pcodeSet(1, 12) +		// H factor 
							  pcodeSet(1,  8) +		// V factor 
							  0;					// select matrix 

    // Cr component 
	num[i] = 24; code[i ++] = pcodeSet(3, 16) +		// component number 
							  pcodeSet(1, 12) +		// H factor 
							  pcodeSet(1,  8) +		// V factor 
							  0;					// select matrix 
	coding(i); i = 0;

    MJPEG_PutHuffHead();
    // scan header
	num[i] = 16; code[i ++] = 0xFFDA;				// SOS = 0xDA
	num[i] = 24; code[i ++] = pcodeSet(12, 8) + 3;	// length + 3 components 
    
    // Y component 
	num[i] = 16; code[i ++] = pcodeSet(1, 8) +		// index 
							  pcodeSet(0, 4) +		// DC huffman table index
							  0;					// AC huffman table index 
    
    // Cb component 
	num[i] = 16; code[i ++] = pcodeSet(2, 8) +		// index 
							  pcodeSet(1, 4) +		// DC huffman table index
							  1;					// AC huffman table index 
    
    // Cr component 
	num[i] = 16; code[i ++] = pcodeSet(3, 8) +		// index 
							  pcodeSet(1, 4) +		// DC huffman table index
							  1;					// AC huffman table index 

	num[i] = 24; code[i ++] = pcodeSet(0, 16) +		// Ss (not used) 
							  pcodeSet(63, 8) +		// Se (not used) 
							  0;					// (not used) 
	return coding(i);
}

void gen_mjpeghdr_to_package(TPackage *ex_package, SINT32 *pack_no, SINT32 cols, SINT32 rows)
{
	SINT32 i, j;
	UINT16 *mjpeghdr_buffer=(UINT16*) malloc(sizeof(UINT16)*2048);
	UINT8 *vlc_stream=(UINT8*) malloc(1024);
	UINT16 start_device_mem = 0x3e00;
	SINT32 start_mjpeghdr_addr = MJPEG_HDR_Addr;
	SINT32 len, iCount = 28, buffer_pos = 0;
//	ITech_VLC *vlc = (ITech_VLC *)g_hVlc;
	
	memset(mjpeghdr_buffer, 0, 2048 * sizeof(UINT16));
//	vlc->StrInit(vlc_stream);
	pvlcInit(&vlc, 1, vlc_stream);
//	vlc->StrHead();
	MJPEG_StrHead();
	for (i = 1; i < 32; i++) {
//		g_frmInfo.fq = i;
//		vlc->StrInit(vlc_stream);
		pvlcInit(&vlc, 1, vlc_stream);
//		vlc->FrmHead();
		MJPEG_FrmHead(i,cols,rows);
		memcpy((UINT8 *)mjpeghdr_buffer + buffer_pos, vlc_stream, 20 * sizeof(SINT32));
		buffer_pos += 20 * sizeof(SINT32);
	}
//	vlc->StrInit(vlc_stream);
	pvlcInit(&vlc, 1, vlc_stream);
	len = MJPEG_FrmHead(i,cols,rows);
	memcpy((UINT8 *)mjpeghdr_buffer + buffer_pos, vlc_stream + 20 * sizeof(SINT32), len / 8);
	buffer_pos += len / 8 - 20 * sizeof(SINT32);

#ifdef _BIG_ENDIAN_ //#ifdef SPARC
#ifdef _little_endian_init_buffer_
	for(i=0;i<buffer_pos/2;i++)
		mjpeghdr_buffer[i]=((mjpeghdr_buffer[i]&0x00ff)<<8)+ ((mjpeghdr_buffer[i]&0xff00)>>8);
#endif
#endif

	j = *pack_no;
	for (i = 0; i < buffer_pos / 2; i += iCount)
	{
		if (buffer_pos / 2 - i < 28)
		{
			iCount = buffer_pos / 2 - i;
			ex_package[j].init_buff[0] = 0x4000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			ex_package[j].init_buff[31] = (start_mjpeghdr_addr >> 8);
			j++; iCount = 28;
		}
		else if (0x4000 - start_device_mem > 28) 
		{
			iCount = 28;
			ex_package[j].init_buff[0] = 0x1000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			j++;
			start_device_mem += (UINT16)iCount;
		}
		else
		{
			iCount = 0x4000 - start_device_mem;
			ex_package[j].init_buff[0] = 0x4000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			ex_package[j].init_buff[31] = (start_mjpeghdr_addr >> 8);
			start_mjpeghdr_addr += 0x0100;
			start_device_mem = 0x3e00; 
			j++;
		}
	}
	*pack_no = j;
	free(mjpeghdr_buffer);
	free(vlc_stream);
}

void config_package(TPackage *ex_package, SINT32 *pack_no,
					CHIP_CONFIG *chip_con,
					DEINTERLACE_CONFIG *deinterlace_con,
					SINT32 width, SINT32 height) {

	SINT8 big_endian = 0;
	SINT32 j;
	SINT32 mode;

	j = *pack_no;
	ex_package[j].init_buff[0] = 0x200e;	//	0x20xx, 14 pairs of write operations
	ex_package[j].init_buff[1] = 0x0000;	
	ex_package[j].init_buff[2] = 0xc002;	//	for vip task descriptors
	ex_package[j].init_buff[3] = 0x14b4;
	ex_package[j].init_buff[4] = 0xc003;	//	for vip task descriptors
	ex_package[j].init_buff[5] = 0x28b4;
	ex_package[j].init_buff[6] = 0xc004;	//	for vip task descriptors
	ex_package[j].init_buff[7] = 0x3c5a;
	ex_package[j].init_buff[8] = 0xdc05;	//	default Micron -6 SDRAM: cas 2, trp 2, trcd 2, trfc 7, trc 7 or 001L_1M1N_QQQQ_PPPP
	ex_package[j].init_buff[9] = 0x2a77;
	ex_package[j].init_buff[10] = 0xc6c3;	//	0 for MPEG4, H26x, or 1	for MPEG1, MPEG2
	ex_package[j].init_buff[12] = 0xc680;	//  0x0019 for MPEG4, H26x, or 0x0013 for MPEG1, MPEG2
	ex_package[j].init_buff[14] = 0xc780;	//	SRAMa = 0x140, little endian, or SRAMa = 0x140, big endian
	ex_package[j].init_buff[16] = 0xe009;	//	HPI schedule ID = 0x01
	ex_package[j].init_buff[18] = 0xc60f;	///	DQV	schedule ID = 0x8x
	ex_package[j].init_buff[20] = 0xd4ff;	//	MEC schedule - Ic_ID = 0x5x, Sb_ID = 0x4x
	ex_package[j].init_buff[22] = 0xe403;	//	Scheduler round cycle = 2340 clks
	ex_package[j].init_buff[24] = 0xe406;	//	78 events per round
	ex_package[j].init_buff[26] = 0xd411;	//	Ic enable
	ex_package[j].init_buff[28] = 0xd410;	//	Ic jumps to entry (0,0)
	ex_package[j].init_buff[30] = 0x0001;

	mode = chip_con->compress_mode & (GO - 1);
	if (mode == MPEG1 || mode == MPEG2 || mode == MOTIONJPEG)
	{
		ex_package[j].init_buff[11] = 0x0001;
//		ex_package[j].init_buff[13] = 0x0013;
		ex_package[j].init_buff[13] = 0x00d3; // 11 01 0 0 1 1
	}
	else if (mode == MPEG4)
	{
		ex_package[j].init_buff[11] = 0x0000;
//		ex_package[j].init_buff[13] = 0x0019;
		ex_package[j].init_buff[13] = 0x00f1; // 11 11 0 0 0 1
	}
	else if (mode == H263 || mode == H261)	
	{
		ex_package[j].init_buff[11] = 0x0000;
//		ex_package[j].init_buff[13] = 0x0019;
		ex_package[j].init_buff[13] = 0x0061; // 01 10 0 0 0 1
	}

	if (big_endian) ex_package[j].init_buff[15] = 0x0340;
	else ex_package[j].init_buff[15] = 0x0140;

	ex_package[j].init_buff[17] = 0x0001;
	ex_package[j].init_buff[19] = 0x0008;
	ex_package[j].init_buff[21] = 0x0002;
	ex_package[j].init_buff[23] = 2340;
	ex_package[j].init_buff[25] = 75;		// 7077 debug
	ex_package[j].init_buff[27] = 0x0001;
	ex_package[j].init_buff[29] = 0xa1d6;
	ex_package[j].init_buff[31] = 0x2801;

	j++;

#ifdef VXWORKS
	ex_package[j].init_buff[0] = 0x200e;	// 0x20xx, 2 pairs of write operations
#else
	ex_package[j].init_buff[0] = 0x200d;
#endif
	ex_package[j].init_buff[1] = 0x0000;	

	ex_package[j].init_buff[2] = 0xe402;	// enable mpegx output schedule
	ex_package[j].init_buff[3] = 0x018b;
	ex_package[j].init_buff[4] = 0xe401;
	ex_package[j].init_buff[5] = 0x8b01;

	if (chip_con->compress_mode & GO)
	{
		ex_package[j].init_buff[3] = 0x0180;	// enable gostream output schedule
	}

    ex_package[j].init_buff[6] = 0xd472;
    ex_package[j].init_buff[7] = 0x0170;	// deinterlace uv src1 addr = 370(progressive)
    if (chip_con->cfg_deinterlace==1)
        ex_package[j].init_buff[7] = 0x01b0;	// deinterlace uv src1 addr = 434(interlace)

	ex_package[j].init_buff[8] = 0xd475;
    ex_package[j].init_buff[9] = 0x0009; //  deinterlace fixed_dcs[3:0] //(progressive)

    if (chip_con->cfg_deinterlace==1)
	{
		if (deinterlace_con->DeInter_Always_Motion == 1)
        	ex_package[j].init_buff[9] = 0x000a; //  deinterlace fixed_dcs[3:0] //(motion)
		else if (deinterlace_con->DeInter_Always_Blending == 1)
        	ex_package[j].init_buff[9] = 0x0008; //  deinterlace fixed_dcs[3:0] //(blending)
		else if (deinterlace_con->DeInter_Always_Weave == 1)
        	ex_package[j].init_buff[9] = 0x0009; //  deinterlace fixed_dcs[3:0] //(weave)
		else 
			ex_package[j].init_buff[9] = 0x0000; //  deinterlace fixed_dcs[3:0] //(interlace)
	}

	ex_package[j].init_buff[10] = 0xc404;
	if (mode == MPEG4)
	{
		if (chip_con->cfg_sub_mode == 0)
			ex_package[j].init_buff[11] = 0x0001;
		else
			ex_package[j].init_buff[11] = 0x0011;
	}
	else if (mode == MPEG1)
		ex_package[j].init_buff[11] = 0x0002;
	else if (mode == MPEG2) {
		ex_package[j].init_buff[11] = 0x0004;
		if (chip_con->cfg_deinterlace == 2)
			ex_package[j].init_buff[11] = 0x0044;
	}
	else if (mode == H263)
		ex_package[j].init_buff[11] = 0x0008;
	else if (mode == MOTIONJPEG)
		ex_package[j].init_buff[11] = 0x0020;

	ex_package[j].init_buff[12] = 0x8000 + 0x3f0a;
	ex_package[j].init_buff[13] =(chip_con->compress_mode == MPEG1 ? 1 : 
								 (chip_con->compress_mode == MPEG2 ? 2 : 
								 (chip_con->compress_mode == H263  ? 4 : 
								 (chip_con->compress_mode == MPEG4 ? 8 : 
								 (chip_con->compress_mode == MOTIONJPEG ? 16:0))))) +
								 // 0: no GOP & Frame header, 1: MPEG1, 2: MPEG2, 4:H263, 8: MPEG4 
								 (chip_con->mp4_repeat_seqhead? 0x0040:0) +
								 // bit6 = 1 means repeated seqhead@MPEG4
								 (chip_con->DVD_compliant? 0x0200:0) +
								 // bit9 = 1 means DVD_compliant@MPEG2
								 (chip_con->gop_header_enable? 0x0400:0) +
								 // bit10 is gop_header_enable flag
								 ((chip_con->DVD_compliant && chip_con->cfg_ivtc_enable)? 0x0800:0);
								 // bit11 is DVD IVTC flag

	ex_package[j].init_buff[14] = 0x8000 + 0x3f0b;
	ex_package[j].init_buff[15] = 0;

	ex_package[j].init_buff[16] = 0x8000 + 0x5d5a;// OSD font base address (high)
	if (chip_con->seqMode == IPB)
		ex_package[j].init_buff[17] = 0x14;
	else
		ex_package[j].init_buff[17] = 0x0a;

	ex_package[j].init_buff[18] = 0x8000 + 0x3f0c; // reset value for vip input next addr
	if (chip_con->seqMode == IPB)
		ex_package[j].init_buff[19] = 0x0d4c; //0x0020+0x034b*4
	else
		ex_package[j].init_buff[19] = 0x036b; //0x0020+0x034b*1

	ex_package[j].init_buff[20] = 0x8000 + 0x3f0d; // reset value for ivtc pattern
	ex_package[j].init_buff[21] = 0x0000;
	ex_package[j].init_buff[22] = 0x8000 + 0x4683; // threshold AC value for setting to 0
	ex_package[j].init_buff[23] = (UINT16) chip_con->THACCoeffSet0;

	ex_package[j].init_buff[24] = 0x8000 + 0x440a; // mpegx macro block number
	ex_package[j].init_buff[25] = ((width / 16 ) << 8) + (height / 16 / (chip_con->cfg_deinterlace / 2 + 1));

	ex_package[j].init_buff[26] = 0x8000 + 0x601a; // HPI buffer capacity
	ex_package[j].init_buff[27] = (UINT16) chip_con->HPIBufferCapacity;
#ifdef VXWORKS
	ex_package[j].init_buff[28] = 0x8000 + 0x6016; // HPI Configuration Register
	ex_package[j].init_buff[29] = 0x8010;
#endif
	j++;

/*	ex_package[j].init_buff[0]  = 0x200e; //ivtc mem
	ex_package[j].init_buff[1]  = 0x0000;	

	ex_package[j].init_buff[2]  = 0xBFC0;
	ex_package[j].init_buff[3]  = 0x0000;
	ex_package[j].init_buff[4]  = 0xBFC1;
	ex_package[j].init_buff[5]  = 0x0000;
	ex_package[j].init_buff[6]  = 0xBFC2;
	ex_package[j].init_buff[7]  = 0x0000;
	ex_package[j].init_buff[8]  = 0xBFC3;
	ex_package[j].init_buff[9]  = 0x0000;
	ex_package[j].init_buff[10] = 0xBFC4;
	ex_package[j].init_buff[11] = 0x0000;
	ex_package[j].init_buff[12] = 0xBFC5;
	ex_package[j].init_buff[13] = 0x0000;
	ex_package[j].init_buff[14] = 0xBFC6;
	ex_package[j].init_buff[15] = 0x0000;
	ex_package[j].init_buff[16] = 0xBFC7;
	ex_package[j].init_buff[17] = 0x0000;
	ex_package[j].init_buff[18] = 0xBFC8; //minimal motion statistic high; reset to 4
	ex_package[j].init_buff[19] = 0x0004;
	ex_package[j].init_buff[20] = 0xBFC9; //minimal motion statistic low; reset to 0
	ex_package[j].init_buff[21] = 0x0000;
	ex_package[j].init_buff[22] = 0xBFCA; //last pattern; reset to 0
	ex_package[j].init_buff[23] = 0x0000;
	ex_package[j].init_buff[24] = 0xBFCB; //last pattern repeat count; reset to 0
	ex_package[j].init_buff[25] = 0x0000;
	ex_package[j].init_buff[26] = 0xBFCC; //motion statistc threshold
	ex_package[j].init_buff[27] = 0x0200;
	ex_package[j].init_buff[28] = 0xBFCD; //max pattern repeat count; 0x8000 means disable ivtc
	if(chip_con->cfg_ivtc_enable) 
		ex_package[j].init_buff[29] = (UINT16) chip_con->ivtc_holding_time;
	else 
		ex_package[j].init_buff[29] = 0x8000;

	j++;

	ex_package[j].init_buff[0]  = 0x200e; //ivtc mem
	ex_package[j].init_buff[1]  = 0x0000;	

	ex_package[j].init_buff[2]  = 0xBFD0;
	ex_package[j].init_buff[3]  = 0x27D8;
	ex_package[j].init_buff[4]  = 0xBFD1;
	ex_package[j].init_buff[5]  = 0x0000;
	ex_package[j].init_buff[6]  = 0xBFD2;
	ex_package[j].init_buff[7]  = 0x4EB1;
	ex_package[j].init_buff[8]  = 0xBFD3;
	ex_package[j].init_buff[9]  = 0x0000;
	ex_package[j].init_buff[10] = 0xBFD4;
	ex_package[j].init_buff[11] = 0x8D72;
	ex_package[j].init_buff[12] = 0xBFD5;
	ex_package[j].init_buff[13] = 0x0000;
	ex_package[j].init_buff[14] = 0xBFD6;
	ex_package[j].init_buff[15] = 0x8D72;
	ex_package[j].init_buff[16] = 0xBFD7;
	ex_package[j].init_buff[17] = 0x0000;
	ex_package[j].init_buff[18] = 0xBFD8;
	ex_package[j].init_buff[19] = 0x1BE4;
	ex_package[j].init_buff[20] = 0xBFD9;
	ex_package[j].init_buff[21] = 0x0000;
	ex_package[j].init_buff[22] = 0xBFCE;	// current field index
	ex_package[j].init_buff[23] = 0x0003;
	ex_package[j].init_buff[24] = 0xBFCF;	// current field counter
	ex_package[j].init_buff[25] = 0x0003;
	ex_package[j].init_buff[26] = 0xe402;	// shut off one of the qdq sch according to compress mode
	ex_package[j].init_buff[27] = 0x0088;
	ex_package[j].init_buff[28] = 0xe401;
	ex_package[j].init_buff[29] = 0x8f01;
	if (chip_con->compress_mode & GO)
		ex_package[j].init_buff[29] = 0x8e01;
*/
	ex_package[j].init_buff[0] = 0x2008;
	ex_package[j].init_buff[1] = 0x0000;	
	ex_package[j].init_buff[2] = 0x8000 + 0x6402;	// shut off one of the qdq sch according to compress mode
	ex_package[j].init_buff[3] = 0x0088;
	ex_package[j].init_buff[4] = 0x8000 + 0x6401;
	ex_package[j].init_buff[5] = 0x8f01;
	if (chip_con->compress_mode & GO)
		ex_package[j].init_buff[5] = 0x8e01;
	ex_package[j].init_buff[6]  = 0x8000 + 0x3f6a;	// 0x3f6a-b : TOTAL_BITS_USED_ADDR2
	ex_package[j].init_buff[7]  = 0;
	ex_package[j].init_buff[8]  = 0x8000 + 0x3f6b;
	ex_package[j].init_buff[9]  = 0;
	ex_package[j].init_buff[10] = 0x8000 + 0x3f6c;	// 0x3f6c-d : TOTAL_EXPT_ADDR2
	ex_package[j].init_buff[11] = 0;
	ex_package[j].init_buff[12] = 0x8000 + 0x3f6d; 
	ex_package[j].init_buff[13] = 0;
	ex_package[j].init_buff[14] = 0x8000 + 0x3f6e;	// 0x3f6e-f : window_counter
	ex_package[j].init_buff[15] = 0;
	ex_package[j].init_buff[16] = 0x8000 + 0x3f6f; 
	ex_package[j].init_buff[17] = 0;

	j++;

	ex_package[j].init_buff[0]  = 0x200e;
	ex_package[j].init_buff[1]  = 0x0000;

	ex_package[j].init_buff[2]  = 0x8000 + 0x3f66;	// 0x3F68 : window_size low 16-bit
	ex_package[j].init_buff[3]  = (UINT16) chip_con->brctrl_window_size & 0xFFFF;
	ex_package[j].init_buff[4]  = 0x8000 + 0x3f67;	// 0x3F69 : window_size high 16-bit
	ex_package[j].init_buff[5]  = (UINT16) ((chip_con->brctrl_window_size & 0xFFFF0000)>>16);
	ex_package[j].init_buff[6]  = 0x8000 + 0x3f68;	// 0x3F68 : QMIN
	ex_package[j].init_buff[7]  = (UINT16) chip_con->brctrl_qmin;
	ex_package[j].init_buff[8]  = 0x8000 + 0x3f69;	// 0x3F69 : QMAX
	ex_package[j].init_buff[9]  = (UINT16) chip_con->brctrl_qmax;
	ex_package[j].init_buff[10] = 0x8000 + 0x3fe0;	// encode frame number
	ex_package[j].init_buff[11] = 0;
	ex_package[j].init_buff[12] = 0x8000 + 0x3fe1;	// reserved 
	ex_package[j].init_buff[13] = 0;
	ex_package[j].init_buff[14] = 0x8000 + 0x3fe2;	// reserved
	ex_package[j].init_buff[15] = 0;
	ex_package[j].init_buff[16] = 0x8000 + 0x3fe3;	// sub gop size
	ex_package[j].init_buff[17] = (UINT16) chip_con->nSubGOP;
	ex_package[j].init_buff[18] = 0x8000 + 0x4031;	// Hardware VBI enable flag
	ex_package[j].init_buff[19] = (UINT16) chip_con->cfg_vbi_enable;
	ex_package[j].init_buff[20] = 0x8000 + 0x401c;	// vip_interrupt_mask
	ex_package[j].init_buff[21] = 0x001f;
	/* begin of set VIP auto roll back addresses */
	ex_package[j].init_buff[22] = 0x8000 + 0x5d8c;
	ex_package[j].init_buff[23] = 0x0015;
	ex_package[j].init_buff[24] = 0x8000 + 0x5d94;
	ex_package[j].init_buff[25] = 0x0015;
	if(chip_con->seqMode==IPB) {
		ex_package[j].init_buff[26] = 0x8000 + 0x5d88;
		ex_package[j].init_buff[27] = 0x1401;
		ex_package[j].init_buff[28] = 0x8000 + 0x5d90;
		ex_package[j].init_buff[29] = 0x1401;
	}
	else {
		ex_package[j].init_buff[26] = 0x8000 + 0x5d88;
		ex_package[j].init_buff[27] = 0x0a01;
		ex_package[j].init_buff[28] = 0x8000 + 0x5d90;
		ex_package[j].init_buff[29] = 0x0a01;
	}
	/* end of set VIP auto roll back addresses */

	j++;

	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0x8000 + 0x3fe4;	// mpeg4_sec
	ex_package[j].init_buff[3] = 0;
	ex_package[j].init_buff[4] = 0x8000 + 0x3fe5;	// mpeg4_min
	ex_package[j].init_buff[5] = 0;
	ex_package[j].init_buff[6] = 0x8000 + 0x3fe6;	// mpeg4_hour
	ex_package[j].init_buff[7] = 0;
	ex_package[j].init_buff[8] = 0x8000 + 0x3fe7;
	ex_package[j].init_buff[9] = ((UINT16) (chip_con->Frame_rate/1000)) << 8;
	ex_package[j].init_buff[10] = 0x8000 + 0x3fe8;	// OSD index base address
	ex_package[j].init_buff[11] = 0x3a00;
	ex_package[j].init_buff[12] = 0x8000 + 0x3fe9;	// OSD index address reg
	ex_package[j].init_buff[13] = 0;
	ex_package[j].init_buff[14] = 0x8000 + 0x3fea;	// vip_cnt_l
	ex_package[j].init_buff[15] = 0;
	ex_package[j].init_buff[16] = 0x8000 + 0x3feb;	// vip_cnt_h
	ex_package[j].init_buff[17] = 0;
	ex_package[j].init_buff[18] = 0x8000 + 0x3fec;	// gio_frame_flags
	ex_package[j].init_buff[19] = (UINT16) (chip_con->cfg_vbi_enable            +
								  (chip_con->cfg_motion_detection_enable & 0xa) +
								  ((chip_con->ivtc_holding_time&1) << 14)       +
								  (chip_con->brctrl_dup_enable << 15));
	ex_package[j].init_buff[20] = 0x8000 + 0x3fed;	// gio_brc_otf_status
	ex_package[j].init_buff[21] = 0;
	ex_package[j].init_buff[22] = 0x8000 + 0x3fee;	// gio_brc_otf_target_iq
	ex_package[j].init_buff[23] = 0;
	ex_package[j].init_buff[24] = 0x8000 + 0x3fef;	// gio_brc_otf_target_pq
	ex_package[j].init_buff[25] = 0;
	ex_package[j].init_buff[26] = 0x8000 + 0x3ff0;	// gio_v_sync_bitplane
	ex_package[j].init_buff[27] = (UINT16) chip_con->v_sync_bitplane;
	ex_package[j].init_buff[28] = 0x8000 + 0x3ff1;	// gio_v_sync_counter
	ex_package[j].init_buff[29] = 0;

	j++;

	*pack_no = j;

}

void	final_package(TPackage *ex_package, SINT32 *pack_no, SINT8 fgs_elbp,
					  SINT32 width, SINT32 height, CHIP_CONFIG *chip_con)
{
	SINT32 j;
	SINT32 mode;
	SINT32 interlace_coding;
//	SINT32 rcc_indicator;
	SINT8 deqscale;
	j = *pack_no;
	mode = chip_con->compress_mode & (GO - 1);
	interlace_coding = (chip_con->cfg_interlace && chip_con->cfg_deinterlace == 2) ? 1 : 0;
//	rcc_indicator = (chip_con->myFOURCC==1 && mode==MPEG4)? 1:0;
	deqscale = (chip_con->iniQScale >> fgs_elbp);
	chip_con->cfg_sub_mode = (mode == MPEG4 && chip_con->cfg_sub_mode != 0) ? 1 : 0;

	ex_package[j].init_buff[0] = 0x8000;
	ex_package[j].init_buff[1] = 0;
	
	ex_package[j].init_buff[2] = 0x0000;
	ex_package[j].init_buff[3] = 0x0000;
	//@2				//	initial frame number low 16 bits
	//@3				//	initial frame number high 13 bits

	ex_package[j].init_buff[4] = 0;
	ex_package[j].init_buff[5] = 0; // mapping to C-bus 0x4005
	ex_package[j].init_buff[6] = 0; // mapping to C-bus 0x4006
	ex_package[j].init_buff[7] = 2; // mapping to C-bus 0x4007

	ex_package[j].init_buff[8] = (chip_con->cfg_yuvj_rgb << 15) | ((chip_con->cfg_interlace && (chip_con->cfg_deinterlace < 2)) << 14) | (chip_con->cfg_bayer_pattern << 12) | 
			                     (chip_con->cfg_iip_enable << 11) | (chip_con->hv_resync_enable << 10) | ((chip_con->cfg_deinterlace == 1) << 9) | 
								 (chip_con->cfg_sub_sample << 8) | (0 << 7) | (chip_con->cfg_mode_656 << 6) | 
								 chip_con->cfg_sensor_desc;
	ex_package[j].init_buff[9] = (chip_con->cfg_v_filter_enb << 15) | (chip_con->cfg_y_scale_enb << 14) | ((chip_con->cfg_y_sub_count/16/(chip_con->cfg_deinterlace/2+1)) << 8) | 
								 (chip_con->cfg_h_filter_enb << 7 ) | (chip_con->cfg_x_scale_enb << 6 ) | (chip_con->cfg_x_sub_count/16);
	ex_package[j].init_buff[10] = (chip_con->cfg_y_sub_window << 15) | ((chip_con->cfg_y_sub_offset/4/(chip_con->cfg_deinterlace/2+1)) << 8) |
								  (chip_con->cfg_x_sub_window << 7) | (chip_con->cfg_x_sub_offset/4);
	ex_package[j].init_buff[11] = (chip_con->cfg_eof_delay << 6);
	ex_package[j].init_buff[12] = ((chip_con->cfg_v_filter_format << 15) | (chip_con->cfg_filter_nAY << 10) | (chip_con->cfg_filter_nBY << 5) | chip_con->cfg_filter_nCY);
	ex_package[j].init_buff[13] = ((chip_con->cfg_h_filter_format << 15) | (chip_con->cfg_filter_nAX << 10) | (chip_con->cfg_filter_nBX << 5) | chip_con->cfg_filter_nCX);
	ex_package[j].init_buff[14] = (chip_con->cfg_drop_frame << 8) | (((chip_con->cfg_deinterlace == 1) | (chip_con->cfg_deinterlace == 2)) << 7) | 0x0041;
	//@08..@14		//	for vip preg_8 to preg_e
	if (chip_con->seqMode == IPB)
		ex_package[j].init_buff[15] = 0x0d4c; //0x0020+0x034b*4
	else
		ex_package[j].init_buff[15] = 0x036b; //0x0020+0x034b*1
	//@15				//	0x0020	i/ip
						//	0x07a0	ibbp

	ex_package[j].init_buff[16] = ((height / 16 / (interlace_coding + 1)) << 8) + (width / 16);
	//@16	reg_max		//	[15:08] - maximum macroblock count in vertical direction
						//	[07:00] - maximum macroblock count in horizontal direction
	ex_package[j].init_buff[17] = (chip_con->iniQScale << 8) + (deqscale);
	//@17	reg_scl		//	[12:08] - initial QScale
						//	[04:00] - initial deQScale
	if (chip_con->seqMode == IPB)
		ex_package[j].init_buff[18] = 0x0180; // reg_stt[8]=reg_stt[7]=ipb_indicator
	else
		ex_package[j].init_buff[18] = 0x0000;

	ex_package[j].init_buff[18] = ex_package[j].init_buff[18]+
							  	   (chip_con->do_bitrate_control << 15)+ 
								   (interlace_coding             << 13)+ 
								   (chip_con->is_closed_gop      << 12)+ 
								   (chip_con->cfg_sub_mode       << 11)+ 
								   (chip_con->cfg_four_channel   << 10)+
								   ((chip_con->has_audio_package&0x1) <<  9)+
								   ((chip_con->cfg_motion_detection_enable & 1) << 2)+
								   (chip_con->brctrl_mb_enable   <<  1)+
								   (chip_con->compress_mode < GO);
	//@18	reg_stt		//	[00] = 0 (GO) or 1 (MPEGX)
						//	[01] = 0 MB BRC disable or 1 MB BRC enable 
						//	[02] = 0 (Motion Detection off) or 1 (Motion Detection on)
						//	[03] = 1 (RCC_MPEG4) or 0 (ISO_MPEG4)
						//	[04..06] = 3'h0
						//	[07] = 0 (i/ip) or 1 (ibbp)
						//	[08] = 0 (i/ip) or 1 (ibbp)
						//	[09] = 0 (av_sync off) or 1 (av_sync on)
						//	[10] = 1 (enable 4 channels) or 0 (one channel)
						//	[11] = 1 (ISO MPEG4 B frame) or 0 (WIS B frame)
						//	[12] = 0 (open GOP) or 1 (closed GOP)
						//	[13] = 0 (progressive coding) or 1 (interlacing coding)
						//	[14] = 1'h0
						//  [15] = 0 (bitrate_control off) or 1 (bitrate_control on)

	if(mode == MPEG1 || mode == MPEG2 || mode == MOTIONJPEG)
		ex_package[j].init_buff[19] = 0x81a1;
	else if(mode == MPEG4)
		ex_package[j].init_buff[19] = 0x8121;
	else if(mode == H261)
		ex_package[j].init_buff[19] = 0xc121;
	else if(mode == H263)
		ex_package[j].init_buff[19] = 0x8121;
	//@19	reg_frm		//	[00] = 1
						//	[01..06] = 6'h0
						//	[07] = 0 (mpg4) or 1 (mpg1/2)
						//	[08] = 1
						//	[09] = 0
						//	[10] = 0
						//	[11..13] = 3'h0
						//	[14] = 0 (half pixel) or 1 (no half pixel).
						//	[15] = 1; first gop 
	if (chip_con->dramType == DRAM7007U_4M) {
		ex_package[j].init_buff[20] = 0x0f0b;
		ex_package[j].init_buff[21] = 0x000b;
		ex_package[j].init_buff[22] = 0x5800;
		ex_package[j].init_buff[23] = 0x7fff;
	}
	else if (chip_con->seqMode == IPB) {
		ex_package[j].init_buff[20] = 0x1f15;
		ex_package[j].init_buff[21] = 0x0015;
		ex_package[j].init_buff[22] = 0xa800;
		ex_package[j].init_buff[23] = 0xffff;
	}
	else {
		ex_package[j].init_buff[20] = 0x1f0b;
		ex_package[j].init_buff[21] = 0x000b;
		ex_package[j].init_buff[22] = 0x5800;
		ex_package[j].init_buff[23] = 0xffff;
	}
	//@20				//	auto rollback control word
						//	0x0f08	4M, i/ip
						//	0x1f08	8M,	i/ip
						//	0x1f10	8M,	ibbp

	//@21				//	auto rollback head address h5'
						//	0x0008	4M, i/ip
						//	0x0008	8M,	i/ip
						//	0x0010	8M,	ibbp

	//@22				//	auto rollback head address h16'
						//	0x4000	4M, i/ip
						//	0x4000	8M,	i/ip
						//	0x8000	8M,	ibbp

	//@23				//	auto rollback tail address h16'
						//	0x7fff	4M
						//	0xffff	8M
	ex_package[j].init_buff[24]  = 0x0020+0x034b*0;
	ex_package[j].init_buff[25]  = 0x0020+0x034b*1;
	ex_package[j].init_buff[26]  = 0x0020+0x034b*2;
	ex_package[j].init_buff[27]  = 0x0020+0x034b*3;
	ex_package[j].init_buff[28]  = 0x0020+0x034b*4;
	ex_package[j].init_buff[29]  = 0x0020+0x034b*5;
	//@24				//	0x0020	-	mem[0] = 0x002000
	//@25				//	0x02a0	-	mem[1] = 0x02a000
	//@26				//	0x0520	-	mem[2] = 0x052000
	//@27				//	0x07a0	-	mem[3] = 0x07a000
	//@28				//	0x0a20	-	mem[4] = 0x0a2000
	//@29				//	0x0ca0	-	mem[5] = 0x0ca000
	if (chip_con->seqMode == IONLY)
		ex_package[j].init_buff[30] = 0x0000;
	else
		ex_package[j].init_buff[30] = (UINT16)chip_con->nGOP;
	//@30				//	maximum sub gops in a gop (0 for i only)
	ex_package[j].init_buff[31] = (UINT16)((width / 16) * (height / 16) * chip_con->scene_change_quota / 100);
	//@31				//	maximum i blocks allowed in a p frame (for scene change)

	j++;
	*pack_no = j;
}

void audio_to_package(TPackage *ex_package, SINT32 *pack_no, AUDIOS_CONFIG *audio_config) {
	UINT16 audio_con[21];
	SINT8 addition=0;
	SINT32 nidx=2;
	SINT32 k,l;
	SINT32 j = *pack_no;
	switch(audio_config->main_divider) {
	case 1: audio_config->main_divider=0x0; break;
	case 2: audio_config->main_divider=0x1; break;
	case 4: audio_config->main_divider=0x3; break;
	case 6: audio_config->main_divider=0x5; break;
	case 8: audio_config->main_divider=0x7; break;
	case 12: audio_config->main_divider=0xb; break;
	default: audio_config->main_divider=0x1; break;
	}
	switch(audio_config->BCLK_divider) {
	case 4: audio_config->BCLK_divider=0x0; break;
	case 8: audio_config->BCLK_divider=0x10; break;
	case 12: audio_config->BCLK_divider=0x20; break;
	case 16: audio_config->BCLK_divider=0x30; break;
	case 24: audio_config->BCLK_divider=0x50; break;
	case 32: audio_config->BCLK_divider=0x70; break;
	case 48: audio_config->BCLK_divider=0xb0; break;
	default: audio_config->BCLK_divider=0x10; break;
	}

	audio_con[0]=(UINT16) (	(audio_config->main_divider & 0xf)          +
							(audio_config->BCLK_divider & 0xf0)          +
							(audio_config->OKI_mode               << 8)  +
							(audio_config->OKI_shortframe_mode_clock_gen   << 9)  +
							(audio_config->clock_generator_enable << 10) +
							(audio_config->I2S_master_mode        << 11));

	audio_con[1]=(UINT16) (	(audio_config->I2S_mode & 0x3           ) +
							(audio_config->invert_phase_BCLK    << 2) +
							(audio_config->OKI_shortframe_mode_input_config  << 3) +
							(((audio_config->word_length&0x1f)  << 4) & 0x1f0) +
							(audio_config->auto_SOF_generation  << 9) +
							(audio_config->left_channel_only    << 11)+
							(audio_config->bit8_filter << 12)+
							(audio_config->AC97_enable          << 15));

	audio_con[2]=(UINT16) (audio_config->audio_signature_bit & 0x7);

	audio_con[3]=(UINT16) (audio_config->buffer_config & 0xffff);

	audio_con[3+1]=(UINT16) (audio_config->sample_mapping_reg1 & 0xffff);

	audio_con[3+2]=(UINT16) (audio_config->sample_mapping_reg2 & 0xffff);

	audio_con[3+3]=(UINT16) (audio_config->sample_mapping_reg3 & 0xffff);

	audio_con[3+4]=(UINT16) (audio_config->sample_mapping_reg4 & 0xffff);

	audio_con[3+5]=(UINT16) (audio_config->sample_mapping_reg5 & 0xffff);

	audio_con[3+6]=(UINT16) (audio_config->sample_mapping_reg6 & 0xffff);

	audio_con[3+7]=(UINT16) (audio_config->sample_mapping_reg7 & 0xffff);

	audio_con[11]=(UINT16) (audio_config->adpcm_enable & 0x1);

	audio_con[12]=(UINT16) (audio_config->adpcm_mode & 0x1);

	audio_con[13]=(UINT16) (audio_config->bytes_per_block & 0xffff);

	audio_con[14]=(UINT16) (audio_config->predictor & 0xffff);

	audio_con[15]=(UINT16) (audio_config->delta_left & 0xffff);

	audio_con[16]=(UINT16) (audio_config->delta_right & 0xffff);

	audio_con[17]=(UINT16) (audio_config->coef1_left & 0xffff);

	audio_con[18]=(UINT16) (audio_config->coef1_right & 0xffff);

	audio_con[19]=(UINT16) (audio_config->coef2_left & 0xffff);

	audio_con[20]=(UINT16) (audio_config->coef2_right & 0xffff);

	if(audio_con[0]&0x0400) addition++;
	if(audio_con[1]&0x8000) addition++;

	ex_package[j].init_buff[0] = 0x200c + addition;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0x9002;
	ex_package[j].init_buff[3] = audio_con[2];


	ex_package[j].init_buff[4] = 0x9002;
	ex_package[j].init_buff[5] = audio_con[2];

	ex_package[j].init_buff[6] = 0x9031;
	ex_package[j].init_buff[7] = audio_con[4];
	ex_package[j].init_buff[8] = 0x9032;
	ex_package[j].init_buff[9] = audio_con[5];
	ex_package[j].init_buff[10] = 0x9033;
	ex_package[j].init_buff[11] = audio_con[6];
	ex_package[j].init_buff[12] = 0x9034;
	ex_package[j].init_buff[13] = audio_con[7];
	ex_package[j].init_buff[14] = 0x9035;
	ex_package[j].init_buff[15] = audio_con[8];
	ex_package[j].init_buff[16] = 0x9036;
	ex_package[j].init_buff[17] = audio_con[9];
	ex_package[j].init_buff[18] = 0x9037;
	ex_package[j].init_buff[19] = audio_con[10];
	ex_package[j].init_buff[20] = 0x9040;
	ex_package[j].init_buff[21] = audio_con[11];

	if(!(audio_con[0]&0x0400)) {
		ex_package[j].init_buff[22] = 0x9000;
		ex_package[j].init_buff[23] = audio_con[0];
	}
	else {
		ex_package[j].init_buff[22] = 0x9000;
		ex_package[j].init_buff[23] = audio_con[0]&0xfbff;
	}

	if(!(audio_con[1]&0x8000)) {
		ex_package[j].init_buff[24] = 0x9001;
		ex_package[j].init_buff[25] = audio_con[1];
	}
	else {
		ex_package[j].init_buff[24] = 0x9001;
		ex_package[j].init_buff[25] = audio_con[1]&0x7fff;
	}

	if(audio_con[0]&0x0400) {
		ex_package[j].init_buff[26] = 0x9000;
		ex_package[j].init_buff[27] = audio_con[0];
	}
	if(audio_con[1]&0x8000) {
		if(audio_con[0]&0x0400) {
			ex_package[j].init_buff[28] = 0x9001;
			ex_package[j].init_buff[29] = audio_con[1];
		}
		else {
			ex_package[j].init_buff[26] = 0x9001;
			ex_package[j].init_buff[27] = audio_con[1];
		}
	}

	j++;

	// remove audio software restart to fix Left/Right channels reverse (10/28/03)
	ex_package[j].init_buff[nidx++] = 0x9041;
	ex_package[j].init_buff[nidx++] = audio_con[12];
	ex_package[j].init_buff[nidx++] = 0x9042;
	ex_package[j].init_buff[nidx++] = audio_con[13];
	ex_package[j].init_buff[nidx++] = 0x9043;
	ex_package[j].init_buff[nidx++] = audio_con[14];
	ex_package[j].init_buff[nidx++] = 0x9044;
	ex_package[j].init_buff[nidx++] = audio_con[15];
	ex_package[j].init_buff[nidx++] = 0x9045;
	ex_package[j].init_buff[nidx++] = audio_con[16];
	ex_package[j].init_buff[0] = 0x2000 | ((nidx-2)/2);
	ex_package[j].init_buff[1] = 0x0000;

	nidx = 2;


    if(audio_config->adpcm_enable) 
	{ // ADPCM Enable
		j++;

		ex_package[j].init_buff[nidx++] = 0x9001;
		ex_package[j].init_buff[nidx++] = 0x0301;	// Enable MS-ADPCM mode

		ex_package[j].init_buff[nidx++] = 0x9040;
		ex_package[j].init_buff[nidx++] = 0;	    // Disable ADPCM


		ex_package[j].init_buff[nidx++] = 0x9041;
		ex_package[j].init_buff[nidx++] = (UINT16) (audio_config->adpcm_mode&0x01);
													// 0 -- IMA, 1 -- MS

		ex_package[j].init_buff[nidx++] = 0x9042;
		ex_package[j].init_buff[nidx++] = 256;		// Select 256 Bytes Per Block

		ex_package[j].init_buff[nidx++] = 0x9043;
		ex_package[j].init_buff[nidx++] = 0x0000;   // bPred

		ex_package[j].init_buff[nidx++] = 0x9044;
		ex_package[j].init_buff[nidx++] = 256;		// IDELTA_LEFT

		ex_package[j].init_buff[nidx++] = 0x9045;
		ex_package[j].init_buff[nidx++] = 256;		// IDELTA_RIGHT

		//!!!!!Don't write coef==>1046~1049

		//each packet is 64 bytes long, and the first and last are reserved for other used
		//then it became only 14 * 2(address and data) * 2(bytes) available
		//so to put 88 step size table into 0x1100 to 0x1158 they have to be broken
		//also when write to 0x1100  write 0x9100 in stead that last "bit" means "write"
	 
		if ((audio_config->adpcm_mode&0x1) ==0)//IMA
		{
			for (k = 0,l = 0 ; k < ISSTMAX; k++,l++)
			{
				if (!(l%14))
				{
					ex_package[j].init_buff[0] = 0x2000 | ((nidx-2)/2);
					ex_package[j].init_buff[1] = 0x0000;
					j++;
					nidx = 2;
				}	
				ex_package[j].init_buff[nidx++] = 0x9100+k;
				ex_package[j].init_buff[nidx++] = (UINT16) imaStepSizeTable[k];
			}
		}
		ex_package[j].init_buff[nidx++] = 0x9040;
		ex_package[j].init_buff[nidx++] = 1;		// Enable ADPCM


		ex_package[j].init_buff[nidx++] = 0x9020;
		ex_package[j].init_buff[nidx++] = 1;		// Restart Audio Module


		ex_package[j].init_buff[0] = 0x2000 | ((nidx-2)/2);
		ex_package[j].init_buff[1] = 0x0000;
	}
	j++;
	*pack_no = j;
}

void iip_config_convert(SINT32 *count, UINT16 *iip_Configuration, IIP_CONFIG *iip_con, SINT32 x_sub_count, SINT32 y_sub_count) {

	UINT16 temp=0;
	temp=((UINT16) iip_con->Gamma_R_P0_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P0_Y;
	iip_Configuration[0x00]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P1_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P1_Y;
	iip_Configuration[0x01]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P2_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P2_Y;
	iip_Configuration[0x02]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P3_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P3_Y;
	iip_Configuration[0x03]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P4_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P4_Y;
	iip_Configuration[0x04]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P5_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P5_Y;
	iip_Configuration[0x05]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P6_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P6_Y;
	iip_Configuration[0x06]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P7_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P7_Y;
	iip_Configuration[0x07]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_R_P8_C)<<8;
	temp+=(UINT16) iip_con->Gamma_R_P8_Y;
	iip_Configuration[0x08]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P0_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P0_Y;
	iip_Configuration[0x09]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P1_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P1_Y;
	iip_Configuration[0x0a]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P2_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P2_Y;
	iip_Configuration[0x0b]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P3_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P3_Y;
	iip_Configuration[0x0c]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P4_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P4_Y;
	iip_Configuration[0x0d]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P5_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P5_Y;
	iip_Configuration[0x0e]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P6_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P6_Y;
	iip_Configuration[0x0f]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P7_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P7_Y;
	iip_Configuration[0x10]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_G_P8_C)<<8;
	temp+=(UINT16) iip_con->Gamma_G_P8_Y;
	iip_Configuration[0x11]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P0_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P0_Y;
	iip_Configuration[0x12]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P1_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P1_Y;
	iip_Configuration[0x13]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P2_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P2_Y;
	iip_Configuration[0x14]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P3_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P3_Y;
	iip_Configuration[0x15]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P4_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P4_Y;
	iip_Configuration[0x16]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P5_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P5_Y;
	iip_Configuration[0x17]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P6_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P6_Y;
	iip_Configuration[0x18]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P7_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P7_Y;
	iip_Configuration[0x19]=temp;
	temp=0;
	temp=((UINT16) iip_con->Gamma_B_P8_C)<<8;
	temp+=(UINT16) iip_con->Gamma_B_P8_Y;
	iip_Configuration[0x1a]=temp;
	temp=0;
	temp=((UINT16) iip_con->LPF_Switch)<<8;
	temp+=(UINT16) iip_con->LPF_COEF_Cb_2;
	iip_Configuration[0x1b]=temp;
	temp=0;
	temp=((UINT16) iip_con->LPF_COEF_Y_1)<<8;
	temp+=(UINT16) iip_con->LPF_COEF_Y_0;
	iip_Configuration[0x1c]=temp;
	temp=0;
	temp=((UINT16) iip_con->LPF_COEF_Cr_2)<<8;
	temp+=(UINT16) iip_con->LPF_COEF_Y_2;
	iip_Configuration[0x1d]=temp;
	temp=0;
	temp=((UINT16) iip_con->LPF_COEF_Cr_1)<<8;
	temp+=(UINT16) iip_con->LPF_COEF_Cr_0;
	iip_Configuration[0x1e]=temp;
	temp=0;
	temp=((UINT16) iip_con->LPF_COEF_Cb_1)<<8;
	temp+=(UINT16) iip_con->LPF_COEF_Cb_0;
	iip_Configuration[0x1f]=temp;
	iip_Configuration[0x20]=(UINT16) (x_sub_count+4);
	iip_Configuration[0x21]=(UINT16) (y_sub_count+4);
	iip_Configuration[0x22]=(UINT16) iip_con->CCM_R_0;
	iip_Configuration[0x23]=(UINT16) iip_con->CCM_R_1;
	iip_Configuration[0x24]=(UINT16) iip_con->CCM_R_2;
	iip_Configuration[0x25]=(UINT16) iip_con->CCM_R_3;
	iip_Configuration[0x26]=(UINT16) iip_con->CCM_G_0;
	iip_Configuration[0x27]=(UINT16) iip_con->CCM_G_1;
	iip_Configuration[0x28]=(UINT16) iip_con->CCM_G_2;
	iip_Configuration[0x29]=(UINT16) iip_con->CCM_G_3;
	iip_Configuration[0x2a]=(UINT16) iip_con->CCM_B_0;
	iip_Configuration[0x2b]=(UINT16) iip_con->CCM_B_1;
	iip_Configuration[0x2c]=(UINT16) iip_con->CCM_B_2;
	iip_Configuration[0x2d]=(UINT16) iip_con->CCM_B_3;
	iip_Configuration[0x2e]=(UINT16) iip_con->R_Gain;
	iip_Configuration[0x2f]=(UINT16) iip_con->G_Gain;
	iip_Configuration[0x30]=(UINT16) iip_con->B_Gain;
	iip_Configuration[0x31]=(UINT16) iip_con->R_Offset;
	iip_Configuration[0x32]=(UINT16) iip_con->G_Offset;
	iip_Configuration[0x33]=(UINT16) iip_con->B_Offset;
	iip_Configuration[0x34]=(UINT16) iip_con->B_Thrshld;
	iip_Configuration[0x35]=(UINT16) iip_con->B_Strgth;
	iip_Configuration[0x36]=(UINT16) iip_con->SAT;
	iip_Configuration[0x37]=(UINT16) iip_con->HUE;
	iip_Configuration[0x38]=(UINT16) iip_con->Y_Gain;
	iip_Configuration[0x39]=(UINT16) iip_con->Y_Offset;
	iip_Configuration[0x3a]=(UINT16) iip_con->AE_WIN_T;
	iip_Configuration[0x3b]=(UINT16) iip_con->AE_WIN_B;
	iip_Configuration[0x3c]=(UINT16) iip_con->AE_WIN_L;
	iip_Configuration[0x3d]=(UINT16) iip_con->AE_WIN_R;
	iip_Configuration[0x3e]=(UINT16) iip_con->White_P;
	iip_Configuration[0x3f]=(UINT16) iip_con->Black_P;
	iip_Configuration[0x40]=(UINT16) iip_con->AWB_WIN_T;
	iip_Configuration[0x41]=(UINT16) iip_con->AWB_WIN_B;
	iip_Configuration[0x42]=(UINT16) iip_con->AWB_WIN_L;
	iip_Configuration[0x43]=(UINT16) iip_con->AWB_WIN_R;
	*count=0x44;
}

void iip_to_package(TPackage *ex_package, SINT32 *pack_no, CHIP_CONFIG *chip_con, IIP_CONFIG *iip_con) {
	UINT16 start_device_mem;
	SINT32 i, j, k;
	UINT16 *iip_config_Buffer =  (UINT16*) malloc(1024*sizeof(UINT16));
	UINT32 iCount;
	iCount = 0;
	memset(iip_config_Buffer, 0, 1024 * sizeof(UINT16));
	iip_config_convert(&k, iip_config_Buffer, iip_con,
		chip_con->cfg_x_sub_count, chip_con->cfg_y_sub_count);
	j = *pack_no;
	start_device_mem = 0x4100;	//Icx start address
	for (i = 0; i < k; i += 28)	{
		iCount = ((k - i) > 28 ? 28 : (k - i));
		ex_package[j].init_buff[0] = 0x1000 + iCount;
		ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
		memcpy(&(ex_package[j].init_buff[2]), &iip_config_Buffer[i], iCount * sizeof(UINT16));
		j++;
		start_device_mem += (UINT16)iCount;
	}
	*pack_no = j;
	free(iip_config_Buffer);
}

void osddeinter_to_package(TPackage *ex_package, SINT32 *pack_no,
						   DEINTERLACE_CONFIG *deinterlace_con, OSD_CONFIG *osd_con) {
	SINT32 j;
	j = *pack_no;
	ex_package[j].init_buff[0] = 0x200d;
	ex_package[j].init_buff[1] = 0;

	ex_package[j].init_buff[2] = 0xd46a;      //      {osd_y_color1,osd_y_color0} = {8'hff,8'h0}
	ex_package[j].init_buff[3] = (osd_con->OSDyc1 << 8) + osd_con->OSDyc0;
	ex_package[j].init_buff[4] = 0xd46b;      //      {osd_u_color1,osd_u_color0} = {8'h80,8'h80}
	ex_package[j].init_buff[5] = (osd_con->OSDuc1 << 8) + osd_con->OSDuc0;
	ex_package[j].init_buff[6] = 0xd46c;      //      {osd_v_color1,osd_v_color0} = {8'h80,8'h80}
	ex_package[j].init_buff[7] = (osd_con->OSDvc1 << 8) + osd_con->OSDvc0;

	ex_package[j].init_buff[8] = 0xd468;      //     osd_enable 
	ex_package[j].init_buff[9] = (UINT16)osd_con->DoOSD;

	ex_package[j].init_buff[10] = 0xd476;      //     deinterlace edge_factor 20*20
	ex_package[j].init_buff[11] = deinterlace_con->THedge * deinterlace_con->THedge;
	ex_package[j].init_buff[12] = 0xd477;      //     deinterlace motion th 30*30
	ex_package[j].init_buff[13] = deinterlace_con->THmotion * deinterlace_con->THmotion;
	ex_package[j].init_buff[14] = 0xd478;      //     deinterlace blending th 4*4
	ex_package[j].init_buff[15] = deinterlace_con->THblending * deinterlace_con->THblending;
	ex_package[j].init_buff[16] = 0xd479;      //     deinterlace edge th big 900
	ex_package[j].init_buff[17] = (UINT16)deinterlace_con->THbigedge;
	ex_package[j].init_buff[18] = 0xd47a;      //     deinterlace edge th small 900
	ex_package[j].init_buff[19] = (UINT16)deinterlace_con->THsmalledge;

	ex_package[j].init_buff[20] = 0xd47b;      //      deinterlace edge_factor_detect
	ex_package[j].init_buff[21] = deinterlace_con->THedgestatistics * deinterlace_con->THedgestatistics;
	ex_package[j].init_buff[22] = 0xd47c;      //      deinterlace motion th for detect
	ex_package[j].init_buff[23] = deinterlace_con->THmotionstatistics * deinterlace_con->THmotionstatistics;
	ex_package[j].init_buff[24] = 0xd47d;      //      deinterlace edge big th for detect
	ex_package[j].init_buff[25] = (UINT16)deinterlace_con->THbigedgestatistics;
	ex_package[j].init_buff[26] = 0xd47e;      //      deinterlace edge small th for detect
	ex_package[j].init_buff[27] = (UINT16)deinterlace_con->THsmalledgestatistics;

   	j++;

   	*pack_no = j;
}

/********************************************************************************
*
*   PROCEDURE:  
*		void md_to_package(TPackage *ex_package, SINT32 *pack_no,
*						   MOTION_DETECTION_CONFIG_t *md_con)
*
*   DESCRIPTION:
*		Process the motion detection arguments and fill in the proper packages
*	  to support it.
*  
*   ARGUMENTS:
*		
*
*   RETURNS:
*
*		NONE
*
*   NOTES:
*
********************************************************************************/

void md_to_package(TPackage *ex_package, SINT32 *pack_no,
				   MOTION_DETECTION_CONFIG_t *md_con)
{
	SINT32 j, index, cnt, mb_off, mem_off;
	UINT32 u32MapPackageIndex;
	
	j = *pack_no;
	
	// Motion Detection Thresholds Initialization (0x3f82 ~ 0x3f8b)
	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0;
	ex_package[j].init_buff[2] = (0x8000 + 0x3f82);
	ex_package[j].init_buff[3] = (UINT16) md_con->u32SADThresholdValues[0];
	ex_package[j].init_buff[4] = (0x8000 + 0x3f83);
	ex_package[j].init_buff[5] = (UINT16) md_con->u32SADThresholdValues[1];
	ex_package[j].init_buff[6] = (0x8000 + 0x3f84);
	ex_package[j].init_buff[7] = (UINT16) md_con->u32SADThresholdValues[2];
	ex_package[j].init_buff[8] = (0x8000 + 0x3f85);
	ex_package[j].init_buff[9] = (UINT16) md_con->u32SADThresholdValues[3];
	ex_package[j].init_buff[10] = (0x8000 + 0x3f86);
	ex_package[j].init_buff[11] = (UINT16) md_con->u32MVThresholdValues[0];
	ex_package[j].init_buff[12] = (0x8000 + 0x3f87);
	ex_package[j].init_buff[13] = (UINT16) md_con->u32MVThresholdValues[1];
	ex_package[j].init_buff[14] = (0x8000 + 0x3f88);
	ex_package[j].init_buff[15] = (UINT16) md_con->u32MVThresholdValues[2];
	ex_package[j].init_buff[16] = (0x8000 + 0x3f89);
	ex_package[j].init_buff[17] = (UINT16) md_con->u32MVThresholdValues[3];
	ex_package[j].init_buff[18] = (0x8000 + 0x3f8a);
	ex_package[j].init_buff[19] = (UINT16) md_con->u32SensitivityValues[0];
	ex_package[j].init_buff[20] = (0x8000 + 0x3f8b);
	ex_package[j].init_buff[21] = (UINT16) md_con->u32SensitivityValues[1];
	ex_package[j].init_buff[22] = (0x8000 + 0x3f8c);
	ex_package[j].init_buff[23] = (UINT16) md_con->u32SensitivityValues[2];
	ex_package[j].init_buff[24] = (0x8000 + 0x3f8d);
	ex_package[j].init_buff[25] = (UINT16) md_con->u32SensitivityValues[3];
	ex_package[j].init_buff[26] = (0x8000 + 0x3f8e);
	ex_package[j].init_buff[27] = 0;	// gio_motion_detection_region_map
	ex_package[j].init_buff[28] = (0x8000 + 0x3f8f);
	ex_package[j].init_buff[29] = 0;	// gio_motion_detection_reserved

	j++;
	
#define	XRISC_MOTION_MAP_BASE_ADDR		    0x3ac0
#define	XRISC_MOTION_OUTPUT_MAP_BASE_ADDR	0x3b90
#define	XRISC_MEMORY_WRITE_MASK			    0x8000

	// Motion Detection Map Initialization (0x3af0 ~ 0x3bff)
	for (u32MapPackageIndex=0;
		  u32MapPackageIndex < (MAX_MACROBLOCKS_PER_PICTURE/NUM_MACROBLOCKS_PER_WORD/NUM_WORDS_PER_PACKAGE);
		  u32MapPackageIndex++)
	{
		ex_package[j].init_buff[0] = 0x2000 + NUM_WORDS_PER_PACKAGE;
		ex_package[j].init_buff[1] = 0;
		
		for (index=0; index < NUM_WORDS_PER_PACKAGE; index++ )
		{
			/* for each address within the map, properly format the macroblock map in 3 bit increments */
			ex_package[j].init_buff[(index*2)+2] = (XRISC_MEMORY_WRITE_MASK + XRISC_MOTION_MAP_BASE_ADDR
								+ u32MapPackageIndex*NUM_WORDS_PER_PACKAGE + index);
			ex_package[j].init_buff[(index*2)+3] =
			     (md_con->u32MacroblockMap[7 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 14)
			   | (md_con->u32MacroblockMap[6 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 12)
			   | (md_con->u32MacroblockMap[5 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 10)
			   | (md_con->u32MacroblockMap[4 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 8)
			   | (md_con->u32MacroblockMap[3 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 6)
			   | (md_con->u32MacroblockMap[2 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 4)
			   | (md_con->u32MacroblockMap[1 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD] << 2)
			   | (md_con->u32MacroblockMap[0 + u32MapPackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD + index*NUM_MACROBLOCKS_PER_WORD]);

		}
		
		j++;
	}

	{ // deal with remaining MAX_MACROBLOCKS_PER_PICTURE%(NUM_MACROBLOCKS_PER_WORD*NUM_WORDS_PER_PACKAGE) u32MacroblockMap
		cnt = MAX_MACROBLOCKS_PER_PICTURE%(NUM_MACROBLOCKS_PER_WORD*NUM_WORDS_PER_PACKAGE);
		cnt = (cnt/NUM_MACROBLOCKS_PER_WORD) + (cnt%NUM_MACROBLOCKS_PER_WORD? 1:0);
		if(cnt>0)
		{
			mb_off = (MAX_MACROBLOCKS_PER_PICTURE/NUM_MACROBLOCKS_PER_WORD/NUM_WORDS_PER_PACKAGE)*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD;
			mem_off = MAX_MACROBLOCKS_PER_PICTURE/NUM_MACROBLOCKS_PER_WORD;
			ex_package[j].init_buff[0] = 0x2000 + cnt;
			ex_package[j].init_buff[1] = 0;
			for(index=0;index<cnt;index++)
			{
				ex_package[j].init_buff[(index*2)+2] = (XRISC_MEMORY_WRITE_MASK + XRISC_MOTION_MAP_BASE_ADDR + mem_off + index);
				ex_package[j].init_buff[(index*2)+3] =
					 (md_con->u32MacroblockMap[MIN(7+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 14)
				   | (md_con->u32MacroblockMap[MIN(6+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 12)
				   | (md_con->u32MacroblockMap[MIN(5+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 10)
				   | (md_con->u32MacroblockMap[MIN(4+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 8)
				   | (md_con->u32MacroblockMap[MIN(3+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 6)
				   | (md_con->u32MacroblockMap[MIN(2+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 4)
				   | (md_con->u32MacroblockMap[MIN(1+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)] << 2)
				   | (md_con->u32MacroblockMap[MIN(0+mb_off+index*NUM_MACROBLOCKS_PER_WORD,MAX_MACROBLOCKS_PER_PICTURE-1)]);
			}
			j++;
		}
	}

	/* initialize the output map to 0 */
	for (u32MapPackageIndex=0;
		  u32MapPackageIndex < (MAX_MACROBLOCKS_PER_PICTURE/16/NUM_WORDS_PER_PACKAGE);
		  u32MapPackageIndex++)
	{
		ex_package[j].init_buff[0] = 0x2000 + NUM_WORDS_PER_PACKAGE;
		ex_package[j].init_buff[1] = 0;
		
		for (index=0; index < NUM_WORDS_PER_PACKAGE; index++ )
		{
			ex_package[j].init_buff[(index*2)+2] = (XRISC_MEMORY_WRITE_MASK + XRISC_MOTION_OUTPUT_MAP_BASE_ADDR
								+ u32MapPackageIndex*NUM_WORDS_PER_PACKAGE + index);
			ex_package[j].init_buff[(index*2)+3] = 0;
		}

		j++;
	}
	
	ex_package[j].init_buff[0] = 0x2000 + 6;
	ex_package[j].init_buff[1] = 0;
		
	for (index=0; index < 6; index++ )
	{
		ex_package[j].init_buff[(index*2)+2] = (XRISC_MEMORY_WRITE_MASK + XRISC_MOTION_OUTPUT_MAP_BASE_ADDR
								+ u32MapPackageIndex*NUM_WORDS_PER_PACKAGE + index);
		ex_package[j].init_buff[(index*2)+3] = 0;
	}

	j++;

   	*pack_no = j;
}

/************************** end of md_to_package ********************************/

void brctrl_to_package(TPackage *ex_package, SINT32 *pack_no, APRIL fool, UINT16 *dupframe_length) {
	SINT32 j;
	UINT16 temp;
	j = *pack_no;
	
	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0xBF20; // 0x3F20 : DQ_ADDR : dq
	ex_package[j].init_buff[3] = (UINT16) fool.DQ_ADDR;
	ex_package[j].init_buff[4] = 0xBF21; // 0x3F21 : SGOP_ADDR : sgop
	ex_package[j].init_buff[5] = (UINT16) fool.SGOP_ADDR;
	ex_package[j].init_buff[6] = 0xBF22; // 0x3F22 : QD0_ADDR : QD[0]
	ex_package[j].init_buff[7] = (UINT16) fool.QD0_ADDR;
	ex_package[j].init_buff[8] = 0xBF23; // 0x3F23 : QD1_ADDR : QD[1]
	ex_package[j].init_buff[9] = (UINT16) fool.QD1_ADDR;
	ex_package[j].init_buff[10] = 0xBF24; // 0x3F24 : QD2_ADDR : QD[2]
	ex_package[j].init_buff[11] = (UINT16) fool.QD2_ADDR;
	ex_package[j].init_buff[12] = 0xBF25; // 0x3F25 : QD3_ADDR : QD[3]
	ex_package[j].init_buff[13] = (UINT16) fool.QD3_ADDR;

	ex_package[j].init_buff[14] = 0xBF26; // 0x3F26-7 : SGOP_EXPT_ADDR : sgop_expt
	temp=(UINT16) (fool.SGOP_EXPT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[15] = temp; // lower 16 bits of SGOP_EXPT_ADDR
	ex_package[j].init_buff[16] = 0xBF27;
	temp=(UINT16) ((fool.SGOP_EXPT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[17] = temp; // upper 16 bits of SGOP_EXPT_ADDR

	ex_package[j].init_buff[18] = 0xBF28; // 0x3F28-9 : SGOP_PEAK_ADDR : afrm_peak
	temp=(UINT16) (fool.SGOP_PEAK_ADDR & 0x0000ffff);
	ex_package[j].init_buff[19] = temp; // lower 16 bits of SGOP_PEAK_ADDR
	ex_package[j].init_buff[20] = 0xBF29;
	temp=(UINT16) ((fool.SGOP_PEAK_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[21] = temp; // upper 16 bits of SGOP_PEAK_ADDR

	ex_package[j].init_buff[22] = 0xBF2A; // 0x3F2A-B : VBV_ALERT_ADDR : vbvAlert
	temp=(UINT16) (fool.VBV_ALERT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[23] = temp; // lower 16 bits of VBV_ALERT_ADDR
	ex_package[j].init_buff[24] = 0xBF2B;
	temp=(UINT16) ((fool.VBV_ALERT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[25] = temp; // upper 16 bits of VBV_ALERT_ADDR

	ex_package[j].init_buff[26] = 0xBF2C; // 0x3F2C-D : MIN_LMT_ADDR : MinLmt
	temp=(UINT16) (fool.MIN_LMT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[27] = temp; // lower 16 bits of MIN_LMT_ADDR
	ex_package[j].init_buff[28] = 0xBF2D;
	temp=(UINT16) ((fool.MIN_LMT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[29] = temp; // upper 16 bits of MIN_LMT_ADDR

   	j++;

	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0xBF2E; // 0x3F2E-F : MAX_LMT_ADDR : MaxLmt
	temp=(UINT16) (fool.MAX_LMT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[3] = temp; // lower 16 bits of MAX_LMT_ADDR
	ex_package[j].init_buff[4] = 0xBF2F;
	temp=(UINT16) ((fool.MAX_LMT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[5] = temp; // upper 16 bits of MAX_LMT_ADDR

	ex_package[j].init_buff[6] = 0xBF30; // 0x3F30-1 : COMPLEX0_ADDR : Complex[0]
	temp=(UINT16) (fool.COMPLEX0_ADDR & 0x0000ffff);
	ex_package[j].init_buff[7] = temp; // lower 16 bits of COMPLEX0_ADDR
	ex_package[j].init_buff[8] = 0xBF31;
	temp=(UINT16) ((fool.COMPLEX0_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[9] = temp; // upper 16 bits of COMPLEX0_ADDR

	ex_package[j].init_buff[10] = 0xBF32; // 0x3F32-3 : COMPLEX1_ADDR : Complex[1]
	temp=(UINT16) (fool.COMPLEX1_ADDR & 0x0000ffff);
	ex_package[j].init_buff[11] = temp; // lower 16 bits of COMPLEX1_ADDR
	ex_package[j].init_buff[12] = 0xBF33;
	temp=(UINT16) ((fool.COMPLEX1_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[13] = temp; // upper 16 bits of COMPLEX1_ADDR

	ex_package[j].init_buff[14] = 0xBF34; // 0x3F34-5 : COMPLEX2_ADDR : Complex[2]
	temp=(UINT16) (fool.COMPLEX2_ADDR & 0x0000ffff);
	ex_package[j].init_buff[15] = temp; // lower 16 bits of COMPLEX2_ADDR
	ex_package[j].init_buff[16] = 0xBF35;
	temp=(UINT16) ((fool.COMPLEX2_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[17] = temp; // upper 16 bits of COMPLEX2_ADDR

	ex_package[j].init_buff[18] = 0xBF36; // 0x3F36-7 : COMPLEX3_ADDR : Complex[3]
	temp=(UINT16) (fool.COMPLEX3_ADDR & 0x0000ffff);
	ex_package[j].init_buff[19] = temp; // lower 16 bits of COMPLEX3_ADDR
	ex_package[j].init_buff[20] = 0xBF37;
	temp=(UINT16) ((fool.COMPLEX3_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[21] = temp; // upper 16 bits of COMPLEX3_ADDR

	ex_package[j].init_buff[22] = 0xBF38; // 0x3F38-9 : SGOP_USED_ADDR : sgop_used
	temp=(UINT16) (fool.SGOP_USED_ADDR & 0x0000ffff);
	ex_package[j].init_buff[23] = temp; // lower 16 bits of SGOP_USED_ADDR
	ex_package[j].init_buff[24] = 0xBF39;
	temp=(UINT16) ((fool.SGOP_USED_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[25] = temp; // upper 16 bits of SGOP_USED_ADDR

	ex_package[j].init_buff[26] = 0xBF3A; // 0x3F3A-B : TOTAL_BITS_USED_ADDR : totl_used
	temp=(UINT16) (fool.TOTAL_BITS_USED_ADDR & 0x0000ffff);
	ex_package[j].init_buff[27] = temp; // lower 16 bits of TOTAL_BITS_USED_ADDR
	ex_package[j].init_buff[28] = 0xBF3B;
	temp=(UINT16) ((fool.TOTAL_BITS_USED_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[29] = temp; // upper 16 bits of TOTAL_BITS_USED_ADDR

   	j++;

	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0xBF3C; // 0x3F3C-D : TOTAL_EXPT_ADDR : totl_expt
	temp=(UINT16) (fool.TOTAL_EXPT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[3] = temp; // lower 16 bits of TOTAL_EXPT_ADDR
	ex_package[j].init_buff[4] = 0xBF3D;
	temp=(UINT16) ((fool.TOTAL_EXPT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[5] = temp; // upper 16 bits of TOTAL_EXPT_ADDR

	ex_package[j].init_buff[6] = 0xBF3E; // 0x3F3E-F : VBV_LEVEL_ADDR : vbv_lvl
	temp=(UINT16) (fool.VBV_LEVEL_ADDR & 0x0000ffff);
	ex_package[j].init_buff[7] = temp; // lower 16 bits of VBV_LEVEL_ADDR
	ex_package[j].init_buff[8] = 0xBF3F;
	temp=(UINT16) ((fool.VBV_LEVEL_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[9] = temp; // upper 16 bits of VBV_LEVEL_ADDR

	ex_package[j].init_buff[10] = 0xBF48; // 0x3F48 : FRAME_INDEX_ADDR : FrameIdx
	ex_package[j].init_buff[11] = fool.FRAME_INDEX_ADDR;
	ex_package[j].init_buff[12] = 0xBF49; // 0x3F49 : SGOP_Q_ADDR : sgop_q
	ex_package[j].init_buff[13] = fool.SGOP_Q_ADDR;
	ex_package[j].init_buff[14] = 0xBF4A; // 0x3F4A : Q_ADDR : Q
	ex_package[j].init_buff[15] = fool.Q_ADDR;
	ex_package[j].init_buff[16] = 0xBF4B; // 0x3F4B : SGOP_COEF_ADDR
	ex_package[j].init_buff[17] = fool.SGOP_COEF_ADDR;
	ex_package[j].init_buff[18] = 0xBF4C; // 0x3F4C : PQ_ADDR : PQ
	ex_package[j].init_buff[19] = fool.PQ_ADDR;
	ex_package[j].init_buff[20] = 0xBF4D; // 0x3F4D : NQ_ADDR
	ex_package[j].init_buff[21] = 0;
	ex_package[j].init_buff[22] = 0xBF4E; // 0x3F4E : MODE0_Q_ADDR : Mode[0]
	ex_package[j].init_buff[23] = fool.MODE0_Q_ADDR;
	ex_package[j].init_buff[24] = 0xBF4F; // 0x3F4F : MODE1_Q_ADDR : Mode[1]
	ex_package[j].init_buff[25] = fool.MODE1_Q_ADDR;
	ex_package[j].init_buff[26] = 0xBF50; // 0x3F50 : MODE2_Q_ADDR : Mode[2]
	ex_package[j].init_buff[27] = fool.MODE2_Q_ADDR;
	ex_package[j].init_buff[28] = 0xBF51; // 0x3F51 : MODE3_Q_ADDR : Mode[3]
	ex_package[j].init_buff[29] = fool.MODE3_Q_ADDR;

   	j++;

	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0xBF40; // 0x3F40-1 : SGOP_NEXT_ADDR
	temp=(UINT16) (fool.SGOP_NEXT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[3] = temp; // lower 16 bits of SGOP_NEXT_ADDR
	ex_package[j].init_buff[4] = 0xBF41;
	temp=(UINT16) ((fool.SGOP_NEXT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[5] = temp; // upper 16 bits of SGOP_NEXT_ADDR

	ex_package[j].init_buff[6] = 0xBF42; // 0x3F42-3 : SUM_Q_ADDR
	temp=(UINT16) (fool.SUM_Q_ADDR & 0x0000ffff);
	ex_package[j].init_buff[7] = temp; // lower 16 bits of SUM_Q_ADDR
	ex_package[j].init_buff[8] = 0xBF43;
	temp=(UINT16) ((fool.SUM_Q_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[9] = temp; // upper 16 bits of SUM_Q_ADDR

	ex_package[j].init_buff[10] = 0xBF44; // 0x3F44-5 : LMT_FRM
	ex_package[j].init_buff[11] = 0;
	ex_package[j].init_buff[12] = 0xBF45;
	ex_package[j].init_buff[13] = 0;
	ex_package[j].init_buff[14] = 0xBF46; // 0x3F46 : MB_ADDR
	ex_package[j].init_buff[15] = fool.MB_ADDR;
	ex_package[j].init_buff[16] = 0xBF47; // 0x3F47 : LMT_MB_ADDR
	ex_package[j].init_buff[17] = 0;
	ex_package[j].init_buff[18] = 0xBF64; // 0x3F64-5 : FRAME_LENGTH_DWORDS_ADDR
	ex_package[j].init_buff[19] = 0;
	ex_package[j].init_buff[20] = 0xBF65;
	ex_package[j].init_buff[21] = 0;
	ex_package[j].init_buff[22] = 0xBF18; // 0x3F18 : MP4_DUP_P_PREV_LEN
	ex_package[j].init_buff[23] = dupframe_length[4];
	ex_package[j].init_buff[24] = 0xBF19; // 0x3F19 : MP4_DUP_B_PREV_LEN
	ex_package[j].init_buff[25] = dupframe_length[5];
	ex_package[j].init_buff[26] = 0xBF1A; // 0x3F1A : MP4_DUP_B_POST_LEN
	ex_package[j].init_buff[27] = dupframe_length[6];
	ex_package[j].init_buff[28] = 0xBF1B; // 0x3F1B : MP4_DUP_B_BIDIR_LEN
	ex_package[j].init_buff[29] = dupframe_length[7];

   	j++;

#ifdef	_EXTRA_BRC_DEBUG
	ex_package[j].init_buff[0] = 0x200e;
#else
	ex_package[j].init_buff[0] = 0x2008;
#endif
	ex_package[j].init_buff[1] = 0x0000;

	ex_package[j].init_buff[2] = 0xBF56; // 0x3F56-7 : RC_SGOP0_ADDR : RC_SGOP[0]
	temp=(UINT16) (fool.RC_SGOP0_ADDR & 0x0000ffff);
	ex_package[j].init_buff[3] = temp; // lower 16 bits of RC_SGOP0_ADDR
	ex_package[j].init_buff[4] = 0xBF57;
	temp=(UINT16) ((fool.RC_SGOP0_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[5] = temp; // upper 16 bits of RC_SGOP0_ADDR

	ex_package[j].init_buff[6] = 0xBF58; // 0x3F58-9 : RC_SGOP1_ADDR : RC_SGOP[1]
	temp=(UINT16) (fool.RC_SGOP1_ADDR & 0x0000ffff);
	ex_package[j].init_buff[7] = temp; // lower 16 bits of RC_SGOP1_ADDR
	ex_package[j].init_buff[8] = 0xBF59;
	temp=(UINT16) ((fool.RC_SGOP1_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[9] = temp; // upper 16 bits of RC_SGOP1_ADDR

	ex_package[j].init_buff[10] = 0xBF5A; // 0x3F5A-B : RC_SGOP2_ADDR : RC_SGOP[2]
	temp=(UINT16) (fool.RC_SGOP2_ADDR & 0x0000ffff);
	ex_package[j].init_buff[11] = temp; // lower 16 bits of RC_SGOP2_ADDR
	ex_package[j].init_buff[12] = 0xBF5B;
	temp=(UINT16) ((fool.RC_SGOP2_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[13] = temp; // upper 16 bits of RC_SGOP2_ADDR

	ex_package[j].init_buff[14] = 0xBF5C; // 0x3F5C-D : RC_SGOP3_ADDR : RC_SGOP[3]
	temp=(UINT16) (fool.RC_SGOP3_ADDR & 0x0000ffff);
	ex_package[j].init_buff[15] = temp; // lower 16 bits of RC_SGOP3_ADDR
	ex_package[j].init_buff[16] = 0xBF5D;
	temp=(UINT16) ((fool.RC_SGOP3_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[17] = temp; // upper 16 bits of RC_SGOP3_ADDR
#ifdef	_EXTRA_BRC_DEBUG
	ex_package[j].init_buff[18] = 0xBF5E; // 0x3F5E-F : SUM_USED_ADDR : sum_used
	temp=(UINT16) (fool.SUM_USED_ADDR & 0x0000ffff);
	ex_package[j].init_buff[19] = temp; // lower 16 bits of SUM_USED_ADDR
	ex_package[j].init_buff[20] = 0xBF5F;
	temp=(UINT16) ((fool.SUM_USED_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[21] = temp; // upper 16 bits of SUM_USED_ADDR

	ex_package[j].init_buff[22] = 0xBF60; // 0x3F60-1 : SUM_EXPT_ADDR : sum_expt
	temp=(UINT16) (fool.SUM_EXPT_ADDR & 0x0000ffff);
	ex_package[j].init_buff[23] = temp; // lower 16 bits of SUM_EXPT_ADDR
	ex_package[j].init_buff[24] = 0xBF61;
	temp=(UINT16) ((fool.SUM_EXPT_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[25] = temp; // upper 16 bits of SUM_EXPT_ADDR

	ex_package[j].init_buff[26] = 0xBF62; // 0x3F62-3 : SUM_SGOP_ADDR : sum_sgop
	temp=(UINT16) (fool.SUM_SGOP_ADDR & 0x0000ffff);
	ex_package[j].init_buff[27] = temp; // lower 16 bits of SUM_SGOP_ADDR
	ex_package[j].init_buff[28] = 0xBF63;
	temp=(UINT16) ((fool.SUM_SGOP_ADDR & 0xffff0000)>>16);
	ex_package[j].init_buff[29] = temp; // upper 16 bits of SUM_SGOP_ADDR
#endif
   	j++;

	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;
	ex_package[j].init_buff[2] = 0xBF10; // 0x3F10: MPEG4_last_Pfrm_duplication_flag
	ex_package[j].init_buff[3] = 0x0000;
	ex_package[j].init_buff[4] = 0xBF11; // 0x3F11: MPEG4_modulo_time_base_flag
	ex_package[j].init_buff[5] = 0x0000;
	ex_package[j].init_buff[6] = 0xBF12; // 0x3F12: MPEG4_frame_number_offset
	ex_package[j].init_buff[7] = 0x0000;
	ex_package[j].init_buff[8] = 0xBF13; // 0x3F13: MPEG4_frame_number_base
	ex_package[j].init_buff[9] = 0x0000;
	ex_package[j].init_buff[10] = 0xBF14; // 0x3F14: MPEG4_frame_number_new_offset
	ex_package[j].init_buff[11] = 0x0000;
	ex_package[j].init_buff[12] = 0xBF15; // 0x3F15: MPEG4_frame_number_new_base
	ex_package[j].init_buff[13] = 0x0000;
	ex_package[j].init_buff[14] = 0xBF16; // 0x3F16: MPEG4_frame_number_low16
	ex_package[j].init_buff[15] = 0x0000;
	ex_package[j].init_buff[16] = 0xBF17; // 0x3F17: MPEG4_frame_number_high16
	ex_package[j].init_buff[17] = 0x0000;
	ex_package[j].init_buff[18] = 0xBF7E;
	ex_package[j].init_buff[19] = 0x0000;
	ex_package[j].init_buff[20] = 0xBF7F;
	ex_package[j].init_buff[21] = 0x0001;
	ex_package[j].init_buff[22] = 0xBF52; // 0x3F52 : DUPLICATION_P_PREV_LEN
	ex_package[j].init_buff[23] = dupframe_length[0];
	ex_package[j].init_buff[24] = 0xBF53; // 0x3F53 : DUPLICATION_B_PREV_LEN
	ex_package[j].init_buff[25] = dupframe_length[1];
	ex_package[j].init_buff[26] = 0xBF54; // 0x3F54 : DUPLICATION_B_POST_LEN
	ex_package[j].init_buff[27] = dupframe_length[2];
	ex_package[j].init_buff[28] = 0xBF55; // 0x3F55 : DUPLICATION_B_BIDIR_LEN
	ex_package[j].init_buff[29] = dupframe_length[3];

   	j++;
	*pack_no = j;
}

SINT32 MP4_modulo_FrmHead(SINT32 frm, SINT32 bit_length) {
	SINT32 i = 0;
	num[i] = 24; code[i ++] = 0x000001;					// prefix
	num[i] =  8; code[i ++] = 0xB6;						// vop_start_code
	num[i] =  2; code[i ++] = frm;						// vop_coding_type
	num[i] = 5+bit_length; code[i ++] = pcodeSet(1,4+bit_length)+	// modulo_time_base = 1
	                          pcodeSet(0,3+bit_length)+	// modulo_time_base = 0
	                          pcodeSet(1,2+bit_length)+	// marker
	                          pcodeSet(0,2)+			// vop_time_increment (should be ignored)
	                          pcodeSet(1,1)+			// marker
	                          (frm != D_FRAME);			// vop_coded
	if(frm == P_FRAME) {
		num[i] =  1; code[i ++] = 0;					// vop_rounding_type
	}
	num[i] =  8; code[i ++] = pcodeSet(0,  5) +	1;		// intra_dc_vlc_thr=0, vop_quant=1
	num[i] =  3; code[i ++] = 4;						// vop_fcode_forward [-63.5, 63.5]
	if(frm == B_FRAME) {
		num[i] =  3; code[i ++] = 4;					// vop_fcode_bckward [-63.5, 63.5]
	}
	return coding(i);
}

SINT32 MP4_FrmHead(SINT32 frm, SINT32 bit_length) {
	SINT32 i = 0;
	num[i] = 24; code[i ++] = 0x000001;					// prefix
	num[i] =  8; code[i ++] = 0xB6;						// vop_start_code
	num[i] =  2; code[i ++] = frm;						// vop_coding_type
	num[i] = 4+bit_length; code[i ++] = pcodeSet(0,3+bit_length)+	// modulo_time_base (disabled)
	                          pcodeSet(1,2+bit_length)+	// marker
	                          pcodeSet(0,2) +			// vop_time_increment (should be ignored)
	                          pcodeSet(1,1) +			// marker
	                          (frm != D_FRAME);			// vop_coded
	if(frm == P_FRAME) {
		num[i] =  1; code[i ++] = 0;					// vop_rounding_type
	}
	num[i] =  8; code[i ++] = pcodeSet(0,  5) +	1;		// intra_dc_vlc_thr=0, vop_quant=1
	num[i] =  3; code[i ++] = 4;						// vop_fcode_forward [-63.5, 63.5]
	if(frm == B_FRAME) {
		num[i] =  3; code[i ++] = 4;					// vop_fcode_bckward [-63.5, 63.5]
	}
	return coding(i);
}

SINT32 MP4_FrmTail(void) {
	return nextstartcode();
}

SINT32 H263_FrmHead(SINT32 cols) {
	SINT32 i = 0;
	UINT32 ptype = 0;
	SINT32 frame_no = 0;
	SINT32 frame_fq = 1;
	align(0);
	num[i] = 22; code[i ++] = 0x000020;
	num[i] =  8; code[i ++] = ((UINT32)frame_no) & 0xff;
	if (cols == 128) ptype = 0x1020;
	else if (cols == 176) ptype = 0x1040;
	else if (cols == 352) ptype = 0x1060;
	else if (cols == 704) ptype = 0x1080;
	ptype |= 0x10;
	num[i] = 13; code[i ++] = ptype;
	num[i] =  7; code[i ++] = pcodeSet(frame_fq, 2) + 0;
	return coding(i); 
}

void H263_PutGOB(TMP_BlkInfo *mb11,SINT32 cols) {
	SINT32 i = 0;
	if (mb11->col == 0){
		if (cols <= 352) {
			if (mb11->row != 0) {
				num[i] = 17; code[i ++] = 0x00001;
				num[i] =  5; code[i ++] = mb11->row;
				num[i] =  7; code[i ++] = mb11->Q;
			}
		}
		else if (cols == 704 && mb11->row % 2 == 0) {
			if (mb11->row != 0) {
				num[i] = 17; code[i ++] = 0x00001;
				num[i] =  5; code[i ++] = mb11->row / 2;
				num[i] =  7; code[i ++] = mb11->Q;
			}
		}
		else if (cols == 1408 && mb11->row % 4 == 0) {
			if (mb11->row != 0) {
				num[i] = 17; code[i ++] = 0x00001;
				num[i] =  5; code[i ++] = mb11->row / 4;
				num[i] =  7; code[i ++] = mb11->Q;
			}
		}
		coding(i);
	}
}

SINT32 H263_FrmTail(void) {
	return align(0);
}

SINT32 MP1_FrmHead(SINT32 frm) {
	SINT32 i = 0;
	SINT32 fno = 0;
	num[i] = 24; code[i ++] = 0x000001;
	num[i] =  8; code[i ++] = 0;
	num[i] = 13; code[i ++] = pcodeSet(fno, 3) +	// temporal reference
							  frm + 1;			// frame type
	num[i] = 16; code[i ++] = 0xffff;
	num[i] =  4; code[i ++] = pcodeSet(0, 3) + 4;// f_code
	if (frm == B_FRAME) {
		num[i] = 4; code[i ++] = pcodeSet(0, 3) + 4;
		num[i] = 3; code[i ++] = 0;
	}
	else {
		num[i] = 7; code[i ++] = 0;
	}
	coding(i);
	return align(0);
}

SINT32 MP1_PutAddrInc(SINT32 addrinc) {
	SINT32 i = 0;
	while (addrinc > 33)
	{
		num[i] = 11; code[i++] = 0x08; // macroblock_escape 
		addrinc -= 33;
	}
	num [i]		= addrinctab[addrinc-1][1]; // len
	code[i++]	= addrinctab[addrinc-1][0]; // code
	return coding(i);
}

SINT32 MP1_FrmTail(void) {
	return align(0);
}

SINT32 MP2_FrmHead(SINT32 frm, SINT32 pict_struct, SINT32 interlace, SINT32 dvd) {
	SINT32 i = 0;
	SINT32 fno = 0;
	num[i] = 24; code[i ++] = 0x000001;
	num[i] =  8; code[i ++] = 0;
	num[i] = 13; code[i ++] = pcodeSet(fno, 3) + frm + 1;
	num[i] = 16; code[i ++] = 0xFFFF;
	num[i] = 4; code[i ++] = 0x07;
	if (frm == B_FRAME) {
		num[i] = 4; code[i ++] = 0x07;
		num[i] = 3; code[i ++] = 0;
	}
	else {
		num[i] = 7; code[i ++] = 0;
	}
	num[i] = 24; code[i ++] = 0x000001;
	num[i] =  8; code[i ++] = 0xB5;					// extension_start_code 
	num[i] = 12; code[i ++] = pcodeSet(8, 8) + 0x44;	// extension_start_code_identifier = CODING_ID = 8
													// forward_horizontal_f_code = 4
	num[i] =  8; code[i ++] = (frm == B_FRAME) ? 0x44 : 0xFF;
													// backward_horizontal_f_code = 4, backward_vertical_f_code = 4
	num[i] = 16; code[i ++] = pcodeSet(0, 14) +		// intra_dc_precision = 0
													// picture_structure = 3, frame picture
							  pcodeSet((interlace==2 ? pict_struct : FRAME_STRUCT), 12) +		
							  ((dvd)? 0: pcodeSet((interlace==2 ? 1 : 0), 11)) +		// top_field_first = 0
//							  pcodeSet((interlace==2 ? 1 : 0), 11) +		// top_field_first = 0
							  pcodeSet((interlace==2 ? 0 : 1), 10) +		// frame_pred_frame_dct = 1
							  pcodeSet(0,  9) +		// concealment_motion_vectors  -- currently not implemented
							  pcodeSet(0,  8) +		// q_scale_type = 0
							  pcodeSet(0,  7) +		// intra_vlc_format = 0
							  pcodeSet(0,  6) +		// alternate_scan  = 0
							  pcodeSet(0,  5) +		// repeat_first_field = 0
							  pcodeSet((interlace==2 ? 0 : 1),  4) +		// chroma_420_type = 1
							  pcodeSet((interlace==2 ? 0 : 1),  3) +		// progressive_frame = 1
							  pcodeSet(0,  2) +		// composite_display_flag = 0
							  0;					// stuffing
	coding(i);
	return align(0);
}

void dupfrm_dump_to_dram(TPackage *ex_package, SINT32 *pack_no, UINT8 *buffer, SINT32 start_dram_addr) {
	SINT32 i, j, k;
	UINT16 *mjpeghdr_buffer = (UINT16*) malloc((DupFrame_size/2)*sizeof(UINT16));
	UINT16 start_device_mem = 0x3e00;
	SINT32 iCount = 28;
	memset(mjpeghdr_buffer, 0, (DupFrame_size/2)*sizeof(UINT16));
	memcpy((UINT8 *)mjpeghdr_buffer, buffer, DupFrame_size);
#ifdef _BIG_ENDIAN_ //#ifdef SPARC
#ifdef _little_endian_init_buffer_
	for(i=0;i<(DupFrame_size/2);i++)
		mjpeghdr_buffer[i]=((mjpeghdr_buffer[i]&0x00ff)<<8)+((mjpeghdr_buffer[i]&0xff00)>>8);
#endif
#endif
	k = (DupFrame_size/2);
	j = *pack_no;
	for (i = 0; i < k; i += iCount) {
		if (k - i < 28)	{
			iCount = k - i;
			ex_package[j].init_buff[0] = 0x4000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			ex_package[j].init_buff[31] = (start_dram_addr >> 8);
			j++;
			iCount = 28;
		}
		else if (0x4000 - start_device_mem > 28) {
			iCount = 28;
			ex_package[j].init_buff[0] = 0x1000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			j++;
			start_device_mem += (UINT16)iCount;
		}
		else {
			iCount = 0x4000 - start_device_mem;
			ex_package[j].init_buff[0] = 0x4000 + iCount;
			ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
			memcpy(&(ex_package[j].init_buff[2]), &mjpeghdr_buffer[i], iCount * sizeof(UINT16));
			ex_package[j].init_buff[31] = (start_dram_addr >> 8);
			start_dram_addr += 0x0100;
			start_device_mem = 0x3e00; 
			j++;
		}
	}
	*pack_no = j;
	free(mjpeghdr_buffer);
}

void seqhdr_dump_to_dram(TPackage *ex_package, SINT32 *pack_no, UINT8 *buffer, SINT32 start_dram_addr) {
	SINT32 j;
	UINT16 *buf = (UINT16*) malloc((seqHdr_size/2)*sizeof(UINT16));
	UINT16 start_device_mem = 0x3e00;
	SINT32 iCount = 28;
	memset(buf, 0, (seqHdr_size/2)*sizeof(UINT16));
	memcpy((UINT8 *)buf, buffer, seqHdr_size);
#ifdef _BIG_ENDIAN_ //#ifdef SPARC
#ifdef _little_endian_init_buffer_
	for(j=0;j<(seqHdr_size/2);j++)
		buf[j]=((buf[j]&0x00ff)<<8)+((buf[j]&0xff00)>>8);
#endif
#endif
	j = *pack_no;
	ex_package[j].init_buff[0] = 0x4080 + seqHdr_size/2;
	ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
	memcpy(&(ex_package[j].init_buff[2]), buf, sizeof(UINT16)*seqHdr_size/2);
	ex_package[j].init_buff[31] = (UINT16) start_dram_addr;
	j++;
	*pack_no = j;
	free(buf);
}

void dupfrmMP4_to_package(TPackage *ex_package, SINT32 *pack_no,
						 SINT32 width, SINT32 height,
						 CHIP_CONFIG *chip_con, UINT16 *dupframe_length) {
	SINT32 wismode = chip_con->wis_proprietary_stream;
	UINT8 *dupframe = (UINT8*) malloc(DupFrame_size*sizeof(UINT8));
	UINT8 *dupframeP = (UINT8*) malloc(1700*sizeof(UINT8));
	SINT32 i, bit_count, byte_count;
	memset(dupframe, 0, DupFrame_size);
	{
		/* FDUP_PREV P frame */
		memset(dupframeP, 0, dupPfrm_size);
		pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
		MP4_modulo_FrmHead(P_FRAME,vop_time_increment_bitlength);
		for(i=0;i<(width>>4)*(height>>4);i++) {
			num[0] = 1; code[0] = 1;
			coding(1);
		}
		bit_count=MP4_FrmTail();
		byte_count=bit_count>>3;
		dupframe_length[4]=(UINT16) bit_count;
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
		memcpy(dupframe, dupframeP, dupPfrm_size);

		/* FDUP_PREV B frame */
		memset(dupframeP, 0, dupBffrm_size);
		pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
		MP4_modulo_FrmHead(B_FRAME,vop_time_increment_bitlength);
		for(i=0;i<(width>>4)*(height>>4);i++) {
			if(wismode) {
			num[0] = 2; code[0] = 2;
				coding(1);
			}
			else {
				num[0] = 2; code[0] = 1;
				num[1] = 4; code[1] = 1;
				num[2] = 1; code[2] = 1;
				num[3] = 1; code[3] = 1;
				coding(4);
			}
		}
		bit_count=MP4_FrmTail();
		byte_count=bit_count>>3;
		dupframe_length[5]=(UINT16) bit_count;
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
		memcpy(&(dupframe[dupPfrm_size]), dupframeP, dupBffrm_size);

		/* FDUP_POST B frame */
		memset(dupframeP, 0, dupBbfrm_size);
		pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
		MP4_modulo_FrmHead(B_FRAME,vop_time_increment_bitlength);
		for(i=0;i<(width>>4)*(height>>4);i++) {
			if(wismode) {
				num[0] = 2; code[0] = 3;
				coding(1);
			}
			else {
				num[0] = 2; code[0] = 1;
				num[1] = 3; code[1] = 1;
				num[2] = 1; code[2] = 1;
				num[3] = 1; code[3] = 1;
				coding(4);
			}
		}
		bit_count=MP4_FrmTail();
		byte_count=bit_count>>3;
		dupframe_length[6]=(UINT16) bit_count;
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
		memcpy(&(dupframe[dupPfrm_size+dupBffrm_size]), dupframeP, dupBbfrm_size);

		/* FDUP_BIDIR B frame */
		memset(dupframeP, 0, dupBifrm_size);
		pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
		MP4_modulo_FrmHead(B_FRAME,vop_time_increment_bitlength);
		for(i=0;i<(width>>4)*(height>>4);i++) {
			if(wismode) {
				num[0] = 2; code[0] = 3;
				coding(1);
			}
			else {
				num[0] = 2; code[0] = 1;
				num[1] = 2; code[1] = 1;
				num[2] = 1; code[2] = 1;
				num[3] = 1; code[3] = 1;
				num[4] = 1; code[4] = 1;
				num[5] = 1; code[5] = 1;
				coding(6);
			}
		}
		bit_count=MP4_FrmTail();
		byte_count=bit_count>>3;
		dupframe_length[7]=(UINT16) bit_count;
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
		memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size]), dupframeP, dupBifrm_size);

		/* FDUP_EMPTY B frame */
		memset(dupframeP, 0, dupBefrm_size);
		pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
		MP4_modulo_FrmHead(B_FRAME,vop_time_increment_bitlength);
		bit_count=MP4_FrmTail();
		byte_count=bit_count>>3;
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
		memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size+dupBifrm_size]), dupframeP, dupBefrm_size);
	}

	if(chip_con->seqMode==3&&chip_con->dramType==DRAM7007U_8M)
		dupfrm_dump_to_dram(ex_package, pack_no, dupframe, DUP_MP4_Addr_B);
	else
		dupfrm_dump_to_dram(ex_package, pack_no, dupframe, DUP_MP4_Addr_P);

	free(dupframe);
	free(dupframeP);
}

void blank_Pframe(SINT32 width, SINT32 height, CHIP_CONFIG *chip_con, UINT8 *dup_mem) {
	SINT32 i, j, k, l, bit_count, col, row, MBAinc=0, pict_struct;
	SINT32 interlace = (chip_con->cfg_interlace ? (chip_con->cfg_deinterlace==2 ? 2 : 1) : 0);
	if(chip_con->compress_mode==MPEG4||chip_con->compress_mode==MPEG1||
		chip_con->compress_mode==MPEG2||chip_con->compress_mode==H263) {
		memset(dup_mem, 0, dupPfrm_size);
		pvlcInit(&vlc, 1, dup_mem+2);
		bit_count=coding(0);
		if(chip_con->compress_mode==MPEG4) {
			MP4_FrmHead(P_FRAME,vop_time_increment_bitlength);
			for(i=0;i<(width>>4)*(height>>4);i++) {
				num[0] = 1; code[0] = 1;
				coding(1);
			}
			bit_count=MP4_FrmTail();
		}
		if(chip_con->compress_mode==MPEG1||chip_con->compress_mode==MPEG2) {
			pict_struct=TOP_FIELD;
			if(chip_con->compress_mode==MPEG1)
				MP1_FrmHead(P_FRAME);
			else
				MP2_FrmHead(P_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
			for(j=0;j<(height>>4);j++) {
				for(k=0;k<(width>>4);k++) {
					col=k;
					row=j;
					i=0;
					if (col == 0) {
						num[i] = 24; code[i ++] = 0x000001;
						num[i] =  8; 
						if (interlace == 2 && row >= (height>>4) / 2) {
							code[i ++] = 1 + row - (height>>4) / 2;	// slice_start_code 
						}
						else
							code[i ++] = 1 + row;			// slice_start_code 
  						num[i] = 6; code[i ++] = pcodeSet(1, 1) + 0;	// quantiser_scale_code = 1
  						MBAinc = col + 1;					// first MBAinc denotes absolute position 
						coding(i); i = 0;
					}
					// check if macroblock can be skipped 
					if (col != 0 && col != (width>>4) - 1) {
						MBAinc++;
						coding(i);
						continue;
					}
					// macroblock cannot be skipped 
					// there's no VLC for 'No MC, Not Coded':
					// we have to transmit (0,0) motion vectors
					MP1_PutAddrInc(MBAinc); // macroblock_address_increment 
					MBAinc = 1;
					i=0;
					num [0] = 3; //mbtypetab[1][8].len
					code[0] = 1; //mbtypetab[1][8].code
					coding(1);
					if (interlace == 2) {
						num[i] = 2; code[i++] = 1; coding(i); i = 0;
					}
					// forward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					if (col == (width>>4) - 1) {
						align(0);
						if (interlace == 2 && row == (height>>4) / 2 - 1) {
							if(chip_con->compress_mode == MPEG1) MP1_FrmHead(P_FRAME);
							else {
								pict_struct = BOTTOM_FIELD;
								MP2_FrmHead(P_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
							}
						}
					}
				} // end of for(k)
			} // end of for(j)
			bit_count=MP1_FrmTail();
		}
		if(chip_con->compress_mode==H263) {
			H263_FrmHead(width);
			for(i=0;i<(height>>4);i++) {
				for(l=0;l<(width>>4);l++) {
					TMP_BlkInfo mb;	mb.col=l; mb.row=i;	mb.Q=2;
					H263_PutGOB(&mb,width);
					num[0] = 1; code[0] = 1;
					coding(1);
					if ((mb.col == (width>>4) - 1) &&
						((width>>4) <= 22 || ((width>>4) == 44 && mb.row % 2 == 0) ||
						((width>>4) == 88 && mb.row % 4 == 0)))
						align(0);
				}
			}
			bit_count=H263_FrmTail();
		}
		dup_mem[0]=(UINT8) (bit_count&0x000000ff);
		dup_mem[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
	}
}

void dupframe_to_package(TPackage *ex_package, SINT32 *pack_no,
						 SINT32 width, SINT32 height,
						 CHIP_CONFIG *chip_con, UINT16 *dupframe_length) {

	UINT8 *dupframe = (UINT8*) malloc(DupFrame_size*sizeof(UINT8));
	UINT8 *dupframeP = (UINT8*) malloc(1700*sizeof(UINT8));
	SINT32 flag=0;
	SINT32 i, j, k, l, bit_count, byte_count, col, row, MBAinc=0, pict_struct;
	SINT32 interlace = (chip_con->cfg_interlace ? (chip_con->cfg_deinterlace==2 ? 2 : 1) : 0);
	memset(dupframe, 0, (DupFrame_size-40));

	if(chip_con->compress_mode==MPEG4||chip_con->compress_mode==MPEG1||
		chip_con->compress_mode==MPEG2||chip_con->compress_mode==H263)
	{

		flag++;

		if (chip_con->compress_mode == MPEG4) {
			/* FDUP_PREV P frame */
			memset(dupframeP, 0, dupPfrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			MP4_FrmHead(P_FRAME,vop_time_increment_bitlength);
			for(i=0;i<(width>>4)*(height>>4);i++) {
				num[0] = 1; code[0] = 1;
				coding(1);
			}
			bit_count=MP4_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[0]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(dupframe, dupframeP, dupPfrm_size);

			/* FDUP_PREV B frame */
			memset(dupframeP, 0, dupBffrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			MP4_FrmHead(B_FRAME,vop_time_increment_bitlength);
			for(i=0;i<(width>>4)*(height>>4);i++) {
				if(chip_con->wis_proprietary_stream) {
					num[0] = 2; code[0] = 2;
					coding(1);
				}
				else {
					num[0] = 2; code[0] = 1;
					num[1] = 4; code[1] = 1;
					num[2] = 1; code[2] = 1;
					num[3] = 1; code[3] = 1;
					coding(4);
				}
			}
			bit_count=MP4_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[1]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size]), dupframeP, dupBffrm_size);

			/* FDUP_POST B frame */
			memset(dupframeP, 0, dupBbfrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			MP4_FrmHead(B_FRAME,vop_time_increment_bitlength);
			for(i=0;i<(width>>4)*(height>>4);i++) {
				if(chip_con->wis_proprietary_stream) {
					num[0] = 2; code[0] = 3;
					coding(1);
				}
				else {
					num[0] = 2; code[0] = 1;
					num[1] = 3; code[1] = 1;
					num[2] = 1; code[2] = 1;
					num[3] = 1; code[3] = 1;
					coding(4);
				}
			}
			bit_count=MP4_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[2]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size+dupBffrm_size]), dupframeP, dupBbfrm_size);

			/* FDUP_BIDIR B frame */
			memset(dupframeP, 0, dupBifrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			MP4_FrmHead(B_FRAME,vop_time_increment_bitlength);
			for(i=0;i<(width>>4)*(height>>4);i++) {
				if(chip_con->wis_proprietary_stream) {
					num[0] = 2; code[0] = 3;
					coding(1);
				}
				else {
					num[0] = 2; code[0] = 1;
					num[1] = 2; code[1] = 1;
					num[2] = 1; code[2] = 1;
					num[3] = 1; code[3] = 1;
					num[4] = 1; code[4] = 1;
					num[5] = 1; code[5] = 1;
					coding(6);
				}
			}
			bit_count=MP4_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[3]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size]),
				dupframeP, dupBifrm_size);

			/* FDUP_EMPTY B frame */
			memset(dupframeP, 0, dupBefrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			MP4_FrmHead(B_FRAME,vop_time_increment_bitlength);
			bit_count=MP4_FrmTail();
			byte_count=bit_count>>3;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size+dupBifrm_size]),
				dupframeP, dupBefrm_size);
		} // end of MPEG4

		if (chip_con->compress_mode == MPEG1||chip_con->compress_mode == MPEG2) {
			/* FDUP_PREV P frame */
			pict_struct=TOP_FIELD;
			memset(dupframeP, 0, dupPfrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			if(chip_con->compress_mode == MPEG1) MP1_FrmHead(P_FRAME);
			else MP2_FrmHead(P_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
			for(j=0;j<(height>>4);j++) {
				for(k=0;k<(width>>4);k++) {
					col=k;
					row=j;
					i=0;
					if (col == 0) {
						num[i] = 24; code[i ++] = 0x000001;
						num[i] =  8; 
						if (interlace == 2 && row >= (height>>4) / 2) {
							code[i ++] = 1 + row - (height>>4) / 2;	// slice_start_code 
						}
						else
							code[i ++] = 1 + row;			// slice_start_code 
  						num[i] = 6; code[i ++] = pcodeSet(1, 1) + 0;	// quantiser_scale_code = 1
  						MBAinc = col + 1;							// first MBAinc denotes absolute position 
						coding(i); i = 0;
					}
					// check if macroblock can be skipped 
					if (col != 0 && col != (width>>4) - 1) {
						MBAinc++;
						coding(i);
						continue;
					}
					// macroblock cannot be skipped 
					// there's no VLC for 'No MC, Not Coded':
					// we have to transmit (0,0) motion vectors
					MP1_PutAddrInc(MBAinc); // macroblock_address_increment 
					MBAinc = 1;
					i=0;
					num [0] = 3; //mbtypetab[1][8].len
					code[0] = 1; //mbtypetab[1][8].code
					coding(1);
					if (interlace == 2) {
						num[i] = 2; code[i++] = 1; coding(i); i = 0;
					}
					// forward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					if (col == (width>>4) - 1) {
						align(0);
						if (interlace == 2 && row == (height>>4) / 2 - 1) {
							if(chip_con->compress_mode == MPEG1) MP1_FrmHead(P_FRAME);
							else {
								pict_struct = BOTTOM_FIELD;
								MP2_FrmHead(P_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
							}
						}
					}
				} // end of for(k)
			} // end of for(j)
			bit_count=MP1_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[0]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(dupframe, dupframeP, dupPfrm_size);

			/* FDUP_PREV B frame */
			pict_struct=TOP_FIELD;
			memset(dupframeP, 0, dupBffrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
			else MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
			for(j=0;j<(height>>4);j++) {
				for(k=0;k<(width>>4);k++) {
					col=k;
					row=j;
					i=0;
					if (col == 0) {
						num[i] = 24; code[i ++] = 0x000001;
						num[i] =  8; 
						if (interlace == 2 && row >= (height>>4) / 2) {
							code[i ++] = 1 + row - (height>>4) / 2;	// slice_start_code 
						}
						else
							code[i ++] = 1 + row;			// slice_start_code 
  						num[i] = 6; code[i ++] = pcodeSet(1, 1) + 0;	// quantiser_scale_code = 1
  						MBAinc = col + 1;							// first MBAinc denotes absolute position 
						coding(i); i = 0;
					}
					// check if macroblock can be skipped 
					if (col != 0 && col != (width>>4) - 1) {
						MBAinc++;
						coding(i);
						continue;
					}
					// macroblock cannot be skipped 
					// there's no VLC for 'No MC, Not Coded':
					// we have to transmit (0,0) motion vectors
					MP1_PutAddrInc(MBAinc); // macroblock_address_increment 
					MBAinc = 1;
					i=0;
					num [0] = 4; //mbtypetab[2][8].len
					code[0] = 2; //mbtypetab[2][8].code
					coding(1);
					if (interlace == 2) { // motion type == MC_FIELD
						num[i] = 2; code[i++] = 1; coding(i); i = 0;
					}
					// forward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					if (col == (width>>4) - 1) {
						align(0);
						if (interlace == 2 && row == (height>>4) / 2 - 1) {
							if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
							else {
								pict_struct = BOTTOM_FIELD;
								MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
							}
						}
					}
				} // end of for(k)
			} // end of for(j)
			bit_count=MP1_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[1]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size]), dupframeP, dupBffrm_size);

			/* FDUP_POST B frame */
			pict_struct=TOP_FIELD;
			memset(dupframeP, 0, dupBbfrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
			else MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
			for(j=0;j<(height>>4);j++) {
				for(k=0;k<(width>>4);k++) {
					col=k;
					row=j;
					i=0;
					if (col == 0) {
						num[i] = 24; code[i ++] = 0x000001;
						num[i] =  8; 
						if (interlace == 2 && row >= (height>>4) / 2) {
							code[i ++] = 1 + row - (height>>4) / 2;	// slice_start_code 
						}
						else
							code[i ++] = 1 + row;			// slice_start_code 
  						num[i] = 6; code[i ++] = pcodeSet(1, 1) + 0;	// quantiser_scale_code = 1
  						MBAinc = col + 1;							// first MBAinc denotes absolute position 
						coding(i); i = 0;
					}
					// check if macroblock can be skipped 
					if (col != 0 && col != (width>>4) - 1) {
						// no DCT coefficients and neither first nor last macroblock of slice 
						MBAinc++;
						coding(i);
						continue;
					}
					// macroblock cannot be skipped 
					// there's no VLC for 'No MC, Not Coded':
					// we have to transmit (0,0) motion vectors
					MP1_PutAddrInc(MBAinc); // macroblock_address_increment 
					MBAinc = 1;
					i=0;
					num [0] = 3; //mbtypetab[2][4].len
					code[0] = 2; //mbtypetab[2][4].code
					coding(1);
					if (interlace == 2) { // motion type == MC_FIELD
						num[i] = 2; code[i++] = 1; coding(i); i = 0;
					}
					// backward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					if (col == (width>>4) - 1) {
						align(0);
						if (interlace == 2 && row == (height>>4) / 2 - 1) {
							if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
							else {
								pict_struct = BOTTOM_FIELD;
								MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
							}
						}
					}
				} // end of for(k)
			} // end of for(j)
			bit_count=MP1_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[2]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size+dupBffrm_size]), dupframeP, dupBbfrm_size);

			/* FDUP_BIDIR B frame */
			pict_struct=TOP_FIELD;
			memset(dupframeP, 0, dupBifrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
			else MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
			for(j=0;j<(height>>4);j++) {
				for(k=0;k<(width>>4);k++) {
					col=k;
					row=j;
					i=0;
					if (col == 0) {
						num[i] = 24; code[i ++] = 0x000001;
						num[i] =  8; 
						if (interlace == 2 && row >= (height>>4) / 2) {
							code[i ++] = 1 + row - (height>>4) / 2;	// slice_start_code 
						}
						else
							code[i ++] = 1 + row;			// slice_start_code 
  						num[i] = 6; code[i ++] = pcodeSet(1, 1) + 0;	// quantiser_scale_code = 1
  						MBAinc = col + 1;							// first MBAinc denotes absolute position 
						coding(i); i = 0;
					}
					// check if macroblock can be skipped 
					if (col != 0 && col != (width>>4) - 1) {
						// no DCT coefficients and neither first nor last macroblock of slice 
						MBAinc++;
						coding(i);
						continue;
					}
					// macroblock cannot be skipped 
					// there's no VLC for 'No MC, Not Coded':
					// we have to transmit (0,0) motion vectors
					MP1_PutAddrInc(MBAinc); // macroblock_address_increment 
					MBAinc = 1;
					i=0;
					num [0] = 2; //mbtypetab[2][12].len
					code[0] = 2; //mbtypetab[2][12].code
					coding(1);
					if (interlace == 2 ) { // motion type == MC_FIELD
						num[i] = 2; code[i++] = 1; coding(i); i = 0;
					}
					// forward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					// backward motion vectors, update predictors 
					if (interlace == 2) {
						num[i] = 1; code[i++] = pict_struct - 1;
						coding(i); i = 0;
					}
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					num [i]	=1;	// motionvectab[0].len;
					code[i++] =1;	// motionvectab[0].code;
					coding(i); i = 0;
					if (col == (width>>4) - 1) {
						align(0);
						if (interlace == 2 && row == (height>>4) / 2 - 1) {
							if(chip_con->compress_mode == MPEG1) MP1_FrmHead(B_FRAME);
							else {
								pict_struct = BOTTOM_FIELD;
								MP2_FrmHead(B_FRAME, pict_struct, interlace, chip_con->DVD_compliant);
							}
						}
					}
				} // end of for(k)
			} // end of for(j)
			bit_count=MP1_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[3]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size]),
				dupframeP, dupBifrm_size);
		} // end of MPEG1/2

		if (chip_con->compress_mode == H263) { // Only P frame involved
			memset(dupframeP, 0, dupPfrm_size);
			pvlcInit(&vlc, 1, dupframeP+2); bit_count=coding(0);
			H263_FrmHead(width);
			for(i=0;i<(height>>4);i++) {
				for(l=0;l<(width>>4);l++) {
					TMP_BlkInfo mb;	mb.col=l; mb.row=i;	mb.Q=2;
					H263_PutGOB(&mb,width);
					num[0] = 1; code[0] = 1;
					coding(1);
					if ((mb.col == (width>>4) - 1) && ((width>>4) <= 22 || ((width>>4) == 44 && mb.row % 2 == 0) || ((width>>4) == 88 && mb.row % 4 == 0)))
						align(0);
				}
			}
			bit_count=H263_FrmTail();
			byte_count=bit_count>>3;
			dupframe_length[0]=(UINT16) bit_count;
			dupframeP[0]=(UINT8) (bit_count&0x000000ff);
			dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
			memcpy(dupframe, dupframeP, dupPfrm_size);
		} // end of H263
	}

	bit_count=0;
	memset(dupframeP, 0, seqHdr_size);
	seqhead_to_package(SEQUENCE_TO_BUFFER,NULL,NULL,dupframeP,&bit_count,chip_con,width,height);

	if(bit_count) {
		flag++;
/*		byte_count=bit_count>>3;
		for(l=byte_count-1;l>=0;l--) dupframeP[l+2]=dupframeP[l];
		dupframeP[0]=(UINT8) (bit_count&0x000000ff);
		dupframeP[1]=(UINT8) ((bit_count&0x0000ff00)>>8);
*/		memcpy(&(dupframe[dupPfrm_size+dupBffrm_size+dupBbfrm_size
			+dupBifrm_size+dupBefrm_size]),dupframeP,seqHdr_size);
	}

	if(flag) dupfrm_dump_to_dram(ex_package, pack_no, dupframe, DUP_FRM_Addr);
	free(dupframe);
	free(dupframeP);
}

SINT32 relative_prime(SINT32 N, SINT32 M) {
	SINT32 a;
	if(M>N) { // make sure N>=M
		a=M;
		M=N;
		N=a;
	}
	while(M!=0) {
		a=N%M;
		N=M;
		M=a;
	}
	return N;
}

SINT32 calculate_aspect_ratio(SINT32 w,
							  SINT32 h,
							  CHIP_CONFIG *chip_con,
							  UINT8 *pw,
							  UINT8 *ph) {
	SINT32 aspect_ratio=1;
	SINT32 prime;
	if(chip_con->compress_mode==MPEG1)
	{ // adjust aspect ratio for MPEG1
		if(chip_con->video_format==2)
		{ // 60Hz TV system
			switch(chip_con->aspect_ratio)
			{
			case 1: aspect_ratio=1; break;
			case 2: aspect_ratio=12; break;
			case 3: aspect_ratio=6; break;
			default: break;
			}
		}
		else
		{ // 50Hz TV system
			switch(chip_con->aspect_ratio)
			{
			case 1: aspect_ratio=1; break;
			case 2: aspect_ratio=8; break;
			case 3: aspect_ratio=3; break;
			default: break;
			}
		}
	}
	else if(chip_con->compress_mode==MPEG2)
	{ // keep aspect_ratio untouched for MPEG2
		aspect_ratio = chip_con->aspect_ratio;
	}
	else if(chip_con->compress_mode==MPEG4)
	{ // adjust aspect ratio for MPEG4
		if(chip_con->myFOURCC==1) aspect_ratio=1; //#define DIVX_MPEG4 1 in Struct.h
		if(chip_con->aspect_ratio==2)
		{ // 4:3 picture
			prime=relative_prime(4*h,3*w);
			*pw=(UINT8) 4*h/prime;
			*ph=(UINT8) 3*w/prime;
			//if((chip_con->video_format==2)&&(*pw==10)&&(*ph==11))
			if((chip_con->video_format==2))
			{ // 525-type for 4:3 picture
				aspect_ratio=3;
			}
			//else if((chip_con->video_format==1||chip_con->video_format==3)&&(*pw==12)&&(*ph==11))
			else
			{ // 625-type for 4:3 picture
				aspect_ratio=2;
			}
			/*else
			{
				aspect_ratio=ASPECT_RATIO_EXTENEED_PAR;
			}*/
		}
		if(chip_con->aspect_ratio==3)
		{ // 16:9 picture
			prime=relative_prime(16*h,9*w);
			*pw=(UINT8) 16*h/prime;
			*ph=(UINT8) 9*w/prime;
			//if((chip_con->video_format==2)&&(*pw==40)&&(*ph==33))
			if((chip_con->video_format==2))
			{ // 525-type for 16:9 picture
				aspect_ratio=5;
			}
			//else if((chip_con->video_format==1||chip_con->video_format==3)&&(*pw==16)&&(*ph==11))
			else
			{ // 625-type for 16:9 picture
				aspect_ratio=4;
			}
			/*else
			{
				aspect_ratio=ASPECT_RATIO_EXTENEED_PAR;
			}*/
		}
	}
	return aspect_ratio;
}

void calculate_time_increment(SINT32 fps) {
	while(vop_time_increment_mask>fps) {
		vop_time_increment_mask=vop_time_increment_mask>>1;
		vop_time_increment_bitlength--;
	}
	vop_time_increment_mask=(vop_time_increment_mask<<1)+1;
	vop_time_increment_bitlength++;
}

void seqhead_to_package(SINT8 seqhead_choice,
						TPackage *ex_package,
						SINT32 *pack_no,
						UINT8 *seqhead,
						SINT32 *seqlen,
						CHIP_CONFIG *chip_con,
						SINT32 width,
						SINT32 height) {
	static SINT32 ratetab[8] = { 24000, 24024, 25025, 30000, 30030, 50050, 60000, 60060 };
	UINT8 *strhead;
	UINT8 par_width=0;
	UINT8 par_height=0;
	SINT32 i,j,headcount;
	SINT32 interlace_coding=(chip_con->cfg_interlace==1 && chip_con->cfg_deinterlace==2)? 1:0;
	SINT32 bit_rate=chip_con->target_bitrate/400;
	SINT32 aspect_ratio=calculate_aspect_ratio(width, height, chip_con, &par_width, &par_height);
	SINT32 fps=chip_con->Frame_rate;

	if ((chip_con->compress_mode & GO) ||
		chip_con->compress_mode == H263 ||
		chip_con->compress_mode == MOTIONJPEG) return;

	if(seqhead_choice==SEQUENCE_TO_BUFFER) strhead = seqhead;
	else strhead = (UINT8*) malloc(sizeof(UINT8)*seqHdr_size);
	memset(strhead, 0, seqHdr_size);

	if(interlace_coding) fps=fps*2;
//	if(ivtc&&DVD) fps=fps*5/4;

	pvlcInit(&vlc, 1, strhead+2);
	i = headcount = 0;

	/* Start writing MPEG1/2/4 sequence header */

	if(chip_con->compress_mode == MPEG4) { // MPEG4 sequence header
/*		while(vop_time_increment_mask>fps) {
			vop_time_increment_mask=vop_time_increment_mask>>1;
			vop_time_increment_bitlength--;
		}
		vop_time_increment_mask=(vop_time_increment_mask<<1)+1;
		vop_time_increment_bitlength++;
*/		{
			num[i] = 24; code[i++] = 0x000001;			// prefix
			num[i] =  8; code[i++] = 0xB0;				// visual_object_sequence_start_code
// Begin Changes for VBrick compatibility Shanker 09/17/06
// Changed MPEG4 profile level to SP3 for VBrick compatibility
#ifndef VBRICK_COMPAT 
			num[i] =  8; code[i++] = 0xF5;
			// Profile_and_level_indication: ASP@L5
#else
			num[i] =  8; code[i++] = 0x03;
			// Profile_and_level_indication: SP@L3   or Main@L3=0x33
#endif
// End Changes for VBrick compatibility Shanker 09/17/06

			num[i] = 24; code[i++] = 0x000001;			// prefix
			num[i] =  8; code[i++] = 0xB5;				// visual_object_start_code
			num[i] =  8; code[i++] = 0x09;
			num[i] = 24; code[i++] = 0x000001;			// prefix
			num[i] =  8; code[i++] = 0x00;				// video_object_start_code
			num[i] = 24; code[i++] = 0x000001;			// prefix
			num[i] =  8; code[i++] = 0x20;				// video_object_layer_start_code
// Begin Changes for VBrick compatibility Shanker 09/17/06
// Do not set Object Layer identifier for compatibility with VBrick 6XXXX
#ifndef VBRICK_COMPAT 
			num[i] = 17; code[i++] = pcodeSet(0,16) +	
				// random_accessible_vol = 0
			                         pcodeSet(1,8) +	
				// video_object_type_indication = 1 (video)
			                         pcodeSet(1,7) +	
				// is_object_layer_identifier = 1
			                         pcodeSet(2,3) +	
				// video_object_layer_verid = 0010
			                         1;
				// video_object_layer_priority = 001
#else
			num[i] = 10; code[i++] = pcodeSet(0,9) +
				// random_accessible_vol = 0
						 pcodeSet(1,1) +
				// video_object_type_indication = 1 (video)
				                 0;
				// is_object_layer_identifier = 0
#endif
// End Changes for VBrick compatibility Shanker 09/17/06


			num[i] =  4; code[i++] = aspect_ratio;		// aspect_ratio_info
			if(aspect_ratio==ASPECT_RATIO_EXTENEED_PAR) {
				num[i] =  8; code[i++] = par_width;
				num[i] =  8; code[i++] = par_height;
			}

			num[i] = 20; code[i++] = pcodeSet(0,19) +	// vol_control_parameter = 0
	                                 pcodeSet(0,17) +	// vol_shape = 00 (rectangular)
	                                 pcodeSet(1,16) +	// marker
	                                 fps;				// vop_time_increment_resolution = Frame_rate

			num[i] = 2 + vop_time_increment_bitlength;
			code[i++] = pcodeSet(1,vop_time_increment_bitlength+1)+	// marker
			            pcodeSet(1,vop_time_increment_bitlength)+	// fixed_vop_rate = 1
			            1001;							// fixed_vop_time_increment

			num[i] = 14; code[i++] = pcodeSet(1,13) +	// marker
			                         width;				// video_object_layer_width

			num[i] = 14; code[i++] = pcodeSet(1,13) +	// marker
			                         height;			// video_object_layer_height

// Begin Changes for VBrick compatibility Shanker 09/17/06
#ifndef VBRICK_COMPAT 
			num[i] = 14; code[i++] = pcodeSet(1,13) +	
						// marker
			                         pcodeSet(0,12) +	
						// interlaced = 0
			                         pcodeSet(1,11) +	
						// obmc_disable = 1
			                         pcodeSet(0,9) +	
						// sprite_enable = 00   // this is only 1-bit in old spec
			                         pcodeSet(0,8) +	
						// not_8_bit = 0
			                         pcodeSet(0,7) +	
						// vol_quant_type = 0
			                         pcodeSet(0,6) +	
						// vol_quanter_pixel = 0  // this does not exist in old spec
			                         pcodeSet(1,5) +	
						// complexity_estimatiob_disabled = 1
			                         pcodeSet(1,4) +	
						// resync_marker_disabled = 1
			                         pcodeSet(0,3) +	
						// data_partitioning_enabled = 0
			                         pcodeSet(0,2) +	
						// newpred_enable = 0   // this does not exist in old spec
			                         pcodeSet(0,1) +	
						// reduced_resolution_vop_enable = 0 // this does not exist in old spec
			                         0;				
						// scalability = 0
#else
			num[i] = 10; code[i++] = pcodeSet(1,9) +
						// marker
			                         pcodeSet(0,8) +
						// interlaced = 0
			                         pcodeSet(1,7) +
						// obmc_disable = 1
			                         pcodeSet(0,6) +
						// sprite_enable = 0
			                         pcodeSet(0,5) +
						// not_8_bit = 0
			                         pcodeSet(0,4) +
						// vol_quant_type = 0
			                         pcodeSet(1,3) +
						// complexity_estimatiob_disabled = 1
			                         pcodeSet(1,2) +
						// resync_marker_disabled = 1
			                         pcodeSet(0,1) +
						// data_partitioning_enabled = 0
						0;
						// scalability = 0
#endif
// End Changes for VBrick compatibility Shanker 09/17/06
			coding(i);
			headcount = nextstartcode();				// stuffing
		}
	}
	if(chip_con->compress_mode == MPEG1 || chip_con->compress_mode == MPEG2)
	{ // MPEG1 and MPEG2 common sequence header
		num[i] = 24; code[i++] = 0x000001;		// prefix
		num[i] =  8; code[i++] = 0xB3;			// sequence_start_code
		num[i] = 12; code[i++] = width;			// horizontal_size_value
		num[i] = 12; code[i++] = height;		// vertical_size_value
		num[i] =  4; code[i++] = aspect_ratio;	// aspect_ratio
		for(j=0;j<8;j++) {
			if(ratetab[j]==fps)
				break;
		}
		if(j==8) {
			num[i] =  4; code[i++] = 4;			// default 29.97 fps
		}
		else {
			num[i] =  4; code[i++] = j+1;		// frame_rate_code
		}
		if(chip_con->target_bitrate==0) {
			if(chip_con->compress_mode==MPEG1)
				bit_rate = 0x3FFFF;
			if(chip_con->compress_mode==MPEG2) {
				if(chip_con->DVD_compliant==1)	// 9.8Mbps for DVD
					bit_rate = 9800000/400;
				else // 15Mbps for Main level@Main profile
					bit_rate = 15000000/400;
			}
		}
		num[i] = 19; code[i++] = pcodeSet(bit_rate,1) +	// bit_rate_value
		                         1;						// marker
		num[i] = 13; code[i++] = pcodeSet((chip_con->compress_mode==MPEG2)? 112:20,3) + // vbv_buffer_size_value
		                         pcodeSet(0,2) +	// constrained_parameters_flag = 0
		                         pcodeSet(0,1) +	// load_intra_quantizer_matrix = 0
		                         0;					// load_inter_quantizer_matrix = 0
		coding(i);
		headcount = align(0);						// stuffing
		//headcount = 12 * 8;
	}
	if(chip_con->compress_mode == MPEG2)
	{ // MPEG2 extension sequence header
		i = 0;
		num[i] = 24; code[i++] = 0x000001;
		num[i] =  8; code[i++] = 0xB5;				// extension_start_code 
		num[i] = 12; code[i++] = pcodeSet(1,8) +	// extension_start_code_identifier = SEQ_ID = 1
								 pcodeSet(4,4) +	// profile indication = 0100 (main)
								 8;					// level indication = 1000 (main)
		num[i] = 20; code[i++] = pcodeSet(1-interlace_coding,19) +	// progressive sequence
								 pcodeSet(1,17) +	// chroma_format 
								 pcodeSet(0,15) +	// horizontal_size_extension, width >> 12
								 pcodeSet(0,13) +	// vertical_size_extension, height >> 12
								 pcodeSet(0,1) +	// bit_rate_extension, bitrate/400 >> 18
								 1;					// marker_bit 
		num[i] = 16; code[i++] = pcodeSet(0,8) +	// vbv_buffer_size_extension, vbv_buffer_size >> 10
								 pcodeSet(0,7) +	// low_delay  -- currently not implemented
								 pcodeSet(0,5) +	// frame_rate_extension_n
								 0;					// frame_rate_extension_d 
		coding(i);
		headcount = align(0);						// stuffing
		//headcount = 22 * 8;
//SJ ? why not for SEQUENCE_TO_BUFFER
//		if(seqhead_choice==SEQUENCE_TO_PACKAGE) {
			i = 0;
			num[i] = 24; code[i++] = 0x000001;
			num[i] =  8; code[i++] = 0xB5;				// extension_start_code 
			num[i] = 16; code[i++] = pcodeSet(2,12) +	// extension_start_code_identifier = DISP_ID = 2
									 pcodeSet(chip_con->video_format,9) +
									 pcodeSet(1,8) +	// colour_description = 1
									 5;					// colour_primaries = 5
			num[i] = 16; code[i++] = pcodeSet(5,8) +	// transfer_characteristics = 5
									 5;					// matrix_coefficients = 5
			num[i] = 15; code[i++] = pcodeSet(width,1)+	// display_horizontal_size = width
									 1;					// marker_bit 
			num[i] = 14; code[i++] = height;			// display_vertical_size = height
			coding(i);
			headcount = align(0);						// stuffing
			//headcount = 34 * 8;
//		}
//SJ ? end - why not for SEQUENCE_TO_BUFFER
	}

	/* End writing MPEG1/2/4 sequence header */

	strhead[0]=(UINT8) (headcount&0x000000ff);
	strhead[1]=(UINT8) ((headcount&0x0000ff00)>>8);

	if(seqhead_choice==SEQUENCE_TO_BUFFER) {
		*seqlen = headcount;
	}
	else { // it is SEQUENCE_TO_PACKAGE
		j = *pack_no;
		ex_package[j].init_buff[0] = 0x2006;
		ex_package[j].init_buff[1] = 0;
		ex_package[j].init_buff[2] = 0x8000 + 0x3f08;	// frame_rate
		ex_package[j].init_buff[3] = (UINT16) fps;
		ex_package[j].init_buff[4] = 0x8000 + 0x3f09;
		ex_package[j].init_buff[5] = 0;
		ex_package[j].init_buff[6] = 0x8000 + 0x3ff2;	// mpeg4 vop_time_inc_bit_length
		ex_package[j].init_buff[7] = (UINT16) vop_time_increment_bitlength;
		ex_package[j].init_buff[8] = 0x8000 + 0x3ff3;	// mpeg4 vop_time_increment_mask
		ex_package[j].init_buff[9] = (UINT16) vop_time_increment_mask;
		ex_package[j].init_buff[10] = 0x8000 + 0x3fe6;	// mpeg4_hour
		ex_package[j].init_buff[11] = 0;
		ex_package[j].init_buff[12] = 0x8000 + 0x3fe7;	// Frame_rate/1000
		ex_package[j].init_buff[13] = ((UINT16) (fps/1000)) << 8;
		j++;
		if(chip_con->seq_header_enable==1) {
			headcount -= 8 * 8;
			ex_package[j].init_buff[0] = 0x2007;
			ex_package[j].init_buff[1] = 0;
			for (i = 0; i < 8; i += 2)
			{
				ex_package[j].init_buff[i + 2] = 0x8000 + 0x4800 + (i >> 1); // mpegx_buff_header
				ex_package[j].init_buff[i + 3] = (strhead[i + 2] << 8) + strhead[i + 3];
			}
			ex_package[j].init_buff[10] = 0x8000 + 0x4406; // mpegx_header_count
			ex_package[j].init_buff[11] = 64;
			ex_package[j].init_buff[12] = 0x8000 + 0x4407; // mpegx_blk_count
			ex_package[j].init_buff[13] = (UINT16) headcount;
			ex_package[j].init_buff[14] = 0x8000 + 0x461b; // sch_save_mpegx
			ex_package[j].init_buff[15] = 1;
			j++;
			headcount = (headcount + 31) / 32 * 2;
			ex_package[j].init_buff[0] = 0x2000 + headcount;
			ex_package[j].init_buff[1] = 0;
			for (i = 0; i < headcount * 2; i += 2) {
				ex_package[j].init_buff[i + 2] = 0x8000 + 0x4808 + (i >> 1); // mpegx_buff_blk
				ex_package[j].init_buff[i + 3] = (strhead[i + 10] << 8) + strhead[i + 11];
			}
			j++;
		}
		*pack_no = j;
		free(strhead);
	}
}

void gen_osdindex_pack(TPackage *ex_package, SINT32 *pack_no, SINT32 height)
{

	SINT32 j;
	UINT16 start_device_mem;
	j = *pack_no;
	
	start_device_mem = 0x3a00;	// OSD index map initialization 0x3a00 ~ 0x3aef
	ex_package[j].init_buff[0] = 0x1000 + 2;
	ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
	ex_package[j].init_buff[2] = 0xaaaa;	// OSD_EOF
	ex_package[j].init_buff[3] = 0xaaaa;	// stuffing,because gpmem need DWORD write

	j++;

	*pack_no = j;
}

void gen_quantchange_pack(TPackage *ex_package, SINT32 *pack_no, SINT8 test_quant_change_in_mb)
{
	SINT32 j, k;
	UINT16 start_device_mem;

	start_device_mem = 0x3fe0;
	j = *pack_no;
	ex_package[j].init_buff[0] = 0x1000 + 16;
	ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
	for (k = 0; k < 8; k ++) {
		ex_package[j].init_buff[2 * k + 2] = test_quant_change_in_mb ? quantizer_scale_matrix[k] : 0;
		ex_package[j].init_buff[2 * k + 3] = test_quant_change_in_mb ? ACCoeff_threshold_matrix[(k+15)%16] : 0;
	}
	j++;

	start_device_mem = 0x3ff0;
	ex_package[j].init_buff[0] = 0x1000 + 16;
	ex_package[j].init_buff[1] = 0x8000 + start_device_mem;
	for (k = 0; k < 8; k ++) {
		ex_package[j].init_buff[2 * k + 2] = test_quant_change_in_mb ? quantizer_scale_matrix[k + 8] : 0;
		ex_package[j].init_buff[2 * k + 3] = test_quant_change_in_mb ? ACCoeff_threshold_matrix[k + 7] : 0;
	}
	j++;
	*pack_no = j;
}

void avsync_to_package(TPackage *ex_package, SINT32 *pack_no, SINT32 ratev, SINT32 ratea) {
	SINT32 j = *pack_no;
	SINT32 I,F1,F2,prime;
	ratea = ratea * 1001;
	I = ratea/ratev;
	prime = relative_prime(ratev,ratea%ratev);
	F1 = (ratea%ratev)/prime;
	F2 = ratev/prime - F1;
	ex_package[j].init_buff[0] = 0x200e;
	ex_package[j].init_buff[1] = 0x0000;
	ex_package[j].init_buff[2] = 0x8000 + 0x3f90; // TS (audio timestamp)
	ex_package[j].init_buff[3] = 0x0000;
	ex_package[j].init_buff[4] = 0x8000 + 0x3f91; // adjustS
	ex_package[j].init_buff[5] = 0x0000;
	ex_package[j].init_buff[6] = 0x8000 + 0x3f92; // current decision
	ex_package[j].init_buff[7] = 0x0000;
	ex_package[j].init_buff[8] = 0x8000 + 0x3f93; // last decision
	ex_package[j].init_buff[9] = 0x0000;
	ex_package[j].init_buff[10] = 0x8000 + 0x3ff4; // max(F1,F2)
	ex_package[j].init_buff[11] = (UINT16) (F1>F2? F1:F2);
	ex_package[j].init_buff[12] = 0x8000 + 0x3ff5; // min(F1,F2)
	ex_package[j].init_buff[13] = (UINT16) (F1<F2? F1:F2);
	ex_package[j].init_buff[14] = 0x8000 + 0x3ff6; // I+(max(F1,F2)==F1)
	ex_package[j].init_buff[15] = (UINT16) (I+((F1>F2? F1:F2)==F1));
	ex_package[j].init_buff[16] = 0x8000 + 0x3ff7; // I+(max(F1,F2)==F2)
	ex_package[j].init_buff[17] = (UINT16) (I+((F1>F2? F1:F2)==F2));
	ex_package[j].init_buff[18] = 0x8000 + 0x3ff8; // sum
	ex_package[j].init_buff[19] = 0x0000;
	ex_package[j].init_buff[20] = 0x8000 + 0x3ff9; // P
	ex_package[j].init_buff[21] = 0x0000;
	ex_package[j].init_buff[22] = 0x8000 + 0x3ffa; // I*alpha
	ex_package[j].init_buff[23] = (UINT16) (I*215/100);
	ex_package[j].init_buff[24] = 0x8000 + 0x3ffb; // -I*alpha
	ex_package[j].init_buff[25] = (UINT16) ((-1*I*215/100)&0xFFFF);
	ex_package[j].init_buff[26] = 0x8000 + 0x3f94; // lower 16-bit S
	ex_package[j].init_buff[27] = 0x0000;
	ex_package[j].init_buff[28] = 0x8000 + 0x3f95; // upper 16-bit S
	ex_package[j].init_buff[29] = 0x0000;
	j++;
	*pack_no = j;
}

void generator(TPackage *package_buf, SINT32 *pack_no, TInitBuffer *tBuff,
			   CHIP_CONFIG *chip_con, OSD_CONFIG *osd_con, IIP_CONFIG *iip_con, MOTION_DETECTION_CONFIG_t *md_con,
			   DEINTERLACE_CONFIG *deinterlace_con, AUDIOS_CONFIG *audio_con,
			   SINT32 width, SINT32 height, SINT8 fgs_elbp, SINT8 additional_io,
			   UINT8 *osd_font, SINT32 osd_font_size, SINT8 reset_type) {
					   
	UINT16 dupframe_length[8]={0,0,0,0,0,0,0,0};
	APRIL fool;
	UINT8 *seqhdr_buffer;
	SINT32 seqhdr_bitcnt=0;

	// For registers initialization
	gen_dqvmec_pack(package_buf, pack_no, tBuff);
	gen_mpegx_reg_pack(package_buf, pack_no, tBuff, chip_con->compress_mode);

	// For SRAM initialization
	if(chip_con->dump_to_dram>=1) {	
		gen_dma_pack(package_buf, pack_no, tBuff);
		if(reset_type==0) { // in case quick reset, we skip the following packages
			gen_icxy_ivlc_pack(package_buf, pack_no, tBuff);
			gen_mpegx_tbl_pack(package_buf, pack_no, tBuff, chip_con->compress_mode);
			gen_sch_pack(package_buf, pack_no, tBuff, chip_con->compress_mode, chip_con->clock_rate);
		}
	}

	calculate_time_increment(chip_con->Frame_rate);

	// For DRAM initialization
	if(reset_type==0) { // Normal reset, we go through all DRAM packages
		if(chip_con->dump_to_dram>=2) {	
			gen_qdeq_tbl_pack(package_buf, pack_no, tBuff, chip_con->compress_mode, 1, 1, NULL, NULL);
			gen_vlcbook_pack(package_buf, pack_no, tBuff);
			if(osd_font_size==0)
				gen_osdfont_pack(package_buf, pack_no, tBuff, chip_con->seqMode);
			else
				gen_osdfont_pack2(package_buf, pack_no, osd_font, osd_font_size, chip_con->seqMode);
		}

		if(chip_con->compress_mode==MOTIONJPEG)
			gen_mjpeghdr_to_package(package_buf, pack_no, width>>4, height>>4);
		else
			dupframe_to_package(package_buf, pack_no, width, height, chip_con, dupframe_length);

		if(chip_con->compress_mode==MPEG4)
			dupfrmMP4_to_package(package_buf, pack_no, width, height, chip_con, dupframe_length);
	}
	else { // Quick reset, we skip all DRAM packages except for seqhead buffer
		seqhdr_buffer = (UINT8*) malloc(seqHdr_size*sizeof(UINT8));
		memset(seqhdr_buffer, 0, seqHdr_size);
		seqhead_to_package(SEQUENCE_TO_BUFFER,NULL,NULL,seqhdr_buffer,&seqhdr_bitcnt,chip_con,width,height);
		if(seqhdr_bitcnt)
			seqhdr_dump_to_dram(package_buf, pack_no, seqhdr_buffer, SeqHeader_Addr);
		free(seqhdr_buffer);
	}

	// Calculate BRC related parameters
	Setup(&fool,chip_con->target_bitrate, chip_con->brctrl_peak_rate, chip_con->brctrl_vbv,
		chip_con->brctrl_converge_speed, chip_con->brctrl_LAMBDA, chip_con->brctrl_Q,
		chip_con->Frame_rate, (width*height), (chip_con->nSubGOP - 1),
		chip_con->brctrl_qmin, chip_con->brctrl_qmax, chip_con->brctrl_window_size);

	brctrl_to_package(package_buf, pack_no, fool, dupframe_length);

	osddeinter_to_package(package_buf, pack_no, deinterlace_con, osd_con);

	gen_osdindex_pack(package_buf, pack_no, height);

	gen_gpio_pack(package_buf, pack_no, tBuff, fgs_elbp, chip_con);

	gen_quantchange_pack(package_buf, pack_no, (SINT8) chip_con->test_quant_change_in_mb);

	config_package(package_buf, pack_no, chip_con, deinterlace_con,	width, height);

	seqhead_to_package(SEQUENCE_TO_PACKAGE, package_buf, pack_no, NULL, NULL, chip_con, width, height);

	if(chip_con->cfg_iip_enable)
		iip_to_package(package_buf, pack_no, chip_con, iip_con);

	if(chip_con->cfg_motion_detection_enable)
		md_to_package(package_buf, pack_no, md_con);

	if(chip_con->has_audio_package)
		audio_to_package(package_buf, pack_no, audio_con);

	avsync_to_package(package_buf, pack_no, chip_con->Frame_rate, audio_con->reserved);

	if(additional_io)
		gen_addio_pack(package_buf, pack_no, tBuff);

	final_package(package_buf, pack_no, fgs_elbp, width, height, chip_con);
}

/****************************** end of PacGen_7007SB.c ***********************/
