

#include "portability.h"
#include "rfbproto.h"
#include "rfbclient.h"
#include "sockets.h"
#include "vncauth.h"
#include "rfbio.h"


// These state are sub-states of initialization state
#define PROTO_STATE_RESET               0
#define PROTO_STATE_GET_VERSION         101
#define PROTO_STATE_SEND_VERSION        102
#define PROTO_STATE_GET_SECURITY_TYPE   103
#define PROTO_STATE_AUTHENTICATE        104
#define PROTO_STATE_CHECK_PASSWORD      105
#define PROTO_STATE_SEND_PASSWORD       106
#define PROTO_STATE_GET_AUTH_RESPONSE   107
#define PROTO_STATE_SEND_CLIENT_INIT    108
#define PROTO_STATE_GET_SERVER_INIT     109
#define PROTO_STATE_GET_DESKTOPNAME     110
#define PROTO_STATE_DONE                111

// These states are sub-states of ready state
#define PROTO_STATE_IDLE                    201
#define PROTO_STATE_HANDLE_SERVER_MESSAGE   202
#define PROTO_STATE_GET_FRAMEBUF_UPDATE     203
#define PROTO_STATE_GET_RECTANGLE           204
#define PROTO_STATE_GET_RECT_DATA           205
#define PROTO_STATE_GET_SERVER_CUT_TEXTMSG  206
#define PROTO_STATE_GET_SERVER_CUT_TEXT     207
#define PROTO_STATE_GET_BELLMSG             208
#define PROTO_STATE_ABORT                   210

#define IOERROR(rc) ((rc) != RFB_CLIENT_RC_WAIT && \
                    (rc) != RFB_CLIENT_RC_PARTIAL_IO && \
                    (rc) != RFB_CLIENT_RC_SUCCESS)
/*
 * This performs the initial RFB protocol handshake and takes the
 * client from the "CONNECTED" state to the "READY" state (or
 * possibly to "ERROR" state)
 */
