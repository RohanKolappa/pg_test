#ifndef __H_PROCESSORTHR__
#define __H_PROCESSORTHR__

#include "appthread.hh"
#include "command.hh"
#include "streamer.hh"
#include "squeue.hh"


class CProcessorThread : public CAppThread
{
public:
    CProcessorThread(std::string a_name, int a_affinity);
    ~CProcessorThread();

    int Run();
    void AddStreamer(CStreamer *);
    void RemoveStreamer(CStreamer *);


private:
    std::list<CStreamer *>          m_LStreamers;
    CSQueueSink *                   m_qControllerSink;
    CMutex                          m_mutex;


    int ProcessStreamers();

};
#endif /* __H_PROCESSORTHR__ */
