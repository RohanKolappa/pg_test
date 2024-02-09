#ifndef __H_STREAMER__
#define __H_STREAMER__

//#include "squeue.hh"


#include <string>
#include <map>
#include <inttypes.h>

#include "command.hh"
#include "timekeeper.hh"
#include "notify.hh"
#include "StreamInfo.hh"

#define OBJ_CONNECTED           1
#define OBJ_STARTED             2
#define OBJ_ERROR               4

// Forward declarations
class CSQueueSource;
class CSQueueSink;

/// The CStreamer Parent Class
class CStreamer {

public:
    /// The CStreamer constructor
    CStreamer(int a_id);

    /// The Destructor
    //virtual ~CStreamer();

    void AddReference();
    void RemoveReference();

    virtual void AddStream();
    virtual void DeleteStream();

    virtual int ProcessStream() = 0;

    virtual int StartStream(int nQueueId);
    int Start(int nQueueId);

    virtual int StopStream();
    int Stop();

    int StopAt(timestamp_t nTimecode);



    void ComputeStatistics(int a_timeDiff);
    void SendStatistics(std::string& a_stats);
    virtual void PrintStatistics(std::string& a_stats);
    void UpdateStatus(CCommand *a_pCmd);
    virtual void UpdateCustomStatus(CCommand *a_pCmd);

    virtual int Update(CCommand *a_pCmd) { return 0;};

    void SendError(std::string& a_error);
    virtual void PrintError(CNotify *a_notify);

    void SendNotification(std::string& a_notify);
    virtual void PrintNotification(CNotify *a_notify);

    void SendErrorNotificationString(int a_nErrorCode,
                               const std::string& a_pErrorString);
    void SendErrorNotification(int a_nErrorCode,
                               const std::ostringstream& a_pErrorStream);

//    void SendErrorNotification(int anErrorCode,
//                               const char* a_format,
//                               ...) __attribute__ ((format (printf, 1, 2)));

    void SendErrorNotification(int anErrorCode,
                               const char* a_format,
                               ...);

    void SetError();
    bool IsErrored() { return (m_nState & OBJ_ERROR); };
    bool IsStarted() { return (m_nState & OBJ_STARTED); };
    bool IsConnected() { return (m_nState & OBJ_CONNECTED); };
    void SetConnected() { m_nState |= OBJ_CONNECTED; };
    void SetDisConnected() { m_nState &= ~OBJ_CONNECTED; };

    int GetId() const {return m_nId;};
    int GetQueueId() {return m_nQueueId;};
    int GetState() const {return m_nState;};
    timestamp_t GetTimecode() { return GetCurrentTimeStamp(); };
    bool GetStatsFlag() const { return m_nStatsFlag; };
    void SetStatsFlag(bool nStatsFlag) { m_nStatsFlag = nStatsFlag; };
    bool GetErrorFlag() const { return m_bSendError; };
    bool GetNotifyFlag() const { return m_bSendNotify; };
    double GetBitRate() const { return m_bitRate; };
    unsigned long GetFrameDiff() const { return m_frameDiff; };
    bool IsStarted() const { return m_bStarted; };

    virtual void SetCustomStatusInfo() {};
    static const char * GetInfoString(int a_portType, const char * a_name);
    static const char * GetTypeString(int a_portType);
    static const char * GetStateString(int a_state, std::string &status);
    timestamp_t GetCurrentTimeStamp();
    timestamp_t GetCurrentTimeSpan();
    timestamp_t GetFrameTimeSpan();
    timestamp_t GetTimeStamp() { return m_nTimestamp; };
    CSQueueSink * GetQueueSink() { return m_qQueueSink; };
    CSQueueSource * GetQueueSource() { return m_qQueueSource; };

private:
    int                     m_refCount;
    bool                    m_bSendError;       /// Error flag
    unsigned long           m_nStartCount;
    unsigned long           m_lastBytesTransfered;
    unsigned long           m_lastFrameCount;
    unsigned long           m_bytesDiff;
    unsigned long           m_frameDiff;
    unsigned long           m_statsTick;
    int                     m_nErrorCode;
    int                     m_nLastErrorCode;
    double                  m_bitRate;
    double                  m_avgBitRate;

    static std::map<std::string, std::string> m_infoMap;



protected:
    const int               m_nId;
    int                     m_nPortType;
    int                     m_nEventId;

    bool                    m_bSendNotify;      /// Notify flag
    bool                    m_nStatsFlag;       /// Stats Flag
    bool                    m_bStarted;
    bool                    m_bStopPending;
    unsigned long           m_errors;
    int                     m_nQueueId;                 /// Stream Identifier
    double                  m_dSpeed;
    bool                    m_bTrickMode;

    timestamp_t             m_nStartTimestamp;
    timestamp_t             m_nStartWallClock;
    timestamp_t             m_nObjectCreateTime;

    // The most often used parameters are clubbed together here to
    // give improved cache (L1/L2) performance
    timestamp_t             m_nStopWallClock;
    timestamp_t             m_nStopTime;
    int                     m_nState;
    timestamp_t             m_nTimestamp;
    timestamp_t             m_nFrameWallClock;
    uint64_t                m_bytesTransfered;
    uint64_t                m_frameCount;
    CSQueueSource *         m_qQueueSource;
    CSQueueSink *           m_qQueueSink;

    timestamp_t             m_nErrorTimeStamp;
    std::string             m_pErrorString;

    void SetInfoString(const char * a_name, const char * a_help);
    unsigned long GetStartCount() { return m_nStartCount; };
    void SetStreamInfo(const CStreamInfo * streamInfo);
    void ResetStreamInfo();

    /// The Destructor
    virtual ~CStreamer();


};
#endif /* __H_STREAMER__ */