static int 
CheckRFBInit(RFBClient_t *client)
{
    int rc = RFB_CLIENT_RC_SUCCESS;
    int server_major, server_minor;
    rfbClientInitMsg ci;
    rfbServerInitMsg si;
    RFBASSERT(client != NULL);
    RFBASSERT(client->m_state == RFB_CLIENT_STATE_CONNECTED);

    int previous_substate = client->m_substate;

    switch (client->m_substate) {

    case PROTO_STATE_RESET:
        client->m_substate = PROTO_STATE_GET_VERSION;
        client->m_errorcode = RFB_CLIENT_RC_INITIALIZING;
        break;

    case PROTO_STATE_GET_VERSION:
        
        rc = RFBReadFromServer(client, sz_rfbProtocolVersionMsg);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            client->m_readArea[sz_rfbProtocolVersionMsg] = 0;
            
            if (sscanf((char *)client->m_readArea, rfbProtocolVersionFormat,
                       &server_major, &server_minor) != 2) {
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_PROTOCOL_ERROR;
                break;
            }
            
            client->m_substate = PROTO_STATE_SEND_VERSION;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_SEND_VERSION:

        if (client->m_bytesToWrite == 0) {
            /* We will use Protocol 3.3 always */
            unsigned char buffer[16];
            sprintf((char *)buffer, rfbProtocolVersionFormat, 
                    3, 3);
            rc = RFBWriteToServer(client, buffer, sz_rfbProtocolVersionMsg);
        }
        else {
            rc = RFBWriteToServer(client, NULL, 0);
        }
        if (rc == RFB_CLIENT_RC_SUCCESS) {

            client->m_substate = PROTO_STATE_GET_SECURITY_TYPE;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_SECURITY_TYPE:
        rc = RFBReadFromServer(client, sizeof(CARD32));
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            CARD32 secType;
            secType = *(CARD32 *)(client->m_readArea);
            secType = ntohl(secType);
            
            if (secType == rfbSecTypeInvalid) {
                //ReadConnFailedReason(client);
                // todo handle ReadConnFailedReason protocol
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_AUTH_FAILED;
                break;
            }
            
            if (secType != rfbSecTypeNone && secType != rfbSecTypeVncAuth) {
                LOG_RFB("RFB: Unknown security type from RFB server: %d\n",
                        (int)secType);
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_AUTH_FAILED;
                break;
            }
            LOG_RFB("RFB: Got secType: %d\n", (int)secType);
            client->m_secType = secType;
            client->m_substate = PROTO_STATE_AUTHENTICATE;
            client->m_errorcode = RFB_CLIENT_RC_AUTHORIZING;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_AUTHENTICATE:
        if (client->m_secType == rfbSecTypeNone) {
            // No password needed. Proceed to init state
            client->m_substate = PROTO_STATE_SEND_CLIENT_INIT;
        }
        else if (client->m_secType == rfbSecTypeVncAuth) {
            rc = RFBReadFromServer(client, CHALLENGESIZE);
            if (rc == RFB_CLIENT_RC_SUCCESS) {
                // we keep the challenge in the read area and go to
                // next state
                client->m_substate = PROTO_STATE_CHECK_PASSWORD;
            }            
            else if (IOERROR(rc)) {
                // some error occured
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = rc;
            }
            
        }
        else {
            LOG_RFB("RFB: Unsupported Protocol Type\n");
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = RFB_CLIENT_RC_PROTOCOL_ERROR;
        }
        break;

    case PROTO_STATE_CHECK_PASSWORD:
        // Check if we have a password available
        if (client->m_passwordSet) {
            client->m_substate = PROTO_STATE_SEND_PASSWORD;
        }
        else {
            // Else stay in this state till password is set
            // Inform the main state machine about our wait
            client->m_state = RFB_CLIENT_STATE_ASK_PASSWORD;
            client->m_errorcode = RFB_CLIENT_RC_WAITING_FOR_PASSWORD;
        }
        break;

    case PROTO_STATE_SEND_PASSWORD:

        if (client->m_bytesToWrite == 0) {
            CARD8 challenge[CHALLENGESIZE];
            memcpy(challenge, client->m_readArea, CHALLENGESIZE);

            if (strlen(client->m_password) > 8) {
                client->m_password[8] = '\0';
            }

            vncEncryptBytes(challenge, client->m_password);

            rc = RFBWriteToServer(client, challenge, CHALLENGESIZE);
        }
        else {
            rc = RFBWriteToServer(client, NULL, 0);
        }
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            client->m_substate = PROTO_STATE_GET_AUTH_RESPONSE;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_AUTH_RESPONSE:

        rc = RFBReadFromServer(client, sizeof(CARD32));
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            CARD32 authResult;
            authResult = *(CARD32 *)(client->m_readArea);
            authResult = ntohl(authResult);

            switch (authResult) {
            case rfbAuthOK:
                LOG_RFB("RFB: Authentication successful\n");
                client->m_substate = PROTO_STATE_SEND_CLIENT_INIT;
                break;
            case rfbAuthFailed:
            case rfbAuthTooMany:
            default:
                LOG_RFB("RFB: Authentication Failed\n");
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_AUTH_FAILED;
                break;
            }

        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }

        break;

    case PROTO_STATE_SEND_CLIENT_INIT:

        if (client->m_bytesToWrite == 0) {
            ci.shared = 1; /* We always want to "share" the desktop */
            rc = RFBWriteToServer(client, (unsigned char *)&ci, sz_rfbClientInitMsg);
        }
        else {
            rc = RFBWriteToServer(client, NULL, 0);
        }
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            client->m_substate = PROTO_STATE_GET_SERVER_INIT;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_SERVER_INIT:
        rc = RFBReadFromServer(client, sz_rfbServerInitMsg);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            memcpy(&si, client->m_readArea, sz_rfbServerInitMsg);
            si.framebufferWidth = ntohs(si.framebufferWidth);
            si.framebufferHeight = ntohs(si.framebufferHeight);
            si.format.redMax = ntohs(si.format.redMax);
            si.format.greenMax = ntohs(si.format.greenMax);
            si.format.blueMax = ntohs(si.format.blueMax);
            si.nameLength = ntohl(si.nameLength);

            client->m_frameWidth = si.framebufferWidth;
            client->m_frameHeight = si.framebufferHeight;

            if (si.nameLength > 1024) {
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
                break;
            }
            if (client->m_desktopName != NULL)
                free (client->m_desktopName);
            client->m_desktopName = (char *) malloc(si.nameLength + 1);
            if (client->m_desktopName == NULL) {
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
                break;
            }
            client->m_desktopNameLength = si.nameLength;
            client->m_substate = PROTO_STATE_GET_DESKTOPNAME;

            memcpy(&client->m_pixelFormat, &si.format, sizeof(rfbPixelFormat));

            LOG_RFB("RFB: bpp = %d\n", client->m_pixelFormat.bitsPerPixel);
            LOG_RFB("RFB: depth = %d\n", client->m_pixelFormat.depth);
            LOG_RFB("RFB: endian = %d\n", client->m_pixelFormat.bigEndian);
            LOG_RFB("RFB: truecolor = %d\n", client->m_pixelFormat.trueColour);
            LOG_RFB("RFB: redmax = %d\n", client->m_pixelFormat.redMax);
            LOG_RFB("RFB: redshift = %d\n", client->m_pixelFormat.redShift);
            LOG_RFB("RFB: greenmax = %d\n", client->m_pixelFormat.greenMax);
            LOG_RFB("RFB: greenshift = %d\n", client->m_pixelFormat.greenShift);
            LOG_RFB("RFB: bluemax = %d\n", client->m_pixelFormat.blueMax);
            LOG_RFB("RFB: blueshift = %d\n", client->m_pixelFormat.blueShift);

        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }

        break;

    case PROTO_STATE_GET_DESKTOPNAME:
        rc = RFBReadFromServer(client, client->m_desktopNameLength);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            memcpy(client->m_desktopName, client->m_readArea,
                   client->m_desktopNameLength);
            client->m_desktopName[client->m_desktopNameLength] = '\0';
            LOG_RFB("RFB: Desktop: %s\n", client->m_desktopName);
            client->m_substate = PROTO_STATE_DONE;
        }
        else if (IOERROR(rc)) {
            // some error occured
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_DONE:

        client->m_state = RFB_CLIENT_STATE_READY;
        client->m_substate = PROTO_STATE_IDLE;
        client->m_errorcode = RFB_CLIENT_RC_READY;
        break;

    default:
        RFBASSERT(0);

    }

    // If state changed we return true
    if (client->m_substate != previous_substate) {
        LOG_RFB("RFB: %ld %d -> %d\n",
                client->m_gocount, previous_substate, client->m_substate);
        return 1;
    }

    // Also for any partial IO activity we return true
    if (rc == RFB_CLIENT_RC_PARTIAL_IO)
        return 1;

    return 0;
}

