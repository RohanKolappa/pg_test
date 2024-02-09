
#include "strmsrv_compatibility.h"

#include "common.hh"
#include "mpegtstx.hh"
#include "wis.hh"  // backward compatibility defines
#include "MulticastUtils.hh"

using namespace std;

CMpegTsTx::CMpegTsTx (int nId,
                      char *pDestination,
                      int nDestinationPort,
                      int nTTL,
                      int nInterface) :
    CSender(nId)
{
    m_nPortType = MPEGTSDECODERPORT;
    m_pMPEGTSMuxFilter = NULL;

    if (m_destination.SetAddress(pDestination, nDestinationPort) == -1) {
        SetError();
        SYSLOG_ERROR("Incorrect Internet Address:Port %s:%d\n",
                     pDestination, nDestinationPort);
    }

    CMulticastUtils::RegisterMulticastSendInterface(m_socket, m_destination,
            nInterface, m_sInterfaceInfo);

    if (m_socket.SetDestination(m_destination) == -1) {
        SetError();
        SYSLOG_ERROR("Network Error: %s\n",
                     m_socket.GetLastError().c_str());
    }

    m_socket.SetMcastTTL(nTTL);
    m_framesServed = 0;
    m_packetsServed = 0;
    m_pTokenBucket = new CShaper(25000000, 2, 5, 1316, 200);
    m_nOffset = 0;
}

CMpegTsTx::~CMpegTsTx()
{
    if(m_pMPEGTSMuxFilter) {
        delete (m_pMPEGTSMuxFilter);
    }
    if(m_pTokenBucket) {
        delete(m_pTokenBucket);
    }
    return;
}

int CMpegTsTx::doSend(char *pData, int nLen)
{
    int retval=0;
    int nSendBytes=0;
    int nOffset = 0;

    while (nOffset < nLen) {
        nSendBytes = nLen - nOffset;
        if (nSendBytes > 1316)
            nSendBytes = 1316;

        retval = m_socket.Send(pData, nSendBytes);

        if (retval == -1)  {
            break;
        }
        nOffset += nSendBytes;
        pData += nSendBytes;
        m_packetsServed++;
    }

    m_framesServed++;

    return retval;
}

int CMpegTsTx::ProcessStream()
{
    int retval = 0;
    char tspktData[ 7 * TS_PACKET_SIZE];
    char *tp;

    if (m_qQueueSource == NULL)
        return 0;

    CSFrame *sFrame = m_qQueueSource->GetFrame();
    if (sFrame != NULL) {
        unsigned long nFrameType;

        // Backward compatibility
        if (sFrame->GetFrameVersion() == FRAMEVERSION_0) {
            // Adjust for recordings earler thatn 2.2 release
            // All older recordings were always MPEGTS
            nFrameType = FRAMETYPE_MPEGTS;
        }
        else {
            nFrameType = sFrame->GetFrameType();
        }

        char *pData = sFrame->GetData();
        int nLen  = sFrame->GetLength();

        if (m_nTimestamp == 0) {
            HandleFirstTimestamp(sFrame->GetTimeStamp());
        }
        m_nTimestamp = sFrame->GetTimeStamp();
        m_nFrameWallClock = CTimeKeeper::Now();

        if (sFrame->GetMessageType() == CSFrame::Normal) {

            // if input frame is RAW - send thru TS mux filter
            if( (nFrameType & FRAMETYPE_ENCAPS_MASK) == FRAMETYPE_RAW ) {
                if(! m_pMPEGTSMuxFilter) {
                    // hack - need to figure out the right way to get frametype and bitrate
                    m_pMPEGTSMuxFilter = new MPEGTSMuxFilter(FRAMETYPE_VIDEO | FRAMETYPE_MP2V,
                            6000000,
                            FRAMETYPE_AUDIO | FRAMETYPE_MPA,
                            192000);
                }
                m_pMPEGTSMuxFilter->pushAVFrame(pData,
                        nLen, nFrameType,
                        m_nTimestamp);

                // Gather as many as 7 TS packets into one udp packets
                tp = tspktData;
                int i = 0;
                while(1) {
                    pData = m_pMPEGTSMuxFilter->popTSPacket();
                    if(pData == NULL || i >= NO_OF_TS_PKTS) {
                        int rc = doSend(tspktData, i * TS_PACKET_SIZE);

                        if (rc == -1) {
                            m_errors++;
                            break;
                        }

                        if(pData == NULL) {
                            break;
                        }
                        else {
                            tp = tspktData;
                            i = 0;
                        }

                    }
                    memcpy(tp,pData,TS_PACKET_SIZE);
                    tp += TS_PACKET_SIZE;
                    delete (pData);
                    i++;
                }
                // No token bucket shaping for raw packets that are
                // muxed to MPEGTS
                m_nOffset = nLen;
            }

            // already muxed stream - send directly
            else {
                while (m_nOffset < nLen) {
                    int nSendLength = nLen - m_nOffset;
                    if (nSendLength > 7*TS_PACKET_SIZE)
                        nSendLength = 7*TS_PACKET_SIZE;
                    if (m_pTokenBucket->CheckTokens(nSendLength,
                            m_qQueueSource->Size()) == false)
                        break;
                    int rc = doSend(pData + m_nOffset, nSendLength);
                    m_nOffset += nSendLength;
                    if (rc == -1) {
                        m_errors++;
                        if (m_errors < 100)
                            SYSLOG_ERROR("Could not send data of size %d\n", nLen);
                    }
                }
            }


            if (m_nOffset >= nLen) {
                m_frameCount++;
                m_bytesTransfered += nLen;
                m_qQueueSource->ReleaseFrame(sFrame);
                m_nOffset = 0;
                retval = 1;
            }

        }
        else {
            m_nOffset = 0;
            m_qQueueSource->ReleaseFrame(sFrame);
        }

    }
    else {
        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0) {
            CheckQueueTimestamp();
        }
    }

    CheckSyncStop();

    return retval;
}

void
CMpegTsTx::UpdateCustomStatus(CCommand *a_pCmd)
{

    a_pCmd->SetObjectParam("FramesServed",
                           (unsigned long)m_framesServed);
    a_pCmd->SetObjectParam("PacketsServed",
                           (unsigned long)m_packetsServed);

    a_pCmd->SetObjectParam("Host", m_destination.GetHost());
    a_pCmd->SetObjectParam("Port", (int)m_destination.GetPort());
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectParam("MaxQueueLen",
                               (unsigned long)m_qQueueSource->MaxLength());
    }
    m_pTokenBucket->UpdateCustomStatus(a_pCmd);
    a_pCmd->SetObjectParam("Multicast", (bool)m_destination.IsMulticast());
    a_pCmd->SetObjectParam("MulticastInterface", m_sInterfaceInfo);

}


