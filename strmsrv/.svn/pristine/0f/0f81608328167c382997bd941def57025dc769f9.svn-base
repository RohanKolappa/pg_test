/*
 * AdvancedPTSGenerator.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: durga
 */

#include "TimeStampAdjuster.hh"
#include <math.h>


CTimeStampAdjuster::CTimeStampAdjuster()
{
    m_nAdjustedTS       = 0;
    m_nPrevTimestamp    = 0;
    m_nTotalGaps        = 0;
    m_bDiscontinuity    = false;
}

CTimeStampAdjuster::~CTimeStampAdjuster()
{
}

uint64_t CTimeStampAdjuster::GetAdjustedTS(uint64_t a_nInputTimestamp)
{
    if (m_nPrevTimestamp == 0) {
        m_nPrevTimestamp = a_nInputTimestamp;
        m_bDiscontinuity = false;

        return m_nPrevTimestamp;
    }

    if (m_bDiscontinuity) {
        m_nTotalGaps += (a_nInputTimestamp - m_nPrevTimestamp);
        m_bDiscontinuity = false;
    }

    m_nAdjustedTS = (a_nInputTimestamp - m_nTotalGaps);

    if (m_nPrevTimestamp < a_nInputTimestamp) {
        m_nPrevTimestamp = a_nInputTimestamp;
    }

    return m_nAdjustedTS;
}

void CTimeStampAdjuster::SetDiscontinuity()
{
    m_bDiscontinuity = true;
}
