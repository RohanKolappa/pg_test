/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        CfgFileParser.c
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

#include "typedef.h"
#include "CfgFileParser.h"			  

#if defined(_WINDOWS)
	#include "string.h"			  
#elif defined(_LINUX)
	#ifdef DRIVER
		#include <linux/string.h>
	#else
		#include "string.h"
	#endif
#endif

#if defined(DRIVER) 

	#include "platform.h"			  

	int fseek(FILE *fp, int pos, int mode)
	{
		if(mode==SEEK_SET)
			fp->FilePtr = pos;

		return fp->FilePtr;
	}

	int feof(FILE *fp)
	{
		return (fp->FilePtr>=fp->FileLen);
	}

	FILE *fopen(char *FileName, char *Mode)
	{
		FILE *fp = PIAllocMem(sizeof(FILE));
		int r;
		int len = MAX_FILE_LEN;
		
		r = ReadFileToBuffer(FileName, fp->Buf, &len);
		if(r > 0)
		{
			PIFreeMem(fp);
			return NULL;
		}

		fp->FileLen = len;
		fp->FilePtr = 0;

		return fp;

	}

	int fclose(FILE *fp)
	{
		PIFreeMem(fp);
		return 0;
	}

	char *fgets(char *buf, int buf_len, FILE *fp)
	{
		int i=0;

		while(1)
		{
			if(fp->FilePtr >= fp->FileLen)
				break;
			
			if(fp->Buf[fp->FilePtr]==0x0d || 
               fp->Buf[fp->FilePtr]==0x0a || 
               i>=buf_len)
				break;

			buf[i++] = fp->Buf[(fp->FilePtr)++];
		}

		if(i>=buf_len)
			buf[i-1] = 0;
		else
			buf[i] = 0;

		if(fp->FilePtr < fp->FileLen)
		{
			if(fp->Buf[fp->FilePtr] == 0x0d)
				(fp->FilePtr) ++;
		}

		if(fp->FilePtr < fp->FileLen)
		{
			if(fp->Buf[fp->FilePtr] == 0x0a)
				(fp->FilePtr) ++;
		}

		return buf;
	}

	int fputs(const char *str, FILE *fp)
	{
		return 0;
	}

#elif defined(_WINDOWS) /* defined (_DRIVER) */

	#include "windows.h"

	void GetConfigFilePath(SINT8 *Path)
	{
		char s[256];
		GetWindowsDirectory(s,256);
		strcpy(Path, s);
		strcat(Path, "\\");
	}

#endif /* defined (_DRIVER) */


#define VIDEO_SETTING_FILENAME "video_setting.txt"

void DeleteTabSpace(SINT8 *strS)
{
	SINT32 i, j;
	SINT8 match = 0;

	for (i = strlen(strS) - 1; i >= 0; i--)
	{
		if (match && strS[i] != '\"') 
			continue;

		if (strS[i] == '\"') 
			match = !match;

		if (strS[i] == ' ' || strS[i] == '\t' || strS[i] == '\r' || strS[i] == '\"' || strS[i] == '\n')
		{
			for (j = i + 1; j < (SINT32)strlen(strS); j++)
				strS[j-1] = strS[j];
			strS[j-1] = '\0';
		}
	}

	match = 0;
	for (i = 0; i< (SINT32)strlen(strS) - 2; i++)
	{
		if(strS[i]=='/' && strS[i+1]=='/')
		{
			match = 1;
			break;
		}
	}
	if(match==1)
		strS[i] = 0;
}

#define _isdigit(c) (c >= '0' && c <= '9')
#define _ishex(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

int _atoi(char *s) 
{
	int i = 0;
	while (_isdigit(*s))
		i = i*10 + *(s++) - '0';
	return i;
}

int _htoi(char *s) 
{
	int i = 0;
	int c;
	while (_ishex(*s))
	{
		i = i*16;
		c = *(s++);
		if(c >= '0' && c <= '9')
			i += c-'0';
		else if(c >= 'a' && c <= 'f')
			i += 10+c-'a';
		else
			i += 10+c-'A';
	}

	return i;
}



//	return :
//		0 -- found
//		>0 -- not found
SINT32 GetNextConfigString(FILE *fp, SINT8 *strItem, SINT8 *itemString)
{
	SINT8 strLine[256];
	int name_len = strlen(strItem);

	while(!feof(fp))
	{
		if (fgets(strLine, 128, fp) == NULL) 
			break;

		DeleteTabSpace(strLine);

		if (strLine[0] == 0) 
			continue;

		if ( memcmp(strLine, strItem, name_len) )
			continue;

		if ( strLine[name_len] != '=' )
			continue;

		strcpy(itemString, strLine+name_len+1);
		return 0;
	}

	return 1;
}

SINT32 GetNextConfigInt(FILE *fp, SINT8 *strItem, SINT32 *Val)
{
	int r;
	char itemString[256];

	r = GetNextConfigString(fp, strItem, itemString);
	if(r==0)
		*Val = _atoi(itemString);

	return r;
}

SINT32 GetNextConfigHex(FILE *fp, SINT8 *strItem, SINT32 *Val)
{
	int r;
	char itemString[256];

	r = GetNextConfigString(fp, strItem, itemString);
	if(r==0)
		*Val = _htoi(itemString);

	return r;
}

SINT32 GetADPair(FILE *fp, SINT32 *Num, UINT16 *ADPair)
{
	SINT8 strLine[256];
	int count = 0;
	int i;

	if (!fp) 
		return 1;

	fseek(fp, 0, SEEK_SET);

	while(!feof(fp))
	{
		if (fgets(strLine, 128, fp) == NULL) 
			break;

		DeleteTabSpace(strLine);

		if (strLine[0] == 0) 
			continue;

		if (strLine[0] != '{') 
			continue;

		if (count >= *Num)
			return 3;

		i=1;
		while(strLine[i]!=','&&strLine[i]!=0)
			i++;

		if(strLine[i]==0)
			continue;

		strLine[i]=0;

		ADPair[count*2] = (UINT16)_htoi(strLine+1);
		ADPair[count*2+1] = (UINT16)_htoi(strLine+i+1);
		count++;
	}

	if(count > 0) 
	{
		*Num = count;
		return 0;
	}
	else
		return 2;
}

SINT32 GetSensorInitDataFromFile(char *filename, UINT16 *dev_addr, SINT32 *reg_num, UINT16 *reg_data)
{
	SINT32 Val;

	FILE *fp = fopen(filename, "r");
	if(fp==NULL)
		return 1;

	if( GetNextConfigHex(fp, "device_address", &Val)==0 )
		*dev_addr = (UINT16)Val;
	else
	{
		fclose(fp);
		return 2;
	}

	if( GetADPair(fp, reg_num, reg_data) != 0)
	{
		fclose(fp);
		return 3;
	}
	else
	{
		fclose(fp);
		return 0;
	}
}

/****************************** end of CfgFileParser.c ***********************/

