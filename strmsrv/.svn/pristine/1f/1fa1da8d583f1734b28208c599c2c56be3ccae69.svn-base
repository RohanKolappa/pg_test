#ifndef __HH_V2DTX_H__
#define __HH_V2DTX_H__

#include <queue>

#include "sender.hh"
#include "v2d.h"
#include "common.hh"
#include "shaper.hh"
#include "v2o_lib.h"
#include "rtp.h"
#include "IdProvider.hh"

#define DEFAULT_MCASTADDR       "226.1.1.1"
#define DEFAULT_MCASTTTL        1

class CScapProcessor;

class CV2dTx : public CSender
{
public:
    CV2dTx(int nId,
           int nSrvSock,
           int nTTL,
           char *sMcastAddr,
           int maxBandwidth,
           int bIsV2DTx,
           int nListener,
           int bPolicyCheck,
           char *sPolicyFileName,
           char *sProtocolVersion,
           const char *sAuthScheme,
           const char *sAuthData,
           int nInterface);
    virtual ~CV2dTx();


    int ProcessStream();

    void PrintStatistics(std::string& a_stats);
    void PrintError(CNotify *a_notify);
    void PrintNotification(CNotify *a_notify);
    void UpdateCustomStatus(CCommand *a_pCmd);

    int  Update(CCommand *a_pCmd);
    int GetServerPort() {return m_nServerPort;};
    int SetTCPControlSocket (int nSockfd);
    int SetHTTPControlSocket (int nSockfd);
    int SetHTTPVideoSocket (int nSockfd);
    int SetHTTPAudioSocket (int nSockfd);
    void SetCustomStatusInfo();
    int StartSenderStream(int nQueueId);
    int UpdateStats(V2DTx_t *m_TxSession);

private:
    V2DTx_t *               m_TxSession;
    CShaper *               m_pShaper;
    CScapProcessor *        m_scapProcessor;
    int                     m_nServerPort;
    bool                    m_bSentCachedScap;
    bool                    m_bCheckIFrameRequest;
    bool                    m_bForceIFrameRequest;
    uint64_t                m_framesServed;
    uint64_t                m_bytesServed;
    timestamp_t             m_lastTimeStamp;
    bool                    m_flowcontrol;
    bool                    m_enableSRD;
    bool                    m_bSRDPacketPending;
    bool                    m_bSRDPacketSent;
    unsigned long           m_nSRDPacketOffset;
    unsigned char           m_pSRDPacketBuffer[SIZE_OF_AV_PACKET * 2];
    unsigned char *         m_pSRDInPacket;
    unsigned char *         m_pSRDOutPacket;
    std::queue<unsigned char *>    m_lSRDFreePackets;
    std::queue<unsigned char *>    m_lSRDAvailablePackets;
    unsigned long           m_nMaxBandwidth;
    int                     m_stereoPolicy;
    sliceRedData_t *        m_srd;
    bool                    m_nListener;
    bool                    m_bReleaseId;
    CSQueueSink *           m_qSchedulerSink;
    CSQueueSource *         m_qSchedulerSource;
    char *                  m_sPolicyFileName;
    char *                  m_sProtocolVersion;
    bool                    m_bIsV2DTx;
    bool                    m_bPolicyCheck;
    bool                    m_bIsVideoSrcRTP;
    bool                    m_bIsAudioSrcRTP;
    bool                    m_bNetworkHeadersSent;
    RtpContext_t            m_RTPContext[2];
    unsigned char*          m_pAuxPacket;
    std::string             m_sInterfaceInfo;
    HAL *                   m_pHal;
    int                     m_nFrameOffset;
    static CIdProvider      m_IdProvider;

    int ProcessAltStream();
    void ProcessSCAP(CSFrame *a_sFrame);
    void SendCachedSCAP();
    void EnableSRD();
    void DisableSRD();
    void ScheduleDelete();
    static void CloseSocket(int a_fd);
    static const char * PolicyToString(int a_policy);
    static void ListenerCallback(void *a_object, int a_fd);
    static void ErrorCallback(void *a_object);
    void ErrorCallbackHandler();
    static void NotificationCallback(void *a_object);
    void NotificationCallbackHandler();
    static void SRDNewFrameCallback(void *a_object);
    void SRDNewFrameCallbackHandler();

    int  SendFrameToSRD(CSFrame* a_pSFrame);
    void ProcessVideoFrame(CSFrame* a_pSFrame);
    void ProcessAudioFrame(CSFrame* a_pSFrame);
    void ProcessControlFrame(CSFrame* a_pSFrame);
    void ProcessSRDFrame(bool bInProgress);
    void RecoverSRDPackets();
    void SendConnectionStatus(bool a_status);
    bool SendNetworkHeader();
    void StoppingSender();
    int StopSender();

    // for backward compatibility
    unsigned long RetrieveFrameType(CSFrame *a_frame) {
        if (a_frame->GetFrameVersion() == FRAMEVERSION_0) {
            m_flowcontrol = false;
            return (a_frame->GetMediaFlag() & 0xFFFF);
        }
        return a_frame->GetFrameType();
    }

    enum {RTPCONTEXT_AUDIO = 0, RTPCONTEXT_VIDEO};
};
#endif /*__HH_V2DTX_H__*/
