/*
 * MatroskaMediaServer.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: rkale
 */

#include "MatroskaMediaServer.hh"
#include "MatroskaFileParser.hh"

namespace mediaindex {

CMatroskaMediaServer::CMatroskaMediaServer(const char* a_mediaPath)
: CMediaServer(a_mediaPath)
{
    m_pMatroskaParser = NULL;
}

CMatroskaMediaServer::~CMatroskaMediaServer()
{
    delete m_pMatroskaParser;
}

int CMatroskaMediaServer::Init()
{
    if (m_pMatroskaParser != NULL) {
        SetErrorString("Already Initialized");
        return MSRC_OBJECT_ERROR;
    }

    m_pMatroskaParser = new CMatroskaFileParser(m_mediaPath);

    m_pMatroskaParser->ServeThumbnails(m_bServeThumbnails);

    int rc = m_pMatroskaParser->Parse();
    if (rc != ELIBMC_OK) {
        SetErrorString(m_pMatroskaParser->GetErrorString().c_str());
        return MSRC_MEDIA_ERROR;
    }
    return MSRC_OK;
}

int CMatroskaMediaServer::Start(uint64_t startTimeCode)
{
    if (m_pMatroskaParser == NULL) {
        SetErrorString("Not Yet Initialized");
        return MSRC_OBJECT_ERROR;
    }

    if (m_eState == Started) {
        SetErrorString("Service already started");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Started;

    m_pMatroskaParser->InitializeMediaToTimeOffset(GetOffsetFromTimecode(startTimeCode));

    return 0;
}

int CMatroskaMediaServer::Stop()
{
    if (m_eState == Stopped) {
        SetErrorString("Service already stopped");
        return MSRC_OBJECT_ERROR;
    }

    m_eState = Stopped;

    return 0;
}

int CMatroskaMediaServer::Abort()
{
    return 0;
}

bool CMatroskaMediaServer::IsSnapshot() {
    return m_pMatroskaParser->IsSnapshot();
}

int CMatroskaMediaServer::GetFrame(CCFrame &a_cFrame)
{
    if (m_eState != Started) {
        SetErrorString("Service not yet started");
        return MSRC_OBJECT_ERROR;
    }

    bool retVal = m_pMatroskaParser->GetFrame(a_cFrame);
    if (!retVal) {
        SetErrorString("End of Media Reached");
        return MSRC_END_OF_MEDIA;
    }
    return MSRC_OK;
}


uint64_t CMatroskaMediaServer::GetMediaStartTimestamp()
{
    return m_pMatroskaParser->GetMediaBeginTimestamp();
}

uint64_t CMatroskaMediaServer::GetMediaEndTimestamp()
{
    return m_pMatroskaParser->GetMediaEndTimestamp();
}

uint64_t CMatroskaMediaServer::GetTimecodeAtOffset(uint64_t a_nOffset)
{
    return m_pMatroskaParser->GetTimecodeAtOffset(a_nOffset);
}

uint64_t CMatroskaMediaServer::GetOffsetFromTimecode(uint64_t a_nTimecode)
{
    return m_pMatroskaParser->GetOffsetFromTimecode(a_nTimecode);
}

uint64_t CMatroskaMediaServer::GetMediaDuration()
{
    return m_pMatroskaParser->GetMediaDuration();
}

uint64_t CMatroskaMediaServer::GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset)
{
    return 0;
}

std::list <CGap> CMatroskaMediaServer::GetGaps() {
    return m_pMatroskaParser->GetGaps();
}

};
