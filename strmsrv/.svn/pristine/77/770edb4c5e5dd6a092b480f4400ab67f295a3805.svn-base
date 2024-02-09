/*
 * FFV2dTrack.hh
 *
 *  Created on: Jul 26, 2012
 *      Author: rkale
 */

#ifndef FFV2DTRACK_HH_
#define FFV2DTRACK_HH_

#include "rtp.h"

class CFFV2d;
class CRateController;

class CFFV2dTrack {
public:
    CFFV2dTrack(CFFV2d * pFFV2d, int trackIndex);
    ~CFFV2dTrack();
    void Init();
    void ProcessSRDFrame();
    void AdjustTokenMeter(int value);
    void ResetScap();
    uint64_t GetVideoFrameCount();

private:

    void WriteScap();
    bool CheckBandwidth();
    bool CheckFrameRate();

    int m_nTrackIndex;
    CFFV2d * m_pFFV2d;
    CRateController * m_pRateController;

    int m_nTokenMeter;
    uint64_t m_nUnavailable;
    uint64_t m_nScapOutFrames;
    RtpContext_t m_RTPContext;
    timestamp_t m_nScapWriteFrequency;
    timestamp_t m_nScapWriteTimestamp;
    uint64_t m_nBytesWritten;
    uint64_t m_nFramesWritten;
    uint64_t m_nLastFrameCount;
};

#endif /* FFV2DTRACK_HH_ */
