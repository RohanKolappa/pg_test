/*
 * rfbhandler.cpp
 *
 *  Created on: Oct 3, 2009
 *      Author: rkale
 */
#include "strmsrv_compatibility.h"

#include "common.hh"
#include "rfbhandler.hh"
#include "v2d.h"
#include "v2o_lib.h"

#ifdef HAVE_V2O
CRFBHandler::CRFBHandler(IRFBHandlerCallback * a_pCallbackHandler, HAL *a_hp)
{
    InitializeVariables();

    m_pRFBCallback = a_pCallbackHandler;

    // First get RFB configuration for the current board
    struct tx_rfb_config rfb_config;
    get_tx_rfb_config(a_hp, &rfb_config);

    m_bEnabled = rfb_config.enable;
    strcpy(m_sHost, rfb_config.ipaddress);
    strcpy(m_sPassword, rfb_config.password);
    m_bPasswordAssigned = true;
    m_nPort = rfb_config.port;
    m_nMouseOffsetX = rfb_config.mouse_offset_x;
    m_nMouseOffsetY = rfb_config.mouse_offset_y;


    // Initialize other variables

    // TODO: Consider ports less than 50 to be VNC ports: 5900 + port

    // Enable RFB
    if (m_bEnabled)
        EnableRFB();
}
#endif

CRFBHandler::CRFBHandler(IRFBHandlerCallback * a_pCallbackHandler,
        char *ipaddress, char *password, int port)
{
    InitializeVariables();

    m_pRFBCallback = a_pCallbackHandler;
    m_bEnabled = true;
    strcpy(m_sHost, ipaddress);

    if (password != NULL) {
       strcpy(m_sPassword, password);
       m_bPasswordAssigned = true;
    }
    else {
       strcpy(m_sPassword, "");
       m_bPasswordAssigned = false;
    }
    m_nPort = port;

    // Enable RFB
    EnableRFB();
}

CRFBHandler::~CRFBHandler()
{
    DisableRFB();
}

void CRFBHandler::InitializeVariables()
{
    m_bReady = false;
    m_bScheduleRFB = false;
    m_pClient = NULL;
    m_nKeyboardDropped = 0;
    m_nMouseDropped = 0;
    m_nMouseX = 0;
    m_nMouseY = 0;
    m_nMouseOffsetX = 0;
    m_nMouseOffsetY = 0;
    m_nRFBDisconnctTimestamp = 0;
    m_nRFBReconnectTimeout = 1000;
    m_bPasswordAssigned = false;
    m_nResetCount = 0;
    m_bXpiDevice = false;
}

void CRFBHandler::EnableRFB()
{
    ASSERT(m_bEnabled == true);
    
    if (m_bXpiDevice) {
        if (strcmp(m_sHost, "127.0.0.1") == 0)
            return ; /* Not Processing 127.0.0.1 in XPI*/
    }
    
    if (strlen(m_sHost) > 0) {
        char * password = NULL;
        if (m_bPasswordAssigned)
            password = m_sPassword;
        m_pClient = CreateRFBNetworkClient(m_sHost, m_nPort, password);
    }
    if (m_pClient != NULL)
        RFBStart(m_pClient);

}

void CRFBHandler::DisableRFB()
{
    if (m_pClient != NULL) {
        RFBDestroy(m_pClient);
    }
    m_pClient = NULL;
}

void CRFBHandler::RequestReady()
{
    if (m_bScheduleRFB == true) {
        SendRFBReady();
        m_bScheduleRFB = false;
    }
}

void CRFBHandler::ResetReady()
{

    m_bScheduleRFB = true;

}

int CRFBHandler::SendRFBReady(uint32_t a_nSessionId,
        uint32_t a_nQueryId)
{

    unsigned char buffer[SEND_BUFFER_SIZE];
    int nLen = 0;
        
    SYSLOG_DEBUG("RFBHANDLER: RFB: Sending status message %d to %u:%u",
            m_bReady, a_nSessionId, a_nQueryId);
    nLen = v2dLibCreateRFBStatus(buffer, m_bReady, a_nSessionId, a_nQueryId);
    if (m_pRFBCallback->RetrieveQueueSink() != NULL) {

        CV2DStreamMediaFlag v2dSMediaFlag;
        v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);

        CSFrame * sFrame = new CSFrame((char *) buffer, nLen,
                v2dSMediaFlag.GetMediaFlag(), m_pRFBCallback->RetrieveTimeStamp());

        m_pRFBCallback->RetrieveQueueSink()->InsertFrame(sFrame);

        m_pRFBCallback->UpdateCtlPacketStats(nLen);

    }
    return nLen;
}

