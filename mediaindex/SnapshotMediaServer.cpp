/*
 * SnapshotMediaServer.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: rkale
 */


#include "CommonIncludes.hh"
#include "SnapshotMediaServer.hh"
#include "MediaHeader.hh"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;

namespace mediaindex {

CSnapshotMediaServer::CSnapshotMediaServer(const char* a_mediaPath)
: CMediaServer(a_mediaPath)
{
    m_nStartTimestamp = 0;
    m_bParsed = false;
    m_eState = Stopped;
    m_nCurrentFrameIndex = 0;
    m_bServePT = false;
}

CSnapshotMediaServer::~CSnapshotMediaServer()
{
}

int CSnapshotMediaServer::Init()
{
    int rc;
    if (m_bParsed) {
        SetErrorString("Already Initialized");
        return MSRC_OBJECT_ERROR;
    }

    rc = ParseIndexFile();

    if (rc != MSRC_OK)
        return rc;

    if (m_lSnapshotInfo.size() > 0)
        m_nStartTimestamp = m_lSnapshotInfo[m_nCurrentFrameIndex].m_nTimestamp;

    InitThumbnails();

    return MSRC_OK;
}

int CSnapshotMediaServer::Start(uint64_t a_startTimeCode)
{
    if (!m_bParsed) {
        SetErrorString("Not Yet Initialized");
        return MSRC_OBJECT_ERROR;
    }

    if (m_eState == Started) {
        SetErrorString("Service already started");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Started;
    m_nCurrentFrameIndex = 0;

    return MSRC_OK;
}

int CSnapshotMediaServer::Stop()
{
    if (m_eState == Stopped) {
        SetErrorString("Service already stopped");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Stopped;

    return MSRC_OK;
}

uint64_t CSnapshotMediaServer::GetMediaStartTimestamp()
{
    return m_nStartTimestamp;
}

uint64_t CSnapshotMediaServer::GetMediaEndTimestamp()
{
    return m_nStartTimestamp;
}

uint64_t CSnapshotMediaServer::GetTimecodeAtOffset(uint64_t a_nOffset)
{
    return m_nStartTimestamp;
}

uint64_t CSnapshotMediaServer::GetOffsetFromTimecode(uint64_t a_nTimecode)
{
    return 0;
}

uint64_t CSnapshotMediaServer::GetMediaDuration()
{
    return 0;
}

bool CSnapshotMediaServer::IsSnapshot() {
    return true;
}

uint64_t CSnapshotMediaServer::GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset)
{
    uint64_t snapshotSize = 0;
    for (int index = 0; index < (int) m_lSnapshotInfo.size(); index++) {
        string snapshotFile(m_mediaPath);
        snapshotFile.append(FILESEPERATOR);
        snapshotFile.append(m_lSnapshotInfo[index].m_fileName);

        // Find size of file
        struct stat statbuf;
        if (LSTAT(snapshotFile.c_str(), &statbuf) == -1)
            continue;

        snapshotSize += statbuf.st_size;
    }

    return snapshotSize;
}


list <CGap> CSnapshotMediaServer::GetGaps() {
    list <CGap> lGaps;
    return lGaps;
}

int CSnapshotMediaServer::Abort()
{
    return MSRC_OK;
}

int CSnapshotMediaServer::GetFrame(CCFrame &a_cFrame)
{
    if (m_eState != Started) {
        SetErrorString("Service not yet started");
        return MSRC_OBJECT_ERROR;
    }


    if (ServePrimaryThumbnail(a_cFrame))
        return MSRC_OK;

    while (true) {
        if (m_nCurrentFrameIndex >= (int)m_lSnapshotInfo.size()) {
            return MSRC_END_OF_MEDIA;
        }

        string snapshotFile(m_mediaPath);
        snapshotFile.append(FILESEPERATOR);
        snapshotFile.append(m_lSnapshotInfo[m_nCurrentFrameIndex].m_fileName);

        int nBufferSize = ReadImageFile(snapshotFile);
        if (nBufferSize <= 0) {
            m_nCurrentFrameIndex++;
            continue;
        }

        // Fill up the frame
        a_cFrame.SetMessageType(CCFrame::Snapshot);
        a_cFrame.SetTimeStamp(m_lSnapshotInfo[m_nCurrentFrameIndex].m_nTimestamp);
        a_cFrame.SetTimeOffset(0);
        a_cFrame.SetMediaFlag(m_lSnapshotInfo[m_nCurrentFrameIndex].m_nType & IMAGETYPE_MASK);
        a_cFrame.SetLength(nBufferSize);
        a_cFrame.SetData(m_pImageFrameBuffer);

        m_nCurrentFrameIndex++;
        break;
    }
    return MSRC_OK;
}

int CSnapshotMediaServer::ParseIndexFile() {

    string indexfile(m_mediaPath);
    indexfile.append(FILESEPERATOR);
    indexfile.append("SSINDEX");
    m_bParsed = true;

    FILE *fp = NULL;
    
#ifdef WINDWOS
    fp = fopen(indexfile.c_str(), "rb");
#else
    fp = fopen(indexfile.c_str(), "r");
#endif

    if (fp == NULL) {
        SetErrorString("Index file for snapshot not found");
        return MSRC_MEDIA_ERROR;
    }

    char linebuf[128];
    while (fgets(linebuf, 255, fp) != NULL) {
        char tsstr[64];
        char filename[64];

        sscanf(linebuf, "%s %s", tsstr, filename);
        uint64_t timestamp = STRTOULL(tsstr, NULL, 0);

        int imageType = GetImageType(filename);
        if (imageType == IMAGETYPE_UNKNOWN)
            continue;

        CImageFileInfo snapshotInfo(timestamp, imageType, filename);
        m_lSnapshotInfo.push_back(snapshotInfo);
    }

    fclose(fp);

    return MSRC_OK;
}

void CSnapshotMediaServer::InitThumbnails()
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

bool CSnapshotMediaServer::ServePrimaryThumbnail(CCFrame & a_cFrame)
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
    a_cFrame.SetTimeStamp(m_nStartTimestamp);
    a_cFrame.SetTimeOffset(0);
    a_cFrame.SetMediaFlag(mediaFlag);
    a_cFrame.SetLength(nBufferSize);
    a_cFrame.SetData(m_pImageFrameBuffer);

    return true;
}


};
