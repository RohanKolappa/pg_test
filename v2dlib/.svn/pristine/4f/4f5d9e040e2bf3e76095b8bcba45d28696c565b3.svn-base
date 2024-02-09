#include "v2dlib_compatibility.h"
#include <math.h>

#include "v2d.h"
#include "v2dcommon.h"

int _v2d_debug_level =  V2DLIB_INFO;
void (*v2dLibLoggerCallback)(int logLevel, char * logMessage) = NULL;


const char * HTTPRequestFmt=
                                "GET /v2dmedia?action=update&operation=output&dsttype=V2DDecoderPort&streamtype=V2D&port=%d&data=%s";
const char * HTTPResponseFmt=
                                "HTTP/1.0 200 OK\n"
                                "Content-Type: text/html\n"
                                "Content-Length: 0\n"
                                "\r\n"
                                "<HTML>\n"
                                "Session found \n"
                                "<br>%s<br>\n"
                                "<br>%d<br>\n"
                                "</HTML>\n";


#ifdef WINDOWS
int windows_gettimeofday(struct timeval* tp, int* tz)
{
    struct timeb tb;
    ftime(&tb);
    tp->tv_sec = (long)tb.time;
    tp->tv_usec = 1000*tb.millitm;
    return 0;
}

void v2dLibSetTime(unsigned long long currentTime)
{
    // This mechanism not used on windows
}

#elif STREAMING_SERVER

//unsigned long long _v2d_current_time = 0;

void v2dLibSetTime(unsigned long long currentTime)
{
    // This mechanism not used on any longer on linux too
}

#endif

/**
 * v2dLibStartup - makes v2d lib ready
 *
 * Returns 0 if successful otherwise error code
 */
int v2dLibStartup()
{
    int bRetVal = 0;
    struct timeval tv;

#ifdef WINDOWS
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD(2,2), &wsaData ) )
        bRetVal = WSAGetLastError();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

    // seed random number generator using current time
#ifdef WINDOWS
    GETTIMEOFDAY(&tv, NULL);
    srand(tv.tv_sec + tv.tv_usec);
#else
    gettimeofday(&tv, NULL);
    srandom(tv.tv_sec + tv.tv_usec);
#endif

    return bRetVal;
}

/**
 * v2dLibCleanup
 *
 * Returns 0 if successful otherwise error code
 */
int v2dLibCleanup()
{
    int bRetVal = 0;

#ifdef WINDOWS
    if ( WSACleanup() )
        bRetVal = WSAGetLastError();
#endif

    return bRetVal;
}

void v2dLibSetLogger(void (*LoggerCallback)(int logLevel, char * logMessage))
{
    v2dLibLoggerCallback = LoggerCallback;
}

int v2dLibIsScap(char *pPayload)
{

    int protid = GET_PROTOCOL_UNIT_ID((unsigned char *) pPayload);
    if (protid == IDENT_SERVER_CAP) {
        return 1;
    }
    return 0;
}

void v2dLibDumpHex(unsigned char dat[], int datsize)
{
#ifndef WINDOWS
    int i;
    for (i = 0; i < datsize; i += 8) {
        if (i <= datsize - 8)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x %02x %02x %02x %02x %02x\n",
                dat[i], dat[i+1], dat[i+2], dat[i+3],
                dat[i+4], dat[i+5], dat[i+6], dat[i+7]);
        else if (i == datsize - 7)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x %02x %02x %02x %02x\n",
                dat[i], dat[i+1], dat[i+2], dat[i+3],
                dat[i+4], dat[i+5], dat[i+6]);
        else if (i == datsize - 6)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x %02x %02x %02x\n",
                dat[i], dat[i+1], dat[i+2], dat[i+3], dat[i+4], dat[i+5]);
        else if (i == datsize - 5)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x %02x %02x\n",
                dat[i], dat[i+1], dat[i+2], dat[i+3], dat[i+4]);
        else if (i == datsize - 4)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x %02x\n", dat[i], dat[i+1], dat[i+2], dat[i+3]);
        else if (i == datsize - 3)
            syslog(LOG_LOCAL5|LOG_INFO,
                "%02x %02x %02x\n", dat[i], dat[i+1], dat[i+2]);
        else if (i == datsize - 2)
            syslog(LOG_LOCAL5|LOG_INFO, "%02x %02x\n", dat[i], dat[i+1]);
        else if (i == datsize - 1)
            syslog(LOG_LOCAL5|LOG_INFO, "%02x\n", dat[i]);
    }
#endif
}

int v2dLibParseScap(ScapData_t *scap, char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_CAP) {
        return -1;
    }
    memset(scap, 0x00, sizeof(ScapData_t));


    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: SCAP:\n", __func__);
    v2dLibDumpHex((unsigned char *) pPayload, len);
