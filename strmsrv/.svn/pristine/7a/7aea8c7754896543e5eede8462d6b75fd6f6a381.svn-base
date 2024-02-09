
#include "strmsrv_compatibility.h"
#include "common.hh"
#include "rtptx.hh"
#include "MulticastUtils.hh"

using namespace std;

/*
  RFC 1889 / RFC 3550 - RTP
  v=2 : RTP version
  p=0 : padding present (may be needed for MPEG2, MPEG4 is always octet aligned frames)
  x=0 : extension header present
  cc=0 : number of csrc's present
  m : mbit
  pt : payload type
  seq=++ : sequence number
  ts = PTS : timestamp
  ssrc= random() : src id
  csrc= NULL : contributing src ids

  RFC 3551 - RTP A/V Profile
  audio/MPA  : pt=14 rate=90000
  video  : rate = 90000
  video/MPV  : pt=32
  video/MP2T : pt=33
  dynamic pt : 96-127
  RTP port : even UDP
  RTCP port : odd UDP

  RFC 2250 - RTP Payload for MPEG1/MPEG2 A/V
  pt = as defined by 3551
  m=1 - discontinuity
  ts = PTS of first frame in packet, repeat on all packets if frag

  Video specific header
  32 bits (=bitfield specifying presence of SeqHeader, etc..)

  Audio specific header
  32 bits (=0 if no fragmentation)

  MIME type spec for Video - per RFC 3555

  MIME type spec for Audio - per RFC 3555

  RFC 3555 - SDP MIME type registrations for A/V
  Audio MPEG1Layer2 (ISO/IEC 11172-3) : audio/MPA
  required params: none
  Video MPEG1/2 ES (ISO/IEC 13818-2)  : video/MPV
  required params: none
  Video MPEG2 TS (ISO/IEC 13818-1)    : video/MP2T
  required params: none

  RFC 3016 - RTP Payload for MPEG4 A/V
  pt = use "dynamic range" (ie 96, 97 etc..)
  m=1 - last/only packet of VOP, also set when multiple VOP in single RTP packet
  ts = PTS of first frame in packet, repeat on all packets if frag

  Video Specific Header : none

  Audio Specific Header : none

  MIME type for Video (ISO/IEC 14496-2) :  video/MP4V_ES
  required params: none
  optional params:
  rate: PTS resolution : default = 90000
  profile_level_id: Table G-1 of ISO/IEC 14496-2 : default = 1 (Simple Profile/Level 1)
  config: Subclause 6.2.1 of ISO/IEC 14469-2
  MIME type for Audio (ISO/IEC 14496-3) :  audio/MP4A-LATM
  required params:
  rate=90000 OR samplerate
  * (cpresent=0 + config) : if not muxed into the RTP stream
  optional params:
  profile_level_id: "Audio Profile Level" from ISO/IEC 14496-1 : default = 30 (Natural Audio Profile/Level 1)
  object: "Object Type ID" from ISO/IEC 14496-3
  bitrate: total stream data rate
  cpresent: is config muxed in the RTP payload : default = 1
  config: "StreamMuxConfig" from ISO/IEC 14496-3

  RFC 3640 - RTP Payload for MPEG4 ES
  pt = use "dynamic range" or specified by RTP Profile
  m=1 : last frag or one/more complete frames in packet
  ts = PTS of first frame in packet

  Video Specific Header : none

  Audio (AAC-hbr) Specific Header :
  AU-headers-len : 16 bits
  AU-header : 1 per audio frame in this packet - following format
  AU-size : 13 bits
  AU-index/index-delta : 3 bits (=0)

  MIME type for Video (ISO/IEC 14496-2) :  video/mpeg4-generic
  required params:
  streamtype : Table 9 of ISO/IEC 14496-1 : 5 for video
  profile-level-id : Table G-1 of ISO/IEC 14496-2 : 0x01 MPEG4-SimpleProfile, 0x0F MPEG2-MP@ML
  config: Subclause 6.2.1 of ISO/IEC 14469-2
  rate: 90000
  mode: generic
  optional params:
  objectType: Table 8 of ISO/IEC 14496-1

  MIME type for Audio (High Bitrate AAC - max frame size = 8191 octets) :  audio/mpeg4-generic
  required params:
  streamtype : Table 9 of ISO/IEC 14496-1 : 6 for audio
  profile-level-id : Table 5 of ISO/IEC 14496-1 : 0x05 MP4-MainNatural, 0x19 MPEG2Audio-Part7 0x1C MPEG1-Audio
  config : "AudioSpecificConfig" from ISO/IEC 14496-3 : AAC-LC/48000/2 = 1190
  rate: sample rate
  channels: number of channels
  mode: AAC-hbr
  sizeLength: 13
  indexLength: 3
  indexDeltaLength: 3
  optional params:
  objectType: Table 8 of ISO/IEC 14496-1

  * AAC Config in ES:
  *
  * AudioObjectType             5 bits   : AAC_LC = 2
  * samplingFrequencyIndex      4 bits   : 48000 = 3
  * if (samplingFrequencyIndex == 0xF)
  *     samplingFrequency       24 bits
  * channelConfiguration        4 bits   : stereo = 2
  * GA_SpecificConfig
  *     FrameLengthFlag         1 bit 1024 or 960
  *     DependsOnCoreCoder      1 bit (always 0)
  *     ExtensionFlag           1 bit (always 0)

  // StreamMuxConfig for LATM : 0x40,0x00,0x23,0x20,0x3f,0xc0
  pConfig[0] = 0x40;
  pConfig[1] = ((profile+1 & 0x10) >> 5);
  pConfig[2] = (((profile + 1) & 0x0f) << 4) | (samplingRateIndex & 0x0f);
  pConfig[3] = (channels << 4);
  pConfig[4] = 0x3f;
  pConfig[5] = 0xc0;


  RFC 2327 - SDP

*/

