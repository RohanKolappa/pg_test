/*
 * ffv2d.cpp
 *
 *  Created on: Jun 24, 2010
 *      Author: rkalem_strMediaDestDir
 */

#include "strmsrv_compatibility.h"
#include <math.h>
#include <sstream>

#include "FFV2d.hh"
#include "ScapProcessor.hh"
#include "squeue.hh"
#include "vframeimager.hh"
#include "utils.hh"
#include "FFUtils.hh"
#include "MediaServer.hh"
#include "MediaWriter.hh"
#include "sframe.hh"

#include "vframedecoder.hh"
#include "ThumbnailGenerator.h"
#include "URLFetcher.hh"

//#define USE_MEDIAWRITER

using namespace std;
using namespace mediaindex;

CScapDataForDec::CScapDataForDec(int a_nWidth, int a_nHeight, int a_nSliceSize,
                 eEncodeFormat a_eEncodeFormat, ePixelFormat a_eDecodePixFormat)
{
    m_nWidth = a_nWidth;
    m_nHeight = a_nHeight;
    m_nSliceSize = a_nSliceSize;
    m_eEncodeFormat = a_eEncodeFormat;
    m_eDecodePixelFormat = a_eDecodePixFormat;
}

CScapDataForDec::~CScapDataForDec()
{

}

int CScapDataForDec::GetWidth()
{
    return m_nWidth;
}
int CScapDataForDec::GetHeight()
{
    return m_nHeight;
}

int CScapDataForDec::GetSliceSize()
{
    return m_nSliceSize;
}

eEncodeFormat CScapDataForDec::GetEncodeFormat()
{
    return m_eEncodeFormat;
}

ePixelFormat CScapDataForDec::GetDecodeFormat()
{
    return m_eDecodePixelFormat;
}

CFFV2d::CFFV2d(int nId, std::vector<int> & trackSpeeds, bool bShowProgress,
               bool bLiveMode, bool bNativeOutput, bool bCreateTracks)
: CFFCodec("V2D", bShowProgress, bLiveMode, bNativeOutput)
{
    m_trackSpeeds = trackSpeeds;
    m_nNumbTracks = trackSpeeds.size();
    m_bCreateTracks = bCreateTracks;
    m_nCurrentTimestamp = 0;
    m_nBeginTimestamp = 0;
    m_nEndTimestamp = 0;
    m_nElapsed = 0;

    m_srd = NULL;
    m_nControlFrames = 0;
    m_nVideoFrames = 0;
    m_nAudioFrames = 0;
    m_nResolutionChangedCount = 0;
    m_nThumbnailPublishErrors = 0;
    m_bIsVideoSrcRTP = false;
    m_bDone = false;
    m_bStarted = false;
    m_nProcessedBytes = 0;
    m_nLastProcessedBytes = 0;

    m_PercentComplete = 0.0;
    m_LastPercentComplete = 0.0;
    m_nDuration = 0;

    m_strMediaDestDir = "";
    m_bQueuedFrameForPrimaryTN = false;

    m_nDecodeSize = 0;
    m_pDecodeBuffer = NULL;

    m_scapProcessor = new CScapProcessor();

    CreateQueues(nId);
    CreateTrackHandlers();

    m_eDecodePixelFormat = eBGRX32;
    m_pVFrameDecoder = NULL;
    m_pMediaWriter = NULL;
    m_pScapDataForTN = NULL;

    m_pTimeStampFile = NULL;

    if (m_bLiveMode == true) {
        this->Start();
    }
}

CFFV2d::~CFFV2d()
{
    delete m_scapProcessor;

    DisableSRD();

    DeleteQueues();
    DeleteTrackHandlers();

    delete m_pVFrameDecoder;

    delete [] m_pDecodeBuffer;

    delete m_pScapDataForTN;

    delete m_pMediaWriter;
}

void CFFV2d::CreateQueues(int nId) {
    if (m_bCreateTracks == true) {
        for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
            ostringstream qid;
            qid << "FFQueue_" << nId << "_" << m_trackSpeeds[trackIndex];
            m_qFFQueueSink[trackIndex] = CSQueueManager::Instance()->GetQueueSink(qid.str());
        }
    }

    ostringstream tnQID;
    tnQID << "TNQueueSource_" << nId;
    m_qTNQueueSource = CSQueueManager::Instance()->GetQueueSource(tnQID.str());
    m_qTNQueueSource->SetMaxBytes(20 * 1024 * 1024);

    m_qTNQueueSink = CSQueueManager::Instance()->GetQueueSink(tnQID.str());
}

void CFFV2d::DeleteQueues() {
    if (m_bCreateTracks == true) {
        for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
            if (m_qFFQueueSink[trackIndex] != NULL)
                CSQueueManager::Instance()->ReleaseQueueSink(m_qFFQueueSink[trackIndex]);
        }
    }

    if (m_qTNQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qTNQueueSource);

    if (m_qTNQueueSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qTNQueueSink);
}

void CFFV2d::CreateTrackHandlers() {
    if (m_bCreateTracks == false)
        return;

    for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
        m_pV2DTracks[trackIndex] = new CFFV2dTrack(this, trackIndex);
    }
}

void CFFV2d::DeleteTrackHandlers() {
    if (m_bCreateTracks == false)
        return;

    for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
        delete m_pV2DTracks[trackIndex];
    }
}

