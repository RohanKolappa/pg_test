#include "strmsrv_compatibility.h"
#include <sys/stat.h>
#include <math.h>
#include "common.hh"
#include "shaper.hh"

#define MAXCONTROLBUCKET (64 * 1024)
#define CONTROLBYTESPERTICK (16)


unsigned long CShaper::m_nInterfaceLinkSpeed = 0;
timestamp_t   CShaper::m_nLinkSpeedTimestamp = 0;

// Shaper configuration takes all bandwidth in bits per second

CShaper::CShaper (unsigned long nMaxbps,
                  unsigned long nMinBurst,
                  unsigned long nMaxBurst,
                  unsigned long nStep,
                  unsigned long nQueueThreshold)
{
    /* Set the token bucket parameters */
    ASSERT(nMaxbps > 0);
    ASSERT(nMaxBurst >= nMinBurst);

    InitVariables();

    m_nMaxbps = nMaxbps;
    m_nMinBurst = nMinBurst;
    m_nMaxBurst = nMaxBurst;
    m_nBackoffCount = 0;
    m_nBytesPerTick = (double)m_nMaxbps/(8.0 * HIGHRES_TICKS_PER_SECOND);
    if (m_nBytesPerTick < 1.0)
        m_nBytesPerTick = 1.0;
    m_nMinBurstSize = (unsigned long)(round(m_nBytesPerTick * m_nMinBurst *
            (HIGHRES_TICKS_PER_SECOND/1000)));
    m_nMaxBurstSize = (unsigned long)(round(m_nBytesPerTick * m_nMaxBurst *
            (HIGHRES_TICKS_PER_SECOND/1000)));

    m_nLinkSpeed = 100 * 1024 * 1024;

    // Set burst sizes to be at least the MTU
    if (m_nMinBurstSize < 1500)
        m_nMinBurstSize = 1500;
    if (m_nMaxBurstSize < 1500)
        m_nMaxBurstSize = 1500;

    m_nQueueThreshold = nQueueThreshold;

    m_nTokenBucket = m_nMinBurstSize;
    m_nBurstSize = m_nMinBurstSize;
    m_tLastTimeStamp = CTimeKeeper::GetHighResTime();
    m_nStep = nStep;


#ifdef PROFILE_BW
    m_nPeakBurstCount = 0;
    m_nBurstCount = 0;
    m_nIndexUpdates = 0;
    m_nLastIndex = 0;
    m_nCurrentIndex = 0;
    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        m_nBwProfileBytes[i] = 0;
    }
#endif

}

CShaper::CShaper (unsigned long nMaxbps,
                  double dBurstRatio,
                  unsigned long nBurstDuration)
{
    ASSERT(nMaxbps > 0);
    ASSERT(dBurstRatio >= 1.0);

    InitVariables();

    m_nMaxbps = nMaxbps;
    m_nBurstDuration = nBurstDuration;
    m_dBurstRatio = dBurstRatio;
    m_dConfiguredBurstRatio = dBurstRatio;
    if (m_dBurstRatio < 1.0)
        m_dBurstRatio = 1.0;

    m_nLinkSpeed = 100 * 1024 * 1024;

    // limit burst duration to less than 100 ms
    if (m_nBurstDuration < 5)
        m_nBurstDuration = 5;

    ComputeTokenBucketParameters();

    m_tLastTimeStamp = CTimeKeeper::GetHighResTime();

#ifdef PROFILE_BW
    m_nBurstCount = 0;
    m_nPeakBurstCount = 0;
#endif
}

CShaper::~CShaper()
{
    return;
}

void CShaper::InitVariables()
{
    m_nBackoffCount = 0;
    m_nPeakBackoffCount = 0;
    m_nPeakTokenBucket = 0;
    m_nTokenBucketAdjust = 0;
    m_nPeakTokenBucketAdjust = 0;
    m_nControlTokenBucket = 0;
    m_nControlBackoffCount = 0;
    m_nPeakBurstSize = 0;
    m_nUnthrottleCount = 0;
    m_nSafetyValveCount = 0;
    m_nMinBurstSize = 0;
    m_nMaxBurstSize = 0;
    m_nBurstDuration = 0;
    m_nPeakBytesPerTick = 0;
    m_nMinBurst = 0;
    m_nMaxBurst = 0;
    m_dConfiguredBurstRatio = 0.0;
    m_dBurstRatio = 0.0;
    m_nStep = 0;
    m_nQueueThreshold = 0;
}

