/*
 * ratecontroller.hh
 *
 *  Created on: Dec 17, 2010
 *      Author: rkale
 */

#ifndef RATECONTROLLER_HH_
#define RATECONTROLLER_HH_

#include "timekeeper.hh"

class CRateController
{
public:
    CRateController(double fRate);
    ~CRateController();

    void SetRate(double fRate);
    bool Allow(timestamp_t nCurrentTime = 0);
    double GetRate() { return m_fRate; };
    uint64_t GetBackoffCount() { return m_nBackoffCount; };

private:
    double m_fRate;
    uint32_t m_nTokenBucket;
    uint64_t m_nBackoffCount;
    uint32_t m_nTokenPerUnit;
    uint32_t m_nBurstSize;
    double m_nTokensPerTick;
    timestamp_t m_tLastTimeStamp;
};
#endif /* RATECONTROLLER_HH_ */