sliceRedData_t * CFFV2d::GetSRD()
{
    return m_srd;
}

timestamp_t CFFV2d::GetCurrentTimeStamp()
{
    return m_nCurrentTimestamp;
}

CSQueueSink * CFFV2d::GetFFQueueSinkAt(int index)
{
    return m_qFFQueueSink[index];
}

int CFFV2d::GetTrackSpeed(int index)
{
    return m_trackSpeeds[index];
}

uint8_t * CFFV2d::GetScapBuffer()
{
    return m_scapProcessor->GetCachedSCAPBuffer();
}

int CFFV2d::GetScapBufferSize()
{
    return m_scapProcessor->GetCachedSCAPLength();
}

void CFFV2d::ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset)
{
    if (m_bDone)
        return;

    if (sFrame == NULL) {
        return;
    }

    InitThumbnails();

    m_nElapsed = nMediaOffset;

    if (m_nCurrentTimestamp == 0) {
        m_nCurrentTimestamp = sFrame->GetTimeStamp();
        m_nBeginTimestamp = m_nCurrentTimestamp;
        if (m_bCreateTracks == true) {
            for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++)
                m_pV2DTracks[trackIndex]->Init();
        }
    }

    m_nCurrentTimestamp = sFrame->GetTimeStamp();

    // Track Start/Stop of stream
    if (sFrame->GetMessageType() != CSFrame::Normal) {
        if (sFrame->GetMessageType() == CSFrame::StartOfStream) {
            if (m_bCreateTracks == true) {
                for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
                    CSFrame *pSFrame = new CSFrame(sFrame->GetTimeStamp(),
                                        CSFrame::StartOfStream);
                    m_qFFQueueSink[trackIndex]->InsertFrame(pSFrame);
                }
            }
            if (m_pMediaWriter != NULL)
                m_pMediaWriter->Start(sFrame->GetTimeStamp());
            m_bStarted = true;
        }
        else if (sFrame->GetMessageType() == CSFrame::EndOfStream) {
            if (m_bCreateTracks == true) {
                for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
                    CSFrame *pSFrame = new CSFrame(sFrame->GetTimeStamp(),
                                        CSFrame::EndOfStream);
                    m_qFFQueueSink[trackIndex]->InsertFrame(pSFrame);
                }
            }
            if (m_pMediaWriter != NULL)
                m_pMediaWriter->Stop(sFrame->GetTimeStamp());
            m_bStarted = false;
            m_nEndTimestamp = m_nCurrentTimestamp;
        }
        else {
            StopStream();
            return;
        }
    }

    // Ignore packets that come in when stream is not started
    if (!m_bStarted)
        return;

    // Throw away non V2D packets
    if (sFrame->GetStreamType(STREAMTYPE_V2D) != STREAMTYPE_V2D) {
        StopStream();
        return;
    }

    m_nProcessedBytes += sFrame->GetLength();

    CV2DStreamMediaFlag v2dSMediaFlag(sFrame->GetMediaFlag());
    short nChannel = v2dSMediaFlag.GetChannel();

    switch (nChannel) {

    case FRAMETYPE_V2D_CHAN_CTL:
        ProcessControlFrame(sFrame);
        break;

    case FRAMETYPE_V2D_CHAN_VIDEO:
        m_bIsVideoSrcRTP =
           (v2dSMediaFlag.GetEncaps() == FRAMETYPE_V2D_ENCAPS_RTP);
        ProcessVideoFrame(sFrame);
        break;

    case FRAMETYPE_V2D_CHAN_AUDIO:
        ProcessAudioFrame(sFrame);
        break;

    default:
        break;
    }

    if (m_bCreateTracks == true) {
        for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++) {
            m_pV2DTracks[trackIndex]->ProcessSRDFrame();
        }
    }

    PrintProgress();

    PutFrameInQueueForTN();

    if (m_bLiveMode == false) {
        CreateThumbnails();
    }
}

void CFFV2d::ProcessControlFrame(CSFrame *sFrame) {
    m_nControlFrames++;
    ProcessSCAP(sFrame);
}

void CFFV2d::ProcessSCAP(CSFrame *sFrame) {

    if (!m_scapProcessor->Process(sFrame->GetData()))
        return;

    if (m_scapProcessor->ResolutionChangeDetected()) {
        if (m_bCreateTracks == true) {
            for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++)
                m_pV2DTracks[trackIndex]->ResetScap();
        }
        m_nResolutionChangedCount++;

        DisableSRD();
        EnableSRD();
    }


    if (m_srd == NULL)
        return;

    m_srd->sliceSize = m_scapProcessor->GetSliceSize();
    m_srd->MaxSliceNo = m_scapProcessor->GetMaxSliceNumber();
    setStereoVideo(m_srd, m_scapProcessor->GetStereoMode());
    setCompMode(m_srd, m_scapProcessor->GetVideoCompMode());
    setMotionCompMode(m_srd, m_scapProcessor->GetVideoMotionCompMode());

    if (m_scapProcessor->ResolutionChangeDetected()) {
        PutScapInQueueForTN();
    }
}

