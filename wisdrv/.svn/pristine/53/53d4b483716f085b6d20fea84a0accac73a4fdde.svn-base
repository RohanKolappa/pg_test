/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        platform.c
*
*    DESCRIPTION:
*        provide platform-independented functions
*
*
*    AUTHOR:
*	     
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#define __KERNEL_SYSCALLS__
#include <linux/slab.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/syscall.h>
#include <linux/syscall.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/vmalloc.h>

#include "platform.h"									 
#include "debug.h"

int errno;


UINT32 GetDWORDValueFromRegistry(char *Name, UINT32 DefaultValue)
{
    return 0;
}

void GetConfigFilePath(SINT8 *Path)
{
	strcpy(Path, "/etc/wis/");	
}

int ReadFileToBuffer(char *FileName, UINT8 *Buf, int *ReadLen)
{
	mm_segment_t fs = get_fs();
	int fd, len;
//	struct kstat stat;
//	int r;

	set_fs(get_ds());

	fd = open(FileName, O_RDONLY, 0);

	if (fd == -1)
	{
		PrintMsg(1, ("can not open file '%s'", FileName));
		set_fs(fs);
		return 1;
	}

//	vfs_fstat(fd, &stat);
//	len = stat.size;
//	PrintMsg(1, ("<DRV> ReadFileToBuffer Len %d", len));

//	if(len > *ReadLen)
//	{
//		PrintMsg(1, ("<DRV> ReadFileToBuffer : Buffer too small"));
//		set_fs(fs);
//		return 2;
//	}

	len = read(fd, Buf, *ReadLen);

    if (len<=0)
	{
		PrintMsg(1, ("<DRV> ReadFileToBuffer : Read Error"));
		close(fd);
		set_fs(fs);
		return 3;
	}

	PrintMsg(1, ("<DRV> ReadFileToBuffer Len %d", len));

	close(fd);
	set_fs(fs);
	*ReadLen = len;

	return 0;
}

int WriteBufferToFile(char *FileName, UINT8 *Buf, int WriteLen)
{
	return 0;
}

// return 0 : event set
//        1 : time out
int PIWaitEvent(wait_queue_head_t *pEvent, SINT32 Timeout)
{
	int r = sleep_on_timeout(pEvent, (Timeout*HZ));

	if(r>0)
		return 0;
	else
		return 1;	
}

void PIReleaseNotificationEvent(wait_queue_head_t *pKEvent)
{
	return;
}

void PIInitializeEvent(wait_queue_head_t *pKEvent)
{
	init_waitqueue_head(pKEvent);
}

void PISetEvent(wait_queue_head_t *pKEvent)
{
	wake_up(pKEvent);
}

void PIResetEvent(wait_queue_head_t *pKEvent)
{
	return;
}

void PISleep(SINT32 ms)
{
	mdelay(ms);
}

void *PIAllocMem(SINT32 Size)
{
	return kmalloc(Size, GFP_KERNEL);
}

void PIFreeMem(void *p)
{
	if(p)
	{
		kfree(p);
		p = 0;
	}
}

void *PIAllocVirtualMem(SINT32 Size)
{
	return vmalloc(Size);
}

void PIFreeVirtualMem(void *p)
{
	{
		vfree(p);
		p = 0;
	}
}

void PICopyMem(void *pDest, void *pSrc, UINT32 Length)
{
	memcpy(pDest, pSrc, Length);	
}

int  PICmpMem(void *pDest, void *pSrc, UINT32 Length)
{
	return 0;
}

void PIFillMem(void *p, UINT32 Length, UINT8 Fill)
{
	memset(p, Fill, Length);
}

/****************************** end of platform.c ****************************/
