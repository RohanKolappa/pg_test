/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*   FILE: 
*		motion_detection.h
*
*   DESCRIPTION:
*		Motion Detection header file
*
*	NOTES:
*
*   $Id: motion_detection.h,v 1.2 2007-01-10 22:47:29 sjain Exp $
*
******************************************************************************/

#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include "wis_types.h"
#include "PacGen_7007SB.h"

#define ENCODER_THRESHOLD_MARKER		0x0041
#define	ENCODER_THRESHOLD_RETURN		0x0040

#define	ENCODER_MACROBLOCK_MAP_MARKER	0x0042
#define	ENCODER_MACROBLOCK_MAP_RETURN	0x0040

/* NOTE:  This structure is little-endian only */
typedef struct motion_threshold_package_s {
    uint16 u16MotionThresholdMarker;	/* actual value 0x0041 */
    uint16 u16SADThresholdValues[4];	/* Give the SAD thresholds in order of region 1, region 2, region 3, region 4 */
    uint16 u16Reserved1[3];
    uint16 u16MVThresholdValues[4];	/* Give the Motion Vector thresholds in order of region 1, region 2, region3, region 4 */
    uint16 u16Reserved2[3];
    uint16 u8SensitivityValues[4];	/* number of times each region must have macroblocks that exceed the threshold before signaling the host within a frame */
    uint16 u16Reserved3[13];
} motion_threshold_package_t;

typedef struct motion_macroblock_map_package_s {

    uint16 u16MotionPackageMarker;	/* actual value 0x0042 */
    uint16 u16StartingMacroblockNumber;	/* macroblock number (must be divided by 8 (ie 0, 30, 60, 90, ..., 150 (if D1)) */
    uint16 u16MacroblockMap[30];	/* bits 0-1, 2-3, 4-5, ..., 15-16 represent a macroblock, in that order. */

} motion_macroblock_map_package_t;

#define PACKAGE_LEN_WORDS                       32
#define NUM_WORDS_PER_PACKAGE                   30
#define	NUM_MACROBLOCKS_PER_PACKAGE				240
#define	NUM_MACROBLOCKS_PER_WORD16				8
#define	MAX_MACROBLOCK_MAP_PACKAGES				(MAX_NUM_MACROBLOCKS/NUM_MACROBLOCKS_PER_PACKAGE+1)

#ifdef __linux__
#define ENCODER_SUCCESS 0
#define ENCODER_FAILURE 1
#endif

/* FIXUP: Dan - this does not go in this file. This should be in platform.h */
#if defined(_BIG_ENDIAN_)
#define	LITTLE_ENDIAN_TO_CPU_ENDIAN16(dest, source)						\
		  dest = ((source << 8) | (source >> 8))
#define LITTLE_ENDIAN16_GET_LSB(myShort)				\
				((myShort & 0xFF))
#define LITTLE_ENDIAN16_GET_MSB(myShort)				\
				((myShort & 0xFF00) >> 8)
#define LITTLE_ENDIAN16_PUT_LSB(myByte)					\
				((myByte & 0xFF))
#define LITTLE_ENDIAN16_PUT_MSB(myByte)					\
				((myByte & 0xFF) << 8)
#else /* it's little endian */
#define	LITTLE_ENDIAN_TO_CPU_ENDIAN16(dest, source)						\
		  dest = source
#define LITTLE_ENDIAN16_GET_LSB(myShort)				\
				((myShort & 0xFF00) >> 8)
#define LITTLE_ENDIAN16_GET_MSB(myShort)				\
				((myShort & 0xFF))
#define LITTLE_ENDIAN16_PUT_LSB(myByte)					\
				((myByte & 0xFF) << 8)
#define LITTLE_ENDIAN16_PUT_MSB(myByte)					\
				((myByte & 0xFF))
#endif



#if defined(__linux__) && defined (__KERNEL__)

/* give a two dimensional array where the second index is PACKAGE_LEN_WORDS */
#define ENCODER_SEND_PACKAGE(packageArray, index)		\
{ int r; r=(pdxc->hw_funcs.DownloadBuffer)(pdxc, (UINT8 *)packageArray, 64); \
/* { int i; printk("\nXYXr=%d\n",r); for (i=0; i < 32; i++) {\
   printk("%04x ", packageArray[i]);}} */\
}

#endif 


#endif				/* MOTION_DETECTION_H */

/**************************** end of motion_detection.h ****************************/