/*
  Profile:

  #define MP4AV_AAC_MAIN_PROFILE        0
  #define MP4AV_AAC_LC_PROFILE          1
  #define MP4AV_AAC_SSR_PROFILE         2
  #define MP4AV_AAC_LTP_PROFILE         3

  Sampling Rate Index:

  u_int32_t AdtsSamplingRates[NUM_ADTS_SAMPLING_RATES] = {
  96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
  16000, 12000, 11025, 8000, 7350, 0, 0, 0
  };
*/


CRtpTx::CRtpTx (int nId,
                char *pDestination,
                int nDestinationPort,
                int nTTL, int videoTs, int audioTs,
                unsigned long videoFrameRate,
                unsigned long videoFrameRateDivisor,
                int nInterface,
                int bAudioOnly):
    CSender(nId)
{
    m_nPortType = RTPDECODERPORT;
    m_nOffset   = sizeof (rawhdr_t);
    m_nAudioSeq = 0;
    m_nVideoSeq = 0;

    m_nInputVideoSsrc    = 0;
    m_nInputAudioSsrc    = 0;

    m_nInVideoSsrcCount  = 0;
    m_nInAudioSsrcCount  = 0;

    m_nOutVideoSsrcCount = 0;
    m_nOutAudioSsrcCount = 0;

    GenerateNewSSRCs();

    m_nVideoTs = (unsigned long)videoTs;
    m_nAudioTs = (unsigned long)audioTs;

    m_nVideoFrameRate = videoFrameRate;
    m_nVideoFrameRateDivisor = videoFrameRateDivisor;

    m_nAudioPktsSend  = 0;
    m_nAudioBytesSend = 0;
    m_nVideoPktsSend  = 0;
    m_nVideoBytesSend = 0;

    m_nWisRawCount = 0;
    m_nCodecMP2VCount = 0;
    m_nCodecMP4VCount = 0;
    m_nCodecMPACount = 0;
    m_nCodecAACCount = 0;

    m_pcModifiedFrame   = NULL;
    m_nModifiedFrameLen = 0;

    m_nDiscontinuities  = 0;

    if((m_nVideoTs < MIN_VIDEOTS) || (m_nAudioTs < MIN_AUDIOTS)) {
        SetError();
        SYSLOG_ERROR("Clock Rate less than minimum value m_nVideoTs %lo m_nAudioTs %lo\n",
                m_nVideoTs, m_nAudioTs);
    }

    m_pRtpBuffer        = new char[sizeof(rtp3016_t) + RTPPKTSIZE];
    memset (m_pRtpBuffer, 0, sizeof(rtp3016_t) + RTPPKTSIZE);

    /* Setup networking */
    m_bAudioOnly = bAudioOnly ? true : false;
    if (!bAudioOnly) {
        if (m_VideoDestination.SetAddress(pDestination, nDestinationPort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                    pDestination, nDestinationPort);
        }

        if (m_AudioDestination.SetAddress(pDestination, nDestinationPort + 2) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                    pDestination, nDestinationPort + 2);
        }
        CMulticastUtils::RegisterMulticastSendInterface(m_socket, m_VideoDestination,
                nInterface, m_sInterfaceInfo);
    }
    else {
        if (m_AudioDestination.SetAddress(pDestination, nDestinationPort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                    pDestination, nDestinationPort);
        }
        CMulticastUtils::RegisterMulticastSendInterface(m_socket, m_AudioDestination,
                nInterface, m_sInterfaceInfo);
    }


    /* Set the multicast TTL */
    m_socket.SetMcastTTL(nTTL);

}

