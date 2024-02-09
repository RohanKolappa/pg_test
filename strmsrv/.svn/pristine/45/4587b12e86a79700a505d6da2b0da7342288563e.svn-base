
#include "strmsrv_compatibility.h"

#include "common.hh"
#include "timekeeper.hh"
#include "mutex.hh"

timestamp_t
CTimeKeeper::m_currentTime;
timestamp_t
CTimeKeeper::m_startTime;
CMutex
CTimeKeeper::m_mutex;

#ifdef THREAD_LOCAL_TIME
// The variable storing the current time is now thread local instead
// of a global variable. This will ensure that threads running on different
// processors will not contend of access to this memory location
pthread_key_t 
CTimeKeeper::m_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

void 
CTimeKeeper::DestroyKey(void *ptr) {
    if (ptr != NULL)
        free(ptr);
}

void 
CTimeKeeper::MakeKey() {
    (void) pthread_key_create(&m_key, DestroyKey);
}

#endif // THREAD_LOCAL_TIME

void
CTimeKeeper::Init()
{
#ifdef THREAD_LOCAL_TIME
    void *ptr;
    (void) pthread_once(&key_once, MakeKey);
    if ((ptr = pthread_getspecific(m_key)) == NULL) {
        // TODO: free this memory uisng destructor on pthread_key_create();
        SYSLOG_INFO("Creating thread local storage for time\n");
        ptr = malloc(sizeof(timestamp_t));
        (void) pthread_setspecific(m_key, ptr);
    }
    UpdateTime();
    SYSLOG_INFO("Geting first time %lld\n", Now());
#endif // THREAD_LOCAL_TIME
}


/// Updates the current time using gettimeofday() system call.
/// To avoid system call hit all over our code, we use this
/// function to update the current time occasionally and the
/// Now() function just returns the current time
void
CTimeKeeper::UpdateTime()
{
    CScopedLock lock(m_mutex);
    struct timeval ts;
    gettimeofday(&ts, NULL);
#ifndef THREAD_LOCAL_TIME
    m_currentTime =
        ((unsigned long long) ts.tv_sec) * 1000 +
        (ts.tv_usec/1000);
#else
    timestamp_t * currentTime = (timestamp_t *) pthread_getspecific(m_key);
    ASSERT(currentTime != NULL);
    *currentTime = 
        ((unsigned long long) ts.tv_sec) * 1000 +
        (ts.tv_usec/1000);
#endif
}

#if 0
timestamp_t 
CTimeKeeper::Now() { 
    timestamp_t * currentTime = (timestamp_t *) pthread_getspecific(m_key);
    ASSERT(currentTime != NULL);
    return *currentTime; 
}
#endif

void
CTimeKeeper::Begin()
{
    struct timeval ts;
    gettimeofday(&ts, NULL);
    m_startTime =
        ((unsigned long long) ts.tv_sec) * 1000 +
        (ts.tv_usec/1000);

}

/// Return the real current time (non-cached -- use gettimeofday())
timestamp_t
CTimeKeeper::GetTime()
{
    struct timeval ts;
    gettimeofday(&ts, NULL);
    timestamp_t tTimeStamp =  (((unsigned long long) ts.tv_sec) * 1000 +
                          (ts.tv_usec/1000));

    return tTimeStamp;
}

/// Return the real current time (non-cached -- use gettimeofday())
timestamp_t
CTimeKeeper::GetHighResTime()
{

    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (timestamp_t) (((unsigned long long) ts.tv_sec) * HIGHRES_TICKS_PER_SECOND +
                          (ts.tv_usec / (1000000/HIGHRES_TICKS_PER_SECOND)));

}

/// Return the real current time (non-cached -- use gettimeofday())
timestamp_t
CTimeKeeper::GetTimeInMicroSec()
{

    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (timestamp_t) (((unsigned long long) ts.tv_sec) * 1000000 + 
                           ts.tv_usec);

}

/// Return time duration in days:hr:min:sec.milliseconds format.
/// Returns pointer to the provided buffer expected to be at leat 32 bytes long
char * 
CTimeKeeper::GetDurationString(unsigned long a_milliseconds, char *a_buffer)
{
    int seconds = a_milliseconds/1000;
    int ms = a_milliseconds%1000;
    int days = seconds/86400;
    seconds = seconds%86400;
    int hours = seconds/3600;
    seconds = seconds%3600;
    int mins = seconds/60;
    seconds = seconds%60;
    int len = 0;
    if (days > 0)
        len += sprintf(a_buffer, "%d:", days);
    snprintf(a_buffer + len, 32 - len, "%02d:%02d:%02d.%03d",
             hours, mins, seconds, ms);
    return a_buffer;
}


timestamp_t
CTimeKeeper::GetTimeStamp(struct timeval *ts)
{

    return ((unsigned long long) ts->tv_sec) * 1000 +
        (ts->tv_usec/1000);

}

void
CTimeKeeper::GetTimeVal(struct timeval *tv, timestamp_t ts)
{
  tv->tv_sec = ts / 1000;
  tv->tv_usec = (ts % 1000) * 1000;

  return;
}

