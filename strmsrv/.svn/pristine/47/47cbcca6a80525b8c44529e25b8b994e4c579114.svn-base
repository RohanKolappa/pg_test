/*
 * meter.cpp
 *
 *  Created on: Dec 20, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include "meter.hh"


CMeter::CMeter(int a_nBurst)
{
    m_nBurst = a_nBurst;
    m_nMeter = m_nBurst;
    m_nBackoffCount = 0;
}

CMeter::~CMeter()
{

}


void CMeter::Fill()
{
    m_nMeter++;
    if (m_nMeter > m_nBurst)
        m_nMeter = m_nBurst;
}

bool CMeter::Allow()
{
    if (m_nMeter > 0) {
        m_nMeter--;
        return true;
    }
    m_nBackoffCount++;
    return false;
}
