// IFileCreator.cpp: implementation of the IFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "IPVSTranscode_compatibility.h"
#include "IFileCreator.h"
//#include "MP2TSFileCreator.h"
//#include "MP4FileCreator.h"
#include "V2dFileCreator.h"
//#include "AxAuthor_Defines.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IFileCreator::IFileCreator()
{
	
}

IFileCreator::~IFileCreator()
{
	
}


IFileCreator* IFileCreator::CreateObj(std::string& a_rStrBlobDir,
                                      std::string& a_rStrOutFile,
                                      StTranscodeOpts & a_rStArgList)
{
	//if(mediaType == MEDIATYPE_V2D)
		return (new CV2dFileCreator(a_rStrBlobDir, a_rStrOutFile, a_rStArgList));
    
	/*if(mediaType == MEDIATYPE_MPEGTS)
		return (new CMP2TSFileCreator(srcBlobDir,outFile,startMin,endMin,allMins));

	if(mediaType == MEDIATYPE_MPEG4)
		return (new CMP4FileCreator(srcBlobDir,outFile,startMin,endMin,allMins));*/
	
	return NULL;
}

#if (defined(__WIN32__) || defined(_WIN32))
char *getMultiByteStr(LPOLESTR wStr)
{
	DWORD lastErr = 0;

	char *mbStr = NULL;		

	int szReqd = 0;
	szReqd = wcstombs(NULL, wStr, szReqd);

	mbStr = new char[szReqd+1];

	int count = wcstombs(mbStr, wStr, szReqd+1);

	if(count == -1)
	{
		// parsed invalid Unicode char
		lastErr = GetLastError();

		delete [] mbStr;
		return NULL;
	}	

	return mbStr;
}
#endif