void CFFV2d::EnableSRD()
{
    m_srd = (sliceRedData_t *) malloc(sizeof(sliceRedData_t));
    if (m_srd != NULL) {
        if (initSRD(m_srd) < 0) {
            destroySRD(m_srd);
            free(m_srd);
            m_srd = NULL;
        }
        else {
            setStereoPolicy(m_srd, SP_SOURCE);
            SRDSetCodecVersion(m_srd, m_scapProcessor->GetCodecVersion());

            SRDEnableInternalRefresh(m_srd);
            int nTracks = 1;
            if (m_bCreateTracks == true)
            	nTracks = m_nNumbTracks;
            SRDSetNumberOfTracks(m_srd, nTracks);
        }
    }
}

void CFFV2d::DisableSRD()
{
    if (m_srd != NULL) {
        destroySRD(m_srd);
        free(m_srd);
        m_srd = NULL;
    }
}

void CFFV2d::ProcessVideoFrame(CSFrame *sFrame) {
    m_nVideoFrames++;
    if (m_srd == NULL) {
        return;
    }

    SendFrameToSRD(sFrame);
}

void CFFV2d::ProcessAudioFrame(CSFrame *sFrame) {
    // Audio frames are ignored
    m_nAudioFrames++;
}

int CFFV2d::SendFrameToSRD(CSFrame* sFrame)
{
    const int nDataLen    = sFrame->GetLength();
    const int nDataOffset = (m_bIsVideoSrcRTP) ?
                            RTP_HEADER_SIZE : V2D_PACKET_HEADER_SIZE;
    int nDoneLen = 0;
    unsigned char* pCurrData =
        (unsigned char *)(sFrame->GetData() + nDataOffset);

    while (nDoneLen < nDataLen) {
        if (m_bCreateTracks == true) {
            for (int trackIndex = 0; trackIndex < m_nNumbTracks; trackIndex++)
                m_pV2DTracks[trackIndex]->AdjustTokenMeter(1);
        }

        findFramesinPacket(m_srd, pCurrData);
        pCurrData += nDataOffset + SRD_VIDEO_PACKET_SIZE;
        nDoneLen  += nDataOffset + SRD_VIDEO_PACKET_SIZE;
    }

    return 0;
}

void CFFV2d::PrepareForFinalThumbnails()
{
    while (m_qTNQueueSource->Size() > 1) {
        if (((GetThumbnailHandler()->IsPrimaryTNEnabled() == true) &&
              GetThumbnailHandler()->IsPrimaryTNCreated() == false) ||
                GetThumbnailHandler()->IsStreamTNEnabled() == true)
        {
            CSFrame *pCSFrame = m_qTNQueueSource->GetFrame();
            if (pCSFrame != NULL) {
                if (pCSFrame->GetMessageType() == CSFrame::Custom) {
                    SetScapDataForTN(pCSFrame);
                }
                m_qTNQueueSource->ReleaseFrame(pCSFrame);
            }
        }
    }

    GetThumbnailHandler()->SetFinalThumbnailsStatus(true);
}

void CFFV2d::StopStream()
{
    if (m_bDone == false) {
        m_bDone = true;
    }

    if ((GetThumbnailHandler()->IsPrimaryTNEnabled() == true )
            && GetThumbnailHandler()->IsPrimaryTNCreated() == false) {
        GetThumbnailHandler()->SetPrimaryTNOffset(0);
        GetThumbnailHandler()->SetNextTNOffset(0);

        PutFrameInQueueForTN(true);
    }

    m_bStopStreamIssued = true;

    //If Primary thumbnail is not yet created, then drain the queue and create the thumbnail with latest frame.
    if (GetThumbnailHandler()->IsPrimaryTNCreated() == false) {
        if (m_bLiveMode == false) {
            PrepareForFinalThumbnails();
            CreateThumbnails();
        }
        else {
            while ((!GetThumbnailHandler()->IsFinalThumbailsProcessed() &&
                    !GetThumbnailHandler()->IsPrimaryTNCreated()) ||
                   (!m_qTNQueueSource->Size() == 0)) {
                LightSleep();
            }
        }
    }

    if (m_bLiveMode == true)
        Stop();

    //Still didn't get the correct frame, then create a black frame as thumbnail.
    if (GetThumbnailHandler()->IsPrimaryTNEnabled() == true &&
            GetThumbnailHandler()->IsPrimaryTNCreated() == false) {
        CreateBlackThumbnails();
    }

    if (m_pTimeStampFile != NULL) {
        fclose(m_pTimeStampFile);
        m_pTimeStampFile = NULL;
    }
}

void CFFV2d::CreateBlackThumbnails()
{
    if (m_pDecodeBuffer != NULL) {
        delete [] m_pDecodeBuffer;
        m_pDecodeBuffer = NULL;
    }

    int nFrameWidth = DEFAULT_FRAME_WIDTH;
    int nFrameHeight = DEFAULT_FRAME_HEIGHT;

    m_nDecodeSize = nFrameWidth * nFrameHeight * 4;
    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];
    char strFileName[255];
    sprintf(strFileName,"%s/tn.jpg",
            GetThumbnailHandler()->GetTNDirectory().c_str());

    if (CreateImage(nFrameWidth, nFrameHeight,
                    GetThumbnailHandler()->GetPrimaryTNWidth(),
                    GetThumbnailHandler()->GetPrimaryTNHeight(),
                    strFileName) == true) {
        GetThumbnailHandler()->SetPrimaryTNCreated();

        GetThumbnailHandler()->IncrementTNCount();
    }
}

