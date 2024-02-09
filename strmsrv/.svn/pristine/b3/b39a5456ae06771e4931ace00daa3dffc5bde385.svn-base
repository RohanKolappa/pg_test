#ifndef _RTP_FRAMEFILTER_HH
#define _RTP_FRAMEFILTER_HH

#include <time.h>
#include <list>

#include "rtp.hh"
#include "rtpconst.hh"

class RTPFrame {

  private:
    int initFrame(rtp3016_t *pkt, int size);
    int completeFrame();

  public:
    unsigned long _ts;
    int _complete;
    char *_frame;
    int _frameLen;
    int _frameType;
    time_t _updatedAt;

    RTPFrame(unsigned long ts, int frameType);
    ~RTPFrame();
    int handlePacket(rtp3016_t *pkt, int size);
};

class RTPFrameFilter {

  private:
    std::list<RTPFrame *> _frameList;
    std::list<RTPFrame *> :: iterator _fIter;
    int doGC();

  public:
    RTPFrameFilter();
    int handlePacket(rtp3016_t *pkt, int size, int frameType, RTPFrame *&rtpFrame);
    ~RTPFrameFilter();
    
};

#endif