static RFBRect_t * CreateRectangle(int x, int y, int width, int height, int count) {
    RFBRect_t * pRect;

    pRect = malloc(sizeof(RFBRect_t));
    if (pRect == NULL)
        return NULL;

    pRect->m_XPos = x;
    pRect->m_YPos = y;
    pRect->m_Width = width;
    pRect->m_Height = height;
    pRect->m_Encoding = 0;
    pRect->m_NumberOfRectangles = count;
    pRect->m_RectangleIndex = 0;
    pRect->m_Length = width * height * 4;
    pRect->m_Data = (unsigned char *) malloc(pRect->m_Length);


    if (pRect->m_Data == NULL) {
        free(pRect);
        return NULL;
    }

    return pRect;
}

// This allows clients to create a rectangle without any context
RFBRect_t * RFBCreateRectangle(int x, int y, int width, int height, int count, int index) {
    RFBRect_t * rectangle = CreateRectangle(x, y, width, height, count);
    if (index < 0)
        rectangle->m_RectangleIndex = 0;
    else if (index < count)
        rectangle->m_RectangleIndex = index;
    else
        rectangle->m_RectangleIndex = count - 1;
    return rectangle;
}

void RFBDestroyRectangle(RFBRect_t * pRect)
{
    if (pRect != NULL) {
        if (pRect->m_Data != NULL)
            free(pRect->m_Data);
        free(pRect);
    }

}

