#ifndef _RTPFRAMEUTILS_HH
#define _RTPFRAMEUTILS_HH
#include "rtp.hh"

#define RTP_VIDEO_MP4VES_LABEL "MP4VES"
#define RTP_AUDIO_MPA_LABEL "MPA"

int getAudioFrameType(char *aCodec);
int getVideoFrameType(char *vCodec);

unsigned long mapFrameType(int frameType, rtp3016_t *pkt, int size);
unsigned long decodeMP4VESFrameType(rtp3016_t *pkt, int size);

#endif
