#ifndef __HH_V2DFF_H__
#define __HH_V2DFF_H__

#include "sender.hh"
#include "v2d.h"

class CV2dFF : public CSender
{
public:
    CV2dFF(int nId);
    ~CV2dFF();
        
    int ProcessStream();
        
    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);
    int  Update(CCommand *a_pCmd);

private:
    int                     m_nSendQueueId;
    CSQueueSink *           m_qSendQueueSink;
    sliceRedData_t *        m_srd;
    bool                    m_bFramePending;
    unsigned long           m_lastReducedFrameCount;
    unsigned char           m_pSRDPacket[SIZE_OF_AV_PACKET];
    
    unsigned long RetrieveFrameType(CSFrame *a_frame) {
        if (a_frame->GetFrameVersion() == FRAMEVERSION_0) {
            return (a_frame->GetMediaFlag() & 0xFFFF);
        }
        return a_frame->GetFrameType();
    }

};
#endif /*__HH_V2DTX_H__*/
