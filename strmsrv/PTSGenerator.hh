/*
 * CTimestampHandler.h
 *
 *  Created on: Jun 6, 2013
 *      Author: durga
 */
#include "strmsrv_compatibility.h"
#include "averager.hh"

#ifndef CPTSGENERATOR_H_
#define CPTSGENERATOR_H_

class CPTSGenerator {
public:
    CPTSGenerator(uint32_t a_nWallClockTicksPerSec,
                  uint32_t a_nRTPTicksPerSec);
    virtual ~CPTSGenerator();

    uint64_t GetPTSForRTPTimestamp(uint32_t a_nRTPTimestamp,
                                   uint64_t a_nCurrentWallClock = 0);
    void SetDiscontinuity(uint64_t a_nGaps = 0);

    void AdjustPTSWithSilence(uint64_t a_nDuration);
private:
    uint64_t m_nPTS;
    uint32_t m_nPrevRTPTimestamp;
    uint64_t m_nPrevWallClockTime;
    uint64_t m_nGaps;

    uint32_t m_nWallClockTicksPerSec;
    uint32_t m_nRTPTicksPerSec;

    bool m_bDiscontinuity;

    void ResetDiscontinuityCounters();
    uint64_t ComputeDiscontinuityAdjustment(uint64_t a_nCurrentWallClock);
};

#endif /* CPTSGENERATOR_H_ */
