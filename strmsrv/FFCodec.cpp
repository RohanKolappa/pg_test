/*
 * CFFCodec.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */
#include "strmsrv_compatibility.h"

#include "FFCodec.hh"

#include "FFV2d.hh"
#include "FFSnapshot.hh"
#include "FFRtp.hh"
#include "sframe.hh"
#include "timekeeper.hh"
#include "ThumbnailHandler.h"
#include "RTPMediaHandler.h"
#include "StandardFormatHandler.hh"
#include "SnapshotHandler.hh"
#include "MpegtsMediaHandler.hh"
#include "FFMpegts.hh"

CFFCodec::CFFCodec(std::string a_strStreamType, bool a_bShowProgress,
        bool a_bLiveMode, bool a_bNativeOutput) {
    strcpy(m_pThreadName, "FFCodecThread");
    m_pCodecHandler = NULL;

    m_strStreamType = a_strStreamType;

    m_strMediaDestDir = "";
    m_nDuration = 0;

    m_nElapsed = 0;
    m_nProcessedBytes = 0;
    m_nLastProcessedBytes = 0;
    m_PercentComplete = 0.0;
    m_LastPercentComplete = 0.0;

    m_bError = false;
    m_strErrorString = "";

    m_strPublishError = "";

    m_bShowProgress = a_bShowProgress;
    m_priority = 0;

    m_bLiveMode = a_bLiveMode;
    m_bStopStreamIssued = false;

    m_bNativeOutput = a_bNativeOutput;

    m_nNativeWidth = -1;
    m_nNativeHeight = -1;

    m_nTotalFramesInQueue = 0;
    m_nCurrentFramesInQueue = 0;
    m_nTotalScapsInQueue = 0;
    m_nCurrentScapsInQueue = 0;

    m_nBytesWritten = 0;
    m_nCurrentTNTS = 0;

    CreateCodecHandler();
}

CFFCodec::~CFFCodec() {
    DestroyCodecHandler();
}

CFFCodec* CFFCodec::CreateObj(std::string a_strStreamType, int nId, std::vector<int> & trackSpeeds,
                              bool bShowProgress, bool bCreateTracks, bool bLiveMode, bool bNativeOutput,
                              std::string a_strSDPFile)
{
    if (strcmp(a_strStreamType.c_str(), "V2D") == 0) {
        return new CFFV2d(nId, trackSpeeds, bShowProgress, bLiveMode, bNativeOutput, bCreateTracks);
    }
    else if(strcmp(a_strStreamType.c_str(), "SNAPSHOT") == 0) {
        return new CFFSnapshot(nId, trackSpeeds, bShowProgress, bNativeOutput, bCreateTracks);
    }
    else if (strcmp(a_strStreamType.c_str(), "RTP") == 0) {
        return new CFFRtp(nId, a_strSDPFile, bShowProgress, bLiveMode, bNativeOutput);
    }
    else if (strcmp(a_strStreamType.c_str(), "MPEGTS") == 0) {
        return new CFFMpegts(nId, bShowProgress, bLiveMode, bNativeOutput);
    }

    return NULL;
}

void CFFCodec::SetMediaDestDir(std::string a_strMediaDestDir)
{
    m_strMediaDestDir = a_strMediaDestDir;
}

std::string CFFCodec::GetMediaDestDir()
{
    return m_strMediaDestDir;
}

void CFFCodec::SetMediaSrcDir(std::string a_strMediaSrcDir)
{
    m_strMediaSrcDir = a_strMediaSrcDir;
}

std::string CFFCodec::GetMediaSrcDir()
{
    return m_strMediaSrcDir;
}

void CFFCodec::SetPublishURL(std::string a_strPublishURL)
{
    m_strPublishURL = a_strPublishURL;
}

std::string CFFCodec::GetPublishURL()
{
    return m_strPublishURL;
}

uint64_t CFFCodec::GetThumbnailPublishErrors() {
    return 0; // Base class just returns 0, subclass overides
}

std::string CFFCodec::GetPublishError()
{
    return "";
}

void CFFCodec::SetDuration(timestamp_t nDuration)
{
    m_nDuration = nDuration;
}

void CFFCodec::PrintProgress() {
    if (!m_bShowProgress)
        return;

    if (m_nDuration == 0)
        return;

    // Print progress at least every 5 megs or every additional 10% completion
    m_PercentComplete = ((m_nElapsed * 100.0)/m_nDuration);
    if (((m_PercentComplete - m_LastPercentComplete) >= 10.0) ||
            (m_nProcessedBytes - m_nLastProcessedBytes > 5 * 1024 * 1024)) {
        PrintStats();
        m_LastPercentComplete = m_PercentComplete;
        m_nLastProcessedBytes = m_nProcessedBytes;

    }
}

void CFFCodec::SetErrorString(std::string a_strErrorString)
{
    m_bError = true;
    m_strErrorString = a_strErrorString;
}

std::string CFFCodec::GetErrorString()
{
    return m_strErrorString;
}

int CFFCodec::Run()
{
    return 0;
}

int CFFCodec::GetNativeWidth()
{
    return m_nNativeWidth;
}

int CFFCodec::GetNativeHeight()
{
    return m_nNativeHeight;
}

int CFFCodec::GetTotalFramesInQueue()
{
    return m_nTotalFramesInQueue;
}

int CFFCodec::GetCurrentFramesInQueue()
{
    return m_nCurrentFramesInQueue;
}

int CFFCodec::GetTotalScapsInQueue()
{
    return m_nTotalScapsInQueue;
}

int CFFCodec::GetCurrentScapsInQueue()
{
    return m_nCurrentScapsInQueue;
}

uint64_t CFFCodec::GetCurrentThumbnailTS()
{
    return m_nCurrentTNTS;
}

uint64_t CFFCodec::GetBytesWritten()
{
    return m_nBytesWritten;
}

CThumbnailHandler* CFFCodec::GetThumbnailHandler()
{
    return dynamic_cast<CThumbnailHandler*>(m_pCodecHandler);
}

void CFFCodec::CreateCodecHandler()
{
    if (m_bNativeOutput == true) {
        if (strcmp(m_strStreamType.c_str(), "RTP") == 0) {
            m_pCodecHandler = new CRTPMediaHandler();
        }
        else if (strcmp(m_strStreamType.c_str(), "SNAPSHOT") == 0) {
            m_pCodecHandler = new CSnapshotHandler();
        }
        else if (strcmp(m_strStreamType.c_str(), "MPEGTS") == 0) {
            m_pCodecHandler = new CMpegtsMediaHandler();
        }
    }
    else {
        m_pCodecHandler = new CThumbnailHandler();
    }
}

void CFFCodec::DestroyCodecHandler()
{
    delete m_pCodecHandler;
}

CStandardFormatHandler * CFFCodec::GetStandardMediaHandler()
{
    if (m_pCodecHandler == NULL)
        CreateCodecHandler();

    return dynamic_cast<CStandardFormatHandler*>(m_pCodecHandler);
}

bool CFFCodec::ProceedToProcess()
{
    return m_pCodecHandler->ProceedToProcess();
}

bool CFFCodec::IsThumbnailCreationEnabled()
{
    return m_pCodecHandler->IsThumbnailCreationEnabled();
}

bool CFFCodec::IsNativeOutpuEnabled()
{
    return m_pCodecHandler->IsNativeOutputEnabled();
}
