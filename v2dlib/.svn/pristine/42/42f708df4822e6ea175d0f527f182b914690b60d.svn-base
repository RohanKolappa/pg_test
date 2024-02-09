#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "v2d.h"
#include "v2dcommon.h"

int g_nUnicast;

//#define DEBUG

#ifdef    DEBUG
static void dump(unsigned char *buf, int len) {
    int i, j;
    for (i = 0; i < len; i += 16) {
        for (j = 0; j < 16 && (i + j) < len; j++)
            printf("0x%02x ", buf[i + j]);
        printf("\n");
    }
}
#endif


static void print_help(void) {
    printf("Usage: relay -s IP_ADDR [optional-argrments]\n");
    printf("Optional arguments:\n");
    printf("\t-a 1|2|3\t\tSet avoption to video, audio or both\n");
    printf("\t-m 1|0\t\tMake multicast connection if 1 else 0\n");
    printf("\t-M Addr\t\tUse multicast address 'Addr'\n");
    printf("\t-p Value\tUse server port number 'Value'\n");
    printf("\t-P Value\tUse AV port number 'Value'\n");
    printf("\t-b Value\tSet bit rate to 'Value'\n");
    printf("\t-k Value\tSet keyboard/mouse switch timeout to 'Value'\n");
    printf("\t\twhere Value can be 255 (don't switch) or 0-30 (secs)\n");
    printf("\t-h\t\tPrint this message\n");
}