CRtpTx::~CRtpTx()
{
    delete [] m_pRtpBuffer;
    delete [] m_pcModifiedFrame;
    return;
}

int CRtpTx::Mpeg4FindVopOffset (uint8_t *pBuf, uint32_t buflen)
{
    uint8_t *p = pBuf;
    while (buflen > 4) {
        if (p[0] == 0x0 &&
            p[1] == 0x0 &&
            p[2] == 0x1 &&
            p[3] == MPEG4_VOP_START) {
            return (p - pBuf);
        }
        p++;
        buflen--;
    }
    return 0;
}

// per RFC 3640
int CRtpTx::SendAudioAACFrame(char *pData, int nLen, unsigned long ts)
{
    int nRtpHdrSize, nPayloadHdrSize, nPayloadLen;
    int retval=0;

    rtp3016_t *pRtpHdr=NULL;
    pRtpHdr = (rtp3016_t *)m_pRtpBuffer;

    // Set RTP headers
    pRtpHdr->ph_cc = 0;
    pRtpHdr->ph_x = 0;
    pRtpHdr->ph_p = 0;
    pRtpHdr->ph_v = 2;
    pRtpHdr->ph_pt = 97;  // Payload Type: use dynamic range
    /*
      pRtpHdr->ph_ts = ts;
      // set Timestamp rate = to sample rate (hard coded to 48K for now)
      pRtpHdr->ph_ts = ntohl(pRtpHdr->ph_ts);
      pRtpHdr->ph_ts = pRtpHdr->ph_ts / 90 * 48;
      pRtpHdr->ph_ts = htonl(pRtpHdr->ph_ts);
    */
    //pRtpHdr->ph_ts = htonl(ts * 48);
    pRtpHdr->ph_ts = htonl(ts * (m_nAudioTs/1000));
    pRtpHdr->ph_ssrc = htonl(m_nAudioSsrc);

    nRtpHdrSize = sizeof(rtp3016_t);

    //Set Payload specific headers
    m_pRtpBuffer[nRtpHdrSize] = 0x00;
    m_pRtpBuffer[nRtpHdrSize + 1] = 16;
    m_pRtpBuffer[nRtpHdrSize + 2] = nLen >> 5;
    m_pRtpBuffer[nRtpHdrSize + 3] = (nLen & 0x1f) << 3;
    nPayloadHdrSize = 4;

    // send out packet payload
    // for now just send out one AAC frame as one RTP packet
    // need to look into sending more than one packet in one RTP packet
    while( nLen ) {
        // inc seq number
        pRtpHdr->ph_seq = htons(m_nAudioSeq++);
        // reset marker bit
        pRtpHdr->ph_m = 0;
        // calc usable payload len
        nPayloadLen = RTPPKTSIZE - nPayloadHdrSize;
        if( nLen <= nPayloadLen ) {
            nPayloadLen = nLen;
            pRtpHdr->ph_m = 1;  // mark last frag/single pkt with mbit=1
        }
        // copy payload to rtp buffer
        memcpy (m_pRtpBuffer + nRtpHdrSize + nPayloadHdrSize,
                pData, nPayloadLen);
        // send it off
        retval = m_socket.Send(m_pRtpBuffer,
                               nRtpHdrSize + nPayloadHdrSize + nPayloadLen,
                               m_AudioDestination);

        if (retval <= 0) break;

        m_nAudioPktsSend++;
        m_nAudioBytesSend += nPayloadLen;
        pData += nPayloadLen;
        nLen -= nPayloadLen;
    }

    return retval;
}

