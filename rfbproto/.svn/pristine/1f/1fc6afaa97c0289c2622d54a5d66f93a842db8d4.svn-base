#ifndef _RFBCLIENT_H_
#define _RFBCLIENT_H_

#include "rfbproto.h"

#define RFB_BUF_SIZE 8192

typedef struct RFBRect {
    int    m_XPos;
    int    m_YPos;
    int    m_Width;
    int    m_Height;
    int    m_Length;
    int    m_Encoding;
    int    m_NumberOfRectangles;
    int    m_RectangleIndex;
    unsigned char * m_Data;
} RFBRect_t;

#define LAST_RECTANGLE(pRect) (((pRect)->m_RectangleIndex + 1) >= ((pRect)->m_NumberOfRectangles))

/* Data structures */
typedef struct RFBClient {
    char *              m_server;
    int                 m_port;
    int                 m_connectionType;
    int                 m_fd;
    int                 m_frameWidth;
    int                 m_frameHeight;
    int                 m_passwordSet;
    char *              m_password;
    char *              m_desktopName;
    int                 m_desktopNameLength;
    int                 m_serverCutTextLength;
    int                 m_protocolMinorVersion;
    int                 m_state;
    int                 m_substate;
    int                 m_errorcode;
    unsigned long       m_gocount;
    unsigned long       m_reset;
    unsigned char       m_readBuf[RFB_BUF_SIZE];
    unsigned char       m_fixedArea[RFB_BUF_SIZE];
    unsigned char *     m_readArea;
    int                 m_readAreaLength;
    int                 m_bytesToRead;
    int                 m_buffered;
    unsigned char *     m_readPtr;
    unsigned char       m_writeArea[RFB_BUF_SIZE];
    int                 m_bytesToWrite;
    int                 m_maxBytesToWrite;
    unsigned char *     m_writePtr;
    unsigned char *     m_cutTextBuffer;
    int                 m_secType;
    unsigned long       m_keyboardSent;
    unsigned long       m_mouseSent;
    unsigned long       m_bytesWritten;
    unsigned long       m_bytesRead;
    unsigned long       m_packetsRead;

    rfbPixelFormat      m_pixelFormat;
    int                 m_nRectangleCount;
    int                 m_nRemainingRectangles;
    RFBRect_t *         m_pInputRect;
    RFBRect_t *         m_pOuputRect;
} RFBClient_t;

/* Hash Defines */
#define RFB_CLIENT_STATE_RESET                  0
#define RFB_CLIENT_STATE_STARTED                1
#define RFB_CLIENT_STATE_READY                  2
#define RFB_CLIENT_STATE_ERROR                  3
#define RFB_CLIENT_STATE_CONNECTING             4
#define RFB_CLIENT_STATE_CONNECTED              5
#define RFB_CLIENT_STATE_ASK_PASSWORD           6

#define RFB_CLIENT_RC_SUCCESS                   0
#define RFB_CLIENT_RC_STATE_ERROR               1
#define RFB_CLIENT_RC_NETWORK_ERROR             2
#define RFB_CLIENT_RC_AUTH_FAILED               3
#define RFB_CLIENT_RC_WAIT                      4
#define RFB_CLIENT_RC_PROTOCOL_ERROR            5
#define RFB_CLIENT_RC_OUT_OF_MEMORY             6
#define RFB_CLIENT_RC_TIMEDOUT                  7
#define RFB_CLIENT_RC_IDLE                      8
#define RFB_CLIENT_RC_CONNECTING                9
#define RFB_CLIENT_RC_CONNECTED                 10
#define RFB_CLIENT_RC_AUTHORIZING               11
#define RFB_CLIENT_RC_READY                     12
#define RFB_CLIENT_RC_REMOTE_CLOSED             13
#define RFB_CLIENT_RC_INITIALIZING              14
#define RFB_CLIENT_RC_SERVER_UNAVAILABLE        15
#define RFB_CLIENT_RC_INTERNAL_ERROR            16
#define RFB_CLIENT_RC_PARTIAL_IO                17
#define RFB_CLIENT_RC_WAITING_FOR_PASSWORD      18


#define RFB_CLIENT_CT_TCP               1
#define RFB_CLIENT_CT_SERIAL            2

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif
    extern RFBClient_t * CreateRFBNetworkClient(const char *host, int port, const char * password);
    extern void RFBSetPassword(RFBClient_t *client, const char * password);
    extern int RFBStart(RFBClient_t *client);
    extern int RFB_Go(RFBClient_t *client);
    extern void RFBReset(RFBClient_t *client);
    extern void RFBDestroy(RFBClient_t *client);
    extern int RFBState(RFBClient_t *client);
    extern int RFBErrorCode(RFBClient_t *client);
    extern char * RFBErrorString(RFBClient_t *client);
    extern int SendMouseEvent(RFBClient_t * client, int x, int y, int buttonMask);
    extern int SendKeyboardEvent(RFBClient_t * client, unsigned long key, int down);
    extern int RFBSendVideoRequest(RFBClient_t *client, int xPos, int yPos,
            int width, int height, int incremental);
    extern RFBRect_t * RFBGetRectangle(RFBClient_t * client);
    extern RFBRect_t * RFBCreateRectangle(int x, int y, int width, int height, int count, int index);
    extern void RFBDestroyRectangle(RFBRect_t * pRect);
    extern int RFBSendSupportedEncodings(RFBClient_t * client);
    extern int RFBSendExpectedPixelFormat(RFBClient_t * client);


#ifdef __cplusplus
}
#endif



#endif
