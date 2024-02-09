/*
 * v2dplayer.hh
 *
 *  Created on: Jul 5, 2010
 *      Author: rkale
 */

#ifndef V2DPLAYERTX_HH_
#define V2DPLAYERTX_HH_

#include <queue>

#include "sender.hh"
#include "v2d.h"
#include "common.hh"
#include "shaper.hh"

#include "v2o_lib.h"

#include "rtp.h"
#include "vframedecoder.hh"

class CVideoFrameDecoder;
class CV2DDecoderThread;
class CAudioRenderer;
class CScapProcessor;

class CV2dPlayerTx: public CSender {
public:
    CV2dPlayerTx(int aId, int pixelformat);
    virtual ~CV2dPlayerTx();

    int ProcessStream();

    void PrintStatistics(std::string& a_stats);
    void PrintError(CNotify *a_notify);
    void PrintNotification(CNotify *a_notify);
    void UpdateCustomStatus(CCommand *a_pCmd);

    int Update(CCommand *a_pCmd);
    void SetCustomStatusInfo();
    int StartSenderStream(int nQueueId);

private:

    bool                    m_bSRDPacketSent;
    CSQueueSink *           m_qRendererSink;
    CSQueueSource *         m_qRendererSource;

    CScapProcessor *        m_scapProcessor;
    sliceRedData_t *        m_srd;
    int                     m_nPixelFormat;

    bool                    m_bIsVideoSrcRTP;
    bool                    m_bIsAudioSrcRTP;
    bool                    m_bRFBReady;
    int                     m_nFrameLength;
    int                     m_outBufferLen;
    uint8_t *               m_outBuffer;
    uint8_t *               m_pAudioDecodeBuffer;
    CV2DDecoderThread  *    m_pDecoderThread;
    CAudioRenderer *        m_pAudioRenderer;
    uint64_t                m_nVideoPackets;
    uint64_t                m_nVideoBytes;
    uint64_t                m_nAudioPackets;
    uint64_t                m_nAudioBytes;
    uint64_t                m_nControlPackets;
    uint64_t                m_nControlBytes;
    uint32_t                m_nQueryId;
    uint32_t                m_nSessionId;

    int ProcessAltStream();
    void HandleSCAP();
    void SendCachedSCAP();
    void CreateSRD();
    void DestroySRD();
    void ScheduleDelete();
    static void ScapCallback(void *a_object, ScapData_t *scap);
    static void ListenerCallback(void *a_object, int a_fd);
    static void ErrorCallback(void *a_object);
    void ErrorCallbackHandler();
    static void NotificationCallback(void *a_object);
    void NotificationCallbackHandler();
    static void SRDNewFrameCallback(void *a_object);
    void SRDNewFrameCallbackHandler();
    bool ReadyToDecodeFrame();
    bool GetFrameFromSRD();
    void CreateOutBuffer(int length);

    void SendKeyboardEvent(uint32_t key, uint8_t down);
    void SendMouseEvent(uint16_t x, uint16_t y, uint8_t buttonmask);
    void SendRFBStatusQuery();
    int ProcessRFBStatus(char *pData);
    void SendControlPacket(uint8_t *a_pBuffer, int a_nLen);

    int  SendFrameToSRD(CSFrame* a_pSFrame);
    void ProcessVideoFrame(CSFrame* a_pSFrame);
    void ProcessAudioFrame(CSFrame* a_pSFrame);
    void ProcessControlFrame(CSFrame* a_pSFrame);
    void ProcessSRDFrame(bool bInProgress);
    void RecoverSRDPackets();
    void DecodeAudio(uint8_t * buffer, int len);
    void RecordToFile(uint8_t * data, int length);

    enum {RTPCONTEXT_AUDIO = 0, RTPCONTEXT_VIDEO};

};

#endif /* V2DPLAYERTX_HH_ */
