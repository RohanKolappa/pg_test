/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        gpio.h
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

#ifndef _GPIO_H_
#define _GPIO_H_

#include "typedef.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GPIO_MODE
#define _GPIO_MODE
#define GM_ONCHIP	0
#define GM_CYPRESS	1
#endif

// Mode == 0 : OnChip GPIO
//         1 : Cypress GPIO
SINT32	GPIO_WritePins(PDEVICE_EXTENSION_COMMON pdxc, SINT32 WriteNum, SINT32 *Index, SINT32 *Value, SINT32 Mode);
SINT32	GPIO_ReadPins(PDEVICE_EXTENSION_COMMON pdxc, SINT32 ReadNum, SINT32 *Index, SINT32 *Value, SINT32 Mode);

#ifdef __cplusplus
}
#endif

#endif

/********************************** end of gpio.h ****************************/