// per RFC 2250
int CRtpTx::SendAudioMPAFrame(char *pData, int nLen, unsigned long ts)
{
    int nRtpHdrSize, nPayloadHdrSize, nPayloadLen;
    int retval=0;

    rtp3016_t *pRtpHdr=NULL;
    pRtpHdr = (rtp3016_t *)m_pRtpBuffer;

    // Set RTP headers
    pRtpHdr->ph_cc = 0;
    pRtpHdr->ph_x = 0;
    pRtpHdr->ph_p = 0;
    pRtpHdr->ph_v = 2;
    pRtpHdr->ph_pt = 14;  // Payload Type: 14
    //  pRtpHdr->ph_pt = 97;
    //  pRtpHdr->ph_ts = ts;

    //pRtpHdr->ph_ts = htonl(ts * 90);
    pRtpHdr->ph_ts = htonl(ts * (m_nAudioTs/1000));
    pRtpHdr->ph_ssrc = htonl(m_nAudioSsrc);

    nRtpHdrSize = sizeof(rtp3016_t);

    //Set Payload specific headers
    m_pRtpBuffer[nRtpHdrSize] = 0x00;
    m_pRtpBuffer[nRtpHdrSize + 1] = 0x00;
    m_pRtpBuffer[nRtpHdrSize + 2] = 0x00;
    m_pRtpBuffer[nRtpHdrSize + 3] = 0x00;
    nPayloadHdrSize = 4;

    // send out packet payload
    // not sure if frag into multiple RTP packets is legal
    // should not matter for us - we come here with 576 byte audio packet - will go out as one RTP packet
    // need to look into sending more than one packet in one RTP packet - is that legal?
    while( nLen ) {
        // inc seq number
        pRtpHdr->ph_seq = htons(m_nAudioSeq++);
        // reset marker bit
        pRtpHdr->ph_m = 0;
        // calc usable payload len
        nPayloadLen = RTPPKTSIZE - nPayloadHdrSize;
        if( nLen <= nPayloadLen ) {
            nPayloadLen = nLen;
            pRtpHdr->ph_m = 1;  // mark last frag/single pkt with mbit=1
        }
        // copy payload to rtp buffer
        memcpy (m_pRtpBuffer + nRtpHdrSize + nPayloadHdrSize,
                pData, nPayloadLen);
        // send it off
        retval = m_socket.Send(m_pRtpBuffer,
                               nRtpHdrSize + nPayloadHdrSize + nPayloadLen,
                               m_AudioDestination);

        if (retval <= 0) break;

        // bump counters
        m_nAudioPktsSend++;
        m_nAudioBytesSend += nPayloadLen;
        pData += nPayloadLen;
        nLen -= nPayloadLen;
    }

    return retval;
}

