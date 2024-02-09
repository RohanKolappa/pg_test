/*
 * CImageWriter.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: durga
 */

#include "strmsrv_compatibility.h"
#include <sstream>

#include "common.hh"
#include "squeue.hh"
#include "imagewriter.hh"
#include "FFMediaProcessor.hh"

#include "GenerateUniqueID.h"
#include "v2o_lib.h"

using namespace std;

CPortUse CImageWriter::m_portsUsed;


CImageWriter::CImageWriter(int a_nId,
        int a_nIOPort,
        char *a_pMediaDir,
        char *a_pStreamType,
        bool a_bEnableTNs,
        bool a_bEnableSnapshot,
        bool a_bEnableLargeTN,
        bool a_bEnableSmallTN,
        int a_nSmallTNWidth, int a_nSmallTNHeight,
        int a_nLargeTNWidth, int a_nLargeTNHeight,
        int a_nThumbnailInterval,
        char * a_pSDPFilePath,
        char * a_publishURL)
    : CStreamer(a_nId) {

    m_nPortType = IMAGEWRITERPORT;

    m_bError = false;

    m_pFFMediaProcessor = NULL;

    m_IsStartNeeded = true;

    m_pMediaDir = new char[MAXFILENAMELEN+1];
    m_pStreamType = new char[MAXFILENAMELEN+1];
    m_pSDPFilePath = new char[MAXFILENAMELEN+1];

    memset(m_pMediaDir, 0, MAXFILENAMELEN);
    memset(m_pStreamType, 0, MAXFILENAMELEN);
    memset(m_pSDPFilePath, 0, MAXFILENAMELEN);

    sprintf(m_pMediaDir, "%s", a_pMediaDir);
    sprintf(m_pStreamType, "%s", a_pStreamType);
    sprintf(m_pSDPFilePath, "%s", a_pSDPFilePath);

    m_bEnableTNs = a_bEnableTNs;
    m_bEnableNative = a_bEnableSnapshot;
    m_bEnableLargeTN = a_bEnableLargeTN;
    m_bEnableSmallTN = a_bEnableSmallTN;
    m_nSmallTN_Width = a_nSmallTNWidth;
    m_nSmallTN_Height = a_nSmallTNHeight;
    m_nLargeTN_Width = a_nLargeTNWidth;
    m_nLargeTN_Height = a_nLargeTNHeight;

    m_nThumbnailInterval = a_nThumbnailInterval;
    m_strPublishURL = a_publishURL;

    m_nIOPort = a_nIOPort;

    RetrieveThumbnailConfiguration();
    SetMediaProcessorParams();

}

