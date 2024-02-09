/*
 *  This file is called main.c, because it contains most of the new functions
 *  for use with LibVNCServer.
 *
 *  LibVNCServer (C) 2001 Johannes E. Schindelin <Johannes.Schindelin@gmx.de>
 *  Original OSXvnc (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc (C) 1999 AT&T Laboratories Cambridge.
 *  All Rights Reserved.
 *
 */

/******************************************************************************************************/
#include "vncrfb.h"

#include <stdarg.h>

#ifndef false
#define false 0
#define true -1
#endif


#ifdef LIBVNCSERVER_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif


#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>



#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
MUTEX(logMutex);
#endif

static int rfbEnableLogging = 1;


#ifdef LIBVNCSERVER_WORDS_BIGENDIAN
char rfbEndianTest = 0;
#else
char rfbEndianTest = -1;
#endif

/* from rfbserver.c */
void rfbIncrClientRef(rfbClientPtr cl);
void rfbDecrClientRef(rfbClientPtr cl);


/******************************************************************************************************/
 /* user can enable/disable messages to the screen */
void rfbLogEnable(int enabled)
{
  rfbEnableLogging = enabled;
}



/******************************************************************************************************/
/* rfbLog prints a time-stamped message to the log file (stderr). */
void
rfbDefaultLog(const char *format, ...)
{
    va_list args;
#if 0
    char buf[2560];
    time_t log_clock;
#endif
    if(!rfbEnableLogging) return;

    LOCK(logMutex);

    va_start(args, format);


#if 0
//original, plus time
    time(&log_clock);
    strftime(buf, 255, "%d/%m/%Y %X ", localtime(&log_clock));
    fprintf(stderr,buf);
#endif
// as printf

    vfprintf(stderr, format, args); fflush(stderr);
    va_end(args);

    UNLOCK(logMutex);
}

// set some pointers to functions
 rfbLogProc rfbLog=rfbDefaultLog;
 rfbLogProc rfbErr=rfbDefaultLog;
 rfbLogProc rfbLogPerror=rfbDefaultLog;






#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
/******************************************************************************************************/
#if 0 // not used in Netviz2 fake server
static void *clientOutput(void *data)  // framebuffer updates to a Client
{
 rfbClientPtr cl = (rfbClientPtr)data;
 rfbBool haveUpdate;

 while(1)
  {
   haveUpdate = false;
   while(haveUpdate == false)
    {
     if(cl->sock == -1)
       {
        /* Client has been disconnected. */
        return NULL;
       }

     LOCK(cl->updateMutex);

     haveUpdate = FB_UPDATE_PENDING(cl);

     UNLOCK(cl->updateMutex);

     if(!haveUpdate)
       {
        WAIT(cl->updateCond, cl->updateMutex);
        UNLOCK(cl->updateMutex); /* we really needn't lock now. */
       }

    }

        /* OK, now, to save bandwidth, wait a little while for more updates to come along. */
   usleep(cl->screen->deferUpdateTime * 1000);

        /* Now actually send the update. */
   rfbIncrClientRef(cl);
   rfbDecrClientRef(cl);

  }/* while(1) */

 return NULL;
}
#endif

/******************************************************************************************************/
static void *            //input thread
clientInput(void *data)
{
 rfbClientPtr cl = (rfbClientPtr)data;
 //pthread_t output_thread;
// pthread_create(&output_thread, NULL, clientOutput, (void *)cl);

 cl->IsClientRinning  = TRUE;

 while(1)
  {
   rfbProcessClientMessage(cl);
         /* timeout, client is silent */
   if(cl->sock == -1) break;    /* Client has disconnected, rfbCloseClient() was run already. */
#if 0
   if(cl->screen->ToEnd == TRUE)
     {
      rfbCloseClient(cl);
      break;
     }
#endif
  }

   /* Get rid of the output thread. */
 //LOCK(cl->updateMutex);

 //TSIGNAL(cl->updateCond);

 //UNLOCK(cl->updateMutex);

// IF_PTHREADS(pthread_join(output_thread, NULL));


 cl->IsClientRinning  = FALSE;

 rfbClientConnectionGone(cl);

// return NULL;

// printf ("\t\txxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Client thread %p exit\n", cl);
 pthread_exit(NULL);
 return (void*) NULL;
}



