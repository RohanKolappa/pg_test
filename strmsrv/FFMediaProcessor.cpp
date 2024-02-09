/*
 * FFMediaProcessor.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include <sstream>

#include "FFMediaProcessor.hh"
#include "FFTrackWriter.hh"

#include "FFUtils.hh"

#include "FFCodec.hh"
#include "StandardFormatHandler.hh"

using namespace mediaindex;
using namespace std;

CFFMediaProcessor::CFFMediaProcessor(
        std::string mediaDir,
        std::string mediaDestDir,
        std::vector<int> trackSpeeds,
        std::string strStreamType,
        int nId,
        bool bLiveMode,
        bool bShowProgress,
        std::string strSDPFile)
{
    m_nId = nId;
    m_mediaDir = mediaDir;
    m_mediaDestDir = mediaDestDir;
    m_bLiveMode = bLiveMode;
    m_bShowProgress = bShowProgress;
    m_pMediaServer = NULL;
    m_bError = false;
    m_pFFCodec = NULL;
    m_nTrimLength = 0;
    m_bSyncWrites = false;
    m_bAppendMode = false;

    m_bCreateFFTracks = true;

    m_bEnableLiveTN = false;
    m_bEnableSnapshot = false;
    m_bEnableLargeTN = false;
    m_bEnableSmallTN = false;
    m_nLiveTNInterval = DEFAULT_TIME_FOR_PRIMARY_TN;
    m_bEnablePrimaryTN = false;
    m_nPrimaryTNWidth = DEFAULT_PRIMARY_TN_WIDTH;
    m_nPrimaryTNHeight = DEFAULT_PRIMARY_TN_HEIGHT;
    m_nPrimaryTNOffset = DEFAULT_TIME_FOR_PRIMARY_TN;
    m_nStreamTNWidth = DEFAULT_STREAM_TN_WIDTH;
    m_nStreamTNHeight = DEFAULT_STREAM_TN_HEIGHT;
    m_nThumbnailInterval = DEFAULT_TIME_INTERVAL_FOR_STREAM_TN;
    m_bEnableStreamTN = false;

    m_strStreamType = strStreamType;

    m_strSDPFile = strSDPFile;

    m_bNativeOutput = false;
    m_strNativeOutputFile = "";

    m_trackSpeeds = trackSpeeds;

    m_nNumbTracks = m_trackSpeeds.size();
    m_bThumbnailInitializationNeeded = true;

    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        m_qFFQueueSource[trackNumb] = NULL;
        m_pTrackWriters[trackNumb] = NULL;
    }
}

CFFMediaProcessor::~CFFMediaProcessor()
{
    if (m_pFFCodec != NULL)
        m_pFFCodec->StopStream();

    DeleteQueues();
    DeleteTrackWriters();

    delete m_pMediaServer;
    delete m_pFFCodec;
}

void CFFMediaProcessor::SetTrimLength(uint64_t trimLength)
{
    m_nTrimLength = trimLength;
}

void CFFMediaProcessor::SetSyncWrites(bool syncWrites)
{
    m_bSyncWrites = syncWrites;
}

void CFFMediaProcessor::SetAppendMode(bool appendMode)
{
    m_bAppendMode = appendMode;
}

void CFFMediaProcessor::SetDropBuffers(bool dropBuffers)
{
    // Can be set by client code when there is need to gracefully
    // backoff under disk I/O pressure
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (m_pTrackWriters[trackNumb] != NULL)
            m_pTrackWriters[trackNumb]->SetDropBuffers(dropBuffers);
    }

}

bool CFFMediaProcessor::InitMedia()
{
    if (!VerifyDestinationDirectory())
        return false;

    if (!CreateFFProcessor())
        return false;

    CreateTrackWriters();

    SetThumbnailProperties();
    SetStandardMediaProperties();

    return true;
}

bool CFFMediaProcessor::ProcessMedia()
{
    if (!CreateMediaServer())
        return false;

    if (!InitMedia())
        return false;

    // Read frames from input file and process them
    while (true) {

        CCFrame cframe;
        if (!GetMediaServerFrame(cframe))
            break;

        if (!ProcessMediaServerFrame(cframe))
            break;

        ProcessTrackWriters();

        if (m_bError)
            break;
    }

    if (m_pFFCodec != NULL)
        m_pFFCodec->StopStream();

    m_pMediaServer->Stop();

    if (m_bError)
        return false;
    return true;
}

bool CFFMediaProcessor::CreateFFProcessor() {

    CreateQueues();

    return true;
}

bool CFFMediaProcessor::CreateFFCodec() {

    m_pFFCodec = CFFCodec::CreateObj(m_strStreamType, m_nId, m_trackSpeeds, m_bShowProgress,
                                     m_bCreateFFTracks, m_bLiveMode, m_bNativeOutput, m_strSDPFile);
    if (m_pFFCodec == NULL)
        return false;

    if (m_pMediaServer != NULL)
        m_pFFCodec->SetDuration(m_pMediaServer->GetMediaDuration());

    return true;
}

void CFFMediaProcessor::CreateQueues()
{
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        ostringstream qid;
        qid << "FFQueue_" << m_nId << "_" << m_trackSpeeds[trackNumb];
        m_qFFQueueSource[trackNumb] = CSQueueManager::Instance()->GetQueueSource(qid.str());
    }
}

void CFFMediaProcessor::DeleteQueues() {
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (m_qFFQueueSource[trackNumb] != NULL)
            CSQueueManager::Instance()->ReleaseQueueSource(m_qFFQueueSource[trackNumb]);
    }
}


bool CFFMediaProcessor::ProcessMediaServerFrame(CCFrame & cframe)
{
    if (m_pFFCodec == NULL) {
        return false;
    }

    // Send the frame for FF Processing
    CSFrame * sFrame = NULL;
    if (cframe.GetMessageType() == CCFrame::Normal) {
        sFrame = new CSFrame((const char *)cframe.GetData(),
                cframe.GetLength(),
                cframe.GetMediaFlag(),
                (timestamp_t) cframe.GetTimeStamp());
        m_pFFCodec->ProcessFrame(sFrame, cframe.GetTimeOffset());
    }
    else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
        sFrame = new CSFrame(cframe.GetTimeStamp(), CSFrame::StartOfStream);
        m_pFFCodec->ProcessFrame(sFrame, cframe.GetTimeOffset());
    }
    else if (cframe.GetMessageType() == CCFrame::MediaStopped) {
        sFrame = new CSFrame(cframe.GetTimeStamp(), CSFrame::EndOfStream);
        m_pFFCodec->ProcessFrame(sFrame, cframe.GetTimeOffset());
    }
    else if (cframe.GetMessageType() == CCFrame::Snapshot){
        sFrame = new CSFrame((const char *)cframe.GetData(),
                cframe.GetLength(),
                cframe.GetMediaFlag(),
                (timestamp_t) cframe.GetTimeStamp());
        m_pFFCodec->ProcessFrame(sFrame, cframe.GetTimeOffset());
    }
    delete sFrame;

    return true;

}

void CFFMediaProcessor::ProcessTrackWriters() {
    if (m_bCreateFFTracks == false) {
        return;
    }

    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (!m_pTrackWriters[trackNumb]->HandleFrames()) {
            SetErrorString(m_pTrackWriters[trackNumb]->GetErrorString());
            break;
        }
    }
}

bool CFFMediaProcessor::Start(timestamp_t nStartTimestamp) {
    SetupThumbnailParamsIfNeeded();

    if (m_bCreateFFTracks == true) {
        ProcessTrackWriters();
    }

    if (m_pFFCodec != NULL) {
        CSFrame * sFrame = new CSFrame(nStartTimestamp, CSFrame::StartOfStream);
        m_pFFCodec->ProcessFrame(sFrame);

        delete sFrame;
    }
    return (!m_bError);
}

bool CFFMediaProcessor::Stop(timestamp_t nStopTimestamp) {
    SetupThumbnailParamsIfNeeded();

    if (m_bCreateFFTracks == true) {
        ProcessTrackWriters();
    }

    if (m_pFFCodec != NULL) {
        CSFrame * sFrame = new CSFrame(nStopTimestamp, CSFrame::EndOfStream);
        m_pFFCodec->ProcessFrame(sFrame);

        delete sFrame;
    }
    return (!m_bError);
}

bool CFFMediaProcessor::PutFrame(CSFrame * sFrame)  {
    SetupThumbnailParamsIfNeeded();

    if (m_bCreateFFTracks == true) {
        ProcessTrackWriters();
    }

     if (m_pFFCodec != NULL) {
         m_pFFCodec->ProcessFrame(sFrame);
    }

    return (!m_bError);
}

bool CFFMediaProcessor::GetMediaServerFrame(CCFrame & cframe) {
    int rc;
    if ((rc = m_pMediaServer->GetFrame(cframe)) != MSRC_OK) {
        if (rc != MSRC_END_OF_MEDIA) {
            printf("MediaServer Get Frame Failed: %s\n",
                    m_pMediaServer->GetErrorString());
        }
        return false;
    }
    return true;
}

bool CFFMediaProcessor::CreateMediaServer() {
    m_pMediaServer = CMediaServer::Create(m_mediaDir.c_str());
    if (m_pMediaServer == NULL) {
        SetErrorString("Failed to create MediaServer object");
        return false;
    }

    if (m_pMediaServer->Init() != MSRC_OK) {
        SetErrorString("MediaServer Init failed for " + m_mediaDir);
        return false;
    }

    if (m_pMediaServer->Start(0) != MSRC_OK) {
        SetErrorString("MediaServer Start failed for " + m_mediaDir);
        return false;
    }
    return true;
}


bool CFFMediaProcessor::VerifyDestinationDirectory() {
    if (!CFFUtils::IsDirectory(m_mediaDestDir)) {
        SetErrorString("Not a directory: " + m_mediaDestDir);
        return false;
    }
    return true;
}


bool CFFMediaProcessor::CreateTrackWriters()
{
    if (m_bCreateFFTracks == false) {
        return false;
    }

    // Create media writers
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        ostringstream trackDirStream;
        trackDirStream << m_mediaDestDir << "/ff_" << m_trackSpeeds[trackNumb];
        string trackDir = trackDirStream.str();

        m_pTrackWriters[trackNumb] = new CFFTrackWriter(m_qFFQueueSource[trackNumb],
                trackDir, m_nTrimLength, m_bSyncWrites, m_bAppendMode);

        if (!m_pTrackWriters[trackNumb]->Init()) {
            SetErrorString(m_pTrackWriters[trackNumb]->GetErrorString());
            return false;
        }
    }
    return true;
}

void CFFMediaProcessor::DeleteTrackWriters() {
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        delete m_pTrackWriters[trackNumb];
    }
}

uint64_t CFFMediaProcessor::GetBytesWritten()
{
    uint64_t bytesWriiten = 0;
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (m_pTrackWriters[trackNumb] == NULL)
            break;
        bytesWriiten += m_pTrackWriters[trackNumb]->GetBytesWritten();
    }
    return bytesWriiten;
}

uint64_t CFFMediaProcessor::GetBytesDeleted()
{
    uint64_t bytesDeleted = 0;
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (m_pTrackWriters[trackNumb] == NULL)
            break;
        bytesDeleted += m_pTrackWriters[trackNumb]->GetBytesDeleted();
    }
    return bytesDeleted;
}

uint64_t CFFMediaProcessor::GetBuffersDropped()
{
    uint64_t buffersDropped = 0;
    for (int trackNumb = 0; trackNumb < m_nNumbTracks; trackNumb++) {
        if (m_qFFQueueSource[trackNumb] != NULL) {
            m_qFFQueueSource[trackNumb]->DropCount();
        }
    }
    return buffersDropped;
}

void CFFMediaProcessor::SetErrorString(string errorString)
{
    m_bError = true;
    m_errorString = errorString;
}

string CFFMediaProcessor::GetErrorString()
{
    return m_errorString;
}

void CFFMediaProcessor::EnableFFTracks(bool a_bEnableFFTracts)
{
    m_bCreateFFTracks = a_bEnableFFTracts;
}

bool CFFMediaProcessor::SetStandardMediaProperties()
{
    if (m_bNativeOutput == true) {
        CStandardFormatHandler *pStandardMediaHandler = dynamic_cast<CStandardFormatHandler*>
                (m_pFFCodec->GetStandardMediaHandler());
        if (pStandardMediaHandler == NULL)
            return false;

        if (m_strNativeOutputFile.empty() == true)
            return false;

        pStandardMediaHandler->SetOutputFileName(m_strNativeOutputFile);
        pStandardMediaHandler->SetNativeOutput();
    }

    return true;
}

bool CFFMediaProcessor::SetThumbnailProperties()
{
    m_bThumbnailInitializationNeeded = false;

    if (m_pFFCodec == NULL) {
    	CreateFFCodec();
    }

    if (m_pFFCodec == NULL)
        return false;

    CThumbnailHandler *pThumbnailHandler = m_pFFCodec->GetThumbnailHandler();
    if (pThumbnailHandler == NULL)
        return false;

    if (m_bEnablePrimaryTN == true) {
        pThumbnailHandler->EnablePrimaryTN(m_bEnablePrimaryTN);

        pThumbnailHandler->SetPrimaryTNResolution(
                m_nPrimaryTNWidth, m_nPrimaryTNHeight);

        pThumbnailHandler->SetPrimaryTNOffset(
                m_nPrimaryTNOffset);
    }

    if (m_bEnableStreamTN == true) {
        pThumbnailHandler->EnableStreamTN(m_bEnableStreamTN);
        pThumbnailHandler->SetStreamTNFrequency(
                m_nThumbnailInterval);

        pThumbnailHandler->SetStreamTNResolution(
                m_nStreamTNWidth, m_nStreamTNHeight);
    }

    if (m_bEnableLiveTN == true) {
        pThumbnailHandler->EnableLiveTN(m_bEnableLiveTN,
                                        m_bEnableSnapshot,
                                        m_bEnableLargeTN,
                                        m_bEnableSmallTN);
        pThumbnailHandler->SetPrimaryTNResolution(
                m_nPrimaryTNWidth, m_nPrimaryTNHeight);
        pThumbnailHandler->SetStreamTNResolution(
                m_nStreamTNWidth, m_nStreamTNHeight);
        if (m_nThumbnailInterval != 0) {
            pThumbnailHandler->SetStreamTNFrequency(
                    m_nThumbnailInterval);
        }
    }

    m_pFFCodec->SetMediaDestDir(m_mediaDestDir);

    // TODO: this should finally move to a call on thumbnail handler
    m_pFFCodec->SetPublishURL(m_strPublishURL);

    return true;
}

void CFFMediaProcessor::EnableLiveTN(bool a_bEnableTNs,
                                     bool a_bEnableSnapshot,
                                     bool a_bEnableLargeTN,
                                     bool a_bEnableSmallTN)
{
    m_bEnableLiveTN = a_bEnableTNs;
    m_bEnableSnapshot = a_bEnableSnapshot;
    m_bEnableLargeTN = a_bEnableLargeTN;
    m_bEnableSmallTN = a_bEnableSmallTN;
    m_bThumbnailInitializationNeeded = true;
}


void CFFMediaProcessor::SetupThumbnailParamsIfNeeded()
{
    if (m_bThumbnailInitializationNeeded)
        SetThumbnailProperties();
}

bool CFFMediaProcessor::IsLiveTNEnabled()
{
    if (m_pFFCodec != NULL)
    	return m_pFFCodec->GetThumbnailHandler()->IsLiveThumbnailEnabled();

    return m_bEnableLiveTN;
}

bool CFFMediaProcessor::IsSnapshotEnabled()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsSnapshotEnabled();

    return false;
}

bool CFFMediaProcessor::IsLargeTNEnabled()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsLargeTNEnabled();

    return false;
}

bool CFFMediaProcessor::IsSmallTNEnabled()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsSmallTNEnabled();

    return false;
}

void CFFMediaProcessor::EnablePrimaryTN(bool a_bEnable)
{
    m_bEnablePrimaryTN = a_bEnable;
    m_bThumbnailInitializationNeeded = true;
}

bool CFFMediaProcessor::IsPrimaryTNEnabled()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsPrimaryTNEnabled();

    return false;
}

bool CFFMediaProcessor::IsPrimaryTNCreated()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsPrimaryTNCreated();

    return false;
}

void CFFMediaProcessor::SetPrimaryTNResolution(int a_nWidth, int a_nHeight)
{
    m_nPrimaryTNWidth = a_nWidth;
    m_nPrimaryTNHeight = a_nHeight;
    m_bThumbnailInitializationNeeded = true;
}

int CFFMediaProcessor::GetPrimaryTNWidth()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetPrimaryTNWidth();

    return 0;
}

int CFFMediaProcessor::GetPrimaryTNHeight()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetPrimaryTNHeight();

    return 0;
}

void CFFMediaProcessor::SetStreamTNResolution(int a_nWidth, int a_nHeight)
{
    m_nStreamTNWidth = a_nWidth;
    m_nStreamTNHeight = a_nHeight;
    m_bThumbnailInitializationNeeded = true;
}

int CFFMediaProcessor::GetStreamTNWidth()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetStreamTNWidth();

    return 0;
}

int CFFMediaProcessor::GetStreamTNHeight()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetStreamTNHeight();

    return 0;
}

void CFFMediaProcessor::SetStreamTNTimeInterval(uint64_t a_nInterval)
{
    m_nThumbnailInterval = a_nInterval;
    m_bThumbnailInitializationNeeded = true;
}

uint64_t CFFMediaProcessor::GetStreamTNTimeInterval()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetStreamTNFrequency();

    return 0;
}

void CFFMediaProcessor::SetPrimaryTNOffset(uint64_t a_nOffset)
{
    m_nPrimaryTNOffset = a_nOffset;
    m_bThumbnailInitializationNeeded = true;
}

uint64_t CFFMediaProcessor::GetPrimaryTNOffset()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetPrimaryTNOffset();

    return 0;
}

void CFFMediaProcessor::EnableStreamTN(bool a_bEnable)
{
    m_bEnableStreamTN = a_bEnable;
    m_bThumbnailInitializationNeeded = true;
}

bool CFFMediaProcessor::IsStreamTNEnabled()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->IsStreamTNEnabled();

    return false;
}

void CFFMediaProcessor::SetStreamType(std::string a_strStreamType)
{
    m_strStreamType = a_strStreamType;
}

std::string CFFMediaProcessor::GetStreamType()
{
    return m_strStreamType;
}

std::string CFFMediaProcessor::GetMediaDestDir()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetMediaDestDir();

    return "";
}

void CFFMediaProcessor::SetPublishURL(std::string a_strPublishURL)
{
    m_strPublishURL = a_strPublishURL;
    m_bThumbnailInitializationNeeded = true;
}

std::string CFFMediaProcessor::GetPublishURL()
{
    return m_strPublishURL;
}


uint64_t CFFMediaProcessor::GetPublishErrors() {
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailPublishErrors();
    return 0;
}

std::string CFFMediaProcessor::GetPublishError()
{
	if (m_pFFCodec != NULL)
	    return m_pFFCodec->GetPublishError();

    return "";
}

int CFFMediaProcessor::GetNativeWidth()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetNativeWidth();

    return -1;
}

int CFFMediaProcessor::GetNativeHeight()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetNativeHeight();

    return -1;
}

int CFFMediaProcessor::GetTotalThumbnailCount()
{
    if(m_pFFCodec != NULL)
        return m_pFFCodec->GetThumbnailHandler()->GetTotalThumbnailCount();

    return 0;
}

int CFFMediaProcessor::GetTotalFramesInQueue()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetTotalFramesInQueue();

    return 0;
}

int CFFMediaProcessor::GetCurrentFramesInQueue()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetCurrentFramesInQueue();

    return 0;
}

int CFFMediaProcessor::GetTotalScapsInQueue()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetTotalScapsInQueue();

    return 0;
}

int CFFMediaProcessor::GetCurrentScapsInQueue()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetCurrentScapsInQueue();

    return 0;
}

uint64_t CFFMediaProcessor::GetCurrentTNTimestamp()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetCurrentThumbnailTS();

    return 0;
}

uint64_t CFFMediaProcessor::GetBytesWrittenForTN()
{
    if (m_pFFCodec != NULL)
        return m_pFFCodec->GetBytesWritten();

    return 0;
}

void CFFMediaProcessor::SetNativeOutputMode()
{
    m_bNativeOutput = true;
}

void CFFMediaProcessor::SetNativeOutputFileName(std::string a_strFileName)
{
    m_strNativeOutputFile = a_strFileName;
}

void CFFMediaProcessor::StartMedia(uint64_t a_nStartTime)
{
    if (m_pFFCodec == NULL)
        return;

    CSFrame *sFrame = new CSFrame(a_nStartTime, CSFrame::StartOfStream);
    m_pFFCodec->ProcessFrame(sFrame, a_nStartTime);
    delete sFrame;
}

void CFFMediaProcessor::StopMedia(uint64_t a_nStopTime)
{
    if (m_pFFCodec == NULL)
   	    return;

    CSFrame *sFrame = new CSFrame(a_nStopTime, CSFrame::EndOfStream);
    m_pFFCodec->ProcessFrame(sFrame, a_nStopTime);
    delete sFrame;
}

void CFFMediaProcessor::ProcessMediaFrame(mediaindex::CCFrame & a_rFrame)
{
    if (m_pFFCodec == NULL)
        return;

    CSFrame *sFrame = new CSFrame((const char *)a_rFrame.GetData(),
                         a_rFrame.GetLength(),
                         a_rFrame.GetMediaFlag(),
                         (timestamp_t) a_rFrame.GetTimeStamp());
    m_pFFCodec->ProcessFrame(sFrame, a_rFrame.GetTimeOffset());

    delete sFrame;
}
