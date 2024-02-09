/*
 * vncserver.c - deal with server-side of the RFB protocol.
 */

/*
 *  Copyright (C) 2002 RealVNC Ltd.
 *  OSXvnc Copyright (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc code Copyright (C) 1999 AT&T Laboratories Cambridge.
 *  All Rights Reserved.
 *
 */

#include <string.h>
#include "vncrfb.h"

#ifdef LIBVNCSERVER_HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef WIN32
#define write(sock,buf,len) send(sock,buf,len,0)
#else
#ifdef LIBVNCSERVER_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <pwd.h>
#ifdef LIBVNCSERVER_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef LIBVNCSERVER_HAVE_NETINET_IN_H
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif
#endif


static rfbClientPtr pointerClient = NULL;  /* "Mutex" for pointer events */

static void rfbProcessClientProtocolVersion(rfbClientPtr cl);
static void rfbProcessClientNormalMessage(rfbClientPtr cl);
static void rfbProcessClientInitMessage(rfbClientPtr cl);


#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
   /******************************************************************************************************/
   void rfbIncrClientRef(rfbClientPtr cl)
   {
     LOCK(cl->refCountMutex);
     cl->refCount++;
     UNLOCK(cl->refCountMutex);
   }

   /******************************************************************************************************/
   void rfbDecrClientRef(rfbClientPtr cl)
   {
     LOCK(cl->refCountMutex);
     cl->refCount--;
     if(cl->refCount<=0) /* just to be sure also < 0 */
       TSIGNAL(cl->deleteCond);
     UNLOCK(cl->refCountMutex);
   }
#else
   /******************************************************************************************************/
   void rfbIncrClientRef(rfbClientPtr cl) {}
   /******************************************************************************************************/
   void rfbDecrClientRef(rfbClientPtr cl) {}
#endif

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
//MUTEX(rfbClientListMutex);
#endif

struct rfbClientIterator
{
  rfbClientPtr next;
  rfbScreenInfoPtr screen;
};



/******************************************************************************************************/
void rfbClientListInit(rfbScreenInfoPtr rfbScreen)
{
 if(sizeof(rfbBool) != 1)
   {
       /* a sanity check */
    fprintf(stderr,"rfbBool's sizeof() is not 1 but actually %u !\n",sizeof(rfbBool));
     /* we cannot continue, because rfbBool is supposed to be char everywhere */
    exit(1);
   }

 rfbScreen->clientHead = NULL;
 INIT_MUTEX(rfbScreen->rfbClientListMutex);
}



/******************************************************************************************************/
rfbClientIteratorPtr rfbGetClientIterator(rfbScreenInfoPtr rfbScreen)
{
 rfbClientIteratorPtr i;

rfbLog("\r\n +++malloc rfbClientIteratorPtr  in rfbGetClientIterator() \r\n");
 i = (rfbClientIteratorPtr)malloc(sizeof(struct rfbClientIterator));
 i->next = 0;
 i->screen = rfbScreen;

 return i;
}



/******************************************************************************************************/
rfbClientPtr rfbClientIteratorHead(rfbClientIteratorPtr i)
{
#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD

 if(i->next != 0)
   {
    rfbDecrClientRef(i->next);
    rfbIncrClientRef(i->screen->clientHead);
   }

#endif
 LOCK(i->screen->rfbClientListMutex);

  i->next = i->screen->clientHead;

 UNLOCK(i->screen->rfbClientListMutex);

 return i->next;
}


/******************************************************************************************************/
rfbClientPtr rfbClientIteratorNext(rfbClientIteratorPtr i)
{
 if(i->next == 0)
   {
    LOCK(i->screen->rfbClientListMutex);

     i->next = i->screen->clientHead;

    UNLOCK(i->screen->rfbClientListMutex);
   }
 else
   {
    IF_PTHREADS(rfbClientPtr cl = i->next);
     i->next = i->next->next;
    IF_PTHREADS(rfbDecrClientRef(cl));
   }

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
 while(i->next && (i->next->sock < 0)) i->next = i->next->next;

 if(i->next) rfbIncrClientRef(i->next);
#endif

 return i->next;
}



