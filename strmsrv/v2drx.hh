#ifndef __HH_V2DRX_H__
#define __HH_V2DRX_H__

#include "receiver.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "common.hh"
#include "rtp.h"

class CV2dRx : public CReceiver
{

public:
    CV2dRx(int nId,
           char *dest,
           int port,
           int bandwidth,
           int multicast,
           int avoption,
           int videoport,
           int audioport,
           int audiotype,
           int httpport,
           const char *sAuthScheme,
           const char *sAuthData,
           intlist interfaceList);

    ~CV2dRx();


    int ProcessStream();

    void PrintStatistics(std::string& a_stats);
    void PrintError(CNotify *a_notify);
    void PrintNotification(CNotify *a_notify);
    void SendRtpFilter();
    void UpdateCustomStatus(CCommand *a_pCmd);
    void SetCustomStatusInfo();
    int  Update(CCommand *a_pCmd);

private:
    V2DRx_t *                   m_RxSession;
    char                        m_errorString[256];
    RtpContext_t                m_RTPContext[2];
    unsigned char*              m_pRTPPacket;
    std::string                 m_sInterfaceInfo;
    HAL *                       m_pHal;
    bool                        m_bRTPFilterSent;

    enum {RTPCONTEXT_AUDIO = 0, RTPCONTEXT_VIDEO};

    void SetFirstTimestamp();
    int ProcessAltStream();

};
#endif /*__HH_V2DRX_H__*/
