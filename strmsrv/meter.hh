/*
 * meter.hh
 *
 *  Created on: Dec 20, 2010
 *      Author: rkale
 */

#ifndef METER_HH_
#define METER_HH_

class CMeter
{
public:
    CMeter(int a_nBurst = 4);
    ~CMeter();

    void Fill();
    bool Allow();
    uint64_t GetBackoffCount() { return m_nBackoffCount; };

private:
    int m_nBurst;
    int m_nMeter;
    uint64_t m_nBackoffCount;
};


#endif /* METER_HH_ */
