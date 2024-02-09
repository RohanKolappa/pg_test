/*
 * MPEG4AudioRTPParser.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: rkale
 */



#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "bitstream.h"

#include "MPEG4AudioRTPParser.hh"


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


CMPEG4AudioRTPParser::CMPEG4AudioRTPParser(unsigned char aPayloadType)
    : CRTPParser(aPayloadType, false) {

    m_nSizeLength = 0;
    m_nIndexLength = 0;
    m_nIndexDeltaLength = 0;

    m_au_headers = NULL;

    m_au_headers_allocated = 0;
    m_nb_au_headers = 0;
    m_au_headers_length_bytes = 0;
    m_cur_au_index = 0;
}

CMPEG4AudioRTPParser::~CMPEG4AudioRTPParser() {

}

void CMPEG4AudioRTPParser::RetrieveCodecConfig(MediaSubsession *a_pSubsession,
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

    m_nSizeLength = a_pSubsession->fmtp_sizelength();
    m_nIndexLength = a_pSubsession->fmtp_indexlength();
    m_nIndexDeltaLength = a_pSubsession->fmtp_indexdeltalength();

}

int CMPEG4AudioRTPParser::Depayload(
        AVPacket * pkt,
        uint32_t * timestamp,
        const uint8_t * buf,
        int len, int flags)
{

    if (ParseAu(buf))
        return -1;


    buf += m_au_headers_length_bytes + 2;
    len -= m_au_headers_length_bytes + 2;

    /* XXX: Fixme we only handle the case where rtp_parse_mp4_au define
                    one au_header */
    av_new_packet(pkt, m_au_headers[0].size);
    memcpy(pkt->data, buf, m_au_headers[0].size);

    return 0;
}


int CMPEG4AudioRTPParser::ParseAu(const uint8_t *buf)
{
    int au_header_size, i;
    //GetBitContext getbitcontext;

    /* decode the first 2 bytes where the AUHeader sections are stored
       length in bits */
    uint16_t au_headers_length = *((uint16_t *)buf);
    au_headers_length = ntohs(au_headers_length);

    if (au_headers_length > 1500)
      return -1;

    m_au_headers_length_bytes = (au_headers_length + 7) / 8;

    /* skip AU headers length section (2 bytes) */
    buf += 2;

    CBitStream pBitStream(buf, m_au_headers_length_bytes * 8);

    //init_get_bits(&getbitcontext, buf, data->au_headers_length_bytes * 8);

    /* XXX: Wrong if optionnal additional sections are present (cts, dts etc...) */
    au_header_size = m_nSizeLength + m_nIndexLength;
    if (au_header_size <= 0 || (au_headers_length % au_header_size != 0))
        return -1;

    m_nb_au_headers = au_headers_length / au_header_size;
    if (!m_au_headers || m_au_headers_allocated < m_nb_au_headers) {
        av_free(m_au_headers);
        m_au_headers = (struct AUHeaders *) av_malloc(sizeof(struct AUHeaders) * m_nb_au_headers);
        m_au_headers_allocated = m_nb_au_headers;
    }

    /* XXX: We handle multiple AU Section as only one (need to fix this for interleaving)
       The FAAD decoder does not behave correctly when sending each AU one by one
       but does when sending the whole as one big packet...  */
    m_au_headers[0].size = 0;
    m_au_headers[0].index = 0;
    for (i = 0; i < m_nb_au_headers; ++i) {
//        m_au_headers[0].size += get_bits_long(&getbitcontext, m_nSizeLength);
//        m_au_headers[0].index = get_bits_long(&getbitcontext, m_nIndexLength);
        m_au_headers[0].size += pBitStream.getBitsValue(m_nSizeLength);
        m_au_headers[0].index = pBitStream.getBitsValue(m_nIndexLength);
    }

    m_nb_au_headers = 1;

    return 0;
}