void CFFV2d::InitThumbnails()
{
    if (GetThumbnailHandler()->IsCreatingAnyTN() == false)
        return;

#ifndef USE_MEDIAWRITER
    if (GetThumbnailHandler()->IsLiveThumbnailEnabled() == true) {
        if (false == CFFUtils::IsDirectory(m_strMediaDestDir.c_str())) {
            printf("Thumbnail destination path %s not found.\n", m_strMediaDestDir.c_str());
            return;
        }
        GetThumbnailHandler()->SetTNDirectory(m_strMediaDestDir);
    }
    else {
        if (!GetThumbnailHandler()->IsTNDirectoryCreated()) {
            GetThumbnailHandler()->CreateTNDirectory(m_strMediaDestDir);
        }
    }

    if (m_pTimeStampFile == NULL) {
        char strBuffer[255];
        sprintf(strBuffer,"%s/timestamp.txt",
                GetThumbnailHandler()->GetTNDirectory().c_str());

        m_pTimeStampFile = fopen(strBuffer, "w");
        if (m_pTimeStampFile == NULL) {
            printf("Failed in creating timestamp.txt \n");
        }
    }
#else
    if (m_pMediaWriter == NULL) {
        m_pMediaWriter = new CMediaWriter(m_strMediaDestDir.c_str());
        m_pMediaWriter->EnableThumbnails(true);
        m_pMediaWriter->EnableMedia(false);
        m_pMediaWriter->Init();
    }
#endif
}

void CFFV2d::SetScapDataForTN(CSFrame * a_pCSFrame)
{
    CSBuffer *pBuffer = a_pCSFrame->GetBuffer();
    CScapDataForDec *pScapData = (CScapDataForDec*)pBuffer->GetBuffer(0);

    if (pScapData == NULL) {
    	return;
    }

    if (m_pScapDataForTN != NULL) {
        delete m_pScapDataForTN;
        m_pScapDataForTN = NULL;
    }

    m_pScapDataForTN = new CScapDataForDec(pScapData->GetWidth(), pScapData->GetHeight(),
                                           pScapData->GetSliceSize(), pScapData->GetEncodeFormat(),
                                           pScapData->GetDecodeFormat());
    if (m_pScapDataForTN == NULL)
        return;

    InitializeDecoder();

    m_nNativeWidth = m_pScapDataForTN->GetWidth();
    m_nNativeHeight = m_pScapDataForTN->GetHeight();

    GetThumbnailHandler()->ReCreateThumbnailGenerator(m_nNativeWidth,
                                                      m_nNativeHeight,
                                                      m_eDecodePixelFormat);
}

void CFFV2d::PutScapInQueueForTN()
{
    if (GetThumbnailHandler()->IsCreatingAnyTN() == false)
        return;

    if (!GetThumbnailHandler()->IsStreamTNEnabled() &&
            GetThumbnailHandler()->IsPrimaryTNCreated() &&
            !GetThumbnailHandler()->IsLiveThumbnailEnabled())
        return;

    CSFrame *sFrame = new CSFrame(m_nCurrentTimestamp, CSFrame::Custom);

    CSBuffer *sBuffer = new CSBuffer(sizeof(CScapDataForDec) , 64);
    char * buffer = sBuffer->GetBuffer(0);
    new (buffer) CScapDataForDec(m_scapProcessor->GetWidth(),
                                 m_scapProcessor->GetHeight(),
                                 m_scapProcessor->GetSliceSize(),
                                 eYUV422P,
                                 m_eDecodePixelFormat); // Using "placement new" ctor

    sFrame->SetData(sBuffer, 0, sizeof(CScapDataForDec));

    m_qTNQueueSink->InsertFrame(sFrame);

    m_nTotalScapsInQueue++;
    m_nCurrentScapsInQueue++;
}

void CFFV2d::PutFrameInQueueForTN(bool bStopped)
{
    if (GetThumbnailHandler()->IsCreatingAnyTN() == false)
        return;

    if (m_srd == NULL)
        return;

    if (GetThumbnailHandler()->IsStreamTNEnabled() == false &&
            GetThumbnailHandler()->IsPrimaryTNCreated() == true &&
             GetThumbnailHandler()->IsLiveThumbnailEnabled() == false)
        return;

    //If it after stop message, we are trying to create tn with
    //the frame data, even if srd doesn't contain any full-frame.
    if ((m_srd->FullFrameCount < 1) && (bStopped == false))
        return;

    if (!NeedToQueueFrameForThumbnail())
        return;

    int frameSize = getFullFrameSize(m_srd);
    if (frameSize <= 0) {
        return;
    }

    unsigned char * pInBuffer = new unsigned char[frameSize];
    if (pInBuffer == NULL) {
        return;
    }

    int rc = getFullFrame(m_srd, pInBuffer, frameSize);
    if (rc != frameSize) {
        delete [] pInBuffer;
        return;
    }

    CSFrame *pSFrame = new CSFrame(m_nCurrentTimestamp, CSFrame::Normal);
    pSFrame->SetData((char*)pInBuffer, frameSize);
    m_qTNQueueSink->InsertFrame(pSFrame);

    m_nTotalFramesInQueue++;

    {
        CScopedLock lock(m_Mutex);
        m_nCurrentFramesInQueue++;
    }

//    if ((GetThumbnailHandler()->GetNextTNOffset() == 0 ||
//            (m_nCurrentTimestamp - m_nBeginTimestamp) >= GetThumbnailHandler()->GetNextTNOffset())) {
//        GetThumbnailHandler()->SetNextTNOffset(GetThumbnailHandler()->GetStreamTNFrequency());
//    }

    if ((GetThumbnailHandler()->GetNextTNOffset() == 0 ||
            (m_nCurrentTimestamp - m_nBeginTimestamp) >= GetThumbnailHandler()->GetNextTNOffset())) {
        if (GetThumbnailHandler()->GetStreamTNFrequency() > 0 &&
                (GetThumbnailHandler()->IsStreamTNEnabled() || GetThumbnailHandler()->IsLiveThumbnailEnabled())) {
            while ((m_nCurrentTimestamp - m_nBeginTimestamp) >= GetThumbnailHandler()->GetNextTNOffset())
                GetThumbnailHandler()->SetNextTNOffset(GetThumbnailHandler()->GetStreamTNFrequency());
        }
    }

    delete [] pInBuffer;
}

