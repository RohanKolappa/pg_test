#ifndef INDEXDATA_HH
#define INDEXDATA_HH

#include <list>

using namespace std;

namespace mediaindex {

class Interval {
public:
    Interval(uint64_t a_nBegin, uint64_t a_nEnd) {
        m_nBegin = a_nBegin;
        m_nEnd = a_nEnd;
    }
    uint64_t GetBeginTime() {
        return m_nBegin;
    }
    uint64_t GetEndTime() {
        return m_nEnd;
    }
    void SetBeginTime(uint64_t a_nBegin) {
        m_nBegin = a_nBegin;
    }
    void SetEndTime(uint64_t a_nEnd) {
        m_nEnd = a_nEnd;
    }
    uint64_t GetLength() {
        if (m_nEnd < m_nBegin)
            return 0;
        return m_nEnd - m_nBegin;
    }
private:
    uint64_t m_nBegin;
    uint64_t m_nEnd;
};

class IndexData {
public:
    IndexData(char *mediaID, char *homedir);
    ~IndexData();
    char *toString();
    int init(char *rangeStr);
    uint64_t getBeginTime() {
        return m_pClip->GetBeginTime();
    }
    uint64_t getEndTime() {
        return m_pClip->GetEndTime();
    }
    void setBeginTime(uint64_t a_nTime) {
        m_pClip->SetBeginTime(a_nTime);
    }
    void setEndTime(uint64_t a_nTime) {
        m_pClip->SetEndTime(a_nTime);
    }

    uint64_t getLength();
    int updateStart(uint64_t startTime, uint64_t timeCode);
    int updateStop(uint64_t timeCode);
    Interval *getGapContaining(uint64_t startTimeCode);
    Interval *getGapStartingIn(uint64_t startTimeCode, uint64_t length);
    uint64_t getTimecodeAtOffset(uint64_t offset);
    uint64_t getOffsetFromTimecode(uint64_t timeCode);
    Interval *getGapAfter(uint64_t startTimeCode);

    int setLastError(const char *desc);
    char *getLastError();
    void dumpGapInfo();
    bool HasGaps();
    void AddGap(Interval *a_gap);
    Interval *GetFirstGap();
    int InProgress() {
        return m_bInProgress;
    }
    char * GetMediaID() {
        return m_pMediaID;
    }
    char * GetHomeDir() {
        return m_pHomeDir;
    }
private:
    char * getTimeString(int64_t a_milliseconds, char *a_buffer);
    char * getDateString(uint64_t a_milliseconds, char *a_buffer);
    char * m_pLastError;

    Interval *m_pClip;
    list<Interval *> m_GapList;
    list<Interval *> :: iterator m_gIter;
    int m_bInProgress;
    char *m_pMediaID;
    char *m_pHomeDir;


};

};

#endif