RFBRect_t * RFBGetRectangle(RFBClient_t * client)
{
    RFBRect_t *rect;

    if (client->m_pOuputRect == NULL)
        return NULL;
    rect = client->m_pOuputRect;
    client->m_pOuputRect = NULL;
    return rect;
}

/*
 * This performs the RFB protocol during READY state.
 * It performs state transitions needed while reading data
 * from the server
 */
static int CheckRFBReady(RFBClient_t * client)
{
    int type;
    int rc = RFB_CLIENT_RC_SUCCESS;
    rfbFramebufferUpdateMsg fbupdate;
    rfbFramebufferUpdateRectHeader rect;
    rfbServerCutTextMsg servercuttext;

    RFBASSERT(client != NULL);
    RFBASSERT(client->m_state == RFB_CLIENT_STATE_READY);

    int previous_substate = client->m_substate;
    //LOG_RFB("RFB: CheckRFBReady %d \n", client->m_substate);

    switch(client->m_substate) {

    case PROTO_STATE_IDLE:
        // Check network activity
        rc = FlushIO(client);

        // Check for error
        if (IOERROR(rc)) {
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = rc;
            break;
        }

        // Did we read some packet?
        if (client->m_buffered > 0) {
            client->m_substate = PROTO_STATE_HANDLE_SERVER_MESSAGE;
        }
        break;

    case PROTO_STATE_HANDLE_SERVER_MESSAGE:
        // Read the first byte to find message type
        type = *(client->m_readBuf);
        if (type == rfbFramebufferUpdate) {
            LOG_RFB("RFB: Got frame update packet\n");
            client->m_substate = PROTO_STATE_GET_FRAMEBUF_UPDATE;
        }
        else if (type == rfbServerCutText) {
            LOG_RFB("RFB: Got Server cut text\n");
            client->m_substate = PROTO_STATE_GET_SERVER_CUT_TEXTMSG;
        }
        else if (type == rfbBell) {
            LOG_RFB("RFB: Got Server cut text\n");
            client->m_substate = PROTO_STATE_GET_BELLMSG;
        }
        else {
            LOG_RFB("Not handling server message of type %d yet.\n", type);
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = RFB_CLIENT_RC_PROTOCOL_ERROR;
        }
        break;

    case PROTO_STATE_GET_FRAMEBUF_UPDATE:
        rc = RFBReadFromServer(client, sz_rfbFramebufferUpdateMsg);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            memcpy(&fbupdate, client->m_readArea, sz_rfbFramebufferUpdateMsg);
            fbupdate.nRects = ntohs(fbupdate.nRects);
            LOG_RFB("RFB: fbupdate %d rects\n", fbupdate.nRects);
            client->m_nRectangleCount = fbupdate.nRects;
            client->m_nRemainingRectangles = fbupdate.nRects;
            client->m_substate = PROTO_STATE_GET_RECTANGLE;
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_SERVER_CUT_TEXTMSG:
        rc = RFBReadFromServer(client, sz_rfbServerCutTextMsg);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            memcpy(&servercuttext, client->m_readArea, sz_rfbServerCutTextMsg);
            servercuttext.length = ntohl(servercuttext.length);
            LOG_RFB("RFB: servercuttext size = %lu\n", servercuttext.length);
            client->m_serverCutTextLength = servercuttext.length;
            // Go read the cut text string
            client->m_substate = PROTO_STATE_GET_SERVER_CUT_TEXT;
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_BELLMSG:
        rc = RFBReadFromServer(client, sz_rfbBellMsg);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            client->m_substate = PROTO_STATE_IDLE;
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_SERVER_CUT_TEXT:
        if (client->m_cutTextBuffer != NULL)
            free(client->m_cutTextBuffer);
        client->m_cutTextBuffer = (unsigned char *) malloc(client->m_serverCutTextLength + 1);
        if (client->m_cutTextBuffer == NULL) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
            break;
        }
        memset(client->m_cutTextBuffer, 0x00, client->m_serverCutTextLength + 1);
        RFBSetReadArea(client, client->m_cutTextBuffer, client->m_serverCutTextLength);
        rc = RFBReadFromServer(client, client->m_serverCutTextLength);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            LOG_RFB("RFB: Cut Text Buffer = %s\n", (char *)client->m_cutTextBuffer);
            RFBResetReadArea(client);
            client->m_substate = PROTO_STATE_IDLE;
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_RECTANGLE:
        rc = RFBReadFromServer(client, sz_rfbFramebufferUpdateRectHeader);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            memcpy(&rect, client->m_readArea, sz_rfbFramebufferUpdateRectHeader);
            rect.r.x = ntohs(rect.r.x);
            rect.r.y = ntohs(rect.r.y);
            rect.r.w = ntohs(rect.r.w);
            rect.r.h = ntohs(rect.r.h);
            rect.encoding = ntohl(rect.encoding);
            LOG_RFB("RFB: rect encoding %ld, x=%d, y=%d, w=%d, h=%d\n",
                    rect.encoding,
                    rect.r.x, rect.r.y, rect.r.w, rect.r.h);

            if (rect.encoding != rfbEncodingRaw) {
                LOG_RFB("Unexpected encoding.\n");
                client->m_substate = PROTO_STATE_ABORT;
                client->m_errorcode = RFB_CLIENT_RC_PROTOCOL_ERROR;
            }
            else {
                // Allocate rect to receive data
                client->m_pInputRect = CreateRectangle(rect.r.x, rect.r.y,
                        rect.r.w, rect.r.h, client->m_nRectangleCount);
                client->m_substate = PROTO_STATE_GET_RECT_DATA;
            }
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_GET_RECT_DATA:
        RFBSetReadArea(client, client->m_pInputRect->m_Data,
                client->m_pInputRect->m_Length);
        rc = RFBReadFromServer(client, client->m_pInputRect->m_Length);
        if (rc == RFB_CLIENT_RC_SUCCESS) {
            LOG_RFB("Read Rectangle of length %d\n",
                    client->m_pInputRect->m_Length);
            RFBResetReadArea(client);
            // New rectangle ready for delivery
            if (client->m_pOuputRect != NULL) {
                RFBDestroyRectangle(client->m_pOuputRect);
            }
            client->m_pOuputRect = client->m_pInputRect;
            client->m_pOuputRect->m_RectangleIndex =
                    client->m_nRectangleCount - client->m_nRemainingRectangles;

            // Any more rectangles to process??
            if (--client->m_nRemainingRectangles > 0) {
                client->m_substate = PROTO_STATE_GET_RECTANGLE;
            }
            else {
                client->m_substate = PROTO_STATE_IDLE;
            }
        }
        else if (IOERROR(rc)) {
            client->m_substate = PROTO_STATE_ABORT;
            client->m_errorcode = rc;
        }
        break;

    case PROTO_STATE_ABORT:
        client->m_state = RFB_CLIENT_STATE_ERROR;
        client->m_substate = PROTO_STATE_IDLE;
        break;

    default:
        RFBASSERT(0);
    }

    // If state changed return true
    if (client->m_substate != previous_substate) {
        LOG_RFB("RFB: %ld %d -> %d\n",
                client->m_gocount, previous_substate, client->m_substate);
        return 1;
    }

    // Also for any partial IO activity we return true
    if (rc == RFB_CLIENT_RC_PARTIAL_IO)
        return 1;

    return 0;
}


