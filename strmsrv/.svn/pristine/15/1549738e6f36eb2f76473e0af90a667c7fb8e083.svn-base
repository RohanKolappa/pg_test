#ifndef __H_READERTHR__
#define __H_READERTHR__

#include "streamerthr.hh"

/// The Reader Thread Class
class CReaderThr : public CStreamerThread
{
private:
    char *                          m_pMediaDir;

    CStreamer * handleAdd(CCommand *pCmd);
    int handleQueryStreamer(CCommand *pCmd, timestamp_t & a_timeStamp);

public:
    CReaderThr(const char *pMediaDir);
    ~CReaderThr();

    int handleSyncStart(CCommand *pCmd);

};
#endif /* __H_READERTHR__ */
