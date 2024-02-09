
#include "strmsrv_compatibility.h"
#include <sched.h>
#ifndef WINDOWS
#include <sys/syscall.h>
#endif

#include "appthread.hh"
#include "common.hh"

/// Constructor for the Generic Application thread

bool CAppThread::m_bUseRealtime = true;

CAppThread::CAppThread()
{
    m_bExit = 0;
    m_pThreadName = new char[64];
    m_priority = 10;  // Default priority set to 10
    m_bDetached = false; // Thread is joinable by default;
    m_bNice = false; // If true, be nicer than default for non real-time threads.

    m_affinity = -1; // Affinity disabled by default

    if (m_pThreadName != NULL)
        strcpy(m_pThreadName, "FixMe");


    m_nSleepCount = 0;
    m_nSleepTooLongCount = 0;
    m_avgSleepTime = 0.0;
    m_avgComputeTime = 0.0;

    gettimeofday (&m_nt1, NULL);
    gettimeofday (&m_nt2, NULL);

}

/// Destructor for the Generic Application thread
CAppThread::~CAppThread()
{
    delete[] m_pThreadName;
}

/// Start the thread
int CAppThread::Start()
{
    int rc;
    // Create thread attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Set thread priority
    if (m_priority > 0 && m_bUseRealtime) {
        struct sched_param schedParam;
        memset (&schedParam, 0, sizeof(schedParam));
        pthread_attr_getschedparam(&attr, &schedParam);

        int min_priority = sched_get_priority_min(SCHED_RR);
        schedParam.sched_priority = min_priority + m_priority;
        SYSLOG_DEBUG("Setting sched policy %d, priority = %d, max = %d\n",
                     SCHED_RR, schedParam.sched_priority,
                     sched_get_priority_max(SCHED_RR));
        // PSCW - 10/01/2012: changed call sequence to set policy to SCHED_RR, 
        // then set priority (sourceware.org/bugzilla/show_bug.cgi?id=10828) 
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if ((rc = pthread_attr_setschedparam(&attr, &schedParam)) != 0) {
            SYSLOG_ERROR("Failed setting schedParam priority. rc %d.", rc);
        }
    } else {
        // Could not get the new thread to be a normal (non realtime) thread
        // to work here using "attr". So let the thread be created
        // with whatever pthreads creates and then change it later from
        // within the threads start method
        SYSLOG_DEBUG("The thread %s is supposed to be non-realtime.", m_pThreadName);
    }


    // Create thread
    rc = pthread_create(&m_tId, &attr, CAppThread::EntryPoint, this);
    if (rc != 0) {
        // Major error in application framework. Best to abort
        SYSLOG_ERROR("Could not create thread: %s. Aborting..." , strerror(errno));
        exit(0);
    }

    // Detach thread if needed
    if (m_bDetached == true) {
        if (pthread_detach(m_tId)) {
            SYSLOG_ERROR("Could not detach thread %s", m_pThreadName);
        }
    }

    pthread_attr_destroy(&attr);


    return 0;
}

void * CAppThread::EntryPoint(void *pThis)
{

    CAppThread * pThr = static_cast<CAppThread *> (pThis);


    AddThreadName(pThr->m_pThreadName);  // used for debugging

    // Initialize the per thread TimeKeeper data
    CTimeKeeper::Init();

#ifndef WINDOWS
    SYSLOG_INFO("Taskid: %lu", syscall(SYS_gettid));
#endif

    if (pThr->m_affinity >= 0) {
#ifndef WINDOWS
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(pThr->m_affinity, &cpuset);
        if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
            SYSLOG_ERROR("Could not set cpu affinity to %d for %s",
                         pThr->m_affinity, pThr->m_pThreadName);
        }
#endif

    }

    // If thread priority was set to zero, make this a "normal" thread
    // ie, non realtime thread
    {
        if (pThr->m_priority == 0 && m_bUseRealtime) {
            struct sched_param schedParam;
            memset (&schedParam, 0, sizeof(schedParam));

            SYSLOG_DEBUG("Changing current threads scheduler to normal");
            int rc = pthread_setschedparam(pThr->m_tId, SCHED_OTHER, &schedParam);
            SYSLOG_DEBUG("pthread_setschedparam: %d %d", rc, errno);

#ifndef WINDOWS
            // We can additionally be nicer than other normal processes on the system
            if (pThr->m_bNice)
                nice(5);
#endif

        }
    }

    {
#ifndef WINDOWS
        struct sched_param schedParam;
        int policy = sched_getscheduler(0);
        sched_getparam(0, &schedParam);
        SYSLOG_DEBUG("Sched policy for %x = %d, priority = %d\n",
                     (int) pThr->m_tId, policy, schedParam.sched_priority);
#endif
    }

    // Call the thread's custom Run() method. The method is expected
    // to quit the Run() main loop (if any) when m_bExit is set to 1
    pThr->Run();

    // Remove thread name
    RemoveThreadName();

    //pthread_exit(NULL);
    return NULL;
}


