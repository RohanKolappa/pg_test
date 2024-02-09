/*
 * splash.hh
 *
 *  Created on: Oct 18, 2009
 *      Author: rkale
 */

#ifndef SPLASH_HH_
#define SPLASH_HH_

#include <map>

#include "vsplashframe.hh"
#include "appthread.hh"
#include "mutex.hh"
#include "v2d.h"
#include "v2o_lib.h"

class CVideoSplashFrame;

class CSplash: public CAppThread {
public:

    CSplash(HAL *hp, bool a_bIsTx);

    ~CSplash();

    int Run();

    int GetLUT(struct LUT_struct *a_LUT, int & a_nMQuant, int & a_nDCQuant,
            int & a_nSliceSize, int & a_nCompMode, int & a_nMotionCompMode);

    int GetSplashType();

    int GetSCAP(ScapData_t *scap);

    int GetSplashBandwidth();

    int GetSplashPacket(char * a_buffer);

    bool IsReady();
    void SetFSRefresh();

private:

    unsigned short m_nHRes;
    unsigned short m_nVRes;
    unsigned short m_nOrigHRes;
    unsigned short m_nOrigVRes;
    double m_dRefresh;
    double m_dPCR;
    unsigned short m_nHfp;
    unsigned short m_nVfp;
    unsigned short m_nHbp;
    unsigned short m_nVbp;
    unsigned short m_nHSync;
    unsigned short m_nVSync;
    unsigned short m_nMQuant;
    unsigned short m_nDCQuant;
    unsigned short m_nSliceSize;
    bool m_bStereo;

    CVideoSplashFrame *m_pVideoSplashLeft;
    CVideoSplashFrame *m_pVideoSplashRight;

    bool m_bReady;
    bool m_bIsTx;
    bool m_bIsXpi;
    int m_nChannel;
    int m_nType;
    int m_nRate;
    int m_nLRFlag;
    bool m_bEnable;
    int  m_nCodecVersion;

    void InitTxSplash(HAL *hp);
    void InitRxSplash(HAL *hp);

    void InitializeVariables();

    bool CheckAnimationParameters();
    bool Animate();
    int MotionRun();
};

#endif /* SPLASH_HH_ */
