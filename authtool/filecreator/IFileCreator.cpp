// IFileCreator.cpp: implementation of the IFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "IFileCreator.h"
#include "MP2TSFileCreator.h"
#include "MP4FileCreator.h"
#include "V2DFileCreator.h"
#include "AxAuthor_Defines.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IFileCreator::IFileCreator()
{
	
}

IFileCreator::~IFileCreator()
{
	
}


IFileCreator* IFileCreator::CreateObj(int mediaType,
									  LPOLESTR srcBlobDir, 
									  LPOLESTR outFile, 
									  const int startMin, 
									  const int endMin, 
									  const bool allMins)
{
	if(mediaType == MEDIATYPE_V2D)
		return (new CV2dFileCreator(srcBlobDir,outFile,startMin,endMin,allMins));
    
	if(mediaType == MEDIATYPE_MPEGTS)
		return (new CMP2TSFileCreator(srcBlobDir,outFile,startMin,endMin,allMins));

	if(mediaType == MEDIATYPE_MPEG4)
		return (new CMP4FileCreator(srcBlobDir,outFile,startMin,endMin,allMins));
	
	return NULL;
}

char *getMultiByteStr(LPOLESTR wStr)
{
	DWORD lastErr = 0;

	char *mbStr = NULL;		

	int szReqd = 0;
	szReqd = wcstombs(NULL, wStr, szReqd);

	mbStr = new char[szReqd+1];

	size_t count = wcstombs(mbStr, wStr, szReqd+1);

	if(count == -1)
	{
		// parsed invalid Unicode char
		lastErr = GetLastError();

		delete [] mbStr;
		return NULL;
	}	

	return mbStr;
}
