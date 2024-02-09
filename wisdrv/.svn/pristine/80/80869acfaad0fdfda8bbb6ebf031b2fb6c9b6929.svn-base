/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*   FILE: 
*		motion_detection.c
*
*   DESCRIPTION:
*		Motion Detection driver
*
*	NOTES:
*		This is the motion detection driver interface.
*
*   $Id: motion_detection.c,v 1.2 2007-01-10 22:47:29 sjain Exp $
*
******************************************************************************/

#include "wis_types.h"
#include "motion_detection_api.h"
#include "motion_detection.h"
#include "PacGen_7007SB.h"

#if 0
extern TCFGVIDEOEX  encoderVariableSettings;                                         
extern TVIDEOCFGFIX encoderFixedSettings;
#endif

/********************************************************************************
*
*   PROCEDURE:  
*		status_t ED_MDInit(void);
*
*   DESCRIPTION:
*		Initialize motion detection in the chip.  This is to be called before
*	 the initial package is generated as it only affects the initial package
*	 (ala pacgen).
*  
*   ARGUMENTS:
*
*		NONE
*
*   RETURNS:
*
*	 	ENCODER_SUCCESS		- motion detection was successfully initialized.
*		ENCODER_FAILURE		- motion detection could not be initialized.
*
*   NOTES:
*		Call this before Pacgen.  If you don't call this, you can't use motion
*	detection.
*
********************************************************************************/

status_t ED_MDInit(void)
{

    status_t returnStatus=ENCODER_SUCCESS;

#if 0  /* would normally be used when setting the initial state */
        /* since the USB Linux driver does not support the same */
        /* structures as the embedded linux drivers, this code  */
        /* can not be used as such */

    /* enable the motion detection feature */
    encoderVariableSettings.misccfg.reserved |= 1;

    /* initialize motion detection to harmless values */
    for ( s32Index=0; s32Index < MAX_REGIONS_OF_INTEREST; s32Index++ )
    {
        encoderFixedSettings.mdcfg.u8SensitivityValues[s32Index]    = MD_MAX_SENSITIVITY_VALUE;
        encoderFixedSettings.mdcfg.u16SADThresholdValues[s32Index]  = MD_MAX_SAD_THRESHOLD;
        encoderFixedSettings.mdcfg.u16MVThresholdValues[s32Index]   = MD_MAX_MV_THRESHOLD;
    }

    /* initialize motion detection to harmless values */
    for ( s32Index=0; s32Index < MAX_NUM_MACROBLOCKS; s32Index++ )
    {
        encoderFixedSettings.mdcfg.u16MacroblockMap[s32Index]       = 0;  /* set to region 0 */
    }

#endif

    return(returnStatus);

}

/*************************** end of ED_MDInit ******************************/

/********************************************************************************
*
*   PROCEDURE:  
*		status_t ED_SetMDThresholdsAndSensitivity(uint16 arrs16MotionThresholds[][NUM_MOTION_TYPES],
*							  uint16 *arru8MotionSensitivity)
*
*   DESCRIPTION:
*		Set the motion vector and SAD (Sum of Absolute Differences) thresholds
*	for each region.  Also set the sensitivity thresholds for each region.
*  
*   ARGUMENTS:
*
*		arrs16MotionThresholds - 2 dimensional array of thresholds (example
*								 defined in motion_detection_api.h) of thresholds
*								 for the device.
*		arru8MotionSensitivity - 1 dimensional array of sensitivities.
*
*   RETURNS:
*
*	 	ENCODER_SUCCESS		- the macroblock thresholds have been set in the device.
*		ENCODER_FAILURE		- the macroblock thresholds had invalid thresholds.
*
*   NOTES:
*		Set the thresholds FIRST and then the map.
*
********************************************************************************/

