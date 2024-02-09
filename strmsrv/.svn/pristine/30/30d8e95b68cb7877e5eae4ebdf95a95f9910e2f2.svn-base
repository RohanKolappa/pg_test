

#include "strmsrv_compatibility.h"
#include "senderthr.hh"
#include "v2dtx.hh"
#include "mpegtstx.hh"
#include "rtptx.hh"
#include "udptx.hh"
#include "v2dff.hh"
#include "common.hh"

#ifdef HAVE_V2O
#include "fpgatx.hh"
#include "fpgaaudiotx.hh"
#endif

#include "v2dplayertx.hh"
#include "rtpplayertx.hh"

using namespace std;

/// Constructor for the Sender thread
CSenderThr::CSenderThr() :
        CStreamerThread("Sender")
{
}

/// Destructor for the Sender thread
CSenderThr::~CSenderThr()
{
    SYSLOG_TRACE("Destructing %s:%s\n", __FILE__, __FUNCTION__);
}


int
CSenderThr::handleSyncStop(CCommand *pCmd)
{
    intlist pSyncIdList;
    timestamp_t nTimecode;
    int nIds, nCtr;
    int nObjId = 0;
    int nState = 0;
    list<CStreamer *>::iterator iter;

    nTimecode = pCmd->GetTimecode();
    timestamp_t reqTimeCode = nTimecode;
    nIds = pCmd->GetIntListVar("idlist", pSyncIdList);

    for (nCtr = 0; nCtr < nIds; nCtr++) {
        nObjId = pSyncIdList[nCtr];
        for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {
            if (nObjId != (*iter)->GetId())
                continue;
            if (nTimecode < (*iter)->GetTimecode()) {
                nTimecode = (*iter)->GetTimecode();
            }
        }
    }

    for (nCtr = 0; nCtr < nIds; nCtr++) {
        nObjId = pSyncIdList[nCtr];
        for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {
            if (nObjId != (*iter)->GetId())
                continue;
            if ((*iter)->IsStarted()) {
                SYSLOG_INFO("StopAt req=%"PRIu64", actual=%"PRIu64"\n",
                            reqTimeCode, nTimecode);
                (*iter)->StopAt(nTimecode);
                nState = (*iter)->GetState();
            }
        }
    }
    pCmd->SetResult(CMDSUCCESS);
    pCmd->SetTimecode(nTimecode);
    pCmd->SetStatus(nState);
    return 0;
}


int
CSenderThr::handleStop(CCommand *pCmd)
{
    timestamp_t nTimecode;
    int nObjId = 0;
    int nState = 0;
    list<CStreamer *>::iterator iter;

    nTimecode = pCmd->GetTimecode();
    nObjId = pCmd->GetId();

    for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {
        if (nObjId != (*iter)->GetId()) continue;

        if (!((*iter)->IsStarted()))
            break;

        (*iter)->StopAt(nTimecode);
        nTimecode = (*iter)->GetTimecode();
        nState = (*iter)->GetState();
        pCmd->SetStatus(nState);
        pCmd->SetTimecode(nTimecode);
        pCmd->SetResult(CMDSUCCESS);
        return 0;
    }

    pCmd->SetResult(CMDFAILURE);
    return 0;
}


