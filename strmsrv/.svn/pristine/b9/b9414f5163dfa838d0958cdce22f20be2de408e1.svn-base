
#include "strmsrv_compatibility.h"
#include <sys/time.h>

#include "condition.hh"
#include "common.hh"

CCondition::CCondition() {

    // create mutex
    pthread_mutexattr_t mutexAttribute;
    pthread_mutexattr_init(&mutexAttribute);
    pthread_mutexattr_settype(&mutexAttribute, PTHREAD_MUTEX_FAST_NP);
    pthread_mutex_init (&m_mutex, &mutexAttribute);

    // create condition
    pthread_cond_init(&m_condition, NULL);

    // initial state is not signalled
    m_state = false;
}

CCondition::~CCondition() {
    if (pthread_mutex_destroy(&m_mutex) == EBUSY) {
        SYSLOG_ERROR("Warning: Some thread still owns the lock on this variable.");
    }
    if (pthread_cond_destroy(&m_condition) == EBUSY) {
        SYSLOG_ERROR("Warning: Some thread is still waiting on this condition.");
    }
}

void CCondition::Wait() {

    pthread_mutex_lock(&m_mutex);
    m_state = false;
    while (m_state == false) {
        pthread_cond_wait(&m_condition, &m_mutex);
    }
    pthread_mutex_unlock(&m_mutex);

}

void CCondition::TimedWait(unsigned long a_nMicroSeconds) {

    struct timespec ts;
    int rc = 0;

#ifdef WINDOWS
    timeval tvStart;
    gettimeofday(&tvStart, 0);
    ts.tv_sec = tvStart.tv_sec;
    ts.tv_nsec = tvStart.tv_usec * 1000;
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif
    ts.tv_nsec += a_nMicroSeconds * 1000;
    if (ts.tv_nsec > 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    pthread_mutex_lock(&m_mutex);
    m_state = false;
    while (m_state == false && rc == 0) {
        rc = pthread_cond_timedwait(&m_condition, &m_mutex, &ts);
    }
    pthread_mutex_unlock(&m_mutex);
}

void CCondition::Signal() {

    pthread_mutex_lock(&m_mutex);
    if (m_state == false) {
        m_state = true;
        pthread_cond_signal(&m_condition);
    }
    pthread_mutex_unlock(&m_mutex);
}

void CCondition::SignalAll() {

    pthread_mutex_lock(&m_mutex);
    if (m_state == false) {
        m_state = true;
        pthread_cond_broadcast(&m_condition);
    }
    pthread_mutex_unlock(&m_mutex);
}