// per RFC 3016
int CRtpTx::SendVideoMP4Frame(char *pData, int nLen, unsigned long ts)
{
    int nRtpHdrSize, nPayloadHdrSize, nPayloadLen;
    int retval=0;

    if (m_bAudioOnly)
        return 0;

    rtp3016_t *pRtpHdr=NULL;
    pRtpHdr = (rtp3016_t *)m_pRtpBuffer;

    // Set RTP headers
    pRtpHdr->ph_cc = 0;
    pRtpHdr->ph_x = 0;
    pRtpHdr->ph_p = 0;
    pRtpHdr->ph_v = 2;
    pRtpHdr->ph_pt = 96;  // Payload Type: use dynamic range
    //  pRtpHdr->ph_ts = ts;

    //pRtpHdr->ph_ts = htonl(ts * 90);
    pRtpHdr->ph_ts = htonl(ts * (m_nVideoTs/1000));
    pRtpHdr->ph_ssrc = htonl(m_nVideoSsrc);

    nRtpHdrSize = sizeof(rtp3016_t);

    //Set Payload specific headers
    // = none for mpeg4-video
    nPayloadHdrSize = 0;

    //Locate start of VOP header and strip off everything before it
    /*
      int nStartOff=0;
      nStartOff = Mpeg4FindVopOffset((uint8_t *)pData, nLen);
      pData += nStartOff;
      nLen -= nStartOff;
    */
    // send out packet payload - frag into multiple RTP packets if needed
    // no need to check for padding - input frame payload should be octet aligned
    while( nLen ) {
        // inc seq number
        pRtpHdr->ph_seq = htons(m_nVideoSeq++);
        // reset marker bit
        pRtpHdr->ph_m = 0;
        // calc usable payload len
        nPayloadLen = RTPPKTSIZE - nPayloadHdrSize;
        if( nLen <= nPayloadLen ) {
            nPayloadLen = nLen;
            pRtpHdr->ph_m = 1;  // mark last frag/single pkt with mbit=1
        }
        // copy payload to rtp buffer
        memcpy (m_pRtpBuffer + nRtpHdrSize + nPayloadHdrSize,
                pData, nPayloadLen);
        // send it off
        retval = m_socket.Send(m_pRtpBuffer,
                               nRtpHdrSize + nPayloadHdrSize + nPayloadLen,
                               m_VideoDestination);

        if (retval <= 0) break;

        // bump counters
        m_nVideoPktsSend++;
        m_nVideoBytesSend += nPayloadLen;
        pData += nPayloadLen;
        nLen -= nPayloadLen;
    }

    return retval;
}

