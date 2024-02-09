/*
 * CDataDropper.h
 *
 *  Created on: Apr 10, 2014
 *      Author: durga
 */

#ifndef CDATADROPPER_H_
#define CDATADROPPER_H_

#include <stdint.h>


class CDataDropper {
public:
    CDataDropper(double a_dPercentage, int a_nDropsRepeat = 1,
                 int a_nRandomSeed = 0);
    virtual ~CDataDropper();

    bool ShouldDrop();
    uint64_t GetDropCount();

private:
    void SetRandomSeed(int a_nRandomSeed);

    double m_dPercentage;
    int m_nDropsRepeat;
    int m_nRandomSeed;
    uint64_t m_nRandomNumber;
    int m_nDropCount;
    uint64_t m_nTotalDropCount;
    bool m_bDoDrop;
};

#endif /* CDATADROPPER_H_ */
