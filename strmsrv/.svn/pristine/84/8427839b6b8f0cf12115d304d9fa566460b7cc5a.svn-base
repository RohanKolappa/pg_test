/*
 * encodermutex.hh
 *
 *  Created on: Nov 23, 2009
 *      Author: rkale
 */

#ifndef ENCODERMUTEX_HH_
#define ENCODERMUTEX_HH_

#include "vframeencoder.hh"

// Frame encoder Mutex wrapped around StreamingServer's Mutex class
class CEncoderMutex : public IEncoderMutex
{
public:
    void Lock() {
        m_Mutex.Lock();
    };

    void Unlock() {
        m_Mutex.Unlock();
    }
private:
    CMutex m_Mutex;
};

#endif /* ENCODERMUTEX_HH_ */