int CRtpTx::SendAudioMP4LATMFrame(char *pData, int nLen, unsigned long ts)
{
    int nRtpHdrSize, nPayloadHdrSize, nPayloadLen;
    int retval=0;

    rtp3016_t *pRtpHdr=NULL;
    pRtpHdr = (rtp3016_t *)m_pRtpBuffer;

    // Set RTP headers
    pRtpHdr->ph_cc = 0;
    pRtpHdr->ph_x = 0;
    pRtpHdr->ph_p = 0;
    pRtpHdr->ph_v = 2;
    pRtpHdr->ph_pt = 97;  // Payload Type: use dynamic range

    //  pRtpHdr->ph_ts = ts;
    /* pRtpHdr->ph_ts = htonl(ts * 90);
    // set Timestamp rate = to sample rate (hard coded to 48K for now)
    pRtpHdr->ph_ts = ntohl(pRtpHdr->ph_ts);
    pRtpHdr->ph_ts = pRtpHdr->ph_ts / 90 * 48;
    pRtpHdr->ph_ts = htonl(pRtpHdr->ph_ts);*/

    pRtpHdr->ph_ts = htonl(ts * (m_nAudioTs/1000));

    pRtpHdr->ph_ssrc = htonl(m_nAudioSsrc);

    nRtpHdrSize = sizeof(rtp3016_t);

    //Set Payload specific headers
    // = LATM headers for mpeg4-audio
    nPayloadHdrSize = (nLen / 255) + 1;
    for (uint8_t j=0; j<nPayloadHdrSize; j++) {
        m_pRtpBuffer[nRtpHdrSize + j] = 255;
    }
    m_pRtpBuffer[nRtpHdrSize + nPayloadHdrSize -1] = nLen % 255;

    // send out packet payload - frag into multiple RTP packets if needed
    // no need to check for padding - input frame payload should be octet aligned
    while( nLen ) {
        // inc seq number
        pRtpHdr->ph_seq = htons(m_nAudioSeq++);
        // reset marker bit
        pRtpHdr->ph_m = 0;
        // calc usable payload len
        nPayloadLen = RTPPKTSIZE - nPayloadHdrSize;
        if( nLen <= nPayloadLen ) {
            nPayloadLen = nLen;
            pRtpHdr->ph_m = 1;  // mark last frag/single pkt with mbit=1
        }
        // copy payload to rtp buffer
        memcpy (m_pRtpBuffer + nRtpHdrSize + nPayloadHdrSize,
                pData, nPayloadLen);
        // send it off
        retval = m_socket.Send(m_pRtpBuffer,
                               nRtpHdrSize + nPayloadHdrSize + nPayloadLen,
                               m_AudioDestination);

        if (retval <= 0) break;

        // bump counters
        m_nAudioPktsSend++;
        m_nAudioBytesSend += nPayloadLen;
        pData += nPayloadLen;
        nLen -= nPayloadLen;
    }

    return retval;
}

int
CRtpTx::ProcessStream()
{
    int retval = 0;

    if (m_qQueueSource == NULL)
        return 0;

    CSFrame *sFrame = m_qQueueSource->GetFrame();

    if (sFrame) {
        if (sFrame->HasDiscontinuity()) {
            GenerateNewSSRCs();
            m_nDiscontinuities++;
        }

        char * pData = sFrame->GetData();

        int nLen = sFrame->GetLength();

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        if (sFrame->GetMessageType() == CSFrame::Normal) {

            unsigned long nStreamType = sFrame->GetStreamType();

            if (nStreamType == STREAMTYPE_RTP) {
                CRTPStreamMediaFlag rtpSMediaFlag(sFrame->GetMediaFlag());
                switch(rtpSMediaFlag.GetEncaps()) {
                case FRAMETYPE_RTP_ENCAPS_RTP:
                    switch (rtpSMediaFlag.GetChannel()) {
                    case FRAMETYPE_RTP_CHAN_VIDEO:

                        HandleInputSSRCChange(pData, nLen, FRAMETYPE_RTP_CHAN_VIDEO);
                        ModifyFrameSSRC(pData, nLen, FRAMETYPE_RTP_CHAN_VIDEO);

                        if (m_bAudioOnly)
                            break;

                        retval = m_socket.Send(m_pcModifiedFrame, nLen,
                                            m_VideoDestination);
                        if (retval > 0) {
                            m_nVideoPktsSend++;
                            m_nVideoBytesSend += nLen;
                        }
                        break;

                    case FRAMETYPE_RTP_CHAN_AUDIO:

                        HandleInputSSRCChange(pData, nLen, FRAMETYPE_RTP_CHAN_AUDIO);
                        ModifyFrameSSRC(pData, nLen, FRAMETYPE_RTP_CHAN_AUDIO);
                        retval = m_socket.Send(m_pcModifiedFrame, nLen,
                                            m_AudioDestination);
                        if (retval > 0) {
                            m_nAudioPktsSend++;
                            m_nAudioBytesSend += nLen;
                        }
                        break;

                    default:

                        m_errors++;
                        break;
                    }
                    break;
                default:
                    m_errors++;
                    break;
                }
            }
            else if (nStreamType == STREAMTYPE_WIS) {
                CWISStreamMediaFlag wisSMediaFlag(sFrame->GetMediaFlag());
                unsigned long nTimestamp = (unsigned long)m_nTimestamp;
                short nCodec = wisSMediaFlag.GetCodec();
                switch(wisSMediaFlag.GetEncaps()) {
                case FRAMETYPE_WIS_ENCAPS_RAW:
                    m_nWisRawCount++;
                    switch (wisSMediaFlag.GetChannel()) {
                    case (FRAMETYPE_WIS_CHAN_VIDEO):
                        switch (nCodec) {
                        case (FRAMETYPE_WIS_CODEC_MP2V):
                            m_nCodecMP2VCount++;
                            break;
                        case (FRAMETYPE_WIS_CODEC_MP4V):
                            m_nCodecMP4VCount++;
                            SendVideoMP4Frame(pData, nLen, nTimestamp);
                            break;
                        default:
                            m_errors++;
                            break;
                        }
                        break;
                    case (FRAMETYPE_WIS_CHAN_AUDIO):
                        switch (nCodec) {
                        case (FRAMETYPE_WIS_CODEC_MPA):
                            m_nCodecMPACount++;
                            SendAudioMPAFrame(pData, nLen, nTimestamp);
                            break;
                        case (FRAMETYPE_WIS_CODEC_AAC):
                            m_nCodecAACCount++;
                            SendAudioAACFrame(pData, nLen, nTimestamp);
                            break;
                        default:
                            m_errors++;
                            break;
                        }
                        break;
                    default:
                        m_errors++;
                        break;
                    }
                    break;
                default:
                    m_errors++;
                    break;
                }
            }
            m_frameCount++;
            m_bytesTransfered += sFrame->GetLength();
        }

        m_qQueueSource->ReleaseFrame(sFrame);
        retval = 1;


    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0) {
            CheckQueueTimestamp();
        }
    }

    CheckSyncStop();

    return retval;
}

