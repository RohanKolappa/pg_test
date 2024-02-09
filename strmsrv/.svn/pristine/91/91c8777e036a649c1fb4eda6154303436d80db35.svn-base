#ifndef __PERF_H_
#define __PERF_H_

#define PERFAVGWIN   32

#define PERF_MAGIC                      0xABDCCDBA
#define PERF_TYPE_PROGRESS              0x01
#define PERF_TYPE_STOP                  0x02
#define PERF_TYPE_START                 0x03

struct SOutOfBandHeader
{
    unsigned long h_magic;
    unsigned short h_sessionid;
    unsigned short h_type;
};

struct SDataHeader
{
    unsigned long long h_nTimestamp;
    union {
        unsigned long long h_nSequence;
        struct SOutOfBandHeader h_oob;
    };
    
};

struct SPerfData {
    unsigned long long m_packetCount;
    unsigned long long m_progressPacketCount;
    unsigned long long m_timestampProgress;
    unsigned long m_sessionID;
    unsigned long m_intervalByteCount;
    unsigned long long m_startWallClock;
    unsigned long long m_startTimeStamp;
    unsigned long long m_streamStartTimeStamp;
    unsigned long long m_streamStopTimeStamp;
    unsigned long long m_timestampNow;
    unsigned long long m_packetWallClock;
    unsigned long long m_progressWallClock;
    long long m_progressOffSet;
    unsigned long long m_timestampPacket;
    unsigned long long m_intervalTimestamp;
    unsigned long long m_intervalPacketCount;
    unsigned long long m_expectedSequence;
    unsigned long m_totalPacketDiff;
    int m_resetNeeded;
    long m_thisInterval;
    long m_intervalTicks;
    long long m_transit;
    long long m_minTransit;
    unsigned long m_sequenceError;
    unsigned long m_outOfOrderError;
    unsigned long m_totalDropped;
    unsigned long m_intervalDropped;
    double m_jitter;
    double m_maxJitter;
    double m_avgMaxJitter;
    double m_avgJitter;
    double m_avgJitterTotal;
    double m_avgJitterArr[PERFAVGWIN];
    int m_avgJitterIdx;
    double m_runningAvgJitter;
    double m_avgMaxJitterTotal;
    double m_avgMaxJitterArr[PERFAVGWIN];
    int m_avgMaxJitterIdx;
    double m_totalJitter;
    double m_totalRangeJitter;
    double m_rangeJitter;
    double m_avgRangeJitter;
    double m_bitrate;
    double m_packetsPerSecond;
    double m_packetRateJitter;
    double m_latency;
};

extern char * bitrateToString(double bitrate);

extern char * perfSummaryString(struct SPerfData *pD);

extern char * playTimeString(unsigned long long playTime);

extern char * playTimeStringDiff(unsigned long long startTime);

extern int computePerformaceStats(struct SPerfData *pPerfData,
                            struct SDataHeader *pDataHeader,
                            int packetLength, int interval, int timeout);

extern char * timeStampToDate(unsigned long long timeStamp, char *buf);

extern unsigned long long GetTimeNow();

#endif

