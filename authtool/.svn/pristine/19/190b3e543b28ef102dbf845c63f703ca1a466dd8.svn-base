// MP2TSFileCreator.cpp: implementation of the CMP2TSFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "string.h"

#include <fcntl.h>
#include <io.h>

#include "MP2TSFileCreator.h"

#include "CommonIncludes.hh"
#include "MediaServer.hh"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMP2TSFileCreator::CMP2TSFileCreator(LPOLESTR blobDir, 
									 LPOLESTR tsOutFile, 
									 const int startMin, 
									 const int endMin, 
									 bool allMins)
{
	
	
	m_nFile = 0;
	m_totalWriteBytes = 0;
	
	m_blobDir = blobDir;
	m_tsOutFile = tsOutFile;
	
	m_nStartMin = 0;
	m_nEndMin = 0;
	
	if(!allMins)
	{
		m_nStartMin = startMin;
		m_nEndMin = endMin;
		printf(" allMins false start %d end %d\n",m_nStartMin,m_nEndMin); 
	}
	else
		printf(" allMins set to true\n");
	
	
	
}

CMP2TSFileCreator::~CMP2TSFileCreator()
{
	
}

int CMP2TSFileCreator::Initialize()
{

	m_nFile = _wopen(m_tsOutFile,_O_CREAT|_O_TRUNC|_O_WRONLY|_O_BINARY, 00666);
	if(m_nFile == -1)
	{
		printf(" Cannot open file %s retCode %d !!!!\n",m_tsOutFile,m_nFile);
		return 1;
	}

	if(m_nStartMin > m_nEndMin)
	{
		printf("Invalid minute parameters, m_nStartMin %d, m_nEndMin %d\n",m_nStartMin,m_nEndMin);
		return 1;
	}
	
	
	return 0;
	
}

int CMP2TSFileCreator::CreateFile()
{
	
	int frames = 0;
	int rc;
	int mp2 = 1;
	CCFrame cframe;		

	char *mbStrBlobDir = getMultiByteStr(m_blobDir);	

	CMediaServer *ms = new CMediaServer(mbStrBlobDir);
	if (ms == NULL) 
	{
        printf("Could not create MediaServer object!!\n");
        return 1;
	}
	
	if (ms->Init() != MSRC_OK) 
	{
        printf("MediaServer Init Failed: %s\n",
			ms->GetErrorString());
        return 1;
	}
	
	if (ms->Start(m_nStartMin) != MSRC_OK) 
	{
        printf("MediaServer Start Failed: %s\n",
			ms->GetErrorString());
        return 1;
	}

	uint64_t tfirstTimestamp = 0;
	uint64_t sampleTimeStamp = 0;
	uint64_t difftstamp = 0;
		
	
	while(1) 
	{
		if (ms->GetFrame(cframe) != MSRC_OK) 
		{
			printf("MediaServer Get Frame Failed: %s\n",
				ms->GetErrorString());
			break;
		}

		sampleTimeStamp = cframe.GetTimeStamp();

		
		if(tfirstTimestamp == 0)
				tfirstTimestamp = sampleTimeStamp;
				
		if((sampleTimeStamp - tfirstTimestamp) < (m_nStartMin * 60000))
			continue;
		
		if(m_nEndMin)
		{
			difftstamp = sampleTimeStamp - tfirstTimestamp;
			//if(difftstamp >= totalClipDuration)
			if(difftstamp >= (m_nEndMin * 60000))
				break;
			//printf("currentTimestamp %I64u tfirstTimestamp %I64u difftstamp %I64u\n",sampleTimeStamp,tfirstTimestamp,difftstamp);
		}
		
		
		rc = write(m_nFile,cframe.GetData(),cframe.GetLength());
		if (rc <= 0)
		{
			printf("Could not write to %s incoming bytes %d,return code %d errno %d\n",
				m_tsOutFile,cframe.GetLength(),rc,errno);
			
		}
		else
		{
			m_totalWriteBytes++;
		}
		
		
		frames++;
    }
	
    printf("tfirstTimestamp %I64u currentTimestamp %I64u difftstamp %I64u\n",tfirstTimestamp,sampleTimeStamp,difftstamp);
	printf("Found %d frames, totalWriteBytes %d\n",frames,m_totalWriteBytes);
	
    ms->Stop();
	
	_close(m_nFile);
	delete ms;
	
	delete [] mbStrBlobDir;

	return 0;
	
}