void
CRtpTx::UpdateCustomStatus(CCommand *a_pCmd)
{

    a_pCmd->SetObjectParam("Video Clockrate",
                           (unsigned long)m_nVideoTs);
    a_pCmd->SetObjectParam("Audio Clockrate",
                           (unsigned long)m_nAudioTs);
    a_pCmd->SetObjectParam("VideoDestination",
                           m_VideoDestination.GetHost());
    a_pCmd->SetObjectParam("AudioDestination",
                           m_AudioDestination.GetHost());
    a_pCmd->SetObjectConstantParam("VideoPort",
                                   m_VideoDestination.GetPort());
    a_pCmd->SetObjectConstantParam("AudioPort",
                                   m_AudioDestination.GetPort());
    a_pCmd->SetObjectParam("Multicast", (bool)m_VideoDestination.IsMulticast());
    a_pCmd->SetObjectParam("MulticastInterface", m_sInterfaceInfo);

    a_pCmd->SetObjectCounterParam("VideoPackets", m_nVideoPktsSend);
    a_pCmd->SetObjectCounterParam("AudioPackets", m_nAudioPktsSend);
    a_pCmd->SetObjectByteCounterParam("VideoBytes", m_nVideoBytesSend);
    a_pCmd->SetObjectByteCounterParam("AudioBytes", m_nAudioBytesSend);
    a_pCmd->SetObjectCounterParam("WisRawCount", m_nWisRawCount);
    a_pCmd->SetObjectCounterParam("CodecMP2VCount",m_nCodecMP2VCount);
    a_pCmd->SetObjectCounterParam("CodecMP4VCount",m_nCodecMP4VCount);
    a_pCmd->SetObjectCounterParam("CodecMPACount",m_nCodecMPACount);
    a_pCmd->SetObjectCounterParam("CodecAACCount",m_nCodecAACCount);
    a_pCmd->SetObjectCounterParam("FrameDiscontinuities",m_nDiscontinuities);

    if (a_pCmd->StatusLevel(2)) {
        a_pCmd->SetObjectParam("InputVideoSSRC", m_nInputVideoSsrc);
        a_pCmd->SetObjectParam("InputAudioSSRC", m_nInputAudioSsrc);
        a_pCmd->SetObjectParam("OutputVideoSSRC", m_nVideoSsrc);
        a_pCmd->SetObjectParam("OutputAudioSSRC", m_nAudioSsrc);

        a_pCmd->SetObjectCounterParam("InputVideoSSRCCount", m_nInVideoSsrcCount);
        a_pCmd->SetObjectCounterParam("InputAudioSSRCCount", m_nInAudioSsrcCount);

        a_pCmd->SetObjectCounterParam("OutputVideoSSRCCount", m_nOutVideoSsrcCount);
        a_pCmd->SetObjectCounterParam("OutputAudioSSRCCount", m_nOutAudioSsrcCount);
    }
}

