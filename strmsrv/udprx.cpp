

#include "strmsrv_compatibility.h"
#include "udprx.hh"
#include "perf.hh"
#include "common.hh"
#include "MulticastUtils.hh"

using namespace std;

typedef struct data_hdr
{
    unsigned long long h_nTimestamp;
    unsigned long long h_nSequence;
} data_hdr_t;

const int CUdpRx::m_BUFFER_SIZE = 64 * 1024;

CUdpRx::CUdpRx(int nId,
               int a_sourcePort,
               const char * a_mcastaddr,
               int nPerfInterval,
               int a_genBW,
               intlist & a_interfaceList) :
    CReceiver(nId)
{
    m_nPortType = UDPNETWORKPORT;
    m_nPort = a_sourcePort;
    m_sBuffer = NULL;
    m_readOffset = 0;
    m_perfData = NULL;
    m_perfSnapShot = NULL;
    m_generateBw = a_genBW;
    m_nTotalbits = 0;
    m_nSequence = 0;
    m_fillBuffer = NULL;

    if (strcmp(a_mcastaddr, "") != 0) {
        if (m_source.SetAddress(a_mcastaddr, a_sourcePort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                         a_mcastaddr, a_sourcePort);
            return;
        }
    }
    else {
        if (m_source.SetAddress(a_sourcePort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Port %d\n", a_sourcePort);
            return;
        }
    }

    CMulticastUtils::RegisterMulticastRecvInterfaces(m_socket, m_source,
            a_interfaceList, m_sInterfaceInfo);

    if (m_socket.SetSource(m_source) == -1) {
        SetError();
        SYSLOG_ERROR("Network Error: %s\n",
                     m_socket.GetLastError().c_str());
        return;
    }


    // Get hold of a big SBuffer
    m_sBuffer = new CSBuffer(m_BUFFER_SIZE);
    m_sBuffer->AddReference();


    m_perfInterval  = 0;
    m_perfData = NULL;
    m_perfSnapShot = NULL;
    if (nPerfInterval > 0) {
        m_perfData      = (struct SPerfData *) malloc(sizeof(SPerfData));
        m_perfSnapShot  = (struct SPerfData *) malloc(sizeof(SPerfData));
    }
    if (m_perfData != NULL && m_perfSnapShot != NULL) {
        m_perfInterval  = nPerfInterval;
        memset(m_perfData, 0x00, sizeof(SPerfData));
        memset(m_perfSnapShot, 0x00, sizeof(SPerfData));
    }

    m_socket.SetReceiveBufferSize(65535 * 10);
    SetConnected();

    /* If generate bandwidth requested */
    if (m_generateBw > 0) {
        m_socket.Close();
        m_perfInterval = 0;
        m_fillBuffer = new unsigned char[m_BUFFER_SIZE];
        for (int i = 0; i < m_BUFFER_SIZE; i++) {
            m_fillBuffer[i] = random() % 255;
        }
    }
}

CUdpRx::~CUdpRx() {

    if (m_perfData != NULL)
        free (m_perfData);
    if (m_perfSnapShot != NULL)
        free (m_perfSnapShot);
    if (m_sBuffer)
        m_sBuffer->RemoveReference();
    if (m_fillBuffer != NULL)
        delete [] m_fillBuffer;
}

int CUdpRx::ProcessStream() {
    int len = 0;

    if (m_qQueueSink == NULL)
        return 0;

    if (IsErrored())
        return 0;

    char *mesgbuf = m_sBuffer->GetBuffer(m_readOffset);
    if (mesgbuf == NULL) { // memory pressure, give up on this object
        SetError();
        m_socket.Close();
        return 0;
    }

    if (m_nTimestamp == 0) {
        m_nStartTimestamp = CTimeKeeper::Now();
        m_nStartWallClock = CTimeKeeper::Now();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_BEGIN;
        m_nTotalbits = 0;
    }
    m_nTimestamp = CTimeKeeper::Now();
    m_nFrameWallClock = CTimeKeeper::Now();


    if (m_generateBw == 0) {
        if (!m_socket.IsValid())
            return 0;

        len = m_socket.Receive(mesgbuf, m_BUFFER_SIZE - m_readOffset);
    }
    else {

        if (m_nTimestamp > m_nStartTimestamp) {
            double currentbitrate =
                ( 1000.0 * m_nTotalbits)/((1.0 * (int)(m_nTimestamp - m_nStartTimestamp)));
            if (currentbitrate < (double)m_generateBw) {
                data_hdr_t data_header;

                data_header.h_nTimestamp = CTimeKeeper::GetTimeInMicroSec();
                data_header.h_nSequence = m_nSequence;

                ASSERT(m_BUFFER_SIZE - m_readOffset >= 1024);
                memcpy(mesgbuf, m_fillBuffer + m_readOffset, 1024);
                memcpy(mesgbuf, &data_header, sizeof(data_hdr_t));
                len = 1024;
                m_nSequence++;
                m_nTotalbits += 1024 * 8;
            }
        }
    }
    if (len > 0) {

        CSFrame *sFrame = new CSFrame(m_sBuffer, m_readOffset,
                                      len,
                                      CSFrame::CreateMediaFlag(0, STREAMTYPE_UDP),
                                      m_nTimestamp);
        m_qQueueSink->InsertFrame(sFrame);
        m_bytesTransfered += len;
        m_frameCount++;
        m_readOffset += len;

        if (m_perfInterval > 0) {
            struct SDataHeader DataHeader;
            memcpy(&DataHeader, mesgbuf, sizeof(DataHeader));
            if (computePerformaceStats(m_perfData,
                                       &DataHeader,
                                       len,
                                       m_perfInterval, 5000)) {
                // interval reached, save a copy
                memcpy(m_perfSnapShot, m_perfData,
                       sizeof(struct SPerfData));
            }
        }

    }

    if (m_BUFFER_SIZE - m_readOffset < 2048) {
        m_sBuffer->RemoveReference();
        m_sBuffer = new CSBuffer(m_BUFFER_SIZE);
        m_sBuffer->AddReference();
        m_readOffset = 0;
    }
    if (len > 0)
        return 1;

    return 0;
}

void
CUdpRx::PrintStatistics(string &a_stats)
{

}


void
CUdpRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("StartTime", m_nStartTimestamp);
    a_pCmd->SetObjectParam("Peer", m_socket.GetPeerAddress().GetHost());
    a_pCmd->SetObjectParam("Port", (int)m_source.GetPort());
    a_pCmd->SetObjectParam("LocalHost", m_source.GetHost());
    a_pCmd->SetObjectParam("Multicast", (bool)m_source.IsMulticast());
    a_pCmd->SetObjectParam("MulticastInterfaces", m_sInterfaceInfo);

    if (m_perfSnapShot != NULL) {

        a_pCmd->SetObjectConstantParam("PerfInterval",
                                       m_perfInterval);
        a_pCmd->SetObjectErrorCounterParam("SequenceError",
                                           m_perfSnapShot->m_sequenceError);
        a_pCmd->SetObjectErrorCounterParam("OutOfOrder",
                                           m_perfSnapShot->m_outOfOrderError);
        a_pCmd->SetObjectErrorCounterParam("Dropped",
                                           m_perfSnapShot->m_totalDropped);
        a_pCmd->SetObjectParam("AvgJitter",
                               (double)m_perfSnapShot->m_avgJitter);
        a_pCmd->SetObjectParam("RunAvgJitter",
                               (double)m_perfSnapShot->m_runningAvgJitter);
        a_pCmd->SetObjectParam("AvgMaxJitter",
                               (double)m_perfSnapShot->m_avgMaxJitter);
        a_pCmd->SetObjectParam("RangeJitter",
                               (double)m_perfSnapShot->m_avgRangeJitter);
        a_pCmd->SetObjectParam("PacketsInInterval",
                               (unsigned long)m_perfSnapShot->m_intervalPacketCount);

    }
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("MaxQueueLen",
                               (unsigned long)m_qQueueSource->MaxLength());
#ifdef SSDEBUG
        a_pCmd->SetObjectParam("AverageQueueLen",
                               (double)m_qQueueSource->AvgSize());
#endif
    }

#ifdef SSDEBUG
    if (m_qQueueSink != NULL) {
        a_pCmd->SetObjectParam("SinkContention",
                               (unsigned long)m_qQueueSink->GetContention());
    }
#endif

}
