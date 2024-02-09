#include "strmsrv_compatibility.h"
#include "rtputils.hh"

char * WriteAudioPacketIntoBuffer(char* a_pRtpPkt,
                            const int a_nRtpPktLen,
                            char* a_pRevAudioBuffer,
                            int* a_pnBuffLen)
{
    memcpy(a_pRevAudioBuffer + (*a_pnBuffLen), a_pRtpPkt + RTP_HEADER_SIZE,
            a_nRtpPktLen - RTP_HEADER_SIZE);

    *a_pnBuffLen = (*a_pnBuffLen) + a_nRtpPktLen - RTP_HEADER_SIZE;
    return a_pRevAudioBuffer;

}

char* ConvertAudioPacketToFPGA(char* a_pRtpPkt,
                                const int a_nRtpPktLen,
                                char* a_pV2dPkt,
                                int* a_pnV2dPktLen)
{
    uint32_t nSOF = htonl(V2D_AUDIO_SOF_CODE);
    uint32_t nEOF = htonl(V2D_AUDIO_EOF_CODE);

    memcpy(a_pV2dPkt, &nSOF, sizeof(nSOF));
    memset(a_pV2dPkt + 4, 0, 4);
    memcpy(a_pV2dPkt + 8, a_pRtpPkt, a_nRtpPktLen);
    memcpy(a_pV2dPkt + 8 + a_nRtpPktLen, &nEOF, sizeof(nEOF));

    *a_pnV2dPktLen = 12 + a_nRtpPktLen;
    return a_pV2dPkt;
}

char* ConvertAudioPacketToFPGAI50Tx(char* a_pRtpPkt,
                                    const int a_nRtpPktLen,
                                    char* a_pV2dPkt,
                                    int* a_pnV2dPktLen)
{
    memcpy(a_pV2dPkt, a_pRtpPkt, a_nRtpPktLen);

    *a_pnV2dPktLen = a_nRtpPktLen;
    return a_pV2dPkt;
}
char* ConvertAudioPacketFromV2DToRTP(char* a_pV2dPkt,
                                        const int a_nV2dPktLen,
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext)
{
    a_pRtpContext->m_nSequenceNum = (V2DGETSEQUENCENUM(a_pV2dPkt) & 0xFFFF);
    RTPInsertHeader(a_pRtpContext, (unsigned char*)a_pRtpPkt, 0);
    memcpy(a_pRtpPkt + RTP_HEADER_SIZE,
        a_pV2dPkt + V2D_AUDIO_FRAME_DATAOFFSET, V2D_AUDIO_FRAME_SIZE);

    *a_pnRtpPktLen = RTP_AUDIOBUFF_SIZE;
    return a_pRtpPkt;
}

char* ConvertAudioPacketFromV2DToRTPRx(char* a_pV2dPkt,
                                        const int a_nV2dPktLen,
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext)
{
    a_pRtpContext->m_nSequenceNum = (V2DGETSEQUENCENUM(a_pV2dPkt) & 0xFFFF);

    // For V2d -> RTP in Rx we can use the same memory space since a
    // RTP pkt is smaller than V2d. hdr in the front in place of the V2d hdr.
    // The Fpga on RX returns 1024 bytes of data and there is no SOF,
    // Audio Stream id and EOF
    RTPInsertHeader(a_pRtpContext, (unsigned char*)(a_pV2dPkt + 4), 0);
    *a_pnRtpPktLen = RTP_HEADER_SIZE + a_nV2dPktLen - 16;
    return (a_pV2dPkt + 4);
}

char* ConvertAudioPacketFromRTPToV2D(char* a_pRtpPkt,
                                        const int a_nRtpPktLen,
                                        char* a_pV2dPkt,
                                        int* a_pnV2dPktLen)
{
    int nSeqNum = RTPGETSEQUENCENUM(a_pRtpPkt);
    uint32_t nSOF = htonl(V2D_AUDIO_SOF_CODE);
    uint32_t nEOF = htonl(V2D_AUDIO_EOF_CODE);

    memset(a_pV2dPkt, 0, V2D_PACKET_SEQNUM_LEN);
    memcpy(a_pV2dPkt + V2D_PACKET_SEQNUM_LEN, &nSeqNum, sizeof(nSeqNum));
    memset(a_pV2dPkt + V2D_PACKET_SEQNUM_LEN + 4, 0, 4);
    memcpy(a_pV2dPkt + 2 * V2D_PACKET_SEQNUM_LEN, &nSOF, sizeof(nSOF));
    memset(a_pV2dPkt + 2 * V2D_PACKET_SEQNUM_LEN + 4, 0, 4);
    memcpy(a_pV2dPkt + 2 * V2D_PACKET_SEQNUM_LEN + 8,
        a_pRtpPkt + RTP_HEADER_SIZE, a_nRtpPktLen - RTP_HEADER_SIZE);
    memcpy(a_pV2dPkt + 24 + a_nRtpPktLen - RTP_HEADER_SIZE, &nEOF,
        sizeof(nEOF));

    *a_pnV2dPktLen = 28 + a_nRtpPktLen - RTP_HEADER_SIZE;
    return a_pV2dPkt;
}

char* ConvertVideoPacketFromV2DToRTP(char *a_pV2dPkt,
                                        const int a_nV2dPktLen,
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext)
{
    a_pRtpContext->m_nSequenceNum = (V2DGETSEQUENCENUM(a_pV2dPkt) & 0xFFFF);
    RTPInsertHeader(a_pRtpContext, (unsigned char*)a_pRtpPkt, 1);
    memcpy(a_pRtpPkt + RTP_HEADER_SIZE, a_pV2dPkt + V2D_PACKET_HEADER_SIZE,
        SEND_BUFFER_SIZE);

    *a_pnRtpPktLen = RTP_VIDEOBUFF_SIZE;
    return a_pRtpPkt;
}

char* ConvertVideoPacketFromRTPToV2D(char *a_pRtpPkt,
                                        const int a_nRtpPktLen,
                                        char* a_pV2dPkt,
                                        int* a_pnV2dPktLen)
{
    int nSeqNum = RTPGETSEQUENCENUM(a_pRtpPkt);

    memset(a_pV2dPkt, 0, V2D_PACKET_SEQNUM_LEN);
    memcpy(a_pV2dPkt + V2D_PACKET_SEQNUM_LEN, &nSeqNum, sizeof(nSeqNum));
    memset(a_pV2dPkt + V2D_PACKET_SEQNUM_LEN + 4, 0, 4);
    memcpy(a_pV2dPkt + 2 * V2D_PACKET_SEQNUM_LEN,
        a_pRtpPkt + RTP_HEADER_SIZE, SRD_VIDEO_PACKET_SIZE);

    *a_pnV2dPktLen = AV_BUFFER_SIZE;
    return a_pV2dPkt;
}

