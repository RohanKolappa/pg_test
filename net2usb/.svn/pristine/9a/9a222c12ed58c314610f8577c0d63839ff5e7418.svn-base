#ifndef VNCRFB_H
#define VNCRFB_H

/*
 * rfb.h - header file for RFB DDX implementation.
 */

/*
 *  Copyright (C) 2002 RealVNC Ltd.
 *  OSXvnc Copyright (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc code Copyright (C) 1999 AT&T Laboratories Cambridge.
 *  All Rights Reserved.
 */

#if(defined __cplusplus)
extern "C"
{
#endif


#include "vncrfbconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vncrfbproto.h"

#ifdef LIBVNCSERVER_HAVE_SYS_TYPES_H
/*#include <sys/types.h> */
#include <types.h>

// ddd7
#include <unistd.h>



#endif

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
#include <pthread.h>
#if 0 /* debugging */
#define LOCK(mutex) (rfbLog("%s:%d LOCK(%s,0x%x)\n",__FILE__,__LINE__,#mutex,&(mutex)), pthread_mutex_lock(&(mutex)))
#define UNLOCK(mutex) (rfbLog("%s:%d UNLOCK(%s,0x%x)\n",__FILE__,__LINE__,#mutex,&(mutex)), pthread_mutex_unlock(&(mutex)))
#define MUTEX(mutex) pthread_mutex_t (mutex)
#define INIT_MUTEX(mutex) (rfbLog("%s:%d INIT_MUTEX(%s,0x%x)\n",__FILE__,__LINE__,#mutex,&(mutex)), pthread_mutex_init(&(mutex),NULL))
#define TINI_MUTEX(mutex) (rfbLog("%s:%d TINI_MUTEX(%s)\n",__FILE__,__LINE__,#mutex), pthread_mutex_destroy(&(mutex)))
#define TSIGNAL(cond) (rfbLog("%s:%d TSIGNAL(%s)\n",__FILE__,__LINE__,#cond), pthread_cond_signal(&(cond)))
#define WAIT(cond,mutex) (rfbLog("%s:%d WAIT(%s,%s)\n",__FILE__,__LINE__,#cond,#mutex), pthread_cond_wait(&(cond),&(mutex)))
#define COND(cond) pthread_cond_t (cond)
#define INIT_COND(cond) (rfbLog("%s:%d INIT_COND(%s)\n",__FILE__,__LINE__,#cond), pthread_cond_init(&(cond),NULL))
#define TINI_COND(cond) (rfbLog("%s:%d TINI_COND(%s)\n",__FILE__,__LINE__,#cond), pthread_cond_destroy(&(cond)))
#define IF_PTHREADS(x) x
#else
   // mutexes
#define LOCK(mutex) pthread_mutex_lock(&(mutex));
#define UNLOCK(mutex) pthread_mutex_unlock(&(mutex));
   // declare mutex as a variable of type 'pthread_mutex_t'
#define MUTEX(mutex) pthread_mutex_t (mutex)
#define INIT_MUTEX(mutex) pthread_mutex_init(&(mutex),NULL)
#define TINI_MUTEX(mutex) pthread_mutex_destroy(&(mutex))
#define TSIGNAL(cond) pthread_cond_signal(&(cond))
#define WAIT(cond,mutex) pthread_cond_wait(&(cond),&(mutex))
#define COND(cond) pthread_cond_t (cond)
#define INIT_COND(cond) pthread_cond_init(&(cond),NULL)
#define TINI_COND(cond) pthread_cond_destroy(&(cond))
#define IF_PTHREADS(x) x
#endif
#else
#define LOCK(mutex)
#define UNLOCK(mutex)
#define MUTEX(mutex)
#define INIT_MUTEX(mutex)
#define TINI_MUTEX(mutex)
#define TSIGNAL(cond)
#define WAIT(cond,mutex) this_is_unsupported
#define COND(cond)
#define INIT_COND(cond)
#define TINI_COND(cond)
#define IF_PTHREADS(x)
#endif

/* end of stuff for autoconf */



struct _rfbClientRec;
struct _rfbScreenInfo;

enum rfbNewClientAction {
   RFB_CLIENT_ACCEPT,
   RFB_CLIENT_ON_HOLD,
   RFB_CLIENT_REFUSE
};

typedef void (*rfbKbdAddEventProcPtr) (rfbBool down, rfbKeySym keySym, struct _rfbClientRec* cl);
typedef void (*rfbPtrAddEventProcPtr) (int buttonMask, int x, int y, struct _rfbClientRec* cl);
typedef enum rfbNewClientAction (*rfbNewClientHookPtr)(struct _rfbClientRec* cl);
typedef void (*rfbDisplayHookPtr)(struct _rfbClientRec* cl);

//========= orig
typedef struct {
  uint32_t count;
  rfbBool is16; /* is the data format short? */
  union {
    uint8_t* bytes;
    uint16_t* shorts;
  } data; /* there have to be count*3 entries */
} rfbColourMap;

/*
 * Per-screen (framebuffer) structure.  There can be as many as you wish,
 * each serving different clients. However, you have to call
 * rfbProcessEvents for each of these.
 */

typedef struct _rfbScreenInfo
{
    int width;
    int paddedWidthInBytes;
    int height;
    int depth;
    int bitsPerPixel;
    int sizeInBytes;

    /* some screen specific data can be put into a struct where screenData
     * points to. You need this if you have more than one screen at the
     * same time while using the same functions.
     */
    void* screenData;


    rfbBool cursorIsDrawn;        /* TRUE if the cursor is currently drawn */


    /* additions by libvncserver */

    rfbPixelFormat serverFormat;

    //=========== orig
   rfbColourMap colourMap; /* set this if rfbServerFormat.trueColour==FALSE */

    const char* desktopName;
    char thisHost[255];

    rfbBool autoPort;
    int port;
    SOCKET listenSock;
    int maxSock;
    int maxFd;
    fd_set allFds;

    rfbBool socketInitDone;
    SOCKET inetdSock;
    rfbBool inetdInitDone;

    int udpPort;
    SOCKET udpSock;
    struct _rfbClientRec* udpClient;
    rfbBool udpSockConnected;
    struct sockaddr_in udpRemoteAddr;

    int maxClientWait;

    /* http stuff */
    rfbBool httpInitDone;
    rfbBool httpEnableProxyConnect;
    int httpPort;
    char* httpDir;
    SOCKET httpListenSock;
    SOCKET httpSock;


    /* this is the amount of milliseconds to wait at least before sending
     * an update. */
    int deferUpdateTime;

    char* screen;

    rfbBool alwaysShared;
    rfbBool neverShared;
    rfbBool dontDisconnect;
    struct _rfbClientRec* clientHead;

    /* cursor */
    int cursorX, cursorY,oldCursorX,oldCursorY;



    rfbKbdAddEventProcPtr kbdAddEvent;
    rfbPtrAddEventProcPtr ptrAddEvent;


    /* newClientHook is called just after a new client is created */
    rfbNewClientHookPtr newClientHook;

 #ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    MUTEX(cursorMutex);
    rfbBool backgroundLoop;
#endif

    /* if TRUE, an ignoring signal handler is installed for SIGPIPE */
    rfbBool ignoreSIGPIPE;

  volatile  int ToEnd;  /* true - stop the server */
  volatile  int ListenerRuns;  /* true - listenerRun thread is running */

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    MUTEX(rfbClientListMutex);

  volatile pid_t listenerPid;
#endif

} rfbScreenInfo, *rfbScreenInfoPtr;



/*
 * Per-client structure.
 */

typedef void (*ClientGoneHookPtr)(struct _rfbClientRec* cl);

typedef struct _rfbClientRec {

    /* back pointer to the screen */
    rfbScreenInfoPtr screen;

    /* private data. You should put any application client specific data
     * into a struct and let clientData point to it. Don't forget to
     * free the struct via clientGoneHook!
     *
     * This is useful if the IO functions have to behave client specific.
     */
    void* clientData;
    ClientGoneHookPtr clientGoneHook;

    SOCKET sock;
    char *host;

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    pthread_t client_thread;
#endif
                                /* Possible client states: */
    enum {
        RFB_PROTOCOL_VERSION,   /* establishing protocol version */
        RFB_AUTHENTICATION,     /* authenticating, this step is skip in SCN */
        RFB_INITIALISATION,     /* sending initialisation messages */
        RFB_NORMAL              /* normal protocol messages */
    } state;

    rfbBool onHold;
    int preferredEncoding;

    rfbBool viewOnly;




    /* The following member represents the state of the "deferred update" timer
       - when the framebuffer is modified and the client is ready, in most
       cases it is more efficient to defer sending the update by a few
       milliseconds so that several changes to the framebuffer can be combined
       into a single update. */

      struct timeval startDeferring;

    /* translateFn points to the translation function which is used to copy
       and translate a rectangle from the framebuffer to an output buffer. */


    rfbPixelFormat format;



    /* statistics */

    int bytesSent[MAX_ENCODINGS];
    int rectanglesSent[MAX_ENCODINGS];
    int lastRectMarkersSent;
    int lastRectBytesSent;
    int cursorShapeBytesSent;
    int cursorShapeUpdatesSent;
    int cursorPosBytesSent;
    int cursorPosUpdatesSent;
    int framebufferUpdateMessagesSent;
    int rawBytesEquivalent;
    int keyEventsRcvd;
    int pointerEventsRcvd;



    rfbBool enableCursorPosUpdates;   /* client supports cursor position updates */
    rfbBool cursorWasMoved;           /* cursor position update should be sent */


    struct _rfbClientRec *prev;
    struct _rfbClientRec *next;

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    /* whenever a client is referenced, the refCount has to be incremented
       and afterwards decremented, so that the client is not cleaned up
       while being referenced.
       Use the functions rfbIncrClientRef(cl) and rfbDecrClientRef(cl);
    */
    int refCount;
    MUTEX(refCountMutex);
    COND(deleteCond);

    MUTEX(outputMutex);
    MUTEX(updateMutex);
    COND(updateCond);
#endif

    rfbBool IsClientRinning;

    // Netviz 2
    int width;
    int height;
    rfbBool IsClientNetviz;

} rfbClientRec, *rfbClientPtr;


/*
 * This macro is used to test whether there is a framebuffer update needing to
 * be sent to the client.
 */
#define FB_UPDATE_PENDING(cl) ((cl)->enableCursorPosUpdates && (cl)->cursorWasMoved)



/*
 * Macros for endian swapping.
 */

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

#define Swap24(l) ((((l) & 0xff) << 16) | (((l) >> 16) & 0xff) | \
                   (((l) & 0x00ff00)))