/******************************************************************************************************/
void rfbReleaseClientIterator(rfbClientIteratorPtr iterator)
{
 IF_PTHREADS(if(iterator->next) rfbDecrClientRef(iterator->next));

rfbLog("\r\n ---free rfbClientIteratorPtr in rfbReleaseClientIterator() \r\n");
 free(iterator);
}



/******************************************************************************************************/
/*
 * rfbNewClientConnection is called from sockets.c when a new connection
 * comes in.
 */
void rfbNewClientConnection(rfbScreenInfoPtr rfbScreen, int sock)
{
 rfbClientPtr cl;
 cl = rfbNewClient(rfbScreen, sock);
}



/******************************************************************************************************/
rfbClientPtr rfbNewTCPOrUDPClient(rfbScreenInfoPtr rfbScreen, int sock, rfbBool isUDP)
{
 rfbProtocolVersionMsg pv;
 rfbClientIteratorPtr iterator;
 rfbClientPtr cl,cl_;
 struct sockaddr_in addr;

printf("\t\tNew TCP or UDP client\n") ;

 socklen_t addrlen = sizeof(struct sockaddr_in);

rfbLog("\r\n +++calloc rfbClientPtr in rfbNewTCPOrUDPClient() \r\n");
 cl = (rfbClientPtr)calloc(sizeof(rfbClientRec),1);

 cl->screen = rfbScreen;
 cl->sock = sock;
 cl->viewOnly = FALSE;

 rfbResetStats(cl);

 if(isUDP)
   {
    rfbLog(" accepted UDP client\n");
   }
 else
   {    // new connection is TCP
    int one = 1;

    int ret = getpeername(sock, (struct sockaddr *)&addr, &addrlen);
    if (ret)
    {
        rfbLogPerror("getpeername failed !\n");
        close(sock);
        return NULL;
    }
    cl->host = strdup(inet_ntoa(addr.sin_addr));

    rfbLog("  other clients:\n");
    iterator = rfbGetClientIterator(rfbScreen);

    while ((cl_ = rfbClientIteratorNext(iterator)) != NULL) rfbLog("     %s\n",cl_->host);

    rfbReleaseClientIterator(iterator);

#ifndef WIN32
    if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
      {
       rfbLogPerror("fcntl failed");
       close(sock);

       return NULL;
      }
#endif

    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one)) < 0)
      {
       rfbLogPerror("setsockopt failed");
       close(sock);
       if (cl)
       {
           free(cl);
       }
       return NULL;
      }

    FD_SET(sock, &(rfbScreen->allFds));
    rfbScreen->maxFd = max(sock, rfbScreen->maxFd);

    INIT_MUTEX(cl->outputMutex);
    INIT_MUTEX(cl->refCountMutex);
    INIT_COND(cl->deleteCond);

    cl->state = RFB_PROTOCOL_VERSION;

    cl->preferredEncoding = rfbEncodingRaw;


    INIT_MUTEX(cl->updateMutex);
    INIT_COND(cl->updateCond);

    cl->format = cl->screen->serverFormat;
    // Netviz2
    cl->width = cl->screen->width;
    cl->height = cl->screen->height;
    cl->IsClientNetviz = FALSE;

    LOCK(rfbScreen->rfbClientListMutex);
      IF_PTHREADS(cl->refCount = 0);
      cl->next = rfbScreen->clientHead;
      cl->prev = NULL;
      if(rfbScreen->clientHead) rfbScreen->clientHead->prev = cl;
      rfbScreen->clientHead = cl;
    UNLOCK(rfbScreen->rfbClientListMutex);

    cl->enableCursorPosUpdates = FALSE;

   /* send to the client RFB 003 003, first phase of handshaking */
    sprintf(pv, rfbProtocolVersionFormat, rfbProtocolMajorVersion, rfbProtocolMinorVersion);

    if(rfbWriteExact(cl, pv, sz_rfbProtocolVersionMsg) < 0)
      {
       rfbLogPerror("rfbNewClient: write");
       rfbCloseClient(cl);
        /* TODO: memory leak here (cl is never freed)
         * can rfbClientConnectionGone called at this time?
         * tim@tjansen.de
         */
       return NULL;
      }

   } // end of new connection is TCP




 cl->clientGoneHook = rfbDoNothingWithClient;

 switch(cl->screen->newClientHook(cl))
   {
    case RFB_CLIENT_ON_HOLD:
       cl->onHold = TRUE;
       break;
    case RFB_CLIENT_ACCEPT:
       cl->onHold = FALSE;
       break;
    case RFB_CLIENT_REFUSE:
        rfbLog("\r\n-- eee2 ---:\r\n");
       rfbCloseClient(cl);
       rfbClientConnectionGone(cl);
       cl = NULL;
       break;
   }

 return cl;
}



