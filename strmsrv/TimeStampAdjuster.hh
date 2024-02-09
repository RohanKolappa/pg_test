/*
 * CAdvancedPTSGenerator.h
 *
 *  Created on: Aug 5, 2013
 *      Author: durga
 */

#include "strmsrv_compatibility.h"


#ifndef CADVANCEDPTSGENERATOR_H_
#define CADVANCEDPTSGENERATOR_H_


class CTimeStampAdjuster {
public:
    CTimeStampAdjuster();
    virtual ~CTimeStampAdjuster();

    uint64_t GetAdjustedTS(uint64_t a_nInputTimestamp);
    void SetDiscontinuity();

private:
    uint64_t m_nAdjustedTS;
    uint64_t m_nPrevTimestamp;
    uint64_t m_nTotalGaps;
    bool     m_bDiscontinuity;
};

#endif /* CADVANCEDPTSGENERATOR_H_ */
