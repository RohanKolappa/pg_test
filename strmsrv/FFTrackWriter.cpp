/*
 * FFTrackWriter.cpp
 *
 *  Created on: Jul 30, 2012
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "FFTrackWriter.hh"
#include "FFUtils.hh"

using namespace mediaindex;
using namespace std;


CFFTrackWriter::CFFTrackWriter(CSQueueSource * qFFQueueSource,
        std::string & trackDir,
        uint64_t trimLength,
        bool bSyncWrites,
        bool bAppendMode)
{
    m_qFFQueueSource = qFFQueueSource;
    m_trackDirectory = trackDir;
    m_nTrimLength = trimLength;
    m_bError = false;
    m_errorString = "";
    m_bSyncWrites = bSyncWrites;
    m_bAppendMode = bAppendMode;

    m_pMediaWriter = new CMediaWriter(trackDir.c_str());
    m_pMediaWriter->SetAppendMode(m_bAppendMode);
}

CFFTrackWriter::~CFFTrackWriter()
{
    delete m_pMediaWriter;
}

bool CFFTrackWriter::Init()
{
    if (!DeleteTrackDirectory())
        return false;

    if (!CreateTrackDirectory())
        return false;

    if (m_pMediaWriter->Init(m_nTrimLength, m_bSyncWrites) != MSRC_OK) {
        string errorString = "MediaWriter Init Failed: %" +
                string(m_pMediaWriter->GetErrorString());
        SetErrorString(errorString);
        return false;
    }
    return true;
}

bool CFFTrackWriter::HandleFrames() {

    while (true) {
        CSFrame * sFrame = m_qFFQueueSource->GetFrame();
        if (sFrame == NULL)
            break;

        if (sFrame->GetMessageType() == CSFrame::Normal) {
            WriteFrame(sFrame);
        }
        else if (sFrame->GetMessageType() == CSFrame::StartOfStream) {
            StartWriter(sFrame->GetTimeStamp());
        }
        else if (sFrame->GetMessageType() == CSFrame::EndOfStream) {
            StopWriter(sFrame->GetTimeStamp());
        }

        m_qFFQueueSource->ReleaseFrame(sFrame);
    }
    if (m_bError)
        return false;
    return true;
}

void CFFTrackWriter::SetDropBuffers(bool dropBuffers)
{
    if (m_pMediaWriter != NULL)
        m_pMediaWriter->SetDropBuffers(dropBuffers);
}


void CFFTrackWriter::WriteFrame(CSFrame *sFrame)
{
    if (m_bError)
        return;
    CCFrame cframe((uint8_t *)sFrame->GetData(),
            sFrame->GetLength(),
            sFrame->GetMediaFlag(),
            sFrame->GetTimeStamp());

    int rc = m_pMediaWriter->PutFrame(cframe);
    if (rc != MSRC_OK) {
        string errorString = "MediaWriter Error: " + string(m_pMediaWriter->GetErrorString());
        SetErrorString(errorString);
    }
}

void CFFTrackWriter::StartWriter(timestamp_t nTimeStamp)
{
    if (m_bError)
        return;
    if (m_pMediaWriter->Start(nTimeStamp) != MSRC_OK) {
        string errorString = "MediaWriter Start Failed: %" +
                string(m_pMediaWriter->GetErrorString());
        SetErrorString(errorString);
    }
}

void CFFTrackWriter::StopWriter(timestamp_t nTimeStamp)
{
    if (m_bError)
        return;
    if (m_pMediaWriter->Stop(nTimeStamp) != MSRC_OK) {
        string errorString = "MediaWriter Stop Failed: %" +
                string(m_pMediaWriter->GetErrorString());
        SetErrorString(errorString);
    }
}

bool CFFTrackWriter::CreateTrackDirectory() {
    if (!CFFUtils::CreateDirectory(m_trackDirectory)) {
        if (!m_bAppendMode) {
            SetErrorString("Could not create directory: " + m_trackDirectory);
            return false;
        }
    }
    return true;
}

bool CFFTrackWriter::DeleteTrackDirectory() {

    if (m_bAppendMode)
        return true;

    if (!CFFUtils::IsDirectory(m_trackDirectory))
        return true;

    if (!CFFUtils::DeleteDirectoryTree(m_trackDirectory.c_str())) {
        SetErrorString("Could not delete directory: " + m_trackDirectory);
        return false;
    }

    return true;
}

void CFFTrackWriter::SetErrorString(string errorString)
{
    m_bError = true;
    m_errorString = errorString;
}

uint64_t CFFTrackWriter::GetBytesWritten()
{
    return m_pMediaWriter->GetBytesWritten();
}

uint64_t CFFTrackWriter::GetBytesDeleted()
{
    return m_pMediaWriter->GetBytesDeleted();
}

string CFFTrackWriter::GetErrorString()
{
    return m_errorString;
}
