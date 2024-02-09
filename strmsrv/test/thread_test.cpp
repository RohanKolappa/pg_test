/*
 * thread_test.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: rkale
 */

#ifndef THREAD_TEST_CPP_
#define THREAD_TEST_CPP_


#include "strmsrv_compatibility.h"

#include "gtest/gtest.h"
#include "appthread.hh"
#include "detachedthread.hh"
#include "watcher.hh"
#include "common.hh"

class CThreadTest : public testing::Test {
protected:
    virtual void SetUp() {
        DisableSSSyslog();
        CAppThread::DisableRealtime();
    }

    virtual void TearDown() {
    }

private:
};


class TestAppThread : public CAppThread
{
public:
    TestAppThread(bool bDetached) : m_bHasStarted(false), m_bHasStopped(false) {
        m_bDetached = bDetached;
    };
    int Run() {
        m_bHasStarted = true;
        while (!m_bExit) {
            LightSleep();
        }
        m_bHasStopped = true;
        return 0;
    };

    bool HasStarted() {return m_bHasStarted; }
    bool HasStopped() {return m_bHasStopped; }
private:
    bool m_bHasStarted;
    bool m_bHasStopped;
};


TEST_F(CThreadTest, ThreadStart) {

    TestAppThread * appThread = new TestAppThread(false);
    CWatcher<TestAppThread> watcher(appThread);

    appThread->Start();

    EXPECT_TRUE(watcher.WaitFor(&TestAppThread::HasStarted));

    appThread->Stop();
    delete appThread;
}

TEST_F(CThreadTest, JoinableThreadStop) {

    TestAppThread * appThread = new TestAppThread(false);

    appThread->Start();
    appThread->Stop();

    // Since this thread is joinable, by the time stop returns,
    // the thread should have exited
    EXPECT_TRUE(appThread->HasStopped());

    delete appThread;
}

TEST_F(CThreadTest, DetachedThreadStop) {

    TestAppThread * appThread = new TestAppThread(true);
    CWatcher<TestAppThread> watcher(appThread);

    appThread->Start();
    appThread->Stop();

    // Since this thread is detached, by the time stop returns,
    // the thread may not have exited, but eventually it should
    EXPECT_TRUE(watcher.WaitFor(&TestAppThread::HasStopped));

    delete appThread;
}

class TestDetachedThread : public CDetachedThread
{
public:
    TestDetachedThread(CStreamer *a_pStreamer, bool & a_bIsDeleted)
    : CDetachedThread(a_pStreamer, "test"), m_bIsDeleted(a_bIsDeleted) {};
    virtual ~TestDetachedThread() {
        // We want to ensure delete is called
        m_bIsDeleted = true;
    }
    int RunThread() {
        while (!m_bExit) {
            LightSleep();
        }
        return 0;
    };

private:
    bool & m_bIsDeleted;

};

class MockStreamer : public CStreamer {
public:
    MockStreamer(int a_nId, bool & a_bIsDeleted) :  CStreamer(a_nId), m_bIsDeleted(a_bIsDeleted) { };
    ~MockStreamer() {
        // We want to ensure delete is called
        m_bIsDeleted = true;
    }
    virtual int ProcessStream() {
        return 0;
    }
private:
    bool & m_bIsDeleted;
};


TEST_F(CThreadTest, DetachableThread) {

    bool streamerDeleted = false;
    bool threadDeleted = false;
    MockStreamer *streamer = new MockStreamer(1234, streamerDeleted);
    TestDetachedThread * detachedThread = new TestDetachedThread(streamer, threadDeleted);

    detachedThread->Start();

    // Stopping the thread should eventually end up deleting
    // both the streamer and the thread
    detachedThread->Stop();

    CWatcher<MockStreamer> watcher(streamer);
    EXPECT_TRUE(watcher.WaitFor(streamerDeleted));
    EXPECT_TRUE(watcher.WaitFor(threadDeleted));
}

#endif /* THREAD_TEST_CPP_ */
