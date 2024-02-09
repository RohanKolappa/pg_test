/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        CfgFileParser.h
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


#ifndef _CFG_FILE_PARSER_H_
#define _CFG_FILE_PARSER_H_

#include "typedef.h"

#ifndef __GOChip_FWD_DEFINED__
#include "Struct.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef DRIVER

	#define MAX_FILE_LEN 65536

	typedef struct _FILE
	{
		SINT8 Buf[MAX_FILE_LEN];
		SINT32 FileLen;
		SINT32 FilePtr;
	} FILE;

	#define SEEK_SET    0

	int fseek(FILE *fp, int pos, int mode);
	int feof(FILE *fp);
	FILE *fopen(char *FileName, char *Mode);
	int fclose(FILE *fp);
	char *fgets(char *buf, int buf_len, FILE *fp);
	int fputs(const char *str, FILE *fp);

#else 

	#include "stdio.h"

#endif

SINT32 GetNextConfigInt(FILE *fp, SINT8 *strItem, SINT32 *Val);

SINT32 GetSensorInitDataFromFile(SINT8 *filename, UINT16 *dev_addr, SINT32 *reg_num, UINT16 *reg_data);

#ifdef __cplusplus
}
#endif


#endif 

/****************************** end of CfgFileParser.h ***********************/