#define Swap32(l) (((l) >> 24) | \
                   (((l) & 0x00ff0000) >> 8)  | \
                   (((l) & 0x0000ff00) << 8)  | \
                   ((l) << 24))


extern char rfbEndianTest;

#define Swap16IfLE(s) (rfbEndianTest ? Swap16(s) : (s))
#define Swap24IfLE(l) (rfbEndianTest ? Swap24(l) : (l))
#define Swap32IfLE(l) (rfbEndianTest ? Swap32(l) : (l))



/* sockets.c */

extern int rfbMaxClientWait;

extern void rfbInitSockets(rfbScreenInfoPtr rfbScreen);
extern void rfbDisconnectUDPSock(rfbScreenInfoPtr rfbScreen);
extern void rfbCloseClient(rfbClientPtr cl);
extern int rfbReadExact(rfbClientPtr cl, char *buf, int len);
extern int rfbReadExactTimeout(rfbClientPtr cl, char *buf, int len,int timeout);
extern int rfbWriteExact(rfbClientPtr cl, const char *buf, int len);
extern void rfbCheckFds(rfbScreenInfoPtr rfbScreen,long usec);
extern int rfbConnect(rfbScreenInfoPtr rfbScreen, char* host, int port);
extern int rfbConnectToTcpAddr(char* host, int port);
extern int rfbListenOnTCPPort(int port);
extern int rfbListenOnUDPPort(int port);



