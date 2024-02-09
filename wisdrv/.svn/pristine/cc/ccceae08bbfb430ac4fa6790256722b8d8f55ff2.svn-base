/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        function.h
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
//////////////////////////////////////////////////////////////////////
//
// Created by Jun Ding 9/19/2001

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// functional operation
int CheckInterruptPipe(PDEVICE_EXTENSION_COMMON pdxc, int InterruptType, int InterruptData);

// we use these two functions because in HPI mode, the interrupt will come back before 
// WriteInterrupt() return
int PrepCheckInterruptPipe(PDEVICE_EXTENSION_COMMON pdxc, int InterruptType, int InterruptData);
int CheckInterruptPipe2(PDEVICE_EXTENSION_COMMON pdxc, int InterruptType, int InterruptData);

int BootupChip(PDEVICE_EXTENSION_COMMON pdxc);
int AllocateBuffers(PDEVICE_EXTENSION_COMMON pdxc);
int InitializeDevice(PDEVICE_EXTENSION_COMMON pdxc);
int UninitializeDevice(PDEVICE_EXTENSION_COMMON pdxc);
void ReleaseBuffers(PDEVICE_EXTENSION_COMMON pdxc);
void ClearOnClose(PDEVICE_EXTENSION_COMMON pdxc);

int WriteCBusRegFW(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 Data);
int ReadCBusRegFW(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 *pData);

int WriteHPIRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 Data);
int ReadHPIRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 *pData);

SINT32 OnCreate(PDEVICE_EXTENSION_COMMON pdxc);
SINT32 OnClose(PDEVICE_EXTENSION_COMMON pdxc);
SINT32 OnSurpriseRemoval(PDEVICE_EXTENSION_COMMON pdxc);
SINT32 OnRemoveDevice(PDEVICE_EXTENSION_COMMON pdxc);
SINT32 OnAddDevice(PDEVICE_EXTENSION_COMMON pdxc);

void InitBoardInterface(PDEVICE_EXTENSION_COMMON pdxc);

void FormatSysFileName(SINT8 *FileName);

void ForceDropFrame(PDEVICE_EXTENSION_COMMON pdxc);
void ForceDuplicateFrame(PDEVICE_EXTENSION_COMMON pdxc);

void ReadVideoStreamCallBack(PDEVICE_EXTENSION_COMMON pdxc);
void ReadAudioStreamCallBack(PDEVICE_EXTENSION_COMMON pdxc);
void ReadInterruptCallBack(PDEVICE_EXTENSION_COMMON pdxc);

void SaveDefaultChipConfig(PDEVICE_EXTENSION_COMMON pdxc);

// StartBitIndex should >= StopBitIndex
UINT32 GetBits(UINT32 Source, int StartBitIndex, int StopBitIndex);
void SetBits(UINT32 *pSource, int StartBitIndex, int StopBitIndex, UINT32 Value);
#if defined(__AVSYNC__)
void NotifyAVOffset(PDEVICE_EXTENSION_COMMON pdxc, SINT32 Offset);
void SetAVSyncPara(PDEVICE_EXTENSION_COMMON pdxc, int audio_delay_threshould, int av_sync_check_inteval);
#endif
/*
	typedef struct
	{
		...
		long reserved;	//this 32-bit setting has five fields as described below
	} TCFGBRCTRL

	bit[6:0]   = qmin (1 < QMIN <= QMAX)
	bit[7]     = brc_dup_enable (0: disabled, 1:enabled)
	bit[14:8]  = qmax (QMIN <= QMAX < 32)
	bit[15]    = mb_brc_enable (0: disabled, 1:enabled)
	bit[31:16] = brc_window_size (unit: minute)
				 (if 30fps and brc_window_size=1, window_size=1800 frames)

*/

#define SET_Q_MIN(Var, Val)			SetBits((UINT32 *)(&Var), 6, 0, Val)
#define SET_Q_MAX(Var, Val)			SetBits((UINT32 *)(&Var), 14, 8, Val)
#define SET_BRC_DUP_ENB(Var, Val)	SetBits((UINT32 *)(&Var), 7, 7, Val)
#define SET_MB_BRC_ENB(Var, Val)	SetBits((UINT32 *)(&Var), 15, 15, Val)
#define SET_BRC_WINDOW(Var, Val)	SetBits((UINT32 *)(&Var), 31, 16, Val)

#define GET_Q_MIN(Var)			GetBits((UINT32)(Var), 6, 0)
#define GET_Q_MAX(Var)			GetBits((UINT32)(Var), 14, 8)
#define GET_BRC_DUP_ENB(Var)	GetBits((UINT32)(Var), 7, 7)
#define GET_MB_BRC_ENB(Var)		GetBits((UINT32)(Var), 15, 15)
#define GET_BRC_WINDOW(Var)		GetBits((UINT32)(Var), 31, 16)

#ifdef _WATCHDOG_THREAD
void StartWatchDogThread(PDEVICE_EXTENSION_COMMON context);
void StopWatchDogThread(PDEVICE_EXTENSION_COMMON context);
#endif

SINT32 BoardCallBack_OnVSyncLost(void *context);

#ifdef __cplusplus
}
#endif

#endif

/****************************** end of function.h ****************************/
