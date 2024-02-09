#ifndef __H_STREAMERTHR__
#define __H_STREAMERTHR__

#include "appthread.hh"
#include "command.hh"
#include "streamer.hh"
#include "squeue.hh"
#include "processorthr.hh"

class CStreamerThread : public CAppThread
{
public:
    CStreamerThread(std::string a_threadName);
    virtual ~CStreamerThread();
    virtual int Run();

protected:
    std::list<CCommand *>           m_LCommands;
    std::list<CStreamer *>          m_LStreamers;
    CSQueueSource *                 m_qStreamerSource;
    CSQueueSink *                   m_qControllerSink;
    int                             m_delta;
    int                             m_threadId;
    double                          m_bitrate;
    int                             m_poolSize;
    int                             m_numbCPUs;
    std::vector<CProcessorThread *> m_processorThreadList;

    virtual CStreamer * handleAdd(CCommand *pCmd) = 0;
    virtual int handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer);
    virtual int handleDelete(CCommand *pCmd);
    virtual int handleReset(CCommand *pCmd);

    virtual int ProcessCommands();
    virtual int handleSyncStart(CCommand *pCmd);
    virtual int handleSyncStop(CCommand *pCmd);
    virtual int handleStop(CCommand *pCmd);
    virtual int handleQueryStreamer(CCommand *pCmd, timestamp_t & a_timeStamp);
    virtual void resetGlobalStats();

    int ProcessStreamers();
    int SendStatistics();
    int SendErrors();
    int SendNotifications();
    int SendResponse();
    int SendStatus(CCommand *pCmd);
    int SendObjectList(CCommand *pCmd);
    int SendObjectListByQueue(CCommand *pCmd);
    int SendObjectStatus(CCommand *pCmd);
    int SendStatusHelp(CCommand *pCmd);

private:
    int handleUpdate(CCommand *pCmd);
    int handleQuery(CCommand *pCmd);
    

};
#endif /* __H_STREAMERTHR__ */
