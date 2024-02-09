

#include "strmsrv_compatibility.h"
#include <math.h>
#include <stdarg.h>
#include <sstream>

#include "common.hh"
#include "streamer.hh"
#include "squeue.hh"
#include "notify.hh"

using namespace std;

map<string, string> CStreamer::m_infoMap;

CStreamer::CStreamer(int a_nId) :
    m_refCount(0),
    m_bSendError(false),
    m_nStartCount(0),
    m_lastBytesTransfered(0),
    m_lastFrameCount(0),
    m_bytesDiff(0),
    m_frameDiff(0),
    m_statsTick(0),
    m_bitRate(0.0),
    m_avgBitRate(0.0),
    m_nId(a_nId),
    m_nPortType(0),
    m_nEventId(NOTIFICATION_EVENT_NOP),
    m_bSendNotify(false),
    m_nStatsFlag(true),
    m_bStarted(false),
    m_bStopPending(false),
    m_errors(0),
    m_nQueueId(0),
    m_dSpeed(0.0),
    m_bTrickMode(false),
    m_nStartTimestamp(0),
    m_nStartWallClock(0),
    m_nStopWallClock(0),
    m_nStopTime(0),
    m_nState(0),
    m_nTimestamp(0),
    m_nFrameWallClock(0),
    m_bytesTransfered(0),
    m_frameCount(0),
    m_qQueueSource(NULL),
    m_qQueueSink(NULL)
{
    SYSLOG_TRACE("Constructing %d in %s\n", m_nId, __FUNCTION__);
    m_nObjectCreateTime = CTimeKeeper::Now();

    m_nErrorCode = 0;
    m_nLastErrorCode = 0;
    m_nErrorTimeStamp = 0;
}

CStreamer::~CStreamer()
{
    ASSERT(m_refCount == 0);
    SYSLOG_TRACE("Destructing %d in %s\n", m_nId, __FUNCTION__);
}

void
CStreamer::AddReference()
{
    m_refCount++;
}

void
CStreamer::RemoveReference()
{
    ASSERT(m_refCount > 0);
    m_refCount--;
    if (m_refCount <= 0) {
        delete(this);
    }
}

void
CStreamer::AddStream()
{
    AddReference();
}

void
CStreamer::DeleteStream()
{
    RemoveReference();
}

int
CStreamer::StartStream(int nQueueId)
{
    // Default implementation does nothing.
    // Subclasses customize as needed.
    return 0;
}

/// Start routine for all streamers
int
CStreamer::Start(int nQueueId)
{
    ASSERT(m_bStarted == false);
    if (m_bStarted == true)
        return 0;
    SYSLOG_DEBUG("Starting id=%d, qid=%d\n", m_nId, nQueueId);
    m_nQueueId = nQueueId;
    m_nState |= OBJ_STARTED;
    m_nStopTime=0;
    m_bStarted = true;
    m_bStopPending = false;
    m_nTimestamp = 0;
    m_nFrameWallClock = 0;
    m_nStartTimestamp = 0;
    m_nStartCount++;

    return StartStream(nQueueId);

};

int
CStreamer::StopStream()
{
    // Default implementation does nothing.
    // Subclasses customize as needed.
    return 0;
}

/// Stop routine for all streamers
int
CStreamer::Stop()
{
    ASSERT(m_bStarted == true);
    if (m_bStarted == false)
        return 0;

    m_nStopWallClock = CTimeKeeper::Now();

    SYSLOG_DEBUG("Stopping id=%d, qid=%d\n", m_nId, m_nQueueId);
    m_nQueueId = 0;
    m_nState &= (0xff ^ OBJ_STARTED);
    m_bStarted = false;
    m_bStopPending = false;
    m_bSendNotify = true;
    m_nEventId = NOTIFICATION_EVENT_END;
    return StopStream();

}

int
CStreamer::StopAt(timestamp_t nTimecode)
{
    if (!nTimecode) {
        Stop();
    }
    else {
        m_nStopTime = nTimecode;
        m_bStopPending = true;
    }
    return 0;
}

