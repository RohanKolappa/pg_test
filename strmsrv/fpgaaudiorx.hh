#ifndef __HH_FPGAAUDIORX_H__
#define __HH_FPGAAUDIORX_H__

#include "receiver.hh"
#include "command.hh"
#include "common.hh"
#include "rtp.h"
#include "portuse.hh"
#include "v2d.h"
#include "v2o_lib.h"

#define RTPCONTEXT_AUDIO 0

class CFPGAAudioRx : public CReceiver
{
public:
    CFPGAAudioRx(int nId, int nIOPort, int nAudioType);
    
    ~CFPGAAudioRx();
    
    int ProcessStream();
    int ProcessAltStream();
    
    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);
    
private:
    // File Handle
    int     m_hAudio;
    int     m_nMaxfd;

    int     m_nAudioType;

    HAL     *m_pHal;
    int     m_nIOPort;
    static  CPortUse m_portsUsed;

    bool    m_bV2DI50Rx;
    bool    m_bXpiDevice;
    uint64_t m_nAudioSOFerrors;
    
    RtpContext_t        m_RTPContext[1];

    int GetAudioFrame(void);

    int Update(CCommand *a_pCmd);
    
 
};
#endif /* __HH_FPGAAUDIORX_H__ */
