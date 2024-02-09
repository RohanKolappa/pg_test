/*
 * MinuteFileMediaServer.cpp
 *
 *  Created on: Dec 16, 2011
 *      Author: rkale
 */

#include "CommonIncludes.hh"
#include "MinuteFileMediaServer.hh"
#include "MediaHeader.hh"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;

// Debug can be turned on locally if needed by defining
// DEBUG_CLASS (un-comment below hashdef)
//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define ASSERT(x) assert(x)
#define dummylog printf
#else
inline void dummylog(const char *format, ...) {};
#define ASSERT(x) do {} while (0)
#endif

#define SYSLOG_ERROR dummylog
#define SYSLOG_INFO dummylog
#define SYSLOG_DEBUG dummylog

namespace mediaindex {

CMinuteFileMediaServer::CMinuteFileMediaServer(const char* a_mediaPath)
: CMediaServer(a_mediaPath)
{
    m_pSourceIndex = NULL;
    m_nRetries = 0;
    m_nBufferSize = 524288;
    m_pReadBuffer = new uint8_t[m_nBufferSize + 1];
    m_pFileName = NULL;
    m_bAbort = false;
    m_bServePT = false;
    m_nIndexFileNumber = 0;
    m_nPrevIndexFileNumber = -1;
    m_bHaveMediaFrame = false;

    ResetVariables(); // Initialize all per session variables

}

CMinuteFileMediaServer::~CMinuteFileMediaServer()
{
    if (m_pFileName)
        free(m_pFileName);
    delete m_pSourceIndex;
    delete [] m_pReadBuffer;
}

// Common variable initialization routine
void
CMinuteFileMediaServer::ResetVariables()
{
    m_nFile = -1;
    m_nReadTimestamp = 0;
    m_nFrameTimestamp = 0;
    m_nFrameFlags = 0;
    m_nFrameLength = 0;
    m_pFrameData = 0;
    m_nMediaStartTimestamp = 0;
    m_nFirstMediaStartTimestamp = 0;
    m_nMediaStopTimestamp = 0;
    m_nMediaBeginTimestamp = 0;
    m_bFileOpen = false;
    m_fileError = false;
    m_bEndOfMedia = false;
    m_bNeedNewChunk = true;
    m_nReadOffset = 0;
    m_nReadBufferSize = 0;
    m_nFilePosition = 0;
    if (m_pFileName)
        free(m_pFileName);
    m_pFileName = strdup("");
    m_nReadUntil = 0xffffffffffffffffULL;
    m_nFrameCount = 0;
    m_nFramesInChunkCount = 0;
    m_readOperations = 0;
    m_fileCount = 0;
    m_nGapCount = 0;
    m_nFrameErrors = 0;
    m_nSearchOffset = -1;
    m_eState = Stopped;
    m_bServeEOM = false;
    m_bServeSOM = false;
    m_bEOMServed = false;
    m_nTotalGaps = 0;
    m_nFrameTimeOffset = 0;
    m_bHaveMediaFrame = false;

    m_nIndexFileNumber = 0;
    m_nPrevIndexFileNumber = -1;
    m_bServePT = m_bHaveThumbnails; // true if we had thumbnails
    m_lThumbnailInfo.clear();
}


int CMinuteFileMediaServer::Init()
{
    // Check if memory allocation. in constructor succeeded
    if (!m_pReadBuffer) {
        SetErrorString("Out of Memory");
        return MSRC_NO_RESOURCES;
    }

    if (m_pSourceIndex != NULL) {
        SetErrorString("Already Initialized");
        return MSRC_OBJECT_ERROR;
    }

    InitThumbnails();

    m_pSourceIndex = createSourceIndex(m_mediaPath, NULL);

    if (m_pSourceIndex == NULL) {
        SetErrorString("Out of Memory");
        return MSRC_NO_RESOURCES;
    }

    if (m_pSourceIndex->start(0, 0) != INDEX_OK) {
        SetErrorString(m_pSourceIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }


    return MSRC_OK;
}

int CMinuteFileMediaServer::Start(uint64_t a_startTimeCode)
{
    if (m_pSourceIndex == NULL) {
        SetErrorString("Not Yet Initialized");
        return MSRC_OBJECT_ERROR;
    }

    if (m_eState == Started) {
        SetErrorString("Service already started");
        return MSRC_OBJECT_ERROR;
    }

    ResetVariables(); // Re-Initialize all per session variables

    if (m_pSourceIndex->start(0, a_startTimeCode) != INDEX_OK) {
        SetErrorString(m_pSourceIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }

    m_eState = Started;

    m_nMediaBeginTimestamp = GetMediaStartTimestamp();

    if (a_startTimeCode > m_nMediaBeginTimestamp) {
        // Not starting from the beginning, so disable primary thumbnail
        m_bServePT = false;
    }

    m_nTotalGaps = GapsSoFar(a_startTimeCode);

    m_nReadTimestamp = a_startTimeCode; // First timecode to look for

    int rc;
    if ((rc = FindFirstFrame()) != 1) {
        // The next GetFrame should return immediately
        if (rc == -1) {
            // Abort request received during first search
            SetErrorString("Operation Aborted");
        }
        else {
            SetErrorString("End of Media Reached");
        }
        SetEndOfMedia(GetMediaEndTimestamp());

        return MSRC_MEDIA_ERROR;
    }

    return MSRC_OK;
}

int CMinuteFileMediaServer::Stop()
{
    if (m_eState == Stopped) {
        SetErrorString("Service already stopped");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Stopped;

    return MSRC_OK;
}

uint64_t CMinuteFileMediaServer::GetMediaStartTimestamp()
{
    if (m_pSourceIndex == NULL)
        return 0;

    uint64_t nTimecode = 0;
    int rc = m_pSourceIndex->getBeginTimecode(&nTimecode);

    if (rc != INDEX_OK)
        return 0;

    return nTimecode;
}

uint64_t CMinuteFileMediaServer::GetMediaEndTimestamp()
{
    if (m_pSourceIndex == NULL)
        return 0;

    uint64_t nTimecode = 0;
    int rc = m_pSourceIndex->getEndTimecode(&nTimecode);

    if (rc != INDEX_OK)
        return 0;

    return nTimecode;
}

uint64_t CMinuteFileMediaServer::GetTimecodeAtOffset(uint64_t a_nOffset)
{
    if (m_pSourceIndex == NULL)
        return 0;

    uint64_t nTimecode = 0;
    int rc = m_pSourceIndex->getTimecodeAtOffset(a_nOffset, &nTimecode);

    if (rc != INDEX_ERROR)
        return nTimecode;

    return 0;
}

uint64_t CMinuteFileMediaServer::GetOffsetFromTimecode(uint64_t a_nTimecode)
{
    if (m_pSourceIndex == NULL)
        return 0;

    uint64_t nOffset = 0;
    int rc = m_pSourceIndex->getOffsetFromTimecode(a_nTimecode, &nOffset);

    if (rc != INDEX_OK)
        return 0;

    return nOffset;

}

uint64_t CMinuteFileMediaServer::GetMediaDuration()
{
    return GetOffsetFromTimecode(GetMediaEndTimestamp()) -
            GetOffsetFromTimecode(GetMediaStartTimestamp());
}

uint64_t CMinuteFileMediaServer::GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset)
{
    char *pFileName = NULL;
    char *pPreviousFileName = NULL;
    uint64_t totalSize = 0;

    if (m_pSourceIndex == NULL) {
        return 0;
    }
    uint64_t beginTimestamp = GetTimecodeAtOffset(a_nBeginOffset);
    uint64_t endTimestamp = GetTimecodeAtOffset(a_nEndOffset);
    uint64_t timestamp = beginTimestamp;

    if (m_pSourceIndex->start(0, beginTimestamp) != INDEX_OK) {
        return 0;
    }
    while (true) {
        uint64_t nBeginTimecode = 0;
        uint64_t nEndTimecode = 0;
        uint64_t length;
        uint64_t nOffset;
        uint64_t nNextStartTimeCode = 0;
        int discontinuity = 0;
        int eom = 0;

        int rc = m_pSourceIndex->getNext(timestamp, -1, &nBeginTimecode, &nEndTimecode,
                &pFileName, &nNextStartTimeCode,
                &nOffset, &length, &discontinuity, &eom);
        if (rc == INDEX_ERROR || pFileName == NULL) {
            break;
        }
        // When there are gaps, files get repeated, so we compare against
        // last retrieved file name
        if (pPreviousFileName == NULL ||
                strcmp(pPreviousFileName, pFileName) != 0) {
            struct stat statbuf;
            if (stat(pFileName, &statbuf) != -1) {
                totalSize += statbuf.st_size;
            }
        }
        if (pPreviousFileName != NULL)
            free(pPreviousFileName);
        pPreviousFileName = pFileName;
        pFileName = NULL;
        timestamp = nEndTimecode;
        if (nEndTimecode >= endTimestamp || eom) {
            break;
        }
    }
    if (pPreviousFileName != NULL)
        free(pPreviousFileName);

    return totalSize;
}


list <CGap> CMinuteFileMediaServer::GetGaps() {
    list <CGap> lGaps;
    uint64_t request, gapstart, gapend;
    uint64_t mediaendtime = GetMediaEndTimestamp();
    if (mediaendtime == 0)
        return lGaps;
    if (GetMediaStartTimestamp() == 0)
        return lGaps;

    request = 0;
    while (true) {
        if (m_pSourceIndex->getNextGap(request, &gapstart, &gapend) == 0)
            break;
        CGap x(gapstart, gapend);
        lGaps.push_back(x);

        // get ready for next request
        request = gapend;
        if (request == 0 || request >= mediaendtime)
            break;
    }
    return lGaps;
}

// Total up all the gaps seen before this timestamp
uint64_t CMinuteFileMediaServer::GapsSoFar(uint64_t a_nTimestamp) {
    if (a_nTimestamp < GetMediaStartTimestamp()) {
        a_nTimestamp = GetMediaStartTimestamp();
    }

    uint64_t nGapsSoFar = 0;

    uint64_t request, gapstart, gapend;
    uint64_t mediaendtime = GetMediaEndTimestamp();
    if (mediaendtime == 0)
        return nGapsSoFar;
    if (GetMediaStartTimestamp() == 0)
        return nGapsSoFar;

    //return 0;

    request = 0;
    while (true) {
        if (m_pSourceIndex->getNextGap(request, &gapstart, &gapend) == 0)
            break;
        if (a_nTimestamp < gapstart)
            break;

        if (gapend >= gapstart) {
            nGapsSoFar += gapend - gapstart;
        }

        if (a_nTimestamp < gapend)
            break;

        // get ready for next request
        request = gapend;
        if (request == 0 || request >= mediaendtime)
            break;
    }
    return nGapsSoFar;
}

uint64_t CMinuteFileMediaServer::GetTimeSpan(uint64_t a_nTimestamp)
{
    if (m_nMediaBeginTimestamp == 0)
        return 0;
    if (a_nTimestamp > m_nMediaBeginTimestamp)
        return (a_nTimestamp - m_nMediaBeginTimestamp);
    return 0;
}

void CMinuteFileMediaServer::CheckTimeOffset(uint64_t a_nTimestamp)
{
#ifdef DEBUG_CLASS
        uint64_t nOffset;
        m_pSourceIndex->getOffsetFromTimecode(a_nTimestamp, &nOffset);
        if (m_nFrameTimeOffset != nOffset) {
            SYSLOG_DEBUG("%llu != %llu %llu %llu %llu\n", m_nFrameTimeOffset, nOffset,
                    a_nTimestamp, m_nMediaBeginTimestamp, m_nTotalGaps);
        }
        ASSERT(m_nFrameTimeOffset == nOffset);
#endif
}

// This can be called from an independent thread and will cause the
// this object to quit its processing/search loop as soon as possible.
// The object will behave as if end of media has been reached.
int CMinuteFileMediaServer::Abort()
{
    m_bAbort = true;
    return MSRC_OK;
}

bool CMinuteFileMediaServer::ServeDiscontinuities(CCFrame & a_cFrame) {

    if (m_bServeEOM) {
        ASSERT(m_nMediaStopTimestamp != 0);
        a_cFrame.SetMessageType(CCFrame::MediaStopped);
        a_cFrame.SetTimeStamp(m_nMediaStopTimestamp);
        a_cFrame.SetTimeOffset(m_nFrameTimeOffset);
        a_cFrame.SetMediaFlag(0);
        a_cFrame.SetLength(0);
        a_cFrame.SetData(0);
        m_nMediaStopTimestamp = 0; // reset till next gap
        m_bServeEOM = false;
        m_bEOMServed = true;
        return true;
    }

    if (m_bServeSOM != 0) {
        ASSERT(m_nMediaStartTimestamp != 0);
        a_cFrame.SetMessageType(CCFrame::MediaStarted);
        a_cFrame.SetTimeStamp(m_nMediaStartTimestamp);
        a_cFrame.SetTimeOffset(m_nFrameTimeOffset);
        a_cFrame.SetMediaFlag(0);
        a_cFrame.SetLength(0);
        a_cFrame.SetData(0);

        m_nMediaStartTimestamp = 0; // reset till next gap
        m_bServeSOM = false;
        m_bEOMServed = false; // Since media started again, another EOM is pending
        return true;
    }
    return false;
}

bool CMinuteFileMediaServer::ServeEOM(CCFrame & a_cFrame) {

    if (m_bEOMServed == false) {
        a_cFrame.SetMessageType(CCFrame::MediaStopped);
        a_cFrame.SetTimeStamp(GetMediaEndTimestamp());
        a_cFrame.SetTimeOffset(m_nFrameTimeOffset);
        a_cFrame.SetMediaFlag(0);
        a_cFrame.SetLength(0);
        a_cFrame.SetData(0);
        m_bEOMServed = true;
        return true;
    }

    return false;
}
// Retrieves the next frame and returns the contents in the CFrame
// object to the client
int
CMinuteFileMediaServer::GetFrame(CCFrame & a_cFrame)
{
    if (m_eState != Started) {
        SetErrorString("Service not yet started");
        return MSRC_OBJECT_ERROR;
    }

    if (m_bEndOfMedia) {
        if (ServeStreamThumbnail(a_cFrame)) {
            return MSRC_OK;
        }
        if (ServeDiscontinuities(a_cFrame)) {
            SYSLOG_DEBUG("Serving Gap before End of media %llu\n", m_nFrameCount);
            return MSRC_OK;
        }
        if (ServeEOM(a_cFrame)) {
            SYSLOG_DEBUG("Serving EOM before End of media %llu\n", m_nFrameCount);
            return MSRC_OK;
        }
        SYSLOG_DEBUG("End of Media already served?? %llu\n", m_nFrameCount);
        return MSRC_END_OF_MEDIA;
    }

    // loop till we find the next frame to serve
    int loopcount = 0;
    uint32_t nFileCount = m_fileCount;
    int32_t nFilePosition = m_nFilePosition;
    while (1) {

        if (ServeDiscontinuities(a_cFrame))
            return MSRC_OK;

        // Check if a new chunk of data needs to be read
        if (m_bNeedNewChunk) {
            SYSLOG_DEBUG("Need chunk %llu\n", m_nFrameCount);

            if (m_bAbort == true) {
                SetErrorString("Operation Aborted");
                SetEndOfMedia(GetMediaEndTimestamp());
                return MSRC_END_OF_MEDIA;
            }
            else if (CheckFile() == true) {
                ReadChunk();
                m_bNeedNewChunk = false;
            }
            else {
                // End of data reached??
                if (m_bEndOfMedia) {
                    SetErrorString("End of Media Reached");
                }
                else {
                    SetErrorString("Premature end of Media");
                    SetEndOfMedia(GetMediaEndTimestamp());
                }

                if (ServeStreamThumbnail(a_cFrame)) {
                    return MSRC_OK;
                }
                if (ServeDiscontinuities(a_cFrame)) {
                    SYSLOG_DEBUG("Serving a Gap before End of media %llu\n", m_nFrameCount);
                    return MSRC_OK;
                }
                SYSLOG_DEBUG("Serving End of media %llu\n", m_nFrameCount);
                return MSRC_END_OF_MEDIA;
            }
        }

        if (ServeDiscontinuities(a_cFrame))
            return MSRC_OK;

        if (ServePrimaryThumbnail(a_cFrame))
            return MSRC_OK;

        // Get next frame from the current chunk
        if (m_bHaveMediaFrame || (ProcessChunk() > 0)) {

            if (ServeStreamThumbnail(a_cFrame)) {
                // Skipped draining a regular media frame. Serve in next round.
                m_bHaveMediaFrame = true;
                return MSRC_OK;
            }

            a_cFrame.SetMessageType(CCFrame::Normal);
            a_cFrame.SetTimeStamp(m_nFrameTimestamp);
            a_cFrame.SetTimeOffset(m_nFrameTimeOffset);
            a_cFrame.SetMediaFlag(m_nFrameFlags);
            a_cFrame.SetLength(m_nFrameLength);
            a_cFrame.SetData(m_pFrameData);

            m_bHaveMediaFrame = false;
            return MSRC_OK;
        }

        // This loop should not take more than a couple of iterations
        // unless we are skipping files (due to silence).
        // If we take more than 2 loops, data is possibly corrupted
        // We keep looping as long as the file location keeps moving
        // or we keep opening new files
        if (nFileCount == m_fileCount && nFilePosition == m_nFilePosition) {
            loopcount++;
        }
        else {
            nFileCount = m_fileCount;
            nFilePosition = m_nFilePosition;
        }
        if (loopcount > 3)
            break;
        SYSLOG_DEBUG("Loop count %d @ %llu %u...\n",
                loopcount, m_nFrameCount, m_nFramesInChunkCount);

    }

    //ASSERT(0);
    SYSLOG_DEBUG("Media Corrupted\n");
    SetErrorString("Media Corrupted");
    SetEndOfMedia(GetMediaEndTimestamp());
    return MSRC_END_OF_MEDIA;
}


/*
 * Brief description of the variables used in the deserialization algorithm:
 *
 * m_pReadBuffer: Pointer to the beginning of data buffer, into which
 * data is read from the disk
 *
 * m_nBufferSize: Allocated size of the above data buffer (m_pReadBuffer)
 *
 * m_nReadBufferSize: Size of data read into data buffer (during read chunk)
 *
 * m_nReadOffset: Current offset of the data already read (parsed) in
 * the data buffer
 *
 * m_nReadTimestamp: Initially stores the first timestamp we are
 * looking for in the media and after the first frame is found it
 * tracks the timestamp of the last deserialized frame
 *
 */

/// Routine to check if we have to move to the next file
/// Assign the file names on the disk
int CMinuteFileMediaServer::GetNextFile()
{
    char *pFileName=NULL;
    uint64_t nBeginTimecode=0;
    uint64_t nEndTimecode=0;
    uint64_t length;
    uint64_t nOffset;
    uint64_t nNextStartTimeCode = 0;
    int rc;
    int discontinuity = 0;
    int eom = 0;

    m_bFileOpen = false;

    if (m_pSourceIndex == NULL) {
        SYSLOG_ERROR("Failed to index media.\n");
        goto ERROR_LABEL;
    }

    rc = m_pSourceIndex->getNext(m_nReadTimestamp, -1, &nBeginTimecode, &nEndTimecode,
                                 &pFileName, &nNextStartTimeCode,
                                 &nOffset, &length, &discontinuity, &eom);
    if (rc == INDEX_ERROR) {
        if (eom == 1 && nBeginTimecode == 0) {
            // MediaIndex misbehaving..., assume indexing error
            eom = 0;
        }
        else if (eom == 1) {
            SYSLOG_DEBUG("End of Media: %s\n", m_pSourceIndex->getLastError());
        }
        else {
            SYSLOG_ERROR("Failed to get next media file for timestamp %llu: %s\n",
                         m_nReadTimestamp, m_pSourceIndex->getLastError());
        }
        if (pFileName)
            free(pFileName);
        goto ERROR_LABEL;
    }

    ASSERT(pFileName != NULL);

    SYSLOG_DEBUG("File: %s\n", pFileName);
    SYSLOG_DEBUG("%8s :%llu %10llu\n", "TC:", m_nReadTimestamp, GetTimeSpan(m_nReadTimestamp));
    SYSLOG_DEBUG("%8s :%llu %10llu\n", "begin:", nBeginTimecode, GetTimeSpan(nBeginTimecode));
    SYSLOG_DEBUG("%8s :%llu %10llu\n", "End:", nEndTimecode, GetTimeSpan(nEndTimecode));
    SYSLOG_DEBUG("%8s :%llu %10llu\n", "Next:", nNextStartTimeCode, GetTimeSpan(nNextStartTimeCode));
    SYSLOG_DEBUG("Discontinuity:=%d nOffset=%llu length=%llu\n",
                 discontinuity, nOffset, length);
    SYSLOG_DEBUG("Filecount %d, Retries %d, eom=%d %s\n", m_fileCount, m_nRetries, eom,
                 rc == INDEX_RETRY ? "Repeat" : "New");

    if (eom == 1) {
        SYSLOG_DEBUG("End of Media Detected.\n");
        SetEndOfMedia(nEndTimecode);
        if (discontinuity) {
            // previous m_nReadUntil should have the gap begin
            SetDiscontinuity(m_nReadUntil, nBeginTimecode);
        }
        else {
        //    SetEndOfMedia(nEndTimecode);
        }
        free(pFileName);
        goto ERROR_LABEL;
    }

#ifdef WINDOWS
    m_nFile = OPEN(pFileName, _O_RDONLY | _O_BINARY);
#else
    m_nFile = OPEN(pFileName, O_RDONLY);
#endif


    if (m_nFile > 0) {
        // file open successful
        m_bFileOpen = true;
        if (strcmp(m_pFileName, pFileName) != 0) {
            // got a new file
            free(m_pFileName);
            m_pFileName = strdup(pFileName);
            m_fileCount++;
            m_nFilePosition = 0;

            // Determine index file number
            char * baseName = strrchr(m_pFileName, FILESEPERATOR_CHR);
            if (baseName != NULL)
                m_nIndexFileNumber = atoi(baseName + 1);
        }
        else {
            // got the same old file
            m_nRetries++;

            if (length == 0) { // End of media. TODO: is this the
                // definitive measure for end of media ??
                free(pFileName);
                CLOSE(m_nFile);
                m_nFile = -1;
                m_bFileOpen = false;
                SetEndOfMedia(nEndTimecode);
                goto ERROR_LABEL;
            }
            SYSLOG_DEBUG("Seeking to position %u in file %s readbuffersize=%u\n",
                         m_nFilePosition, pFileName, m_nReadBufferSize);
            if (lseek (m_nFile, (int)m_nFilePosition, SEEK_SET) < 0) {
                SYSLOG_ERROR("Failed seeking to current position in file %s\n", pFileName);
                free(pFileName);
                CLOSE(m_nFile);
                m_nFile = -1;
                m_bFileOpen = false;
                goto ERROR_LABEL;
            }

        }


        // Set discontinuity timestamp
        if (discontinuity) {
            // previous m_nReadUntil should have the gap begin
            SetDiscontinuity(m_nReadUntil, nBeginTimecode);
        }

        // Set "Read Until" time stamp so that we do not read beyond
        // the next gap start within the same file
        m_nReadUntil = nEndTimecode;

    }
    else {
        // File open failed. Possibly minute file does not exist due
        // to "silence" in this minute. EOM has not yet been reported
        // by MediaIndex library so we can try to get the next file
        SYSLOG_DEBUG("Failed to open media file %s\n", pFileName);
        m_nFile = -1;
        m_bFileOpen = false;
    }

    free(pFileName);
    return 0;

 ERROR_LABEL:
    if (eom == 1) {
    }
    else {
        SYSLOG_ERROR("Error In GetNextFile().\n");
    }
    m_fileError = true;
    return -1;
}

void
CMinuteFileMediaServer::SetEndOfMedia(uint64_t a_nEndtimeCode)
{
    m_nMediaStopTimestamp = a_nEndtimeCode;
    m_nFrameTimeOffset = (a_nEndtimeCode - m_nMediaBeginTimestamp) - m_nTotalGaps;
    //CheckTimeOffset(a_nEndtimeCode);
    m_bEndOfMedia = true;
    m_bServeEOM = true;
}

void
CMinuteFileMediaServer::SetDiscontinuity(uint64_t a_nEndtimeCode, uint64_t a_nBegintimeCode )
{
    // End of previous media before gap starts
    if (m_nGapCount > 0) { // avoid stop before first start
        m_nMediaStopTimestamp = a_nEndtimeCode;
        m_bServeEOM = true;
    }


    // Start of next media segment (gap ends)
    m_nMediaStartTimestamp = a_nBegintimeCode;
    if (m_nFirstMediaStartTimestamp == 0)
        m_nFirstMediaStartTimestamp = m_nMediaStartTimestamp;

    if (m_nGapCount == 0) {
        ASSERT(m_nMediaBeginTimestamp == m_nMediaStartTimestamp);
    }
    else {
        m_nTotalGaps += a_nBegintimeCode - a_nEndtimeCode;
    }
    m_bServeSOM = true;

    // Update Current Time Offset
    m_nFrameTimeOffset = (a_nBegintimeCode - m_nMediaBeginTimestamp) - m_nTotalGaps;
    CheckTimeOffset(a_nBegintimeCode);

    m_nGapCount++;

}

/// Create a new buffer. If a_fresh is false, copy the last partial
/// frame of data from the old buffer into the beginning of the new
/// buffer
void
CMinuteFileMediaServer::GetNewBuffer(bool a_fresh)
{
    uint32_t nOffset = 0;

    ASSERT(m_nReadBufferSize <= m_nBufferSize);
    ASSERT(m_nReadBufferSize >= m_nReadOffset);

    // Copy remaining unread data into it
    if (!a_fresh) {
        nOffset = m_nReadBufferSize - m_nReadOffset;
        SYSLOG_DEBUG("Adjusting chunk data nOffset = %u m_nReadBufferSize = %u, m_nReadOffset = %u\n",
                     nOffset, m_nReadBufferSize, m_nReadOffset);
        if (nOffset == m_nBufferSize) {
            // should not happen ordinarily but could happen
            // theoretically if there is not even a single frame found
            // in a whole chunk (data file possibly corrupt). We throw
            // away the chunk and start afresh
            SYSLOG_ERROR("Corrupt media file?? Chunk reset\n");
            nOffset = 0;
            m_nReadOffset = 0;
            //ASSERT(0);
        }
        memcpy(m_pReadBuffer, m_pReadBuffer + m_nReadOffset, nOffset);
    }
    m_nReadOffset = 0;
    m_nReadBufferSize = nOffset;

}

bool
CMinuteFileMediaServer::CheckFile()
{
    // Check if a new file needs to be opened
    if (!m_bFileOpen) {
        GetNextFile();
        CheckStreamThumbnails();
    }

    // We better have the file opened by now
    if (!m_bFileOpen || m_nFile <= 0) {
        return false;
    }
    return true;
}

/// Read a chunk of data into the new buffer
int
CMinuteFileMediaServer::ReadChunk()
{

    ASSERT(m_nReadBufferSize < m_nBufferSize);
    ASSERT(m_nFile > 0);
    ASSERT(m_pReadBuffer != NULL);

    m_nFramesInChunkCount = 0;
    SYSLOG_DEBUG("Reading  Chunk %d from file: %s @ %u\n",
                 m_readOperations, m_pFileName, m_nFilePosition);
    int nReadBytes = READ(m_nFile ,
                          m_pReadBuffer + m_nReadBufferSize,
                          m_nBufferSize - m_nReadBufferSize);

    m_readOperations++;
    if (nReadBytes <= 0) {
        // nReadBytes == 0 : eof reached.
        // nReadBytes == -1 : error on file
        if (m_nReadBufferSize != 0) {
            // Since we have reached EOF, we expect our offset to be exactly zero.
            // Else we have got lost somewhere in this (possibly corrupted) minute file
            SYSLOG_INFO("Warning: Unexpected offset %u within file %s (corrupt file). Resetting offset.\n",
                         m_nReadBufferSize, m_pFileName);
            m_nReadBufferSize = 0;
        }

        SYSLOG_DEBUG("EOF (%d) on file: %s %u @ %u\n", nReadBytes,
                     m_pFileName, m_nReadBufferSize, m_nFilePosition);
        close(m_nFile);
        m_nFile = -1;
        m_bFileOpen = false;  // trigger new file open
        if (nReadBytes < 0) {
            SYSLOG_DEBUG("Read Error on file  %s:\n",
                         m_pFileName);
            return -2;
        }
        return -1;
    }
    m_nReadBufferSize += nReadBytes;
    m_nFilePosition += nReadBytes;

    return 0;
}

/// Deserializes the data buffer and retrieves the next frame
uint32_t
CMinuteFileMediaServer::GetNextFrame(uint64_t & a_timestamp,
                           uint32_t & a_length, uint32_t & a_flags)
{

    uint8_t *pData = m_pReadBuffer + m_nReadOffset;

    // Do we have a complete frame starting at current offset in the
    // current chunk?
    if (m_nReadOffset + sizeof(media_header_t) > m_nReadBufferSize)
        return 0; // too short to fit the header

    media_header_t * pHdr = (media_header_t *) pData;
    if (m_nReadOffset + sizeof(media_header_t) + pHdr->h_nLen > m_nReadBufferSize)
        return 0; // too short to fit the data

    // Frame found
    a_timestamp = pHdr->h_nTimestamp;
    a_length = pHdr->h_nLen;
    a_flags = pHdr->h_nFlags;

    // Compute offset into the buffer at which the actual data starts
    uint32_t  offset = m_nReadOffset + sizeof(media_header_t);

    // Adjust the current read offset
    m_nReadOffset += pHdr->h_nLen + sizeof(media_header_t);


    return offset;
}

// If index file is available for the current minute file, locate the
// nearest offset from which we can start scanning for a frame with
// the given timestamp
uint32_t
CMinuteFileMediaServer::SearchTimestamp(uint64_t a_timestamp)
{
    char *indexfile;
    char buf[256];
    uint32_t offset = 0;
    uint32_t nextOffset;
    uint64_t nextTimestamp;

    SYSLOG_DEBUG("Looking for  offset at %llu\n", a_timestamp);

    // Try to open index file
    indexfile = (char *) malloc(strlen(m_pFileName) + 16);
    if (indexfile == NULL)
        return 0;
    sprintf(indexfile, "%s.idx", m_pFileName);
    FILE *fs = NULL;

#ifdef WINDOWS
    fs = fopen(indexfile, "rb");
#else
    fs = fopen(indexfile, "r");
#endif

    free(indexfile);

    // File not found: caller needs to search from beginning
    if (fs == NULL)
        return 0;

    // Parse each line in the file
    while (fgets(buf, 32, fs) != NULL) {
#ifdef WINDOWS
        if (sscanf(buf, "%20I64u %10u", &nextTimestamp, &nextOffset) >= 2) {
#else
        if (sscanf(buf, "%20llu %10u", &nextTimestamp, &nextOffset) >= 2) {
#endif
            if (nextTimestamp < a_timestamp) {
                offset = nextOffset;
            }
            else {
                break; // Found the closest
            }
        }
        else {
            break; // cannot scan the next line so we give up
        }
    }

    // close the index file
    fclose(fs);

    SYSLOG_DEBUG("Found the closest offset for %llu at %u\n", a_timestamp, offset);
    return offset;
}

int
CMinuteFileMediaServer::ProcessChunk()
{
    uint32_t offset;
    uint32_t pLength;
    uint32_t pFlags;
    uint64_t pTimestamp;

    while(1) {
        offset = GetNextFrame(pTimestamp, pLength, pFlags);

        if (offset > 0) { // found a complete frame

            // Back out if we have read beyond the recommended limit
            // (happens if there are gaps in the minute file)
            if (pTimestamp > m_nReadUntil) {
                SYSLOG_DEBUG("Read Until %lld Reached: %s %u\n",
                             m_nReadUntil,
                             m_pFileName, m_nReadBufferSize);
                m_nReadTimestamp = m_nReadUntil;
                m_nFilePosition -= m_nReadBufferSize - m_nReadOffset;
                m_nFilePosition -= pLength + sizeof(media_header_t);
                close(m_nFile);
                m_nFile = -1;
                m_bFileOpen = false;
                GetNewBuffer(true);
                m_bNeedNewChunk = true;
                break;
            }

            //Check for spurious frame.
            if (pTimestamp < (m_nMediaBeginTimestamp + m_nFrameTimeOffset + m_nTotalGaps))
                continue;

            // Timestamps should ordinarily monotonically increase.
            // Send the frame along if this is true else flag an
            // error
            if ((m_nReadTimestamp) <= pTimestamp) {
                m_nReadTimestamp = pTimestamp;

                // Save the frame details to return back to client
                m_nFrameTimestamp = pTimestamp;
                m_nFrameTimeOffset = (m_nFrameTimestamp - m_nMediaBeginTimestamp) - m_nTotalGaps;
                CheckTimeOffset(m_nFrameTimestamp);
                m_pFrameData = m_pReadBuffer + offset;
                m_nFrameFlags = pFlags;
                m_nFrameLength = pLength;
                m_nFrameCount++;
                m_nFramesInChunkCount++;
                return 1;
            }
            else {
                m_nFrameErrors++;
            }
        }
        else {
            // Reached end of chunk. Read next chunk.
            SYSLOG_DEBUG("Reached end of chunk, need a new one. %llu %u\n",
                    m_nFrameCount, m_nFramesInChunkCount);
            GetNewBuffer(false);
            m_bNeedNewChunk = true;
            break;
        }
    }

    return 0;
}


int
CMinuteFileMediaServer::FindFirstFrame()
{
    uint32_t offset;
    uint32_t pLength;
    uint32_t pFlags;
    uint64_t pTimestamp;

    while(1) {
        if (m_bAbort) { // Abort request, quit searching
            return -1;
        }
        ASSERT(m_bNeedNewChunk == true);
        if (m_bNeedNewChunk) {

            if (CheckFile() == false) {
                // Did not find starting timestamp due to some file error.
                // Go to normal mode
                SYSLOG_ERROR("Error finding starting timestamp %llu in the first minute file.\n",
                             m_nReadTimestamp);
                if (m_bEndOfMedia)
                    return 0;
                else
                    return 1;
            }
            // If index file file is available, use that to speed up our search
            if (m_nSearchOffset == -1) {
                m_nSearchOffset = SearchTimestamp(m_nReadTimestamp);
                if (m_nSearchOffset > 0) {
                    SYSLOG_DEBUG("Chunk Read: Seeking to position %d in file %s\n",
                                 m_nSearchOffset, m_pFileName);
                    m_nFilePosition = m_nSearchOffset;
                    if (lseek (m_nFile, (int)m_nSearchOffset, SEEK_SET) < 0) {
                        SYSLOG_ERROR("Failed seeking to current position in file %s\n",
                                     m_pFileName);
                        return 0; // TODO should we return here?
                    }
                }
                m_nSearchOffset = 0; // reset since we do this only one time
            }

            if (ReadChunk() < 0) {
                // Did not find starting timestamp due to EOF on file.
                // Go to normal mode
                SYSLOG_DEBUG("Could not locate Start of Data in %s due to EOF on file\n",
                             m_pFileName);
                return 1;

            }
            m_bNeedNewChunk = false;
        }

        // Deserialize frames from the chunk
        while(1) {
            offset = GetNextFrame(pTimestamp, pLength, pFlags);
            if (offset > 0) {
                // Compare timestamp against m_nReadTimestamp to see
                // if we found the first frame we are looking for
                //ASSERT(m_nReadTimestamp > 0);
                if ((m_nReadTimestamp) <= pTimestamp) {
                    // Found the first frame
                    // Back out one frame, for the next getframe to pick up this frame
                    m_nReadTimestamp = pTimestamp;
                    m_nReadOffset -= pLength + sizeof(media_header_t);
                    return 1;
                }
                else {
                    // continue searching in current chunk
                }
            }
            else {
                // Reached end of chunk. First frame not found yet.
                // Read another chunk
                GetNewBuffer(false);
                m_bNeedNewChunk = true;
                break; // out of inner while
            }
        }
    }

    return 0;

}

void CMinuteFileMediaServer::InitThumbnails()
{
    if (!m_bServeThumbnails)
        return;


    m_ThumbnailDir = m_mediaPath;
    m_ThumbnailDir.append(FILESEPERATOR);
    m_ThumbnailDir.append("tn");
    m_ThumbnailDir.append(FILESEPERATOR);

    const boost::filesystem::path aPath(m_ThumbnailDir);
    if (boost::filesystem::exists(aPath)) {
        m_bHaveThumbnails = true;
        m_bServePT = true;
    }
}

bool CMinuteFileMediaServer::ServePrimaryThumbnail(CCFrame & a_cFrame)
{
    if (!m_bServeThumbnails)
        return false;

    if (!m_bServePT)
        return false;

    m_bServePT = false;

    int nBufferSize;
    uint32_t mediaFlag = IMAGETYPE_JPEG;
    nBufferSize = ReadImageFile(m_ThumbnailDir + "tn.jpg");
    if (nBufferSize < 0) { // Try png file
        mediaFlag = IMAGETYPE_PNG;
        nBufferSize = ReadImageFile(m_ThumbnailDir + "tn.png");
        if (nBufferSize < 0) {
            return false;
        }
    }

    mediaFlag |= TNTYPE_PRIMARY;

    a_cFrame.SetMessageType(CCFrame::Thumbnail);
    a_cFrame.SetTimeStamp(m_nMediaBeginTimestamp);
    a_cFrame.SetTimeOffset(0);
    a_cFrame.SetMediaFlag(mediaFlag);
    a_cFrame.SetLength(nBufferSize);
    a_cFrame.SetData(m_pImageFrameBuffer);

    return true;
}

bool CMinuteFileMediaServer::ServeStreamThumbnail(CCFrame & a_cFrame)
{

    if (!m_bServeThumbnails)
        return false;

    // If there is a thumbnail with timestamp lesser than the next media frame
    // serve that thumbnail. Else keep it in the list to check for next round.

    // If there are thumbnails discovered with timestamps earlier than when the media
    // was started, throw them away.

    CImageFileInfo imageInfo;
    while (m_lThumbnailInfo.size() > 0) {
        imageInfo = m_lThumbnailInfo.front();
        if (imageInfo.m_nTimestamp >= m_nFirstMediaStartTimestamp)
            break;
        m_lThumbnailInfo.pop_front();
    }
    if (m_lThumbnailInfo.size() == 0)
        return false;

    if (imageInfo.m_nTimestamp > m_nFrameTimestamp && !m_bEndOfMedia) {
        return false;
    }
    m_lThumbnailInfo.pop_front();

    int nBufferSize = ReadImageFile(m_ThumbnailDir + imageInfo.m_fileName);
    if (nBufferSize <= 0)
        return false;


    uint32_t mediaFlag = imageInfo.m_nType & IMAGETYPE_MASK;
    mediaFlag |= TNTYPE_STREAM;

    a_cFrame.SetMessageType(CCFrame::Thumbnail);
    a_cFrame.SetTimeStamp(imageInfo.m_nTimestamp);
    uint64_t nTimeOffset = (imageInfo.m_nTimestamp - m_nMediaBeginTimestamp) - m_nTotalGaps;
    a_cFrame.SetTimeOffset(nTimeOffset);
    a_cFrame.SetMediaFlag(mediaFlag);
    a_cFrame.SetLength(nBufferSize);
    a_cFrame.SetData(m_pImageFrameBuffer);

    return true;
}

void CMinuteFileMediaServer::CheckStreamThumbnails()
{
    if (!m_bServeThumbnails)
        return;

    if (m_nPrevIndexFileNumber == m_nIndexFileNumber)
        return;

    m_nPrevIndexFileNumber = m_nIndexFileNumber;

    // We do not clear the previous list in m_lThumbnailInfo since
    // we need to serve to serve all TNs from
    // the last minute file before serving the ones in the next minute file

    string indexFileName(m_ThumbnailDir);
    char indexbuf[32];
    sprintf(indexbuf, "%d", m_nIndexFileNumber);
    indexFileName.append(indexbuf);
    indexFileName.append(".tdx");

    // Try to open index file
    FILE *fp = NULL;

#ifdef WINDOWS
    fp = fopen(indexFileName.c_str(), "rb");
#else
    fp = fopen(indexFileName.c_str(), "r");
#endif

    if (fp == NULL)
        return;


    char linebuf[128];
    while (fgets(linebuf, 255, fp) != NULL) {
        char tsstr[64];
        char filename[64];

        sscanf(linebuf, "%s %s", tsstr, filename);
        uint64_t timestamp = STRTOULL(tsstr, NULL, 0);

        int imageType = GetImageType(filename);
        if (imageType == IMAGETYPE_UNKNOWN)
            continue;

        CImageFileInfo tnInfo(timestamp, imageType, filename);
        m_lThumbnailInfo.push_back(tnInfo);
    }

    fclose(fp);
}

};
