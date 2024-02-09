/*
 * MinuteFileMediaServer.hh
 *
 *  Created on: Dec 16, 2011
 *      Author: rkale
 */

#ifndef MINUTEFILEMEDIASERVER_HH_
#define MINUTEFILEMEDIASERVER_HH_

#include "MediaServer.hh"

#include <string>

namespace mediaindex {


class CMinuteFileMediaServer : public CMediaServer {
public:
    CMinuteFileMediaServer(const char* a_mediaPath);
    virtual ~CMinuteFileMediaServer();

    int Init();
    int Start(uint64_t startTimeCode);
    int Stop();
    int Abort();

    int GetFrame(CCFrame &a_cFrame);

    uint64_t GetMediaStartTimestamp();
    uint64_t GetMediaEndTimestamp();
    uint64_t GetTimecodeAtOffset(uint64_t a_nOffset);
    uint64_t GetOffsetFromTimecode(uint64_t a_nTimecode);
    uint64_t GetMediaDuration();
    uint64_t GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset);

    std::list <CGap> GetGaps();

private:
    SourceIndexInterface* m_pSourceIndex;
    uint8_t * m_pReadBuffer;
    uint32_t m_nBufferSize;
    uint32_t m_nReadBufferSize;
    uint32_t m_nReadOffset;
    int m_nRetries;
    uint64_t m_nReadTimestamp;
    bool m_bFileOpen;
    bool m_fileError;
    bool m_bEndOfMedia;
    bool m_bNeedNewChunk;
    bool m_bAbort;
    bool m_bServeEOM;
    bool m_bServeSOM;
    bool m_bEOMServed;
    bool m_bServePT;
    bool m_bHaveMediaFrame;
    int m_nFile;
    int32_t m_nFilePosition;
    char * m_pFileName;
    uint32_t m_fileCount;
    uint32_t m_nGapCount;
    uint64_t m_nReadUntil;
    uint64_t m_nFrameErrors;
    uint64_t m_nFrameCount;
    uint32_t m_nFramesInChunkCount;
    uint64_t m_nFrameTimestamp;
    uint64_t m_nMediaBeginTimestamp;
    uint64_t m_nMediaStartTimestamp;
    uint64_t m_nMediaStopTimestamp;
    uint64_t m_nFirstMediaStartTimestamp;
    uint64_t m_nFrameTimeOffset;
    uint64_t m_nTotalGaps;
    uint32_t m_nFrameFlags;
    uint32_t m_nFrameLength;
    uint8_t * m_pFrameData;
    int m_nSearchOffset;
    std::string m_ThumbnailDir;
    int m_nIndexFileNumber;
    int m_nPrevIndexFileNumber;
    std::list <CImageFileInfo> m_lThumbnailInfo;

    // Internal routines
    void ResetVariables();
    int GetNextFile();
    bool ReadytoStart();
    void GetNewBuffer(bool a_fresh);
    bool CheckFile();
    int ReadChunk();
    int ProcessChunk();
    int FindFirstFrame();
    bool ServeDiscontinuities(CCFrame & a_cFrame);
    bool ServeEOM(CCFrame & a_cFrame);
    void SetEndOfMedia(uint64_t a_nEndTimecode);
    void SetDiscontinuity(uint64_t a_nEndtimeCode, uint64_t a_nBegintimeCode);
    uint64_t GetTimeSpan(uint64_t a_nTimestamp);
    void CheckTimeOffset(uint64_t a_nTimestamp);
    uint64_t GapsSoFar(uint64_t a_nTimestamp);
    uint32_t SearchTimestamp(uint64_t a_timestamp);
    uint32_t GetNextFrame(uint64_t & a_timestamp,
                          uint32_t & a_length,
                          uint32_t & a_flags);
    void InitThumbnails();
    bool ServePrimaryThumbnail(CCFrame & a_cFrame);
    bool ServeStreamThumbnail(CCFrame & a_cFrame);
    void CheckStreamThumbnails();

};

};

#endif /* MINUTEFILEMEDIASERVER_HH_ */
