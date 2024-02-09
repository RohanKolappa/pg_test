#ifndef __H_CONDITION__
#define __H_CONDITION__

#include <errno.h>
#include <pthread.h>


class CCondition {
private:
        pthread_mutex_t         m_mutex;
        pthread_cond_t          m_condition;
        bool                    m_state;
public:
        CCondition();
        ~CCondition();

        void Wait();
        void TimedWait(unsigned long a_nMicroSeconds);
        void Signal();
        void SignalAll();
};

#endif /* __H_CONDITION__ */