bool CFFV2d::NeedToQueueFrameForThumbnail() {
	//In live mode, if there are couple of frame in the queue,
	//we are avoiding placing another complete frame in the queue.
    if (m_bLiveMode) {
        if (m_nCurrentFramesInQueue > 1)
            return false;
    }

    // Is it time to serve the primary thumbnail
    if (GetThumbnailHandler()->IsPrimaryTNEnabled()) {
        if (!m_bQueuedFrameForPrimaryTN) {
            if (m_nCurrentTimestamp - m_nBeginTimestamp > GetThumbnailHandler()->GetPrimaryTNOffset()) {
                m_bQueuedFrameForPrimaryTN = true;
                return true;
            }
        }
    }

    // Are all kinds of live thumbnails disabled
    if (GetThumbnailHandler()->IsLiveThumbnailEnabled()) {
        if (!(GetThumbnailHandler()->IsSnapshotEnabled() ||
                GetThumbnailHandler()->IsLargeTNEnabled() ||
                GetThumbnailHandler()->IsSmallTNEnabled()))
                return false;
    }

    // Is is time to create a periodic thumbnail
    if (GetThumbnailHandler()->IsStreamTNEnabled() ||
            GetThumbnailHandler()->IsLiveThumbnailEnabled()) {

       if (GetThumbnailHandler()->GetNextTNOffset() == 0)
           return true;

       if ((m_nCurrentTimestamp - m_nBeginTimestamp) >= GetThumbnailHandler()->GetNextTNOffset())
           return true;
    }

    return false;
}

int CFFV2d::Run()
{
    while (!m_bExit) {
        CreateThumbnails();
        LightSleep();

        if (m_bStopStreamIssued == true) {
            PrepareForFinalThumbnails();
        }
    }

    return 0;
}

void CFFV2d::CreateThumbnails() {

    if (GetThumbnailHandler()->IsCreatingAnyTN() == false)
        return;

    if (GetThumbnailHandler()->IsPrimaryTNCreated() == true &&
            GetThumbnailHandler()->IsStreamTNEnabled() == false
            && GetThumbnailHandler()->IsLiveThumbnailEnabled() == false)
        return;

    CSFrame *pCSFrame = m_qTNQueueSource->GetFrame();
    if (pCSFrame == NULL)
        return;

    if (pCSFrame->GetMessageType() == CSFrame::Normal) {
        CreatePrimaryTN(pCSFrame);
        CreateStreamTN(pCSFrame);

        {
            CScopedLock lock(m_Mutex);
            m_nCurrentFramesInQueue--;
        }
    }
    else if (pCSFrame->GetMessageType() == CSFrame::Custom) {
        SetScapDataForTN(pCSFrame);

        m_nCurrentScapsInQueue--;
    }
    m_qTNQueueSource->ReleaseFrame(pCSFrame);
}

void CFFV2d::CreatePrimaryTN(CSFrame *a_pCSFrame)
{
    if (GetThumbnailHandler()->IsPrimaryTNEnabled() == false ||
            GetThumbnailHandler()->IsPrimaryTNCreated() == true)
        return;

    uint8_t *pFrameBuffer = (uint8_t*)a_pCSFrame->GetData();
    int nFrameBuffLen = a_pCSFrame->GetLength();
    uint64_t nCurrentTimestamp = a_pCSFrame->GetTimeStamp();

    if (nCurrentTimestamp - m_nBeginTimestamp > GetThumbnailHandler()->GetPrimaryTNOffset()) {
        char strFileName[255];
        sprintf(strFileName,"%s/tn.jpg", GetThumbnailHandler()->GetTNDirectory().c_str());

        if (CreateSingleImage(pFrameBuffer, nFrameBuffLen, strFileName,
                              GetThumbnailHandler()->GetPrimaryTNWidth(),
                              GetThumbnailHandler()->GetPrimaryTNHeight()) == true) {
            GetThumbnailHandler()->SetPrimaryTNCreated();

            m_nCurrentTNTS = nCurrentTimestamp;
            GetThumbnailHandler()->IncrementTNCount();
        }
    }
}

