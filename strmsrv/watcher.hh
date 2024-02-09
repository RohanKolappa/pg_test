/*
 * watcher.hh
 *
 *  Created on: Feb 14, 2011
 *      Author: rkale
 */

#ifndef WATCHER_HH_
#define WATCHER_HH_

#include "timekeeper.hh"

template <class T>
class CWatcher {

public:
    CWatcher(T *aTarget) : m_nTarget(aTarget) {
        m_nTimeout = 1000;
        SetStartTime();
    }

    bool Done() {
       if ((CTimeKeeper::GetTime() - m_nBeginTime) > m_nTimeout)
           return true;
       return false;
    }

    // WaitFor functions wait for the the given condition to be
    // satisfied within the timeout period
    bool WaitFor(bool (T::*func) (void) ) {
        SetStartTime();
        while (!Done()) {
            if ((m_nTarget->*func)()) {
                return true;
            }
        }
        return false;
    }

    bool WaitFor(bool &m_aVariable) {
        SetStartTime();
        while (!Done()) {
            if (m_aVariable) {
                return true;
            }
        }
        return false;
    }

    bool WaitFor(uint64_t (T::*func) (void), uint64_t aValue) {
        SetStartTime();
        while (!Done()) {
            if ((m_nTarget->*func)() == aValue) {
                return true;
            }
        }
        return false;
    }

    bool WaitFor(int (T::*func) (void), int aValue) {
        SetStartTime();
        while (!Done()) {
            if ((m_nTarget->*func)() == aValue) {
                return true;
            }
        }
        return false;
    }

private:
    T *m_nTarget;
    timestamp_t m_nBeginTime;
    unsigned long m_nTimeout;

    void SetStartTime() {
        m_nBeginTime = CTimeKeeper::GetTime();
    }

};

#endif /* WATCHER_HH_ */