#endif

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
			LOG_SYS(
				V2DLIB_ERROR,
				"V2DLib: Malformed scap , code %d scaplength %d @%d",
				code,len, i);
            return -1;
        }

        switch(code) {
        case SCAP_IDENT_VIDEO_NAME:
            scap->m_videoNameSet = 1;
            memset(scap->m_videoName, 0x00, 64);
            GET_PROTOCOL_UNIT_VALUE_STR(scap->m_videoName, cp, 64);
            scap->m_videoName[63] = 0x00;
            break;
        case SCAP_IDENT_VIDEO_HRES:
            scap->m_hResSet = 1;
            scap->m_hRes = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_VRES:
            scap->m_vResSet = 1;
            scap->m_vRes = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_REFRESH:
            scap->m_videoRefreshSet = 1;
            scap->m_videoRefresh = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            scap->m_videoRefresh = scap->m_videoRefresh/1000.0;
            break;
        case SCAP_IDENT_VIDEO_PCR:
            scap->m_videoPcrSet = 1;
            scap->m_videoPcr = GET_PROTOCOL_UNIT_VALUE(cp, plen);
	    scap->m_videoPcr = scap->m_videoPcr/1000.0;
            break;
        case SCAP_IDENT_VIDEO_HFP:
            scap->m_videoHfpSet = 1;
            scap->m_videoHfp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_HBP:
            scap->m_videoHbpSet = 1;
            scap->m_videoHbp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_HSYNC:
            scap->m_videoHsyncSet = 1;
            scap->m_videoHsync = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_VFP:
            scap->m_videoVfpSet = 1;
            scap->m_videoVfp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_VBP:
            scap->m_videoVbpSet = 1;
            scap->m_videoVbp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_VSYNC:
            scap->m_videoVsyncSet = 1;
            scap->m_videoVsync = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_CHANGED:
            scap->m_videoRateChangedSet = 1;
            scap->m_videoRateChanged = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_MCAST_STATUS:
            break;
        case SCAP_IDENT_MCAST_ADDRESS:
            scap->m_mcastAddressSet = 1;
            scap->m_mcastAddress = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_PORT:
            scap->m_videoPortSet = 1;
            scap->m_videoPort = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_MQUANT:
            scap->m_mQuantSet = 1;
            scap->m_mQuant = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_DCQUANT:
            scap->m_dcQuantSet = 1;
            scap->m_dcQuant = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_SLICESIZE:
            scap->m_sliceSizeSet = 1;
            scap->m_sliceSize = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_ACNOIZE:
            break;
        case SCAP_IDENT_VIDEO_DCNOIZE:
            break;
        case SCAP_IDENT_VIDEO_STEREO:
            scap->m_videoStereoSet = 1;
            scap->m_videoStereo = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_COMPMODE:
            scap->m_videoCompmodeSet = 1;
            scap->m_videoCompmode = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_MOTIONCOMPMODE:
            scap->m_videoMotionCompmodeSet = 1;
            scap->m_videoMotionCompmode = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_REV_AUDIO_PORT:
            scap->m_revAudioPortSet = 1;
            scap->m_revAudioPort = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_AUDIO_TYPE:
            scap->m_audioTypeSet = 1;
            scap->m_audioType = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            if (scap->m_audioType < AUDIOTYPE_MIN || scap->m_audioType > AUDIOTYPE_MAX)
                scap->m_audioType = AUDIOTYPE_DEFAULT;
            break;
        case SCAP_IDENT_AUDIO_PORT:
            scap->m_audioPortSet = 1;
            scap->m_audioPort = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_KB_TYPE:
            scap->m_kbTypeSet = 1;
            scap->m_kbType = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            if(scap->m_kbType != KB_TYPE_SGI)
                scap->m_kbType = KB_TYPE_PC;
            break;
        case SCAP_IDENT_MS_TYPE:
            scap->m_msTypeSet = 1;
            scap->m_msType = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            if (scap->m_msType != MS_TYPE_IMPS2)
                scap->m_msType = MS_TYPE_PS2;
            break;
        case SCAP_IDENT_VIDEO_ID:
            scap->m_videoIdSet = 1;
            scap->m_videoId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_ORIG_HRES:
            scap->m_origHresSet = 1;
            scap->m_origHres = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_ORIG_VRES:
            scap->m_origVresSet = 1;
            scap->m_origVres = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_VIDEO_CODEC_VERSION:
            scap->m_videoCodecVersionSet = 1;
            scap->m_videoCodecVersion = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_PROTOCOL_VERSION:
            scap->m_protocolVersionSet = 1;
            scap->m_protocolVersion = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case SCAP_IDENT_AUDIO_FREQ:
            scap->m_nAudioFreqSet = 1;
            scap->m_nAudioFreqency = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown SCAP: %d\n", code);
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    return len;
}

int v2dLibDumpScap(char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;
    char buff[64];
    int value;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_CAP) {
        return -1;
    }


    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP DUMP\n");

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(
				V2DLIB_ERROR,
				"V2DLib: Malformed scap , code %d scaplength %d",
				code,len);
            return -1;
        }

        switch(code) {
        case SCAP_IDENT_VIDEO_NAME:
            GET_PROTOCOL_UNIT_VALUE_STR(buff, cp, 64);
            buff[63] = 0x00;
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_NAME: %s\n", buff);
            break;
        case SCAP_IDENT_VIDEO_HRES:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_HRES: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_VRES:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_VRES: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_REFRESH:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_REFRESH: %f\n", value/1000.0); */
            break;
        case SCAP_IDENT_VIDEO_PCR:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_PCR: %f\n", value/1000.0); */
            break;
        case SCAP_IDENT_VIDEO_HFP:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_HFP: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_HBP:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_HBP: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_HSYNC:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_HSYNC: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_VFP:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_VFP: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_VBP:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_VBP: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_VSYNC:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_VSYNC: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_CHANGED:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_CHANGED: %d\n", value);
            break;
        case SCAP_IDENT_MCAST_STATUS:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_MCAST_STATUS: %d\n", value);
            break;
        case SCAP_IDENT_MCAST_ADDRESS:
            GET_PROTOCOL_UNIT_VALUE_STR(buff, cp, 64);
            buff[63] = 0x00;
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_MCAST_ADDRESS: %s\n", buff);
            break;
        case SCAP_IDENT_VIDEO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_PORT: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_MQUANT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_MQUANT: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_DCQUANT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_DCQUANT: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_SLICESIZE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_SLICESIZE: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_ACNOIZE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_ACNOIZE: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_DCNOIZE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_DCNOIZE: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_STEREO:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_NAME: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_COMPMODE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_COMPMODE: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_MOTIONCOMPMODE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_MOTIONCOMPMODE: %d\n", value);
            break;
        case SCAP_IDENT_REV_AUDIO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_REV_AUDIO_PORT: %d\n", value);
            break;
        case SCAP_IDENT_AUDIO_TYPE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_AUDIO_TYPE: %d\n", value);
            break;
        case SCAP_IDENT_AUDIO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_AUDIO_PORT: %d\n", value);
            break;
        case SCAP_IDENT_KB_TYPE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_KB_TYPE: %d\n", value);
            break;
        case SCAP_IDENT_MS_TYPE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_MS_TYPE: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_ID:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_ID: %d\n", value);
            break;
        case SCAP_IDENT_VIDEO_ORIG_HRES:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_ORIG_HRES: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_ORIG_VRES:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
/*             LOG_SYS("V2DLib: SCAP_IDENT_VIDEO_ORIG_VRES: %d\n", value); */
            break;
        case SCAP_IDENT_VIDEO_CODEC_VERSION:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_VIDEO_CODEC_VERSION: %d\n", value);
            break;
        case SCAP_IDENT_PROTOCOL_VERSION:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: SCAP_IDENT_PROTOCOL_VERSION: %d\n", value);
            break;
        default:
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Unknown SCAP: %d\n", code);
            break;
        }

        (void) value; // only used in debug mode, mask unused warning
        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

/***********************************************************
 * Trim string (remove white space) from beginning and end *
 ***********************************************************/

char *trim_string(char *str) {
    char *cp = str;

    if (str == NULL)
        return NULL;

    while (*cp != '\0')
        cp++;
    cp--;

    while (*cp == ' ' ||
           *cp == '\t' ||
           *cp == '\r' ||
           *cp == '\n')
        cp--;
    cp++;

    *cp = '\0';

    cp = str;
    while (*cp == ' ' ||
           *cp == '\t')
        cp++;

    return cp;
}


/* Not populating network and netmask to the
 * matched_policy_entry now. To take care of the case where we see
 * multiple 0.0.0.0 netmasks, we use bMaskBit32 we cannot change the
 * maskbits < best_maskbits because otherwise it will not take the
 * earliest possible match
 */

