/*
 * vncrx.hh
 *
 *  Created on: Nov 22, 2009
 *      Author: rkale
 */

#ifndef VNCRX_HH_
#define VNCRX_HH_

#include "receiver.hh"
#include "rfbclient.h"
#include "vncencoderthr.hh"
#include "rfbhandler.hh"
#include "rfbcallback.hh"
#include "fbrx.hh"


class CVNCRx : public CFBRx, public IRFBHandlerCallback
{

public:
    CVNCRx(int nId,
           char *serverip,
           int port,
           char *password,
           int compression,
           int codecversion);

    virtual ~CVNCRx();

    void UpdateCustomStatus(CCommand *a_pCmd);

    bool CheckKBMEvent(unsigned long a_sessionId) {return true;};
    void UpdateKBMEvent(unsigned long a_sessionId);
    void UpdateCtlPacketStats(int nLen) {};
    virtual CSQueueSink * RetrieveQueueSink() {
        return GetQueueSink();
    }
    virtual timestamp_t RetrieveTimeStamp() {
        return GetTimeStamp();
    }
    void HandleRFBConnected();
    void HandleRFBDisconnected();
    int HandleRFBReady();

protected:
    virtual int CustomUpdate(CCommand *a_pCmd);
    virtual int ProcessFrameBuffers();
    virtual void ProcessAVRequest(AVReqData_t *);
    virtual void ProcessRFBQuery(char *pPayload);
    virtual void ProcessKeyboard(char *pPayload);
    virtual void ProcessMouse(char *pPayload);
    virtual void RequestReady();

private:
    CRFBHandler * m_pRFB;

    unsigned long m_nRectangleCount;
    unsigned long m_nFBRequestCount;
    unsigned long m_nPartialUpdates;
    timestamp_t m_nFBRequestTimestamp;
    timestamp_t m_tKBMTimeStamp;
    bool m_bReceivedFBUpdate;

    void InitializeVideo();
    void ProcessRectangles();
    int RequestFBUpdates();

};


#endif /* VNCRX_HH_ */
