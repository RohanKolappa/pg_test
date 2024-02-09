
#include "strmsrv_compatibility.h"
//#include <math.h>
#include "perf.hh"


char * timeStampToDate(unsigned long long timeStamp, char *buf) {
#ifdef WINDOWS
    strcpy(buf, "");
#else
    time_t secs = (timeStamp/1000000);
    ctime_r(&secs, buf);
    char *cp = strrchr(buf, ' ');
    sprintf(cp, ".%03d", (int)((timeStamp/1000) % 1000));
#endif
    return buf;
}

char * bitrateToString(double bitrate) {
    static char retbuf[32];

    if (bitrate > 1000000.0 )
        snprintf(retbuf, 32, "%.3fM", bitrate/1048576.0);
    else if (bitrate > 1000.0)
        snprintf(retbuf, 32, "%.3fK", bitrate/1024.0);
    else
        snprintf(retbuf, 32, "%.0f", bitrate);
    return retbuf;
}

char * perfSummaryString(struct SPerfData *pD) {
    static char retbuf[256];

    if (pD == NULL) {
        strcpy(retbuf, "");
    }
    else {
        snprintf(retbuf, 255,
                 "r:%s e:%lu o:%lu d:%lu j:%.3lf,%.3lf,%.3lf p:%"PRIu64" t:%"PRIu64"",
                 bitrateToString(pD->m_bitrate),
                 pD->m_sequenceError,
                 pD->m_outOfOrderError,
                 pD->m_totalDropped,
                 pD->m_avgMaxJitter,
                 pD->m_runningAvgJitter,
                 pD->m_avgRangeJitter,
                 pD->m_intervalPacketCount,
                 pD->m_packetCount
                 );
    }
    return retbuf;

}

char * playTimeString(unsigned long long playTime) {
    static char retbuf[32];
    unsigned long playSec = playTime/1000000;
    int hr = playSec/3600;
    int min = (playSec%3600)/60;
    int sec = playSec%60;

    snprintf(retbuf, 31, "%02d:%02d:%02d.%03d", hr, min, sec, (int)playTime%1000);
    return retbuf;

}

char * playTimeStringDiff(unsigned long long startTime) {

    // First get time now
    unsigned long long ts = GetTimeNow();

    if (startTime == 0)
        startTime = ts;

    return playTimeString(ts - startTime);

}

unsigned long long GetTimeNow() {

    // Getet time now
    struct timeval t1;
    gettimeofday(&t1, NULL);
    unsigned long long ts =
        ((unsigned long long) t1.tv_sec) * 1000000 +
        (t1.tv_usec);
    return ts;
}

