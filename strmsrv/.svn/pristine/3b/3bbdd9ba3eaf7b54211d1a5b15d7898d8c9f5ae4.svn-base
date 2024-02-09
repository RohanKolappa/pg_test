/*
 * MPEG4LATMAudioRTPParser.cpp
 *
 *  Created on: May 12, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "bitstream.h"
#include "MPEG4LATMAudioRTPParser.hh"


static int hex_to_data(uint8_t *data, const char *p)
{
    int c, len, v;

    len = 0;
    v = 1;
    for (;;) {
        p += strspn(p, " \t\r\n");
        if (*p == '\0')
            break;
        c = toupper((unsigned char) *p++);
        if (c >= '0' && c <= '9')
            c = c - '0';
        else if (c >= 'A' && c <= 'F')
            c = c - 'A' + 10;
        else
            break;
        v = (v << 4) | c;
        if (v & 0x100) {
            if (data)
                data[len] = v;
            len++;
            v = 1;
        }
    }
    return len;
}


CMPEG4LATMAudioRTPParser::CMPEG4LATMAudioRTPParser(unsigned char aPayloadType)
    : CRTPParser(aPayloadType, false) {

    m_pBuffer = NULL;
    m_dynamicBuffer = NULL;
    m_nCurrentTimestamp = 0;
    m_nPosition = 0;
    m_nLength = 0;
}

CMPEG4LATMAudioRTPParser::~CMPEG4LATMAudioRTPParser()
{
    AVIOContext *dynamicBuffer = (AVIOContext *) m_dynamicBuffer;

    if (dynamicBuffer) {
        uint8_t *p;
        url_close_dyn_buf(dynamicBuffer, &p);
        av_free(p);
    }

    av_free(m_pBuffer);
}

void CMPEG4LATMAudioRTPParser::RetrieveCodecConfig(MediaSubsession *a_pSubsession,
        AVCodecContext * a_pCodecCtx)
{

    if (a_pSubsession == NULL || a_pCodecCtx == NULL)
        return;

    if (a_pSubsession->fmtp_config() != NULL) {
        int len = hex_to_data(NULL, a_pSubsession->fmtp_config());
        av_free(a_pCodecCtx->extradata);
        a_pCodecCtx->extradata = (uint8_t *) av_mallocz(len + FF_INPUT_BUFFER_PADDING_SIZE);
        if (a_pCodecCtx->extradata != NULL) {
            a_pCodecCtx->extradata_size = len;
            hex_to_data(a_pCodecCtx->extradata, a_pSubsession->fmtp_config());
        }
    }

}


int CMPEG4LATMAudioRTPParser::Depayload(
        AVPacket * pkt,
        uint32_t * timestamp,
        const uint8_t * buf,
        int len, int flags)
{

#if 1
    AVIOContext *dynamicBuffer = (AVIOContext *) m_dynamicBuffer;

    int ret, cur_len;

    if (buf) {
        if (!dynamicBuffer || m_nCurrentTimestamp != *timestamp) {
            //printf("Need new buffers %d\n", m_nCurrentTimestamp);
            av_freep(&m_pBuffer);
            if (dynamicBuffer) {
                url_close_dyn_buf(dynamicBuffer, &m_pBuffer);
            }
            dynamicBuffer = NULL;
            m_dynamicBuffer = NULL;

            av_freep(&m_pBuffer);

            m_nCurrentTimestamp = *timestamp;
            if ((ret = url_open_dyn_buf(&dynamicBuffer)) < 0) {
                return ret;
            }
            m_dynamicBuffer = dynamicBuffer;
        }
        avio_write(dynamicBuffer, buf, len);

        if (!(flags & RTP_HEADER_FLAGS_MARKER)) {
            // TODO handle this return value properly
            return AVERROR(EAGAIN);
        }

        av_free(m_pBuffer);
        //printf("Freeing latest dynamic buffer\n");
        m_nLength = url_close_dyn_buf(dynamicBuffer, &m_pBuffer);
        dynamicBuffer = NULL;
        m_dynamicBuffer = NULL;
        m_nPosition = 0;
    }

    if (m_pBuffer == NULL) {
        return AVERROR(EIO);
    }

    cur_len = 0;
    while (m_nPosition < m_nLength) {
        uint8_t val = m_pBuffer[m_nPosition++];
        cur_len += val;
        if (val != 0xff)
            break;
    }
    if (m_nPosition + cur_len > m_nLength) {
        return AVERROR(EIO);
    }

    if ((ret = av_new_packet(pkt, cur_len)) < 0) {
        return ret;
    }
    memcpy(pkt->data, m_pBuffer + m_nPosition, cur_len);
    m_nPosition += cur_len;

    return m_nPosition < m_nLength;
#else
    // Experimental code assuming no fragmentation
    int ret, cur_len;

    m_nPosition = 0;
    m_nLength = len;
    cur_len = 0;
    while (m_nPosition < m_nLength) {
        uint8_t val = buf[m_nPosition++];
        cur_len += val;
        if (val != 0xff)
            break;
    }
    if (m_nPosition + cur_len > m_nLength) {
        printf("LATM parsing bad packet\n");
        return AVERROR(EIO);
    }

    if ((ret = av_new_packet(pkt, cur_len)) < 0) {
        printf("Failed to create a new packet\n");
        return ret;
    }
    memcpy(pkt->data, buf + m_nPosition, cur_len);
    m_nPosition += cur_len;

    return m_nPosition < m_nLength;

#endif
}


