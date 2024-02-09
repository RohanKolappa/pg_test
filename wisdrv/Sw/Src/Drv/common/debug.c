/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        debug.c
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

#include "platform.h"
#include "debug.h"

//unsigned long DebugFlags = DEBUG_ALL_MESSAGES & (~DEBUG_FUNC_TRACE);
//unsigned long DebugFlags = DEBUG_ALL_MESSAGES;
//unsigned long DebugFlags = DEBUG_NO_MESSAGES;
//unsigned long DebugFlags = DEBUG_AVSYNC| DEBUG_VIDEO_STREAMBUFFER| DEBUG_AUDIO_STREAMBUFFER;
//unsigned long DebugFlags = DEBUG_ALWAYS_DISPLAY;
unsigned long DebugFlags = DEBUG_ALWAYS_DISPLAY | DEBUG_CONFIGURATION;
unsigned long SavedDebugFlags;

void SuspendDebugOutput()
{
    if(DebugFlags != DEBUG_NO_MESSAGES)
    { 
	SavedDebugFlags = DebugFlags;
	DebugFlags = DEBUG_NO_MESSAGES;
    }
}

void ResumeDebugOutput()
{
	DebugFlags = SavedDebugFlags;
}

void DumpBuffer(PVOID pvBuffer, UINT32 length)
{
#define BYTES_PER_LINE 0x10

    char    str[64], str2[64];
	int     i,j;
	unsigned char* ptr = (unsigned char*)pvBuffer;

	for (i = 0; i < (int)((length + BYTES_PER_LINE - 1) / BYTES_PER_LINE); i++)
	{
		sprintf(str,"%04X ",(i*BYTES_PER_LINE));
		for (j = 0; j < (int)BYTES_PER_LINE; j++)
		{
			if (((i * BYTES_PER_LINE) + j) < (int)length)
			{
				sprintf(str2,"%02X ",*ptr++);
				strcat(str,str2);
			}
		}
        PrintMsg(DEBUG_ALWAYS_DISPLAY, ("%s", str));
	}
}

/******************************** end of debug.c *****************************/