/******************************************************************************************************/
rfbClientPtr rfbNewClient(rfbScreenInfoPtr rfbScreen, int sock)
{
 return(rfbNewTCPOrUDPClient(rfbScreen, sock, FALSE));
}



/******************************************************************************************************/
rfbClientPtr rfbNewUDPClient(rfbScreenInfoPtr rfbScreen)
{
 rfbScreen->udpClient = rfbNewTCPOrUDPClient(rfbScreen, rfbScreen->udpSock, TRUE);

 return(rfbScreen->udpClient);
}



/******************************************************************************************************/
/*
 * rfbClientConnectionGone is called from sockets.c just after a connection
 * has gone away.
 */

void rfbClientConnectionGone(rfbClientPtr cl)
{
 int i;

 LOCK(cl->screen->rfbClientListMutex);

 if(cl->prev)
    cl->prev->next = cl->next;
 else
    cl->screen->clientHead = cl->next;

 if(cl->next)
   cl->next->prev = cl->prev;


#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD

 if(cl->screen->backgroundLoop != FALSE)
   do{
      LOCK(cl->refCountMutex);
        i = cl->refCount;
      UNLOCK(cl->refCountMutex);

      if(i > 0) WAIT(cl->deleteCond, cl->refCountMutex);

     } while(i > 0);

#endif

 if(cl->sock >= 0) FD_CLR(cl->sock, &(cl->screen->allFds));

 cl->clientGoneHook(cl);

 rfbLog("Client %s gone\n", cl->host);
rfbLog("\r\n ---free cl->host \r\n");
 free(cl->host);

 if(pointerClient == cl) pointerClient = NULL;


 UNLOCK(cl->screen->rfbClientListMutex);

 TINI_COND(cl->updateCond);
 TINI_MUTEX(cl->updateMutex);

 UNLOCK(cl->outputMutex);
 TINI_MUTEX(cl->outputMutex);

 rfbPrintStats(cl);

rfbLog("\r\n ---free cl in rfbClientConnectionGone() \r\n");
 free(cl);

 return;
}




/******************************************************************************************************/
/*
 * rfbProcessClientMessage is called when there is data to read from a client.
 */
void rfbProcessClientMessage(rfbClientPtr cl)
{	
 switch(cl->state)
   {
    case RFB_PROTOCOL_VERSION:
        rfbProcessClientProtocolVersion(cl);
        return;
    case RFB_INITIALISATION:
        rfbProcessClientInitMessage(cl);
        return;
    default:
        rfbProcessClientNormalMessage(cl);
        return;
   }

 return;
}



/******************************************************************************************************/
/*
 * rfbProcessClientProtocolVersion is called when the client sends its
 * protocol version.
 */