/*

Shaper controls bandwidth using two token buckets. First bucket
controls the primary configured bandwidth. The second bucket controls
the peak bandwidth according to Burst Ratio setting.

The content of the buckets (tokens) are in units of bytes. Every time
a packet needs to be let out, CheckTokens() checks if the packet needs
to be allowed through. Bytes are added to the bucket depending on the
time elapsed since the last time a packet was let through. If the
bucket now has more bytes than the size of the requested packet, it is
allowed to pass. The size of the bucket is then reduced by the size of
the packet.

Time is computed using clock resolution of "Ticks". Currently we use
10000 ticks per second.  BytesPerTick represents the rate at which
bytes need to be added to the bucket per clock Tick.

The BurstSize of each bucket is the upper limit to the number of
tokens (bytes) the bucket can contain. If incrementing the bucket
size (depending on elapsed time) results in a value higher than
the BurstSize, it is capped to the BurstSize.

The BurstSize of the first bucket is computed using the configured
BurstRatio and BurstDuration.

*/

#define PEAK_BURST_DURATION   3

void
CShaper::ComputeTokenBucketParameters()
{


    m_nBytesPerTick = (double)m_nMaxbps/(8.0 * HIGHRES_TICKS_PER_SECOND);
    if (m_nBytesPerTick < 1.0)
        m_nBytesPerTick = 1.0;

    // Recompute link speed and burst ratio if peak goes above link speed
    DetermineLinkSpeed();
    m_dBurstRatio = m_dConfiguredBurstRatio;
    if (m_nMaxbps * m_dBurstRatio > m_nLinkSpeed) {
        m_dBurstRatio = (m_nLinkSpeed * 1.0)/m_nMaxbps;
    }
    if (m_dBurstRatio < 1.0)
        m_dBurstRatio = 1.0;

    // Primary bucket's max size should has enough tokens to allow the
    // peak rake to run through for the configured duration before the
    // bucket gets drained
    m_nBurstSize = (unsigned long)(round(m_nBytesPerTick *
                                         (m_dBurstRatio - 1.0) *
                                         m_nBurstDuration *
                                         (HIGHRES_TICKS_PER_SECOND/1000)));

    // Compute Peak burst size
    double maxBytesPerTick = (double)(m_nLinkSpeed)/(8.0 * HIGHRES_TICKS_PER_SECOND);

    m_nPeakBurstSize =
        (unsigned long)(round(1.0 * maxBytesPerTick *
                              PEAK_BURST_DURATION *
                              (HIGHRES_TICKS_PER_SECOND/1000)));

    if (m_nBurstSize < m_nPeakBurstSize)
        m_nBurstSize = m_nPeakBurstSize;

    // Set burst size to be at least the MTU
    if (m_nBurstSize < 1500)
        m_nBurstSize = 1500;

    // Prefill the primary bucket
    m_nTokenBucket = m_nBurstSize;

    // Compute parameters for peak rate
    unsigned long peakRate = (unsigned long) (m_nMaxbps * m_dBurstRatio);
    if (peakRate > m_nLinkSpeed)
        peakRate = m_nLinkSpeed;

    m_nPeakBytesPerTick = (double)peakRate/(8.0 * HIGHRES_TICKS_PER_SECOND);

    if (m_nPeakBytesPerTick > maxBytesPerTick)
        m_nPeakBytesPerTick = maxBytesPerTick;


#if 0
    m_nPeakBurstSize =
        (unsigned long)(round(m_nPeakBytesPerTick +
                              (maxBytesPerTick - m_nPeakBytesPerTick) * 0.5) *
                              PEAK_BURST_DURATION *
                              (HIGHRES_TICKS_PER_SECOND/1000));
#endif


    // Prefill the secondary bucket
    m_nPeakTokenBucket = m_nPeakBurstSize;

    // Fill control bucket
    m_nControlTokenBucket = MAXCONTROLBUCKET;


}

