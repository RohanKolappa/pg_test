/*
 * v2ddecoderthr.hh
 *
 *  Created on: Jul 8, 2010
 *      Author: rkale
 */

#ifndef V2DDECODERTHR_HH_
#define V2DDECODERTHR_HH_

#include "appthread.hh"
#include "common.hh"
#include "mutex.hh"
#include "codecdefs.h"

class CCommand;
class CVideoFrameDecoder;
class CSQueueSink;
class CSQueueSource;
class CSBuffer;
struct SwsContext;
struct AVFrame;

class CV2DDecoderThread : public CAppThread {

public:
    CV2DDecoderThread(int aId, CSQueueSink *qRendererSink, CSQueueSource *qRendererSource);
    virtual ~CV2DDecoderThread();
    virtual int Run();
    void SetupDecoder(int width, int height, int pixelFormat, int sliceSize,
            bool isSVC, eEncodeFormat encodeFormat);
    bool IsReady();
    void DecodeFrame(unsigned char * aBuffer, int aLen);
    void SendRFBStatus(bool a_bRFBReady);
    void UpdateCustomStatus(CCommand *a_pCmd);
    void SetVideoDebugLevel(int a_debugLevel);
    bool CanDecodeFrame();
    void SetupScaledDimensions(int aScalingMode, int aScaledWidth, int aScaledHeight);

private:
    int DecodeProcessor();
    int ProcessAltStream();
    void FreeSBuffers();
    int GetDoubleBufferIndex();
    void SetupScaledDimensions();
    void SetupScaler();
    void SetupDoubleBuffers();

    CMutex                  m_mutex;
    CSQueueSink *           m_qRendererSink;
    CSQueueSource *         m_qRendererSource;
    unsigned char *         m_inBuffer;
    int                     m_inBufferLen;
    bool                    m_bSendFrameInfo;
    bool                    m_bDecoderDebug;
    bool                    m_bSendRFBStatus;
    bool                    m_bRFBReady;
    CVideoFrameDecoder *    m_pFrameDecoder;
    CSBuffer *              m_sBuffer[2];
    int                     m_nDoubleBufferIndex;
    SwsContext *            m_pSwsCtx;
    AVFrame *               m_pDestPictureArray[2];

    unsigned char *         m_pDecodeBuffer;
    unsigned char *         m_pY;
    unsigned char *         m_pU;
    unsigned char *         m_pV;
    int                     m_nDecodeSize;
    int                     m_nScaledDecodeSize;
    unsigned char *         m_pImageBuffer;
    int                     m_nImageBufferSize;
    int                     m_nWidth;
    int                     m_nHeight;
    int                     m_nScaledWidth;
    int                     m_nScaledHeight;
    int                     m_nScalingMode;
    int                     m_nSliceSize;
    ePixelFormat            m_ePixelFormat;
    uint64_t                m_nDecodedFrames;
};

#endif /* V2DDECODERTHR_HH_ */