static void rfbProcessClientProtocolVersion(rfbClientPtr cl)
{
 rfbProtocolVersionMsg pv;
 int n, major_, minor_;
 char failureReason[256];

 if((n = rfbReadExact(cl, pv, sz_rfbProtocolVersionMsg)) <= 0)
   {
    if(n == 0)
       rfbLog("rfbProcessClientProtocolVersion: client gone\n");
    else
       rfbLogPerror("rfbProcessClientProtocolVersion: read");

    rfbCloseClient(cl);
    return;
   }

 pv[sz_rfbProtocolVersionMsg] = 0;

 if(sscanf(pv, rfbProtocolVersionFormat, &major_, &minor_) != 2)
   {
    char name[1024];
    if(sscanf(pv, "RFB %03d.%03d %1024s\n", &major_, &minor_, name) != 3)
      {
       rfbErr("rfbProcessClientProtocolVersion: not a valid RFB client\n");
       rfbCloseClient(cl);
       return;
      }

rfbLog("\r\n  ---  free(cl->host) in rfbProcessClientProtocolVersion();");
    free(cl->host);
    cl->host = strdup(name);
   }

 rfbLog("Protocol version %d.%d\n", major_, minor_);

 if(major_ != rfbProtocolMajorVersion)
   {
     /* Major version mismatch - send a ConnFailed message */
    rfbErr("Major version mismatch\n");

    sprintf(failureReason, "RFB protocol version mismatch - server %d.%d, client %d.%d",
                rfbProtocolMajorVersion,rfbProtocolMinorVersion,major_,minor_);

    rfbClientConnFailed(cl, failureReason);
    return;
   }

 if(minor_ != rfbProtocolMinorVersion)
   {
     /* Minor version mismatch - warn but try to continue */
    rfbLog("Ignoring minor version mismatch\n");
   }

   // here was located password checking code, but now is wiped away

                                      /* no need of authorisation */
 *(uint32_t *)failureReason = Swap32IfLE(rfbNoAuth);

 cl->state = RFB_INITIALISATION;

 if(rfbWriteExact(cl, failureReason, 4) < 0)
   {
    rfbLogPerror("rfbAuthNewClient: write");
    rfbCloseClient(cl);
   }


 return;
}



/******************************************************************************************************/
/*
 * rfbClientConnFailed is called when a client connection has failed either
 * because it talks the wrong protocol or it has failed authentication.
 */

void rfbClientConnFailed(rfbClientPtr cl, char *reason)
{
 char *buf;
 int len = strlen(reason);

 buf = (char *)malloc(8 + len);

 ((uint32_t *)buf)[0] = Swap32IfLE(rfbConnFailed);
 ((uint32_t *)buf)[1] = Swap32IfLE(len);

 memcpy(buf + 8, reason, len);

 if(rfbWriteExact(cl, buf, 8 + len) < 0) rfbLogPerror("rfbClientConnFailed: write");

 free(buf);
 rfbCloseClient(cl);

 return;
}



/******************************************************************************************************/
/*
 * rfbProcessClientInitMessage is called when the client sends its
 * initialisation message.
 */

static void rfbProcessClientInitMessage(rfbClientPtr cl)
{
 rfbClientInitMsg ci;
 char buf[256];
 int len, n;
 rfbClientIteratorPtr iterator;
 rfbClientPtr otherCl;
 rfbServerInitMsg *si = (rfbServerInitMsg *)buf;

 if((n = rfbReadExact(cl, (char *)&ci, sz_rfbClientInitMsg)) <= 0)
   {
    if(n == 0)
      rfbLog("rfbProcessClientInitMessage: client gone\n");
    else
      rfbLogPerror("rfbProcessClientInitMessage: read");

    rfbCloseClient(cl);
    return;
   }

   /* size of the windows which openes on the client`s screen */
 si->framebufferWidth = Swap16IfLE(cl->screen->width);
    si->framebufferHeight = Swap16IfLE(cl->screen->height);
    /* 1,2,4 bytes .. */
    si->format = cl->screen->serverFormat;

/*    si->format.redMax = Swap16IfLE(si->format.redMax);
    si->format.greenMax = Swap16IfLE(si->format.greenMax);
    si->format.blueMax = Swap16IfLE(si->format.blueMax);
*/

 if(strlen(cl->screen->desktopName) > 128)      /* sanity check on desktop name len */
   ((char*)cl->screen->desktopName)[128] = 0;

 strcpy(buf + sz_rfbServerInitMsg, cl->screen->desktopName);
 len = strlen(buf + sz_rfbServerInitMsg);
 si->nameLength = Swap32IfLE(len);

 if(rfbWriteExact(cl, buf, sz_rfbServerInitMsg + len) < 0)
   {
    rfbLogPerror("rfbProcessClientInitMessage: write");
    rfbLog("\r\n-- eee1 ---:\r\n");
    rfbCloseClient(cl);
    return;
   }

 cl->state = RFB_NORMAL;

 if( (cl->screen->neverShared || (!cl->screen->alwaysShared && !ci.shared)))
   {
     if(cl->screen->dontDisconnect)
       {
        iterator = rfbGetClientIterator(cl->screen);
        while ((otherCl = rfbClientIteratorNext(iterator)) != NULL)
          {
           if( (otherCl != cl) && (otherCl->state == RFB_NORMAL))
             {
              rfbLog("-dontdisconnect: Not shared & existing client\n");
              rfbLog("  refusing new client %s\n", cl->host);
              rfbCloseClient(cl);
              rfbReleaseClientIterator(iterator);
              return;
             }
          }

        rfbReleaseClientIterator(iterator);
       }
     else
       {
        iterator = rfbGetClientIterator(cl->screen);
        while((otherCl = rfbClientIteratorNext(iterator)) != NULL)
          {
           if( (otherCl != cl) && (otherCl->state == RFB_NORMAL) )
             {
              rfbLog("Not shared - closing connection to client %s\n",  otherCl->host);
              rfbCloseClient(otherCl);
             }
          }

        rfbReleaseClientIterator(iterator);
       }

   }

 return;
}