int v2dLibParsePolicy(struct in_addr clientipaddr,
                      struct policy_entry *matched_policy_entry,
                      char * policyfile) {


    char  linbuf[256], savebuf[256], *c, *cp, *name = NULL, *value = NULL;
    FILE *fp;
    int retval = -1, idx, index = -1;
    int bClientFound = 0;
    struct in_addr addr, network_addr, netmask_addr;
    char i, maskbits, best_maskbits;
    int bMaskBit32 = 1;
    const char *filename = NULL;

    network_addr.s_addr = 0;
    netmask_addr.s_addr = 0;

    addr = clientipaddr;

    addr.s_addr = ntohl(addr.s_addr);
    best_maskbits = 32;

#ifdef WINDOWS
    // On windows, simulate a match all policy entry with default parameters
    matched_policy_entry->enable = 1;
    matched_policy_entry->max_bandwidth = 100 * 1024 *1024;
    matched_policy_entry->min_compression = 6;
    matched_policy_entry->max_compression = 6;
    matched_policy_entry->burst_ratio = 2.0;
    matched_policy_entry->burst_duration = 50;
    matched_policy_entry->unicast = 1;
    matched_policy_entry->multicast_init = 1;
    matched_policy_entry->multicast_part = 1;
    matched_policy_entry->unicast_preemptible = 0;
    retval = 0;
    return retval;
#endif

    if (policyfile == NULL)
        filename = "/V2O/config/authlistconf.txt";
    else
        filename = policyfile;

    LOG_SYS(V2DLIB_VERBOSE,
            "V2DLib: v2dLibParsePolicy ENTERING , checking policy for %s in %s...",
            inet_ntoa(clientipaddr), filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        LOG_SYS(V2DLIB_ERROR,
            "V2DLib: Unable to open config file %s", filename);
        return -1;
    }

   /****************************************
    * Read file a line at a time and parse *
    ****************************************/
    do {

      c = fgets(linbuf, 256 ,fp);

      strcpy(savebuf, linbuf);

      /*****************************
       * Throw away comment, etc.  *
       *****************************/
      if( c == NULL || *c == '#')
          continue;

      if (strlen(c) <= 3) {
          continue;
      }

      cp = c;
      cp = trim_string(cp);


      name = cp;


      if (strlen(name) <= 1) {   /* whole line */
          LOG_SYS(V2DLIB_WARN, "Parse error at line: %s", savebuf);
          continue;
      }


      while(*cp != '\0' && *cp != ' ' && *cp != '\t')
          cp++;

      if (*cp == '\0') {
          LOG_SYS(V2DLIB_WARN, "Missing value at line: %s", savebuf);
          continue;
      }

      *cp = '\0';
      cp++;


      if (strlen(name) <= 1) {  /* just the name */
          LOG_SYS(V2DLIB_WARN, "Bad parameter \"%s\" ", name);
          continue;
      }


      while(*cp == ' ' || *cp == '\t')
          cp++;


      value = cp;

      if (strlen(value) < 1) {
          LOG_SYS(V2DLIB_WARN, "Parse error at line: %s", savebuf);
          continue;
      }

      cp = name;

      while(*cp != '.' && *cp != '\0')
          cp++;
      if (*cp == '.') {
          *cp = '\0';
          cp++;
          idx = atoi(cp);
      }
      else {
          LOG_SYS(V2DLIB_WARN, "Parse error at line %s", savebuf);
          continue;
      }

      if(!STRCASECMP(name,"AUTHID")){
            bClientFound = 0;
      }

      if(!STRCASECMP(name,"NETWORK")){

#ifdef WINDOWS
    	  network_addr.s_addr = inet_addr(value);
    	  if (network_addr.s_addr == INADDR_NONE ) {
    		  LOG_SYS(V2DLIB_WARN,
    				  "V2DLib Could not determine network: %s",value);
    		  continue;
    	  }
#else
    	  if (inet_aton(value, &network_addr) == 0) {
    		  LOG_SYS(V2DLIB_WARN,
    				  "V2DLib Could not determine network: %s",value);
    		  continue;
    	  }
#endif
      }

      if(!STRCASECMP(name,"NETMASK")){

#ifdef WINDOWS
    	  network_addr.s_addr = inet_addr(value);
#else
         if (inet_aton(value, &netmask_addr) == 0) {
                LOG_SYS(V2DLIB_WARN,
                    "V2DLib Could not determine netmask: %s",value);
                continue;
            }
#endif
      }

      if (STRCASECMP(name,"ENABLE") == 0) {

         if (STRCASECMP(value,"yes") == 0)
              matched_policy_entry->enable = 1;
         else
              matched_policy_entry->enable = 0;

         if(matched_policy_entry->enable){

             network_addr.s_addr = ntohl(network_addr.s_addr);
             netmask_addr.s_addr = ntohl(netmask_addr.s_addr);

             if ((addr.s_addr & netmask_addr.s_addr) ==
                    (network_addr.s_addr & netmask_addr.s_addr)) {

                    for (i = 0; i < 32; i++)
                        if ((1<<i) & netmask_addr.s_addr)
                            break;

                    maskbits = i;
                    if (maskbits < best_maskbits) {
                        bClientFound = 1;
                        best_maskbits = maskbits;
                    }
                    else
                        bClientFound = 0;
                    if(maskbits == 32 && bMaskBit32)
                        bClientFound = 1;

                    retval = 0;

             }// if addr or-ing and comparing
             else
                 bClientFound = 0;

          }//if matched_policy_entry->enable
          else
              bClientFound = 0;
      }//ENABLE

      // if client ip is found save the rest of the parameters and check and enforce policy
      if(bClientFound){

          bMaskBit32 = 0;
          index = idx;

          if(!STRCASECMP(name,"ENABLE") && !STRCASECMP(value,"Yes"))
              matched_policy_entry->enable = 1;
          else if(!STRCASECMP(name,"ENABLE") && !STRCASECMP(value,"No"))
              matched_policy_entry->enable = 0;

          if(!STRCASECMP(name,"MAX_RATE")){

              if((strstr(value,"M") != NULL) || (strstr(value,"m") != NULL))
                  matched_policy_entry->max_bandwidth = atoi(value)*1024*1024/8;// Its in Bytes
              else if((strstr(value,"K") != NULL) || (strstr(value,"k") != NULL))
                  matched_policy_entry->max_bandwidth = atoi(value)*1024/8;// Its in Bytes
              else
                  matched_policy_entry->max_bandwidth = atoi(value)/8;// Its in Bytes
          }//MAX_RATE

          if(!STRCASECMP(name,"MIN_COMPRESSION"))
              matched_policy_entry->min_compression = atoi(value);

          if(!STRCASECMP(name,"MAX_COMPRESSION"))
              matched_policy_entry->max_compression = atoi(value);

          if(!STRCASECMP(name,"BURST_RATIO"))
              matched_policy_entry->burst_ratio = (float) atof(value);

          if(!STRCASECMP(name,"BURST_DURATION"))
              matched_policy_entry->burst_duration = atoi(value);

          if(!STRCASECMP(name,"UNICAST") && !STRCASECMP(value,"Yes"))
              matched_policy_entry->unicast = 1;
          else if(!STRCASECMP(name,"UNICAST") && !STRCASECMP(value,"No"))
              matched_policy_entry->unicast = 0;

          if(!STRCASECMP(name,"MULTICAST_INIT") && !STRCASECMP(value,"Yes"))
              matched_policy_entry->multicast_init = 1;
          else if(!STRCASECMP(name,"MULTICAST_INIT") && !STRCASECMP(value,"No"))
              matched_policy_entry->multicast_init = 0;

          if(!STRCASECMP(name,"MULTICAST_PART") && !STRCASECMP(value,"Yes"))
              matched_policy_entry->multicast_part = 1;
          else if(!STRCASECMP(name,"MULTICAST_PART") && !STRCASECMP(value,"No"))
              matched_policy_entry->multicast_part = 0;

          if(!STRCASECMP(name,"UNICAST_PREEMPTIBLE") && !STRCASECMP(value,"Yes"))
              matched_policy_entry->unicast_preemptible = 1;
          else if(!STRCASECMP(name,"UNICAST_PREEMPTIBLE") && !STRCASECMP(value,"No"))
              matched_policy_entry->unicast_preemptible = 0;

      }//if bClientFound

    }while(c != NULL);

    fclose(fp);


    LOG_SYS(V2DLIB_VERBOSE,
        "V2DLib: v2dLibParsePolicy  RETURNING  retval %d index %d !!!!!!",
        retval, index);
    (void) index; // only used in debug mode, mask unused warning

    return retval;

}



