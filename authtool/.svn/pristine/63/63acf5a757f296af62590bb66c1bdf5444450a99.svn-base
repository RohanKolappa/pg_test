#include "stdafx.h"
#include "V2dFileCreator.h"
#include "MediaServer.hh"
#include "v2d.h"

#include <iostream>
#include <string>
#include <memory>
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::auto_ptr;

#define V2DTIMESCALE 1000
#define EXTENDED_FRAMEHEADER_SIZE 9
#define SOF2 0x00000101

CV2dFileCreator::CV2dFileCreator(LPOLESTR a_szBlobDir, LPOLESTR a_szV2DOutFile, 
        const unsigned int a_nStartMinute, const unsigned int a_nEndMinute,
        bool a_bAllMinutes) 
        : m_nTotalCFrames(0)
        , m_nVideoCFrames(0)
        , m_nVideoCFramesDiscarded(0)
        , m_nAudioCFrames(0)
        , m_nAudioCFramesDiscarded(0)
        , m_nControlCFrames(0)
        , m_hV2DParser(NULL)
        , m_nLastVideoFrameTimeStamp(0)
        , m_nVFramesWritten(0)
        , m_nVFramesWriteErr(0)
        , m_nWidth(0)
        , m_nHeight(0)
        , m_nSliceSize(0)
        , m_pVideoFrameBuff(0)
        , m_nVideoFrameBuffLen(0)
{
	m_szBlobDir     = a_szBlobDir;
	m_szV2DOutFile  = a_szV2DOutFile;
	
	if(!a_bAllMinutes)
	{
		m_nStartMinute = a_nStartMinute;
		m_nEndMinute   = a_nEndMinute;
	}
	else
    {
        m_nStartMinute = 0;
        m_nEndMinute   = 1; // TODO: Remove this after prototyping
    }

	m_nMP4LibVerbosity  = MP4_DETAILS_ERROR;
    m_iCreateFlags      = 0;
}

CV2dFileCreator::~CV2dFileCreator(void)
{
    v2d_parser_close(m_hV2DParser);
    delete [] m_pVideoFrameBuff;
}

int CV2dFileCreator::Initialize()
{
    m_hV2DParser = v2d_parser_init();

    char *pmbStrOutFile = getMultiByteStr(m_szV2DOutFile);

	m_hV2DFile = MP4CreateEx(pmbStrOutFile, m_nMP4LibVerbosity, 
        (m_iCreateFlags != 0), 1, 0, "v2d ", 0, NULL, 0);

    delete [] pmbStrOutFile;
    return (m_hV2DFile) ? 0 : 1;
}

int CV2dFileCreator::CreateFile()
{
    bool bErr = false, bRc = false, bVideoTrackPresent = false;
    CCFrame cFrame;
    
    char *pmbStrBlobDir = getMultiByteStr(m_szBlobDir);
	
	auto_ptr<CMediaServer> pMediaServer( new CMediaServer(pmbStrBlobDir) );
    if (pMediaServer.get() == NULL) 
	{
        bErr = true;
        cerr << "Could not create MediaServer object. " << endl;
        goto v2dfilecreate_cleanup;
	}
	
    if (pMediaServer->Init() != MSRC_OK) 
	{
        bErr = true;
        cerr << "MediaServer Init Failed: " << 
            pMediaServer->GetErrorString() << endl;
        goto v2dfilecreate_cleanup;
	}
	
    if (pMediaServer->Start(m_nStartMinute) != MSRC_OK) 
	{
        bErr = true;
        cerr << "MediaServer Start Failed: " << pMediaServer->GetErrorString()
            << endl;
        goto v2dfilecreate_cleanup;
	} 

	uint32_t nSampleLength = 0;
	uint64_t nSampleTimeStamp = 0;
	uint32_t nSampleMediaFlag = 0;
    uint64_t nFirstTimestamp = 0;

	while(1) 
	{
        if (pMediaServer->GetFrame(cFrame) != MSRC_OK) 
		{
			cerr << "MediaServer Get Frame Failed: " << 
                pMediaServer->GetErrorString() << endl;
			break;
		}
        m_nTotalCFrames++;

        nSampleLength       = cFrame.GetLength();
        nSampleTimeStamp    = cFrame.GetTimeStamp();
        nSampleMediaFlag    = cFrame.GetMediaFlag();

        // TODO: Check for for filesize before writing because of mpeg4ip
        // library limitation of 4 GB files.

		if(!nFirstTimestamp)
            nFirstTimestamp = nSampleTimeStamp;

		if((nSampleTimeStamp - nFirstTimestamp) < (m_nStartMinute * 60000))
			continue;
				
		if(m_nEndMinute)
		{
			if( (nSampleTimeStamp - nFirstTimestamp) >= (m_nEndMinute * 60000) )
				break;
		}				
		
        if(ISV2DCONTROLFRAME(nSampleMediaFlag))
        {
            m_nControlCFrames++;
            HandleControlCFrame(cFrame);
        }
		else if (ISV2DVIDEOFRAME(nSampleMediaFlag))
		{
			m_nVideoCFrames++;
            if(!m_nWidth || !m_nHeight || !m_nSliceSize)
            {
                m_nVideoCFramesDiscarded++;
                continue;
            }

			if(!bVideoTrackPresent)
			{
				m_nVideoTrackId = MP4AddVideoTrack(
                    m_hV2DFile,
                    V2DTIMESCALE,
                    0,
                    m_nWidth, m_nHeight,
                    MP4_MPEG4_VIDEO_TYPE);
				if (m_nVideoTrackId == MP4_INVALID_TRACK_ID)
					break;
				bVideoTrackPresent = true;
			}
				
            HandleVideoCFrame(cFrame);
		}
        else if(ISV2DAUDIOFRAME(nSampleMediaFlag))
        {
            m_nAudioCFrames++;
        }
    }
	
    pMediaServer->Stop();

v2dfilecreate_cleanup:   
    delete [] pmbStrBlobDir;
    MP4Close(m_hV2DFile);

	cout <<"Total Frames written " << m_nVFramesWritten << endl;
    DumpOutFileInfo();
    return (bErr) ? 1 : 0;
}