const char *
CStreamer::GetTypeString(int a_portType) {
    switch (a_portType) {
    case V2DNETWORKPORT:
        return "V2DNetworkPort";
    case VNCNETWORKPORT:
        return "VNCNetworkPort";
    case WINFBENCODERPORT:
        return "WINFBEncoderPort";
    case WISENCODERPORT:
        return "WISEncoderPort";
    case V2DENCODERPORT:
        return "V2DEncoderPort";
    case RTPNETWORKPORT:
        return "RTPNetworkPort";
    case UDPNETWORKPORT:
        return "UDPNetworkPort";
    case MPEGTSNETWORKPORT:
        return "MPEGTSNetworkPort";
    case DMSMEDIAPORT:
        return "DMSMediaPort";
    case V2DDECODERPORT:
        return "V2DDecoderPort";
    case MPEGTSDECODERPORT:
        return "MPEGTSDecoderPort";
    case RTPDECODERPORT:
        return "RTPDecoderPort";
    case UDPDECODERPORT:
        return "UDPDecoderPort";
    case DMSRECORDERPORT:
        return "DMSRecorderPort";
    case DELETERPORT:
        return "DeleterPort";
    case SCHEDULERPORT:
        return "SchedulerPort";
    case V2DFFPORT:
        return "V2DFastFwdPort";
    case V2DHWPORT:
        return "V2DHardWarePort";
    case V2DAUDIOENCODERPORT:
        return "V2DAudioEncoderPort";
    case V2DAUDIODECODERPORT:
        return "V2DAudioDecoderPort";
    case V2DPLAYERPORT:
        return "V2DPlayerPort";
    case RTPPLAYERPORT:
        return "RTPPlayerPort";
    case IMAGEWRITERPORT:
        return "ImageWriterPort";
    default:
        ASSERT(0);
        return "Unknown";

    }
}

const char *
CStreamer::GetStateString(int a_state, string &status) {

    if (a_state & OBJ_ERROR)
        status.append("Error ");
    if (a_state & OBJ_CONNECTED)
        status.append("Connected ");
    if (a_state & OBJ_STARTED)
        status.append("Started");
    else
        status.append("Stopped");

    return status.c_str();

}

void
CStreamer::ComputeStatistics(int a_timeDiff) {

    m_frameDiff = m_frameCount - m_lastFrameCount;
    m_bytesDiff = m_bytesTransfered - m_lastBytesTransfered;
    if (a_timeDiff > 1) {
        m_bitRate = (m_bytesDiff*8000.0)/(1048576.0*a_timeDiff);
    }
    if (m_bytesDiff == 0) {
        m_avgBitRate = 0.0;
        m_statsTick = 0;
    }
    else {
        double alpha;
        m_statsTick++;
        if (m_statsTick <= 2)
            alpha = 1.0;
        else if (m_statsTick <= 10)
            alpha = 0.5;
        else
            alpha = 0.1;
        m_avgBitRate = alpha * m_bitRate + (1.0 - alpha) * m_avgBitRate;
    }
    m_lastBytesTransfered = m_bytesTransfered;
    m_lastFrameCount = m_frameCount;
}

void
CStreamer::PrintStatistics(string &a_stats) {
    // Default implementation does nothing.
    // Subclasses customize as needed.
}

void
CStreamer::SendStatistics(string &a_stats)
{
    string stats;
    char cp[256];


    sprintf (cp, "%s i:%d t:%d q:%d p:%lu b:%lu r:0 ",
             STATUSMSG, m_nId, m_nPortType, m_nQueueId,
             m_frameDiff, m_bytesDiff);

    stats.append(cp);

    PrintStatistics(stats);
    stats.append(" ");

    //SYSLOG_INFO ("%s\n", stats.c_str());
    a_stats.append(stats);
}

void
CStreamer::UpdateCustomStatus(CCommand *a_pCmd)
{
    // Default implementation does nothing.
    // Subclasses customize as needed.
}

