/*
 * ratecontroller.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <math.h>
#include "common.hh"
#include "ratecontroller.hh"
#include "command.hh"

CRateController::CRateController(double fRate)
{
    SetRate(fRate);
}

CRateController::~CRateController()
{

}

void CRateController::SetRate(double fRate)
{
    m_nTokenPerUnit = 1000;
    m_fRate = fRate;

    // Scale burst size to the configured rate
    m_nBurstSize = (int)(m_fRate * m_nTokenPerUnit);
    if (m_nBurstSize < 5 * m_nTokenPerUnit)
        m_nBurstSize = 5 * m_nTokenPerUnit;

    m_nTokenBucket = m_nBurstSize;
    m_nBackoffCount = 0;
    m_nTokensPerTick = ((double)m_fRate * m_nTokenPerUnit)/(1.0 * HIGHRES_TICKS_PER_SECOND);
    m_tLastTimeStamp = CTimeKeeper::GetHighResTime();
}

bool CRateController::Allow(timestamp_t nCurrentTime)
{
    timestamp_t tNow = nCurrentTime;
    if (tNow == 0)
        tNow = CTimeKeeper::GetHighResTime();
    unsigned long long tDiff = (tNow - m_tLastTimeStamp);

    // Check against primary token bucket

    // Fill up bucket with tokens
    uint32_t nTokenBucket = m_nTokenBucket +
        (uint32_t)((double)tDiff * m_nTokensPerTick);

    if (nTokenBucket > m_nBurstSize) {
        nTokenBucket = m_nBurstSize;
    }

    // Do we have enough tokens to allow next unit??
    if (nTokenBucket <= m_nTokenPerUnit) {
        m_nBackoffCount++;
        return false;
    }
    m_nTokenBucket = nTokenBucket - m_nTokenPerUnit;

    // Set the timestamp for next call
    m_tLastTimeStamp = tNow;

    return true;
}


