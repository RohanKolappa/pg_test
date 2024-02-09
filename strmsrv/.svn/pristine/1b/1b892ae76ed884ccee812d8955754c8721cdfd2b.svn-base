#include "strmsrv_compatibility.h"

#include "RTPFrameUtils.hh"
#include "rtpconst.hh"
#include "sframe.hh"

#define MP4_VIDEO_OBJECT_PLANE_SC 0xB6
#define MP4_VOP_TYPE_I 0
unsigned long decodeMP4VESFrameType(rtp3016_t *pkt, int size) {

    unsigned char *headerStart = (unsigned char *)pkt;
    headerStart = headerStart + sizeof(rtp3016_t);
    int packetSize = size - sizeof(rtp3016_t);

    int i, len = packetSize;
    unsigned char*  ptr = headerStart;
    while ((*ptr != MP4_VIDEO_OBJECT_PLANE_SC) && (len > 3)) {
        len = (headerStart + packetSize) - ptr;
        for (i = 0; i < len - 3; i++) {
            if (ptr[i] == 0 && ptr[i + 1] == 0 && ptr[i + 2] == 1) {
                ptr += (i + 3);
                break;
            }
        }
        if(i == (len - 3)) {
            // printf("P");
            return FRAMETYPE_P; // No info - default to non I-Frame
        }
    }
    unsigned char thisbyte = *(ptr + 1);
    thisbyte >>= (8 - 2);
    if (thisbyte == MP4_VOP_TYPE_I) {
        // printf("I\n");
        return FRAMETYPE_I;
    }
    else {
        // printf("P");
        return FRAMETYPE_P;
    }
}

int getAudioFrameType(char *aCodec) {
    int frameType = (FRAMETYPE_RAW | FRAMETYPE_AUDIO);
    if((aCodec != NULL) && (!strcmp(aCodec, RTP_AUDIO_MPA_LABEL)) )
        frameType |= FRAMETYPE_MPA;
    return frameType;
}

int getVideoFrameType(char *vCodec) {
    int frameType = (FRAMETYPE_RAW | FRAMETYPE_VIDEO);
    if(!strcmp(vCodec, RTP_VIDEO_MP4VES_LABEL))
        frameType |= FRAMETYPE_MP4V;
    return frameType;
}

unsigned long mapFrameType(int frameType, rtp3016_t *pkt, int size) {
    switch(frameType & FRAMETYPE_AVCODEC_MASK) {
    case (FRAMETYPE_VIDEO | FRAMETYPE_MP4V):
        return (frameType | decodeMP4VESFrameType(pkt, size));
    default:
        return frameType;
    };
}
