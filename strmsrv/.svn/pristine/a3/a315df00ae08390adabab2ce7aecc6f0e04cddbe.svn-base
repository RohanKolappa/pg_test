
#include "strmsrv_compatibility.h"

#include "RTPFrameFilter.hh"
#include "wis.hh"
#include "sframe.hh"
#include "RTPFrameUtils.hh"
#include "common.hh"


RTPFrame::RTPFrame(unsigned long ts, int frameType) {
    _ts = ts;
    _complete = 0;
    _frameType = frameType;
    _frameLen = 0;
    _frame = NULL;
    time(&_updatedAt);
}

RTPFrame::~RTPFrame() {
    if(_frame != NULL)
        free(_frame);
    _frame = NULL;
}

int RTPFrame::initFrame(rtp3016_t *pkt, int size) {
    _frameLen = sizeof(rawhdr_t);
    _frame = (char *)malloc(_frameLen);
    if(_frame == NULL) {
        SYSLOG_ERROR("malloc failed to alloc memory\n");
        return RTP_ERROR;
    }
    rawhdr_t *hdr = (rawhdr_t *)_frame;
    hdr->packetFlag = mapFrameType(_frameType, pkt, size);

    hdr->packetTimestamp = _ts;

    return RTP_OK;
}

int RTPFrame::completeFrame() {

    rawhdr_t *hdr = (rawhdr_t *)_frame;
    hdr->packetSize = htonl(_frameLen);

    if( (hdr->packetSize % PACKET_BLOCK_SIZE) == 0)
        return RTP_OK;

    // Pad to be a multiple of PACKET_BLOCK_SIZE
    int padBytes = PACKET_BLOCK_SIZE - (hdr->packetSize % PACKET_BLOCK_SIZE);
    _frame = (char *)realloc(_frame, _frameLen + padBytes);
    if(_frame == NULL) {
        SYSLOG_ERROR("realloc failed to alloc memory\n");
        return RTP_ERROR;
    }
    char *buf = (char *)_frame;
    memset(&buf[_frameLen], 0, padBytes);
    _frameLen = _frameLen + padBytes;
    return RTP_OK;
}

int RTPFrame::handlePacket(rtp3016_t *pkt, int size) {

    time(&_updatedAt);

    if(_frame == NULL) { // Initialize the header user by rtptx
        if(initFrame(pkt, size) != RTP_OK)
            return RTP_ERROR;
    }

    int offset = sizeof(rtp3016_t);
    if((_frameType & FRAMETYPE_AVCODEC_MASK) == (FRAMETYPE_AUDIO | FRAMETYPE_MPA)) { // RFC 2250: section 3.5
        offset = offset + 4;
    }

    size = size - offset;
    char *tmp = _frame;
    _frame = (char *)realloc(_frame, _frameLen + size);

    if(_frame == NULL) {
        SYSLOG_ERROR("realloc failed to alloc memory\n");
        free(tmp);
        return RTP_ERROR;
    }

    char *source = (char *) pkt;
    memcpy(&_frame[_frameLen], &source[offset], size);
    _frameLen = _frameLen + size;

    if(!pkt->ph_m)
        return RTP_OK;

    _complete = 1;
    return completeFrame();

}

RTPFrameFilter::RTPFrameFilter() {
    _frameList.clear();
}

int RTPFrameFilter::doGC() {
    return 1;
//    time_t now;
//    time(&now);
//    for(_fIter = _frameList.begin();
//        _fIter != _frameList.end(); _fIter++) {
//        RTPFrame *rf = (*_fIter);
//        if((now - rf->_updatedAt) > RTP_EXPIRY) {
//            delete rf;
//            _frameList.erase(_fIter);
//        }
//    }
}

int RTPFrameFilter::handlePacket(rtp3016_t *pkt, int size, int frameType,
                                 RTPFrame *&rtpFrame) {
    int isNewFrame = 0;

    // Find the corresonding rtpFrame
    rtpFrame = NULL;
    for(_fIter = _frameList.begin();
        _fIter != _frameList.end(); ++_fIter) {
        RTPFrame *rf = (*_fIter);
        if(rf->_ts == pkt->ph_ts) {
            rtpFrame = rf;
            break;
        }
    }

    // Create a new rtpFrame if required
    if(rtpFrame == NULL) {
        rtpFrame = new RTPFrame(pkt->ph_ts, frameType);
        isNewFrame = 1;
    }

    // Add the packet to the rtpFrame
    if(rtpFrame->handlePacket(pkt, size) != RTP_OK) {
        SYSLOG_ERROR("failed to handle packet\n");
        rtpFrame = NULL;
        return RTP_ERROR;
    }

    doGC();

    if(rtpFrame->_complete != 1) { // New frame and not complete, buffer it
        if(isNewFrame)
            _frameList.push_back(rtpFrame);
        rtpFrame = NULL;
        return RTP_OK;
    }

    // Frame is complete
    if(!isNewFrame) { // Old frame and complete, free the buffer
        _frameList.erase(_fIter);
    }

    return RTP_OK;
}

RTPFrameFilter::~RTPFrameFilter() {

    for(_fIter = _frameList.begin(); _fIter != _frameList.end(); _fIter++) {
        delete (*_fIter);
    }
    _frameList.clear();

}