// Set the current bandwidth limit
void
CShaper::SetBandwidth(unsigned long nMaxbps)
{

    ASSERT(nMaxbps > 0);

    m_nMaxbps = nMaxbps;

    ComputeTokenBucketParameters();
}

void
CShaper::SetBandwidth(unsigned long nMaxbps,
                  double dBurstRatio,
                  unsigned long nBurstDuration)
{

    ASSERT(nMaxbps > 0);

    m_nMaxbps = nMaxbps;
    m_nBurstDuration = nBurstDuration;
    m_dBurstRatio = dBurstRatio;
    m_dConfiguredBurstRatio = dBurstRatio;
    if (m_dBurstRatio < 1.0)
        m_dBurstRatio = 1.0;

    ComputeTokenBucketParameters();
}

// Token bucket algorithm to control flow rate using a backoff policy
// based on queue size
bool
CShaper::CheckTokens(unsigned long a_size, unsigned long q_size) {

    timestamp_t tNow = CTimeKeeper::GetHighResTime();
    unsigned long tDiff = (tNow - m_tLastTimeStamp);
    m_tLastTimeStamp = tNow;

    // fill up bucket with tokens
    m_nTokenBucket += (unsigned long)((double)tDiff * m_nBytesPerTick);
    if (m_nTokenBucket > m_nBurstSize)
        m_nTokenBucket = m_nBurstSize;

    // Can we transfer the next packet ??
    if (m_nTokenBucket <= a_size) {
        // Safety valve: Unthrottle the burst size if we start piling
        // up the queue. If queue size too long, let the packet through anyway
        if (q_size > m_nQueueThreshold) {
            m_nSafetyValveCount++;
            goto ALLOW_TOKEN;
        }
        else if (q_size > (m_nQueueThreshold / 2)) {
            if (m_nBurstSize < m_nMaxBurstSize) {
                m_nUnthrottleCount++;
                m_nBurstSize += m_nStep;
            }
        }
        else {
            m_nBurstSize = m_nMinBurstSize;
        }
        m_nBackoffCount++;
        return false;
    }
    else {
        m_nBurstSize = m_nMinBurstSize;
    }

  ALLOW_TOKEN:
    // Take away 'a_size' bytes from the buckets
    if (m_nTokenBucket > a_size)
        m_nTokenBucket -= a_size;
    else
        m_nTokenBucket = 0;

    return true;
}

