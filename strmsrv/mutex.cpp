
#include "strmsrv_compatibility.h"
#include <errno.h>

#include "common.hh"
#include "mutex.hh"

unsigned long CMutex::m_totalContention;

CMutex::CMutex() {

    pthread_mutexattr_t mutexAttribute;
    pthread_mutexattr_init(&mutexAttribute);
    pthread_mutexattr_settype(&mutexAttribute, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init (&m_mutex, &mutexAttribute);
    m_contention = 0;
}

CMutex::~CMutex() {
    if (pthread_mutex_destroy(&m_mutex) == EBUSY) {
        SYSLOG_ERROR("Warning: Some thread still owns the lock on this variable.");
    }
}

