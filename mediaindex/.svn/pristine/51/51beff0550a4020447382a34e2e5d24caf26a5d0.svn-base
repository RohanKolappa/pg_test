/*
 * MediaWriter.cpp
 *
 *  Created on: Feb 24, 2010
 *      Author: rkale
 */

#include <ctype.h>
#include <stdio.h>

#include "CommonIncludes.hh"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include "MediaServer.hh"
#include "MediaWriter.hh"
#include "MediaHeader.hh"

#include <string>

namespace bfs = boost::filesystem;

// Debug can be turned on locally if needed by defining
// DEBUG_CLASS (un-comment below hashdef)
//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define ASSERT(x) assert(x)
#define dummylog printf
#else
// Variable number of args in macros not supported by VC++. Hence the
// below ugliness...
inline void dummylog(const char *format, ...) {};
#define ASSERT(x) do {} while (0)
#endif

#define SYSLOG_DEBUG dummylog



#define MAXFILENAMELEN          256

typedef struct hdr
{
    uint64_t         h_nTimestamp;
    uint32_t         h_nFlags;
    uint32_t         h_nLen;
} hdr_t;

namespace mediaindex {

CMediaWriter::CMediaWriter(const char* a_mediaDirectory)
{

    m_mediaDirectory = NULL;
    if (a_mediaDirectory != NULL) {
        m_mediaDirectory = new char [strlen(a_mediaDirectory) + 1];
        strcpy(m_mediaDirectory, a_mediaDirectory);
    }

    m_errorString = strdup("No Error");
    m_pDestinationIndex = NULL;
    m_pTNDestinationIndex = NULL;
    m_nBufferSize = 750000;
    m_pWriteBuffer = new uint8_t[m_nBufferSize + 1];
    m_pFileName = NULL;;
    m_eState = Stopped;
    m_nSnapshotIndex = 0;

    m_nFileStartTime = 0;
    m_nFileRemainingLength = 60000;

    m_nFileFD = 0;
    m_nIndexFileFD = 0;
    m_nSync = 0;

    m_pFileName = new char[MAXFILENAMELEN+1];
    m_pIndexFileName = new char[MAXFILENAMELEN+1];
    memset(m_pFileName, 0, MAXFILENAMELEN);
    memset(m_pIndexFileName, 0, MAXFILENAMELEN);

    m_nWriteOperations = 0;
    m_nBytesWritten = 0;

    m_nTimestamp = 0;
    m_nIndexTimestamp = 0;
    m_nIndexOffset = 0;
    m_nLastWriteTime = 0;
    m_nWriteOffset = 0;
    m_nMediaStartTime = 0;
    m_bAppendMode = false;
    m_bDropBuffers = false;
    m_bInitThumbnails = false;
    m_bDoThumbnails = false;
    m_bDoMedia = true;
}


CMediaWriter::~CMediaWriter()
{
    CloseFiles();

    delete [] m_pFileName;
    delete [] m_pIndexFileName;
    delete [] m_mediaDirectory;
    delete m_pDestinationIndex;
    delete m_pTNDestinationIndex;
    delete [] m_pWriteBuffer;
    if (m_errorString)
        free(m_errorString);

}

void
CMediaWriter::SetErrorString(const char *a_errorString)
{
    m_eState = Errored;
    if (m_errorString)
        free(m_errorString);
    m_errorString = strdup(a_errorString);
}

int
CMediaWriter::CleanMediaDirectory()
{
    // Check directory exists and delete all files that look like media in it
    bfs::path dirPath(m_mediaDirectory);
    if (!bfs::exists(dirPath)) {
        char buffer[256];
        snprintf(buffer, 255, "Cannot open directory \"%s\"", m_mediaDirectory);
        SetErrorString(buffer);
        return MSRC_MEDIA_ERROR;
    }

    bfs::directory_iterator end;
    for (bfs::directory_iterator iter(dirPath); iter != end; ++iter) {
        if (bfs::is_directory(iter->status()))
            continue; // Skip directory

        bool bDelete = false;
        char basename[64 + 1];
        basename[64] = 0;
#if defined(__MINGW32__) || defined(__ARM__)
        strncpy(basename, iter->path().leaf().string().c_str(), 64);
#else
        strncpy(basename, iter->path().leaf().c_str(), 64);
#endif
        if (strcmp(basename, "INDEX") == 0)
            bDelete = true;
        const char *cp = basename;
        bool bMinuteFile = true;
        while (*cp != '\0') {
            if (!isdigit(*cp)) {
                if (strcmp(cp, ".idx") == 0)
                    break;
                bMinuteFile = false;
                break;
            }
            cp++;
        }
        if (bMinuteFile)
            bDelete = true;
        if (!bDelete)
            continue;
        bfs::remove_all(iter->path());
    }

    return MSRC_OK;
}

int
CMediaWriter::Init(uint64_t trimLength, bool syncWrites)
{
#ifndef WINDOWS
    if (syncWrites)
        m_nSync = O_SYNC;
#endif

    if (!m_pWriteBuffer) {
        SetErrorString("Out of Memory");
        return MSRC_NO_RESOURCES;
    }

    if (m_bDoThumbnails)
        InitThumbnails();

    if (m_bDoMedia)
        return InitMedia(trimLength);

    return MSRC_OK;
}

int CMediaWriter::InitMedia(uint64_t trimLength)
{
    if (!m_bAppendMode) {
        int rc = CleanMediaDirectory();
        if (rc != MSRC_OK)
            return rc;
    }

    if (m_pDestinationIndex != NULL) {
        SetErrorString("Already Initialized");
        return MSRC_OBJECT_ERROR;
    }

    m_pDestinationIndex = createDestIndex(m_mediaDirectory, NULL, trimLength);
    if (m_pDestinationIndex == NULL) {
        SetErrorString("Out of Memory");
        return MSRC_NO_RESOURCES;
    }

    return MSRC_OK;
}

int CMediaWriter::Start(uint64_t nTimecode)
{
    if (m_eState == Started) {
        SetErrorString("Service already started");
        return MSRC_OBJECT_ERROR;
    }

    if (nTimecode == 0) {
        SetErrorString("Invalid Start Timecode");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Started;

    if (m_bDoThumbnails)
        StartThumbnails(nTimecode);

    if (m_bDoMedia)
        return StartMedia(nTimecode);

    return MSRC_OK;


}

int CMediaWriter::StartMedia(uint64_t nTimecode)
{
    if (m_pDestinationIndex->start(nTimecode, nTimecode) != INDEX_OK) {
        SetErrorString(m_pDestinationIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }

    // Initialize our time and open the first file
    m_nTimestamp = nTimecode;
    m_nMediaStartTime = nTimecode;
    CheckFile();

    return MSRC_OK;
}

int CMediaWriter::Stop(uint64_t nTimecode)
{
    if (m_eState == Stopped) {
        SetErrorString("Service already stopped");
        return MSRC_OBJECT_ERROR;
    }

    if (nTimecode == 0) {
        SetErrorString("Invalid Stop Timecode");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Stopped;

    if (m_bDoThumbnails)
        StopThumbnails(nTimecode);

    if (m_bDoMedia)
        return StopMedia(nTimecode);

    return MSRC_OK;


}

int CMediaWriter::StopMedia(uint64_t nTimecode)
{

    // First write out whatever is collected so far.
    // Need to call CheckFile too to ensure we write out
    // older silent minute files if needed.
    m_nTimestamp = nTimecode;
    CheckFile();
    FlushBuffer();

    if (m_pDestinationIndex->stop(nTimecode) != INDEX_OK) {
        SetErrorString(m_pDestinationIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }
    return MSRC_OK;
}

void CMediaWriter::SetAppendMode(bool appendMode)
{
    m_bAppendMode = appendMode;
}

void CMediaWriter::SetDropBuffers(bool dropBuffers)
{
    m_bDropBuffers = dropBuffers;
}

int CMediaWriter::PutFrame(CCFrame & a_cFrame)
{
    if (m_eState == Errored) {
        return MSRC_OBJECT_ERROR;
    }

    if (m_eState != Started) {
        SetErrorString("Service not yet started");
        return MSRC_OBJECT_ERROR;
    }

    if (a_cFrame.GetMessageType() != CCFrame::Normal) {
        SetErrorString("Wrong type of frame detected.");
        return MSRC_OBJECT_ERROR;
    }

    if (!m_bDoMedia)
        return MSRC_OK;

    // Track current timestamp
    m_nTimestamp = a_cFrame.GetTimeStamp();

    CheckFile();

    // Write the frame header
    hdr_t   hdr;
    hdr.h_nFlags = a_cFrame.GetMediaFlag();
    hdr.h_nTimestamp = m_nTimestamp;
    hdr.h_nLen   = a_cFrame.GetLength();

    if ((int)hdr.h_nLen + (int)sizeof(hdr) + m_nWriteOffset >
            m_nBufferSize) {
        FlushBuffer();
    }
    if (m_nTimestamp - m_nLastWriteTime > 1000) {
        FlushBuffer();
    }

    // Set the timestamp and file offset of the first frame going
    // out in the next chunk
    if (m_nIndexTimestamp == 0) {
        m_nIndexTimestamp = m_nTimestamp;
        SYSLOG_DEBUG("Setting Index offset %u at %llu\n",
                m_nIndexOffset, m_nIndexTimestamp);
        m_nLastIndexOffset = m_nIndexOffset;
    }
    memcpy(m_pWriteBuffer + m_nWriteOffset,
           (void *)&hdr, sizeof (hdr_t));
    m_nWriteOffset += sizeof(hdr_t);

    memcpy(m_pWriteBuffer + m_nWriteOffset,
           (void *)a_cFrame.GetData(), a_cFrame.GetLength());
    m_nWriteOffset += hdr.h_nLen;

    return MSRC_OK;
}

uint64_t
CMediaWriter::GetCurrentTimeStamp() {
#if 0
    timestamp_t nTimeStamp = 0;
    if (m_nTimestamp > 0) {
        ASSERT(m_nFrameWallClock > 0);
        if (m_nState & OBJ_STARTED) {
            nTimeStamp = m_nTimestamp + (CTimeKeeper::Now() - m_nFrameWallClock);
        }
        else {
            ASSERT(m_nStopWallClock > 0);
            nTimeStamp = m_nTimestamp + (m_nStopWallClock - m_nFrameWallClock);
        }
    }
    return nTimeStamp;
#else
    return m_nTimestamp;
#endif
}

void CMediaWriter::CloseFiles() {

    if (m_nFileFD > 0) {
        close(m_nFileFD);
        m_nFileFD = 0;
    }
    if (m_nIndexFileFD > 0) {
        close(m_nIndexFileFD);
        m_nIndexFileFD = 0;
    }

}

int CMediaWriter::GetNextFile(uint64_t a_nTimestamp)
{
    unsigned long long  offset, length;
    char *pFileName = NULL;

    // Close current file handles
    CloseFiles();

    if (m_pDestinationIndex == NULL)
        return 0;

    m_pDestinationIndex->getNext(a_nTimestamp, &pFileName, &offset, &length);

    if (pFileName == NULL)
        return 0;
    SYSLOG_DEBUG("File %s, Timestamp %llu, offset=%llu Remaining %llu\n",
            pFileName, a_nTimestamp, offset, length);

#if defined WINDOWS
    m_nFileFD = open(pFileName,
                     O_CREAT | O_APPEND | O_WRONLY | O_BINARY, S_IRUSR | S_IWUSR);
#else
    m_nFileFD = open(pFileName,
                     O_CREAT | O_APPEND | O_WRONLY | m_nSync, 00666);
#endif
    if (m_nFileFD > 0) {

        // Use RemainingLength to record how many more 'ticks' we
        // will be writing to this minute file
        m_nFileRemainingLength = static_cast<uint32_t>(length);
        sprintf(m_pFileName, pFileName);
        sprintf(m_pIndexFileName, "%s.idx", pFileName);

        // Open index file too
#if defined WINDOWS
        m_nIndexFileFD = open(m_pIndexFileName,
                              O_CREAT | O_APPEND | O_WRONLY | O_BINARY, S_IRUSR | S_IWUSR);
#else
        m_nIndexFileFD = open(m_pIndexFileName,
                              O_CREAT | O_APPEND | O_WRONLY, 00666);
#endif
        if (m_nIndexFileFD <= 0) {
            SetErrorString("Failed to open index file.");
            CloseFiles();
        }

    }
    else {
        SetErrorString("Failed to open minute file.");
        CloseFiles();
    }

    free(pFileName);

    return 0;
}

int CMediaWriter::CheckFile()
{
    int problem = 0;

    if (m_nTimestamp == 0) {
        SetErrorString("Inconsistent internal state. Bug??.");
        return -1;
    }

  REDO:
    if (problem++ > 10) {
        ASSERT(0);
    }
    uint64_t nTimestamp = GetCurrentTimeStamp();
    bool bRequestNext = false;
    bool bRedo = false;
    // We are done with this minute file if we have written upto the
    // the remaining length of the file
    if ((m_nFileStartTime > 0) && (nTimestamp > m_nFileStartTime) &&
            ((nTimestamp - m_nFileStartTime) >= m_nFileRemainingLength) ) {

        SYSLOG_DEBUG("Scheduling GetNextFile with %llu due to rollover\n", nTimestamp);
        FlushBuffer();
        bRequestNext = true;
        // If the new timestamp is too far ahead (will happen if there
        // a silence lasting more than a minute) we need to open and
        // close all intermediate minute files
        if ((nTimestamp - m_nFileStartTime) >= 60000 + m_nFileRemainingLength) {
            if ((nTimestamp - m_nMediaStartTime) >= 60000 + m_nFileRemainingLength) {
                nTimestamp = m_nFileStartTime + 60000;
                SYSLOG_DEBUG("Scheduling REDO with %llu\n", nTimestamp);
                bRedo = true;
            }
        }

        m_nFileStartTime = 0;
    }
    else if (m_nFileStartTime == 0) {
        SYSLOG_DEBUG("Scheduling GetNextFile with %llu due to first call\n", nTimestamp);
        bRequestNext = true;
    }

    if (bRequestNext) {
        // Open next file
        GetNextFile(nTimestamp);

        // Starting to write to a new file so reset these
        m_nIndexTimestamp = 0;
        m_nIndexOffset = 0;

        // Set FileStartTime to current time
        m_nFileStartTime = nTimestamp;

    }
    if (bRedo)
        goto REDO;

    return 0;
}

/// Routine to Empty Buffer to the disk
int CMediaWriter::FlushBuffer()
{
    if (m_nFileFD <= 0 || m_nIndexFileFD <= 0) {
        SetErrorString("Inconsistent internal state. Bug??.");
        return -1;
    }

    if (m_bDropBuffers) {
        // Can be set by client code when there is need to gracefully
        // backoff under disk I/O pressure
        m_nWriteOffset = 0;
        m_nIndexTimestamp = 0;
        m_nLastWriteTime = m_nTimestamp;
        return 0;
    }

    // Write to minute file
    int rc  = write(m_nFileFD, m_pWriteBuffer, m_nWriteOffset);
    if (rc < 0) {
        char buffer[256];
        snprintf(buffer, 255, "Write failed for file %s", m_pFileName);
        SetErrorString(buffer);
        CloseFiles();
        return -1;
    }
    m_nBytesWritten += m_nWriteOffset;
    m_nLastWriteTime = m_nTimestamp;
    m_nIndexOffset += m_nWriteOffset;
    m_nWriteOffset = 0;
    m_nWriteOperations++;

    // Also write out the index file
    if (m_nIndexTimestamp != 0) {
        char buf[64];
#ifdef WINDOWS
        sprintf(buf, "%I64u %u\n", m_nIndexTimestamp, m_nLastIndexOffset);
#else
        sprintf(buf, "%"PRIu64" %u\n", m_nIndexTimestamp, m_nLastIndexOffset);
#endif
        int len = strlen(buf);
        rc  = write(m_nIndexFileFD, buf, len);
        if (rc < 0) {
            char buffer[256];
            snprintf(buffer, 255, "Write failed for file %s", m_pIndexFileName);
            SetErrorString(buffer);
            CloseFiles();
            return -1;
        }

        m_nBytesWritten += len;
        m_nIndexTimestamp = 0;
    }

    return 0;
}

uint64_t CMediaWriter::GetBytesWritten() {
    return m_nBytesWritten;
}

uint64_t CMediaWriter::GetBytesDeleted() {
    if (m_pDestinationIndex == NULL)
        return 0;
    return m_pDestinationIndex->getBytesDeleted();
}

int CMediaWriter::AddSnapshot(CCFrame & a_cFrame)
{
    if (a_cFrame.GetMessageType() != CCFrame::Snapshot) {
        SetErrorString("Wrong type of frame detected.");
        return MSRC_OBJECT_ERROR;
    }

    std::string indexfile(m_mediaDirectory);
    indexfile.append(FILESEPERATOR);
    indexfile.append("SSINDEX");

    std::string snapshotfile("snapshot_");
    char indexbuf[64];
    sprintf(indexbuf, "%d", m_nSnapshotIndex);
    m_nSnapshotIndex++;
    snapshotfile.append(indexbuf);
    snapshotfile.append(".");
    snapshotfile.append(GetImageExtension(a_cFrame.GetMediaFlag() & IMAGETYPE_MASK));

    // Write the index file
    snprintf(indexbuf, 64, "%"PRIu64" %s\n", a_cFrame.GetTimeStamp(), snapshotfile.c_str());
    WriteFileFromBuffer(indexfile, (uint8_t *)indexbuf, strlen(indexbuf), true);

    // Write the snapshot file
    std::string snapshotfilepath(m_mediaDirectory);
    snapshotfilepath.append(FILESEPERATOR);
    snapshotfilepath.append(snapshotfile);

    WriteFileFromBuffer(snapshotfilepath, a_cFrame.GetData(), a_cFrame.GetLength());

    return MSRC_OK;
}


int CMediaWriter::AddThumbnail(CCFrame & a_cFrame)
{
    if (a_cFrame.GetMessageType() != CCFrame::Thumbnail) {
        SetErrorString("Wrong type of frame detected.");
        return MSRC_OBJECT_ERROR;
    }

    if (m_eState != Started &&
            (a_cFrame.GetMediaFlag() & TNTYPE_MASK) == TNTYPE_STREAM) {
        SetErrorString("Service not yet started");
        return MSRC_OBJECT_ERROR;
    }


    if (!m_bDoThumbnails)
        return MSRC_OK;

    if ((a_cFrame.GetMediaFlag() & TNTYPE_MASK) == TNTYPE_PRIMARY) {
        std::string tnFileName(m_ThumbnailDir);
        tnFileName.append(FILESEPERATOR);
        tnFileName.append("tn");
        tnFileName.append(".");
        tnFileName.append(GetImageExtension(a_cFrame.GetMediaFlag() & IMAGETYPE_MASK));
        WriteFileFromBuffer(tnFileName, a_cFrame.GetData(), a_cFrame.GetLength());
    }
    else if ((a_cFrame.GetMediaFlag() & TNTYPE_MASK) == TNTYPE_STREAM) {
        char buffer[32];
        sprintf(buffer, "%"PRIu64"", a_cFrame.GetTimeStamp());
        std::string tnFileName("");
        tnFileName.append(buffer);
        tnFileName.append(".");
        tnFileName.append(GetImageExtension(a_cFrame.GetMediaFlag() & IMAGETYPE_MASK));

        UpdateIndexFile(tnFileName, a_cFrame.GetTimeStamp());

        std::string tnFullFileName(m_ThumbnailDir);
        tnFullFileName.append(FILESEPERATOR);
        tnFullFileName.append(tnFileName);
        WriteFileFromBuffer(tnFullFileName, a_cFrame.GetData(), a_cFrame.GetLength());
    }

    return MSRC_OK;
}

int CMediaWriter::InitThumbnails()
{
    if (m_bInitThumbnails)
        return MSRC_OK;
    m_bInitThumbnails = true;

    m_ThumbnailDir = m_mediaDirectory;
    m_ThumbnailDir.append(FILESEPERATOR);
    m_ThumbnailDir.append("tn");

    const boost::filesystem::path aPath(m_ThumbnailDir);
    if (boost::filesystem::exists(aPath)) {
        boost::filesystem::remove_all(aPath);
    }
    boost::filesystem::create_directory(aPath);


    m_pTNDestinationIndex = createDestIndex((char *)m_ThumbnailDir.c_str(), NULL, 0);
    if (m_pTNDestinationIndex == NULL) {
        SetErrorString("Out of Memory");
        return MSRC_NO_RESOURCES;
    }

    return MSRC_OK;
}

int CMediaWriter::StartThumbnails(uint64_t nTimecode)
{
    if (m_pTNDestinationIndex->start(nTimecode, nTimecode) != INDEX_OK) {
        SetErrorString(m_pTNDestinationIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }
    return MSRC_OK;
}

int CMediaWriter::StopThumbnails(uint64_t nTimecode)
{
    if (m_pTNDestinationIndex->stop(nTimecode) != INDEX_OK) {
        SetErrorString(m_pTNDestinationIndex->getLastError());
        return MSRC_MEDIA_ERROR;
    }
    return MSRC_OK;
}

void CMediaWriter::EnableThumbnails(bool bEnable)
{
    m_bDoThumbnails = bEnable;
}

void CMediaWriter::EnableMedia(bool bEnable)
{
    m_bDoMedia = bEnable;
}

void CMediaWriter::UpdateIndexFile(std::string tnFileName, uint64_t nTimestamp)
{
    uint64_t beginTimeCode = 0;
    m_pTNDestinationIndex->getBeginTimecode(&beginTimeCode);
    if (nTimestamp <= beginTimeCode)
        return;

    int indexFileNumber = (nTimestamp - beginTimeCode)/60000;

    std::string indexFileName(m_ThumbnailDir);
    indexFileName.append(FILESEPERATOR);
    char indexbuf[64];
    snprintf(indexbuf, 64, "%d", indexFileNumber);
    indexFileName.append(indexbuf);
    indexFileName.append(".tdx");

    snprintf(indexbuf, 64, "%"PRIu64" %s\n", nTimestamp, tnFileName.c_str());
    WriteFileFromBuffer(indexFileName, (uint8_t *)indexbuf, strlen(indexbuf), true);
}

void CMediaWriter::WriteFileFromBuffer(std::string fileName,
        uint8_t * buffer, int bufferLen, bool append)
{
    int mode = O_CREAT | O_WRONLY | m_nSync;

#ifdef WINDOWS
    mode |= O_BINARY;
#endif
    
    if (append)
        mode |= O_APPEND;

    int fd = open(fileName.c_str(), mode, 00666);
    if (fd > 0) {
        write(fd, buffer, bufferLen);
        close(fd);
    }
}


std::string CMediaWriter::GetImageExtension(int imageType) {
    switch (imageType) {
    case IMAGETYPE_JPEG:
        return "jpg";
    case IMAGETYPE_PNG:
        return "png";
    case IMAGETYPE_BMP:
        return "bmp";
    case IMAGETYPE_GIF:
        return "gif";
    case IMAGETYPE_TIFF:
        return "tif";
    }
    return "";
}

};