void CFFV2d::CreateStreamTN(CSFrame *a_pCSFrame)
{
    uint8_t *pFrameBuffer = (uint8_t*)a_pCSFrame->GetData();
    int nFrameBuffLen = a_pCSFrame->GetLength();
    uint64_t nCurrentTimestamp = a_pCSFrame->GetTimeStamp();

    if ((GetThumbnailHandler()->IsStreamTNEnabled() == false &&
            GetThumbnailHandler()->IsLiveThumbnailEnabled() == false)
            || GetThumbnailHandler()->GetStreamTNFrequency() <= 0) {
        return;
    }

    char strBuffer[255];
    bool bCreatedThumbnail = false;

    if (GetThumbnailHandler()->IsStreamTNEnabled() == true) {
        sprintf(strBuffer, "%s/%"PRIu64".jpg",
                GetThumbnailHandler()->GetTNDirectory().c_str(),
                nCurrentTimestamp);
        if (CreateSingleImage(pFrameBuffer, nFrameBuffLen, strBuffer,
                              GetThumbnailHandler()->GetStreamTNWidth(),
                              GetThumbnailHandler()->GetStreamTNHeight()) == true ) {
            GetThumbnailHandler()->SetFirstStreamTNCreated();
            GetThumbnailHandler()->IncrementTNCount();
            bCreatedThumbnail = true;
        }
    }
    else {

        //Using same set of members(Primary and Streams TN parameters) for Live thumbnails also.
        if (GetThumbnailHandler()->IsLargeTNEnabled() == true) {
            sprintf(strBuffer, "%s/tn_big.jpg",
                    GetThumbnailHandler()->GetTNDirectory().c_str());
            if (CreateSingleImage(pFrameBuffer, nFrameBuffLen, strBuffer,
                                  GetThumbnailHandler()->GetPrimaryTNWidth(),
                                  GetThumbnailHandler()->GetPrimaryTNHeight()) == true ) {
                GetThumbnailHandler()->IncrementTNCount();
            	bCreatedThumbnail = true;
            }
        }

        if (GetThumbnailHandler()->IsSmallTNEnabled() == true ) {
            if (GetThumbnailHandler()->IsLargeTNEnabled() == false) {
                sprintf(strBuffer, "%s/tn_small.jpg",
                        GetThumbnailHandler()->GetTNDirectory().c_str());
                if (CreateSingleImage(pFrameBuffer, nFrameBuffLen, strBuffer,
                                      GetThumbnailHandler()->GetStreamTNWidth(),
                                      GetThumbnailHandler()->GetStreamTNHeight()) == true ) {
                    GetThumbnailHandler()->IncrementTNCount();
                    bCreatedThumbnail = true;
                }
            }
            else
            {
                GetThumbnailHandler()->SetThumbnailResolution(
                            GetThumbnailHandler()->GetStreamTNWidth(),
                            GetThumbnailHandler()->GetStreamTNHeight());

                const int tnSize = GetThumbnailHandler()->GetStreamTNWidth() *
                                   GetThumbnailHandler()->GetStreamTNHeight() * 3;
                unsigned char *pTNBuffer = new unsigned char[tnSize];

                if (m_pDecodeBuffer != NULL) {
                    int nTNSize = GetThumbnailHandler()->CreateImage(&m_pDecodeBuffer, &pTNBuffer, tnSize);
                    if (nTNSize <= 0) {
                        delete [] pTNBuffer;
                        return;
                    }
                    sprintf(strBuffer, "%s/tn_small.jpg",
                            GetThumbnailHandler()->GetTNDirectory().c_str());
                    GetThumbnailHandler()->CreateImageFile(&pTNBuffer, nTNSize, strBuffer);
                    GetThumbnailHandler()->IncrementTNCount();

                    bCreatedThumbnail = true;

                    m_nBytesWritten += nTNSize;
                }
                delete [] pTNBuffer;
            }
        }

        if (GetThumbnailHandler()->IsSnapshotEnabled() == true ) {
            if (GetThumbnailHandler()->IsLargeTNEnabled() == false &&
                    GetThumbnailHandler()->IsSmallTNEnabled() == false) {
                sprintf(strBuffer, "%s/snapshot.jpg",
                        GetThumbnailHandler()->GetTNDirectory().c_str());
                if (CreateSingleImage(pFrameBuffer, nFrameBuffLen, strBuffer,
                                      m_pScapDataForTN->GetWidth(),
                                      m_pScapDataForTN->GetHeight())) {
                    GetThumbnailHandler()->IncrementTNCount();
                    bCreatedThumbnail = true;
                }
            }
            else {
                GetThumbnailHandler()->SetThumbnailResolution(m_pScapDataForTN->GetWidth(),
                                                              m_pScapDataForTN->GetHeight());

                const int tnSize = m_pScapDataForTN->GetWidth() * m_pScapDataForTN->GetHeight() * 3;
                unsigned char *pTNBuffer = new unsigned char[tnSize];

                int nTNSize = GetThumbnailHandler()->CreateImage(&m_pDecodeBuffer, &pTNBuffer, tnSize);
                if (nTNSize <= 0) {
                    delete [] pTNBuffer;
                    return;
                }
                sprintf(strBuffer, "%s/snapshot.jpg",
                        GetThumbnailHandler()->GetTNDirectory().c_str());
                GetThumbnailHandler()->CreateImageFile(&pTNBuffer, nTNSize, strBuffer);
                GetThumbnailHandler()->IncrementTNCount();

                delete [] pTNBuffer;
                bCreatedThumbnail = true;

                m_nBytesWritten += nTNSize;
            }
        }
    }

    if (bCreatedThumbnail == true) {
        if(m_pTimeStampFile != NULL) {
            sprintf(strBuffer, "%"PRIu64"\n", nCurrentTimestamp);

            fseek(m_pTimeStampFile, 0, SEEK_SET);
            fwrite(strBuffer, 1, strlen(strBuffer), m_pTimeStampFile);
        }

        m_nCurrentTNTS = nCurrentTimestamp;
    }

    if (bCreatedThumbnail == true) {
        PublishThumbnails();
    }
}