#ifndef WINDOWS  //placed to compile in windows


int bits_to_bytes(const char *bitstring) {
    char *cc, *br;
    double rate;
    int brate;

    br = strdup(bitstring);
    if (((cc = strchr(br, 'k')) != NULL) ||
        ((cc = strchr(br, 'K')) != NULL) ) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = (int) round(rate * 1024.0 / 8.0);
    }
    else if (((cc = strchr(br, 'm')) != NULL) ||
             ((cc = strchr(br, 'M')) != NULL) ) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = (int) round(rate * 1024.0 * 1024.0/ 8.0);
    }
    else {
        sscanf(br, "%lf", &rate);
        brate = (int) round(rate / 8.0);
    }

    free(br);
    return brate;
}



#endif   //placed to compile in windows


int v2dLibParseAVReq(AVReqData_t *avreq, char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;
    unsigned long value;
    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_AVREQ)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(V2DLIB_ERROR, "V2DLib: v2dLibParseAVReq: Malformed AVREQ: %d", code);
            return -1;
        }
        switch(code) {
        case AVREQ_IDENT_VIDEO_PORT:
        case AVREQ_IDENT_AV_QUALITY:
        case AVREQ_IDENT_AV_SLICESIZE:
        case AVREQ_IDENT_AV_ACNOIZE:
        case AVREQ_IDENT_AV_DCNOIZE:
        case AVREQ_IDENT_AUDIO_PORT:
        case AVREQ_IDENT_PROTOCOL_VERSION:
            break;
        case AVREQ_IDENT_AV_MULTICAST:
            avreq->m_multicastSet = 1;
            avreq->m_multicast = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_FS_REFRESH:
            avreq->m_slicedropSet = 1;
            avreq->m_slicedrop = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_AV_BW:
            avreq->m_videoRateSet = 1;
            avreq->m_videoRate = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_AV_MQUANT:
            avreq->m_mQuantSet = 1;
            avreq->m_mQuant = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_AV_DCQUANT:
            avreq->m_dcQuantSet = 1;
            avreq->m_dcQuant = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_AV_OPTION:
            avreq->m_avoption = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            avreq->m_avoptionSet = 1;
            break;
        case AVREQ_IDENT_AUDIO_TYPE:
            avreq->m_audioTypeSet = 1;
            avreq->m_audioType = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_IPADDR:
            printf ("%s\n", (char *)cp);
            avreq->m_clientAddr.sin_addr.s_addr = inet_addr((char *)cp);
            avreq->m_clientAddrSet=1;
            break;
        case AVREQ_IDENT_MIN_COMPRESSION:
            avreq->m_minCompSet = 1;
            avreq->m_minComp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_MAX_COMPRESSION:
            avreq->m_maxCompSet = 1;
            avreq->m_maxComp = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case AVREQ_IDENT_BURST_RATIO:
            avreq->m_burstRatioSet = 1;
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            avreq->m_burstRatio = (float) value;
            avreq->m_burstRatio = avreq->m_burstRatio/BURST_RATIO_CONVERSION_FACTOR;
            break;
        case AVREQ_IDENT_BURST_DURATION:
            avreq->m_burstDurationSet = 1;
            avreq->m_burstDuration = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown AVREQ: %d\n", code);
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

int v2dLibDumpAVReq(unsigned char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;
    int value;

    cp = pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_AVREQ)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(V2DLIB_ERROR, "V2DLib: Malformed AVREQ");
            return -1;
        }
        switch(code) {
        case AVREQ_IDENT_VIDEO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Video Port: %d\n", value);
            break;
        case AVREQ_IDENT_AV_BW:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Bandwidth: %d\n", value);
            break;
        case AVREQ_IDENT_AV_MULTICAST:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Multicast: %d\n", value);
            break;
        case AVREQ_IDENT_AV_MQUANT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Mquant: %d\n", value);
            break;
        case AVREQ_IDENT_AV_DCQUANT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ DCQuant: %d\n", value);
            break;
        case AVREQ_IDENT_AUDIO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Port: %d\n", value);
            break;
        case AVREQ_IDENT_AV_OPTION:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ AVOption: %d\n", value);
            break;
        case AVREQ_IDENT_AUDIO_TYPE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Audio Type: %d\n", value);
            break;
        case AVREQ_IDENT_FS_REFRESH:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: AVREQ Request IFrame: %d\n", value);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown AVREQ: %d\n", code);
            break;
        }
        (void) value; // only used in debug mode, mask unused warning
        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

