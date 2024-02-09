#ifndef __H_SENDERTHR__
#define __H_SENDERTHR__

#include "streamerthr.hh"

/// The Sender Thread Class
class CSenderThr : public CStreamerThread
{
private:
    CStreamer * handleAdd(CCommand *pCmd);
    int handleSyncStop(CCommand *pCmd);
    int handleStop(CCommand *pCmd);
    int handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer);
    
public:
    CSenderThr();
    ~CSenderThr();
    
};
#endif /* __H_SENDERTHR__ */