/// Stop the thread
int CAppThread::Stop()
{
    //int nThreadStatus = 0;

    // Set flag that will cause the thread to break out of its main
    // Run() loop
    m_bExit = 1;

    // Wait for the thread to exit
    if (m_bDetached == false) {
        //pthread_join ( m_tId, (void **)&nThreadStatus);
        pthread_join(m_tId, NULL);
    }
    else {
        SYSLOG_DEBUG("Not waiting for a join for thread %s\n", m_pThreadName);
    }


    return 0;
}

void
CAppThread::Sleep(int a_milliseconds)
{

#ifndef WINDOWS
    struct timespec tp;
    tp.tv_sec = 0;
    tp.tv_nsec = (a_milliseconds * 1000000);
    nanosleep (&tp, NULL);
#else
    usleep(a_milliseconds * 1000);
#endif
}

void
CAppThread::LightSleep()
{

#ifndef WINDOWS
    struct timespec tp;

    //return PrioritySleep(1);

    //m_condition.TimedWait(500);
    //return;

    tp.tv_sec = 0;
    tp.tv_nsec = (1000000 - 1000);
    nanosleep (&tp, NULL);
#else
    usleep(1000);
#endif
}

#define ALPHA 0.05
#define DO_AVGTIME

void
CAppThread::PrioritySleep(int a_milliseconds)
{
    m_nSleepCount++;

    //struct timespec tp;
    int delta;

    //tp.tv_sec = 0;
    //tp.tv_nsec = (1000000 - 1000) * a_milliseconds;


    gettimeofday (&m_nt1, NULL);

#ifdef DO_AVGTIME
    delta = (m_nt1.tv_sec - m_nt2.tv_sec) * 1000000 +
            (m_nt1.tv_usec - m_nt2.tv_usec);
    if (delta > 5000*a_milliseconds) {
        SYSLOG_INFO("Compute Too Long -- %d\n", delta);
    }
    if (m_avgComputeTime < 0.00001)
        m_avgComputeTime = delta;
    m_avgComputeTime = (double)ALPHA * delta +
                       (double)(1.0 - ALPHA) * m_avgComputeTime;

    if (m_nSleepCount % 1000 == 0) {
        SYSLOG_INFO("Compute time =  %f\n",
                    m_avgComputeTime);
    }
#endif

    //     nanosleep (&tp, NULL);
#ifndef WINDOWS
    m_condition.TimedWait(500);
#else
    //usleep(500);
    ::Sleep(1);
#endif
    gettimeofday (&m_nt2, NULL);

    delta = (m_nt2.tv_sec - m_nt1.tv_sec) * 1000000 +
            (m_nt2.tv_usec - m_nt1.tv_usec);

#ifdef DO_AVGTIME
    if (m_avgSleepTime < 0.00001)
        m_avgSleepTime = delta;
    m_avgSleepTime = (double)ALPHA * delta +
                     (double)(1.0 - ALPHA) * m_avgSleepTime;

    if (m_nSleepCount % 1000 == 0) {
        SYSLOG_INFO("Sleep time = %f, Sleep counts: %lu %lu\n",
                    m_avgSleepTime, m_nSleepCount, m_nSleepTooLongCount);
    }
#endif

    delta = delta / 1000;
    if (delta > 5*a_milliseconds) {
        //SYSLOG_INFO("Sleep Too Long -- %d\n", delta);
        m_nSleepTooLongCount++;
    }
}


void
CAppThread::YieldThread()
{
    sched_yield();
}

void
CAppThread::DisableRealtime()
{
    m_bUseRealtime = false;
}
