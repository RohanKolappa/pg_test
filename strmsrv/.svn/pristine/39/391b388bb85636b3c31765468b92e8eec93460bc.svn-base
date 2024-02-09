#ifndef __H_DELETER__
#define __H_DELETER__


#include "streamer.hh"
#include "deleterdiskiothr.hh"

// Forward Declarations
class CDeleterDiskIOThread;

/// The Deleter Parent Class
class CDeleter : public CStreamer {

public:

    CDeleter(int nId,
             char *a_directory);

    ~CDeleter();

    int ProcessStream();
    int StartStream(int nQueueId);
    int StopStream();
    void DeleteStream();

    void SetNotification(int a_nEventId);
    void PrintNotification(CNotify *a_notify);

    void UpdateCustomStatus(CCommand *a_pCmd);
    char * GetDirectoryName() {
        return m_pDirectoryName;
    }

private:
    CDeleterDiskIOThread *      m_pDeleterDiskIOThread;
    char *                      m_pDirectoryName;

};
#endif /* __H_DELETER__ */
