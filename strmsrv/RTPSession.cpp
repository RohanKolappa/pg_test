#include "strmsrv_compatibility.h"

#include "common.hh"
#include "RTPSession.hh"
#include "MulticastUtils.hh"

using namespace std;

RTPSubSession::RTPSubSession(int subSessionID, unsigned long frameType,
        char *ip, int port)
{
    _subSessionID = subSessionID;
    _frameType = frameType;
    _ip = strdup(ip);
    _port = port;
    _nExpectedSeqNum = UINT_MAX;

    _frameCount = 0;
    _packetCount = 0;
    _packetLoss = 0;
    _byteCount = 0;
}

/// Set up the UDP socket connection for the subsession
int RTPSubSession::setupConnection(intlist & a_interfaceList)
{
    if (_address.SetAddress(_ip, _port) == -1) {
        SYSLOG_ERROR("Incorrect Address/Port %s:%d\n", _ip, _port);
        return RTP_ERROR;
    }


    CMulticastUtils::RegisterMulticastRecvInterfaces(_socket, _address,
            a_interfaceList, _interfaceInfo);

    if (_socket.SetSource(_address) == -1) {
        SYSLOG_ERROR("Network Error: %s\n",
                _socket.GetLastError().c_str());
        return RTP_ERROR;
    }

    return RTP_OK;
}

RTPSubSession::~RTPSubSession()
{
    free(_ip);
}

RTPSession::RTPSession(intlist & a_interfaceList)
{
    _lastStatus = RTP_OK;
    _interfaceList = a_interfaceList;
}

RTPSession::~RTPSession()
{
    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        delete (*_ssIter);
    }
    _subSessionList.clear();
}

/// Add a audio/video/data subsession to the RTPSession
int RTPSession::addSubSession(int subSessionID, unsigned long frameType,
        char *ip, int port, int nRecvBufSize)
{

    RTPSubSession *ss = new RTPSubSession(subSessionID, frameType, ip, port);
    int status = ss->setupConnection(_interfaceList);
    if (status != RTP_OK) {
        delete ss;
        return status;
    }

    if(nRecvBufSize > 0) {
        ss->_socket.SetReceiveBufferSize(nRecvBufSize);
    }

    _subSessionList.push_back(ss);
    return RTP_OK;
}

/// Receive the packets for all the subSessions
int RTPSession::receivePackets(list<RTPSessionPkt *> &plist)
{

    char buf[RTP_MTU];

    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        RTPSubSession *ss = (*_ssIter);

        int size = ss->_socket.Receive(buf, RTP_MTU);
        if (size <= 0) {
            continue;
        }

        rtp3016_t *rtphdr = (rtp3016_t *) buf;

        if (rtphdr->ph_m)
            ss->_frameCount++;
        ss->_packetCount++;
        ss->_byteCount += size;
//
//        uint32_t nTimestamp = ntohl(rtphdr->ph_ts);
//        if ((nTimestamp/3600) % 25 == 0)
//            printf("Network Receive %"PRIu64" %u\n",
//                    CTimeKeeper::GetTimeInMicroSec(), nTimestamp);

        ss->checkPacketSequence(rtphdr);

        RTPSessionPkt *pkt = new RTPSessionPkt();
        pkt->_subSessionID = ss->_subSessionID;
        pkt->_frameType = ss->_frameType;
        pkt->_size = size;
        pkt->_rtpHeader = (rtp3016_t *) malloc(size);
        memcpy(pkt->_rtpHeader, buf, size);
        plist.push_back(pkt);
    }

    return _lastStatus;
}

uint64_t RTPSession::getPacketCount(int subSessionID)
{
    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        if ((*_ssIter)->_subSessionID == subSessionID)
            return (*_ssIter)->_packetCount;
    }
    return 0;
}

uint64_t RTPSession::getPacketLoss(int subSessionID)
{
    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        if ((*_ssIter)->_subSessionID == subSessionID)
            return (*_ssIter)->_packetLoss;
    }
    return 0;
}

uint64_t RTPSession::getByteCount(int subSessionID)
{
    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        if ((*_ssIter)->_subSessionID == subSessionID)
            return (*_ssIter)->_byteCount;
    }
    return 0;
}


string RTPSession::getPeerAddress(int subSessionID)
{

    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
        if (((*_ssIter)->_subSessionID == subSessionID)
                && ((*_ssIter)->_packetCount > 0))
            return (*_ssIter)->_socket.GetPeerAddress().GetHost();
    }

    return "";
}

bool RTPSession::isMulticast()
{

    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
            return (*_ssIter)->_socket.IsMulticast();
    }

    return false;
}

string RTPSession::getInterfaceInfo()
{

    for (_ssIter = _subSessionList.begin(); _ssIter != _subSessionList.end(); ++_ssIter) {
            return (*_ssIter)->_interfaceInfo;
    }

    return "";
}