// Token bucket algorithm to control flow rate with no backoff policy
bool
CShaper::CheckTokens(unsigned long a_size) {

    timestamp_t tNow = CTimeKeeper::GetHighResTime();
    unsigned long long tDiff = (tNow - m_tLastTimeStamp);

    // Check against primary token bucket

    // Fill up bucket with tokens
    unsigned long nTokenBucket = m_nTokenBucket +
        (unsigned long)((double)tDiff * m_nBytesPerTick);

    if (m_nTokenBucketAdjust > 0) {
        if (nTokenBucket >= m_nTokenBucketAdjust) {
            nTokenBucket -= m_nTokenBucketAdjust;
        }
        else {
            nTokenBucket = 0;
        }
    }

    if (nTokenBucket > m_nBurstSize) {
#ifdef PROFILE_BW
        m_nBurstCount++;
#endif
        nTokenBucket = m_nBurstSize;
    }


    // Can we transfer the next packet ??
    if (nTokenBucket < a_size) {
        m_nBackoffCount++;
        return false;
    }

    // Check against peak token bucket

    // Fill up peak token bucket with tokens
    unsigned long nPeakTokenBucket = m_nPeakTokenBucket +
        (unsigned long)((double)tDiff * m_nPeakBytesPerTick);

    if (m_nPeakTokenBucketAdjust > 0) {
        if (nPeakTokenBucket >= m_nPeakTokenBucketAdjust) {
            nPeakTokenBucket -= m_nPeakTokenBucketAdjust;
        }
        else {
            nPeakTokenBucket = 0;
        }
    }

    if (nPeakTokenBucket > m_nPeakBurstSize) {
#ifdef PROFILE_BW
        m_nPeakBurstCount++;
#endif
        nPeakTokenBucket = m_nPeakBurstSize;
    }

    // Can we still transfer the next packet ??
    if (nPeakTokenBucket < a_size) {
        m_nPeakBackoffCount++;
        return false;
    }

#if 0
    // Fill up control token bucket with tokens
    unsigned nControlTokenBucket = m_nControlTokenBucket +
        (unsigned long)((double)tDiff * CONTROLBYTESPERTICK);
    if (nControlTokenBucket > MAXCONTROLBUCKET)
        nControlTokenBucket = MAXCONTROLBUCKET;

    // Check against control token bucket
    if (nControlTokenBucket < a_size) {
        m_nControlBackoffCount++;
        return false;
    }
#endif

    // Take away 'a_size' bytes from both the buckets
    // a_size will always be less than the TokenBucket size else
    // we would had returned false above
    m_nTokenBucket = nTokenBucket - a_size;
    m_nPeakTokenBucket = nPeakTokenBucket - a_size;

    // Reset the adjust token buckets
    m_nTokenBucketAdjust = 0;
    m_nPeakTokenBucketAdjust = 0;

#if 0
    // Also take away 'a_size' bytes from control bucket
    m_nControlTokenBucket = nControlTokenBucket - a_size;
#endif

#ifdef PROFILE_BW
    m_nCurrentIndex = (tNow/ ((HIGHRES_TICKS_PER_SECOND * PROFILE_RESOLUTION)/ 1000)) % HISTOGRAM_SIZE;
    if (m_nCurrentIndex != m_nLastIndex) {
        int idx = (m_nLastIndex + 1) % HISTOGRAM_SIZE;
        while(1) {
            m_nBwProfileBytes[idx] = 0;
            if (idx == m_nCurrentIndex)
                break;
            idx = (idx+1) % HISTOGRAM_SIZE;
        }
        m_nLastIndex = m_nCurrentIndex;
    }
    m_nBwProfileBytes[m_nCurrentIndex] += a_size;
    m_nIndexUpdates++;
#endif

    // Set the timestamp for next call
    m_tLastTimeStamp = tNow;

    return true;
}

// Put back tokens to the control bucket, that CheckTokens may have taken away
// Client should only call after a successful CheckTokens(). This is useful if
// client decides to not allow bandwidth to be used for some reason
// after a successful CheckToken()
void
CShaper::RevertTokens(unsigned long a_size) {

    // Put back 'a_size' bytes from both the buckets
    m_nTokenBucket += a_size;
    if (m_nTokenBucket > m_nBurstSize) {
        m_nTokenBucket = m_nBurstSize;
    }

    m_nPeakTokenBucket += a_size;
}

void
CShaper::DumpProfile() {
#ifdef PROFILE_BW
    int idx;
    for (int i = 0; i < HISTOGRAM_SIZE - 1; i++) {
        idx = (m_nCurrentIndex + 1 + i) % HISTOGRAM_SIZE ;
        SYSLOG_INFO("Profile %.3f %f",
                    (i * PROFILE_RESOLUTION * 1.0)/1000.0,
                    (m_nBwProfileBytes[idx] * 8000.0)/(PROFILE_RESOLUTION * 1024.0 * 1024.0) );
    }
#endif
}

// Add tokens to the control bucket
void
CShaper::ControlTokens(unsigned long a_size) {

    m_nControlTokenBucket += a_size;
    if (m_nControlTokenBucket > MAXCONTROLBUCKET)
        m_nControlTokenBucket = MAXCONTROLBUCKET;

}