status_t ED_SetMDThresholdsAndSensitivity(UINT16 arrs16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES],
                                          UINT16 *arru8MotionSensitivity,
                                          PDEVICE_EXTENSION_COMMON pdxc)
{
    uint32 u32ThresholdIndex;
    motion_threshold_package_t sMotionThresholdPackage;
    status_t returnStatus;
    uint16 *pu16Package;
    uint16 IntVal, IntData;

    /* initialize to successful */
    returnStatus = ENCODER_SUCCESS;

    memset(&sMotionThresholdPackage,0,sizeof(motion_threshold_package_t));

    /* first set the marker in this package */
    LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionThresholdPackage.u16MotionThresholdMarker,ENCODER_THRESHOLD_MARKER);

    /* fill in the thresholds to the appropriate location */
    for ( u32ThresholdIndex=0; u32ThresholdIndex < MAX_REGIONS_OF_INTEREST; u32ThresholdIndex++ )
    {

        /* if the thresholds are negative, this is illegal, so return a failure */
        if ( (arrs16MotionThresholds[u32ThresholdIndex][MOTION_VECTOR_THRESHOLD] < 0)
             || (arrs16MotionThresholds[u32ThresholdIndex][SAD_THRESHOLD] < 0) )
        {
            returnStatus = ENCODER_FAILURE;
        }

        /* copy the motion vector threshold */
        LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionThresholdPackage.u16MVThresholdValues[u32ThresholdIndex],
                                      arrs16MotionThresholds[u32ThresholdIndex][MOTION_VECTOR_THRESHOLD]);

        /* copy the SAD threshold */
        LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionThresholdPackage.u16SADThresholdValues[u32ThresholdIndex],
                                      arrs16MotionThresholds[u32ThresholdIndex][SAD_THRESHOLD]);

        /* copy the sensitivity values */
        LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionThresholdPackage.u8SensitivityValues[u32ThresholdIndex],
                                      arru8MotionSensitivity[u32ThresholdIndex]);
    }

    pu16Package = (uint16 *) &sMotionThresholdPackage.u16MotionThresholdMarker;

    /* download the package to our device */
    ENCODER_SEND_PACKAGE(pu16Package, s32Index);

    /* wait for the package completion value */
    pdxc->CheckInterruptType = ENCODER_THRESHOLD_RETURN;
    pdxc->CheckInterruptData = -1;

#if 0 /* we should get the interrupt return back first, but we don't because */
    /* the interrupt structure is broken */
    returnStatus = (pdxc->hw_funcs.ReadInterrupt)(pdxc, &IntVal, &IntData);

    if ( (IntVal & ENCODER_RETURN_VALUE_MASK) != ENCODER_THRESHOLD_RETURN )
    {
        printk("Error: SetMDTS: %x\n", IntVal);
        returnStatus = -1;
    }
#endif

    return(returnStatus);

}

/**************** end of ED_SetMDThresholdsAndSensitivity **********************/

/********************************************************************************
*
*   PROCEDURE:  
*		status_t ED_SetMDRegions(uint8 *arru8MotionCoordinates,
*								 uint32 u32MaxXCoordinate,
*								 uint32 u32MaxYCoordinate)
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

int ED_SetMDRegions(uint8 *arru8MotionCoordinates,
                    uint32 u32MaxXCoordinate,
                    uint32 u32MaxYCoordinate, PDEVICE_EXTENSION_COMMON pdxc)
{
    status_t returnStatus=ENCODER_SUCCESS;
    sint32 s32MDRegionIndex;
    sint32 s32PackageIndex;
    sint32 s32Package;
    uint16 *pu16Package[PACKAGE_LEN_WORDS];
    motion_macroblock_map_package_t
    sMotionMacroblockMapPackage[MAX_MACROBLOCK_MAP_PACKAGES];
    uint16 IntVal, IntData;
    bool_t numExtraPackages;

    /* assume that there will be no extra
     * package at the end unless told otherwise */
    numExtraPackages = 0;

#if 0
    int i;
    printk("DAN: Coordinates\n");
    for ( i=0; i<u32MaxXCoordinate*u32MaxYCoordinate; i++ )
    {
        printk("%d, ", arru8MotionCoordinates[i]);
        if ( (i!=0) && !(i%32) )
            printk("\n");
    }