int main(int argc, char **argv)
{
    char pStatBuffer[240], pHostname[32];
    char pMcastAddr[32];
    struct timeval tNow, tPrev;
    int delta, opt;
    int bMulticast, nAVOption, nBitrate, nSvrPort, nVPort, nAPort, nKMIdle;
    V2DTx_t *pTxSession=NULL, *pTxSession2=NULL, *activeKMSession;
    V2DRx_t *pRxSession=NULL;
    V2DAv_t *t;
    int i, nLen=0;
    struct timeval ts, lastKMTime;
    long elapsed_usecs;
    int retval, rc, tx1connect=0, tx2connect=0;
    unsigned char videobuffer[200*1048];
    unsigned char videobuffer2[200*1048];
    unsigned char audiobuffer[200*1048];
    unsigned char audiobuffer2[200*1048];

    pHostname[0] = '\0';
    bMulticast=FALSE;
    nAVOption=AVOPTION_VIDEO;
    nBitrate=1000000;
    nSvrPort=6060;
    nVPort=6060;
    strcpy((char *) pMcastAddr, "226.1.1.1");
    nKMIdle=255;

    while ((opt = getopt(argc, argv, "s:h:a:p:b:m:M:k:")) > 0) {
        switch (opt) {
        case 'm':
             bMulticast=atoi(optarg);
             break;
        case 'a':
             nAVOption=atoi(optarg);
             break;
        case 'M':
             strncpy(pMcastAddr, optarg, 32);
             break;
	case 'k':
	     nKMIdle=atoi(optarg);
	     break;
        case 'p':
             nSvrPort=atoi(optarg);
             break;
        case 'P':
             nVPort=atoi(optarg);
             break;
        case 'b':
             nBitrate=atoi(optarg);
             break;
        case 's':
             strncpy(pHostname, optarg, sizeof(pHostname)); 
             break;
        case 'h':
             print_help();
            return 0;
        }
    }
    if (pHostname[0] == '\0') {
        print_help();
        return -1;
    }
    if (nAVOption < 1 || nAVOption > 3)
        nAVOption = AVOPTION_VIDEO;
    if (nKMIdle < 0 || nKMIdle > 30)
        nKMIdle = 255;
    nAPort = nVPort + 1;

    printf("%s: svr port %d, mcast addr %s, av port %d/%d, av option %d\n",
        __FUNCTION__, nSvrPort, pMcastAddr, nVPort, nAPort, nAVOption);

    g_nUnicast = 0;

    pTxSession=Tx_Create(7070, 1, pMcastAddr, 0, 0, NULL, NULL);
    if (pTxSession == NULL) {
        printf("Failed to create TX session. errno %d.\n", errno); 
	exit(0);
    }
    else 
        printf("First Tx session created. Waiting for connection on 7070\n");


    pTxSession2=Tx_Create(7071, 1, pMcastAddr, 0, 0, NULL, NULL);
    if (pTxSession2 == NULL) 
        printf("Failed to create second TX session. errno %d.\n", errno); 
    else 
        printf("Second Tx session created. Waiting for connection on 7071\n");

    gettimeofday (&tPrev, NULL);
    activeKMSession = pTxSession; 
    lastKMTime.tv_sec = tPrev.tv_sec;
    lastKMTime.tv_usec = tPrev.tv_usec;

    while (1) {

	retval = 0;

        rc=Tx_Go(pTxSession);
        if (rc < 0) {
            printf("pTxSession returned code %d, errno %d\n",
			((-rc) & 0xff00) >> 8, (-rc) & 0x00ff);
            if (Tx_GetCreqType(pTxSession) == CREQ_TYPE_UNICAST)
                g_nUnicast = 0;
            tx1connect = 0;
            if (pRxSession && tx2connect == 0) {
                Rx_Destroy(pRxSession); 
                pRxSession = NULL;
            }
            continue;
        }
	else if (tx1connect==0 && pTxSession->m_nSvrState==SERVER_STATE_READY){
                tx1connect=1;
    		printf("Connected to %s on port %d\n", 
				inet_ntoa(pTxSession->m_Destination.sin_addr), 
				ntohs(pTxSession->m_Destination.sin_port));
        }
	retval += rc;

        rc=Tx_Go(pTxSession2);
        if (rc < 0) {
            printf("pTxSession2 returned code %d, errno %d\n",
			((-rc) & 0xff00) >> 8, (-rc) & 0x00ff);
            if (Tx_GetCreqType(pTxSession2) == CREQ_TYPE_UNICAST)
                g_nUnicast = 0;
            tx2connect = 0; 
            if (pRxSession && tx1connect == 0) {
                Rx_Destroy(pRxSession); 
                pRxSession = NULL;
            }
            continue;
        }
	else if (tx2connect==0 && pTxSession2->m_nSvrState==SERVER_STATE_READY){
                tx2connect = 1;
    		printf("Connected to %s on port %d\n", 
				inet_ntoa(pTxSession2->m_Destination.sin_addr), 
				ntohs(pTxSession2->m_Destination.sin_port));
        }
	retval += rc;

        if (pRxSession == NULL && (tx1connect || tx2connect)) {
            pRxSession=Rx_Create(pHostname, 
                                 nSvrPort, 
                                 nBitrate, 
                                 bMulticast, 
                                 nVPort, 
				 nAPort,
                                 nAVOption, 
                                 AUDIOTYPE_DEFAULT,
                                 0);

            if (pRxSession == NULL) {
                printf("Failed to create RX session. errno %d.\n", errno); 
	        exit(0);
            }
        }

        if (!pRxSession) continue;

        if (pRxSession->m_nCtlSock == 0) {
            Rx_OpenMediaConnection(pRxSession);
        }
        rc = Rx_Go(pRxSession);
        if (rc < 0) {
            Rx_CloseMediaConnection(pRxSession);
            printf("Program exiting with code %d, errno %d\n",
			((-rc) & 0xff00) >> 8, (-rc) & 0x00ff);
            break;
        }
	retval += rc;

        if (pRxSession->m_nFlag & (I_CTL|I_SER)) {
            nLen = htons(*(unsigned short *)pRxSession->m_CtlRcv);
            if (pRxSession->m_nFlag & I_CTL) {
                if (pTxSession->m_nSvrState==SERVER_STATE_READY) { 
                    memcpy(pTxSession->m_CtlSnd, pRxSession->m_CtlRcv, nLen);
                    pTxSession->m_nFlag |= O_CTL;
                }        
                if (pTxSession2->m_nSvrState==SERVER_STATE_READY) {
                    memcpy(pTxSession2->m_CtlSnd, pRxSession->m_CtlRcv, nLen);
                    pTxSession2->m_nFlag |= O_CTL;
                }        
                pRxSession->m_nFlag &= ~I_CTL;
            }
            if ((pRxSession->m_nFlag & I_SER) &&
				!(activeKMSession->m_nFlag & O_CTL)) {
                memcpy(activeKMSession->m_CtlSnd, pRxSession->m_CtlRcv, nLen);
                activeKMSession->m_nFlag |= O_CTL;
                pRxSession->m_nFlag &= ~I_SER;
            }
        }

	for (i = 0; i < pTxSession->m_nNumCreq; i++) {
		if (pTxSession->m_Creq[i].name==AVREQ_IDENT_AV_OPTION) {
			nAVOption = atoi(pTxSession->m_Creq[i].value);
			break;
		}
	}

        if ((nAVOption!=AVOPTION_AUDIO) &&
			(pRxSession->m_nFlag & I_VIDEO) &&
			!(pTxSession->m_nFlag & O_VIDEO)) {
            nLen=pRxSession->m_nVideoLen;
            if (pTxSession->m_bControlling && pTxSession->m_AV) {
                t = pTxSession->m_AV;
                memcpy (videobuffer, pRxSession->m_VideoRcv, nLen);
                t->m_VideoSndPtr = videobuffer;
            }
            if (pTxSession2->m_bControlling && pTxSession2->m_AV) {
                t = pTxSession2->m_AV;
                memcpy (videobuffer2, pRxSession->m_VideoRcv, nLen);
                t->m_VideoSndPtr = videobuffer2;
            }
            if (pTxSession->m_bControlling) 
                pTxSession->m_nFlag |= O_VIDEO;
            if (pTxSession2->m_bControlling) 
                pTxSession2->m_nFlag |= O_VIDEO;

            pRxSession->m_nFlag &= ~I_VIDEO;
        }
        if ((nAVOption!=AVOPTION_VIDEO) &&
	    		(pRxSession->m_nFlag & I_AUDIO) &&
	    		!(pTxSession->m_nFlag & O_AUDIO)) {

            nLen=pRxSession->m_nAudioLen;
            if (pTxSession->m_bControlling && pTxSession->m_AV) {
                t = pTxSession2->m_AV;
                memcpy(audiobuffer, pRxSession->m_AudioRcv, nLen);
                t->m_AudioSndPtr = audiobuffer;
                pTxSession->m_nFlag |= O_AUDIO;
            }
            if (pTxSession2->m_bControlling && pTxSession2->m_AV) {
                t = pTxSession2->m_AV;
                memcpy(audiobuffer2, pRxSession->m_AudioRcv, nLen);
                t->m_AudioSndPtr = audiobuffer2;
                pTxSession2->m_nFlag |= O_AUDIO;
            }
            pRxSession->m_nFlag &= ~I_AUDIO;
        }
        if ((pTxSession && (pTxSession->m_nFlag & (I_SER|I_KB|I_MS))) || 
        	(pTxSession2 && (pTxSession2->m_nFlag & (I_SER|I_KB|I_MS)))) {
            if (nKMIdle != 255) {
                gettimeofday (&ts, NULL);
                elapsed_usecs = ts.tv_usec - lastKMTime.tv_usec +
				(ts.tv_sec - lastKMTime.tv_sec) * 1000000;
                if (elapsed_usecs > nKMIdle * 1000000) {
                    if (pTxSession &&
                               (pTxSession->m_nFlag & (I_SER|I_KB|I_MS))) {
                        activeKMSession = pTxSession;
                        lastKMTime.tv_usec = ts.tv_usec;
                        lastKMTime.tv_sec = ts.tv_sec;
                    }
                    else if (pTxSession2 &&
                    		(pTxSession->m_nFlag & (I_SER|I_KB|I_MS))) {
                        activeKMSession = pTxSession2;
                        lastKMTime.tv_usec = ts.tv_usec;
                        lastKMTime.tv_sec = ts.tv_sec;
                    }
#ifdef	DEBUG
                    printf("KM control switched to to TX %d\n", 
				(activeKMSession == pTxSession) ? 1 : 2);
#endif
                } 
            }       
            if (!(pRxSession->m_nFlag & O_CTL) &&
            		(activeKMSession->m_nFlag & (I_SER|I_KB|I_MS))) {
                nLen = htons(*(unsigned short *)activeKMSession->m_CtlRcv);
                memcpy(pRxSession->m_CtlSnd, activeKMSession->m_CtlRcv, nLen);
                pRxSession->m_nFlag |= O_CTL;
                if (pTxSession)
                    pTxSession->m_nFlag &= ~(I_SER|I_KB|I_MS);
                if (pTxSession2)
                    pTxSession2->m_nFlag &= ~(I_SER|I_KB|I_MS);
#ifdef    DEBUG
                printf("Copied %d bytes %s data from RX\n", nLen,
			(activeKMSession->m_nFlag & I_SER) ? "SER" : 
			((activeKMSession->m_nFlag & I_KB) ? "KB" : "MS"));
                dump(activeKMSession->m_CtlRcv, nLen);
#endif
            }
        }

        if (retval == 0) 
            usleep(1);
	else {
            /// Get Statistics
            gettimeofday(&tNow, NULL);
            delta = ((tNow.tv_sec - tPrev.tv_sec) * 1000)  +
                ((tNow.tv_usec - tPrev.tv_usec) / 1000 );
            if (delta >= 1000) {
                tPrev.tv_sec = tNow.tv_sec;
                tPrev.tv_usec = tNow.tv_usec;
            	Rx_PrintStatistics(pRxSession, pStatBuffer, 160);
                printf ("RX:  %s\n", pStatBuffer);
                Tx_PrintStatistics(pTxSession, pStatBuffer, 160);
                printf ("TX1: %s\n", pStatBuffer);
                Tx_PrintStatistics(pTxSession2, pStatBuffer, 160);
                printf ("TX2: %s\n", pStatBuffer);
            }
        }
    }
    if (pTxSession)
        Tx_Destroy(pTxSession);
    if (pTxSession2)
        Tx_Destroy(pTxSession2);
    if (pRxSession)
        Rx_Destroy(pRxSession);


    return 0;
}
