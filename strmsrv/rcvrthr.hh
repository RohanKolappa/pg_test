#ifndef __H_RCVRTHR__
#define __H_RCVRTHR__

#include "streamerthr.hh"

/// The Receiver Thread Class
class CRcvrThr : public CStreamerThread
{
private:

    CStreamer * handleAdd(CCommand *pCmd);
    int handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer);

public:
        CRcvrThr();
        ~CRcvrThr();

};
#endif /* __H_RCVRTHR__ */
