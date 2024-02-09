/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        BoardEntry.c
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
// BoardEntry.c

#include "typedef.h"
#include "Board.h"

#include "debug.h"

extern BOARD_INTERFACE BI_XMen;
extern BOARD_INTERFACE BI_XMenII;
extern BOARD_INTERFACE BI_XMenIII;
extern BOARD_INTERFACE BI_MatrixII;
extern BOARD_INTERFACE BI_MatrixReload;
extern BOARD_INTERFACE BI_MatrixRevolution;
extern BOARD_INTERFACE BI_Twister;
extern BOARD_INTERFACE BI_TVTuner;
extern BOARD_INTERFACE BI_ISpy;
extern BOARD_INTERFACE BI_ISpyII;


BOARD_INTERFACE *g_BoardInterfaces[] = 
{
	&BI_XMen,
	&BI_XMenII,
	&BI_XMenIII,	
	&BI_MatrixII,
	&BI_MatrixReload,
	&BI_MatrixRevolution,
	&BI_Twister,
	&BI_TVTuner,
	&BI_ISpy,
	&BI_ISpyII,
}; 

UINT32 g_BoardNum = sizeof(g_BoardInterfaces)/sizeof(g_BoardInterfaces[0]);
ACCESS_FUNCTIONS* g_pAF = NULL;

void BOARD_API GetBoardModuleCaps(
				AVSTREAM_COOKIE cookie,
				ACCESS_FUNCTIONS *pAF, 
				BOARD_INTERFACE ***BoardInterfaces, 
				UINT32 *BoardNum)
{
	g_pAF = pAF;
	*BoardInterfaces = g_BoardInterfaces;
	*BoardNum = g_BoardNum;
}

