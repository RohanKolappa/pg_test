/*
 * rtpparser.cpp
 *
 *  Created on: Apr 20, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "RTPParser.hh"


CRTPParser::CRTPParser(unsigned char aPayloadType, bool aIsVideo) {
    m_PayloadType = aPayloadType;
    m_nPacketCount = 0;
    m_nBadPackets = 0;
    m_bIsVideo = aIsVideo;
    m_nTimestamp = 0;
    m_nBeginTimestamp = 0;
    m_nScaledTimestamp = 0;
    m_nScaledBeginTimestamp = 0;
    m_nLastTimestamp = 0;
    m_nTimeResolution = 90000;
    m_fCurrentDrift = 0.0;
    m_fDrift = 0.0;
    m_marker = false;
}

CRTPParser::~CRTPParser() {

}


bool CRTPParser::GetPacket(uint8_t *aInBuffer, int aInSize, AVPacket *avPacket)
{
    do {
        if (aInSize < 12)
            break;
        uint32_t headerBits = ntohl(*(uint32_t *) (aInBuffer));
        aInBuffer += 4;

        // Verify RTP version
        if ((headerBits & 0xC0000000) != 0x80000000)
            break;

        // Verify Payload Type.
        if ((unsigned char) ((headerBits & 0x007F0000) >> 16) != m_PayloadType) {
           // printf("Bad payload %d != %d\n", (headerBits & 0x007F0000) >> 16, m_PayloadType);
            break;
        }

        // Is packet marked?
        m_marker = (headerBits & 0x00800000) >> 23;

        // Get Sequence number
        m_nSeqNumber = (uint16_t) (headerBits & 0xFFFF);

        // Get Timestamp
        m_nTimestamp = ntohl(*(uint32_t*) (aInBuffer));
        aInBuffer += 4;

        // Get SSRC
        m_SSRC = ntohl(*(uint32_t*) (aInBuffer));
        aInBuffer += 4;

        aInSize -= 12;

        // Parse and ignore any CSRC identifiers present
        int nCSRCCount = (headerBits >> 24) & 0xF;
        if (aInSize < nCSRCCount * 4)
            break;
        aInBuffer += nCSRCCount * 4;
        aInSize -= nCSRCCount * 4;

        // Parse and ignore any RTP header extensions
        if (headerBits & 0x10000000) {
            if (aInSize < 4)
                break;
            uint32_t extensionHeader = ntohl(*(uint32_t*) (aInBuffer));
            aInBuffer += 4;
            aInSize -= 4;
            int extensionHeaderLength = 4 * (extensionHeader & 0xFFFF);
            if (aInSize < extensionHeaderLength)
                break;
            aInBuffer += extensionHeaderLength;
            aInSize -= extensionHeaderLength;
        }

        // Strip off padding bytes
        if (headerBits & 0x20000000) {
            if (aInSize == 0)
                break;
            int numPaddingBytes =
                    (aInBuffer)[aInSize - 1];
            if (aInSize < numPaddingBytes)
                break;
            aInSize -= numPaddingBytes;
        }

        // Packet is all good
        m_nPacketCount++;
        EvaluateTimestamp();

        uint32_t timestamp = m_nTimestamp;
        int flags = 0;
        if (m_marker)
            flags |= RTP_HEADER_FLAGS_MARKER;

        av_init_packet(avPacket);

        int rc = Depayload(avPacket, &timestamp, aInBuffer, aInSize, flags);
        if (rc < 0)
            break;
        return true;

    } while (0);

    // Some error
    // printf("Bad packet\n");
    m_nBadPackets++;
    return false;
}

void CRTPParser::RetrieveCodecConfig(MediaSubsession *a_pSubsession,
        AVCodecContext * a_pCodecCtx)
{
    // Base class does nothing (yet)
}

int CRTPParser::Depayload(
        AVPacket * pkt,
        uint32_t * timestamp,
        const uint8_t * buf,
        int len, int flags)
{
    // Default Depayloader just copies the packet
    int ret;
    if ((ret = av_new_packet(pkt, len)) < 0) {
        return ret;
    }

    memcpy(pkt->data, buf, len);
    return 0;
}

void CRTPParser::EvaluateTimestamp()
{

    if (m_nScaledBeginTimestamp == 0) {
        ResetTimestamps();
        return;
    }


    if (m_nTimestamp < m_nLastTimestamp) {
        printf("Rolled over\n");
        ResetTimestamps();
    }

    if (m_nTimestamp == 0) {
        m_nScaledTimestamp = CTimeKeeper::Now();
        return;
    }

    if (m_nTimestamp == m_nLastTimestamp)
        return;

    timestamp_t now = CTimeKeeper::Now();
    const double alpha = 0.05;

    double timeDiff = ((double)(1000.0 * (m_nTimestamp - m_nBeginTimestamp))) /
            m_nTimeResolution;
    m_nScaledTimestamp = m_nScaledBeginTimestamp + (uint64_t) timeDiff + (int64_t)m_fDrift;

    int64_t currentDrift = now - m_nScaledTimestamp;
    m_fCurrentDrift = (double)alpha * currentDrift + (double)(1.0 - alpha) * m_fCurrentDrift;

    // Estimate drift
    if (now - m_nScaledBeginTimestamp >= 1000) {
        double timeDiff = ((double)(1000.0 * (m_nTimestamp - m_nBeginTimestamp))) /
                m_nTimeResolution;
        timestamp_t nScaledTimestamp = m_nScaledBeginTimestamp + (uint64_t) timeDiff;
        int64_t drift = now - nScaledTimestamp;

        m_fDrift = (double)alpha * drift + (double)(1.0 - alpha) * m_fDrift;
    }

//    if ((m_nTimestamp/3600) % 25 == 0)
//        printf("%s Current Drift = %f Computed Drift = %f\n",
//                m_bIsVideo ? "Video" : "Audio",
//                m_fCurrentDrift, m_fDrift);

    m_nLastTimestamp = m_nTimestamp;

}

void CRTPParser::ResetTimestamps() {
    //printf("Timestamp reset\n");
    m_nScaledTimestamp = CTimeKeeper::Now();
    m_nScaledBeginTimestamp = CTimeKeeper::Now();
    m_nBeginTimestamp = m_nTimestamp;
    m_nLastTimestamp = m_nTimestamp;
    m_fCurrentDrift = 0.0;
    m_fDrift = 0.0;
}