bool CFFV2d::CreateSingleImage(
        unsigned char *a_pFrameData,
        int a_nFrameSize,
        char * a_pStrFileName,
        int a_nThumbnailWidth,
        int a_nThumbnailHeight) {

    if (m_pScapDataForTN == NULL) {
        return false;
    }

    if (m_pVFrameDecoder->Decode(a_pFrameData, a_nFrameSize, m_pDecodeBuffer, m_nDecodeSize) != -1) {
        if (m_pDecodeBuffer != NULL) {
            return CreateImage(m_pScapDataForTN->GetWidth(), m_pScapDataForTN->GetHeight(),
                               a_nThumbnailWidth, a_nThumbnailHeight, a_pStrFileName);
        }
    }

    return true;
}

bool CFFV2d::CreateImage(int a_nInputWidth, int a_nInputHeight,
                         int a_nOutputWidth, int a_nOutputHeight,
                         char * a_pStrFileName)
{
    if (false == GetThumbnailHandler()->ReCreateThumbnailGenerator(
            a_nInputWidth, a_nInputHeight, m_eDecodePixelFormat)) {
        return false;
    }

    GetThumbnailHandler()->SetThumbnailResolution(a_nOutputWidth, a_nOutputHeight);

    const int tnSize = a_nOutputWidth * a_nOutputHeight * 3;
    unsigned char *pTNBuffer = new unsigned char[tnSize];

    int nTNSize = GetThumbnailHandler()->CreateImage(&m_pDecodeBuffer, &pTNBuffer, tnSize);
    if (nTNSize <= 0) {
        delete [] pTNBuffer;
        return false;
    }

#ifndef USE_MEDIAWRITER
    GetThumbnailHandler()->CreateImageFile(&pTNBuffer, nTNSize, a_pStrFileName);
#else
        uint32_t mediaFlag = IMAGETYPE_JPEG & IMAGETYPE_MASK;
        if (nTimestamp == 0) {
            mediaFlag |= TNTYPE_PRIMARY;
        }
        else {
            mediaFlag |= TNTYPE_STREAM;
        }
        CCFrame aFrame;
        aFrame.SetMessageType(CCFrame::Thumbnail);
        aFrame.SetTimeStamp(nTimestamp);
        aFrame.SetMediaFlag(mediaFlag);
        aFrame.SetData(pTNBuffer);
        aFrame.SetLength(nTNSize);
        if (m_pMediaWriter != NULL)
            m_pMediaWriter->AddThumbnail(aFrame);
#endif

    delete [] pTNBuffer;

    m_nBytesWritten += nTNSize;

    return true;
}


void CFFV2d::PublishThumbnails()
{
    if (!NeedToPublishThumbnails()) {
        return;
    }

    char tnFilePath[255];
    if (GetThumbnailHandler()->IsSmallTNEnabled()) {
        sprintf(tnFilePath, "%s/tn_small.jpg",
                GetThumbnailHandler()->GetTNDirectory().c_str());
        UploadThumbnail(tnFilePath, "tn_small.jpg", "SMALL");
    }
    if (GetThumbnailHandler()->IsLargeTNEnabled()) {
        sprintf(tnFilePath, "%s/tn_big.jpg",
                GetThumbnailHandler()->GetTNDirectory().c_str());
        UploadThumbnail(tnFilePath, "tn_large.jpg", "LARGE");
    }
    if (GetThumbnailHandler()->IsSnapshotEnabled()) {
        sprintf(tnFilePath, "%s/snapshot.jpg",
                GetThumbnailHandler()->GetTNDirectory().c_str());
        UploadThumbnail(tnFilePath, "tn_native.jpg", "NATIVE");
    }

}

void CFFV2d::UploadThumbnail(const char * tnFilePath, const char * tnFileName, const char *tnSize)
{
    std::string publishURL = m_strPublishURL + "&size=" + tnSize;
    CFileUploader *uploader = new CFileUploader(publishURL.c_str());
    if (!uploader->UploadFile(tnFilePath, tnFileName)) {
        m_nThumbnailPublishErrors++;

        CScopedLock lock(m_Mutex);
        m_strPublishError = uploader->GetErrorMsg();
    }
    delete uploader;
}

bool CFFV2d::NeedToPublishThumbnails() {
    if (m_strPublishURL.length() <= 0)
        return false;
    return true;
}

uint64_t CFFV2d::GetThumbnailPublishErrors() {
    return m_nThumbnailPublishErrors;
}

std::string CFFV2d::GetPublishError()
{
	CScopedLock lock(m_Mutex);

    return m_strPublishError;
}

