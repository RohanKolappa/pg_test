#include "strmsrv_compatibility.h"
#include <sstream>

#include "readerdiskiothr.hh"
#include "reader.hh"
#include "squeue.hh"
#include "diskio.hh"
#include "MediaIndexUtils.hh"
#include "common.hh"

#include "FFUtils.hh"

using namespace std;

#define HWMARK_BYTES   (12 * 1024 * 1024)
#define LWMARK_BYTES   (1 * 1024 * 1024/2)

#define HWMARK_TIME   4000
#define LWMARK_TIME   3000

#define MAX_RETRY_PRINTS        10

CReaderDiskIOThread::CReaderDiskIOThread(CReader *a_reader,
                                         const char *a_pMediaName,
                                         const char *a_pMediaDir,
                                         double dSpeed,
                                         timestamp_t a_nTimecode,
                                         timestamp_t a_nOffSet)
{
    m_priority = 5; // lower than the other threads which are at 10
    m_bDetached = true; // This a non-joinable thread;

    // Set our pointer to the Reader object and set an extra reference
    // to it
    m_pReader = a_reader;
    m_pReader->AddReference();

    if (m_pThreadName != NULL) {
        sprintf(m_pThreadName, "ReaderDiskIO_%d", m_pReader->GetId());
    }
    m_bError = false;

    m_pSourceIndex = NULL;

    m_pMediaName = new char[MAXFILENAMELEN+1];
    m_pMediaDir = new char[MAXFILENAMELEN+1];
    m_pFileName = new char[MAXFILENAMELEN+1];

    memset(m_pMediaName, 0, MAXFILENAMELEN);
    memset(m_pMediaDir, 0, MAXFILENAMELEN);
    memset(m_pFileName, 0, MAXFILENAMELEN);

    sprintf(m_pMediaName, "%s", a_pMediaName);
    sprintf(m_pMediaDir, "%s", a_pMediaDir);
    m_pFileBaseName = m_pFileName;

    m_dSpeed = dSpeed;

    m_nReadTimestamp = a_nTimecode;
    m_nOffsetTimestamp = a_nOffSet;


    ostringstream qid;
    qid << "ReaderDiskIO_" << m_pReader->GetId();
    m_qDiskIOSink = CSQueueManager::Instance()->GetQueueSink(qid.str());

    m_buffering = false;
    m_nBufferOverRuns = 0;
    m_nBufferUnderRuns = 0;
    m_nFile = -1;


    m_fileCount = 0;
    m_fileError = false;
    m_bNeedNewChunk = true;
    m_bStartOfStreamSent = false;
    m_nSearchOffset = -1;
    m_nQueueTime = 0;
    m_nFileIndex = -1;


    m_nDiskIOStartTime = 0;

    m_nReadUntil = 0xffffffffffffffffULL;
    m_bFileOpen = false;
    m_nGapCount = 0;
    m_nRetries = 0;

    m_nReadOffset = 0;
    m_nReadBufferSize = 0;
    m_nBufferSize = 750000;

    m_sBuffer = new CSBuffer(m_nBufferSize + 1);
    m_sBuffer->AddReference();
    m_pReadBuffer = m_sBuffer->GetBuffer(0);

    m_readOperations = 0;
    m_nFilePosition = 0;
    m_nFrameErrors = 0;
    m_nMediaStartOffset = 0;

    m_nObjectCreateTime = CTimeKeeper::Now();

}

CReaderDiskIOThread::~CReaderDiskIOThread()
{

    if (m_pSourceIndex)
        delete(m_pSourceIndex);

    delete[] m_pFileName;
    delete[] m_pMediaName;
    delete[] m_pMediaDir;

    if (m_sBuffer)
        m_sBuffer->RemoveReference();

    if (m_qDiskIOSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qDiskIOSink);

    if (m_nFile > 0)
        close(m_nFile);

}