void
CStreamer::UpdateStatus(CCommand *a_pCmd)
{
    char buffer[32];
    // Update status common for all objects
    a_pCmd->SetCurrentObject(m_nId);
    a_pCmd->SetObjectParam("Type", GetTypeString(m_nPortType));
    a_pCmd->SetObjectConstantParam("QueueId", m_nQueueId);
    a_pCmd->SetObjectParam("Error", (bool)(m_nState & OBJ_ERROR));
    a_pCmd->SetObjectParam("Connected", (bool)(m_nState & OBJ_CONNECTED));
    a_pCmd->SetObjectParam("Started", (bool) (m_nState & OBJ_STARTED));
    a_pCmd->SetObjectParam("Timestamp", (unsigned long long) GetCurrentTimeStamp());

    a_pCmd->SetObjectParam("Duration",
                           CTimeKeeper::GetDurationString(CTimeKeeper::Now() - m_nObjectCreateTime, buffer));

    a_pCmd->SetObjectParam("StreamDuration",
                           CTimeKeeper::GetDurationString(GetCurrentTimeSpan(), buffer));

    a_pCmd->SetObjectParam("FrameDuration",
                           CTimeKeeper::GetDurationString(GetFrameTimeSpan(), buffer));
    a_pCmd->SetObjectErrorCounterParam("ErrorCount", m_errors);
    a_pCmd->SetObjectByteCounterParam("BytesTransfered", m_bytesTransfered);
    a_pCmd->SetObjectGuageParam("BytesPerSec", m_bytesDiff);
    a_pCmd->SetObjectCounterParam("FramesTransfered", m_frameCount);
    a_pCmd->SetObjectGuageParam("FramesPerSec", m_frameDiff);
    a_pCmd->SetObjectParam("Bitrate", (double)m_bitRate);
    a_pCmd->SetObjectParam("AvgBitrate", (double)m_avgBitRate);
    a_pCmd->SetObjectParam("FrameSize",
                           (double)((m_frameDiff > 1) ?
                                    (1.0*m_bytesDiff)/m_frameDiff : 0.0));

    a_pCmd->SetObjectCounterParam("StartCount", m_nStartCount);
    a_pCmd->SetObjectParam("ErrorString", m_pErrorString.c_str());
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectErrorCounterParam("QueueDrops",
                                           m_qQueueSource->DropCount());
    }
    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectParam("Speed", m_dSpeed);
        a_pCmd->SetObjectParam("TrickMode", m_bTrickMode);
    }
    // Update custom stats
    UpdateCustomStatus(a_pCmd);
}

void
CStreamer::PrintError(CNotify *a_notify) {
    // Default implementation does nothing.
    // Subclasses customize as needed.
}

void
CStreamer::SendErrorNotificationString(int a_nErrorCode,
                                 const string& a_pErrorString)
{
    m_nErrorCode = a_nErrorCode;
    m_pErrorString = a_pErrorString;
    m_bSendError = true;
}

void
CStreamer::SendErrorNotification(int a_nErrorCode,
                                 const ostringstream & a_pErrorStream)
{
    SendErrorNotificationString(a_nErrorCode, a_pErrorStream.str());
}

void
CStreamer::SendErrorNotification(int a_nErrorCode,
        const char* a_format, ...)
{

    char buffer[256];
    va_list va;
    va_start(va, a_format);
    vsnprintf(buffer, 255, a_format, va);
    va_end(va);

    SendErrorNotificationString(a_nErrorCode, string(buffer));
}

void
CStreamer::SetError()
{
    m_nState |= OBJ_ERROR;
    m_errors++;
}


void
CStreamer::SendError(string &a_error)
{
    if (!m_bSendError)
        return;

    // Throttle error notifications

    // If error code has not changed, wait for a minute before
    // sending the same notification again
    if (m_nErrorCode == m_nLastErrorCode) {
        if (CTimeKeeper::Now() - m_nErrorTimeStamp < 60000) {
            m_bSendError = false;
            return;
        }
    }

    // If error code HAS changed, wait for a second before sending
    // the new notification
    if (CTimeKeeper::Now() - m_nErrorTimeStamp < 1000) {
        m_bSendError = false;
        return;
    }

    if (m_nErrorCode == 0)
        return;

    // Create new "error" notify object
    CNotify *pNotify = new CNotify(NOTIFICATION_EVENT_ERROR, m_nId, m_nPortType,
                                   m_nQueueId, m_nState);
    a_error.append(NOTIFYMSG);
    a_error.append(" ");

    // Create Stock Error Notification
    pNotify->CreateErrorNotification(m_nErrorCode, m_pErrorString);

    // Add in object specific data
    PrintError(pNotify);

    // Retrieve final notification string
    a_error.append(pNotify->GetMessage());

    delete pNotify;


    m_nLastErrorCode = m_nErrorCode;
    m_nErrorTimeStamp = CTimeKeeper::Now();
    m_bSendError = false;
}


