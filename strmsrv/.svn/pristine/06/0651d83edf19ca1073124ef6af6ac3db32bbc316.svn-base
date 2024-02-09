/*
 * FFV2dTrack.cpp
 *
 *  Created on: Jul 26, 2012
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "squeue.hh"

#include "FFV2dTrack.hh"
#include "FFV2d.hh"
#include "ratecontroller.hh"
#include "rtp.h"


CFFV2dTrack::CFFV2dTrack(CFFV2d * pFFV2d, int trackIndex)
: m_RTPContext()
{
    m_pFFV2d = pFFV2d;
    m_nTrackIndex = trackIndex;

    m_nTokenMeter = 0;
    m_nUnavailable = 0;
    m_nScapOutFrames = 0;
    m_nScapWriteTimestamp = 0;
    m_nBytesWritten = 0;
    m_nFramesWritten = 0;
    m_nLastFrameCount = 0;

    // Scale the scap writing frequncy
    m_nScapWriteFrequency = (5000 * m_pFFV2d->GetTrackSpeed(m_nTrackIndex)) / 2;

    double fRate = 30.0;
    if (m_pFFV2d->GetTrackSpeed(m_nTrackIndex) > 1)
        fRate = fRate/m_pFFV2d->GetTrackSpeed(m_nTrackIndex);
    m_pRateController = new CRateController(fRate);
}

CFFV2dTrack::~CFFV2dTrack()
{
    delete m_pRateController;
}

void CFFV2dTrack::Init()
{
    m_nScapWriteTimestamp = 0;

    RTPSetCurrentTime(&m_RTPContext, m_pFFV2d->GetCurrentTimeStamp());
    RTPInitContext(&m_RTPContext, RTPAUDIOCLOCKFREQ,
                    0, RTPVIDEOPAYLOAD);
}

void CFFV2dTrack::AdjustTokenMeter(int value) {
    m_nTokenMeter += value;
}

void CFFV2dTrack::ResetScap()
{
    m_nScapWriteTimestamp = 0;
}

uint64_t CFFV2dTrack::GetVideoFrameCount()
{
    return m_nLastFrameCount;
}

void CFFV2dTrack::WriteScap() {

    if (m_pFFV2d->GetScapBufferSize() == 0)
        return;

    if (m_nScapWriteTimestamp == 0 ||
            (m_pFFV2d->GetCurrentTimeStamp() - m_nScapWriteTimestamp) >= m_nScapWriteFrequency) {

        m_nScapWriteTimestamp = m_pFFV2d->GetCurrentTimeStamp();
        m_nScapOutFrames++;

        CV2DStreamMediaFlag v2dSMediaFlag;
        v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

        CSFrame *sFrame = new CSFrame((const char *)m_pFFV2d->GetScapBuffer(),
                m_pFFV2d->GetScapBufferSize(),
                v2dSMediaFlag.GetMediaFlag(),
                m_pFFV2d->GetCurrentTimeStamp());
        m_pFFV2d->GetFFQueueSinkAt(m_nTrackIndex)->InsertFrame(sFrame);
    }
}

bool CFFV2dTrack::CheckBandwidth()
{
    if (m_nTokenMeter < m_pFFV2d->GetTrackSpeed(m_nTrackIndex))
        return false;
    return true;
}

bool CFFV2dTrack::CheckFrameRate()
{
    uint64_t frameCount;
    if (m_nTrackIndex == 0)
        frameCount = m_pFFV2d->GetSRD()->outFrameCount;
    else
        frameCount = m_pFFV2d->GetSRD()->outFrameCountPerTrack[m_nTrackIndex];

    if (frameCount > m_nLastFrameCount) {
        if (!m_pRateController->Allow(m_pFFV2d->GetCurrentTimeStamp() * 10)) {
            return false;
        }
        m_nLastFrameCount = frameCount;
    }

    return true;
}

void CFFV2dTrack::ProcessSRDFrame()
{
    WriteScap();

    if (m_pFFV2d->GetSRD() == NULL)
        return;

    if (!CheckBandwidth())
        return;

    if (!CheckFrameRate())
        return;

    unsigned char pSRDInPacket[SIZE_OF_AV_PACKET];
    const int nDataOffset = RTP_HEADER_SIZE;

    const int needPadding = 1;
    int len = getPacketsForTrack(m_pFFV2d->GetSRD(), pSRDInPacket + nDataOffset,
            SEND_BUFFER_SIZE, needPadding, m_nTrackIndex);
    if (len == -1) {
        m_nUnavailable++;
        return;
    }

    m_nTokenMeter -= m_pFFV2d->GetTrackSpeed(m_nTrackIndex);

    RTPSetCurrentTime(&m_RTPContext, m_pFFV2d->GetCurrentTimeStamp());
    RTPInsertHeader(&m_RTPContext, pSRDInPacket, 1);

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetEncaps(FRAMETYPE_V2D_ENCAPS_RTP);
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_VIDEO);

    CSFrame *sFrame = new CSFrame((const char *)pSRDInPacket,
            RTP_HEADER_SIZE + SEND_BUFFER_SIZE,
            v2dSMediaFlag.GetMediaFlag(),
            m_pFFV2d->GetCurrentTimeStamp());
    m_nBytesWritten += sFrame->GetLength();
    m_pFFV2d->GetFFQueueSinkAt(m_nTrackIndex)->InsertFrame(sFrame);
    m_nFramesWritten++;
}