/******************************************************************************************************/
// this is called when user invokes rfbRunEventLoop(..., ..., TRUE)
static void* listenerRun(void *data)
{
 int client_fd;  //sock
 struct sockaddr_in peer;
 rfbClientPtr cl;
 socklen_t len;

 rfbScreenInfoPtr screen = (rfbScreenInfoPtr)data;

 screen->listenerPid = getpid();
 //rfbLog("\r\n Listener pid is %u", screen->listenerPid);



 len = sizeof(peer);



 screen->ListenerRuns = true;

	 /* TODO: this thread won`t die by restarting the server */
 while((client_fd = accept(screen->listenSock, (struct sockaddr*)&peer, &len)) >= 0)
    {
	  // listen for new clients, and create new thread for every client
     cl = rfbNewClient(screen,client_fd);

     cl->screen = screen;
// this is in vncserver.c:
//rfbClientPtr rfbNewClient(rfbScreenInfoPtr rfbScreen, int sock)
 // return(rfbNewTCPOrUDPClient(rfbScreen, sock, FALSE));

     len = sizeof(peer);
     if (!cl->onHold && (screen->ToEnd == FALSE) ) rfbStartOnHoldClient(cl); //create input thread

    }


 // pass through all input threads and terminate them. Now this is simulated with a simple wait

 usleep(10000);
rfbLog("\r\n screen->ListenerRuns = FALSE;\r\n");
 screen->ListenerRuns = FALSE;
 pthread_exit(NULL);
// return(NULL);
}


/******************************************************************************************************/
void
rfbStartOnHoldClient(rfbClientPtr cl)
{
	// Client thread is default JOINABLE --> global resources are not released upon pthread_exit
	// Thread needs to be DETACHABLE
	pthread_attr_t attr = {{0}};
	pthread_attr_init(&attr);
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
	{
		printf ("Error pthread_attr_setdetachstate !\n");
	}


	if (pthread_create(&cl->client_thread, &attr, clientInput, (void *)cl) != 0)
	{
		printf("Error creating client thread !\n");
	}
	pthread_attr_destroy(&attr);

//	printf("\t\txxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Client thread created \n");
}

#else

   //no threads
/******************************************************************************************************/
void
rfbStartOnHoldClient(rfbClientPtr cl)
{
   cl->onHold = FALSE;
}

#endif


/******************************************************************************************************/
void
rfbRefuseOnHoldClient(rfbClientPtr cl)
{
    rfbCloseClient(cl);
    rfbClientConnectionGone(cl);
}



/******************************************************************************************************/
static void
rfbDefaultKbdAddEvent(rfbBool down, rfbKeySym keySym, rfbClientPtr cl)
{

}


/******************************************************************************************************/
void
rfbDefaultPtrAddEvent(int buttonMask, int x, int y, rfbClientPtr cl)
{
#if 0

  rfbClientIteratorPtr iterator;
  rfbClientPtr other_client;

  if (x != cl->screen->cursorX || y != cl->screen->cursorY) {
    LOCK(cl->screen->cursorMutex);
    if (!cl->screen->cursorIsDrawn) {
      cl->screen->cursorX = x;
      cl->screen->cursorY = y;
    }
    UNLOCK(cl->screen->cursorMutex);

    /* The cursor was moved by this client, so don't send CursorPos. */
    if (cl->enableCursorPosUpdates)
      cl->cursorWasMoved = FALSE;

    /* But inform all remaining clients about this cursor movement. */
    iterator = rfbGetClientIterator(cl->screen);
    while ((other_client = rfbClientIteratorNext(iterator)) != NULL) {
      if (other_client != cl && other_client->enableCursorPosUpdates) {
   other_client->cursorWasMoved = TRUE;
      }
    }
    rfbReleaseClientIterator(iterator);
  }

#endif
}



/******************************************************************************************************/
void rfbDoNothingWithClient(rfbClientPtr cl)
{
/* dummy function, used when the client is disconnected */
}


/******************************************************************************************************/
enum rfbNewClientAction rfbDefaultNewClientHook(rfbClientPtr cl)
{
   return RFB_CLIENT_ACCEPT;
}