void
CStreamer::PrintNotification(CNotify *a_notify) {
    // Default implementation does nothing.
    // Subclasses customize as needed.
}

void
CStreamer::SendNotification(string &a_notify)
{
    if (!m_bSendNotify)
        return;

    // Create new notify object
    CNotify *pNotify = new CNotify(m_nEventId, m_nId, m_nPortType, m_nQueueId, m_nState);
    a_notify.append(NOTIFYMSG);
    a_notify.append(" ");

    // Fill in stock parameters common for all objects
    switch (m_nEventId) {
    case NOTIFICATION_EVENT_BEGIN:
        pNotify->SetNotificationData("StartTimeStamp", m_nStartTimestamp);
        pNotify->SetNotificationData("StartWallClock", m_nStartWallClock);
        break;
    case NOTIFICATION_EVENT_END:
        pNotify->SetNotificationData("StopTimeStamp", GetCurrentTimeStamp());
        pNotify->SetNotificationData("StopWallClock", m_nStopWallClock);
        break;
    default:
        break;
    }

    // Add in object specific data
    PrintNotification(pNotify);

    // Retrieve final notification string
    a_notify.append(pNotify->GetMessage());

    delete pNotify;

    m_bSendNotify = false;

}

void
CStreamer::SetInfoString(const char * a_name, const char * a_help)
{
    ostringstream uname;
    uname << m_nPortType << "_" << a_name;
    m_infoMap[uname.str()] = a_help;

}

const char *
CStreamer::GetInfoString(int a_portType, const char * a_name)
{
    ostringstream uname;
    uname << a_portType << "_" << a_name;
    if (m_infoMap.count(uname.str()) == 0)
        return NULL;
    return m_infoMap[uname.str()].c_str();

}

void CStreamer::ResetStreamInfo() {
    m_dSpeed = 1.0;
    m_bTrickMode = false;
}

void CStreamer::SetStreamInfo(const CStreamInfo * streamInfo) {
    m_dSpeed = streamInfo->GetSpeed();
    m_bTrickMode = streamInfo->GetTrickMode();
}

timestamp_t
CStreamer::GetCurrentTimeStamp() {
    timestamp_t nTimeStamp = 0;
    if (m_nTimestamp > 0 && m_nFrameWallClock > 0) {
        if (m_nState & OBJ_STARTED) {
            if (!m_bTrickMode)
                nTimeStamp = m_nTimestamp + (CTimeKeeper::Now() - m_nFrameWallClock);
            else
                nTimeStamp = m_nTimestamp +
                    (uint64_t)round((CTimeKeeper::Now() - m_nFrameWallClock) * m_dSpeed);
        }
        else {
            if (m_nStopWallClock > 0) {
                if (!m_bTrickMode)
                    nTimeStamp = m_nTimestamp + (m_nStopWallClock - m_nFrameWallClock);
                else
                    nTimeStamp = m_nTimestamp +
                       (uint64_t)round((m_nStopWallClock - m_nFrameWallClock) * m_dSpeed);
            }
            else {
                nTimeStamp = 0;
            }
        }
    }
    return nTimeStamp;
}

timestamp_t
CStreamer::GetCurrentTimeSpan() {
    return (GetCurrentTimeStamp() ? GetCurrentTimeStamp() - m_nStartTimestamp : 0);
};

timestamp_t
CStreamer::GetFrameTimeSpan() {

    timestamp_t nTimeStamp = 0;
    if (m_nTimestamp > 0) {
        ASSERT(m_nStartTimestamp > 0);
        nTimeStamp = m_nTimestamp  - m_nStartTimestamp;
    }
    return nTimeStamp;
};