/* rfbserver.c */

/* Routines to iterate over the client list in a thread-safe way.
   Only a single iterator can be in use at a time process-wide. */
typedef struct rfbClientIterator *rfbClientIteratorPtr;

extern void rfbClientListInit(rfbScreenInfoPtr rfbScreen);
extern rfbClientIteratorPtr rfbGetClientIterator(rfbScreenInfoPtr rfbScreen);
extern rfbClientPtr rfbClientIteratorNext(rfbClientIteratorPtr iterator);
extern void rfbReleaseClientIterator(rfbClientIteratorPtr iterator);

extern void rfbNewClientConnection(rfbScreenInfoPtr rfbScreen,int sock);
extern rfbClientPtr rfbNewClient(rfbScreenInfoPtr rfbScreen,int sock);
extern rfbClientPtr rfbNewUDPClient(rfbScreenInfoPtr rfbScreen);
extern void rfbClientConnectionGone(rfbClientPtr cl);
extern void rfbProcessClientMessage(rfbClientPtr cl);
extern void rfbClientConnFailed(rfbClientPtr cl, char *reason);
extern void rfbNewUDPConnection(rfbScreenInfoPtr rfbScreen,int sock);
extern void rfbProcessUDPInput(rfbScreenInfoPtr rfbScreen);