// Allow (force) a_size tokens to pass through the shaper
void
CShaper::AllowTokens(unsigned long a_size) {


    unsigned long nTokenBucket;
    unsigned long nPeakTokenBucket;

    timestamp_t tNow = CTimeKeeper::GetHighResTime();
    unsigned long long tDiff = (tNow - m_tLastTimeStamp);

    // Fill up primary bucket with tokens
    nTokenBucket = m_nTokenBucket +
        (unsigned long)((double)tDiff * m_nBytesPerTick);
    if (nTokenBucket > m_nBurstSize)
        nTokenBucket = m_nBurstSize;


    // Fill up peak token bucket with tokens
    nPeakTokenBucket = m_nPeakTokenBucket +
        (unsigned long)((double)tDiff * m_nPeakBytesPerTick);
    if (nPeakTokenBucket > m_nPeakBurstSize)
        nPeakTokenBucket = m_nPeakBurstSize;


    // Take away 'a_size' bytes from both the buckets
    if (nTokenBucket > a_size) {
        m_nTokenBucket = nTokenBucket - a_size;
    }
    else {
        m_nTokenBucket = 0;
        m_nTokenBucketAdjust += a_size - nTokenBucket;
        if (m_nTokenBucketAdjust > m_nBurstSize)
            m_nTokenBucketAdjust = m_nBurstSize;
    }

    if (nPeakTokenBucket > a_size) {
        m_nPeakTokenBucket = nPeakTokenBucket - a_size;
    }
    else {
        m_nPeakTokenBucket = 0;
        m_nPeakTokenBucketAdjust += a_size - nPeakTokenBucket;
        if (m_nPeakTokenBucketAdjust > m_nPeakBurstSize)
            m_nPeakTokenBucketAdjust = m_nPeakBurstSize;
    }

    // Set the timestamp for next call
    m_tLastTimeStamp = tNow;

}

void
CShaper::DetermineLinkSpeed()
{
#ifndef WINDOWS
    int nComputedLinkSpeed = 0;
    int max_interfaces = 3;
#endif

    m_nLinkSpeed = 100 * 1024 * 1024;

#ifndef WINDOWS

    // ethtool is slow for probing interfaces that don't exist
    // On I50s assume link speed is 100 mbps
    // On XPi shaper may be created but not used.
    struct stat statbuf;
    if (stat("/etc/config/hw_profile_xpi.txt", &statbuf) != -1) {
        SYSLOG_DEBUG("Detected XPI device");
        return;
    }
    if (stat("/etc/config/hw_profile_v2dtx.txt", &statbuf) != -1) {
        SYSLOG_DEBUG("Detected I50 Tx");
        return;
    }
    if (stat("/etc/config/hw_profile_v2drx.txt", &statbuf) != -1) {
        SYSLOG_DEBUG("Detected I50 Tx");
        return;
    }
    if (stat("/etc/config/hw_profile_v2dxp200.txt", &statbuf) != -1) {
        SYSLOG_DEBUG("Detected XP device");
        // On XP200 assume only one interface
        max_interfaces = 1;
    }


    // Retrieving link speed is expensive so do it no more than once a second
    if (CTimeKeeper::Now() > m_nLinkSpeedTimestamp + 1000 || m_nInterfaceLinkSpeed == 0) {
    	nComputedLinkSpeed = RetrieveLinkSpeed(max_interfaces);
        m_nInterfaceLinkSpeed = nComputedLinkSpeed;
    	m_nLinkSpeedTimestamp = CTimeKeeper::Now();
    }
    else {
    	nComputedLinkSpeed = m_nInterfaceLinkSpeed;
    }

    m_nLinkSpeed = nComputedLinkSpeed;

    if (m_nLinkSpeed > 100 * 1024 * 1024) {
    	// Gigabit speeds scaled down to no more than
    	// twice the configured bandwidth
    	m_nLinkSpeed = m_nMaxbps * 2;
    	if (m_nLinkSpeed < 100 * 1024 * 1024)
    		m_nLinkSpeed = 100 * 1024 * 1024;
    }

    SYSLOG_DEBUG("Link Speed : %lu", m_nLinkSpeed);
#endif
}

