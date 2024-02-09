#ifndef __H_WRITERTHR__
#define __H_WRITERTHR__

#include "streamerthr.hh"

/// The Writer Thread Class
class CWriterThr : public CStreamerThread
{
private:
    char *                          m_pMediaDir;
    
    CStreamer * handleAdd(CCommand *pCmd);

    int handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer);

protected:
    virtual void resetGlobalStats();

public:
    CWriterThr(const char *pMediaDir);
    ~CWriterThr();
    
};
#endif /* __H_WRITERTHR__ */
