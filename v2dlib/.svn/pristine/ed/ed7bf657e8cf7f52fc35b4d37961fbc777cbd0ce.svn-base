#include "rtp.h"
#include "v2dcommon.h"


static void RTPGetTime(RtpContext_t* a_pRtpContext, struct timeval * timevalp)
{
    (timevalp)->tv_sec = a_pRtpContext->m_nCurrentTime/1000;
    (timevalp)->tv_usec = (a_pRtpContext->m_nCurrentTime % 1000) * 1000;
}

void RTPSetCurrentTime(RtpContext_t* a_pRtpContext, uint64_t currentTime)
{
    a_pRtpContext->m_nCurrentTime = currentTime;
}

int RTPInitContext(RtpContext_t* a_pRtpContext,
                    const unsigned int a_nFrequency,
                    const unsigned int a_nFrameSize,
                    const unsigned int a_nPayloadType)
{

    a_pRtpContext->m_nSequenceNum = (unsigned short)v2dLibGetRandomNumber();
    a_pRtpContext->m_nSSRC = v2dLibGetRandomNumber();
    a_pRtpContext->m_nTimeStamp = v2dLibGetRandomNumber();
    a_pRtpContext->m_nTimeStampPrev = a_pRtpContext->m_nTimeStamp;
    a_pRtpContext->m_nFrequency = a_nFrequency;
    a_pRtpContext->m_nFrameSize = a_nFrameSize;
    a_pRtpContext->m_nPayloadType = a_nPayloadType;
    RTPGetTime(a_pRtpContext, &a_pRtpContext->m_tPrevPacketTime);
    return 0;
}

int RTPInsertHeader(RtpContext_t* a_pRtpContext,
                    unsigned char* a_pBuff,
                    const unsigned int a_bNewFrame)
{
    RtpHdr_t* pRtpHdr = (RtpHdr_t*)a_pBuff;
    unsigned long nRtpTimestamp  = 0;

    struct timeval tCurrTime;

    RTPGetTime(a_pRtpContext, &tCurrTime);

    if (a_pRtpContext->m_nPayloadType == RTPAUDIOPAYLOAD) {
        unsigned int nTimeDiff = tCurrTime.tv_sec
                - a_pRtpContext->m_tPrevPacketTime.tv_sec;

        /* Silence Adjustment */
        if (nTimeDiff >= RTP_SILENCECOMPENSATION_PERIOD) {
            a_pRtpContext->m_nTimeStamp += (unsigned long) (nTimeDiff
                    * a_pRtpContext->m_nFrequency);
        }

        /* Timestamp increment = packetization interval * sampling rate */
        /* packetzn. interval in samples = 1012 B / (2 B/sample * 2 chans) */
        /* 253 samples = 253/16704 secs of audio (16704 = sampling freq) */
        /* secs of audio in 1 pkt * sampling rate */
        a_pRtpContext->m_nTimeStamp += a_pRtpContext->m_nFrameSize;
        a_pRtpContext->m_nTimeStampPrev = a_pRtpContext->m_nTimeStamp;
        nRtpTimestamp = a_pRtpContext->m_nTimeStamp;
    }
    else {
        unsigned int nTimeDiff = 0;
        /* Timestamp should increase if this packet begins with a new frame */
        if (a_bNewFrame) {
            GETTIMEDIFF(tCurrTime, a_pRtpContext->m_tPrevPacketTime, nTimeDiff);
            a_pRtpContext->m_nTimeStamp += nTimeDiff;
            a_pRtpContext->m_nTimeStampPrev = a_pRtpContext->m_nTimeStamp;

            nRtpTimestamp = a_pRtpContext->m_nTimeStamp;
        }
        else {
            nRtpTimestamp = a_pRtpContext->m_nTimeStampPrev;
        }
    }

    pRtpHdr->ph_cc      = 0;
    pRtpHdr->ph_x	    = 0;
    pRtpHdr->ph_p	    = 0;
    pRtpHdr->ph_v	    = RTPVERSION_RFC3550;
    pRtpHdr->ph_pt	    = a_pRtpContext->m_nPayloadType;
    pRtpHdr->ph_m	    = 0;
    pRtpHdr->ph_seq	    = htons(a_pRtpContext->m_nSequenceNum++);
    pRtpHdr->ph_ts	    = htonl(nRtpTimestamp);
    pRtpHdr->ph_ssrc	= htonl(a_pRtpContext->m_nSSRC);

    a_pRtpContext->m_tPrevPacketTime = tCurrTime;
    return 0;
}

void RTPCreateNewSSRC(RtpContext_t* a_pContext)
{
    const unsigned long prevSSRC = a_pContext->m_nSSRC;
    int iMaxIters = 100;

    while(a_pContext->m_nSSRC == prevSSRC && iMaxIters--)
    {
        a_pContext->m_nSSRC = v2dLibGetRandomNumber();
    }

}

