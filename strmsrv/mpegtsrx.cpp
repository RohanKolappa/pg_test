#include "strmsrv_compatibility.h"
#include "mpegtsrx.hh"
#include "common.hh"
#include "MulticastUtils.hh"

using namespace std;

const int CMpegTsRx::m_BUFFER_SIZE = 64 * 1024;

CMpegTsRx::CMpegTsRx(int nId, char* sourceIp, char* multicastIp, int destPort,
        intlist & a_interfaceList) :
    CReceiver(nId)
{
    m_nPortType = MPEGTSNETWORKPORT;
    m_nPort = destPort;
    m_sBuffer = NULL;
    m_readOffset = 0;
    m_reallocCount = 0;
    m_nonmulterr = 0;
    m_maxPacketSize = 0;
    m_maxPacketLastSec = 0;
    m_wrongSourceErr = 0;
    strcpy(m_multicastIp, "");
    strcpy(m_sourceIp, "");
    m_bissrcIp = false;

    // If it is multicast
    if (strcmp(multicastIp, "") != 0) {
        strncpy(m_multicastIp, multicastIp, 60);

        if (m_source.SetAddress(m_multicastIp, destPort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Port %d\n", destPort);
            return;
        }
    }
    else {
        if (m_source.SetAddress(destPort) == -1) {
            SetError();
            SYSLOG_ERROR("Incorrect Internet Port %d\n", destPort);
            return;
        }
    }

    if (strcmp(sourceIp, "") != 0) {
        strncpy(m_sourceIp, sourceIp, 60);
        m_srcCheck.SetAddress(m_sourceIp, 0);
        m_bissrcIp = true;
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

    m_socket.SetReceiveBufferSize(65535 * 10);
    SetConnected();
}

CMpegTsRx::~CMpegTsRx()
{

    if (m_sBuffer)
        m_sBuffer->RemoveReference();

}

int CMpegTsRx::ProcessStream()
{

    if (m_qQueueSink == NULL)
        return 0;

    if (!m_socket.IsValid())
        return 0;

    if (IsErrored())
        return 0;

    char *mesgbuf = m_sBuffer->GetBuffer(m_readOffset);
    if (mesgbuf == NULL) { // memory pressure, give up on this object
        SetError();
        m_socket.Close();
        return 0;
    }

    int len = m_socket.Receive(mesgbuf, m_BUFFER_SIZE - m_readOffset);

    // If sourceIp is given check for it
    if (m_bissrcIp) {
        CInternetAddress tmpAddr = m_socket.GetPeerAddress();
        if (!m_srcCheck.EqualsAddress(tmpAddr)) {
            m_wrongSourceErr++;
            return 0;
        }
    }

    if (len > 0) {

        if ((len % 188) != 0)
            m_nonmulterr++;

        if (len > m_maxPacketSize)
            m_maxPacketSize = len;

        if (len > m_maxPacketLastSec)
            m_maxPacketLastSec = len;

        if (m_nTimestamp == 0) {
            m_nStartTimestamp = CTimeKeeper::Now();
            m_nStartWallClock = CTimeKeeper::Now();
            m_bSendNotify = true;
            m_nEventId = NOTIFICATION_EVENT_BEGIN;
        }
        m_nTimestamp = CTimeKeeper::Now();
        m_nFrameWallClock = CTimeKeeper::Now();

        CSFrame *sFrame = new CSFrame(m_sBuffer, m_readOffset, len,
                CSFrame::CreateMediaFlag(FRAMETYPE_MPEGTS, STREAMTYPE_UDP),
                m_nTimestamp);
        m_qQueueSink->InsertFrame(sFrame);
        m_bytesTransfered += len;
        m_frameCount++;
        m_readOffset += len;
    }

    if (m_BUFFER_SIZE - m_readOffset < 2048) {
        m_sBuffer->RemoveReference();
        m_sBuffer = new CSBuffer(m_BUFFER_SIZE);
        m_sBuffer->AddReference();
        m_readOffset = 0;
        m_reallocCount++;
    }
    if (len > 0)
        return 1;

    return 0;

}
void CMpegTsRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("Host", m_socket.GetPeerAddress().GetHost());
    a_pCmd->SetObjectParam("Port", (int) m_source.GetPort());
    a_pCmd->SetObjectParam("MulticastIP", m_multicastIp);
    a_pCmd->SetObjectParam("SourceIP", m_sourceIp);
    a_pCmd->SetObjectParam("SourceErr", (int) m_wrongSourceErr);
    a_pCmd->SetObjectParam("ReallocCount", (int) m_reallocCount);
    a_pCmd->SetObjectParam("MultError", (int) m_nonmulterr);
    a_pCmd->SetObjectParam("MaxPktSize", (int) m_maxPacketSize);
    a_pCmd->SetObjectParam("MaxPktSizeinLastSampleTime",
            (int) m_maxPacketLastSec);
    a_pCmd->SetObjectParam("Multicast", (bool)m_source.IsMulticast());
    a_pCmd->SetObjectParam("MulticastInterfaces", m_sInterfaceInfo);
    m_maxPacketLastSec = 0;
}