// Common init
static void RFBCommonInit(RFBClient_t *client)
{
    client->m_gocount = 0;
    client->m_frameWidth = 0;
    client->m_frameHeight = 0;
    if (client->m_desktopName != NULL)
        free(client->m_desktopName);
    client->m_desktopName = strdup("");
    if (client->m_cutTextBuffer != NULL)
        free(client->m_cutTextBuffer);
    client->m_cutTextBuffer = NULL;
    client->m_serverCutTextLength = 0;
    client->m_state = RFB_CLIENT_STATE_RESET;
    client->m_substate = PROTO_STATE_RESET;
    client->m_errorcode = RFB_CLIENT_RC_IDLE;
    client->m_secType = 0;
    client->m_mouseSent = 0;
    client->m_keyboardSent = 0;

    RFBResetIO(client);
    RFBResetReadArea(client);
}

/*
 * Create a "Network" RFB object (uses the usual TCP VNC protocol as
 * opposed to serial etc.)
 */
RFBClient_t * CreateRFBNetworkClient(const char *host, int port, const char *password) {

    // Verify input
    if (host == NULL)
        return NULL;
    if (port < 0)
        return NULL;

    // Small numbered ports map to port 5900 + port
    if (port < 100) {
        port += 5900;
    }

    // Allocate memory
    RFBClient_t * client = (RFBClient_t * ) malloc(sizeof(RFBClient_t));
    if (client == NULL)
        return NULL;

    // Initialize variables
    memset(client, 0x00, sizeof(RFBClient_t));
    client->m_server = strdup(host);
    if (client->m_server == NULL) {
        free(client);
        return NULL;
    }
    client->m_port = port;
    client->m_fd = -1;
    client->m_connectionType = RFB_CLIENT_CT_TCP;

    if (password == NULL) {
        client->m_password = strdup("");
        client->m_passwordSet = 0;
    }
    else {
        client->m_password = strdup(password);
        client->m_passwordSet = 1;
    }

    if (client->m_password == NULL) {
        free(client);
        return NULL;
    }

    RFBCommonInit(client);

    LOG_RFB("RFB: Created\n");

    return client;
}

