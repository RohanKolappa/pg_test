#ifndef __H_KIMEKEEPER__
#define __H_KIMEKEEPER__


#include <stdint.h>
#include "mutex.hh"

#define HIGHRES_TICKS_PER_SECOND 10000

typedef uint64_t timestamp_t;

class CTimeKeeper {
private:
    static timestamp_t m_currentTime;
    static timestamp_t m_startTime;
    static CMutex m_mutex;
#ifdef THREAD_LOCAL_TIME
    static pthread_key_t m_key;
    static void DestroyKey(void *ptr);
    static void MakeKey();
#endif

public:
    static void Init();
#ifdef THREAD_LOCAL_TIME
    static timestamp_t Now() { return (*(timestamp_t *) pthread_getspecific(m_key)); };
#else
    static timestamp_t Now() { return m_currentTime; };
#endif

    static void UpdateTime();
    static void Begin();
    static unsigned long Uptime() {return (unsigned long) (Now() - m_startTime);};
    static timestamp_t GetTime();
    static timestamp_t GetHighResTime();
    static timestamp_t GetTimeInMicroSec();
    static timestamp_t GetTimeStamp(struct timeval *ts);
    static void GetTimeVal(struct timeval *tv, timestamp_t ts);
    static char * GetDurationString(unsigned long a_milliseconds, char *a_buffer);
};


// inline long timeDiff(struct timeval &tnow, struct timeval &tprev) {
//         return (tnow.tv_sec - tprev.tv_sec)*1000000 +
//                 (tnow.tv_usec - tprev.tv_usec);
// }




#endif /* __H_KIMEKEEPER__ */
