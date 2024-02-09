


#include "strmsrv_compatibility.h"
#include "processorthr.hh"
#include "common.hh"

using namespace std;

/// Constructor
CProcessorThread::CProcessorThread(string a_name, int a_affinity)
{
    // Set the thread name string
    if (m_pThreadName != NULL) {
        strcpy(m_pThreadName, a_name.c_str());
    }
    SYSLOG_INFO("Creating Thread with name %s", m_pThreadName);

    // Set the cpu affinity
    m_affinity = a_affinity;

    m_qControllerSink =
        CSQueueManager::Instance()->GetQueueSink("Controller");

}

/// Destructor
CProcessorThread::~CProcessorThread()
{

    CSQueueManager::Instance()->ReleaseQueueSink(m_qControllerSink);

}

/// Main run routine
int CProcessorThread::Run()
{
    int count;

    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        count = 0;


        count += ProcessStreamers();

        if (count == 0)
            LightSleep();

    }

    return 0;
}

int
CProcessorThread::ProcessStreamers()
{
    int retval=0;
    list<CStreamer *>::iterator iter;

    CScopedLock lock(m_mutex);

    for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {

        CStreamer * streamer = *iter;

        /* Call the main process stream method on each object */
        if (streamer->ProcessStream() > 0) {
            retval++;
        }

        /* If a notification is created by the object, send it out to
           the controller */
        if (streamer->GetNotifyFlag()) {
            string str;
            streamer->SendNotification(str);
            if (str.length() > 0) {
                CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
                m_qControllerSink->InsertFrame(sFrame);
            }
        }

        /* If an error notification is created by the object, send it out to
           the controller */
        if (streamer->GetErrorFlag()) {
            string str;
            streamer->SendError(str);
            if (str.length() > 0) {
                CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
                m_qControllerSink->InsertFrame(sFrame);
            }
        }
    }
    return retval;
}


void
CProcessorThread::AddStreamer(CStreamer * a_streamer)
{
    CScopedLock lock(m_mutex);
    SYSLOG_INFO("Adding a streamer: %d to %s thread",
                a_streamer->GetId(), m_pThreadName);
    m_LStreamers.push_back(a_streamer);
}


void
CProcessorThread::RemoveStreamer(CStreamer * a_streamer)
{

    list<CStreamer *>::iterator iter;

    CScopedLock lock(m_mutex);
    SYSLOG_INFO("Removing a streamer: %d from %s thread",
                a_streamer->GetId(), m_pThreadName);
    for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {
        if (a_streamer->GetId() != (*iter)->GetId())
            continue;
        ASSERT(a_streamer == (*iter));
        (*iter)->DeleteStream();
        m_LStreamers.erase(iter);
        break;
    }

    return;

}
