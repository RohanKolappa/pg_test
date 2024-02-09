

#include "strmsrv_compatibility.h"
#include "rcvrthr.hh"
#include "vncrx.hh"
#include "v2drx.hh"
#ifdef DOWIS
#include "wisrx.hh"
#include "wisencrx.hh"
#endif
#ifdef HAVE_V2O
#include "fpgarx.hh"
#include "fpgaaudiorx.hh"
#endif
#ifdef WINDOWS
#include "winfbrx.hh"
#endif
#include "rtprx.hh"
#include "udprx.hh"
#include "mpegtsrx.hh"
#include "scheduler.hh"
#include "common.hh"


/// This is the thread code
/// Construtor for the Receiver thread
CRcvrThr::CRcvrThr():
        CStreamerThread("Receiver")
{
}

/// Destructor for the Receiver thread
CRcvrThr::~CRcvrThr()
{
    SYSLOG_TRACE("Destructing %s:%s\n", __FILE__, __FUNCTION__);
}


CStreamer *CRcvrThr::handleAdd(CCommand * pCmd)
{
    CStreamer *pStreamer = NULL;

    switch (pCmd->GetPortType()) {

    case V2DNETWORKPORT: {
        CV2dRx *pV2drx;
        int nAVOption = AVOPTION_VIDEO;
        int nAudiotype = AUDIOTYPE_DEFAULT;
        char pDestination[60];
        char pAuthScheme[80];
        char pAuthData[80];
        int nPort = 0;
        int nBitrate = 0;
        int bMulticast = 0;
        int nVideoport = 0;
        int nAudioport = 0;
        int nHTTPport = 0;
        intlist interfaceList;

        strcpy(pDestination, "");
        strcpy(pAuthScheme, "");
        strcpy(pAuthData, "");

        pCmd->GetVar("srcip", pDestination, 60);
        pCmd->GetIntVar("srcport", &nPort);
        pCmd->GetIntVar("srcbitrate", &nBitrate);
        pCmd->GetIntVar("srcmulticast", &bMulticast);
        pCmd->GetIntVar("avoption", &nAVOption);
        pCmd->GetIntVar("videoport", &nVideoport);
        pCmd->GetIntVar("audioport", &nAudioport);
        pCmd->GetIntVar("audiotype", &nAudiotype);
        pCmd->GetIntVar("httpport", &nHTTPport);
        pCmd->GetIntListVar("interfacelist", interfaceList);
        pCmd->GetVar("authscheme", pAuthScheme, 80);
        pCmd->GetVar("authdata", pAuthData, 80);

        pV2drx = new CV2dRx(pCmd->GetId(),
                            pDestination, nPort, nBitrate,
                            bMulticast, nAVOption,
                            nVideoport, nAudioport, nAudiotype,
                            nHTTPport, pAuthScheme, pAuthData, interfaceList);
        pStreamer = pV2drx;
        break;
    }

#ifdef DOWIS
    case WISENCODERPORT: {
        CWisEncRx *pWisencrx;
        int nPort = 0;

        SYSLOG_INFO("Adding WIS Encoder Port");
        pCmd->GetIntVar("srcport", &nPort);

        pWisencrx = new CWisEncRx(pCmd->GetId(), nPort, pCmd);
        pStreamer = pWisencrx;
        break;
    }
#endif

#ifdef HAVE_V2O
    case V2DENCODERPORT: {
        CFPGARx *pFPGARx;
        int nIOPort = 1;

        pCmd->GetIntVar("ioport", &nIOPort);

        pFPGARx = new CFPGARx(pCmd->GetId(),
                              nIOPort);
        pStreamer = pFPGARx;
        break;

    }
#endif

    case VNCNETWORKPORT: {
        CVNCRx *pVNCRx;
        char serverip[60];
        int port = 5900;
        int compression = 2;
        int codecversion = 1;
        char passwdbuffer[60];
        char * password = NULL;

        strcpy(serverip, "");
        strcpy(passwdbuffer, "");
        pCmd->GetVar("rfbip", serverip, 60);
        pCmd->GetIntVar("rfbport", &port);
        pCmd->GetIntVar("compression", &compression);
        pCmd->GetIntVar("codecversion", &codecversion);


        if (pCmd->GetVar("rfbpasswd", passwdbuffer, 60) == -1)
            password = NULL;
        else
            password = passwdbuffer;

        pVNCRx = new CVNCRx(pCmd->GetId(), serverip,
                port, password, compression, codecversion);
        pStreamer = pVNCRx;
        break;

    }

#ifdef WINDOWS
    case WINFBENCODERPORT: {
        CWinFBRx *pWinFBRx;
        int compression = 2;
        int codecversion = 1;

        pCmd->GetIntVar("compression", &compression);
        pCmd->GetIntVar("codecversion", &codecversion);

        pWinFBRx = new CWinFBRx(pCmd->GetId(), compression, codecversion);
        pStreamer = pWinFBRx;
        break;

    }
#endif

    case RTPNETWORKPORT: {
        CRtpRx *pRtprx;
        char pDestination[60];
        char pAudioDestination[60];
        int nVideoport = 0;
        int nAudioport = 0;
        char pVideoCodec[60];
        char pAudioCodec[60];
        intlist interfaceList;

        strcpy(pDestination, "");
        strcpy(pAudioDestination, "");
        strcpy(pVideoCodec, "");
        strcpy(pAudioCodec, "");

        SYSLOG_INFO("Adding RTP Network Port");
        pCmd->GetVar("srcip", pDestination, 60);
        pCmd->GetVar("audiosrcip", pAudioDestination, 60);
        pCmd->GetIntVar("videoport", &nVideoport);
        pCmd->GetIntVar("audioport", &nAudioport);
        pCmd->GetVar("videocodec", pVideoCodec, 60);
        pCmd->GetVar("audiocodec", pAudioCodec, 60);
        pCmd->GetIntListVar("interfacelist", interfaceList);


        pRtprx = new CRtpRx(pCmd->GetId(),
                            pDestination, pAudioDestination, nVideoport, nAudioport,
                            pVideoCodec, pAudioCodec, interfaceList);
        pStreamer = pRtprx;
        break;
    }

    case UDPNETWORKPORT: {
        CUdpRx *pUdprx;
        int nGenBw = 0;
        int nSourcePort = 60001;
        int nPerfInterval = 0;
        intlist interfaceList;

        char pDestination[60];

        strcpy(pDestination, "");

        SYSLOG_INFO("Adding UDP Network Port");
        pCmd->GetVar("mcastaddr", pDestination, 60);
        pCmd->GetIntVar("srcport", &nSourcePort);
        pCmd->GetIntVar("perfinterval", &nPerfInterval);
        pCmd->GetIntVar("generatebw", &nGenBw);
        pCmd->GetIntListVar("interfacelist", interfaceList);
        pUdprx = new CUdpRx(pCmd->GetId(),
                            nSourcePort, pDestination, nPerfInterval,
                            nGenBw, interfaceList);
        pStreamer = pUdprx;
        break;
    }

    case MPEGTSNETWORKPORT: {
        CMpegTsRx *pMpegtsrx;
        int nDestPort = 1234;
        char pSource[60];
        char pDestination[60];
        intlist interfaceList;

        strcpy(pDestination, "");
        strcpy(pSource, "");

        SYSLOG_INFO("Adding MPEGTS Network Port");
        pCmd->GetVar("srcip", pSource, 60);
        pCmd->GetVar("multicastip", pDestination, 60);
        pCmd->GetIntVar("dstport", &nDestPort);
        pCmd->GetIntListVar("interfacelist", interfaceList);

        SYSLOG_INFO
        ("Adding MPEGTS Network Port sourceIP %s multicastIp %s destport %d\n",
         pSource, pDestination, nDestPort);

        pMpegtsrx = new CMpegTsRx(pCmd->GetId(),
                                  pSource, pDestination, nDestPort, interfaceList);
        pStreamer = pMpegtsrx;
        break;
    }

    case SCHEDULERPORT: {
        CScheduler *pScheduler;

        SYSLOG_INFO("Adding  Scheduler Port");

        pScheduler = new CScheduler(pCmd->GetId());

        pStreamer = pScheduler;

        break;
    }

#ifdef HAVE_V2O
    case V2DAUDIOENCODERPORT: {
        CFPGAAudioRx *pFPGAAudioRx;
        int nIOPort = 1;
        int nAudiotype = AUDIOTYPE_DEFAULT;

        SYSLOG_INFO("Adding Audio Encoder Port");

        pCmd->GetIntVar("audiotype", &nAudiotype);
        pCmd->GetIntVar("ioport", &nIOPort);

        pFPGAAudioRx = new CFPGAAudioRx (pCmd->GetId(), nIOPort, nAudiotype);
        pStreamer = pFPGAAudioRx;
        break;
    }
#endif

    default:
        ASSERT(0);
        break;
    }

    return pStreamer;
}


int
CRcvrThr::handleUpdateStreamer(CCommand * a_pCmd,
                               CStreamer * a_streamer)
{
    int retval = 0;

    switch (a_pCmd->GetPortType()) {

    case RTPNETWORKPORT:
    case UDPNETWORKPORT:
    case MPEGTSNETWORKPORT:
        // No update commands supported yet for the above types
        break;
    case V2DENCODERPORT:
    case V2DNETWORKPORT:
    case WISENCODERPORT:
    case SCHEDULERPORT:
    case VNCNETWORKPORT:
    case WINFBENCODERPORT:
    case V2DAUDIOENCODERPORT: 
        retval = a_streamer->Update(a_pCmd);
        break;

    default:
        ASSERT(0);
        break;
    }


    return retval;
}
