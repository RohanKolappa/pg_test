/*
 * CTimestampHandler.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: durga
 */

#include "PTSGenerator.hh"
#include <math.h>

CPTSGenerator::CPTSGenerator(uint32_t a_nWallClockTicksPerSec,
                             uint32_t a_nRTPTicksPerSec)
{
    m_nPTS               = 0;
    m_nPrevRTPTimestamp  = 0;
    m_nPrevWallClockTime = 0;
    m_nGaps              = 0;
    m_bDiscontinuity     = false;

    m_nWallClockTicksPerSec = a_nWallClockTicksPerSec;//Typically it is 1000, if the wall-clock is in milliseconds.
    m_nRTPTicksPerSec       = a_nRTPTicksPerSec; //Typically it is 90000 for Video, and 8000/16000/etc for Audio.
}

CPTSGenerator::~CPTSGenerator()
{
}

uint64_t CPTSGenerator::GetPTSForRTPTimestamp(uint32_t a_nRTPTimestamp,
                                              uint64_t a_nCurrentWallClock)
{
    if (m_bDiscontinuity) {
        m_nPTS += ComputeDiscontinuityAdjustment(a_nCurrentWallClock) - m_nGaps;

        m_nPrevRTPTimestamp = a_nRTPTimestamp;

        return m_nPTS;
    }

    if (m_nPrevRTPTimestamp == 0) {
        m_nPrevRTPTimestamp = a_nRTPTimestamp;
    }

    m_nPTS += (a_nRTPTimestamp - m_nPrevRTPTimestamp);

    m_nPrevRTPTimestamp = a_nRTPTimestamp;
    m_nPrevWallClockTime = a_nCurrentWallClock;

    return m_nPTS;
}

uint64_t CPTSGenerator::ComputeDiscontinuityAdjustment(uint64_t a_nCurrentWallClock)
{
    if (m_nPrevWallClockTime <= 0) { //Ignoring initial silence, if the stream started with silence.
        ResetDiscontinuityCounters();

        return 0;
    }

    uint64_t nWallClockDiff = (a_nCurrentWallClock - m_nPrevWallClockTime);
    uint64_t nAdjustedPTS = (uint64_t)round((((double)nWallClockDiff * m_nRTPTicksPerSec) /
                                                        (double)m_nWallClockTicksPerSec)) - m_nGaps;
    ResetDiscontinuityCounters();

    return nAdjustedPTS;
}

void CPTSGenerator::SetDiscontinuity(uint64_t a_nGaps)
{
    m_bDiscontinuity = true;

    m_nGaps += (uint64_t) round((((double)a_nGaps * m_nRTPTicksPerSec) /
                                    (double)m_nWallClockTicksPerSec));
}

void CPTSGenerator::ResetDiscontinuityCounters()
{
    m_bDiscontinuity = false;
    m_nGaps = 0;
}