/******************************************************************************************************/
rfbScreenInfoPtr rfbGetScreen(int* argc,char** argv,
 int width,int height,int bitsPerSample,int samplesPerPixel,
 int bytesPerPixel)
{


   rfbScreenInfoPtr screen=malloc(sizeof(rfbScreenInfo));
rfbLog("\r\n +++malloc rfbScreenInfoPtr \r\n");
   INIT_MUTEX(logMutex);

   if(width&3)
     rfbErr("WARNING: Width (%d) is not a multiple of 4. VncViewer has problems with that.\n",width);

   screen->autoPort=FALSE;
   screen->clientHead=0;
   screen->port=5000; /*5900;*/
   screen->socketInitDone=FALSE;

   screen->inetdInitDone = FALSE;
   screen->inetdSock=-1;

   screen->udpSock=-1;
   screen->udpSockConnected=FALSE;
   screen->udpPort=0;
   screen->udpClient=0;

   screen->maxFd=0;
   screen->listenSock=-1;

   screen->httpInitDone=FALSE;
   screen->httpEnableProxyConnect=FALSE;
   screen->httpPort=0;
   screen->httpDir=NULL;
   screen->httpListenSock=-1;
   screen->httpSock=-1;

   screen->desktopName = "screen->desktopName=\" \"";
   screen->alwaysShared = FALSE;
   screen->neverShared = FALSE;
   screen->dontDisconnect = FALSE;

   screen->width = width;
   screen->height = height;
   screen->bitsPerPixel = screen->depth = 8 * bytesPerPixel;

   screen->ignoreSIGPIPE = TRUE;


   gethostname(screen->thisHost, 255);


   screen->paddedWidthInBytes = width * bytesPerPixel;
   screen->serverFormat.bitsPerPixel = screen->bitsPerPixel;

   screen->serverFormat.depth = screen->depth;

#if 0
// ====================== orig
    rfbPixelFormat *format=&screen->serverFormat;
   format->bitsPerPixel = screen->bitsPerPixel;
   format->depth = screen->depth;
   //format->bigEndian = rfbEndianTest?FALSE:TRUE;
   //format->trueColour = TRUE;
   //screen->colourMap.count = 0;
   //screen->colourMap.is16 = 0;
   //screen->colourMap.data.bytes = NULL;


   if (format->bitsPerPixel == 8) {
     format->redMax = 7;
     format->greenMax = 7;
     format->blueMax = 3;
     format->redShift = 0;
     format->greenShift = 3;
     format->blueShift = 6;
   } else {
     format->redMax = (1 << bitsPerSample) - 1;
     format->greenMax = (1 << bitsPerSample) - 1;
     format->blueMax = (1 << bitsPerSample) - 1;
     if(rfbEndianTest) {
       format->redShift = 0;
       format->greenShift = bitsPerSample;
       format->blueShift = bitsPerSample * 2;
     } else {
       if(format->bitsPerPixel==8*3) {
	 format->redShift = bitsPerSample*2;
	 format->greenShift = bitsPerSample*1;
	 format->blueShift = 0;
       } else {
	 format->redShift = bitsPerSample*3;
	 format->greenShift = bitsPerSample*2;
	 format->blueShift = bitsPerSample;
       }
     }
   }
#endif
// =============================================




   /* cursor */
   screen->cursorIsDrawn = TRUE;
   screen->cursorX = 0;
   screen->cursorY = 0;

   screen->ToEnd = FALSE;
   screen->ListenerRuns = FALSE;

   INIT_MUTEX(screen->cursorMutex);

   IF_PTHREADS(screen->backgroundLoop = FALSE);

   // wait 5msec before sending an update to a client
   screen->deferUpdateTime = 5;

   /* proc's and hook's KIBO : our functions are here */

   screen->kbdAddEvent = rfbDefaultKbdAddEvent;
   screen->ptrAddEvent = rfbDefaultPtrAddEvent;


   screen->newClientHook = rfbDefaultNewClientHook;

   screen->listenerPid = (pid_t)0;

   /* initialize client list and iterator mutex */
   rfbClientListInit(screen);

   return(screen);
}