int v2dLibParseNetHeader(NetHdrData_t *hdr, char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;

    hdr->video_set = hdr->audio_set = 0;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_NETHDR)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while(i < len) {
        unsigned short time_to_live;
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(V2DLIB_ERROR,
	        "V2DLib: v2dLibParseNetHeader: Malformed NetHdr: %d", code);
            return -1;
        }
        switch(code) {
        case NETHDR_IDENT_DST_MAC0:
            hdr->dst_mac[0] = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_MAC1:
            hdr->dst_mac[1] = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_MAC2:
            hdr->dst_mac[2] = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_SRC_VIDEO_PORT:
            hdr->video_set = 1;
            hdr->src_video_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_VIDEO_PORT:
            hdr->dst_video_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_VIDEO_ADDR:
            hdr->dst_video_addr = GET_PROTOCOL_UNIT_LONG(cp);
            break;
        case NETHDR_IDENT_SRC_AUDIO_PORT:
            hdr->audio_set = 1;
            hdr->src_audio_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_AUDIO_PORT:
            hdr->dst_audio_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case NETHDR_IDENT_DST_AUDIO_ADDR:
            hdr->dst_audio_addr = GET_PROTOCOL_UNIT_LONG(cp);
            break;
        case NETHDR_IDENT_TIME_TO_LIVE:
            time_to_live = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            hdr->ttl = (unsigned char) (time_to_live & 0xff);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown NetHdr: %d\n", code);
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

int v2dLibParseRtpFilter(RtpFilter_t *filter, char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;

    filter->video_set = filter->audio_set = filter->multicastaddr_set = 0;

    cp = (unsigned char *) pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_RTPFILTER) {
        return -1;
    }

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: RTPFILTER:\n", __func__);
    v2dLibDumpHex((unsigned char *) pPayload, len);
#endif

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(V2DLIB_ERROR,
	        "V2DLib: v2dLibParseRtpFilter: Malformed RtpFilter: %d", code);
            return -1;
        }

        switch(code) {
        case RTPFILTER_IDENT_VPORT:
            filter->video_set = 1;
            filter->video_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RTPFILTER_IDENT_VAES:
            filter->video_aes = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RTPFILTER_IDENT_APORT:
            filter->audio_set = 1;
            filter->audio_port = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RTPFILTER_IDENT_AAES:
            filter->audio_aes = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RTPFILTER_IDENT_MULTICASTADDR:
            filter->multicastaddr_set = 1;
            filter->multicastaddr = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown RtpFilter: %d\n", code);
            break;
        }

        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

int v2dLibDumpNetHeader(unsigned char *pPayload)
{
    unsigned char *cp;
    int i=4;
    int code;
    int len, plen;
    int protid;
    int value;

    cp = pPayload;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_NETHDR)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while(i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0 ) {
            LOG_SYS(V2DLIB_ERROR, "V2DLib: Malformed NETHDR\n");
            return -1;
        }
        switch(code) {
        case NETHDR_IDENT_DST_MAC0:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Mac[0]: 0x%04x\n", value);
            break;
        case NETHDR_IDENT_DST_MAC1:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Mac[1]: 0x%04x\n", value);
            break;
        case NETHDR_IDENT_DST_MAC2:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Mac[2]: 0x%04x\n", value);
            break;
        case NETHDR_IDENT_SRC_VIDEO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Src Video Port: %d\n", value);
            break;
        case NETHDR_IDENT_DST_VIDEO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Video Port: %d\n", value);
            break;
        case NETHDR_IDENT_DST_VIDEO_ADDR:
            value = GET_PROTOCOL_UNIT_LONG(cp);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Video Addr: 0x%08x\n", value);
            break;
        case NETHDR_IDENT_SRC_AUDIO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Src Audio Port: %d\n", value);
            break;
        case NETHDR_IDENT_DST_AUDIO_PORT:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Audio Port: %d\n", value);
            break;
        case NETHDR_IDENT_DST_AUDIO_ADDR:
            value = GET_PROTOCOL_UNIT_LONG(cp);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Dst Audio Addr: 0x%08x\n", value);
            break;
        case NETHDR_IDENT_TIME_TO_LIVE:
            value = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            LOG_SYS(V2DLIB_VERBOSE, "V2DLib: Time To Live: %d\n", value);
            break;
        default:
            LOG_SYS(V2DLIB_WARN, "V2DLib: Unknown NetHdr code: %d\n", code);
            break;
        }

        (void) value; // only used in debug mode, mask unused warning
        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

/**
 * CloseSocket - close socket
 *
 * Input:
 *   nSock - socket handle
 */
void v2dLibCloseSocket(int nSock)
{
#ifdef WINDOWS
    shutdown( nSock, SD_BOTH );
    closesocket( nSock );
#else
    close( nSock );
#endif
}

int v2dLibDisableNagling(int nSock)
{
#ifndef WINDOWS
    int one = 1;
    if (setsockopt(nSock, IPPROTO_TCP, TCP_NODELAY,
                   (char *)&one, sizeof(one)) < 0) {
        return -1;
    }
#endif
    return 0;
}


/**
 * getReturnCode
 *
 * Input:
 *   nSock - error category
 */
int getReturnCode( int sErrorCode )
{
    int iRetVal = sErrorCode;

#ifdef WINDOWS
    iRetVal = RETCODE( RC_SOCKERR, WSAGetLastError() );
#else
    iRetVal = RETCODE( RC_SOCKERR, errno );
#endif

    return iRetVal;
}

#if 0
const char * getClientStateString(int clientState) {

    switch (clientState) {
    case CLIENT_STATE_DISABLED:
        return "Disabled";
    case CLIENT_STATE_IDLE:
        return "Idle";
    case CLIENT_STATE_CONNECTING:
        return "Connecting";
    case CLIENT_STATE_CONNECTED:
        return "Connected";
    case CLIENT_STATE_EXP_DATA_REQ:
        return "Expect Data Request";
    case CLIENT_STATE_SEND_DATA_REQ:
        return "Expect Data Request";
    case CLIENT_STATE_READY:
        return "Ready";
    }
    return "Unknown State";
}
#endif


