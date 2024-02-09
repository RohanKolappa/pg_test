/*
 * rtpaudiodecoderthr.hh
 *
 *  Created on: Apr 25, 2011
 *      Author: rkale
 */

#ifndef RTPAUDIODECODERTHR_HH_
#define RTPAUDIODECODERTHR_HH_


#include "detachedthread.hh"
#include "streamer.hh"
#include "squeue.hh"
#include "command.hh"

// Forward Declarations
class CAudioRenderer;
class MediaSubsession;
class CRTPParser;
struct AVCodecContext;
struct AVCodec;
struct AVPacket;
struct AVCodecParserContext;
struct AVFrame;

class CRTPAudioDecoderThread : public CDetachedThread {

public:
    CRTPAudioDecoderThread(CStreamer *a_pStreamer, MediaSubsession *a_pSubsession);

    int RunThread();
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    ~CRTPAudioDecoderThread();
    int DecodeProcessor();
    bool FindDecoder();
    int Depayload(uint8_t *buffer, int len, AVPacket *avpkt);
    int ParsePacket(uint8_t * packet_buffer, int packet_size);
    void DecodeFrame(uint8_t *frame_buffer, int frame_size);
    void PublishFrame();
    void DetermineFrameSize(int packet_size);
    void InitializeAudioRenderer();
    void SetupCodecParameters();

    bool m_bError;
    CSQueueSource * m_qQueueSource;
    CSQueueSink * m_qRendererSink;
    CSQueueSource * m_qRendererSource;
    CAudioRenderer * m_pAudioRenderer;
    CRTPParser * m_pRTPParser;
    uint8_t * m_pDecodeBuffer;
    uint64_t m_nPacketsDecoded;
    uint64_t m_nFramesDecoded;
    bool m_bSendFrameInfo;
    std::string m_sErrorString;

    MediaSubsession *m_pSubsession;
    AVCodecContext *m_pCodecCtx;
    AVCodec *m_pCodec;
    AVCodecParserContext *m_pParserContext;
};

#endif /* RTPAUDIODECODERTHR_HH_ */