/******************************************************************************************************/
void rfbInitServer(rfbScreenInfoPtr screen)
{
#ifdef WIN32
  WSADATA trash;
  int i=WSAStartup(MAKEWORD(2,2),&trash);
#endif
  rfbInitSockets(screen);
#ifdef LIBVNCSERVER_HAVE_HTTP
  rfbHttpInitSockets(screen);
#endif
  if(screen->ignoreSIGPIPE)
    signal(SIGPIPE,SIG_IGN);
}

#ifndef LIBVNCSERVER_HAVE_GETTIMEOFDAY
#include <fcntl.h>
#include <conio.h>
#include <sys/timeb.h>

/******************************************************************************************************/
void gettimeofday(struct timeval* tv,char* dummy)
{
   SYSTEMTIME t;
   GetSystemTime(&t);
   tv->tv_sec=t.wHour*3600+t.wMinute*60+t.wSecond;
   tv->tv_usec=t.wMilliseconds*1000;
}
#endif





/******************************************************************************************************/
void rfbScreenCleanup(rfbScreenInfoPtr screen)
{
 rfbClientIteratorPtr i = rfbGetClientIterator(screen);

 rfbClientPtr cl, cl1=rfbClientIteratorNext(i);

 while(cl1)
    {
     cl=rfbClientIteratorNext(i);
     rfbClientConnectionGone(cl1);
     cl1=cl;
    }

 rfbReleaseClientIterator(i);

  /* TODO: hang up on all clients and free all reserved memory */


  TINI_MUTEX(screen->rfbClientListMutex);   //ddd6

  TINI_MUTEX(screen->cursorMutex);

  if(screen->listenerPid)
    {
    // rfbLog("\r\n killing listener pid %u", screen->listenerPid);
     if(kill(screen->listenerPid, SIGHUP))
        rfbLog("\r\n failed to kill listener thread");
    }


rfbLog("\r\n ---free rfbScreenInfoPtr in rfbScreenCleanup() \r\n");
  free(screen);

}










/* defined in rfbserver.c, but kind of "private" */
rfbClientPtr rfbClientIteratorHead(rfbClientIteratorPtr i);


/******************************************************************************************************/
 // this function is used when threads are not used
void rfbProcessEvents(rfbScreenInfoPtr screen,long usec)
{
 rfbClientIteratorPtr i;
 rfbClientPtr cl, clPrev;

 //struct timeval tv;

 if(usec < 0) usec = screen->deferUpdateTime * 1000;

 rfbCheckFds(screen, usec);   // it invokes rfbProcessClientMessage
 rfbHttpCheckFds(screen);

 i = rfbGetClientIterator(screen);
 cl = rfbClientIteratorHead(i);

 while(cl)
  {
#if 0
    if(cl->sock >= 0 && !cl->onHold && FB_UPDATE_PENDING(cl))
      {
       if(screen->deferUpdateTime == 0)
         { }
       else
       if(cl->startDeferring.tv_usec == 0)
         {
          gettimeofday(&cl->startDeferring, NULL);
          if(cl->startDeferring.tv_usec == 0) cl->startDeferring.tv_usec++;
         }
       else
        {
         gettimeofday(&tv, NULL);
         if(tv.tv_sec < cl->startDeferring.tv_sec /* at midnight */
         || ((tv.tv_sec-cl->startDeferring.tv_sec)*1000 + (tv.tv_usec-cl->startDeferring.tv_usec)/1000) > screen->deferUpdateTime)
           {
            cl->startDeferring.tv_usec = 0;
           }
        }

      }
#endif
    clPrev = cl;
    cl = rfbClientIteratorNext(i);

    if(clPrev->sock == -1) rfbClientConnectionGone(clPrev);

  }

  rfbReleaseClientIterator(i);
}


/******************************************************************************************************/
void rfbRunEventLoop(rfbScreenInfoPtr screen, long usec, rfbBool runInBackground)
{
 if(runInBackground)
   {
#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    pthread_t listener_thread;
    screen->backgroundLoop = TRUE;
    pthread_create(&listener_thread, NULL, listenerRun, screen);
    return;

#else

    rfbErr("Can't run in background, because I don't have PThreads!\n");
    return;
#endif
   }


 if(usec < 0) usec = screen->deferUpdateTime * 1000;

 while(screen->ToEnd != TRUE) rfbProcessEvents(screen, usec);

}