char *v2dLibGetStatusString(char *buf, int statuscode, int multicast, int http,
        int avoption) {

    switch(statuscode) {
    case STATUSCODE_CLEAR:
        sprintf(buf, "Idle");
        break;
    case STATUSCODE_WRITEERR:
        sprintf(buf, "Error Sending Data");
        break;
    case STATUSCODE_READERR:
        sprintf(buf, "Error Receiving Data");
        break;
    case STATUSCODE_TIMEDOUT:
        sprintf(buf, "Connection Timed Out.");
        break;
    case STATUSCODE_CONNECTING:
        sprintf(buf, "Connecting to Server");
        break;
    case STATUSCODE_REFUSED:
        sprintf(buf, "Connection Refused: Not a server");
        break;
    case STATUSCODE_UNREACHABLE:
        sprintf(buf, "Network Unreachable");
        break;
    case STATUSCODE_HOSTUNREACHABLE:
        sprintf(buf, "Server Unreachable");
        break;
    case STATUSCODE_DISCONNECTING:
        sprintf(buf, "Disconnecting");
        break;
    case STATUSCODE_CONNECTED:
//        sprintf(buf, "Connected");

        sprintf(buf, "Connected:");
        if (multicast)
            strcat(buf, " MULTICAST");
        if (http)
            strcat(buf, " HTTP");
        if (avoption == AVOPTION_VIDEO)
            strcat(buf, " VIDEO");
        else if (avoption == AVOPTION_AUDIO)
            strcat(buf, " AUDIO");
        else
            strcat(buf, " AUDIO/VIDEO");

        break;
    case STATUSCODE_WAITING:
        sprintf(buf, "Waiting for Server to Respond");
        break;
    case STATUSCODE_STIMEDOUT:
        sprintf(buf, "Disconnected: Server Timed Out");
        break;
    case STATUSCODE_SERVERERR:
        sprintf(buf, "Server Disconnecting");
        break;
    case STATUSCODE_MULTICASTERR:
        sprintf(buf, "MCAST Req. to UCAST Server");
        break;
    case STATUSCODE_MISMATCHERR:
        sprintf(buf, "MCAST Req. Mismatch");
        break;
    case STATUSCODE_UNICASTERR:
        sprintf(buf, "UCAST Req. to MCASTing Server");
        break;
    case STATUSCODE_SERVERBUSY:
        sprintf(buf, "Server Busy with Another Client");
        break;
    case STATUSCODE_AUTHERR:
        sprintf(buf, "Client not Authorized");
        break;
    case STATUSCODE_PROTERR:
        sprintf(buf, "Protocol Error");
        break;
    case STATUSCODE_SOCKERR:
        sprintf(buf, "Error create UDP Socket");
        break;
    case STATUSCODE_PREMPTED:
        sprintf(buf, "Disconnected:  Server Preempted");
        break;
    case STATUSCODE_AUTHENTICATING:
        sprintf(buf, "Authenticating");
        break;
    case STATUSCODE_UNKNOWN:
        sprintf(buf, "Unknown Status");
        break;
    default:
        sprintf(buf, "?????");
    }

    return buf;
}

char * getV2DProtocolVersion(unsigned short version, char * buf, int len) {

    snprintf(buf, len, "%d.%d",
             V2D_PROTOCOL_VERSION_MAJOR(version), V2D_PROTOCOL_VERSION_MINOR(version));
    return buf;
}

unsigned short getV2DProtocolVersionValue(char * buf) {

    unsigned short version_value = 0;
    int major = 0;
    int minor = 0;
    if (buf == NULL)
        return version_value;
    if (sscanf(buf, "%d.%d", &major, &minor) == 2) {
        version_value = V2D_PROTOCOL_VERSION(major, minor);
    }
    return version_value;
}

/**
 * nano_sleep - Sleep for a specified number of nano seconds (not a busy sleep)
 *
 *   nSleepTime: sleep time in nano seconds
 *
 */
int nano_sleep(unsigned int nSleepTime)
{
    int rc=-1;

#ifdef WINDOWS
    Sleep(1);
    rc = 0;
#else
    int csufd=0;
    if ((csufd = open("/dev/csu", O_RDONLY)) == -1) {
        //printf("Unable to open /dev/csu.\n");
        usleep(1);
        goto EXIT_LABEL;
    }

    if ((rc = read(csufd, NULL, nSleepTime)) < 0) {
        printf("read error %d: \n", rc);
    }
    close(csufd);
    rc = 0;

 EXIT_LABEL:
#endif

    return rc;
}

int v2dLibCreateRFBStatus(
        uint8_t * buffer,
        int status,
        uint32_t sessionId,
        uint32_t queryId)
{
    int len=0;
    uint8_t *cp;
    int attrlen, attrid, attrval;

    cp = buffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_SERVER_RFBSTATUS); 
    cp += 4;
    len += 4;

    attrid = RFBSTAT_IDENT_SESSIONID;
    attrval = sessionId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    attrid = RFBSTAT_IDENT_QUERYID;
    attrval = queryId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    attrid = RFBSTAT_IDENT_STATUS;
    attrval = status;
    attrlen = 2;
    FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    cp = buffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    return len;
}

int v2dLibCreateRFBStatusQuery(uint8_t * buffer,
                               uint32_t sessionId,
                               uint32_t queryId)
{
    int len=0;
    uint8_t *cp;
    int attrlen, attrid, attrval;

    cp = buffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_RFBQUERY);
    cp += 4;
    len += 4;

    attrid = RFBQUERY_IDENT_SESSIONID;
    attrval = sessionId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    attrid = RFBQUERY_IDENT_QUERYID;
    attrval = queryId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    cp = buffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    return len;
}

int v2dLibParseRFBStatusQuery(
        uint8_t * buffer,
        uint32_t * sessionId,
        uint32_t * queryId)
{
    uint8_t *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;

    *sessionId = 0;
    *queryId = 0;

    cp = buffer;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_RFBQUERY)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            return -1;
        }

        switch (code) {

        case RFBQUERY_IDENT_SESSIONID:
            *sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;

        case RFBQUERY_IDENT_QUERYID:
            *queryId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;

        }
        cp = &cp[plen];
        i += plen;
    }

    return 0;
}

int v2dLibParseRFBStatus(
        uint8_t * buffer,
        uint32_t * sessionId,
        uint32_t * queryId,
        int * status)
{
    uint8_t *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;

    *sessionId = 0;
    *queryId = 0;

    cp = buffer;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_SERVER_RFBSTATUS)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            return -1;
        }

        switch (code) {
        case RFBSTAT_IDENT_SESSIONID:
            *sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_QUERYID:
            *queryId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        case RFBSTAT_IDENT_STATUS:
            *status = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        }
        // Increment pointers for next round
        i += plen;
        cp += plen;
    }

    return 0;
}

int v2dLibParseAbsoluteKeyboard(uint8_t * buffer,
        uint32_t * key,
        uint8_t * down,
        uint32_t * sessionId)
{
    unsigned char *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;
    int got_keyboard = 0;

    *key = 0;
    *down = 0;
    *sessionId = 0;

    cp = (unsigned char *) buffer;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_KBABS)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            return -1;
        }

        switch (code) {
        case KBABS_IDENT_RFB:
            *key = ntohl(*(unsigned long *) (cp + 4));
            *down = *(unsigned char *) (cp + 8);
            got_keyboard = 1;
            break;

        case KBABS_IDENT_SESSIONID:
            *sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;

        }
        cp = &cp[plen];
        i += plen;
    }

    return got_keyboard;
}

