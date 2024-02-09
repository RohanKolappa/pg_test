#include "v2d.h"
#include "common.h"

#ifdef WINDOWS
#define usleep Sleep

int windows_gettimeofday(struct timeval* tp, int* tz)
{
    struct timeb tb;
    ftime(&tb);
    tp->tv_sec = (long)tb.time;
    tp->tv_usec = 1000*tb.millitm;
    return 0;
}
#endif

#ifdef WINDOWS
   #define LOCAL_GETTIMEOFDAY windows_gettimeofday
#else
   #define LOCAL_GETTIMEOFDAY(timevalp, tz) gettimeofday((timevalp), (tz))
#endif



#define MAXREQSIZE  1024
const char * const ResponseFmt=
				"HTTP/1.0 200 OK\n"
				"Content-Type: text/html\n"
				"Content-Length: 0\n"
				"\r\n"
				"<HTML>\n"
				"No Matching objects found \n"
				"<br>%s<br>\n"
				"<br>%d<br>\n"
				"</HTML>\n";

typedef struct hdr
{
	unsigned long long h_nTimestamp;
	int		   h_nFlags;
	int		   h_nLen;
} hdr_t;
typedef struct clients
{
    int     c_nSocket;
    char    c_sRequest[MAXREQSIZE+1];
    char    c_sResponse[MAXREQSIZE+1];
} client_t;

