
#include "strmsrv_compatibility.h"
#include "udptx.hh"
#include "perf.hh"
#include "socket.hh"
#include "common.hh"
#include "MulticastUtils.hh"

using namespace std;

CUdpTx::CUdpTx (int nId,
                char *pDest,
                int nPort,
                int nTTL,
                int nPerfInterval,
                int nInterface) :
    CSender(nId)
{
    m_nPortType = UDPDECODERPORT;

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

    if (m_destination.SetAddress(pDest, nPort) == -1) {
        SetError();
        SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                     pDest, nPort);
    }

    CMulticastUtils::RegisterMulticastSendInterface(m_socket, m_destination,
            nInterface, m_sInterfaceInfo);

    if (m_socket.SetDestination(m_destination) == -1) {
        SetError();
        SYSLOG_ERROR("Network Error: %s\n",
                     m_socket.GetLastError().c_str());
    }

    m_socket.SetMcastTTL(nTTL);

    m_nProgressTimestamp = 0;
    m_nSessionId = 0;
#ifdef SSDEBUG
    // seed random number generator using current time
    srandom(CTimeKeeper::Now());
    m_nSessionId = random();
#endif
}


CUdpTx::~CUdpTx()
{
    if (m_perfData)
        free(m_perfData);
    if (m_perfSnapShot)
        free(m_perfSnapShot);

    return;
}

void
CUdpTx::HandleFirstTimestamp(timestamp_t nFirstTimeStamp)
{
    // Call the base class method first
    CSender::HandleFirstTimestamp(nFirstTimeStamp);

    // Custom init section
#ifdef SSDEBUG
    // Send a "Start" progress packet
    if (m_perfInterval > 0) {
        struct SDataHeader data_header;
        char mesgbuf[128];
        m_nProgressTimestamp = m_nTimestamp;
        data_header.h_nTimestamp = m_nProgressTimestamp;
        data_header.h_oob.h_magic = PERF_MAGIC;
        data_header.h_oob.h_sessionid = m_nSessionId;
        data_header.h_oob.h_type = PERF_TYPE_START;
        memcpy(mesgbuf, &data_header, sizeof(struct SDataHeader));
        m_socket.Send(mesgbuf, 128);
    }
#endif

    return;
}


int
CUdpTx::StopSender()
{
#ifdef SSDEBUG
    // Send a "Stop" progress packet
    if (m_perfInterval > 0) {
        struct SDataHeader data_header;
        char mesgbuf[128];
        m_nProgressTimestamp = GetCurrentTimeStamp();
        data_header.h_nTimestamp = m_nProgressTimestamp;
        data_header.h_oob.h_magic = PERF_MAGIC;
        data_header.h_oob.h_sessionid = m_nSessionId;
        data_header.h_oob.h_type = PERF_TYPE_STOP;
        memcpy(mesgbuf, &data_header, sizeof(struct SDataHeader));
        m_socket.Send(mesgbuf, 128);
    }
#endif
    return 0;
}

int
CUdpTx::ProcessStream()
{

    int retval = 0;

    if (m_qQueueSource == NULL)
        return 0;

    CSFrame *sFrame = m_qQueueSource->GetFrame();

    if (sFrame) {

        char *pData = sFrame->GetData();

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }

        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();
#ifdef SSDEBUG
        m_nProgressTimestamp = m_nTimestamp;
#endif

        if (sFrame->GetMessageType() == CSFrame::Normal) {

            m_bytesTransfered += sFrame->GetLength();
            m_frameCount++;

            // If destination is set, we send the packet
            if (!IsErrored()) {
                int rc = m_socket.Send(pData,
                                   sFrame->GetLength());
                if (rc == -1) {
                    // ignore errors. we do non blocking here
                    // and if packet cannot be sent out (EAGAIN set)
                    // we throw away the packet
                    m_errors++;
                }
            }

            // measure performance if enabled
            if (m_perfInterval > 0) {
                struct SDataHeader DataHeader;
                memcpy(&DataHeader, pData, sizeof(DataHeader));
                if (computePerformaceStats(m_perfData, &DataHeader,
                                           sFrame->GetLength(),
                                           m_perfInterval, 5000)) {
                    // interval reached, save a copy
                    memcpy(m_perfSnapShot, m_perfData,
                           sizeof(struct SPerfData));
                }
            }

        }

        m_qQueueSource->ReleaseFrame(sFrame);

        retval = 1;

    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0) {
            CheckQueueTimestamp();
        }

#ifdef SSDEBUG
        if (m_perfInterval > 0) {
            if (m_nTimestamp > 0 &&
                (GetCurrentTimeStamp() - m_nProgressTimestamp) >= 100) {
                m_nProgressTimestamp = GetCurrentTimeStamp();
                struct SDataHeader data_header;
                char mesgbuf[128];
                data_header.h_nTimestamp = m_nProgressTimestamp;
                data_header.h_oob.h_magic = PERF_MAGIC;
                data_header.h_oob.h_sessionid = m_nSessionId;
                data_header.h_oob.h_type = PERF_TYPE_PROGRESS;
                memcpy(mesgbuf, &data_header, sizeof(struct SDataHeader));
                retval = m_socket.Send(mesgbuf, 128);

            }
        }
#endif

    }

    CheckSyncStop();


    return retval;
}


void
CUdpTx::PrintStatistics(string &a_stats)
{

}


void
CUdpTx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("Host", m_destination.GetHost());
    a_pCmd->SetObjectParam("Port", (int)m_destination.GetPort());
    a_pCmd->SetObjectParam("Multicast", (bool)m_destination.IsMulticast());
    a_pCmd->SetObjectParam("MulticastInterface", m_sInterfaceInfo);

    if (m_perfSnapShot != NULL && m_perfInterval > 0) {

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
        a_pCmd->SetObjectParam("PktRateJitter",
                               (double)m_perfSnapShot->m_packetRateJitter);
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
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("SourceContention",
                               (unsigned long)m_qQueueSource->GetContention());
    }
#endif


}