void CRFBHandler::InitXpiRFBStatus(bool m_bDeviceType)
{
 
    m_bXpiDevice = m_bDeviceType;
    if (m_bXpiDevice) {
        if (!m_bEnabled)
            m_bReady = true;
        
        if (strcmp(m_sHost, "127.0.0.1") == 0)
            DisableRFB();
    }
}

bool CRFBHandler::IsRFBEnabled()
{
    return m_bEnabled;
}

// Handle Update of RFB parameters
int CRFBHandler::Update(CCommand * a_pCmd)
{
    int enableRFB;
    char sRFBip[60];
    char sRFBpasswd[60];
    int nRFBport;
    int nOffset;
    bool restartRFB = false;
    bool updatePassword = false;
    int retval = 0;

    SYSLOG_INFO("Received update");

    if (a_pCmd->GetVar("rfbip", sRFBip, 60) != -1) {
        if (strcmp(m_sHost, sRFBip) != 0) {
            strcpy(m_sHost, sRFBip);
            restartRFB = true;
        }
        retval = 1;
    }

    if (a_pCmd->GetVar("rfbpasswd", sRFBpasswd, 60) != -1) {
        if (m_bPasswordAssigned == false) {
            strcpy(m_sPassword, sRFBpasswd);
            m_bPasswordAssigned = true;
            updatePassword = true;
        }
        else if (strcmp(m_sPassword, sRFBpasswd) != 0) {
            strcpy(m_sPassword, sRFBpasswd);
            updatePassword = true;
        }
        retval = 1;
    }

    if (a_pCmd->GetIntVar("rfbport", &nRFBport) != -1) {
        if (m_nPort != nRFBport) {
            m_nPort = nRFBport;
            restartRFB = true;
        }
        retval = 1;
    }

    if (a_pCmd->GetIntVar("rfbenable", &enableRFB) != -1) {
        if (enableRFB) {
            m_bEnabled = true;
        }
        else {
            m_bEnabled = false;
        }
        retval = 1;
    }

    if (a_pCmd->GetIntVar("rfboffsetx", &nOffset) != -1) {
        if (m_nMouseOffsetX != nOffset) {
            m_nMouseOffsetX = nOffset;
            // no need to restart RFB
        }
        retval = 1;
    }

    if (a_pCmd->GetIntVar("rfboffsety", &nOffset) != -1) {
        if (m_nMouseOffsetY != nOffset) {
            m_nMouseOffsetY = nOffset;
            // no need to restart RFB
        }
        retval = 1;
    }

    // If authentication failed and new password provided, restart the RFB
    if (m_pClient != NULL && updatePassword == true && restartRFB == false) {
        if (m_pClient->m_errorcode == RFB_CLIENT_RC_AUTH_FAILED)
            restartRFB = true;
    }


    if (m_pClient != NULL && updatePassword == true && restartRFB == false) {
        // Waiting for password
        RFBSetPassword(m_pClient, m_sPassword);
    }
    else if (m_bEnabled == false) {
        if (m_pClient != NULL) {
            DisableRFB();
            ASSERT(m_pClient == NULL);
        }
        if (m_bXpiDevice) {
            if (m_bReady == false) {
                m_bReady = true;
                SendRFBReady();
            }
        } else {
            if (m_bReady == true) {
                m_bReady = false;
                SendRFBReady();
            }
        }
    }
    else {
        if (restartRFB) {
            DisableRFB();
            EnableRFB();
        }
        else if (m_pClient == NULL) {
            EnableRFB();
        }
        if (m_bXpiDevice) {
            if (m_bReady == true) {
                m_bReady = false;
                SendRFBReady();
            }
        }
    }
    return retval;
}

int CRFBHandler::ProcessKeyboard(char *pPayload)
{
    uint32_t key = 0;
    uint8_t down = 0;
    uint32_t sessionId = 0;

    if (m_pClient == NULL || m_bReady == false)
        return -1;

    // Parse the payload
    int rc = v2dLibParseAbsoluteKeyboard((uint8_t *)pPayload, &key, &down, &sessionId);

    /* Send the keyboard event if its from the current client or the idle
     time is passed */
    if (rc > 0 && m_pRFBCallback->CheckKBMEvent(sessionId)) {
        m_pRFBCallback->UpdateKBMEvent(sessionId);
        if (SendKeyboardEvent(m_pClient, key, down) != RFB_CLIENT_RC_SUCCESS) {
            SYSLOG_ERROR("RFBHANDLER: RFB Could not send keyboard event: %d\n",
                    RFBErrorCode(m_pClient));
            m_nKeyboardDropped++;
        }
        RFB_Go(m_pClient);
    }
    else {
        m_nKeyboardDropped++;
    }

    return 0;
}

