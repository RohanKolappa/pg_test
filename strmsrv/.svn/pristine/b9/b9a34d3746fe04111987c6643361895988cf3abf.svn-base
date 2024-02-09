/*
 * rtpvideodecoderthr.hh
 *
 *  Created on: Apr 17, 2011
 *      Author: rkale
 */

#ifndef RTPVIDEODECODERTHR_HH_
#define RTPVIDEODECODERTHR_HH_

#include "detachedthread.hh"
#include "streamer.hh"
#include "squeue.hh"
#include "command.hh"
#include "codecdefs.h"

// Forward Declarations
class MediaSubsession;
class CRTPParser;
struct AVCodecContext;
struct AVCodec;
struct AVPacket;
struct AVCodecParserContext;
struct AVFrame;
struct SwsContext;

class CRTPVideoDecoderThread : public CDetachedThread {

public:
    CRTPVideoDecoderThread(CStreamer *a_pStreamer, MediaSubsession *a_pSubsession,
            ePixelFormat a_ePixelFormat);

    int RunThread();
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    ~CRTPVideoDecoderThread();
    int DecodeProcessor();
    int ProcessAltStream();
    bool FindDecoder();
    int Depayload(uint8_t *buffer, int len, AVPacket *avpkt);
    int ParsePacket(uint8_t * packet_buffer, int packet_size);
    void DecodeFrame(uint8_t *frame_buffer, int frame_size);
    void CheckFrameDimensions();
    void SetupScaledDimensions();
    void SetupScaledDimensions(int aScalingMode, int aScaledWidth, int aScaledHeight);
    void SetupDestinationImage();
    void PublishFrame(int aBufferIndex);
    void PublishFrameSmooth(bool bNewPicture);
    void SendFrameInfo();
    void FreeSBuffers();
    int GetDoubleBufferIndex();

    bool m_bError;
    bool m_bNeedsFrameParsing;
    CSQueueSource * m_qQueueSource;
    CSQueueSink * m_qRendererSink;
    CSQueueSource * m_qRendererSource;
    CRTPParser * m_pRTPParser;
    uint8_t * m_pDecodeBuffer;
    CSBuffer * m_sBuffer[2];
    bool m_bSendFrameInfo;

    int m_nDecodeSize;

    struct SwsContext * m_pSwsCtx ;
    ePixelFormat m_ePixelFormat;
    uint64_t m_nPacketsDecoded;
    uint64_t m_nFramesParsed;
    uint64_t m_nFramesDecoded;
    uint64_t m_nPicturesDecoded;
    uint64_t m_nPicturesRendered;
    uint64_t m_nDecodeErrors;
    uint64_t m_nPicturesDropped;
    int32_t m_nFirstTimeOffset;
    CSFrame * m_sDelayedFrame;
    timestamp_t m_nDelayTimestamp;

    int m_nWidth;
    int m_nHeight;
    int m_nScaledWidth;
    int m_nScaledHeight;
    int m_nScalingMode;

    MediaSubsession *m_pSubsession;
    AVCodecContext *m_pCodecCtx;
    AVCodec *m_pCodec;
    AVCodecParserContext *m_pParserContext;
    AVFrame *m_pPicture;
    AVFrame *m_pDestPicture;
    AVFrame *m_pDestPictureArray[2];
};

#endif /* RTPVIDEODECODERTHR_HH_ */