int computePerformaceStats(struct SPerfData *pD,
                           struct SDataHeader *pDataHeader,
                           int packetLength, int interval,
                           int timeout ) {


    int retval = 0;
    int init_needed = 0;

    // check inputs
    if (interval < 10)
        interval = 10;

    interval = interval * 1000; // Convert to microseconds

    // First get time now
    unsigned long long ts = GetTimeNow();

    // Initialize on first call or on timeout (5 secs)

    if ((pD->m_packetCount + pD->m_progressPacketCount) == 0)
        init_needed = 1;

    if (pDataHeader->h_oob.h_magic == PERF_MAGIC &&
        pD->m_sessionID != pDataHeader->h_oob.h_sessionid)
        init_needed = 1;

    if (pD->m_sessionID == 0 &&
        timeout > 0 && 
        (ts - pD->m_timestampNow) > (unsigned long) timeout) 
        init_needed = 1;


    if (init_needed) {
        memset(pD, 0x0, sizeof(struct SPerfData));
        pD->m_startWallClock = ts;
        pD->m_intervalTimestamp = ts;
        pD->m_timestampNow = ts;
        pD->m_startTimeStamp = pDataHeader->h_nTimestamp;
        pD->m_streamStartTimeStamp = pDataHeader->h_nTimestamp;
        pD->m_streamStopTimeStamp = pDataHeader->h_nTimestamp;

        if (pDataHeader->h_oob.h_magic != PERF_MAGIC) {
            pD->m_transit = pD->m_startWallClock -
                pDataHeader->h_nTimestamp;
            pD->m_expectedSequence = pDataHeader->h_nSequence + 1;
            pD->m_packetCount++;
            pD->m_intervalPacketCount++;
            pD->m_intervalByteCount += packetLength;
        }
        else {
            // Progress packet??
            pD->m_progressPacketCount++;
            pD->m_sessionID = pDataHeader->h_oob.h_sessionid;
            pD->m_timestampProgress = pDataHeader->h_nTimestamp;
            pD->m_progressWallClock = ts;
            
        }

        return 1;
    }

    // Progress packet??
    if (pDataHeader->h_oob.h_magic == PERF_MAGIC) {

        pD->m_timestampNow = ts;
        pD->m_progressPacketCount++;
        pD->m_timestampProgress = pDataHeader->h_nTimestamp;
        pD->m_progressWallClock = ts;
        pD->m_sessionID = pDataHeader->h_oob.h_sessionid;

        if (pDataHeader->h_oob.h_type == PERF_TYPE_START) {
            pD->m_streamStartTimeStamp = pDataHeader->h_nTimestamp;
            retval = 1;
        }
        else if (pDataHeader->h_oob.h_type == PERF_TYPE_STOP) {
            pD->m_streamStopTimeStamp = pDataHeader->h_nTimestamp;
            retval = 1;
        }

        pD->m_progressOffSet += (ts - pD->m_progressWallClock) -
            (pDataHeader->h_nTimestamp - pD->m_timestampProgress);

        if (pD->m_timestampNow >= pD->m_intervalTimestamp + interval) {
            pD->m_thisInterval = pD->m_timestampNow - pD->m_intervalTimestamp;
            while (pD->m_timestampNow >= pD->m_intervalTimestamp + interval) {
                pD->m_intervalTimestamp += interval;
            }
            retval = 1;
        }

        return retval;
    }

    pD->m_timestampNow = ts;
    pD->m_packetWallClock = ts;
    pD->m_timestampPacket = pDataHeader->h_nTimestamp;

    // Reset counts that span a interval
    if (pD->m_resetNeeded) {
        pD->m_intervalByteCount = 0;
        pD->m_maxJitter = 0;
        pD->m_totalJitter = 0;
        pD->m_totalRangeJitter = 0;
        pD->m_intervalDropped = 0;
        pD->m_intervalPacketCount = 0;
        pD->m_resetNeeded = 0;
        if (pD->m_intervalTicks % 16 == 0) // min recomputed every 16 ticks
            pD->m_minTransit = 0;
    }

    // Check sequence errors
    unsigned long long sequence = pDataHeader->h_nSequence;
    if (sequence != pD->m_expectedSequence && pD->m_expectedSequence != 0) {
        pD->m_sequenceError++;
        if (sequence > pD->m_expectedSequence) {
            pD->m_totalDropped += (sequence - pD->m_expectedSequence);
            pD->m_intervalDropped += (sequence - pD->m_expectedSequence);
        }
        else {
            pD->m_outOfOrderError++;
        }
    }
    pD->m_expectedSequence = sequence + 1;

#define ALPHA 0.05
    // Compute latency
    long long latency = pD->m_timestampNow - pDataHeader->h_nTimestamp;
    if (latency < 0)
        latency = -latency;
    if (latency > 5000000) // cap to 5 secs
        latency = 5000000;
    pD->m_latency = ALPHA * ((double) latency) + (1.0 - ALPHA) * pD->m_latency;

    // Compute interpacket jitter
    long long transit = pD->m_timestampNow - pDataHeader->h_nTimestamp;
    long long timeOffset = transit - pD->m_transit;
    pD->m_transit = transit;
    if (timeOffset < 0)
        timeOffset = -timeOffset;


    pD->m_jitter += (1./16.) * ((double)timeOffset - pD->m_jitter);
    if (pD->m_maxJitter < pD->m_jitter)
        pD->m_maxJitter = pD->m_jitter;

    pD->m_totalJitter += timeOffset;


    // Another estimate for jitter
    if (pD->m_minTransit > transit || pD->m_minTransit == 0)
        pD->m_minTransit = transit;

    timeOffset =
        transit - pD->m_minTransit;

    if (timeOffset < 0)
        timeOffset = 0;

    pD->m_totalRangeJitter += timeOffset;


    // update counts
    pD->m_packetCount++;
    pD->m_intervalPacketCount++;
    pD->m_intervalByteCount += packetLength;

    // compute things at end of the interval
    if (pD->m_timestampNow >= pD->m_intervalTimestamp + interval) {
        pD->m_thisInterval = pD->m_timestampNow - pD->m_intervalTimestamp;
        while (pD->m_timestampNow >= pD->m_intervalTimestamp + interval) {
            pD->m_intervalTimestamp += interval;
        }
        pD->m_intervalTicks++;
        // bitrate
        pD->m_bitrate = (8000000.0 * pD->m_intervalByteCount)/pD->m_thisInterval;

        // avg jitter
        pD->m_avgJitter = ((double)pD->m_totalJitter)/pD->m_intervalPacketCount;

        // Running average of avg jitter over a 'window'
        pD->m_avgJitterTotal -= pD->m_avgJitterArr[pD->m_avgJitterIdx];
        pD->m_avgJitterArr[pD->m_avgJitterIdx++] = pD->m_avgJitter;
        pD->m_avgJitterIdx = pD->m_avgJitterIdx % PERFAVGWIN;
        pD->m_avgJitterTotal += pD->m_avgJitter;
        pD->m_runningAvgJitter = pD->m_avgJitterTotal / (double) PERFAVGWIN;


        // Running average of max jitter over a 'window'
        pD->m_avgMaxJitterTotal -= pD->m_avgMaxJitterArr[pD->m_avgMaxJitterIdx];
        pD->m_avgMaxJitterArr[pD->m_avgMaxJitterIdx++] = pD->m_maxJitter;
        pD->m_avgMaxJitterIdx = pD->m_avgMaxJitterIdx % PERFAVGWIN;
        pD->m_avgMaxJitterTotal += pD->m_maxJitter;
        pD->m_avgMaxJitter = pD->m_avgMaxJitterTotal / (double) PERFAVGWIN;


        // avg of Range Jitter

        pD->m_rangeJitter = ((double)pD->m_totalRangeJitter)/pD->m_intervalPacketCount;
        pD->m_avgRangeJitter += (1./16.) * (pD->m_rangeJitter - pD->m_avgRangeJitter);


        // yet another estimate for jitter -- packet rate jitter
        pD->m_packetsPerSecond = (1000000.0*pD->m_packetCount)/
            (pD->m_timestampNow - pD->m_startWallClock);
        long packetDiff = (long)((1000.0/interval)*pD->m_intervalPacketCount -
                                 pD->m_packetsPerSecond);

        if (packetDiff < 0)
            packetDiff = -packetDiff;
        pD->m_totalPacketDiff += packetDiff;
        pD->m_packetRateJitter = (pD->m_totalPacketDiff*100.0)/
            (pD->m_intervalTicks * pD->m_packetsPerSecond);

        pD->m_resetNeeded = 1;
        retval = 1;
    }

    return retval;
}