void RFBSetPassword(RFBClient_t *client, const char * password)
{
    if (password == NULL)
        return;

    if (client->m_password != NULL)
        free(client->m_password);

    client->m_password = strdup(password);
    client->m_passwordSet = 1;

}

/*
 * Brings the client back to intial "created" state ready to make a new
 * connection
 */
void RFBReset(RFBClient_t *client)
{
    RFBASSERT(client != NULL);

    if (client->m_fd > 0) {
        close(client->m_fd);
        client->m_fd = -1;
    }

    RFBCommonInit(client);
    client->m_reset++;

    LOG_RFB("RFB: Reset\n");
}

void RFBDestroy(RFBClient_t *client) {

    RFBASSERT(client != NULL);

    RFBReset(client);

    if (client->m_server != NULL)
        free(client->m_server);
    if (client->m_password != NULL)
        free(client->m_password);
    if (client->m_desktopName != NULL)
        free(client->m_desktopName);
    if (client->m_cutTextBuffer != NULL)
        free(client->m_cutTextBuffer);
    if (client != NULL)
        free (client);
    LOG_RFB("RFB: Destroyed\n");

}

int RFBStart(RFBClient_t *client) {
    RFBASSERT(client != NULL);
    RFBASSERT(client->m_state == RFB_CLIENT_STATE_RESET);
    if (client->m_state != RFB_CLIENT_STATE_RESET)
        return -1;
    client->m_state = RFB_CLIENT_STATE_STARTED;
    LOG_RFB("RFB: Started\n");
    return 0;
}

/*
 * The Go method: Give the library some CPU cycles do its stuff.
 * Designed to be completely non-blocking
 */
