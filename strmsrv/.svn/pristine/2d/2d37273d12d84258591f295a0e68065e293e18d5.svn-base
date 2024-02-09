#ifndef __H_DISKIOTHR__
#define __H_DISKIOTHR__

#include "appthread.hh"
#include "command.hh"

/// The DiskIO Thread Class

typedef std::map<std::string, std::string> IOParam;
typedef IOParam::value_type IOEntry;

#define OBJECTTYPE_READER 1


class CDiskIOThr
{
private:
    static timestamp_t m_nLastFileOpTimestamp;
    
public:
    static void SetFileOpTimestamp(timestamp_t a_timestamp);
    static timestamp_t GetFileOpTimestamp();
};
#endif /* __H_DISKIOTHR__ */
