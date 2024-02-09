#ifndef _RTPSESSION_HH
#define _RTPSESSION_HH

#include <list>

#include "rtp.hh"
#include "rtpconst.hh"
#include "socket.hh"
#include "command.hh"


#define RSS_OK 1

class RTPSessionPkt {
public:
    int _subSessionID;
    int _size;
    unsigned long _frameType;
    rtp3016_t *_rtpHeader;
};

class RTPSubSession {
    
private:
    CInternetAddress _address;
    int _port;
    char *_ip;

    unsigned int _nExpectedSeqNum;

public:
    int _subSessionID;
    unsigned long _frameType;
    CSocketDgram _socket;
    uint64_t _frameCount;
    uint64_t _packetCount;
    uint64_t _packetLoss;
    uint64_t _byteCount;
    std::string _interfaceInfo;


    RTPSubSession(int subSessionID, unsigned long frameType, char *ip, int port);
    int setupConnection(intlist & a_interfaceList);
    void checkPacketSequence(const rtp3016_t *a_pRtpHeader);
    ~RTPSubSession();
};

class RTPSession {
    
private:
    
    std::list<RTPSubSession *> _subSessionList;
    std::list<RTPSubSession *> :: iterator _ssIter;
    int _lastStatus;
    intlist _interfaceList;
    
public:
    
    RTPSession(intlist & a_interfaceList);
    ~RTPSession();
    int addSubSession(int subSessionID, unsigned long frameType, char *ip, 
            int port, int nRecvBufSize);
    int deleteSubSession(int subSessionID);
    int receivePackets(std::list<RTPSessionPkt *> &plist);
    int lastStatus() { return _lastStatus; };
    uint64_t getPacketCount(int subSessionID);
    uint64_t getPacketLoss(int subSessionID);
    uint64_t getByteCount(int subSessionID);
    std::string getPeerAddress(int subSessionID);
    std::string getInterfaceInfo();
    bool isMulticast();

};

inline void RTPSubSession::checkPacketSequence(const rtp3016_t *a_pRtpHeader) {

    const uint16_t nSeqNum = ntohs(a_pRtpHeader->ph_seq);

    if(_nExpectedSeqNum == UINT_MAX) {
        _nExpectedSeqNum = nSeqNum + 1;
        return;
    }

    if(nSeqNum != _nExpectedSeqNum)
        _packetLoss++;

    _nExpectedSeqNum = (_nExpectedSeqNum == USHRT_MAX) ? 0 : nSeqNum + 1;
}

#endif