int v2dLibParseAbsoluteMouse(uint8_t * buffer,
        uint16_t * x,
        uint16_t * y,
        uint8_t * buttonmask,
        uint32_t * sessionId)
{
    unsigned char *cp;
    int i = 4;
    int code;
    int len, plen;
    int protid;
    int got_mouse = 0;

    *x = 0;
    *y = 0;
    *buttonmask = 0;
    *sessionId = 0;

    cp = (unsigned char *) buffer;
    protid = GET_PROTOCOL_UNIT_ID(cp);
    if (protid != IDENT_CLIENT_MSABS)
        return -1;

    len = GET_PROTOCOL_PACKET_LENGTH(cp);
    cp += 4;

    while (i < len) {
        plen = GET_PROTOCOL_UNIT_LENGTH(cp);
        code = GET_PROTOCOL_UNIT_ID(cp);
        if (plen <= 0) {
            return -1;
        }

        switch (code) {
        case MSABS_IDENT_RFB:
            *x = ntohs(*(unsigned short *) (cp + 4));
            *y = ntohs(*(unsigned short *) (cp + 6));
            *buttonmask = *(unsigned char *) (cp + 8);
            got_mouse = 1;
            break;

        case MSABS_IDENT_SESSIONID:
            *sessionId = GET_PROTOCOL_UNIT_VALUE(cp, plen);
            break;
        }
        cp = &cp[plen];
        i += plen;
    }
    return got_mouse;
}

int v2dLibCreateAbsoluteKeyboard(
        uint8_t *buffer,
        uint32_t sessionId,
        uint32_t key,
        uint8_t down)
{
    unsigned char buff[5];
    unsigned char *buffp;
    unsigned char *cp;
    int len = 0;
    int attrlen, attrid, attrval;

    /* Create keyboard buffer */
    buffp = buff;
    SET_PROTOCOL_VALUE_LONG(buffp, key);
    buff[4] = down ? 1 : 0;

    /* Create the packet */
    cp = (unsigned char *) buffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_KBABS);
    cp += 4;
    len += 4;

    attrid = KBABS_IDENT_RFB;
    attrlen = 5;
    FILL_PROTOCOL_UNIT(attrlen, attrid, buff, cp);
    len += attrlen+4;
    cp += attrlen+4;

    attrid = KBABS_IDENT_SESSIONID;
    attrval = sessionId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    cp = buffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: returning %d bytes:\n", __func__, len);
    v2dLibDumpHex((unsigned char *) buffer, len);
#endif

    return len;
}

int v2dLibCreateAbsoluteMouse(
        uint8_t *buffer,
        uint32_t sessionId,
        uint16_t x,
        uint16_t y,
        uint8_t buttonmask)
{
    unsigned char buff[5];
    unsigned char *buffp;
    unsigned char *cp;
    int len = 0;
    int attrlen, attrid, attrval;

    /* Create mouse buffer */
    buffp = buff;
    SET_PROTOCOL_VALUE_SHORT(buffp, x);
    SET_PROTOCOL_VALUE_SHORT(buffp + 2, y);
    buff[4] = buttonmask;

    /* Create the packet */
    cp = (unsigned char *) buffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_MSABS);
    cp += 4;
    len += 4;

    attrid = MSABS_IDENT_RFB;
    attrlen = 5;
    FILL_PROTOCOL_UNIT(attrlen, attrid, buff, cp);
    len += attrlen + 4;
    cp += attrlen + 4;

    attrid = MSABS_IDENT_SESSIONID;
    attrval = sessionId;
    attrlen = 4;
    FILL_PROTOCOL_UNIT_LONG(attrid, attrval, cp);
    len += attrlen+4;
    cp += attrlen+4;

    cp = buffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: returning %d bytes:\n", __func__, len);
    v2dLibDumpHex((unsigned char *) buffer, len);
#endif

    return len;
}

int v2dLibCreateFSRefresh(unsigned char * buffer)
{
    int len=0;
    unsigned char *cp;
    int attrlen, attrid, attrval;

    cp = buffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_AVREQ);
    cp += 4;
    len += 4;

    attrid = AVREQ_IDENT_FS_REFRESH;
    attrval = 1;
    attrlen = 2;
    FILL_PROTOCOL_UNIT_SHORT(attrid, attrval, cp);
    len += attrlen+4;
    cp += attrlen+4;

    cp = buffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

    return len;
}

unsigned long v2dLibGetRandomNumber()
{
    unsigned long rn = 0;

#ifdef WINDOWS
    struct timeval tv;
    static unsigned long seed = 0;
    if (seed == 0) {
        // First call sets the seed based on current time
        GETTIMEOFDAY(&tv, NULL);
        seed = tv.tv_sec + tv.tv_usec;
        srand(seed);
    }
    // Get the random number
    rn = (rand() << 16) + rand();
#else
    // Get the random number
    rn = random();
#endif
    // we do not want random to be randomly zero
    if (rn == 0) {
        struct timeval tv;
        GETTIMEOFDAY(&tv, NULL);
        rn = tv.tv_sec + tv.tv_usec;
    }

    return rn;
}

#ifndef WINDOWS

/**************************************************
 * Audio pack for all FPGA TX versions            *
 **************************************************/
int v2dLibAudioPack(unsigned char *obuf,
                    unsigned char *ibuf,
                    int ilen,
                    int atype) {

    unsigned char *bp;
    int i, len;

    /***********************************************************
     * First 8 bytes are video seq#, followed by 8-bytes audio *
     * seq#, then 4-byte audio SOF and 4 byte audio stream ID. *
     ***********************************************************/
    if (*((unsigned long *) &ibuf[20]) == AUDIOID_PCM16 &&
        atype >= AUDIOTYPE_MIN && atype <= AUDIOTYPE_MAX) {

        memcpy(obuf, ibuf, 24);
        bp = &obuf[24]; len = 24;

        switch (atype) {
        case AUDIOTYPE_MONO: 	/* copy 2 bytes, skip 2 bytes */
            for (i = 24; i < ilen - 8; i += 4) {
                memcpy(bp, &ibuf[i], 2);
                bp += 2; len += 2;
            }
            break;
        case AUDIOTYPE_8BIT:	/* copy 1 byte, skip 1 byte */
            for (i = 24; i < ilen - 8; i += 2) {
                *bp = ibuf[i];
                bp++; len++;
            }
            break;
        case AUDIOTYPE_MONO_8BIT:	/* copy 1 byte, skip 3 bytes */
            for (i = 24; i < ilen - 8; i += 4) {
                *bp = ibuf[i];
                bp++; len++;
            }
            break;
        default:
            break;
        }

        /********************************************************
         * The last 8 bytes are copied as is -- even though the *
         * audio end-of-frame is only 4 bytes, we copy (i.e. do *
         * not compress) the last 4 bytes of audio data since   *
         * we have to do everything in units of 8 bytes.        *
         ********************************************************/
        memcpy(bp, &ibuf[ilen - 8], 8);
        len += 8;
    }
    else {
        memcpy(obuf, ibuf, ilen);
        len = ilen;
    }

    return len;
}

