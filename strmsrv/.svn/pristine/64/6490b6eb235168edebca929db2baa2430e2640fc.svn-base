/*
 * MPEG4VideoRTPParser.cpp
 *
 *  Created on: May 3, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "bitstream.h"

#include "MPEG4VideoRTPParser.hh"


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


CMPEG4VideoRTPParser::CMPEG4VideoRTPParser(unsigned char aPayloadType)
    : CRTPParser(aPayloadType, true) {

}

CMPEG4VideoRTPParser::~CMPEG4VideoRTPParser() {

}

void CMPEG4VideoRTPParser::RetrieveCodecConfig(MediaSubsession *a_pSubsession,
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

