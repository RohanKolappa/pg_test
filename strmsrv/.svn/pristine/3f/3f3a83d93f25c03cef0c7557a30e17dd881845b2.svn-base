#ifndef __H_WRITERDISKIOINFO__
#define __H_WRITERDISKIOINFO__


class CWriterDiskIOInfo {

public:
    enum MessageType {
        Write = 1,
        WriteIndex,
        Start,
        Stop,
        GetNext,
        Quit,
        Progress,
    };

    CWriterDiskIOInfo(MessageType a_nMessageType,
                      timestamp_t a_nTimestamp) {
        m_nMessageType = a_nMessageType;
        m_nTimestamp = a_nTimestamp;
    }
    virtual ~CWriterDiskIOInfo() {};

    MessageType GetMessageType() {
        return m_nMessageType;
    }

    timestamp_t GetTimeStamp() {
        return m_nTimestamp;
    }

private:
    MessageType m_nMessageType;
    timestamp_t m_nTimestamp;
};

class CWriterDiskIOProgress : public CWriterDiskIOInfo {
public:
    CWriterDiskIOProgress(uint64_t playLength, uint64_t bytesWritten, uint64_t bytesDeleted,
            bool inProgress, timestamp_t nTimestamp)
    : CWriterDiskIOInfo(Progress, nTimestamp),
      m_playLength(playLength),
      m_bytesWritten(bytesWritten),
      m_bytesDeleted(bytesDeleted),
      m_inProgress(inProgress)
      {
      }
    CWriterDiskIOProgress(CWriterDiskIOProgress * aProgressInfo)
    : CWriterDiskIOInfo(Progress, 0),
      m_playLength(aProgressInfo->GetPlayLength()),
      m_bytesWritten(aProgressInfo->GetBytesWritten()),
      m_bytesDeleted(aProgressInfo->GetBytesDeleted()),
      m_inProgress(aProgressInfo->GetInProgress())
      {
      }
    void AdjustByteCounts(uint64_t bytesWritten, uint64_t bytesDeleted) {
        m_bytesWritten += bytesWritten;
        m_bytesDeleted += bytesDeleted;
    }
    uint64_t GetPlayLength() { return m_playLength; };
    uint64_t GetBytesWritten() { return m_bytesWritten; };
    uint64_t GetBytesDeleted() { return m_bytesDeleted; };
    bool GetInProgress() { return m_inProgress; };

private:
    uint64_t m_playLength;
    uint64_t m_bytesWritten;
    uint64_t m_bytesDeleted;
    bool m_inProgress;
};


#endif // __H_WRITERDISKIOINFO__