CStreamer *
CSenderThr::handleAdd(CCommand *pCmd)
{

    CStreamer   *pStreamer = NULL;

    switch (pCmd->GetPortType()) {

    case V2DDECODERPORT: {

        CV2dTx *pv2dtx;
        int nCallbackPort = 0;
        int nTTL = 1;
        int maxBandwidth = (100 * 1024 * 1024); // default 100 Mbps
        int v2dtx = 0;
        int nListener = 0;
        char pMcastAddr[80];
        char pPolicyFile[80];
        char pAuthScheme[80];
        char pAuthData[80];
        char pProtocolVersion[80];
        int policyCheck = 0;
        int nInterface = 0;

        strcpy(pMcastAddr, "");
        strcpy(pPolicyFile, "");
        strcpy(pAuthScheme, "");
        strcpy(pAuthData, "");
        strcpy(pProtocolVersion, "");

        pCmd->GetIntVar("dstport", &nCallbackPort);
        pCmd->GetIntVar("mcastttl", &nTTL);
        pCmd->GetVar("mcastaddr", pMcastAddr, 80);
        pCmd->GetIntVar("maxbitrate", &maxBandwidth);
        pCmd->GetIntVar("v2dtx", &v2dtx);
        pCmd->GetIntVar("listener", &nListener);
        pCmd->GetVar("policyfile", pPolicyFile, 80);
        pCmd->GetIntVar("policycheck", &policyCheck);
        pCmd->GetIntVar("interface", &nInterface);
        pCmd->GetVar("authscheme", pAuthScheme, 80);
        pCmd->GetVar("authdata", pAuthData, 80);
        pCmd->GetVar("protocolversion", pProtocolVersion, 80);

        pv2dtx = new CV2dTx (pCmd->GetId(),
                             nCallbackPort,
                             nTTL,
                             pMcastAddr,
                             maxBandwidth,
                             v2dtx,
                             nListener,
                             policyCheck,
                             pPolicyFile,
                             pProtocolVersion,
                             pAuthScheme,
                             pAuthData,
                             nInterface);

        pStreamer = pv2dtx;
        break;
    }

    case V2DPLAYERPORT: {

        CV2dPlayerTx *pv2dplayertx;
        int nPixelFormat = 3;
        pCmd->GetIntVar("pixelformat", &nPixelFormat);


        pv2dplayertx = new CV2dPlayerTx (pCmd->GetId(), nPixelFormat);
        pStreamer = pv2dplayertx;
        break;
    }

    case RTPPLAYERPORT: {

        char pSDPFile[256];
        int nPixelFormat = 3;
        pCmd->GetIntVar("pixelformat", &nPixelFormat);

        strcpy(pSDPFile, "");

        pCmd->GetVar("sdpfile", pSDPFile, 256);

        CRtpPlayerTx *prtpplayertx = new CRtpPlayerTx(pCmd->GetId(), pSDPFile, nPixelFormat);
        pStreamer = prtpplayertx;
        break;
    }

    case MPEGTSDECODERPORT: {

        CMpegTsTx *pmpegtstx;
        int nTTL = 1;
        int nDestinationPort = 0;
        char pDestination[80];
        int nInterface = 0;

        strcpy(pDestination, "");

        pCmd->GetVar("dstaddr", pDestination, 80);
        pCmd->GetIntVar("dstport", &nDestinationPort);
        pCmd->GetIntVar("mcastttl", &nTTL);
        pCmd->GetIntVar("interface", &nInterface);

        pmpegtstx = new CMpegTsTx (pCmd->GetId(),
                                   pDestination,
                                   nDestinationPort,
                                   nTTL,
                                   nInterface);
        pStreamer = pmpegtstx;
        break;
    }

    case RTPDECODERPORT: {

        CRtpTx *prtptx;
        int nTTL = 1;
        int nDestinationPort = 0;
        int nInterface = 0;
        int bAudioOnly = 0;

        //default values for Video and Audio Clock Rate
        int nVideoTs = 90000;
        int nAudioTs = 48000;

        //default values for VideoFrameRate and VideoFrameRateDivisor
        unsigned long nVideoFrameRate = 30000;
        unsigned long nVideoFRDivisor = 1001;

        char pDestination[80];

        strcpy(pDestination, "");

        pCmd->GetVar("dstaddr", pDestination, 80);
        pCmd->GetIntVar("dstport", &nDestinationPort);
        pCmd->GetIntVar("mcastttl", &nTTL);
        pCmd->GetIntVar("videoclockrate", &nVideoTs);
        pCmd->GetIntVar("audioclockrate", &nAudioTs);
        pCmd->GetIntVar("interface", &nInterface);
        pCmd->GetIntVar("audioonly", &bAudioOnly);

        prtptx = new CRtpTx (pCmd->GetId(),
                             pDestination,
                             nDestinationPort,
                             nTTL,
                             nVideoTs, nAudioTs,
                             nVideoFrameRate,
                             nVideoFRDivisor,
                             nInterface,
                             bAudioOnly);
        pStreamer = prtptx;
        break;
    }

    case UDPDECODERPORT: {

        CUdpTx *pudptx;
        int nTTL = 1;
        int nDestinationPort = 0;
        char pDestination[80];
        int nPerfInterval = 0;
        int nInterface = 0;

        strcpy(pDestination, "");

        pCmd->GetVar("dstaddr", pDestination, 80);
        pCmd->GetIntVar("dstport", &nDestinationPort);
        pCmd->GetIntVar("mcastttl", &nTTL);
        pCmd->GetIntVar("perfinterval", &nPerfInterval);
        pCmd->GetIntVar("interface", &nInterface);

        pudptx = new CUdpTx (pCmd->GetId(),
                             pDestination,
                             nDestinationPort,
                             nTTL,
                             nPerfInterval,
                             nInterface);
        pStreamer = pudptx;
        break;
    }

    case V2DFFPORT: {

        CV2dFF *pv2dff;

        pv2dff = new CV2dFF (pCmd->GetId());
        pStreamer = pv2dff;
        break;
    }

#ifdef HAVE_V2O
    case V2DHWPORT: {

        CFPGATx     *pfpgatx;
        int nIOPort = 1;
        int nEnableKBMS = 1;
        pCmd->GetIntVar("ioport", &nIOPort);
        pCmd->GetIntVar("enablekbm", &nEnableKBMS);

        pfpgatx = new CFPGATx(pCmd->GetId(), nIOPort, (nEnableKBMS != 0));
        pStreamer = pfpgatx;
        break;
    } case V2DAUDIODECODERPORT: { 
        CFPGAAudioTx *pFPGAAudioTx; 
        int nIOPort = 1; 
        int nAudioport = 0; 
        int nAudiotype = AUDIOTYPE_DEFAULT; 

        pCmd->GetIntVar("audioport", &nAudioport);
        pCmd->GetIntVar("audiotype", &nAudiotype);
        pCmd->GetIntVar("ioport", &nIOPort);

        pFPGAAudioTx = new CFPGAAudioTx (pCmd->GetId(), nIOPort, nAudioport, nAudiotype);
        pStreamer = pFPGAAudioTx;
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
CSenderThr::handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer)
{
    int retval = 0;

    switch (a_pCmd->GetPortType()) {
    case V2DDECODERPORT:
    case V2DHWPORT:
    case V2DFFPORT:
        retval = a_streamer->Update(a_pCmd);
        break;
    }


    return retval;
}



