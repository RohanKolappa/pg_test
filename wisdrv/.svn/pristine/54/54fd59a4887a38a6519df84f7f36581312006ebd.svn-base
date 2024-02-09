/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*   FILE: 
*		motion_detection_api.h
*
*   DESCRIPTION:
*		Motion Detection header file (with API)
*
*	NOTES:
*		This is not the currently used motion detection header file (yet).
*	This is just a sample of a motion detection interface that has not yet been
*   integrated into our driver.
*
*   $Id: motion_detection_api.h,v 1.2 2007-01-10 22:47:29 sjain Exp $
*
******************************************************************************/

#ifndef MOTION_DETECTION_API_H
#define MOTION_DETECTION_API_H

#ifdef __KERNEL__
#include "platform.h"
#include "function.h"
#include "I2C.h"
#include "usb_base.h"
#endif
		
#define	MOTION_DETECTION_INTERRUPT	0x4d44
#define	ENCODER_RETURN_VALUE_MASK	0xfffe

#define	MAX_REGIONS_OF_INTEREST		4
#define	NUM_MOTION_TYPES		2

/* array offsets for setting thresholds in the second entry of the */
/* thresholds array. */
#define	MOTION_VECTOR_THRESHOLD		0
#define	SAD_THRESHOLD			1

#define MD_MAX_MV_THRESHOLD		0x7fff
#define MD_MAX_SAD_THRESHOLD		0x7fff
#define	MD_MAX_SENSITIVITY_VALUE	0x7fff

/* the maximum size is D1 (720x480) */
#define	MAX_HORIZONTAL_PIXEL_NUM	720
//#define	MAX_VERTICAL_PIXEL_NUM		480
#define	MAX_VERTICAL_PIXEL_NUM		576
#define	NUM_PIXELS_IN_MACROBLOCK	16

/* maximum X macroblock number */
#define	MAX_X_COORDINATE     (MAX_HORIZONTAL_PIXEL_NUM/NUM_PIXELS_IN_MACROBLOCK)

/* maximum Y macroblock number */

#define	MAX_Y_COORDINATE     (MAX_VERTICAL_PIXEL_NUM/NUM_PIXELS_IN_MACROBLOCK)

#define	MAX_NUM_MACROBLOCKS  (MAX_X_COORDINATE * MAX_Y_COORDINATE)


#ifdef __KERNEL__
/*
*****************************************************************************
*
*   PROCEDURE:  
*		int ED_MDInit(void);
*
*   DESCRIPTION:
*
*		Initialize motion detection in the chip.  This is to
*	be called before the initial package is generated as it only
*	affects the initial package (ala pacgen).
*  
*   ARGUMENTS:
*
*		NONE
*
*   RETURNS:
*
*  ENCODER_SUCCESS	- motion detection was successfully initialized.
*  ENCODER_FAILURE	- motion detection could not be initialized.
*
*   NOTES:
*	Call this before Pacgen.  If you don't call this, you can't use motion
*	detection.
*
*****************************************************************************
*/

int ED_MDInit(void);

/* for example:
UINT16 arru16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES];
UINT8  arru8MotionSensitivity[MAX_REGIONS_OF_INTEREST];
*/

/******************************************************************************
*
*   PROCEDURE:  
*	int ED_SetMDThresholdsAndSensitivity(UINT16 *arru16MotionThresholds,
*					  UINT16 *arru8MotionSensitivity)
*
*   DESCRIPTION:
*	Set the motion vector and SAD (Sum of Absolute Differences) thresholds
*	for each region.  Also set the sensitivity thresholds for each region.
*  
*   ARGUMENTS:
*
*	arru16MotionThresholds - 2 dimensional array of thresholds (example
*				 defined above) of thresholds for the device.
*	arru8MotionSensitivity - 1 dimensional array of sensitivities.
*
*   RETURNS:
*
*   ENCODER_SUCCESS    - the macroblock thresholds have been set in the device.
*   ENCODER_FAILURE	- the macroblock thresholds had invalid thresholds.
*
*   NOTES:
*		Set the thresholds FIRST and then the map.
*
******************************************************************************/