CImageWriter::~CImageWriter() {
    delete [] m_pMediaDir;
    delete [] m_pStreamType;
    delete [] m_pSDPFilePath;

    delete m_pFFMediaProcessor;

    if (m_nIOPort > 0)
        m_portsUsed.ReleasePort(m_nIOPort);

    if (m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
}

void CImageWriter::RetrieveThumbnailConfiguration()
{
#ifndef WINDOWS
    if (m_nIOPort <= 0)
        return;

    if (!m_portsUsed.ReservePort(m_nIOPort))
        return;

    struct tx_thumbnail_config thumbnail_cfg;
    get_tx_thumbnail_config(m_nIOPort - 1, &thumbnail_cfg);

    m_bEnableTNs = thumbnail_cfg.enable;
    m_bEnableNative = thumbnail_cfg.enable_native;
    m_bEnableLargeTN = thumbnail_cfg.enable_large_thumbnail;
    m_bEnableSmallTN = thumbnail_cfg.enable_small_thumbnail;
    m_nLargeTN_Width = thumbnail_cfg.large_thumbnail_width;
    m_nLargeTN_Height = thumbnail_cfg.large_thumbnail_height;
    m_nSmallTN_Width = thumbnail_cfg.small_thumbnail_width;
    m_nSmallTN_Height = thumbnail_cfg.small_thumbnail_height;
    m_nThumbnailInterval = thumbnail_cfg.thumbnail_interval;
#endif
}

void
CImageWriter::CreateMediaProcessor()
{
    if (m_pFFMediaProcessor != NULL)
        return;

    std::vector<int> trackSpeeds;
    trackSpeeds.push_back(2);
    trackSpeeds.push_back(10);
    trackSpeeds.push_back(60);
    trackSpeeds.push_back(256);

    m_pFFMediaProcessor = new CFFMediaProcessor("", m_pMediaDir, trackSpeeds,
            m_pStreamType, CGenerateUniqueID::GetID(),
            true, false, m_pSDPFilePath);

    m_pFFMediaProcessor->EnableFFTracks(false);

    m_IsStartNeeded = true;
}

void
CImageWriter::DeleteMediaProcessor()
{
    if (m_pFFMediaProcessor == NULL)
        return;

    timestamp_t nStopTimestamp = GetCurrentTimeStamp();
    if (!m_pFFMediaProcessor->Stop(nStopTimestamp)) {
        m_bError = true;
    }

    delete m_pFFMediaProcessor;
    m_pFFMediaProcessor = NULL;
}

int
CImageWriter::StartStream(int nQueueId)
{
    ASSERT(m_qQueueSource == NULL);
    ostringstream qid;
    qid << "Queue_" << nQueueId;
    m_qQueueSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qQueueSource->SetOwner(m_nId);

    // Increase the maximum size of the source queue
    m_qQueueSource->SetMaxSize(50000);
    m_qQueueSource->SetMaxBytes(20 * 1024 * 1024);

    return 0;
};

/// Remove the Queue Assignment
int
CImageWriter::StopStream()
{
    if (m_pFFMediaProcessor != NULL) {
        timestamp_t nStopTimestamp = GetCurrentTimeStamp();
        if (!m_pFFMediaProcessor->Stop(nStopTimestamp)) {
            m_bError = true;
        }
    }

    if (m_qQueueSource != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
        m_qQueueSource = NULL;
    }

    return 0;
}

void
CImageWriter::DeleteStream()
{
    // If we have not been "Stopped" yet do it now
    if (IsStarted()) {
        SYSLOG_DEBUG("Writer still running, Calling stop internally.");
        Stop();
    }

    // Remove our own (streamer) reference also.
    RemoveReference();
}

int CImageWriter::ProcessStream() {
    // Quit if no queue defined yet (writer not "started")
    if (m_qQueueSource == NULL)
        return 0;

    CSFrame * sFrame = m_qQueueSource->GetFrame();

    if (sFrame) {
        if (m_IsStartNeeded == true) {
            if (m_pFFMediaProcessor != NULL) {
                if (!m_pFFMediaProcessor->Start(sFrame->GetTimeStamp())) {
                    m_bError = true;
                }
                m_IsStartNeeded = false;
            }
        }

        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        if (sFrame->GetMessageType() == CSFrame::Normal) {
            if (!m_bError && m_pFFMediaProcessor != NULL) {
                if (!m_pFFMediaProcessor->PutFrame(sFrame)) {
                    m_bError = true;
                    SYSLOG_ERROR("PUT FRAME FAILED. Error = %s\n",
                                  m_pFFMediaProcessor->GetErrorString().c_str());
                }
            }
            m_frameCount++;
            m_bytesTransfered += sFrame->GetLength();
        }
        m_qQueueSource->ReleaseFrame(sFrame);

        return 1;
    }

    return 0;
}

void
CImageWriter::SetMediaProcessorParams()
{
    bool bCreatedNewMediaProcessor = false;

    if (m_pFFMediaProcessor == NULL) {
        CreateMediaProcessor();
        bCreatedNewMediaProcessor = true;
    }

//    if (m_bEnableTNs == false) {
//        DeleteMediaProcessor();
//    }
//    else {
//        if (m_pFFMediaProcessor == NULL) {
//            CreateMediaProcessor();
//            bCreatedNewMediaProcessor = true;
//        }
//    }

    if (m_pFFMediaProcessor == NULL)
        return;

    if (m_bEnableTNs == false) {
        m_pFFMediaProcessor->EnableLiveTN(true,
                                          false,
                                          false,
                                          false);
    }
    else {
        m_pFFMediaProcessor->EnableLiveTN(true,
                                          m_bEnableNative,
                                          m_bEnableLargeTN,
                                          m_bEnableSmallTN);
    }
    m_pFFMediaProcessor->EnableFFTracks(false);
    m_pFFMediaProcessor->EnablePrimaryTN(false);
    m_pFFMediaProcessor->EnableStreamTN(false);

    if (m_nLargeTN_Width != 0 && m_nLargeTN_Height != 0) {
        m_pFFMediaProcessor->SetPrimaryTNResolution(m_nLargeTN_Width, m_nLargeTN_Height);
    }

    if (m_nSmallTN_Width != 0 && m_nSmallTN_Height != 0) {
        m_pFFMediaProcessor->SetStreamTNResolution(m_nSmallTN_Width, m_nSmallTN_Height);
    }

    if (m_nThumbnailInterval > 0) {
        // Input StreamThumbnail frequency is in Seconds. So, converting to milli-seconds.
        m_pFFMediaProcessor->SetStreamTNTimeInterval(m_nThumbnailInterval * 1000);
    }

    m_pFFMediaProcessor->SetPublishURL(m_strPublishURL);

    if (bCreatedNewMediaProcessor) {
        if (!m_pFFMediaProcessor->InitMedia()) {
            m_bError = true;
        }
    }

}

int
CImageWriter::Update(CCommand *a_pCmd)
{
    int nEnable;
    if (a_pCmd->GetIntVar("enabletn", &nEnable) != -1)
        m_bEnableTNs = nEnable > 0 ? true : false;

    if (a_pCmd->GetIntVar("native", &nEnable) != -1)
        m_bEnableNative = nEnable > 0 ? true : false;

    if (a_pCmd->GetIntVar("tnlarge", &nEnable) != -1)
        m_bEnableLargeTN = nEnable > 0 ? true : false;

    if (a_pCmd->GetIntVar("tnsmall", &nEnable) != -1)
        m_bEnableSmallTN = nEnable > 0 ? true : false;

    a_pCmd->GetIntVar("tnlargewidth", &m_nLargeTN_Width);

    a_pCmd->GetIntVar("tnlargeheight", &m_nLargeTN_Height);

    a_pCmd->GetIntVar("tnsmallwidth", &m_nSmallTN_Width);

    a_pCmd->GetIntVar("tnsmallheight", &m_nSmallTN_Height);

    a_pCmd->GetIntVar("tntimeinterval", &m_nThumbnailInterval);

    char publishURL[MAXURLLEN + 1];
    strcpy(publishURL, "");
    if (a_pCmd->GetVar("publishURL", publishURL, MAXURLLEN) != -1)
        m_strPublishURL = publishURL;

    SetMediaProcessorParams();

    return 1;
}


void
CImageWriter::UpdateCustomStatus(CCommand *a_pCmd)
{
    if (strcmp(m_pStreamType, "RTP") == 0)
        a_pCmd->SetObjectParam("SDPFile", m_pSDPFilePath);

    a_pCmd->SetObjectParam("FPGAPort",
                           m_nIOPort);

    if (m_pFFMediaProcessor == NULL)
        return;

    char buffer[32];

    a_pCmd->SetObjectParam("StreamType", m_pFFMediaProcessor->GetStreamType());
    a_pCmd->SetObjectParam("ThumbnailDstPath", m_pFFMediaProcessor->GetMediaDestDir());

    a_pCmd->SetObjectParam("EnableThumbnails", m_bEnableTNs);

    if (a_pCmd->StatusLevel(1)) {
        a_pCmd->SetObjectParam("PublishURL", m_pFFMediaProcessor->GetPublishURL());
        a_pCmd->SetObjectParam("PublishErrorMsg", m_pFFMediaProcessor->GetPublishError());
    }

    a_pCmd->SetObjectErrorCounterParam("PublishErrors",
            m_pFFMediaProcessor->GetPublishErrors());

    a_pCmd->SetObjectCounterParam("ThumbnailsCount",
            m_pFFMediaProcessor->GetTotalThumbnailCount());

    bool bTemp = m_pFFMediaProcessor->IsSnapshotEnabled();
    a_pCmd->SetObjectParam("CreateNative", bTemp);
    a_pCmd->SetObjectParam("NativeWidth",
            m_pFFMediaProcessor->GetNativeWidth());
    a_pCmd->SetObjectParam("NativeHeight",
            m_pFFMediaProcessor->GetNativeHeight());

    bTemp = m_pFFMediaProcessor->IsLargeTNEnabled();
    a_pCmd->SetObjectParam("CreateLargeThumbnail", bTemp);
    a_pCmd->SetObjectParam("LargeThumbnailWidth",
            m_pFFMediaProcessor->GetPrimaryTNWidth());
    a_pCmd->SetObjectParam("LargeThumbnailHeight",
            m_pFFMediaProcessor->GetPrimaryTNHeight());

    bTemp = m_pFFMediaProcessor->IsSmallTNEnabled();
    a_pCmd->SetObjectParam("CreateSmallThumbnail", bTemp);
    a_pCmd->SetObjectParam("SmallThumbnailWidth",
            m_pFFMediaProcessor->GetStreamTNWidth());
    a_pCmd->SetObjectParam("SmallThumbnailHeight",
            m_pFFMediaProcessor->GetStreamTNHeight());

    a_pCmd->SetObjectParam("ThumbnailInterval",
            m_pFFMediaProcessor->GetStreamTNTimeInterval());


    a_pCmd->SetObjectParam("DurationSinceLastTN",
            m_pFFMediaProcessor->GetCurrentTNTimestamp() == 0 ? 0 :
    CTimeKeeper::GetDurationString(CTimeKeeper::Now() -
            m_pFFMediaProcessor->GetCurrentTNTimestamp(), buffer));


    if (a_pCmd->StatusLevel(3)) {
        a_pCmd->SetObjectCounterParam("TotalFramesInQueue",
                m_pFFMediaProcessor->GetTotalFramesInQueue());
        a_pCmd->SetObjectParam("CurrentFramesInQueue",
                m_pFFMediaProcessor->GetCurrentFramesInQueue());

        a_pCmd->SetObjectCounterParam("TotalScapsInQueue",
                m_pFFMediaProcessor->GetTotalScapsInQueue());
        a_pCmd->SetObjectParam("CurrentScapsInQueue",
                m_pFFMediaProcessor->GetCurrentScapsInQueue());

        a_pCmd->SetObjectParam("CurrentTNTimestamp",
                m_pFFMediaProcessor->GetCurrentTNTimestamp());
    }
}