int RFB_Go(RFBClient_t *client) {

    int retval = 0;
    int rc;
    RFBASSERT(client != NULL);

    client->m_gocount++;

    int previous_state = client->m_state;

    switch (client->m_state) {

    case RFB_CLIENT_STATE_RESET:
        client->m_state = RFB_CLIENT_STATE_ERROR;
        client->m_errorcode = RFB_CLIENT_RC_STATE_ERROR;
        break;

    case RFB_CLIENT_STATE_STARTED:
        if (client->m_connectionType == RFB_CLIENT_CT_TCP) {
            rc = TCPConnect(client);
            if (rc == RFB_CLIENT_RC_SUCCESS) {
                client->m_state = RFB_CLIENT_STATE_CONNECTING;
                client->m_errorcode = RFB_CLIENT_RC_CONNECTING;
            }
            else {
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = rc;
            }
        }
        else {
            // other types not yet implemented
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = RFB_CLIENT_RC_STATE_ERROR;
        }
        break;

    case RFB_CLIENT_STATE_CONNECTING:
        if (client->m_connectionType == RFB_CLIENT_CT_TCP) {
            if ((rc = CheckTCPConnection(client)) == RFB_CLIENT_RC_SUCCESS) {
                client->m_state = RFB_CLIENT_STATE_CONNECTED;
                client->m_errorcode = RFB_CLIENT_RC_CONNECTED;
            }
            else if (!IOERROR(rc)) {
                if (client->m_gocount > 2000) {
                    client->m_state = RFB_CLIENT_STATE_ERROR;
                    client->m_errorcode = RFB_CLIENT_RC_TIMEDOUT;
                }
            }
            else {
                client->m_state = RFB_CLIENT_STATE_ERROR;
                client->m_errorcode = rc;
            }
        }
        else {
            // other types not yet implemented
            client->m_state = RFB_CLIENT_STATE_ERROR;
            client->m_errorcode = RFB_CLIENT_RC_STATE_ERROR;

        }

        break;

    case RFB_CLIENT_STATE_CONNECTED:

        // Perform intial handshake that will take us to the next
        // state (READY)
        if (CheckRFBInit(client) == 1) {
            retval = 1;
        }
        break;

    case RFB_CLIENT_STATE_ASK_PASSWORD:
        // The Initial handshake substate machine can put us
        // into this state is password is needed
        // Stay in this state till a password is available
        if (client->m_passwordSet) {
            client->m_state = RFB_CLIENT_STATE_CONNECTED;
            client->m_substate = PROTO_STATE_CHECK_PASSWORD;
            client->m_errorcode = RFB_CLIENT_RC_CONNECTED;
        }
        break;

    case RFB_CLIENT_STATE_READY:

        // Perform Ready state substate transitions
        if (CheckRFBReady(client) == 1) {
            retval = 1;
        }
        break;

    case RFB_CLIENT_STATE_ERROR:
        // Not expected to be called in this state, but just in case
        // stay in this state

        break;
    default:
        RFBASSERT(0);
    }

    // Check if state changed;
    if (client->m_state != previous_state) {
        LOG_RFB("RFB: %ld %d -> %d\n",
                client->m_gocount, previous_state, client->m_state);
        return 1;
    }
    return retval;
}

int RFBState(RFBClient_t *client) {
    RFBASSERT(client != NULL);
    return client->m_state;
}

int RFBErrorCode(RFBClient_t *client) {
    RFBASSERT(client != NULL);
    return client->m_errorcode;
}

char * RFBErrorString(RFBClient_t *client) {
    RFBASSERT(client != NULL);
    switch(client->m_errorcode) {
    case RFB_CLIENT_RC_SUCCESS:
        return "Success";
    case RFB_CLIENT_RC_STATE_ERROR:
        return "State Error";
    case RFB_CLIENT_RC_NETWORK_ERROR:
        return "Network Error";
    case RFB_CLIENT_RC_AUTH_FAILED:
        return "Authentication Failed";
    case RFB_CLIENT_RC_WAIT:
        return "Waiting";
    case RFB_CLIENT_RC_PROTOCOL_ERROR:
        return "Protocol Error";
    case RFB_CLIENT_RC_OUT_OF_MEMORY:
        return "Out of Memory";
    case RFB_CLIENT_RC_TIMEDOUT:
        return "Connection Timedout";
    case RFB_CLIENT_RC_IDLE:
        return "Idle";
    case RFB_CLIENT_RC_CONNECTING:
        return "Connecting";
    case RFB_CLIENT_RC_CONNECTED:
        return "Connected";
    case RFB_CLIENT_RC_AUTHORIZING:
        return "Authorizing";
    case RFB_CLIENT_RC_READY:
        return "Ready";
    case RFB_CLIENT_RC_REMOTE_CLOSED:
        return "Remote Closed";
    case RFB_CLIENT_RC_INITIALIZING:
        return "Initializing";
    case RFB_CLIENT_RC_INTERNAL_ERROR:
        return "Internal Error";
    case RFB_CLIENT_RC_SERVER_UNAVAILABLE:
        return "Server Unavailable";
    case RFB_CLIENT_RC_WAITING_FOR_PASSWORD:
        return "Waiting for password";
    default:
        return "Unknown Error";
    }
}

