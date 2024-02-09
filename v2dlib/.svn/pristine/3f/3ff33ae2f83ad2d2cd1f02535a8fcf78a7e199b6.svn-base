/*
 * TimestampTracker.hh
 *
 *  Created on: Oct 1, 2013
 *      Author: rkale
 */

#ifndef TIMESTAMPTRACKER_HH_
#define TIMESTAMPTRACKER_HH_

#define SW_SIZE  300

class TimestampTracker {
public:
    TimestampTracker()
    {
        m_rtpTimeStamp = 0;
        m_prevRtpTimeStamp = 0;
        m_WallClock = 0;
        m_scaledWallClock = 0;
        m_prevWallClock = 0;
        m_samples = 0;
        m_swIndex = 0;
        m_CurrentTimeOffset = 0;
        m_MinTimeOffset = 0xFFFFFFFF;
        m_AvgMinTimeOffset = 0;
        m_TotalOffset = 0;
        m_MovingAvgTimeOffset = 0;

        m_WCTicksPerSecond = 1000000;
        m_rtpTicksPerSecond = 90000;
        m_scalingFactor = ((double)m_rtpTicksPerSecond)/m_WCTicksPerSecond;
    };

    ~TimestampTracker() {};

    void SetTimestamp(uint32_t timestamp, uint64_t wallclock)
    {
        m_prevWallClock = m_WallClock;
        m_prevRtpTimeStamp = m_rtpTimeStamp;
        m_WallClock = wallclock;
        m_rtpTimeStamp = timestamp;
        m_samples++;

        ComputeTimeScale();
        ComputeTimeOffset();
    }

    void ComputeTimeScale()
    {
        m_scaledWallClock = ((uint64_t) ((double)(m_WallClock * m_scalingFactor))) & 0xFFFFFFFF;
    }

    double ScaleToMilliSeconds(uint32_t value) {
        return ((double) value * m_WCTicksPerSecond)/(m_rtpTicksPerSecond * 1000.0);
    }

    void ComputeTimeOffset()
    {
        m_CurrentTimeOffset = m_scaledWallClock - m_rtpTimeStamp;

        // Track sliding window minimum and average for the time offset
        if (m_samples <= SW_SIZE) { // Window not yet full
            if (m_MinTimeOffset > m_CurrentTimeOffset)
                m_MinTimeOffset = m_CurrentTimeOffset;
            m_TotalOffset += m_CurrentTimeOffset;
            m_MovingAvgTimeOffset = (uint32_t)(((double)m_TotalOffset)/m_samples);
        }
        else {
            if (m_slidingWindow[m_swIndex] == m_MinTimeOffset) {
                // Need to scan for new minimum
                m_MinTimeOffset = 0xFFFFFFFF;
                for (int i = 0; i < SW_SIZE; i++) {
                    if (i == m_swIndex)
                        continue;
                    if (m_MinTimeOffset > m_slidingWindow[i])
                        m_MinTimeOffset = m_slidingWindow[i];
                }
            }
            else if (m_MinTimeOffset > m_CurrentTimeOffset) {
                m_MinTimeOffset = m_CurrentTimeOffset;
            }

            m_TotalOffset -= m_slidingWindow[m_swIndex];
            m_TotalOffset += m_CurrentTimeOffset;
            m_MovingAvgTimeOffset = (uint32_t)(((double)m_TotalOffset)/SW_SIZE);
        }

        m_slidingWindow[m_swIndex] = m_CurrentTimeOffset;
        m_swIndex++;
        m_swIndex = m_swIndex % SW_SIZE;

        double alpha = 0.01;
        if (m_samples <= 2)
            m_AvgMinTimeOffset = m_MinTimeOffset;
        else
            m_AvgMinTimeOffset = (double)alpha * m_MinTimeOffset +
                (double)(1.0 - alpha) * m_AvgMinTimeOffset;

    }

    void PrintStatus()
    {
//        if (m_samples <= 1)
//            return;

#if 1
        printf("WC = %llu, TC = %u (%u %u)\n",
                m_WallClock, m_rtpTimeStamp,
                (int) (m_WallClock -  m_prevWallClock),
                (int) (m_rtpTimeStamp - m_prevRtpTimeStamp));
#endif

#if 1
        printf("TO: %u AvgMin: %u MovAvg: %u (%.3f)\n",
                m_CurrentTimeOffset,
                (uint32_t) m_AvgMinTimeOffset,
                m_MovingAvgTimeOffset, ScaleToMilliSeconds(m_MovingAvgTimeOffset));
#endif
    }

private:
    uint32_t m_rtpTimeStamp;
    uint32_t m_prevRtpTimeStamp;
    uint64_t m_WallClock;
    uint64_t m_prevWallClock;
    uint32_t m_scaledWallClock;
    uint32_t m_CurrentTimeOffset;
    uint32_t m_MinTimeOffset;
    double m_AvgMinTimeOffset;
    uint64_t m_samples;
    uint32_t m_slidingWindow[SW_SIZE];
    uint64_t m_TotalOffset;
    uint32_t m_MovingAvgTimeOffset;
    int m_swIndex;
    int m_WCTicksPerSecond;
    int m_rtpTicksPerSecond;
    double m_scalingFactor;
};


class RandomBufferizer {

public:
    RandomBufferizer() {

    }

    ~RandomBufferizer() {

    }

    void PutBuffer(unsigned char *data, int len) {
        if (len > 2048)
            return;
        BufferItem item;
        item.len = len;
        memcpy(item.data, data, len);
        m_bufferList.push_back(item);
    }

    int GetBuffer(unsigned char * data) {
        if (m_bufferList.size() <= 100)
            return 0;
        BufferItem item = m_bufferList.front();
        int len = item.len;
        memcpy(data, item.data, len);
        m_bufferList.pop_front();
        return len;
    }

private:
    struct BufferItem {
        int len;
        unsigned char data[2048];
    };
    std::list<BufferItem> m_bufferList;
};

#endif /* TIMESTAMPTRACKER_HH_ */