int CRFBHandler::ProcessMouse(char *pPayload)
{
    uint16_t x = 0, y = 0;
    uint8_t buttonmask = 0;
    uint32_t sessionId = 0;

    if (m_pClient == NULL || m_bReady == false)
        return -1;

    // Parse the payload
    int rc = v2dLibParseAbsoluteMouse((uint8_t *)pPayload, &x, &y, &buttonmask, &sessionId);

     /* Send the mouse event if its from the current client or the idle
     time is passed */
    if (rc > 0 && m_pRFBCallback->CheckKBMEvent(sessionId)) {
        m_pRFBCallback->UpdateKBMEvent(sessionId);

        m_nMouseX = x + m_nMouseOffsetX;
        m_nMouseY = y + m_nMouseOffsetY;

        if (SendMouseEvent(m_pClient, m_nMouseX, m_nMouseY, buttonmask)
                != RFB_CLIENT_RC_SUCCESS) {
            SYSLOG_ERROR("RFBHANDLER: RFB Could not send mouse event: %d\n",
                    RFBErrorCode(m_pClient));
            m_nMouseDropped++;
        }
        RFB_Go(m_pClient);
    }
    else {
        m_nMouseDropped++;
    }

    return 0;
}

int CRFBHandler::ProcessVNCKeyboard(unsigned int key, unsigned char down) 
{

    if (m_pClient == NULL || m_bReady == false)
        return  -1;
    
    if (SendKeyboardEvent(m_pClient, key, down) != RFB_CLIENT_RC_SUCCESS) {
        SYSLOG_ERROR("RFBHANDLER:RFB Could not send VNC keyboard event: %d\n", 
                     RFBErrorCode(m_pClient));
        m_nKeyboardDropped++;
    }
    
    RFB_Go(m_pClient);
    
    return 0;
}

int CRFBHandler::ProcessVNCMouse(char buttonMask, 
                                 unsigned short AbsX,
                                 unsigned short AbsY)
{

    if (m_pClient == NULL || m_bReady == false)
        return -1;
    
    if (SendMouseEvent(m_pClient, (AbsX + m_nMouseOffsetX), (AbsY + m_nMouseOffsetY), buttonMask) != RFB_CLIENT_RC_SUCCESS) {
        SYSLOG_ERROR("RFBHANDLER: RFB Could not send VNC mouse event: %d\n",                         RFBErrorCode(m_pClient));
        m_nMouseDropped++;
    }
    
    RFB_Go(m_pClient);
    return 0;
}

// Respond to a RFB Status Query message
int CRFBHandler::ProcessRFBQuery(char *pPayload)
{
    uint32_t sessionId = 0;
    uint32_t queryId = 0;

    int rc = v2dLibParseRFBStatusQuery((uint8_t *)pPayload, &sessionId, &queryId);

    if (rc != -1 && sessionId != 0) {
        SYSLOG_DEBUG("RFBHANDLER: RFB: Received query from %u:%u",
                sessionId, queryId);
        SendRFBReady(sessionId, queryId);
    }
    return 0;
}

int CRFBHandler::ProcessRFBConnection()
{
    int retval = 0;

    if (m_pClient == NULL)
        return 0;

    switch (RFBState(m_pClient)) {

    case RFB_CLIENT_STATE_READY:

        // Did we just become ready??
        if (m_bReady == false) {
            SYSLOG_INFO("RFBHANDLER: RFB Ready!\n");
            m_bReady = true;
            SendRFBReady(); // broadcast RFB status changed
            m_pRFBCallback->HandleRFBConnected();
            m_nRFBReconnectTimeout = 1000;
        }

        // Move the protocol along
        if (RFB_Go(m_pClient) == 1)
            retval = 1;

        // Callback on our creator to let it do what
        // it needs to during normal RFB processing
        if (m_pRFBCallback->HandleRFBReady() == 1)
            retval = 1;

        break;

    case RFB_CLIENT_STATE_ERROR:
        // Did we just get disconnected?
        if (m_bReady == true) {
            SYSLOG_DEBUG("RFBHANDLER: RFB Error: %d\n", RFBErrorCode(m_pClient));
            m_bReady = false;
            SendRFBReady(); // broadcast RFB status changed
            m_pRFBCallback->HandleRFBDisconnected();
            m_nRFBDisconnctTimestamp = 0;
        }


        // Wait upto a second before attempting to try RFB connection again
        if (m_nRFBDisconnctTimestamp == 0) {
            m_nRFBDisconnctTimestamp = CTimeKeeper::Now();
        }
        else if (!AllowReconnect()) {
            // Do not attempt reconnection for certain errors

        }
        else if ((CTimeKeeper::Now() - m_nRFBDisconnctTimestamp)  > m_nRFBReconnectTimeout) {
            SYSLOG_DEBUG("RFBHANDLER: RFB Retrying.\n");
            m_nRFBDisconnctTimestamp = 0;
            m_nResetCount++;
            RFBReset(m_pClient);
            RFBStart(m_pClient);
        }

        break;

    default:
        // All other cases we need to push the protocol along
        RFB_Go(m_pClient);

        break;
    }

    return retval;
}

