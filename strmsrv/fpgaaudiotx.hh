#ifndef __HH_FPGAAUDIOTX_H__
#define __HH_FPGAAUDIOTX_H__

#include "sender.hh"
#include "portuse.hh"
#include "v2d.h"
#include "v2o_lib.h"

#define REVAUDIO_BUFF_SIZE 4000

class CFPGAAudioTx : public CSender
{
public:
    CFPGAAudioTx(int nId, int nIOPort, int nAudioport, int nAudioType);
    ~CFPGAAudioTx();

    int ProcessStream();

    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    uint64_t m_nAudioPackets;
    int m_hAudio;
    int m_nAudioType;
    char * m_pRevAudioBuffer;
    int  m_nBufferLen;
    bool m_bV2DI50Tx;
    int m_nAudioFrameSize;
    bool m_bXpiDevice;

    HAL     *m_pHal;
    int     m_nIOPort;
    static  CPortUse m_portsUsed;

    void ProcessFrameTypeAudio(CSFrame *sFrame);
    void ResetFPGAAudio();

};
#endif /*__HH_FPGAAUDIOTX_H__*/