int SetupHTTPListener(int nHTTPRequestPort, client_t *hClients)
{
    struct sockaddr_in sa;
    int nHTTPSocket;
    int sockoptval=1;
    int i=0;

    for (i=0; i < 3; i++) {
        memset ((void *)&hClients[i], 0, sizeof (client_t));
    }

    /***************************
    * Bind to TCP port        *
    ***************************/
    sa.sin_family = AF_INET;
    sa.sin_port = htons(nHTTPRequestPort);
    memset(sa.sin_zero, 0, sizeof (sa.sin_zero));
    sa.sin_addr.s_addr = INADDR_ANY;

    if ((nHTTPSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("Error creating socket");
        return -1;
    }

    if(setsockopt(nHTTPSocket, SOL_SOCKET, SO_REUSEADDR,
                  &sockoptval, sizeof (sockoptval)) < 0) {
        perror ("Error setting socket options");
        close(nHTTPSocket);
        nHTTPSocket = 0;
        return -1;
    }

    /********************************************
    * Can only try to bind to requested port.  *
    ********************************************/
    if (bind(nHTTPSocket, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror ("Cannot bind");
        close(nHTTPSocket);
        nHTTPSocket = 0;
        return -1;
    }
    if (listen (nHTTPSocket, 5) == -1) {
        perror ("Listen Failed");
        close(nHTTPSocket);
        nHTTPSocket = 0;
        return -1;
    }
    return nHTTPSocket;
}
int ProcessHTTPRequests(int nHTTPSocket, client_t *hClients, V2DTx_t *s)
{
    struct sockaddr_in sPeer;
    socklen_t nPeerlen=sizeof(sPeer);
    fd_set read_fdset, write_fdset;
    int nDescriptors, nMaxfd=-1;
    int nRequestSock=0;
    struct timeval timeout;
    int retval=-1;
    int i=0;
    char *pRequestString=NULL;
    char *pResponseString=NULL;
    int len=0;
    int nSocket=0;

    if (nHTTPSocket <= 0)
    goto EXIT_LABEL;
    /* Get ready for select */
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    /* Add main socket to read fdset */
    FD_SET (nHTTPSocket, &read_fdset);
    if (nMaxfd <= nHTTPSocket)
        nMaxfd = nHTTPSocket+1;


    /* Add client sockets to read and write fdsets. All clients
    get added to read fdset. If any client has a result pending to
    be sent out then it is added to the write fdset */
    for (i=0; i < 3; i++) {
        if (hClients[i].c_nSocket == 0) continue;
        if (strlen (hClients[i].c_sRequest) != 0) continue;
        nSocket = hClients[i].c_nSocket;
        FD_SET (nSocket, &read_fdset);
        if (nMaxfd <= nSocket)
            nMaxfd = nSocket+1;
    }
    for (i=0; i < 3; i++) {
        if (hClients[i].c_nSocket == 0) continue;
        if (strlen (hClients[i].c_sRequest) == 0) continue;
        if (strlen (hClients[i].c_sResponse) == 0) continue;
        nSocket = hClients[i].c_nSocket;
        FD_SET (nSocket, &write_fdset);
        if (nMaxfd <= nSocket)
            nMaxfd = nSocket+1;
    }

    /* Call select */
    nDescriptors = select(nMaxfd+1, &read_fdset, &write_fdset,
                            NULL, &timeout);
    if (nDescriptors <= 0) {
        goto EXIT_LABEL; // nothing to do
    }

    /* If activity on main socket, add the new client into our list of
    * active clients */
    if (FD_ISSET(nHTTPSocket, &read_fdset)) {
        nRequestSock = accept (nHTTPSocket,
                               (struct sockaddr *)&sPeer, &nPeerlen);
        if (nRequestSock > 0) {
            for (i=0 ; i < 3; i++) {
                if (hClients[i].c_nSocket==0) {
                    hClients[i].c_nSocket = nRequestSock;
                    pRequestString = hClients[i].c_sRequest;
                    memset(pRequestString, 0, MAXREQSIZE+1);
                    pResponseString = hClients[i].c_sResponse;
                    memset(pResponseString, 0, MAXREQSIZE+1);
                    break;
                }
            }
        }
    }
    for (i=0 ; i < 3; i++) {
        pRequestString = hClients[i].c_sRequest;
        if (strlen (pRequestString) > 0) continue;
        nRequestSock = hClients[i].c_nSocket;
        if (!nRequestSock) continue;
        if (FD_ISSET(nRequestSock, &read_fdset)) {
            if ((len = recv(nRequestSock,pRequestString,
                            MAXREQSIZE - 32,0)) > 0) {
                printf ("%s\n", pRequestString);
            }
            pResponseString = hClients[i].c_sResponse;
            sprintf (pResponseString, ResponseFmt, "OK", 200);
        }
    }
    for (i=0 ; i < 3; i++) {
        pResponseString = hClients[i].c_sResponse;
        if (strlen (pResponseString)==  0) continue;
        nRequestSock = hClients[i].c_nSocket;
        if (!nRequestSock) continue;
        if (FD_ISSET(nRequestSock, &write_fdset)) {
            pResponseString = hClients[i].c_sResponse;
            //send (nRequestSock, (void *)pResponseString,
                        			//MAXREQSIZE, 0);
            memset(pResponseString, 0, MAXREQSIZE+1);
            switch(i)
            {
                case 0:
                    Tx_SetCtlSocket(s, hClients[i].c_nSocket, 1);
                       break;
                case 1:
                    Tx_SetVideoSocket(s, hClients[i].c_nSocket);
                       break;
                case 2:
                    Tx_SetAudioSocket(s, hClients[i].c_nSocket);
                       break;
            }
        }
    }
    for (i=0 ; i < 3; i++) {
        if (strlen (hClients[i].c_sRequest) == 0) break;
        if (strlen (hClients[i].c_sResponse) != 0) break;
    }
EXIT_LABEL:
    return retval;
}
int Usage()
{
 	printf("Usage: tx -p port -i media [optional-arguments]\n");
	printf("\t-p Value\tUse server port number 'Value'\n");
	printf("\t-i Value\tSet media directory to value\n");
 	printf("Optional arguments:\n");
	printf("\t-M Value\t\tSet the Multicast address 'Value'\n");
	printf("\t-P Value\tUse HTTP port number 'Value'\n");
	printf("\t-d Value\tSet debug mode to value\n");
	printf("\t-h\t\tPrint this message\n");
    exit(0);
    return 0;
}



int main(int argc, char **argv)
{
	char pStatBuffer[240], pFileName[80], pMediaName[80], pMCastAddr[32];
	struct timeval tNow, tPrev;
	int delta, opt;
	int nPort, nTTL;
	V2DTx_t *s=NULL;

    /// HTTP Listener Parameters
    int nHTTPRequestPort=0;
    int nHTTPSocket=0;
    client_t hClients[3];

	/// Disk IO Headers
	hdr_t	hdr;
	int status=0;
	int hdrlen;
	int m_nFile;
	unsigned char m_pData[AV_BUFFER_SIZE+1];
	struct timeval	 ts;
	unsigned long long m_nSessionStartTime, m_nTimestamp, m_nStartTime;
	unsigned long long m_nFrameSequence=0;
	int nDifference;
	int m_nIndex=-1;
	int retval=0;
	int bGotCtrl=0;
	int nOffset=0;
    unsigned long long currentTime;
#ifdef WINDOWS
    int i=1;
#endif

    // Disk IO Initialization
    memset (m_pData, 0, AV_BUFFER_SIZE+1);
	m_nFile = 0;
	hdrlen = sizeof(hdr_t);

	memset (pFileName, 0, 80);
	memset (pMediaName, 0, 80);
	nPort=6060;
        strcpy(pMCastAddr, "226.1.1.1");
	nTTL = 1;

#ifdef WINDOWS
    while ( i < argc) {

      if ( !strcmp(argv[i], "-M"))
      {
        strncpy(pMCastAddr, (argv[i+1]), sizeof(pMCastAddr));
        i+= 2;
      }
      else if (!strcmp(argv[i], "-p"))
      {
        nPort=atoi(argv[i+1]);
        i+= 2;
      }
      else if (!strcmp(argv[i], "-P"))
      {
        nHTTPRequestPort=atoi(argv[i+1]);
        i+= 2;
      }
      else if (!strcmp(argv[i], "-d"))
      {
        doDebug=atoi(argv[i+1]);
        i+= 2;
      }
      else if (!strcmp(argv[i], "-i"))
      {
       strncpy(pMediaName, argv[i+1], sizeof(pMediaName));
        i+= 2;
      }
      else if (!strcmp(argv[i], "-h"))
      {
       Usage();

      }
    }

#else
        while ((opt = getopt(argc, argv, "M:hp:i:d:P:")) > 0) {
        	switch (opt) {
 		case 'M':
			strncpy(pMCastAddr, optarg, sizeof(pMCastAddr));
			break;
		case 'p':
			nPort=atoi(optarg);
 			break;
		case 'P':
			nHTTPRequestPort=atoi(optarg);
 			break;
		case 'd':
			doDebug=atoi(optarg);
			break;
		case 'i':
			strncpy(pMediaName, optarg, sizeof(pMediaName));
 			break;
		case 'h':
            Usage();
		}
	}
#endif
	if (!doDebug) {
		//if (fork()) exit(0);
	}
	s = Tx_Create(nPort, nTTL, pMCastAddr, 0, 0, NULL, NULL);
	if (!s) {
		printf("Could not create session\n");
		exit(0);
	}

    LOCAL_GETTIMEOFDAY(&tNow, NULL);
    tPrev.tv_sec = tNow.tv_sec;
	tPrev.tv_usec = tNow.tv_usec;

    if (nHTTPRequestPort) {
        hClients[0].c_nSocket = 0;
        hClients[1].c_nSocket = 0;
        hClients[2].c_nSocket = 0;
        nHTTPSocket = SetupHTTPListener(nHTTPRequestPort, hClients);
        if (nHTTPSocket < 0) exit(0);
    }

	while(s->m_nTxState != CLIENT_STATE_READY)
	{
        ProcessHTTPRequests(nHTTPSocket, hClients,s);
        LOCAL_GETTIMEOFDAY(&ts, NULL);
        currentTime =
            ((unsigned long long) ts.tv_sec) * 1000 +
            (ts.tv_usec/1000);
        v2dLibSetTime(currentTime);

		retval = Tx_Go(s);
		if (retval < 0) break;
		if (retval == 0) {
			nano_sleep(1);
		}
	}
	LOG_SYS ( "Begin: %s:%d \n", pMediaName, nPort);
    LOCAL_GETTIMEOFDAY(&ts, NULL);

	m_nSessionStartTime =
			((unsigned long long) ts.tv_sec) *1000 +
			(ts.tv_usec/1000);
	m_nTimestamp = 0;
	m_nStartTime = 0;
	while(1) {
		retval=0;
        LOCAL_GETTIMEOFDAY(&ts, NULL);

        currentTime =
            ((unsigned long long) ts.tv_sec) * 1000 +
            (ts.tv_usec/1000);
        v2dLibSetTime(currentTime);

		nOffset = 0;
		nDifference =
			(((unsigned long long) ts.tv_sec) *1000 +
			(ts.tv_usec/1000) - m_nSessionStartTime ) -
			(m_nTimestamp - m_nStartTime);
		if (nDifference < 0) {
			goto EXIT_LABEL;
		}
		if (s->m_nTxState != CLIENT_STATE_READY) break;
		if (s->m_nFlag & O_CTL) goto EXIT_LABEL;
		if (s->m_nFlag & O_VIDEO) goto EXIT_LABEL;

		if (!m_nFile) {
			m_nIndex++;
			sprintf (pFileName, "%s/%d", pMediaName, m_nIndex);
			LOG_SYS( "Open: %s:%d %s\n",
				pMediaName, nPort, pFileName);
			m_nFile = open (pFileName, O_RDONLY);
		}
		if (m_nFile <= 0) {
			printf("Can't open %s. %m\n", pFileName);
			break;
		}
		status = read (m_nFile, (void *) &hdr, hdrlen);
		if (status != hdrlen) {
			close(m_nFile);
			m_nFile=0;
			goto EXIT_LABEL;
		}
		while (!bGotCtrl) {
			status = read (m_nFile, m_pData, hdr.h_nLen);
			status = read (m_nFile, (void *) &hdr, hdrlen);
			if (status != hdrlen) {
				close(m_nFile);
				m_nFile=0;
				goto EXIT_LABEL;
			}
			if (hdr.h_nFlags & O_CTL) {
				bGotCtrl=1;
				break;
			}
		}
		if (hdr.h_nLen == 1032 && (hdr.h_nFlags & O_VIDEO)) {
			nOffset = 8;
		}

		m_nTimestamp = hdr.h_nTimestamp;
		if (m_nFrameSequence == 0) {
			m_nStartTime = m_nTimestamp;
		}
		m_nFrameSequence++;
		status = read (m_nFile, &m_pData[nOffset], hdr.h_nLen);
		if (hdr.h_nFlags & O_CTL) {
			s->m_nFlag |= O_CTL;
			memcpy (s->m_CtlSnd, m_pData, hdr.h_nLen);
		}
		if (hdr.h_nFlags & O_VIDEO) {
			s->m_nFlag |= O_VIDEO;
			s->m_AV->m_VideoSndPtr = (unsigned char *)m_pData;
                        s->m_AV->m_nVideoLen = hdr.h_nLen + nOffset;
		}
		if (hdr.h_nFlags & O_AUDIO) {
			s->m_nFlag |= O_AUDIO;
			s->m_AV->m_AudioSndPtr = (unsigned char *)m_pData;
                        s->m_AV->m_nAudioLen = hdr.h_nLen;
		}
		if (status != hdr.h_nLen) {
			close(m_nFile);
			m_nFile=0;
			goto EXIT_LABEL;
		}
		s->m_nFlag &= ~(I_KB|I_MS|I_SER);
		retval=Tx_Go(s);
        ProcessHTTPRequests(nHTTPSocket, hClients,s);
EXIT_LABEL:
		if (retval == 0 || s->m_nTxState == CLIENT_STATE_IDLE ||
				s->m_nTxState == CLIENT_STATE_DISABLED) {
			nano_sleep(1);
		}
		/// Get Error and Notifications
		if (s->m_bSendError) {
			Tx_SendError(s, pStatBuffer, 160);
			LOG_SYS( "Error: %s:%d %s\n",
				pMediaName, nPort, pStatBuffer);
		}
		if (s->m_bSendNotify) {
			Tx_SendNotification(s, pStatBuffer, 160);
			LOG_SYS( "Notification: %s:%d %s\n",
				pMediaName, nPort, pStatBuffer);
		}
		/// Get Statistics

         LOCAL_GETTIMEOFDAY(&tNow, NULL);
         delta = ((tNow.tv_sec - tPrev.tv_sec) *1000)  +
			((tNow.tv_usec - tPrev.tv_usec)/1000 );
		if (delta < 1000)  continue;

		tPrev.tv_sec = tNow.tv_sec;
		tPrev.tv_usec = tNow.tv_usec;
		Tx_PrintStatistics(s, pStatBuffer, 160);
		LOG_SYS( "TxStat: %s:%d %s\n",
			pMediaName, nPort, pStatBuffer);
	}

	Tx_Destroy(s);

        return 0;
}
