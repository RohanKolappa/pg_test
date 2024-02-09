/*
 * CRTPParser.hh
 *
 *  Created on: Apr 20, 2011
 *      Author: rkale
 */

#ifndef RTPPARSER_HH_
#define RTPPARSER_HH_

#include "timekeeper.hh"

class MediaSubsession;
struct AVCodecContext;
struct AVPacket;

#define RTP_HEADER_FLAGS_MARKER  0x01

class CRTPParser {

public:
    CRTPParser(unsigned char aPayloadType, bool aIsVideo);
    virtual ~CRTPParser();


    bool GetPacket(uint8_t *aInBuffer, int aInSize,
            AVPacket * avPacket);

    virtual void RetrieveCodecConfig(MediaSubsession *a_pSubsession,
            AVCodecContext * a_pCodecCtx);

    bool IsMarked() { return m_marker; };
    uint32_t GetRawTimestamp() { return m_nTimestamp; };
    timestamp_t GetTimestamp() { return m_nScaledTimestamp; };
    uint64_t GetPacketErrors() { return m_nBadPackets; };
    uint32_t GetSSRC() { return m_SSRC; }

protected:
    virtual int Depayload(
            AVPacket * pkt,
            uint32_t * timestamp,
            const uint8_t * buf,
            int len, int flags);

private:
    void EvaluateTimestamp();
    void ResetTimestamps();

    unsigned char m_PayloadType;
    bool m_marker;
    uint16_t m_nSeqNumber;
    uint32_t m_nTimestamp;
    uint32_t m_SSRC;
    uint64_t m_nPacketCount;
    uint64_t m_nBadPackets;
    uint32_t m_nBeginTimestamp;
    uint32_t m_nLastTimestamp;
    double m_fCurrentDrift;
    double m_fDrift;

    timestamp_t m_nScaledTimestamp;;
    timestamp_t m_nScaledBeginTimestamp;;
    int m_nTimeResolution;

    bool m_bIsVideo;
};

#endif /* RTPPARSER_HH_ */