bool CRtpTx::ModifyFrameSSRC(const char* a_pcFrame, const int a_nFrameLen,
                            const short a_nFrameType)
{
    if(a_nFrameLen > m_nModifiedFrameLen)
    {
        delete [] m_pcModifiedFrame;
        m_pcModifiedFrame = new char[a_nFrameLen];
        m_nModifiedFrameLen = a_nFrameLen;
    }

    ::memcpy(m_pcModifiedFrame, a_pcFrame, a_nFrameLen);
    rtp3016_t *pRtpHdr = reinterpret_cast<rtp3016_t *>(m_pcModifiedFrame);
    pRtpHdr->ph_ssrc   = htonl((a_nFrameType == FRAMETYPE_RTP_CHAN_AUDIO)
            ? m_nAudioSsrc : m_nVideoSsrc);

    return true;
}

#define SECURENEWSSRC(NSSRC) \
{ \
    unsigned long nRandom = random(); \
    if ( (nRandom == 0) || (nRandom == (NSSRC)) ) \
    { \
        (NSSRC) += 100; \
    } \
    else \
    { \
        (NSSRC) = nRandom; \
    } \
}

void CRtpTx::GenerateNewSSRCs()
{
    SECURENEWSSRC(m_nAudioSsrc);
    SECURENEWSSRC(m_nVideoSsrc);

    m_nOutVideoSsrcCount++;
    m_nOutAudioSsrcCount++;
}

void CRtpTx::HandleInputSSRCChange(const char *pData, const int a_nFrameLen,
                                   const short a_nFrameType)
{
    if (a_nFrameLen < 12)
        return;

    unsigned long nCurrentSSRC = ntohl(*(uint32_t*) (pData + 8));

    if (a_nFrameType == FRAMETYPE_RTP_CHAN_AUDIO) {
        HandleAudioInputSSRCChange(nCurrentSSRC);
    }
    else {
        HandleVideoInputSSRCChange(nCurrentSSRC);
    }
}

void CRtpTx::HandleVideoInputSSRCChange(unsigned long a_nCurrentSSRC)
{
    if (m_nInputVideoSsrc == 0) {
        m_nInputVideoSsrc = a_nCurrentSSRC;

        return;
    }

    if (a_nCurrentSSRC != m_nInputVideoSsrc) {
        SECURENEWSSRC(m_nVideoSsrc);

        m_nInputVideoSsrc = a_nCurrentSSRC;

        m_nInVideoSsrcCount++;
        m_nOutVideoSsrcCount++;
    }
}

void CRtpTx::HandleAudioInputSSRCChange(unsigned long a_nCurrentSSRC)
{
    if (m_nInputAudioSsrc == 0) {
        m_nInputAudioSsrc = a_nCurrentSSRC;

        return;
    }

    if (a_nCurrentSSRC != m_nInputAudioSsrc) {
        SECURENEWSSRC(m_nAudioSsrc);

        m_nInputAudioSsrc = a_nCurrentSSRC;

        m_nInAudioSsrcCount++;
        m_nOutAudioSsrcCount++;
    }
}

int CRtpTx::StartSenderStream(int nQueueId)
{
    GenerateNewSSRCs();

    return 0;
}
