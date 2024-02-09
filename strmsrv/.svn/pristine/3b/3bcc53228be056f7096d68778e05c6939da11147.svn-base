#ifndef __HH_SHAPER_H__
#define __HH_SHAPER_H__

#include "common.hh"
#include "timekeeper.hh"
#include "command.hh"

class CShaper
{
public:
    CShaper(unsigned long nMaxbps,
            unsigned long nMinBurst,
            unsigned long nMaxBurst,
            unsigned long nStep,
            unsigned long nQueueThreshold);

    CShaper(unsigned long nMaxbps,
            double nBurstRatio,
            unsigned long nBurstDuration);

    ~CShaper();
    bool CheckTokens(unsigned long size,
                     unsigned long qlen);
    bool CheckTokens(unsigned long size);
    void RevertTokens(unsigned long size);
    void AllowTokens(unsigned long size);
    void ControlTokens(unsigned long size);
    void SetBandwidth(unsigned long nMaxbps);
    void SetBandwidth(unsigned long nMaxbps,
                      double nBurstRatio,
                      unsigned long nBurstDuration);
    void UpdateCustomStatus(CCommand *a_pCmd);
    void DumpProfile();

private:
    double                      m_nBytesPerTick;
    double                      m_nPeakBytesPerTick;
    unsigned long               m_nTokenBucket;
    unsigned long               m_nTokenBucketAdjust;
    unsigned long               m_nPeakTokenBucket;
    unsigned long               m_nPeakTokenBucketAdjust;
    unsigned long               m_nBackoffCount;
    unsigned long               m_nPeakBackoffCount;
    unsigned long               m_nBurstSize;
    unsigned long               m_nPeakBurstSize;
    timestamp_t                 m_tLastTimeStamp;
    unsigned long               m_nMaxbps;
    unsigned long               m_nMinBurst;
    unsigned long               m_nMaxBurst;
    unsigned long               m_nStep;
    unsigned long               m_nQueueThreshold;
    unsigned long               m_nMinBurstSize;
    unsigned long               m_nMaxBurstSize;
    unsigned long               m_nBurstDuration;
    unsigned long               m_nControlTokenBucket;
    unsigned long               m_nControlBackoffCount;
    unsigned long               m_nUnthrottleCount;
    unsigned long               m_nSafetyValveCount;
    double                      m_dConfiguredBurstRatio;
    double                      m_dBurstRatio;
    unsigned long               m_nLinkSpeed;
    static unsigned long        m_nInterfaceLinkSpeed;
    static timestamp_t          m_nLinkSpeedTimestamp;

    void InitVariables();
    void ComputeTokenBucketParameters();
    void DetermineLinkSpeed();
    unsigned long RetrieveLinkSpeed(int maxInterfaces);

//#define PROFILE_BW
#define HISTOGRAM_SIZE          1000
#define PROFILE_RESOLUTION      10 // in milliseconds
#ifdef PROFILE_BW
    unsigned long               m_nBurstCount;
    unsigned long               m_nPeakBurstCount;
    int                         m_nIndexUpdates;
    int                         m_nLastIndex;
    int                         m_nCurrentIndex;
    unsigned long               m_nBwProfileBytes[HISTOGRAM_SIZE];
#endif
};

#endif /*__HH_SHAPER_H__*/
