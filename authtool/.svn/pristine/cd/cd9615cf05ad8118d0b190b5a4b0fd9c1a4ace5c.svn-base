#ifndef V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED
#define V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFileCreator.h"
#include "mp4.h"
#include "MediaHeader.hh"
#include "MediaServer.hh"
#include "vframeparser_c.h"

#define ISV2DVIDEOFRAME(flag) ((flag) & (FRAMETYPE_V2D_VIDEO))
#define ISV2DAUDIOFRAME(flag) ((flag) & (FRAMETYPE_V2D_AUDIO))
#define ISV2DCONTROLFRAME(flag) ((flag) & (FRAMETYPE_V2D_CTL))

class CV2dFileCreator :
    public IFileCreator
{
public:
    CV2dFileCreator(LPOLESTR a_szBlobDir, LPOLESTR a_szV2DOutFile, 
        const unsigned int a_nStartMinute, const unsigned int a_nEndMinute,
        bool a_bAllMinutes);
    CV2dFileCreator(void);
    virtual ~CV2dFileCreator(void);

	virtual int Initialize();
	virtual int CreateFile();

    bool DumpOutFileInfo();

private:

    bool HandleVideoCFrame(CCFrame &a_rCFrame);
    bool HandleControlCFrame(CCFrame &a_rCFrame);

    bool WriteVideoFrameToFile(
        const uint64_t& a_rnSampleTimeStamp, 
        const uint8_t* a_pFrameData, 
        const size_t a_nFrameLen);

    unsigned int m_nStartMinute;
    unsigned int m_nEndMinute;
    LPOLESTR m_szBlobDir;
    LPOLESTR m_szV2DOutFile;

    u_int32_t m_nMP4LibVerbosity;
    int m_iCreateFlags;

    uint64_t m_nTotalCFrames;
    uint64_t m_nVideoCFrames;
    uint64_t m_nVideoCFramesDiscarded;
    uint64_t m_nAudioCFrames;
    uint64_t m_nAudioCFramesDiscarded;
    uint64_t m_nControlCFrames;

    MP4FileHandle   m_hV2DFile;
    MP4TrackId      m_nVideoTrackId;

    v2dParserHandle m_hV2DParser;

    uint64_t m_nLastVideoFrameTimeStamp;
    uint32_t m_nVFramesWritten;
    uint32_t m_nVFramesWriteErr;

    uint16_t m_nWidth;
    uint16_t m_nHeight;
    int m_nSliceSize;

    uint8_t*    m_pVideoFrameBuff;
    size_t      m_nVideoFrameBuffLen;
};

#endif // V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED
