/*
 * CFFMpegts.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: durga
 */
#include "strmsrv_compatibility.h"
#include "timekeeper.hh"
#include "utils.hh"
#include "sframe.hh"
#include "MpegtsMediaHandler.hh"
#include "MPEGTSPktParser.hh"
#include "TimeStampAdjuster.hh"
#include "FFMpegts.hh"

const int TS_PACKET_SIZE = 188;

CFFMpegts::CFFMpegts(int a_nId, bool a_bShowProgress,
                     bool a_bLiveMode, bool a_bNativeOutput)
: CFFCodec("MPEGTS", a_bShowProgress, a_bLiveMode, a_bNativeOutput)
{
    m_pParser = NULL;
    m_nBeginTimestamp = 0;
    m_nCurrentTimestamp = 0;

    m_bUpdatedVDiscontinuity = true;
    m_bUpdatedADiscontinuity = true;

    m_nTotalPackets = 0;
    m_nTotalPayloadStarts = 0;

    m_pTSAdjusterForVideo = new CTimeStampAdjuster();
    m_pTSAdjusterForAudio = new CTimeStampAdjuster();
    m_pTSAdjusterForPCR = new CTimeStampAdjuster();
}

CFFMpegts::~CFFMpegts()
{
    StopStream();

    delete m_pParser;

    delete m_pTSAdjusterForVideo;
    delete m_pTSAdjusterForAudio;
    delete m_pTSAdjusterForPCR;
}

void CFFMpegts::Initialize()
{
    m_pParser = new CMPEGTSPktParser();
}

void CFFMpegts::ProcessFrame(CSFrame *a_pSFrame, timestamp_t nMediaOffset)
{
    if (a_pSFrame == NULL) {
        return;
    }

    if (m_bError == true)
        return;

    m_nCurrentTimestamp = a_pSFrame->GetTimeStamp();

    m_nElapsed = nMediaOffset;

    if (m_nBeginTimestamp == 0) {
        m_nBeginTimestamp = m_nCurrentTimestamp;

        Initialize();
    }

    if (a_pSFrame->GetMessageType() == CSFrame::StartOfStream) {
    }
    else if (a_pSFrame->GetMessageType() == CSFrame::EndOfStream) {
        m_bUpdatedVDiscontinuity = false;
        m_bUpdatedADiscontinuity = false;

        m_pTSAdjusterForVideo->SetDiscontinuity();
        m_pTSAdjusterForAudio->SetDiscontinuity();
        m_pTSAdjusterForPCR->SetDiscontinuity();
    }
    else if (a_pSFrame->GetMessageType() == CSFrame::Normal) {
        unsigned long nStreamType = a_pSFrame->GetStreamType();

        if (nStreamType == STREAMTYPE_UDP) {
            CreateOutput(a_pSFrame);
        }
    }

    m_nProcessedBytes += a_pSFrame->GetLength();

    PrintProgress();
}

void CFFMpegts::CreateOutput(CSFrame *a_pSFrame)
{
    CreateStandardMedia(a_pSFrame);
}

void CFFMpegts::CreateStandardMedia(CSFrame *a_pSFrame)
{
    if (a_pSFrame == NULL)
        return;

    if (m_pParser == NULL)
        return;

    CMpegtsMediaHandler *pMpegtsMediaHandler =
            dynamic_cast<CMpegtsMediaHandler*>(GetStandardMediaHandler());
    if (pMpegtsMediaHandler == NULL) {
        return;
    }

    unsigned char *pData = (unsigned char*)a_pSFrame->GetData();
    for (int i = 0; i < a_pSFrame->GetLength(); i += TS_PACKET_SIZE)
    {
        m_nTotalPackets++;

        m_pParser->ParsePacket(pData + i, TS_PACKET_SIZE);

        UpdatePCRTimestamp(pData + i);
        UpdateVideoTimestamp(pData + i);
        UpdateAudioTimestamp(pData + i);

        pMpegtsMediaHandler->WriteFrames(pData + i, TS_PACKET_SIZE);
    }

    m_nBytesWritten += a_pSFrame->GetLength();
}

void CFFMpegts::UpdateAudioTimestamp(uint8_t *a_pPacket)
{
    if (m_pParser->IsItAudioStream()) {
        if (m_bUpdatedADiscontinuity == false) {
            if (m_pParser->UpdateDiscontinuity(a_pPacket, 1)) {
                m_bUpdatedADiscontinuity = true;
            }
        }

        if (m_pParser->IsPTSPresent()) {
            uint64_t nNewPTSValue = m_pTSAdjusterForAudio->GetAdjustedTS(m_pParser->GetPTSValue());
            m_pParser->UpdatePTSValue(a_pPacket, nNewPTSValue);
        }
    }
}

void CFFMpegts::UpdateVideoTimestamp(uint8_t *a_pPacket)
{
    if (m_bUpdatedVDiscontinuity == false) {
        if (m_pParser->UpdateDiscontinuity(a_pPacket, 1)) {
            m_bUpdatedVDiscontinuity = true;
        }
    }

    if (m_pParser->IsItVideoStream()) {
        if (m_pParser->IsPTSPresent()) {
            uint64_t nNewPTSValue = m_pTSAdjusterForVideo->GetAdjustedTS(m_pParser->GetPTSValue());
            m_pParser->UpdatePTSValue(a_pPacket, nNewPTSValue);
        }

        if (m_pParser->IsDTSPresent()) {
            uint64_t nNewDTSValue = m_pTSAdjusterForVideo->GetAdjustedTS(m_pParser->GetDTSValue());
            m_pParser->UpdateDTSValue(a_pPacket, nNewDTSValue);
        }
    }
}

void CFFMpegts::UpdatePCRTimestamp(uint8_t *a_pPacket)
{
    if (m_pParser->IsPCRFieldPresent()) {
        uint64_t nNewValue = m_pTSAdjusterForPCR->GetAdjustedTS(m_pParser->GetPCRValue());
        m_pParser->UpdatePCRValue(a_pPacket, nNewValue);
    }
}

void CFFMpegts::CompleteStandardMedia()
{
    CMpegtsMediaHandler *pMpegtsMediaHandler =
                dynamic_cast<CMpegtsMediaHandler*>(GetStandardMediaHandler());
    if (pMpegtsMediaHandler == NULL) {
        return;
    }

    pMpegtsMediaHandler->CompleteOutput();
}

void CFFMpegts::StopStream()
{
    CompleteStandardMedia();
}

void CFFMpegts::PrintStats()
{
    char buffer[64];
    printf("Processed: %.2f%% %s\n", m_PercentComplete, GetTimeString(m_nElapsed, buffer));
}
