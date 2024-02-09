
#include "strmsrv_compatibility.h"

#include <sstream>
//#include <dirent.h>

#include "deleter.hh"
#include "squeue.hh"
#include "diskio.hh"
#include "common.hh"



CDeleter::CDeleter(int nId, char *a_directory) :
    CStreamer(nId)
{

    m_nPortType = DELETERPORT;

    m_pDirectoryName = new char[MAXFILENAMELEN + 1];

    sprintf (m_pDirectoryName, "%s", a_directory);

    // Remove trailing slash on directory name
#ifdef WINDOWS
    char * cp = strchr(m_pDirectoryName, '/');
#else
    char * cp = rindex(m_pDirectoryName, '/');
#endif
    if (cp != NULL && *(cp + 1) == '\0')
        *cp = '\0';

    // Create and start the Deleter DiskIO thread
    m_pDeleterDiskIOThread = new CDeleterDiskIOThread(this);
    m_pDeleterDiskIOThread->Start();

}

CDeleter::~CDeleter()
{
    delete[] m_pDirectoryName;
}

void
CDeleter::DeleteStream()
{
    // Stop the deleter thread
    m_pDeleterDiskIOThread->Stop();

    // Remove our own (streamer) reference also.
    RemoveReference();
}


int 
CDeleter::ProcessStream()
{

    if (IsErrored())
        return 0;

    /* Nothing to do here... everything happens in the deleterdiskiothr */

    return 0;
}

/// Assign a Queue to the Deleter
int 
CDeleter::StartStream(int nQueueId)
{
    // The deleter does not listen on any queue yet
    return 0;
};

/// Remove the Queue Assignment
int 
CDeleter::StopStream()
{
    // Nothing to do here
    return 0;
}


void
CDeleter::SetNotification(int a_nEventId)
{
    if (m_bSendNotify == false) {
        m_nEventId = a_nEventId;
        m_bSendNotify = true;
    }
}

void
CDeleter::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("DirectoryName", m_pDirectoryName); 
    a_pCmd->SetObjectCounterParam("Deletes", 
                                  m_pDeleterDiskIOThread->GetDeleteOperations());
    a_pCmd->SetObjectCounterParam("FileDeletes",
                                  m_pDeleterDiskIOThread->GetFileDeleteOperations());
    a_pCmd->SetObjectCounterParam("DirectoryDeletes",
                                  m_pDeleterDiskIOThread->GetDirectoryDeleteOperations());
    a_pCmd->SetObjectParam("DeletedBytes",
                                  m_pDeleterDiskIOThread->GetDeletedBytes());
//    a_pCmd->SetObjectParam("DeletePending", m_bDeletePending);
    a_pCmd->SetObjectParam("DeleteDelays",
                           m_pDeleterDiskIOThread->GetDelayedDeletes());
    a_pCmd->SetObjectCounterParam("ForceDeletes", 
                                  m_pDeleterDiskIOThread->GetForceDeleteOperations());
}

void
CDeleter::PrintNotification(CNotify *a_notify)
{
    switch (m_nEventId) {
    case NOTIFICATION_EVENT_PROGRESS:

        a_notify->SetNotificationData("MediaDirectory", m_pDirectoryName);
        a_notify->SetNotificationData("FileDeletes",
                                      m_pDeleterDiskIOThread->GetFileDeleteOperations());
        a_notify->SetNotificationData("DirectoryDeletes",
                                      m_pDeleterDiskIOThread->GetDirectoryDeleteOperations());
        a_notify->SetNotificationData("DeletedBytes",
                                      m_pDeleterDiskIOThread->GetDeletedBytes());

        break;
    default:
        break;
    }
}




