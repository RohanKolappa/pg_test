#ifndef __H_APPTHREAD__
#define __H_APPTHREAD__

#include <sys/time.h>
#include <pthread.h>

#include "condition.hh"
#include "timekeeper.hh"


/// The Generic Application Thread Class

class CAppThread
{
private:
    pthread_t                       m_tId;
    CCondition                      m_condition;
    unsigned long                   m_nSleepCount;
    double                          m_avgSleepTime;
    double                          m_avgComputeTime;
    unsigned long                   m_nSleepTooLongCount;
    struct timeval                  m_nt1;
    struct timeval                  m_nt2;
    static bool                     m_bUseRealtime;

    void PrioritySleep(int a_milliseconds);

protected:
    char *                          m_pThreadName;
    int                             m_priority;
    int                             m_affinity;
    bool                            m_bDetached;
    bool                            m_bNice;

public:
    int                             m_bExit;

    CAppThread();
    virtual ~CAppThread();
    int Start();
    int Stop();
    void Sleep(int a_milliseconds);
    void LightSleep();
    void YieldThread();
    static void * EntryPoint(void *);
    virtual int Run() = 0;
    static void DisableRealtime();

};
#endif /* __H_APPTHREAD__ */
