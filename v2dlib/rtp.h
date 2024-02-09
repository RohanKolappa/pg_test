#ifndef __HH_V2DLIB_RTP_H_
#define __HH_V2DLIB_RTP_H_

#include "v2d.h"

#define RTPVERSION_RFC3550  2
#define RTP_HEADER_SIZE     12
#define RTP_SEQUENCENUM_POS 2
#define RTP_TIMESTAMP_POS   4
#define RTP_SSRC_POS        8
#define RTP_AUDIOBUFF_SIZE  1024
#define RTP_VIDEOBUFF_SIZE  1036

#define RTPMAXPKTSIZE		1460
#define RTPAUDIOPAYLOAD		96
#define RTPVIDEOPAYLOAD		97
#define RTPAUDIOCLOCKFREQ   16315

#define RTP_SILENCECOMPENSATION_PERIOD 2 /* secs */

#define RTPGETSEQUENCENUM(PTR_RTPPACKET) \
    ntohs( *(unsigned short*)((PTR_RTPPACKET) + RTP_SEQUENCENUM_POS) )

#define RTPGETTIMESTAMP(PTR_RTPPACKET) \
    ntohl( *(unsigned long*)((PTR_RTPPACKET) + RTP_TIMESTAMP_POS) )

#define RTPGETSSRC(PTR_RTPPACKET) \
    ntohl( *(unsigned long*)((PTR_RTPPACKET) + RTP_SSRC_POS) )

#define RTPAPPLYNEWSSRC(PTR_RTPPACKET, NSSRC) \
{ \
    RtpHdr_t* pRtpHdr = (RtpHdr_t*)(PTR_RTPPACKET); \
    pRtpHdr->ph_ssrc  = htonl(NSSRC); \
}

typedef struct RtpHeader {
    unsigned short        ph_cc:4;
    unsigned short        ph_x:1;
    unsigned short        ph_p:1;
    unsigned short        ph_v:2;
    unsigned short        ph_pt:7;
    unsigned short        ph_m:1;
    unsigned short        ph_seq;
    unsigned long         ph_ts;
    unsigned long         ph_ssrc;
} RtpHdr_t;

typedef struct RtpContext {
    uint16_t	m_nSequenceNum;
    uint32_t	m_nSSRC;
    uint32_t	m_nTimeStamp;
    uint32_t	m_nTimeStampPrev;
    int32_t	m_nFrequency;
    int32_t	m_nFrameSize;
    int32_t	m_nPayloadType;
    uint64_t        m_nCurrentTime;
    struct timeval  m_tPrevPacketTime;
} RtpContext_t;

#ifdef __cplusplus
extern "C" {
#endif

int RTPInitContext(RtpContext_t* a_pRtpContext,
                    const unsigned int a_nFrequency,
                    const unsigned int a_nFrameSize,
                    const unsigned int a_nPayloadType);

int RTPInsertHeader(RtpContext_t* a_pRtpContext,
                    unsigned char* a_pBuff,
                    const unsigned int a_bNewFrame);

void RTPCreateNewSSRC(RtpContext_t* a_pContext);
void RTPSetCurrentTime(RtpContext_t* a_pRtpContext, uint64_t currentTime);

#ifdef __cplusplus
}
#endif

#endif