int
CReaderDiskIOThread::Run()
{
    // Init
    DiskIOInit();

    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        if (DiskIOProcessor() == -1) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        LightSleep();

    }

    // Remove our reference to the reader object so that it can die
    m_pReader->RemoveReference();

    // We are responsible to delete ourselves
    delete(this);

    return 0;
}


int
CReaderDiskIOThread::DiskIOInit()
{
    int rc = INDEX_OK;

    m_pSourceIndex = mediaindex::createSourceIndex(m_pMediaName, m_pMediaDir);

    // If initial offset is provided, use that to compute starting
    // timecode
    if (m_nOffsetTimestamp > 0) {
        rc = m_pSourceIndex->getTimecodeAtOffset(m_nOffsetTimestamp,
                                                 &m_nReadTimestamp);
        if (rc != INDEX_ERROR) {
            SYSLOG_INFO("Computed Timecode %"PRIu64" for media offset %"PRIu64"",
                        m_nReadTimestamp, m_nOffsetTimestamp);
        }
    }

    if (m_pSourceIndex != NULL && rc != INDEX_ERROR) {

        // Start the mediareader object
        rc = m_pSourceIndex->start(0, m_nReadTimestamp);

        if (rc != INDEX_ERROR) {
            m_pSourceIndex->getOffsetFromTimecode(m_nReadTimestamp,
                                                  &m_nMediaStartOffset);
        }
    }

    if (rc == INDEX_ERROR) {
        ostringstream os;
        os << "Media Index Error: " <<
            m_pSourceIndex->getLastError();
        if (m_nReadTimestamp == 0)
            m_nReadTimestamp = CTimeKeeper::Now();
        CSFrame *pSFrame = CreateErrorNotificationFrame(m_nReadTimestamp, os, false);

        if (pSFrame != NULL)
            m_qDiskIOSink->InsertFrame(pSFrame);

        m_fileError = true;
    }

    if (m_dSpeed > 1.0) {
        string mediaDir = string(m_pMediaDir) + "/blob/" + string(m_pMediaName) + "/";
        //m_trackDir = mediaDir;
        if (!CFFUtils::GetTrackDirForSpeed(mediaDir, m_dSpeed, m_trackDir)) {
            ostringstream os;
            os << "Missing Fast Forward Tracks";
            if (m_nReadTimestamp == 0)
                m_nReadTimestamp = CTimeKeeper::Now();
            CSFrame *pSFrame = CreateErrorNotificationFrame(m_nReadTimestamp, os, false);

            if (pSFrame != NULL)
                m_qDiskIOSink->InsertFrame(pSFrame);

            m_fileError = true;
        }
    }

    return 0;
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


/// Create a new buffer. If a_fresh is false, copy the last partial
/// frame of data from the old buffer into the beginning of the new
/// buffer
void
CReaderDiskIOThread::GetNewBuffer(bool a_fresh)
{
    unsigned long nOffset = 0;

    ASSERT(m_nReadBufferSize <= m_nBufferSize);
    ASSERT(m_nReadBufferSize >= m_nReadOffset);

    // Create a new sbuffer
    CSBuffer *sBuffer = new CSBuffer(m_nBufferSize + 1);
    char * buffer = sBuffer->GetBuffer(0);
    if (buffer == NULL) { // memory pressure, give up on this object
        m_fileError = true;
        SYSLOG_ERROR("Memory Error at %"PRIu64"", m_nReadTimestamp);
        close(m_nFile);
        m_nFile = -1;
        m_bFileOpen = false;
        ostringstream os;
        os << "Out of Memory";
        CSFrame *sFrame =
            CreateErrorNotificationFrame(m_nReadTimestamp, os, true);
        if (sFrame != NULL) {
            m_qDiskIOSink->InsertFrame(sFrame);
        }
        return;
    }
    sBuffer->AddReference();

    // Copy remaining unread data into it
    if (!a_fresh) {
        nOffset = m_nReadBufferSize - m_nReadOffset;

        SYSLOG_TRACE("Adjusting chunk data nOffset = %lu m_nReadBufferSize = %lu, m_nReadOffset = %lu",
                     nOffset, m_nReadBufferSize, m_nReadOffset);

        if (nOffset == m_nBufferSize) {
            // should not happen ordinarily but could happen
            // theoretically if there is not even a single frame found
            // in a whole chunk (data file possibly corrupt). We throw
            // away the chunk and start afresh
            SYSLOG_ERROR("Corrupt media file?? Chunk reset.");
            nOffset = 0;
            m_nReadOffset = 0;
        }
        memcpy(buffer, m_pReadBuffer + m_nReadOffset, nOffset);
    }
    m_nReadOffset = 0;
    m_nReadBufferSize = nOffset;

    // exchange old buffer for new
    m_pReadBuffer = buffer;
    m_sBuffer->RemoveReference();
    m_sBuffer = sBuffer;

}

/// Deserializes the data buffer and retrieves the next frame
unsigned long
CReaderDiskIOThread::GetNextFrame(timestamp_t & a_timestamp,
                                  unsigned long & a_length,
                                  unsigned long & a_flags)
{

    char *pData = m_pReadBuffer + m_nReadOffset;

    // Do we have a complete frame starting at current offset in the
    // current chunk?
    if (m_nReadOffset + sizeof(hdr_t) > m_nReadBufferSize)
        return 0; // too short to fit the header

    hdr_t * pHdr = (hdr_t *) pData;
    if (m_nReadOffset + sizeof(hdr_t) + pHdr->h_nLen > m_nReadBufferSize)
        return 0; // too short to fit the data

    // Frame found
    a_timestamp = pHdr->h_nTimestamp;
    a_length = pHdr->h_nLen;
    a_flags = pHdr->h_nFlags;

    // Compute offset into the buffer at which the actual data starts
    unsigned long offset = m_nReadOffset + sizeof(hdr_t);

    // Adjust the current read offset
    m_nReadOffset += pHdr->h_nLen + sizeof(hdr_t);

    return offset;
}

// If index file is available for the current minute file, locate the
// nearest offset from which we can start scanning for a frame with
// the given timestamp
unsigned long
CReaderDiskIOThread::SearchTimestamp(timestamp_t a_timestamp)
{
    char indexfile[MAXFILENAMELEN+1];
    char buf[256];
    unsigned long offset = 0;
    unsigned long nextOffset;
    timestamp_t nextTimestamp;

    SYSLOG_DEBUG("Looking for  offset at %"PRIu64"\n", a_timestamp);

    // Try to open index file
    sprintf(indexfile, "%s.idx", m_pFileName);
    FILE *fs = fopen(indexfile, "r");

    // File not found: caller needs to search from beginning
    if (fs == NULL)
        return 0;

    // Parse each line in the file
    while (fgets(buf, 32, fs) != NULL) {
        if (sscanf(buf, "%"PRIu64" %lu", &nextTimestamp, &nextOffset) >= 2) {
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

    SYSLOG_DEBUG("Found the closest offset for %"PRIu64" at %lu\n", a_timestamp, offset);
    return offset;
}

/// Read a chunk of data into the new buffer
int
CReaderDiskIOThread::ReadChunk()
{

    ASSERT(m_nReadBufferSize < m_nBufferSize);
    ASSERT(m_nFile > 0);
    ASSERT(m_pReadBuffer != NULL);

    CDiskIOThr::SetFileOpTimestamp(CTimeKeeper::Now());

    int nReadBytes = read(m_nFile ,
                          m_pReadBuffer + m_nReadBufferSize,
                          m_nBufferSize - m_nReadBufferSize);

    if (nReadBytes <= 0) {
        // nReadBytes == 0 : eof reached.
        // nReadBytes == -1 : error on file
        if (m_nReadBufferSize != 0) {
            // Since we have reached EOF, we expect our offset to be exactly zero.
            // Else we have got lost somewhere in this (possibly corrupted) minute file
            SYSLOG_INFO("Warning: Unexpected offset %lu within file %s (corrupt file). Resetting offset.\n",
                         m_nReadBufferSize, m_pFileName);
            m_nReadBufferSize = 0;
        }
        if (m_nRetries < MAX_RETRY_PRINTS) {
            SYSLOG_DEBUG("EOF (%d) on file: %s %lu @ %"PRIu64"\n", nReadBytes,
                         m_pFileName, m_nReadBufferSize, m_nFilePosition);
        }
        close(m_nFile);
        m_nFile = -1;
        m_bFileOpen = false;  // trigger new file open
        if (nReadBytes < 0) {
            SYSLOG_DEBUG("Read Error on file  %s: %s\n",
                         m_pFileName, strerror(errno));
            return -2;
        }
        return -1;
    }
    m_nReadBufferSize += nReadBytes;
    m_nFilePosition += nReadBytes;
    m_readOperations++;

    return 0;
}



/// Routine to check if we have to move to the next file
/// Assign the file names on the disk
int CReaderDiskIOThread::GetNextFile()
{
    char *pFileName = NULL;
    timestamp_t nBeginTimecode = 0;
    timestamp_t nEndTimecode = 0;
    unsigned long long length;
    unsigned long long nOffset;
    timestamp_t nNextStartTimeCode = 0;
    int rc;
    int discontinuity = 0;
    int eom = 0;
    string fileName;

    m_bFileOpen = false;

    if (m_pSourceIndex == NULL) {
        SYSLOG_ERROR("Failed to index media.\n");
        goto ERROR_LABEL;
    }

    rc = m_pSourceIndex->getNext(m_nReadTimestamp, -1,
                                 &nBeginTimecode, &nEndTimecode,
                                 &pFileName, &nNextStartTimeCode,
                                 &nOffset, &length, &discontinuity, &eom);
    if (pFileName) {
        fileName = pFileName;
        free(pFileName);
    }

    fileName = MapToTrackFile(fileName);

    if (rc == INDEX_ERROR) {
        if (eom == 1 && nBeginTimecode == 0) {
            // MediaIndex misbehaving..., assume indexing error
            eom = 0;
        }
        else if (eom == 1) {
            SYSLOG_DEBUG("End of Media: %s\n", m_pSourceIndex->getLastError());
        }
        else {
            SYSLOG_ERROR("Failed to get next media file for timestamp %"PRIu64": %s\n",
                         m_nReadTimestamp, m_pSourceIndex->getLastError());
        }
        goto ERROR_LABEL;
    }

    ASSERT(pFileName != NULL);

    if (m_nRetries < MAX_RETRY_PRINTS) {
        SYSLOG_DEBUG("File: %s\n", fileName.c_str());
        SYSLOG_DEBUG("%8s :%"PRIu64"\n", "TC:", m_nReadTimestamp);
        SYSLOG_DEBUG("%8s :%"PRIu64"\n", "begin:", nBeginTimecode);
        SYSLOG_DEBUG("%8s :%"PRIu64"\n", "End:", nEndTimecode);
        SYSLOG_DEBUG("%8s :%"PRIu64"\n", "Next:", nNextStartTimeCode);
        SYSLOG_DEBUG("Discontinuity:=%d nOffset=%"PRIu64" length=%"PRIu64"\n",
                     discontinuity, nOffset, length);
        SYSLOG_DEBUG("Filecount %d, Retries %d, %s", m_fileCount, m_nRetries,
                     rc == INDEX_RETRY ? "Repeat" : "New");
    }

    if (eom == 1) {
        SYSLOG_DEBUG("End of Media Detected.\n");
        goto ERROR_LABEL;
    }

#ifdef WINDOWS
    m_nFile = open(fileName.c_str(), O_RDONLY, 00666);
#else
    m_nFile = open(fileName.c_str(), O_RDONLY | O_NONBLOCK, 00666);
#endif
    if (m_nFile > 0) {
        // File open successful
        m_bFileOpen = true;
        if (strcmp(m_pFileName, fileName.c_str()) != 0) {
            // got a new file
            sprintf (m_pFileName, "%s", fileName.c_str());
            m_fileCount++;
            m_nFilePosition = 0;
            m_nRetries = 0;
            // set m_pFileBaseName to string after "blob"
            m_pFileBaseName = strrchr(m_pFileName, '/');
            m_nFileIndex = atoi(m_pFileBaseName + 1);
            while (m_pFileBaseName > m_pFileName) {
                m_pFileBaseName--;
                if (*m_pFileBaseName == '/')
                    break;
            }
        }
        else {
            // got the same old file
            m_nRetries++;
            if (m_nRetries < MAX_RETRY_PRINTS)
                SYSLOG_DEBUG("Seeking to position %"PRIu64" in file %s readbuffersize=%lu\n",
                             m_nFilePosition, fileName.c_str(), m_nReadBufferSize);

            if (lseek(m_nFile, (int)m_nFilePosition, SEEK_SET) < 0) {
                if (m_nRetries < MAX_RETRY_PRINTS)
                    SYSLOG_DEBUG("Failed seeking to current position in file %s\n",
                                 fileName.c_str());
                goto ERROR_LABEL;
            }

        }


        // Add special frame to mark discontinuity etc.
        if (discontinuity) {

            if (m_nGapCount > 0) { // ignore first gap

                // Send an EndOfStream marker followed by a Gap
                // marker. The receiver needs to know the gap start
                // and end timestamps.
                CSFrame *pSFrame;
                ASSERT(m_nReadUntil != 0);
                pSFrame = new CSFrame(m_nReadUntil, CSFrame::EndOfStream);
                if (pSFrame != NULL)
                    m_qDiskIOSink->InsertFrame(pSFrame);

                ASSERT(nBeginTimecode != 0);
                pSFrame = new CSFrame(nBeginTimecode, CSFrame::GapPresent);
                if (pSFrame != NULL)
                    m_qDiskIOSink->InsertFrame(pSFrame);
            }

            m_nGapCount++;
        }

        // Set "Read Until" timestamp so that we do not read beyond
        // the next gap start within the same file
        m_nReadUntil = nEndTimecode;

        // If we were looking from offset == 0, i.e. from the beginning of the
        // media, we can now set the expected timestamp of the first frame
        if (m_nReadTimestamp == 0)
            m_nReadTimestamp = nBeginTimecode;
    }
    else {
        // File open failed. Possibly minute file does not exist due
        // to "silence" in this minute. EOM has not yet been reported
        // by MediaIndex library so we can try to get the next file
        SYSLOG_DEBUG("Failed to open media file %s\n", fileName.c_str());
        m_nFile = -1;
        m_bFileOpen = false;
    }

    return 0;

ERROR_LABEL:
    m_fileError = true;

    CSFrame *pSFrame = NULL;
    if (eom == 1) {

        // Send an EndOfMedia frame
        if (!m_bStartOfStreamSent) {
            // If a StartOfStream frame has not gone out yet, send one
            if (m_nReadTimestamp == 0)
                m_nReadTimestamp = nBeginTimecode;
            SendStartOfStream(m_nReadTimestamp);
            if (m_nDiskIOStartTime == 0)
                m_nDiskIOStartTime = CTimeKeeper::Now();
        }

        if (nEndTimecode == 0)
            nEndTimecode = m_nReadTimestamp;
        if (nEndTimecode == 0)
            nEndTimecode = CTimeKeeper::Now();
        pSFrame = new CSFrame(nEndTimecode, CSFrame::EndOfMedia);
    }
    else {
        if (m_nReadTimestamp == 0)
            m_nReadTimestamp = CTimeKeeper::Now();

        ostringstream os;
        os << "Failed to get next media file for timestamp " <<
            m_nReadTimestamp << ": " << m_pSourceIndex->getLastError();
        pSFrame = CreateErrorNotificationFrame(m_nReadTimestamp, os, false);

    }
    if (pSFrame != NULL)
        m_qDiskIOSink->InsertFrame(pSFrame);

    return -1;
}

std::string CReaderDiskIOThread::MapToTrackFile(const std::string & minuteFilePath)
{
    if (m_dSpeed <= 1.0)
        return minuteFilePath;
    if (m_trackDir.length() == 0)
        return minuteFilePath;
    // splice in the trackDirctory just before the minutefilePath
    string::size_type pos = minuteFilePath.find_last_of("/");
    string mappedPath = minuteFilePath.substr(0, pos + 1) + m_trackDir +
            "/" + minuteFilePath.substr(pos + 1);
    return mappedPath;
}

bool
CReaderDiskIOThread::CheckFile()
{
    // Check if a new file needs to be opened
    if (!m_bFileOpen)
        GetNextFile();

    // We better have the file opened by now
    if (!m_bFileOpen || m_nFile <= 0) {
        return false;
    }
    return true;
}

// This thread can afford to block during file IO operations
int
CReaderDiskIOThread::DiskIOProcessor()
{

    unsigned long pLength;
    timestamp_t pTimestamp;
    unsigned long pFlags;
    int offset;

    // No need to do anything if a file error has been detected
    if (m_fileError)
        return 0;

    // If the object was just created, we have to locate the first frame
    // in the minute file
    if (m_nDiskIOStartTime == 0) {

        if (m_bNeedNewChunk) {

            if (CheckFile() == false) {
                // Did not find starting timestamp due to file error.
                // Go to normal mode
                m_nDiskIOStartTime = CTimeKeeper::Now();
                SYSLOG_DEBUG("Could not locate Start of Data in %s due to some file error\n",
                             m_pFileName);
                return 0;
            }

            // If index file file is available, use that to speed up our search
            if (m_nSearchOffset == -1) {
                m_nSearchOffset = SearchTimestamp(m_nReadTimestamp);
                if (m_nSearchOffset > 0) {
                    SYSLOG_DEBUG("Chunk Read: Seeking to position %d in file %s\n",
                                 m_nSearchOffset, m_pFileName);
                    m_nFilePosition = m_nSearchOffset;
                    if (lseek(m_nFile, (int)m_nSearchOffset, SEEK_SET) < 0) {
                        SYSLOG_ERROR("Failed seeking to current position in file %s\n", m_pFileName);
                    }
                }
                m_nSearchOffset = 0; // reset since we do this only one time
            }

            if (ReadChunk() < 0) {
                // Did not find starting timestamp due to EOF on file.
                // Go to normal mode
                m_nDiskIOStartTime = CTimeKeeper::Now();
                SYSLOG_DEBUG("Could not locate Start of Data in %s due to EOF on file\n",
                             m_pFileName);
                return 0;
            }
            m_bNeedNewChunk = false;
            return 0;
        }

        // Deserialize frames from the chunk
        while (1) {
            offset = GetNextFrame(pTimestamp, pLength, pFlags);
            if (offset > 0) {
                // compare timestamp against m_nReadTimestamp to see if
                // we found the first frame we are looking for
                ASSERT(m_nReadTimestamp > 0);
                if ((m_nReadTimestamp) <= pTimestamp) {
                    m_nDiskIOStartTime = CTimeKeeper::Now();

                    SYSLOG_DEBUG("Located Start of Data for ts=%"PRIu64" after %d.\n",
                                 m_nReadTimestamp,
                                 int(m_nDiskIOStartTime - m_nObjectCreateTime));


                    // Send the frame along

                    // First send a StartOfStream frame with the
                    // timestamp of expected start of media: the
                    // actual first frame may not have this timestamp
                    SendStartOfStream(m_nReadTimestamp);

                    // Now send the actual frame
                    m_nReadTimestamp = pTimestamp;
                    ASSERT(pTimestamp != 0);

                    CSFrame *pSFrame =
                        new CSFrame(m_sBuffer,
                                    offset,
                                    pLength,
                                    pFlags,
                                    pTimestamp);

                    if (pSFrame != NULL) {
                        m_qDiskIOSink->InsertFrame(pSFrame);
                    }

                    break;
                }
                else {
                    // continue searching in current chunk
                }
            }
            else {
                // Reached end of chunk. First frame not found
                // yet. Read another chunk
                GetNewBuffer(false);
                m_bNeedNewChunk = true;
                break;
            }
        }
        return 0;
    }

    // Main part of the routine: Read chunks of data from the media file
    // and deserialize frame by frame

    // If we are not buffering, turn it on if we go below the
    // low water mark
    if (!m_buffering) {
        if (m_pReader->GetCurrentTimeStamp() > 0) {
            m_nQueueTime = m_nReadTimestamp - m_pReader->GetCurrentTimeStamp();
        }
        else {
            m_nQueueTime = 0;
        }
        if (((m_nQueueTime < LWMARK_TIME)  &&
                (m_pReader->GetQueueSourceByteCount() <= HWMARK_BYTES)) ||
                (m_pReader->GetQueueSourceByteCount() <= LWMARK_BYTES)) {
            m_buffering = true;
            m_nBufferUnderRuns++;
        }
        else {
            return 0;
        }
    }

    // Check if a new chunk of data needs to be read
    if (m_bNeedNewChunk) {
        if (CheckFile() == true) {
            if (ReadChunk() == -2 ) {
                ostringstream os;
                os << "Read Error on file " << m_pFileName;
                CSFrame *sFrame =
                    CreateErrorNotificationFrame(m_nReadTimestamp, os, false);
                if (sFrame != NULL) {
                    m_qDiskIOSink->InsertFrame(sFrame);
                }

                m_fileError = true;
            }
            m_bNeedNewChunk = false;
        }
        return 0;
    }

    // If StartOfStream not sent yet for any reason, and we are not
    // waiting for a packet in the "future", we should send one now
    if (m_bStartOfStreamSent == false &&
            m_nReadTimestamp < CTimeKeeper::Now()) {
        SendStartOfStream(m_nReadTimestamp);
    }


    // Deserialize frames from the chunk
    while (1) {
        offset = GetNextFrame(pTimestamp, pLength, pFlags);

        if (offset > 0) { // found a complete frame

            // Back out if we have read beyond the recommended limit
            // (happens if there are gaps in the minute file)
            if (pTimestamp > m_nReadUntil) {
                SYSLOG_DEBUG("Read Until %"PRIu64" Reached: %s %lu\n",
                             m_nReadUntil,
                             m_pFileName, m_nReadBufferSize);
                m_nReadTimestamp = m_nReadUntil;
                m_nFilePosition -= m_nReadBufferSize - m_nReadOffset;
                m_nFilePosition -= pLength + sizeof(hdr_t);
                close(m_nFile);
                m_nFile = -1;
                m_bFileOpen = false;
                GetNewBuffer(true);
                m_bNeedNewChunk = true;
                break;
            }

            // Timestamps should ordinarily monotonically increase.
            // Send the frame along if this is true else flag an
            // error
            if ((m_nReadTimestamp) <= pTimestamp) {

                // If "StartOfStream" not sent yet, send it now. This
                // can occur due to any error in the above "locate
                // first frame" block, e.g. for requests in the
                // "future"
                if (m_bStartOfStreamSent == false) {
                    SendStartOfStream(m_nReadTimestamp);
                }

                m_nReadTimestamp = pTimestamp;
                CSFrame *pSFrame =
                    new CSFrame(m_sBuffer,
                                offset,
                                pLength,
                                pFlags,
                                pTimestamp);

                if (pSFrame != NULL) {
                    m_qDiskIOSink->InsertFrame(pSFrame);
                }

            }
            else {
                m_nFrameErrors++;
            }

        }
        else {
            // Reached end of chunk. Read next chunk.
            GetNewBuffer(false);
            m_bNeedNewChunk = true;
            break;
        }
    }

    // Check for high water mark
    if (m_pReader->GetCurrentTimeStamp() > 0) {
        m_nQueueTime = m_nReadTimestamp - m_pReader->GetCurrentTimeStamp();
    }
    else {
        m_nQueueTime = 0;
    }
    if ((m_nQueueTime > HWMARK_TIME) ||
            (m_pReader->GetQueueSourceByteCount() > HWMARK_BYTES)) {
        m_buffering = false;
        m_nBufferOverRuns++;
    }

    return 0;
}

void CReaderDiskIOThread::SendStartOfStream(timestamp_t nTimestamp)
{
    CSFrame *sFrame = new CSFrame(nTimestamp, CSFrame::StartOfStream);
    m_qDiskIOSink->InsertFrame(sFrame);
    m_bStartOfStreamSent = true;
}

// Create a CSFrame to send out error notification on the output queue
CSFrame *
CReaderDiskIOThread::CreateErrorNotificationFrame(timestamp_t a_timestamp,
                                                  ostringstream &os, bool isSystemError) {

    CSFrame *pSFrame =
        new CSFrame(a_timestamp, isSystemError ? CSFrame::SystemError : CSFrame::MediaError);
    pSFrame->SetData(os.str().c_str(), os.str().length());

    SYSLOG_ERROR(os.str().c_str());
    return pSFrame;
}

void
CReaderDiskIOThread::UpdateCustomStatus(CCommand *a_pCmd)
{

    char buffer[32];

    a_pCmd->SetObjectParam("MediaStartOffset",
                           CTimeKeeper::GetDurationString(m_nMediaStartOffset, buffer));
    a_pCmd->SetObjectCounterParam("ReadOperations", m_readOperations);
    a_pCmd->SetObjectCounterParam("FileCount", m_fileCount);
    a_pCmd->SetObjectCounterParam("Retries", m_nRetries);

#ifdef SSDEBUG

    if (m_qDiskIOSink != NULL) {
        a_pCmd->SetObjectCounterParam("DIOSinkContention",
                                      m_qDiskIOSink->GetContention());
    }
#endif

    a_pCmd->SetObjectParam("MediaDirectory", m_pMediaDir);
    a_pCmd->SetObjectParam("MediaName", m_pMediaName);

    a_pCmd->SetObjectParam("FileName", m_pFileBaseName);
    if (a_pCmd->StatusLevel(3)) {
        a_pCmd->SetObjectParam("FullFileName", m_pFileName);
    }
    a_pCmd->SetObjectParam("FileIndex", (int)m_nFileIndex);
    a_pCmd->SetObjectParam("TrackDirectory", m_trackDir);

    a_pCmd->SetObjectParam("FileError", (bool) m_fileError);

    a_pCmd->SetObjectGuageParam("ChunkSize",  m_nBufferSize);
    a_pCmd->SetObjectParam("SearchTime",
                           (unsigned long) (m_nDiskIOStartTime -
                                            m_nObjectCreateTime));
    a_pCmd->SetObjectErrorCounterParam("FrameErrors",
                                       m_nFrameErrors);
    a_pCmd->SetObjectParam("QueueTime", (unsigned long) m_nQueueTime);
    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectCounterParam("BufferOverRuns",
                m_nBufferOverRuns);
        a_pCmd->SetObjectCounterParam("BufferUnderRuns",
                m_nBufferUnderRuns);
    }
}
