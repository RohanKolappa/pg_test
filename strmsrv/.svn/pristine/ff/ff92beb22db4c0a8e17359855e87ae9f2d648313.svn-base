/*
 * H264VideoRTPParser.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "H264VideoRTPParser.hh"


CH264VideoRTPParser::CH264VideoRTPParser(unsigned char aPayloadType)
    : CRTPParser(aPayloadType, true) {

}

CH264VideoRTPParser::~CH264VideoRTPParser() {

}

void CH264VideoRTPParser::RetrieveCodecConfig(MediaSubsession *a_pSubsession,
        AVCodecContext * a_pCodecCtx)
{
    unsigned char spropData[256];
    int spropDataLen = 0;
    unsigned char const start_code[3] = {0x00, 0x00, 0x01};
    if (a_pSubsession == NULL)
        return;

    if (a_pSubsession->fmtp_spropparametersets() == NULL)
        return;

    unsigned numSPropRecords;
    SPropRecord* sPropRecords = parseSPropParameterSets(
            a_pSubsession->fmtp_spropparametersets(),
            numSPropRecords);
    int spropOffset = 0;
    for (unsigned i = 0; i < numSPropRecords; ++i) {
        memcpy(spropData + spropOffset, start_code, 3);
        memcpy(spropData + spropOffset + 3,
                sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
        spropDataLen += 3 + sPropRecords[i].sPropLength;
        spropOffset += spropDataLen;
    }
    delete[] sPropRecords;

    if (spropDataLen > 0) {
        uint8_t *dest = (uint8_t *) av_malloc(spropDataLen +
                FF_INPUT_BUFFER_PADDING_SIZE);
        memset(dest + spropDataLen, 0, FF_INPUT_BUFFER_PADDING_SIZE);
        memcpy(dest, spropData, spropDataLen);
        a_pCodecCtx->extradata = dest;
        a_pCodecCtx->extradata_size = spropDataLen;
    }

}

int CH264VideoRTPParser::Depayload(
        AVPacket * pkt,
        uint32_t * timestamp,
        const uint8_t * buf,
        int len, int flags)
{
    uint8_t nal = buf[0];
    uint8_t type = (nal & 0x1f);
    int result= 0;
    uint8_t start_sequence[]= {0, 0, 1};

    assert(buf);

    if (type >= 1 && type <= 23)
        type = 1;              // simplify the case. (these are all the nal types used internally by the h264 codec)

    switch (type) {
    case 0:                    // undefined, but pass them through
    case 1:
        av_new_packet(pkt, len + sizeof(start_sequence));
        memcpy(pkt->data, start_sequence, sizeof(start_sequence));
        memcpy(pkt->data+sizeof(start_sequence), buf, len);
        break;

    case 24:                   // STAP-A (one packet, multiple nals)
        // consume the STAP-A NAL
        buf++;
        len--;
        // first we are going to figure out the total size....
        {
            int pass= 0;
            int total_length= 0;
            uint8_t *dst= NULL;

            for(pass= 0; pass<2; pass++) {
                const uint8_t *src= buf;
                int src_len= len;

                do {
                    uint16_t nal_size = *((uint16_t *)src);
                    nal_size = ntohs(nal_size);

                    // consume the length of the aggregate...
                    src += 2;
                    src_len -= 2;

                    if (nal_size <= src_len) {
                        if(pass==0) {
                            // counting...
                            total_length+= sizeof(start_sequence)+nal_size;
                        } else {
                            // copying
                            assert(dst);
                            memcpy(dst, start_sequence, sizeof(start_sequence));
                            dst+= sizeof(start_sequence);
                            memcpy(dst, src, nal_size);
                            dst+= nal_size;
                        }
                    } else {
                        printf("Error: nal size exceeds length: %d %d\n", nal_size, src_len);
                    }

                    // eat what we handled...
                    src += nal_size;
                    src_len -= nal_size;

                    if (src_len < 0)
                        printf("Error: Consumed more bytes than we got! (%d)\n", src_len);
                } while (src_len > 2);      // because there could be rtp padding..

                if (pass==0) {
                    // now we know the total size of the packet (with the start sequences added)
                    av_new_packet(pkt, total_length);
                    dst= pkt->data;
                } else {
                    assert(dst-pkt->data==total_length);
                }
            }
        }
        break;

    case 25:                   // STAP-B
    case 26:                   // MTAP-16
    case 27:                   // MTAP-24
    case 29:                   // FU-B
        printf("Error: Unhandled type (%d) (See RFC for implementation details\n",
               type);
        result= -1;
        break;

    case 28:                   // FU-A (fragmented nal)
        buf++;
        len--;                  // skip the fu_indicator
        {
            // these are the same as above, we just redo them here for clarity...
            uint8_t fu_indicator = nal;
            uint8_t fu_header = *buf;   // read the fu_header.
            uint8_t start_bit = fu_header >> 7;
//            uint8_t end_bit = (fu_header & 0x40) >> 6;
            uint8_t nal_type = (fu_header & 0x1f);
            uint8_t reconstructed_nal;

            // reconstruct this packet's true nal; only the data follows..
            reconstructed_nal = fu_indicator & (0xe0);  // the original nal forbidden bit and NRI are stored in this packet's nal;
            reconstructed_nal |= nal_type;

            // skip the fu_header...
            buf++;
            len--;

#ifdef DEBUG
            if (start_bit)
                data->packet_types_received[nal_type]++;
#endif
            if (start_bit) {
                //printf("Found start bit\n");
                // copy in the start sequence, and the reconstructed nal....
                av_new_packet(pkt, sizeof(start_sequence) + sizeof(nal) + len);
                memcpy(pkt->data, start_sequence, sizeof(start_sequence));
                pkt->data[sizeof(start_sequence)]= reconstructed_nal;
                memcpy(pkt->data + sizeof(start_sequence) + sizeof(nal), buf, len);
            } else {
                av_new_packet(pkt, len);
                memcpy(pkt->data, buf, len);
            }
        }
        break;

    case 30:                   // undefined
    case 31:                   // undefined
    default:
        result= -1;
        break;
    }


    return result;
}