unsigned long CShaper::RetrieveLinkSpeed(int maxInterfaces) {
#ifndef WINDOWS
    // Use ethtool to determine speed of all active interfaces
    // and use the max link speed found

	// This is less than ideal. We should really know the exact interface the
	// the traffic is going out on and use that instead

    int nMaxLinkSpeed = 0;
    int nLinkSpeed;
    bool bLinkDetected;
    FILE *inpipe;
    char inbuf[256];
    char command[64];

    for (int i = 0; i < maxInterfaces; i++) {
        sprintf(command, "/sbin/ethtool eth%d", i);
        SYSLOG_DEBUG("Checking eth %d Link Speed ", i);
        inpipe = popen(command, "r");
        nLinkSpeed = 0;
        bLinkDetected = false;
        if (inpipe != NULL) {
            while (fgets(inbuf, sizeof(inbuf), inpipe)) {
                if (strstr(inbuf, "Speed: 100Mb/s")) {
                    nLinkSpeed = 100 * 1024 * 1024;
                }
                else if (strstr(inbuf, "Speed: 1000Mb/s")) {
                    nLinkSpeed = 1000 * 1024 * 1024;
                }
                else if (strstr(inbuf, "Speed: 10Mb/s")) {
                    nLinkSpeed = 10 * 1024 * 1024;
                }
                else if (strstr(inbuf, "Link detected: yes")) {
                    bLinkDetected = true;
                }
            }
            pclose(inpipe);
        }

        if (bLinkDetected && nLinkSpeed > 0) {
            SYSLOG_DEBUG("eth %d Link Speed : %d ", i, nLinkSpeed);
            if (nMaxLinkSpeed < nLinkSpeed)
                nMaxLinkSpeed = nLinkSpeed;
        }
    }
    if (nMaxLinkSpeed == 0) {
    	nMaxLinkSpeed = 100 * 1024 * 1024;
    }

	return nMaxLinkSpeed;
#else
	return 0;
#endif
}

void
CShaper::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectBitrateGuageParam("ShaperMaxBitrate",
                                       m_nMaxbps);
    a_pCmd->SetObjectParam("ShaperBurstRatio",
                                       m_dBurstRatio);
    a_pCmd->SetObjectParam("ShaperBurstDuration",
                                       m_nBurstDuration);
    a_pCmd->SetObjectByterateGuageParam("ShaperByteBucket",
                                        m_nTokenBucket);
    a_pCmd->SetObjectByterateGuageParam("ShaperBurstSize",
                                        m_nBurstSize);
    if (m_nMinBurstSize > 0) {
        a_pCmd->SetObjectByterateGuageParam("ShaperMinBurstSize",
                                            m_nMinBurstSize);
        a_pCmd->SetObjectByterateGuageParam("ShaperMaxBurstSize",
                                            m_nMaxBurstSize);
        a_pCmd->SetObjectCounterParam("ShaperUnthrottle",
                                      m_nUnthrottleCount);
        a_pCmd->SetObjectCounterParam("ShaperSafetyValve",
                                      m_nSafetyValveCount);
    }
    a_pCmd->SetObjectCounterParam("ShaperBackOff",
                                  m_nBackoffCount);
#ifdef PROFILE_BW
    a_pCmd->SetObjectCounterParam("ShaperBurstReached",
                                  m_nBurstCount);
#endif
    a_pCmd->SetObjectParam("ShaperBytesPerTick",
                                  m_nBytesPerTick);

    a_pCmd->SetObjectByterateGuageParam("ShaperPeakBucket",
                                        m_nPeakTokenBucket);
    a_pCmd->SetObjectByterateGuageParam("ShaperPeakBurstSize",
                                        m_nPeakBurstSize);
    a_pCmd->SetObjectCounterParam("ShaperPeakBackOff",
                                  m_nPeakBackoffCount);
#ifdef PROFILE_BW
    a_pCmd->SetObjectCounterParam("ShaperPeakBurstReached",
                                  m_nPeakBurstCount);
#endif
    a_pCmd->SetObjectParam("ShaperPeakBytesPerTick",
                                  m_nPeakBytesPerTick);
    a_pCmd->SetObjectByterateGuageParam("ShaperByteAdjust",
                                        m_nTokenBucketAdjust);
    a_pCmd->SetObjectByterateGuageParam("ShaperPeakAdjust",
                                        m_nPeakTokenBucketAdjust);

    if (a_pCmd->StatusLevel(3))
        a_pCmd->SetObjectBitrateGuageParam("ShaperLinkSpeed",
                                           m_nLinkSpeed);

#if 0
    a_pCmd->SetObjectByterateGuageParam("ShaperControlBucket",
                                        m_nControlTokenBucket);
    a_pCmd->SetObjectCounterParam("ShaperControlBackOff",
                                  m_nControlBackoffCount);
#endif
}