/* httpd.c */

extern void rfbHttpInitSockets(rfbScreenInfoPtr rfbScreen);
extern void rfbHttpCheckFds(rfbScreenInfoPtr rfbScreen);



/* cargs.c */

extern void rfbUsage(void);
extern void rfbPurgeArguments(int* argc,int* position,int count,char *argv[]);
extern rfbBool rfbProcessArguments(rfbScreenInfoPtr rfbScreen,int* argc, char *argv[]);
extern rfbBool rfbProcessSizeArguments(int* width,int* height,int* bpp,int* argc, char *argv[]);









/* cursor handling for the pointer */
extern void rfbDefaultPtrAddEvent(int buttonMask,int x,int y,rfbClientPtr cl);


/* stats.c */

extern void rfbResetStats(rfbClientPtr cl);
extern void rfbPrintStats(rfbClientPtr cl);



#if 0

/* font.c */

typedef struct rfbFontData {
  unsigned char* data;
  /*
    metaData is a 256*5 array:
    for each character
    (offset,width,height,x,y)
  */
  int* metaData;
} rfbFontData,* rfbFontDataPtr;



/* selbox.c */

/* this opens a modal select box. list is an array of strings, the end marked
   with a NULL.
   It returns the index in the list or -1 if cancelled or something else
   wasn't kosher. */
typedef void (*SelectionChangedHookPtr)(int _index);
extern int rfbSelectBox(rfbScreenInfoPtr rfbScreen,
         rfbFontDataPtr font, char** list,
         int x1, int y1, int x2, int y2,
         rfbPixel foreColour, rfbPixel backColour,
         int border,SelectionChangedHookPtr selChangedHook);


#endif

/* main.c */

extern void rfbLogEnable(int enabled);
typedef void (*rfbLogProc)(const char *format, ...);
extern rfbLogProc rfbLog, rfbErr, rfbLogPerror;

void rfbDoNothingWithClient(rfbClientPtr cl);
enum rfbNewClientAction defaultNewClientHook(rfbClientPtr cl);


/* functions to make a vnc server */
extern rfbScreenInfoPtr rfbGetScreen(int* argc, char** argv,
 int width, int height, int bitsPerSample, int samplesPerPixel,
 int bytesPerPixel);

extern void rfbInitServer(rfbScreenInfoPtr rfbScreen);

extern void rfbScreenCleanup(rfbScreenInfoPtr screenInfo);



/* functions to accept/refuse a client that has been put on hold
   by a NewClientHookPtr function. Must not be called in other
   situations. */
extern void rfbStartOnHoldClient(rfbClientPtr cl);
extern void rfbRefuseOnHoldClient(rfbClientPtr cl);

/* call one of these two functions to service the vnc clients.
 usec are the microseconds the select on the fds waits.
 if you are using the event loop, set this to some value > 0, so the
 server doesn't get a high load just by listening. */

extern void rfbRunEventLoop(rfbScreenInfoPtr screenInfo, long usec, rfbBool runInBackground);
extern void rfbProcessEvents(rfbScreenInfoPtr screenInfo,long usec);

#endif

#if(defined __cplusplus)
}
#endif