/******************************************************************************************************/
/*
 * rfbProcessClientNormalMessage is called when the client has sent a normal
 * protocol message.
 */
static void rfbProcessClientNormalMessage(rfbClientPtr cl)
{
 int n=0;
 rfbClientToServerMsg msg;
 if((n = rfbReadExact(cl, (char *)&msg, 1)) <= 0)
   {
    if(n != 0) rfbLogPerror("rfbProcessClientNormalMessage: read");
	 rfbLog("\r\n--Stop in vncserver.c\r\n");
    rfbCloseClient(cl);
    return;
   }

 switch(msg.type)
  {

   case rfbPointerEvent:
     {
      cl->pointerEventsRcvd++;

      if((n = rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbPointerEventMsg - 1)) <= 0)
        {
         if(n != 0) rfbLogPerror("rfbProcessClientNormalMessage: read");
	 rfbLog("\r\n--case rfbPointerEvent:\r\n");
         rfbCloseClient(cl);
         return;
        }

      if(pointerClient && (pointerClient != cl))
       return;

      if(msg.pe.buttonMask == 0)
         pointerClient = NULL;
      else
         pointerClient = cl;

      if(!cl->viewOnly)
        {                // KIBO: invokes our mouse handling function here
         cl->screen->ptrAddEvent(msg.pe.buttonMask, Swap16IfLE(msg.pe.x), Swap16IfLE(msg.pe.y), cl);
        }

      return;
     }


   case rfbKeyEvent:
     {
      cl->keyEventsRcvd++;
      if((n = rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbKeyEventMsg - 1)) <= 0)
        {
         if (n != 0) rfbLogPerror("rfbProcessClientNormalMessage: read");
	 rfbLog("\r\n--case rfbKeyEvent:\r\n");
         rfbCloseClient(cl);
         return;
        }

      if(!cl->viewOnly)
        {           // this line invokes our keyb function KIBO
         cl->screen->kbdAddEvent(msg.ke.down, (rfbKeySym)Swap32IfLE(msg.ke.key), cl);
        }

      return;
     }

#if 1
   case rfbSetEncodings:
     {
      int i;
      uint32_t enc;

      n = rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbSetEncodingsMsg - 1);

      if( n <= 0)
       {
        if(n != 0) rfbLogPerror("rfbProcessClientNormalMessage: read");
        rfbCloseClient(cl);
        return;
       }

      msg.se.nEncodings = Swap16IfLE(msg.se.nEncodings);

      cl->preferredEncoding = -1;

      cl->enableCursorPosUpdates = FALSE;

      for(i = 0; i < msg.se.nEncodings; i++)
         {
          if((n = rfbReadExact(cl, (char *)&enc, 4)) <= 0)
            {
             if(n != 0) rfbLogPerror("rfbProcessClientNormalMessage: read");
             rfbCloseClient(cl);
             return;
            }
          enc = Swap32IfLE(enc);

          rfbLog("rfbProcessClientNormalMessage: encoding type request 0x%X\n", (int)enc);

          switch(enc)
            {
             case rfbEncodingPointerPos:
                if(!cl->enableCursorPosUpdates)
                  {
                   rfbLog("Enabling cursor position updates for client %s\n",  cl->host);
                   cl->enableCursorPosUpdates = TRUE;
                   cl->cursorWasMoved = TRUE;
                  }
                break;

             default:
                rfbLog("rfbProcessClientNormalMessage: ignoring unknown "
                       "encoding type 0x%X\n", (int)enc);
            }

         } // end of for

      if(cl->preferredEncoding == -1) cl->preferredEncoding = rfbEncodingRaw;

