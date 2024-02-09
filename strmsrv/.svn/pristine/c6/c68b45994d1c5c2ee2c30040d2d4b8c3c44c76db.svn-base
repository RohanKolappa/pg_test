/*
 * detachedthread.hh
 *
 *  Created on: Feb 14, 2011
 *      Author: rkale
 */

#ifndef DETACHEDTHREAD_HH_
#define DETACHEDTHREAD_HH_

#include "appthread.hh"
#include "streamer.hh"

// Application threads attached to a streamer object should subclass
// from this instead of the CAppThread directly
// This class tries to ensure that the lifetime of the thread is tied to
// lifetime of the corresponding Streamer object.
// Subclasses should implement RunThread() instead of Run().
class CDetachedThread : public CAppThread
{
public:
    CDetachedThread(CStreamer *a_pStreamer, std::string threadName) : m_pStreamer(a_pStreamer) {

        // This a non-joinable thread;
        m_bDetached = true; // This a non-joinable thread;

        // Set up the thread name
        sprintf(m_pThreadName, "%s_%d", threadName.c_str(), a_pStreamer->GetId());

        // Add a reference to the parent object so that
        // it does not disappear until our thread is done
        m_pStreamer->AddReference();
    };

    virtual ~CDetachedThread() {
        // Remove our reference to the streamer object so that it can safely die
        m_pStreamer->RemoveReference();
    }

    // Pure virtual function that subclasses need to override
    virtual int RunThread() = 0;

    // Our Run() method cleans up after RunThread() returns
    int Run() {
        int rc = RunThread();

        // We are responsible to delete ourselves
        delete (this);

        return rc;
    };


    int GetId() { return m_pStreamer->GetId(); };

private:
    CStreamer *m_pStreamer;

};


#endif /* DETACHEDTHREAD_HH_ */
