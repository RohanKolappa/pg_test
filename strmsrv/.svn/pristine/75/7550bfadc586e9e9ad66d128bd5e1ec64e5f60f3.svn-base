/*
 * rfb.hh
 *
 *  Created on: Oct 3, 2009
 *      Author: rkale
 */

#ifndef RFBHANDLER_HH_
#define RFBHANDLER_HH_

#ifdef HAVE_V2O
#include "fpgarx.hh"
#endif
#include "squeue.hh"
#include "rfbclient.h"
#include "rfbcallback.hh"
#include "command.hh"

class CRFBHandler {
public:

#ifdef HAVE_V2O
    CRFBHandler(IRFBHandlerCallback * a_pCallbackHandler, HAL *a_hp);
#endif
    CRFBHandler(IRFBHandlerCallback * a_pCallbackHandler,
            char *ipaddress, char *password, int port);
    ~CRFBHandler();

    int Update(CCommand * a_pCmd);
    void RequestReady();
    void ResetReady();
    int ProcessRFBQuery(char *pPayload);
    int ProcessKeyboard(char *pPayload);
    int ProcessMouse(char *pPayload);
    int ProcessVNCKeyboard(unsigned int key, unsigned char down);
    int ProcessVNCMouse(char buttonMask, unsigned short AbsX, unsigned short AbsY);
    int ProcessRFBConnection();
    void UpdateCustomStatus(CCommand *a_pCmd);
    void InitXpiRFBStatus(bool m_bDeviceType);
    bool IsRFBEnabled();
    RFBClient_t * GetClient() {
        return m_pClient;
    }

    void SetMouseXOffset(int a_nOffset) {
        m_nMouseOffsetX = a_nOffset;
    }

    void SetMouseYOffset(int a_nOffset) {
        m_nMouseOffsetY = a_nOffset;
    }

    

private:
    RFBClient_t * m_pClient;
    IRFBHandlerCallback * m_pRFBCallback;

    bool m_bEnabled;
    char m_sHost[32 + 1];
    char m_sPassword[32 + 1];
    int m_nPort;
    bool m_bPasswordAssigned;

    bool m_bReady;
    bool m_bXpiDevice;
    int m_nMouseOffsetX;
    int m_nMouseOffsetY;
    int m_nMouseX;
    int m_nMouseY;
    unsigned long m_nResetCount;
    timestamp_t m_nRFBDisconnctTimestamp;
    unsigned long m_nRFBReconnectTimeout;
    unsigned long m_nKeyboardDropped;
    unsigned long m_nMouseDropped;
    bool m_bScheduleRFB;

    void InitializeVariables();
    int SendRFBReady(uint32_t a_nSessionId = 0,
            uint32_t a_nQueryId = 0);
    void EnableRFB();
    void DisableRFB();
    bool AllowReconnect();
};

#endif /* RFBHANDLER_HH_ */