/****************************************************
 * Audio unpack for FPGA version >= 7 (supports     *
 * configurable sample rate)                        *
 * PS: To support different audio types             *
 ****************************************************/
int v2dLibAudioUnPack(unsigned char *obuf,
                      unsigned char *ibuf,
                      int ilen,
                      int atype) {

    int i, len;
    unsigned char *bp;


    /* Bytes 20 to 24 are audio stream identifier */
    if (*((unsigned long *) &ibuf[20]) == AUDIOID_PCM16 &&
		atype >= AUDIOTYPE_MIN && atype <= AUDIOTYPE_MAX) {

        /* First 24 bytes are video seq, audio seq, audio SOF and stream ID */
        memcpy(obuf, ibuf, 24);
        bp = &ibuf[24]; len = 24;

        switch(atype) {
        case AUDIOTYPE_MONO:	/* copy 2 bytes, stuff 2 bytes */
            for (i = 24; i < ilen - 8; i += 2) {
                memcpy(&obuf[len], bp, 2);
                memcpy(&obuf[len+2], &obuf[len], 2);
                bp += 2; len += 4;
            }
            break;
        case AUDIOTYPE_8BIT:	/* copy 1 byte, stuff 1 byte */
            for (i = 24; i < ilen - 8; i++) {
                obuf[len] = *bp;
                obuf[len+1] = 0;
                bp++; len += 2;
            }
            break;
        case AUDIOTYPE_MONO_8BIT:/* copy 1 byte, stuff 3 bytes */
            for (i = 24; i < ilen - 8; i++) {
                obuf[len] = *bp;
                obuf[len+1] = 0;
                memcpy(&obuf[len+2], &obuf[len], 2);
                bp++; len += 4;
            }
            break;
        default:
            break;
        }

        /* The last 4 bytes of data and the 4-byte audio
           end-of-frame are copied as is. */
        memcpy(&obuf[len], &ibuf[ilen-8], 8);
        len += 8;
    }
    else {
        memcpy(obuf, ibuf, ilen);
        len = ilen;
    }

    return len;
}

/***************************************************
 *Fill header into the raw packet of KbBuffer read *
 ***************************************************/

int v2dLibBuildKbData(char *pKbInBuf, char *pKeyboardOutBuffer, int kbLen) {

    unsigned short len = 0, pl;
    unsigned char *cp;

    /***************
     * Fill header *
     ***************/
    cp = (unsigned char *) pKeyboardOutBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_KBDAT);
    cp += 4; len += 4;

    /*************************
     * Fill KB unit and data *
     *************************/
    pl = v2dLibFillProtocolUnit(kbLen, KBDAT_IDENT_MEDIUMRAW, pKbInBuf, cp);
    len += pl; cp += pl;

    /********************************
     * Fill the total packet length *
     ********************************/
    cp = (unsigned char *) pKeyboardOutBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: returning %d bytes:\n", __func__, len);
    v2dLibDumpHex((unsigned char *) pKeyboardOutBuffer, len);
#endif

    return len;
}

/***************************************************
 *Fill header into the raw packet of MsBuffer read *
 ***************************************************/
int v2dLibBuildMsData(char *pMsInBuf, char *pMouseOutBuffer, int msLen){

    unsigned short len = 0, pl;
    unsigned char *cp;

    /***************
     * Fill header *
     ***************/
    cp = (unsigned char *) pMouseOutBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_MSDAT);
    cp += 4; len += 4;

    /*************************
     * Fill MS unit and data *
     *************************/
    pl = v2dLibFillProtocolUnit(msLen, MSDAT_IDENT_BASIC, pMsInBuf, cp);
    len += pl; cp += pl;

    /********************************
     * Fill the total packet length *
     ********************************/
    cp = (unsigned char *) pMouseOutBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: returning %d bytes:\n", __func__, len);
    v2dLibDumpHex((unsigned char *) pMouseOutBuffer, len);
#endif

    return len;
}

/***************************************************
 *Fill header into the raw packet of SerialBuffer read *
 ***************************************************/
int v2dLibBuildSerialData(char *pSerInBuf, char *pSerialOutBuffer, int serLen){

    unsigned short len = 0, pl;
    unsigned char *cp;

    /***************
     * Fill header *
     ***************/
    cp = (unsigned char *) pSerialOutBuffer;
    SET_PROTOCOL_UNIT_ID(cp, IDENT_CLIENT_SERDAT);
    cp += 4; len += 4;

    /*************************
     * Fill Serial unit and data *
     *************************/
    pl = v2dLibFillProtocolUnit(serLen, SERDAT_IDENT_BASIC, pSerInBuf, cp);
    len += pl; cp += pl;

    /********************************
     * Fill the total packet length *
     ********************************/
    cp = (unsigned char *) pSerialOutBuffer;
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#ifdef DEBUG
    syslog(LOG_LOCAL5|LOG_INFO, "%s: returning %d bytes:\n", __func__, len);
    v2dLibDumpHex((unsigned char *) buffer, len);
#endif

    return len;
}

int v2dLibFillProtocolUnit(int len, int unit_id, char *buff, unsigned char *cp) {

    if (len > MAX_PROTOCOL_UNIT_SIZE )
        len = MAX_PROTOCOL_UNIT_SIZE;

    FILL_PROTOCOL_UNIT(len, unit_id, (unsigned char *) buff, cp);

    return len + 4;
}

int
v2dLibGetProtocolUnit(unsigned char *buff,
                      int buff_idx,
                      int buff_size,
                      int *unit_id,
                      unsigned char *cp) {
    int len;

    len = GET_PROTOCOL_UNIT_LENGTH(cp);

    if (len < 4) {
        LOG_SYS(V2DLIB_ERROR,
            "V2DLib: Received bad protocol unit of size %d\n", len);
        return -1;
    }
    len -= 4;

    *unit_id = GET_PROTOCOL_UNIT_ID(cp);

    if (len > MAX_PROTOCOL_UNIT_SIZE )
        len = MAX_PROTOCOL_UNIT_SIZE;
    if (buff_idx + len > buff_size) {
        memcpy(&buff[buff_idx], cp + 4, buff_size - buff_idx);
        memcpy(buff, cp + 4 + buff_size - buff_idx, len - buff_size + buff_idx);
    }
    else {
        memcpy(&buff[buff_idx], cp + 4, len);
    }

    return len;
}

#endif