bool CV2dFileCreator::DumpOutFileInfo()
{
    bool bRet = true;

    char *pszOutFile = getMultiByteStr(m_szV2DOutFile);
	char* info = MP4FileInfo(pszOutFile);  

    if(info)
    {
        bRet = true;
        cout << pszOutFile << " Info: " << endl << info << endl;
    }
    else
    {
        bRet = false;
        cerr << "Failed to get info on " << pszOutFile << endl;
    }

    delete [] pszOutFile;
    return bRet;
}

bool CV2dFileCreator::HandleVideoCFrame(CCFrame &a_rCFrame)
{
    uint32_t nSampleLength      = a_rCFrame.GetLength() - V2D_PACKET_HEADER_SIZE;
    uint64_t nSampleTimeStamp   = a_rCFrame.GetTimeStamp();
    uint8_t *pCFrameData        = a_rCFrame.GetData() + V2D_PACKET_HEADER_SIZE;

    const uint8_t *pOutFrame  = NULL;
    int nOutFrameLen = 0;
    size_t nBytesParsed = 0;

    while(nSampleLength) 
    {
        nBytesParsed = v2d_parse_frame(m_hV2DParser, &pOutFrame, &nOutFrameLen, 
            pCFrameData, nSampleLength);

        nSampleLength -= nBytesParsed;
        pCFrameData += nBytesParsed;

        if(nOutFrameLen)
        {
            WriteVideoFrameToFile(nSampleTimeStamp, pOutFrame, nOutFrameLen);
        }
    }

    return true;
}

bool CV2dFileCreator::HandleControlCFrame(CCFrame &a_rCFrame)
{
    ScapData_t stCapData;
    char *pCtlData = reinterpret_cast<char *>(a_rCFrame.GetData());

    if(v2dLibParseScap( &stCapData, pCtlData ) == 0)
    {
        if (stCapData.m_hResSet) 
        {
            m_nWidth = stCapData.m_hRes;
        }
        if (stCapData.m_vResSet) 
        {
            m_nHeight = stCapData.m_vRes;
        }
        if (stCapData.m_sliceSizeSet) 
        {
            m_nSliceSize = stCapData.m_sliceSize;
        }     
    }

    return true;
}

bool CV2dFileCreator::WriteVideoFrameToFile(
    const uint64_t& a_rnSampleTimeStamp, 
    const uint8_t* a_pFrameData, 
    const size_t a_nFrameLen)
{
    bool bRc = true;
    MP4Duration nVSampleDuration = (!m_nLastVideoFrameTimeStamp) ? 
        0 : (a_rnSampleTimeStamp - m_nLastVideoFrameTimeStamp);

    if(m_nLastVideoFrameTimeStamp && !nVSampleDuration)
        nVSampleDuration = 1;
    m_nLastVideoFrameTimeStamp = a_rnSampleTimeStamp;

    if(m_nVideoFrameBuffLen < a_nFrameLen)
    {
		//if( m_pVideoFrameBuff != NULL)
		//	delete [] m_pVideoFrameBuff;
        m_nVideoFrameBuffLen = a_nFrameLen + EXTENDED_FRAMEHEADER_SIZE;
        m_pVideoFrameBuff = new uint8_t[m_nVideoFrameBuffLen];
    }

    // Add the extended header fields
    ::memset(m_pVideoFrameBuff, 0x00, 1);
    ::memset(m_pVideoFrameBuff+1, 0x00, 1);
    ::memset(m_pVideoFrameBuff+2, 0x01, 1);
    ::memset(m_pVideoFrameBuff+3, 0x01, 1);
    
    uint16_t nWidth     = ::htons(m_nWidth);
    uint16_t nHeight    = ::htons(m_nHeight);
    ::memcpy(m_pVideoFrameBuff+4, &nWidth, sizeof(nWidth));
    ::memcpy(m_pVideoFrameBuff+6, &nHeight, sizeof(nHeight));
    ::memset(m_pVideoFrameBuff+8, m_nSliceSize, 1);

    ::memcpy(m_pVideoFrameBuff+EXTENDED_FRAMEHEADER_SIZE, a_pFrameData, a_nFrameLen);

    if( !MP4WriteSample(m_hV2DFile, m_nVideoTrackId, m_pVideoFrameBuff, 
        a_nFrameLen+EXTENDED_FRAMEHEADER_SIZE, nVSampleDuration) )
    {
        m_nVFramesWriteErr++;
        bRc = false;
    }
    else
    {
        m_nVFramesWritten++;
    }
    
    return bRc;
}
