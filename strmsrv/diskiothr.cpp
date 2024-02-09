#include "strmsrv_compatibility.h"

#include "diskiothr.hh"

timestamp_t
CDiskIOThr::m_nLastFileOpTimestamp = 0;


timestamp_t 
CDiskIOThr::GetFileOpTimestamp() {
    return m_nLastFileOpTimestamp;
}

void
CDiskIOThr::SetFileOpTimestamp(timestamp_t a_timestamp) {
    m_nLastFileOpTimestamp = a_timestamp;
}
