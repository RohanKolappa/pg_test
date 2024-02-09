#ifndef __H_MUTEX__
#define __H_MUTEX__

#ifdef SSDEBUG
#include <errno.h>
#endif
#include <pthread.h>

class CMutex {
public:
    CMutex();
    ~CMutex();
    void Lock();
    void Unlock();
    unsigned long GetContention();
    static unsigned long m_totalContention;

private:
    pthread_mutex_t m_mutex;
    unsigned long m_contention;
};

inline void CMutex::Lock() {
#ifdef SSDEBUG
    // In debug mode we try to measure the mutex contention rate,
    // ie try to count the number of times the mutex cannot be acquired
    // without blocking (some other thread has locked it)
    if (pthread_mutex_trylock(&m_mutex) == EBUSY) {
        m_totalContention++;
        m_contention++;
        pthread_mutex_lock(&m_mutex);
    }
#else
    pthread_mutex_lock(&m_mutex);
#endif
}

inline void CMutex::Unlock() {
    pthread_mutex_unlock(&m_mutex);
}

inline unsigned long CMutex::GetContention() {
    return m_contention;
}

class CScopedLock {
public:
    CScopedLock(CMutex &a_mutex);
    ~CScopedLock();

private:
    CMutex &m_mutex;

};

inline CScopedLock::CScopedLock(CMutex &a_mutex) : m_mutex(a_mutex) {
    m_mutex.Lock();
}

inline CScopedLock::~CScopedLock() {
    m_mutex.Unlock();
}


#endif /* __H_MUTEX__ */
