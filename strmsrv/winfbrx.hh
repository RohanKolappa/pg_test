/*
 * winfbrx.hh
 *
 *  Created on: Nov 29, 2010
 *      Author: rkale
 */

#ifndef WINFBRX_HH_
#define WINFBRX_HH_

#include "vncencoderthr.hh"
#include "fbrx.hh"
#include "resolutionchangelistener.hh"

class CWindowsScreenScraper;
class CWindowsEventHandler;
class CRateController;

class CWinFBRx : public CFBRx, IResolutionChangeListener
{

public:
    CWinFBRx(int nId,
            int compression,
            int codecversion);

    ~CWinFBRx();

    void UpdateCustomStatus(CCommand *a_pCmd);

    virtual void OnResolutionChange();

protected:
    virtual int CustomUpdate(CCommand *a_pCmd);
    virtual int ProcessFrameBuffers();
    virtual void ProcessAVRequest(AVReqData_t *);
    virtual void ProcessRFBQuery(char *pPayload);
    virtual void ProcessKeyboard(char *pPayload);
    virtual void ProcessMouse(char *pPayload);
    virtual void RequestReady();
    virtual void HandleDownstreamConnected();
    virtual void HandleDownstreamDisconnected();
    virtual void SetBandwidth();

private:
    CWindowsScreenScraper *m_pScraper;
    CWindowsEventHandler *m_pEventHandler;
    CRateController *m_pGrabRateController;
    int m_nGrabFrameRate;
    int m_nConnectCount;
    bool m_bEnableScraping;
    bool m_bScheduleRFBReady;
    uint64_t m_nGrabCount;
    uint32_t m_nGrabErrors;
    int m_nRequestBandwidth;

    void InitializeVideo();
    void ProcessScreenUpdates();
    int SendRFBReady(uint32_t a_nSessionId = 0, uint32_t a_nQueryId = 0);

};

#endif /* WINFBRX_HH_ */
