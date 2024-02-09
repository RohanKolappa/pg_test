

#include "strmsrv_compatibility.h"
#include <algorithm>

#include "common.hh"
#include "averager.hh"

template <class T> CAverager<T>::CAverager(int a_nWindow)
{

    SYSLOG_INFO("Averager Window %d", a_nWindow);
    m_nWindow = a_nWindow;
    if (m_nWindow <= 0)
        m_nWindow = 1;

    m_fAlpha = 0.1;
    m_fOneMinusAlpha = 1.0 - m_fAlpha;

    m_nExpAvgerage = 0.0;
    m_nMaxValue = 0;
    m_nMaxSoFarValue = 0;

    m_nMinValue = 0xFFFFFFFF;
    m_nCounter = 0;
    m_bResetRequested = false;

    m_nValues = new T[m_nWindow];
    for (int i = 0; i < m_nWindow; i++) {
        m_nValues[i] = 0;
    }
}

template <class T> CAverager<T>::~CAverager() {
    delete [] m_nValues;
}

template <class T>
void CAverager<T>::Sample(T a_value)
{

    // Compute exponential average
    if (m_nCounter == 0) {
        m_nExpAvgerage = (double)a_value;
    }
    m_nExpAvgerage = (double)m_fAlpha * a_value +
        (double)m_fOneMinusAlpha * m_nExpAvgerage;

    // Accumulate values for moving average
    m_nValues[m_nCounter % m_nWindow] = a_value;

    m_nCounter++;

    // Compute Min/Max
    if (m_bResetRequested) {
        m_nMaxValue = 0;
        m_nMinValue = 0xFFFFFFFF;
        m_bResetRequested = false;
    }

    if (a_value > m_nMaxValue) {
        m_nMaxValue = a_value;
        if (a_value > m_nMaxSoFarValue) {
            m_nMaxSoFarValue = a_value;
        }
    }
    if (a_value < m_nMinValue) {
        m_nMinValue = a_value;
    }
}

template <class T>
double CAverager<T>::GetMovingAverage()
{
    T sum = 0;

    if (m_nCounter == 0)
        return 0.0;

    int samples = m_nCounter < m_nWindow ? m_nCounter : m_nWindow;

    for (int i = 0; i < samples; i++) {
        sum += m_nValues[i];
    }

    return ((double) sum)/samples;
}

template <class T>
double CAverager<T>::GetExponentialAverage()
{
    return m_nExpAvgerage;
}


template <class T>
void CAverager<T>:: ResetStats() {

    m_bResetRequested = true;

}

template <class T>
T CAverager<T>::GetPercentile(int a_percent)
{
    if (m_nCounter == 0)
        return 0;

    int samples = m_nCounter < m_nWindow ? m_nCounter : m_nWindow;

    T *m_nSortedValues = new T[samples];

    memcpy(m_nSortedValues, m_nValues, samples * sizeof(T));

    std::sort(m_nSortedValues, m_nSortedValues + samples);

    int index = int((1.0 * samples * a_percent)/100.0) - 1;

    if (index < 0)
        index = 0;

    T percentile =  m_nSortedValues[index];

    delete [] m_nSortedValues;

    return percentile;
}

template class CAverager<unsigned long>;
template class CAverager<double>;
template class CAverager<uint64_t>;