#endif
    /* for each package, initialize a macroblock map package */

    /* NOTE: because the number of macroblocks is not evenly divisible by the
    * 	     number of macroblocks in a package, we add an extra package at
    *       the end.  The end of this package may contain 0s which would be
    *       written to appended memory.  This appended memory happens to be
    *       the macroblock output map, so this has the effect of clearing 
    *       elements in that map.  In summary, if you use an image size that
    *       is not divisible by 8, the motion output map for that frame, if 
    *       motion has occurred, would be 0.  That said, whenever you update
    *       the map, you should not be looking for motion...therefore, always
    *       add an extra package at the end regardless of the size           */
    for ( s32PackageIndex=0;
          s32PackageIndex < ((u32MaxXCoordinate * u32MaxYCoordinate)/NUM_MACROBLOCKS_PER_PACKAGE)+1;
          s32PackageIndex++ )
    {
        /* set the marker and the stating point */
        LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionMacroblockMapPackage[s32PackageIndex].u16MotionPackageMarker,
                                      ENCODER_MACROBLOCK_MAP_MARKER);
        LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionMacroblockMapPackage[s32PackageIndex].u16StartingMacroblockNumber,
                                      s32PackageIndex*(NUM_MACROBLOCKS_PER_PACKAGE/NUM_MACROBLOCKS_PER_WORD16));

        /* now initialize the contents of the map */
        for ( s32MDRegionIndex=0;
            s32MDRegionIndex < (NUM_MACROBLOCKS_PER_PACKAGE/NUM_MACROBLOCKS_PER_WORD16);
            s32MDRegionIndex++ )
        {

            /* if we have not reached the end of the valid macroblocks for */
            /* this image size, write the map data */
            if ((s32PackageIndex*NUM_MACROBLOCKS_PER_PACKAGE
                + s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16) <
                                       (u32MaxXCoordinate * u32MaxYCoordinate))
            {
                /* write the regions to the package for sending later */
                sMotionMacroblockMapPackage[s32PackageIndex].u16MacroblockMap[s32MDRegionIndex] = 
                (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 7] << 14)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 6] << 12)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 5] << 10)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 4] << 8)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 3] << 6)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 2] << 4)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 1] << 2)
                | (arru8MotionCoordinates[s32MDRegionIndex*NUM_MACROBLOCKS_PER_WORD16 + s32PackageIndex*NUM_WORDS_PER_PACKAGE*NUM_MACROBLOCKS_PER_WORD16 + 0]);
            }
            else /* we have reached the maximum size, so just write 0's for the rest */
            {
                sMotionMacroblockMapPackage[s32PackageIndex].u16MacroblockMap[s32MDRegionIndex] = 0;
                numExtraPackages = 1; /* there is an extra package at the end */
            }

            LITTLE_ENDIAN_TO_CPU_ENDIAN16(sMotionMacroblockMapPackage[s32PackageIndex].u16MacroblockMap[s32MDRegionIndex],
                                          sMotionMacroblockMapPackage[s32PackageIndex].u16MacroblockMap[s32MDRegionIndex]);

        }
    }

    /* set the pointer to the start of the packages for sending to the encoder */
    for ( s32PackageIndex=0;
          s32PackageIndex < ((u32MaxXCoordinate * u32MaxYCoordinate)/NUM_MACROBLOCKS_PER_PACKAGE) + numExtraPackages;
          s32PackageIndex++ )
    {
        pu16Package[s32PackageIndex] = (uint16 *) &sMotionMacroblockMapPackage[s32PackageIndex].u16MotionPackageMarker;

        /* send the package */
        ENCODER_SEND_PACKAGE(pu16Package[s32PackageIndex], s32Index);

#if 0 /* we should get the interrupt return back first, but we don't because */
        /* the interrupt structure is broken */

        /* wait for the package completion value */
        pdxc->CheckInterruptType = ENCODER_MACROBLOCK_MAP_RETURN;
        pdxc->CheckInterruptData = -1;
        (pdxc->hw_funcs.ReadInterrupt)(pdxc, &IntVal, &IntData);

        if ( (IntVal & ENCODER_RETURN_VALUE_MASK) != ENCODER_MACROBLOCK_MAP_RETURN )
        {
            returnStatus = ENCODER_FAILURE;
        }
#endif
    }

    return(returnStatus);

}

/*************************** end of ED_SetMDRegions **************************/

/********************************************************************************
*
*   PROCEDURE:  
*		status_t ED_SetMDDelay(uint32 u32DelayInMilliseconds)
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

status_t ED_SetMDDelay(uint32 u32DelayInMilliseconds)
{

    /* TBD */

    return(ENCODER_FAILURE);

}

/*************************** end of ED_SetMDDelay ******************************/

/********************************************************************************
*
*   PROCEDURE:  
*		status_t EncoderSetMDCallback(ENCODER_MOTION_CALLBACK_t *motionCallback,
*									  void *vptrUserCallbackArgument)
*
*   DESCRIPTION:
*		Register a user callback for when motion has occurred and is to be reported
*	to the application.
*  
*   ARGUMENTS:
*
*		motionCallback - function pointer of type ENCODER_MOTION_CALLBACK_t to
*						 be called when motion is to be reported.
*		vptrUserCallbackArgument - pointer to anything the user would like to
*									have passed into the callback function as
*									an argument.
*
*   RETURNS:
*
*	 	ENCODER_SUCCESS		- callback has been registered.
*		ENCODER_FAILURE		- the user has exceed the maximum number of callbacks
*
*   NOTES:
*
********************************************************************************/

status_t EncoderSetMDCallback(ENCODER_MOTION_CALLBACK_t *motionCallback,
                              void *vptrUserCallbackArgument)
{
    /* TBD */
    return(ENCODER_FAILURE);
}

/*********************** end of EncoderSetMDCallback **************************/

/************************* end of motion_detection.c **************************/