int ED_SetMDThresholdsAndSensitivity(UINT16 arru16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES],
				  UINT16 *arru8MotionSensitivity, PDEVICE_EXTENSION_COMMON pdxc);


/* for each coordinate, fill with the region number, from 1 to 4. */
/* for example:
UINT8 arru8MotionCoordinates[MAX_X_COORDINATE][MAX_Y_COORDINATE];
*/

/********************************************************************************
*
*   PROCEDURE:  
*		int ED_SetMDRegions(UINT8 *arru8MotionCoordinates)
*
*   DESCRIPTION:
*		Set the region numbers for every macroblock to watch for motion.
*  
*   ARGUMENTS:
*
*		arru8MotionCoordinates - two dimensional array of region numbers where
*								 each [x][y] location contains the region number
*								 for that macroblock.  This argument MUST be
*								 exactly the the same number of macroblocks as
*								 the current picture.  If there is a size mismatch
*								 the results are undetermined.
*		u32MaxXCoordinate - The maximum X coordinate for the picture size.  This is
*							really just used for error checking.
*		u32MaxYCoordinate - The maximum Y coordinate for the picture size.  This is
*							really just used for error checking.
*
*   RETURNS:
*
*	 	ENCODER_SUCCESS		- the macroblock map has been set in the device.
*		ENCODER_FAILURE		- the macroblock map had invalid regions or there was
*							  a mismatch in the picture size.
*
*   NOTES:
*		Be careful that this array is the same size as the picture (in macroblocks)
*	 and that the caller has initialized every location in this array to ensure
*	 that the map is correctly set. 
*
********************************************************************************/

int ED_SetMDRegions(UINT8 *arru8MotionCoordinates,
						 UINT32 u32MaxXCoordinate,
						 UINT32 u32MaxYCoordinate,
						 PDEVICE_EXTENSION_COMMON pdxc);


/********************************************************************************
*
*   PROCEDURE:  
*		int ED_SetMDDelay(UINT32 u32DelayInMilliseconds)
*
*   DESCRIPTION:
*		Set a delay for how long to wait before reporting that additional motion
*	has occurred.
*  
*   ARGUMENTS:
*
*		u32DelayInMilliseconds - Delay, in milliseconds, before additional motion
*								 is reported.
*
*   RETURNS:
*
*	 	ENCODER_SUCCESS		- the delay has been set..
*		ENCODER_FAILURE		- TBD
*
*   NOTES:
*		The encoder driver knows how many frames per second, so it will use this
*	value to calculate the number of frames to wait before invoking the user
*	callback every time motion occurs.
*
********************************************************************************/

int ED_SetMDDelay(UINT32 u32DelayInMilliseconds);

								   
/* callback definition for motion events */
/*		pu32MacroblockMotionArrray will contain a bitmap for macroblocks that had motion in this */
/*									frame - TBD TBD TBD */
typedef void (*ENCODER_MOTION_CALLBACK_t)(void* userdata, UINT32 u32RegionMap,
										  UINT32 *pu32MacroblockMotionArray);

/********************************************************************************
*
*   PROCEDURE:  
*   int EncoderSetMDCallback(ENCODER_MOTION_CALLBACK_t *motionCallback,
*			  void *vptrUserCallbackArgument)
*
*   DESCRIPTION:
*   Register a user callback for when motion has occurred and is to be reported
*   to the application.
*  
*   ARGUMENTS:
*
*  motionCallback - function pointer of type ENCODER_MOTION_CALLBACK_t to
*		 be called when motion is to be reported.
*  vptrUserCallbackArgument - pointer to anything the user would like to
*		have passed into the callback function as
*		an argument.
*
*   RETURNS:
*
*  ENCODER_SUCCESS		- callback has been registered.
*  ENCODER_FAILURE		- the user has exceed the maximum number of callbacks
*
*   NOTES:
*
******************************************************************************/

int EncoderSetMDCallback(ENCODER_MOTION_CALLBACK_t *motionCallback,
			 void *vptrUserCallbackArgument);


#endif /* !__KERNEL__ */
#endif /* MOTION_DETECTION_API_H */

/****************** end of motion_detection_api.h **************************/

