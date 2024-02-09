/*++

Copyright (c) 2004  WIS

Module Name:

    DeviceCtr.h

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 WIS 
    All Rights Reserved.

--*/

#ifndef __DEV_CTRL_H
#define __DEV_CTRL_H

#include "common.h"

int ProcessIOCTL(PDEVICE_EXTENSION_COMMON pdxc, VOID *ioBuffer, SINT32 *ret_len);
int ProcessReadRequest(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuffer, SINT32 *buf_len);

#endif