#if 0
      if((cl->enableCursorPosUpdates&& !cl->enableCursorShapeUpdates))
        {
         rfbLog("Disabling cursor position updates for client %s\n", cl->host);
         cl->enableCursorPosUpdates = FALSE;
        }
#endif

      return;
     } // end of    case rfbSetEncodings:



   case rfbFramebufferUpdateRequest: // dummy read
     rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbFramebufferUpdateRequestMsg - 1);
     return;

   case rfbClientCutText: // dummy, ignored
     rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbClientCutTextMsg - 1);
     return;

   case rfbSetPixelFormat:  // dummy read
     rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbSetPixelFormatMsg - 1);
     return;

   case rfbFixColourMapEntries:  // dummy read
     rfbReadExact(cl, ((char *)&msg) + 1, sz_rfbFixColourMapEntriesMsg - 1);
     return;
#endif



   default:
     rfbLog("rfbProcessClientNormalMessage: Ignored unknown message type 0x%X\n",  msg.type);

/*   rfbLog(" ... closing connection\n");
     rfbCloseClient(cl);  */
     return;

  } // end of  switch(msg.type)


 return;
}





/******************************************************************************************************
 *
 * UDP can be used for keyboard and pointer events when the underlying
 * network is highly reliable.  This is really here to support ORL's
 * videotile, whose TCP implementation doesn't like sending lots of small
 * packets (such as 100s of pen readings per second!).
 */

unsigned char ptrAcceleration = 50;

void
rfbNewUDPConnection(rfbScreen,sock)
    rfbScreenInfoPtr rfbScreen;
    int sock;
{
    if (write(sock, &ptrAcceleration, 1) < 0) {
   rfbLogPerror("rfbNewUDPConnection: write");
    }
}


/******************************************************************************************************
 * Because UDP is a message based service, we can't read the first byte and
 * then the rest of the packet separately like we do with TCP.  We will always
 * get a whole packet delivered in one go, so we ask read() for the maximum
 * number of bytes we can possibly get.
 */

void
rfbProcessUDPInput(rfbScreenInfoPtr rfbScreen)
{
    int n;
    rfbClientPtr cl=rfbScreen->udpClient;
    rfbClientToServerMsg msg;

    if((!cl) || cl->onHold)
      return;

    if ((n = read(rfbScreen->udpSock, (char *)&msg, sizeof(msg))) <= 0) {
   if (n < 0) {
       rfbLogPerror("rfbProcessUDPInput: read");
   }
   rfbDisconnectUDPSock(rfbScreen);
   return;
    }

    switch (msg.type) {

    case rfbKeyEvent:
   if (n != sz_rfbKeyEventMsg) {
       rfbErr("rfbProcessUDPInput: key event incorrect length\n");
       rfbDisconnectUDPSock(rfbScreen);
       return;
   }
   cl->screen->kbdAddEvent(msg.ke.down, (rfbKeySym)Swap32IfLE(msg.ke.key), cl);
   break;


    case rfbPointerEvent:
   if (n != sz_rfbPointerEventMsg) {
       rfbErr("rfbProcessUDPInput: ptr event incorrect length\n");
       rfbDisconnectUDPSock(rfbScreen);
       return;
   }
   cl->screen->ptrAddEvent(msg.pe.buttonMask,
          Swap16IfLE(msg.pe.x), Swap16IfLE(msg.pe.y), cl);
   break;

    default:
   rfbErr("rfbProcessUDPInput: unknown message type %d\n",
          msg.type);
   rfbDisconnectUDPSock(rfbScreen);
    }
}