bool CFFV2d::InitializeDecoder()
{
    if (m_pVFrameDecoder != NULL) {
        delete m_pVFrameDecoder;
        m_pVFrameDecoder = NULL;
    }

    if (m_pDecodeBuffer != NULL) {
        delete [] m_pDecodeBuffer;
        m_pDecodeBuffer = NULL;
    }

    v2dDecConfiguration decConfig;
    decConfig.m_nWidth = m_pScapDataForTN->GetWidth();
    decConfig.m_nHeight = m_pScapDataForTN->GetHeight();
    decConfig.m_nSliceSize = m_pScapDataForTN->GetSliceSize();
    decConfig.m_eEncodeFormat = m_pScapDataForTN->GetEncodeFormat();
    decConfig.m_eDecodeFormat = m_pScapDataForTN->GetDecodeFormat();

    m_pVFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);
    m_nDecodeSize = decConfig.m_nDecodeBufLen;

    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];

    return true;
}

void CFFV2d::PrintStats()
{
    char buffer[64];
    printf("Processed: %.2f%% %s\n", m_PercentComplete, GetTimeString(m_nElapsed, buffer));

#if 0
    if (m_nElapsed > 0) {
        if (m_bCreateTracks == true) {
            for (int i = 0; i < m_nNumbTracks; i++) {
                double fps = (m_pV2DTracks[i]->GetVideoFrameCount() * 1000.0) / m_nElapsed;
                printf("FPS[%d] = %.2f\n", i, fps);
            }
        }
    }
#endif

#if 0
    if (m_srd != NULL) {
        printf("SRDOutFrames 0 = %lu \n", m_srd->outFrameCount);
        printf("SRDOutFrames 1 = %lu \n", m_srd->outFrameCountPerTrack[1]);
        printf("SRDOutFrames 2 = %lu \n", m_srd->outFrameCountPerTrack[2]);
        printf("SRDOutputIFrames 0 = %lu \n", m_srd->OutputIFrameCount);
        printf("SRDOutputIFrames 1 = %lu \n", m_srd->OutputIFrameCountPerTrack[1]);
        printf("SRDOutputIFrames 2 = %lu \n", m_srd->OutputIFrameCountPerTrack[2]);
    }
#endif
}

#if 0
void CFFV2d::PrintResults()
{
    char buffer1[64];
    char buffer2[64];
    char buffer3[64];

    if (m_nEndTimestamp > m_nBeginTimestamp)
        m_nElapsed = m_nEndTimestamp - m_nBeginTimestamp;
    else
        m_nElapsed = 1;

    printf("******** Done Processing %d ********\n", m_nSpeedFactor);
    printf("Video Frames = %"PRIu64"\n", m_nVideoFrames);
    printf("Audio Frames = %"PRIu64"\n", m_nAudioFrames);
    printf("Control Frames = %"PRIu64" [Scaps: %"PRIu64" : %"PRIu64" : %"PRIu64"] \n", m_nControlFrames,
            m_nScapFrames, m_nScapOutFrames, m_nResolutionChangedCount);
    printf("Unavailable = %"PRIu64"\n", m_nUnavailable);
    if (m_srd != NULL) {
        printf("SRDInPackets = %lu ", m_srd->inPktCount);
        printf("SRDOutPackets = %lu\n", m_srd->outPktCount);
        printf("SRDInBytes = %lu ", m_srd->inBytes);
        printf("SRDOutBytes = %lu\n", m_srd->outBytes);
        printf("SRDInFrames = %lu ", m_srd->inFrameCount);
        printf("SRDOutFrames = %lu ", m_srd->outFrameCount);
        printf("SRDReducedFrames = %lu\n", m_srd->reducedFrameCount);
        printf("SRDInSlices = %lu ", m_srd->inSlices);
        printf("SRDOutSlices = %lu ", m_srd->outSlices);
        printf("SRDReducedSlices = %lu\n", m_srd->reducedSlices);
        printf("SRDIFrames In = %lu Out = %lu\n",
                m_srd->IFrameCount, m_srd->OutputIFrameCount);
        printf("Bytes Factor = %0.2f\n", (m_srd->inBytes * 1.0)/m_srd->outBytes);
        printf("Frame Factor = %0.2f\n", (m_srd->inFrameCount * 1.0)/m_srd->outFrameCount);
        printf("Slice Factor = %0.2f\n", (m_srd->inSlices * 1.0)/m_srd->outSlices);
        printf("Byte Rate: In = %s Out = %s Scaled = %s\n",
                BitrateToString((m_srd->inBytes * 8000.0)/m_nElapsed, buffer1),
                BitrateToString((m_srd->outBytes * 8000.0)/m_nElapsed, buffer2),
                BitrateToString((m_srd->outBytes * m_nSpeedFactor * 8000.0)/m_nElapsed, buffer3));
        printf("Frame Rate: In = %0.2f Out = %0.2f Scaled = %0.2f\n",
                (m_srd->inFrameCount * 1000.0)/m_nElapsed,
                (m_srd->outFrameCount * 1000.0)/m_nElapsed,
                (m_srd->outFrameCount * m_nSpeedFactor * 1000.0)/m_nElapsed);
        printf("Elapsed: %"PRIu64"  %s Scaled: %s\n",
                m_nElapsed, GetTimeString(m_nElapsed, buffer1),
                GetTimeString(m_nElapsed/m_nSpeedFactor, buffer2));
    }
    printf("*********************************\n");
}

#endif
