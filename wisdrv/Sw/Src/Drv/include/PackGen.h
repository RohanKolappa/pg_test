/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        PackGen.h
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

#ifndef _PACK_GEN_H_
#define _PACK_GEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Struct.h"
#include "common.h"

int GenInitialPacket(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pPack, SINT32 *PackLen);

#ifdef __cplusplus
}
#endif

#endif

/********************************* end of PackGen.h **************************/
