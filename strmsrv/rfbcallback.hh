/*
 * rfbcallback.hh
 *
 *  Created on: Nov 25, 2009
 *      Author: rkale
 */

#ifndef RFBCALLBACK_HH_
#define RFBCALLBACK_HH_

class IRFBHandlerCallback {
public:
    virtual ~IRFBHandlerCallback() {};
    virtual CSQueueSink * RetrieveQueueSink() = 0;
    virtual timestamp_t RetrieveTimeStamp() = 0;
    virtual void UpdateCtlPacketStats(int nLen) = 0;
    virtual bool CheckKBMEvent(unsigned long a_sessionId) = 0;
    virtual void UpdateKBMEvent(unsigned long a_sessionId) = 0;
    virtual void HandleRFBConnected() = 0;
    virtual void HandleRFBDisconnected() = 0;
    virtual int HandleRFBReady() = 0;
};


#endif /* RFBCALLBACK_HH_ */