int
SendMouseEvent(RFBClient_t * client, int x, int y, int buttonMask)
{

    if (client->m_bytesToWrite + 16 > RFB_BUF_SIZE ) {
        client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
        return RFB_CLIENT_RC_OUT_OF_MEMORY;
    }

    rfbPointerEventMsg pe;
    client->m_mouseSent++;
    
    pe.type = rfbPointerEvent;
    pe.buttonMask = buttonMask;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    
    pe.x = htons(x);
    pe.y = htons(y);
    
    RFBWriteToServer(client, (unsigned char *)&pe, sz_rfbPointerEventMsg);
    
    return RFB_CLIENT_RC_SUCCESS;
}


int
SendKeyboardEvent(RFBClient_t * client, unsigned long key, int down)
{

    if (client->m_bytesToWrite + 16 > RFB_BUF_SIZE ) {
        client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
        return RFB_CLIENT_RC_OUT_OF_MEMORY;
    }

    rfbKeyEventMsg ke;
    client->m_keyboardSent++;
    memset(&ke, 0x00, sizeof(ke));
    ke.type = rfbKeyEvent;
    ke.down = down ? 1 : 0;
    ke.key = htonl(key);
    
    RFBWriteToServer(client, (unsigned char *)&ke, sz_rfbKeyEventMsg);
    
    return RFB_CLIENT_RC_SUCCESS;
}

int
RFBSendExpectedPixelFormat(RFBClient_t * client)
{
    if (client->m_bytesToWrite + 16 > RFB_BUF_SIZE ) {
        client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
        return RFB_CLIENT_RC_OUT_OF_MEMORY;
    }

    // We need BGRA so we ask for ARGB in little-endian format
    // that ends up delivering the right format
    rfbSetPixelFormatMsg pixelformat;
    memset(&pixelformat, 0x00, sizeof(pixelformat));
    pixelformat.type = rfbSetPixelFormat;
    pixelformat.format.bitsPerPixel = 32;
    pixelformat.format.depth = 32;
    pixelformat.format.bigEndian = 0;
    pixelformat.format.trueColour = 1;
    pixelformat.format.redMax = htons(255);
    pixelformat.format.redShift = 16;
    pixelformat.format.greenMax = htons(255);
    pixelformat.format.greenShift = 8;
    pixelformat.format.blueMax = htons(255);
    pixelformat.format.blueShift = 0;

    RFBWriteToServer(client, (unsigned char *)&pixelformat, sz_rfbSetPixelFormatMsg);

    return RFB_CLIENT_RC_SUCCESS;
}

int
RFBSendSupportedEncodings(RFBClient_t * client)
{
    if (client->m_bytesToWrite + 16 > RFB_BUF_SIZE ) {
        client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
        return RFB_CLIENT_RC_OUT_OF_MEMORY;
    }

    rfbSetEncodingsMsg se;
    memset(&se, 0x00, sizeof(se));
    se.type = rfbSetEncodings;
    se.nEncodings = htons(1);

    RFBWriteToServer(client, (unsigned char *)&se, sz_rfbSetEncodingsMsg);

    unsigned long encoding = ntohl(0);
    RFBWriteToServer(client, (unsigned char *)&encoding, 4);

    return RFB_CLIENT_RC_SUCCESS;
}

int RFBSendVideoRequest(RFBClient_t *client, int xPos, int yPos,
        int width, int height, int incremental)
{
    if (client->m_bytesToWrite + 16 > RFB_BUF_SIZE ) {
        client->m_errorcode = RFB_CLIENT_RC_OUT_OF_MEMORY;
        return RFB_CLIENT_RC_OUT_OF_MEMORY;
    }

    rfbFramebufferUpdateRequestMsg req;
    memset(&req, 0x00, sizeof(req));

    req.type = rfbFramebufferUpdateRequest;
    req.incremental = incremental;
    req.x = xPos;
    req.y = yPos;
    req.w = htons(width);
    req.h = htons(height);

    RFBWriteToServer(client, (unsigned char *)&req, sz_rfbFramebufferUpdateRequestMsg);

    return RFB_CLIENT_RC_SUCCESS;
}
