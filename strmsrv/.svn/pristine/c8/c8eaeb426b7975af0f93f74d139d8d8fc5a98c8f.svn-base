#ifndef __AVERAGE_H__
#define __AVERAGE_H__


template <class T> class CAverager {

public:
    CAverager(int a_nWindow = 500);
    ~CAverager();

    void Sample(T a_value);
    double GetMovingAverage();
    double GetExponentialAverage();


    T GetMaxValue() {
        return m_nMaxValue;
    }
    T GetMinValue() {
        return m_nMinValue;
    }
    T GetMaxSoFar() {
        return m_nMaxSoFarValue;
    }
    void ResetStats();

    T GetPercentile(int a_percent);
    int GetSampleCount() { return m_nCounter; };

private:
    int m_nWindow;
    int m_nCounter;
    T *m_nValues;
    double m_nExpAvgerage;
    double m_fAlpha;
    double m_fOneMinusAlpha;
    T m_nMaxValue;
    T m_nMinValue;
    T m_nMaxSoFarValue;
    bool m_bResetRequested;

};

#endif                          // __AVERAGE_H__
