#ifndef __H_SENDER__
#define __H_SENDER__


#include "streamer.hh"
#include "squeue.hh"


/// The Sender Parent Class
class CSender : public CStreamer {
private:
protected:

    virtual void HandleFirstTimestamp(timestamp_t nFirstTimeStamp);
    virtual void CheckQueueTimestamp();
    virtual void CheckSyncStop();
    virtual void StoppingSender();
    virtual int StopSender();

    /// Destructor
    virtual ~CSender();

public:
    /// Constructor
    CSender(int a_nId);
    
    
    int StartStream(int nQueueId);
    int StopStream();

    virtual int StartSenderStream(int nQueueId);
    
};
#endif /* __H_SENDER__ */