// Check client error codes to see if there is any reason
// not to try to reconnect
bool CRFBHandler::AllowReconnect()
{
    // No point reconnecting if we have a bad password
//    if (m_pClient->m_errorcode == RFB_CLIENT_RC_AUTH_FAILED)
//        return false;

    if (m_pClient->m_errorcode == RFB_CLIENT_RC_AUTH_FAILED)
        m_nRFBReconnectTimeout = 10000;

    return true;
}

void CRFBHandler::UpdateCustomStatus(CCommand *a_pCmd)
{
    // RFB stats
    a_pCmd->SetObjectParam("RFBEnabled", (bool) m_bEnabled);
    if (m_pClient != NULL) {

        a_pCmd->SetObjectParam("RFBReady", (bool) m_bReady);
        a_pCmd->SetObjectParam("RFBServer", (char *) m_pClient->m_server);
        a_pCmd->SetObjectParam("RFBPort", m_pClient->m_port);
        a_pCmd->SetObjectParam("RFBConnectionStatus",
                (char *) RFBErrorString(m_pClient));
        a_pCmd->SetObjectCounterParam("RFBReset", m_nResetCount);
#ifdef SSDEBUG
        a_pCmd->SetObjectParam("RFBPassword",
                m_pClient->m_password);
#endif
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectParam("RFBPasswordAssigned", (bool) m_bPasswordAssigned);
        }
        a_pCmd->SetObjectParam("RFBDesktop", (char *) m_pClient->m_desktopName);

        a_pCmd->SetObjectConstantParam("RFBWidth", m_pClient->m_frameWidth);
        a_pCmd->SetObjectConstantParam("RFBHeight", m_pClient->m_frameHeight);

        a_pCmd->SetObjectCounterParam("RFBMouseSent", m_pClient->m_mouseSent);
        a_pCmd->SetObjectCounterParam("RFBKeyboardSent",
                m_pClient->m_keyboardSent);
        a_pCmd->SetObjectErrorCounterParam("RFBKeyboardDropped",
                m_nKeyboardDropped);
        a_pCmd->SetObjectErrorCounterParam("RFBMouseDropped", m_nMouseDropped);
        a_pCmd->SetObjectParam("RFBMouseOffsetX", m_nMouseOffsetX);
        a_pCmd->SetObjectParam("RFBMouseOffsetY", m_nMouseOffsetY);
        if (a_pCmd->StatusLevel(1)) {
            a_pCmd->SetObjectParam("RFBMouseCurrentX", m_nMouseX);
            a_pCmd->SetObjectParam("RFBMouseCurrentY", m_nMouseY);
        }
        a_pCmd->SetObjectCounterParam("RFBPacketsRead",
                m_pClient->m_packetsRead);
        a_pCmd->SetObjectByteCounterParam("RFBBytesRead",
                m_pClient->m_bytesRead);
        a_pCmd->SetObjectByteCounterParam("RFBBytesWritten",
                m_pClient->m_bytesWritten);
        if (a_pCmd->StatusLevel(4)) {
            a_pCmd->SetObjectByterateGuageParam("RFBMaxWriteBuffer",
                    m_pClient->m_maxBytesToWrite);
        }

    }
    else {
        a_pCmd->SetObjectParam("RFBServer", (char *) m_sHost);
        a_pCmd->SetObjectParam("RFBPort", m_nPort);
        a_pCmd->SetObjectParam("RFBConnectionStatus", (char *) "Disabled");
    }
}
