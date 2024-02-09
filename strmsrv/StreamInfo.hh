/*
 * StreamInfo.hh
 *
 *  Created on: Aug 8, 2012
 *      Author: rkale
 */

#ifndef STREAMINFO_HH_
#define STREAMINFO_HH_

class CStreamInfo {

public:
    CStreamInfo(double aSpeed, bool bTrickMode) :
        m_speed(aSpeed), m_bTrickMode(bTrickMode)
        { };
    double GetSpeed() const { return m_speed; };
    bool GetTrickMode() const { return m_bTrickMode; };

private:
    double  m_speed;
    bool m_bTrickMode;
};

#endif /* STARTOFSTREAMINFO_HH_ */
