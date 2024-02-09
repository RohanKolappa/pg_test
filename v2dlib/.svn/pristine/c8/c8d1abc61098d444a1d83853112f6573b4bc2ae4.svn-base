#ifndef __HH_V2D_H__
#define __HH_V2D_H__

#include "v2dlib_compatibility.h"
#include "listutils.h"
#include "srd.h"

#ifdef WINDOWS
typedef int socklen_t;
typedef unsigned __int64    UINT64;
#endif


#define V2DLIB_ERROR    0
#define V2DLIB_WARN     1
#define V2DLIB_ALERT    2
#define V2DLIB_INFO     3
#define V2DLIB_VERBOSE  4

#define V2DSTATUS       "V2DSTATUS"
#define V2DNOTIFY       "V2DNOTIFY"

#define V2D_CTRL        1
#define V2D_DATA        2

#define SELECT_FREQ     1

#define CTLSENDINTERVAL     5000
#define DEFAULTSVRPORT      6060
#define DEFAULTVIDEOPORT    6060
#define DEFAULTAUDIOPORT    6061
#define DEFAULTMCASTTTL         1
#define MAX_RECV_INTERFACES     8

//
// Buffer Sizes
//
#define SEND_BUFFER_SIZE        1024
#define AV_BUFFER_SIZE          1040
#define REV_AV_BUFFER_SIZE      1052
#define KB_BUFFER_SIZE          60
#define MS_BUFFER_SIZE          60
#define SER_BUFFER_SIZE         1020

#define V2D_AUDIO_FRAME_DATAOFFSET 24
#define V2D_AUDIO_FRAME_SIZE       1012
/* 1 pkt=1012B, 2 channels, 16 bits/sample ==> 1012/(2*2) */
#define V2D_AUDIO_SAMPLESPERFRAME 253
#define V2D_AUDIO_SOF_CODE        435
#define V2D_AUDIO_EOF_CODE        439
#define V2D_PACKET_SEQNUM_LEN     8
#define V2D_PACKET_HEADER_SIZE    (2 * V2D_PACKET_SEQNUM_LEN)

//
// Server Message Types
//
#define IDENT_SERVER_VER        201 // unused legacy
#define IDENT_SERVER_REJ        202
#define IDENT_SERVER_CAP        203
#define IDENT_SERVER_SERDAT     204
#define IDENT_SERVER_CLS        205
#define IDENT_SERVER_RFBSTATUS  206
#define IDENT_SERVER_RTPFILTER  207

//
// Client Message Types
//
#define IDENT_CLIENT_CAP        301
#define IDENT_CLIENT_AVREQ      302
#define IDENT_CLIENT_CSTAT      304
#define IDENT_CLIENT_KBDAT      305
#define IDENT_CLIENT_MSDAT      306
#define IDENT_CLIENT_SERDAT     307
#define IDENT_CLIENT_KBABS      308
#define IDENT_CLIENT_MSABS      309
#define IDENT_CLIENT_RFBQUERY   310
#define IDENT_CLIENT_NETHDR     311

//
// Client Statitics Reporting
//
#define CSTAT_IDENT_VIDEORATE           701
#define CSTAT_IDENT_VIDEOBYTES          702
#define CSTAT_IDENT_VIDEOPACKETS        703
#define CSTAT_IDENT_VIDEOLOSS           704
#define CSTAT_IDENT_VIDEOOOO            705
#define CSTAT_IDENT_AUDIORATE           706
#define CSTAT_IDENT_AUDIOBYTES          707
#define CSTAT_IDENT_AUDIOPACKETS        708
#define CSTAT_IDENT_AUDIOLOSS           709
#define CSTAT_IDENT_AUDIOOOO            710

//
// Keyboard/Mouse/Serial Data Identifiers
//
#define KBDAT_IDENT_RAW                 801
#define KBDAT_IDENT_MEDIUMRAW           802
#define KBDAT_IDENT_SWCLIENT1           803
#define KBDAT_IDENT_SESSIONID           804

#define SERDAT_IDENT_BASIC              881

#define MSDAT_IDENT_BASIC               901
#define MSDAT_IDENT_SESSIONID           902

#define KBABS_IDENT_RFB                 1501
#define KBABS_IDENT_SESSIONID           1502

#define MSABS_IDENT_RFB                 1601
#define MSABS_IDENT_SESSIONID           1602

//
// Server Capabilities
//
#define SCAP_IDENT_VIDEO_NAME           401
#define SCAP_IDENT_VIDEO_HRES           402
#define SCAP_IDENT_VIDEO_VRES           403
#define SCAP_IDENT_VIDEO_REFRESH        404
#define SCAP_IDENT_VIDEO_PCR            405
#define SCAP_IDENT_VIDEO_HFP            406
#define SCAP_IDENT_VIDEO_HBP            407
#define SCAP_IDENT_VIDEO_HSYNC          408
#define SCAP_IDENT_VIDEO_VFP            409
#define SCAP_IDENT_VIDEO_VBP            410
#define SCAP_IDENT_VIDEO_VSYNC          411
#define SCAP_IDENT_VIDEO_CHANGED        412
#define SCAP_IDENT_MCAST_STATUS         413
#define SCAP_IDENT_MCAST_ADDRESS        414
#define SCAP_IDENT_VIDEO_PORT           415
#define SCAP_IDENT_VIDEO_MQUANT         416
#define SCAP_IDENT_VIDEO_DCQUANT        417
#define SCAP_IDENT_VIDEO_SLICESIZE      418
#define SCAP_IDENT_VIDEO_ACNOIZE        419
#define SCAP_IDENT_VIDEO_DCNOIZE        420
#define SCAP_IDENT_VIDEO_STEREO         421
#define SCAP_IDENT_VIDEO_COMPMODE       422
#define SCAP_IDENT_REV_AUDIO_PORT       423
#define SCAP_IDENT_AUDIO_TYPE           424
#define SCAP_IDENT_AUDIO_PORT           425
#define SCAP_IDENT_KB_TYPE              426
#define SCAP_IDENT_MS_TYPE              427
#define SCAP_IDENT_VIDEO_ID             428
#define SCAP_IDENT_VIDEO_ORIG_HRES      429
#define SCAP_IDENT_VIDEO_ORIG_VRES      430
#define SCAP_IDENT_VIDEO_MOTIONCOMPMODE 431
#define SCAP_IDENT_VIDEO_CODEC_VERSION  435
#define SCAP_IDENT_AUDIO_FREQ           437
#define SCAP_IDENT_PROTOCOL_VERSION     450

//
// Server RFB Status Messages
//
#define RFBSTAT_IDENT_STATUS            1001
#define RFBSTAT_IDENT_SESSIONID         1002
#define RFBSTAT_IDENT_QUERYID           1003

//
// Client RFB Status Query Messages
//
#define RFBQUERY_IDENT_SESSIONID        1101
#define RFBQUERY_IDENT_QUERYID          1102

//
// Destination Information Messages
//
#define NETHDR_IDENT_DST_MAC0           1201
#define NETHDR_IDENT_DST_MAC1           1202
#define NETHDR_IDENT_DST_MAC2           1203
#define NETHDR_IDENT_SRC_VIDEO_PORT     1204
#define NETHDR_IDENT_DST_VIDEO_PORT     1205
#define NETHDR_IDENT_DST_VIDEO_ADDR     1206
#define NETHDR_IDENT_SRC_AUDIO_PORT     1207
#define NETHDR_IDENT_DST_AUDIO_PORT     1208
#define NETHDR_IDENT_DST_AUDIO_ADDR     1209
#define NETHDR_IDENT_TIME_TO_LIVE	    1210

//
// Server RTP Filter Messages
//
#define RTPFILTER_IDENT_VPORT           1301
#define RTPFILTER_IDENT_VAES            1302
#define RTPFILTER_IDENT_APORT           1303
#define RTPFILTER_IDENT_AAES            1304
#define RTPFILTER_IDENT_MULTICASTADDR   1305

//
// HTTP REQUEST/RESPONSE
//
#define HTTP_REQUEST_SIZE               1024
#define HTTP_RESPONSE_SIZE              1024

extern const char * HTTPRequestFmt;
extern const char * HTTPResponseFmt;

//
// Server States
//
#define SERVER_STATE_DISABLED           0
#define SERVER_STATE_IDLE               1
#define SERVER_STATE_READY              2

//
// Client States
//
#define CLIENT_STATE_DISABLED           0
#define CLIENT_STATE_IDLE               1
#define CLIENT_STATE_CONNECTING         2
#define CLIENT_STATE_SENDHTTPREQ        3
#define CLIENT_STATE_RECVHTTPRESP       4
#define CLIENT_STATE_CONNECTED          5
#define CLIENT_STATE_EXP_DATA_REQ       6
#define CLIENT_STATE_SEND_DATA_REQ      7
#define CLIENT_STATE_READY              10

//
// Client AV Flags
//
#define AV_HTTP_CONNECT                 1
#define AV_HTTP_REQUEST                 2
#define AV_HTTP_RESPONSE                3

//
// Client Request Parameters
//
#define AVREQ_IDENT_VIDEO_PORT          501
#define AVREQ_IDENT_AV_BW               502
#define AVREQ_IDENT_AV_MULTICAST        503
#define AVREQ_IDENT_AV_MQUANT           504
#define AVREQ_IDENT_AV_DCQUANT          505
#define AVREQ_IDENT_AV_QUALITY          506
#define AVREQ_IDENT_AV_SLICESIZE        507
#define AVREQ_IDENT_AV_ACNOIZE          508
#define AVREQ_IDENT_AV_DCNOIZE          509
#define AVREQ_IDENT_AUDIO_PORT          510
#define AVREQ_IDENT_AV_OPTION           511
#define   AVOPTION_VIDEO                1
#define   AVOPTION_AUDIO                2
#define   AVOPTION_AUDIO_VIDEO          3
#define AVREQ_IDENT_AUDIO_TYPE          512
#define   AUDIOTYPE_DEFAULT             0
#define   AUDIOTYPE_MONO                1
#define   AUDIOTYPE_8BIT                2
#define   AUDIOTYPE_MONO_8BIT           3
#define   AUDIOID_PCM16                 0x00000000
#define   AUDIOTYPE_MIN                 AUDIOTYPE_MONO
#define   AUDIOTYPE_MAX                 AUDIOTYPE_MONO_8BIT
#define AVREQ_IDENT_FS_REFRESH          514
#define AVREQ_IDENT_VIDEO_ADDRESS       515
#define AVREQ_IDENT_AUDIO_ADDRESS       516
#define AVREQ_IDENT_AV_TTL              517

#define DEFAULT_AVOPTION        AVOPTION_VIDEO
#define MONOSTEREO_STEREO       0
#define MONOSTEREO_MONO         1
#define DEFAULT_MONOSTEREO      MONOSTEREO_STEREO

#define AVREQ_IDENT_PROTOCOL_VERSION    550
#define AVREQ_IDENT_IPADDR              551
#define AVREQ_IDENT_NO_SLICEDROP        552
#define AVREQ_IDENT_MIN_COMPRESSION     553
#define AVREQ_IDENT_MAX_COMPRESSION     554
#define AVREQ_IDENT_BURST_RATIO         555
#define AVREQ_IDENT_BURST_DURATION      556
#define AVREQ_IDENT_AUTH_TOKEN          557


// To convert the float to int and insert to AVREQ
#define BURST_RATIO_CONVERSION_FACTOR  1000

//
// Server Reject Errors
//
#define SREJ_IDENT_BASE                 600
#define SREJ_IDENT_AUTHERR              601
#define   SREJ_ERR_BAD_NETWORK            1
#define   SREJ_ERR_NO_UNICAST             2
#define   SREJ_ERR_NO_MULTICAST_INIT      3
#define   SREJ_ERR_NO_MULTICAST_PART      4
#define SREJ_IDENT_PROTOERR             602
#define   SREJ_ERR_MALFORMED_PACKET       1
#define   SREJ_ERR_UNEXPECTED_ID          2
#define SREJ_IDENT_REQERR               603
#define   SREJ_ERR_UNICAST_ONLY           1
#define   SREJ_ERR_MULTICAST_ONLY         2
#define   SREJ_ERR_ALREADY_CONNECTED      3
#define   SREJ_ERR_MULTICAST_MISMATCH     4
#define SREJ_IDENT_SERVERR              604
#define   SREJ_ERR_UNKNOWN_ERR            1
#define   SREJ_ERR_SERVER_DOWN            2
#define   SREJ_ERR_CONTROLLER_GONE        3
#define   SREJ_ERR_SERVER_PREMPTED        4

//
// Status Codes
//
#define STATUSCODE_CLEAR            0
#define STATUSCODE_WRITEERR         1
#define STATUSCODE_READERR          2
#define STATUSCODE_CONNECTING       3
#define STATUSCODE_REFUSED          4
#define STATUSCODE_TIMEDOUT         5
#define STATUSCODE_UNREACHABLE      6
#define STATUSCODE_HOSTUNREACHABLE  7
#define STATUSCODE_DISCONNECTING    8
#define STATUSCODE_CONNECTED        9
#define STATUSCODE_WAITING          10
#define STATUSCODE_SERVERERR        11
#define STATUSCODE_MULTICASTERR     12
#define STATUSCODE_UNICASTERR       13
#define STATUSCODE_SERVERBUSY       14
#define STATUSCODE_AUTHERR          15
#define STATUSCODE_PROTERR          16
#define STATUSCODE_SOCKERR          17
#define STATUSCODE_STIMEDOUT        18
#define STATUSCODE_PREMPTED         19
#define STATUSCODE_MISMATCHERR      20
#define STATUSCODE_AUTHENTICATING   21
#define STATUSCODE_UNKNOWN          50

//
// Shared memory flags
//
#define SHM_FLAG_MULTICAST      0x00000001
#define SHM_FLAG_VIDEO_ONLY     0x00000010
#define SHM_FLAG_AUDIO_ONLY     0x00000020

//
// AV Descriptors
//
#define AVDESC_IDENT_AV_OPTION          701
#define AVDESC_IDENT_AUDIO_TYPE         702

//
// Client request types
//
#define CREQ_TYPE_UNICAST               0
#define CREQ_TYPE_MULTI_INIT            1
#define CREQ_TYPE_MULTI_JOIN            2

//
// Rx pending task flags
//
#define RX_SEND_RFB_QUERY                0x00001
#define RX_SEND_FS_REFRESH               0x00002

//
// Tx pending task flags
//
#define TX_SEND_DISCONNECT               0x00001
#define TX_PREEMPT_CONNECTION            0x00002
#define TX_KICK_CONNECTION               0x00004
#define TX_DISCONNECT_CLIENT             0x00008

//
// Return codes
//
#define RC_NORMAL                       0
#define RC_WRITEERROR                   1
#define RC_READERROR                    2
#define RC_SOCKERR                      3
#define RC_CONNECTING                   4
#define RC_REFUSED                      5
#define RC_TIMEDOUT                     6
#define RC_UNREACHEABLE                 7
#define RC_HOSTUNREACHABLE              8
#define RC_DISCONNECTING                9
#define RC_WAITING                      10
#define RC_SERVER_TIMEDOUT              11
#define RC_SERVER_CLS                   12
#define RC_CONNECT_ERR                  13
#define RC_INTERNAL_ERR                 14

#define RC_AUTH_ERR                     40
#define RC_AUTH_BAD_NETWORK             41
#define RC_AUTH_NO_UNICAST              42
#define RC_AUTH_NO_MULTICAST_INIT       43
#define RC_AUTH_NO_MULTICAST_PART       44

#define RC_PROTO_ERR                    50
#define RC_PROTO_MALFORMED_PACKET       51
#define RC_PROTO_UNEXPECTED_ID          52

#define RC_REQ_ERR                      60
#define RC_REQ_UNICAST_ONLY             61
#define RC_REQ_MULTICAST_ONLY           62
#define RC_REQ_ALREADY_CONNECTED        63
#define RC_REQ_MULTICAST_MISMATCH       64

#define RC_SERV_ERR                     70
#define RC_SERV_UNKNOWN_ERR             71
#define RC_SERV_SERVER_DOWN             72
#define RC_SERV_CONTROLLER_GONE         73
#define RC_SERV_SERVER_PREMPTED         74

#define RC_UNKNOWN_ERR                  100
#define RC_CRITICAL_ERR                 101

//
// Flags
//
#define O_CTL       0x00000001
#define O_VIDEO     0x00000002
#define O_AUDIO     0x00000004
#define O_KB        0x00000010
#define O_MS        0x00000020
#define O_SER       0x00000040
#define I_CTL       0x00000100
#define I_VIDEO     0x00000200
#define I_AUDIO     0x00000400
#define I_KB        0x00001000
#define I_MS        0x00002000
#define I_SER       0x00004000


/* reference clock multiplication factor */
#define DDSFFACTOR      297.0

#define TwoRaised4      (double)16.000000
#define TwoRaised8      (double)256.000000
#define TwoRaised16     (double)65536.000000
#define TwoRaised24     (double)16777216.000000
#define TwoRaised32     (double)4294967296.000000
#define TwoRaised40     (double)1099511627776.000000
#define TwoRaised48     (double)281474976710656.000000

#define HI_INT  2147483647.0

#define FLOOR(val,significance)         (Floor((double) val))
double Floor(double val);
#define MOD(val, radix)         (Mod((double) val, (double) radix))
double Mod(double val, double radix);

//from /V2O/modules/commonutils.h
#define SERVER_ADDRESS_LEN      64


#define SPLASH_MQUANT           3
#define SPLASH_DCQUANT          3


#define SPLASH_TYPE_GRID    0x00
#define SPLASH_TYPE_TX      0x01
#define SPLASH_TYPE_BLUE    0x02
#define SPLASH_TYPE_BLACK   0x04
#define SPLASH_TYPE_WHITE   0x08
#define SPLASH_TYPE_CBAR    0x10

#define SAMPLERATE_16KHZ        0
#define SAMPLESIZE_16BIT        0
#define SAMPLESIZE_8BIT         1
#define DEFAULT_SAMPLESIZE      SAMPLESIZE_16BIT

#define VIDEO_PORT              6060

#define MAX_PROTOCOL_UNIT_SIZE      128

//from /V2O/modules/loadnet.h
#define BANDWIDTH_LEN           8

//from kbm_control.h
#define MS_ID_PS2               0
#define MS_ID_IMPS2             3
#define MS_TYPE_PS2             MS_ID_PS2
#define MS_TYPE_IMPS2           MS_ID_IMPS2
#define MS_PS2_PKTSIZE          6
#define MS_IMPS2_PKTSIZE        8
#define KB_TYPE_PC              2
#define KB_TYPE_SGI             3


#define LEG_AUDIO_FREQ 16384
#define NGS_AUDIO_FREQ 16000

//
// Macros
//
#define GET_PROTOCOL_VALUE_SHORT(cp) \
    get_protocol_value_short((cp))

#define SET_PROTOCOL_VALUE_SHORT(cp, val) \
    set_protocol_value_short((cp), (val))

#define GET_PROTOCOL_VALUE_LONG(cp) \
    get_protocol_value_long((cp))

#define SET_PROTOCOL_VALUE_LONG(cp, val) \
    set_protocol_value_long((cp), (val))

#define GET_PROTOCOL_PACKET_LENGTH(cp) \
    get_protocol_packet_length((cp))

#define SET_PROTOCOL_PACKET_LENGTH(cp, len) \
    set_protocol_packet_length((cp), (len))

#define FILL_PROTOCOL_UNIT(len, id, buff, cp) \
    fill_protocol_unit((len), (id), (buff), (cp))

#define FILL_PROTOCOL_UNIT_SHORT(id, val, cp) \
    fill_protocol_unit_short((id), (val), (cp))

#define FILL_PROTOCOL_UNIT_LONG(id, val, cp)  \
    fill_protocol_unit_long((id), (val), (cp))

#define GET_PROTOCOL_UNIT_LENGTH(cp) \
    GET_PROTOCOL_PACKET_LENGTH(cp)

#define SET_PROTOCOL_UNIT_LENGTH(cp, len) \
    SET_PROTOCOL_PACKET_LENGTH(cp, len);

#define GET_PROTOCOL_UNIT_ID(cp) \
    get_protocol_unit_id((cp))

#define SET_PROTOCOL_UNIT_ID(cp, id) \
    set_protocol_unit_id((cp), (id))

#define GET_PROTOCOL_UNIT_VALUE(cp, len) \
    ((len)==6)?get_protocol_unit_short((cp)) : \
    get_protocol_unit_long((cp))

#define GET_PROTOCOL_UNIT_VALUE_STR(str, cp, len) \
    memcpy((str), (cp)+4, (len)-4);

#define FILL_PROTOCOL_UNIT_STRING(len, id, str, cp) \
    fill_protocol_unit_string((len), (id), (str), (cp))

#define GET_PROTOCOL_UNIT_SHORT(cp) \
    get_protocol_unit_short(cp)

#define GET_PROTOCOL_UNIT_LONG(cp) \
    get_protocol_unit_long(cp)

#define RETCODE(r,e)    \
    (((r) << 16) | (e))

#define V2D_PROTOCOL_VERSION(major, minor) ((major) << 8 | (minor))
#define V2D_PROTOCOL_VERSION_MAJOR(version) (((version) >> 8) & 0xff)
#define V2D_PROTOCOL_VERSION_MINOR(version) ((version) & 0xff)

#define V2DGETSEQUENCENUM(PTR_PACKET) \
    ( *(unsigned int *)((PTR_PACKET) + V2D_PACKET_SEQNUM_LEN) )

// TODO: This is very close copy of "struct authlist_list" from
// V2O/modules v2dLibParsePolicy uses this to make policy checks in
// standalone mode. Need to avoid this duplication at some point.
// Possibly not do any policy checks in standalone mode in the future.
typedef struct policy_entry {
    struct sockaddr_in  network;
    struct sockaddr_in  netmask;
    int                 enable;
    int                 max_bandwidth;
    int                 min_compression;
    int                 max_compression;
    float               burst_ratio;
    int                 burst_duration;
    int                 unicast;
    int                 multicast_init;
    int                 multicast_part;
    int                 unicast_preemptible;
    struct list_head    policy_list;
} policy_entry_t;

struct splash_frame {
    unsigned char       *sf_buffer;
    int                 sf_len;
    int                 sf_hres;
    int                 sf_vres;
    int                 sf_mquant;
    int                 sf_dcquant;
    int                 sf_slicesize;
    int                 sf_type;
    int                 sf_number_of_frames;
    unsigned char       *sf_splash_buffer;
    int                 sf_splash_len;
};

typedef struct ScapData {

    char m_videoName[80];
    int m_hRes;
    int m_vRes;
    double m_videoRefresh;
    int m_mQuant;
    int m_dcQuant;
    int m_sliceSize;
    int m_videoRateChanged;
    int m_audioType;


    int m_videoNameSet;
    int m_hResSet;
    int m_vResSet;
    int m_videoRefreshSet;
    int m_mQuantSet;
    int m_dcQuantSet;
    int m_sliceSizeSet;
    int m_videoRateChangedSet;
    int m_audioTypeSet;


    int m_videoId;
    int m_mcastAddress;
    int m_videoPort;
    int m_audioPort;
    int m_revAudioPort;
    int m_kbType;
    int m_msType;
    int m_origHres;
    int m_origVres;
    double m_videoPcr;
    int m_videoHfp;
    int m_videoHbp;
    int m_videoHsync;
    int m_videoVfp;
    int m_videoVbp;
    int m_videoVsync;
    int m_videoCompmode;
    int m_videoMotionCompmode;
    int m_videoStereo;
    int m_videoCodecVersion;
    int m_protocolVersion;

    int m_videoIdSet;
    int m_mcastAddressSet;
    int m_videoPortSet;
    int m_audioPortSet;
    int m_revAudioPortSet;
    int m_kbTypeSet;
    int m_msTypeSet;
    int m_origHresSet;
    int m_origVresSet;
    int m_videoPcrSet;
    int m_videoHfpSet;
    int m_videoHbpSet;
    int m_videoHsyncSet;
    int m_videoVfpSet;
    int m_videoVbpSet;
    int m_videoVsyncSet;
    int m_videoCompmodeSet;
    int m_videoMotionCompmodeSet;
    int m_videoStereoSet;
    int m_videoCodecVersionSet;
    int m_protocolVersionSet;
    long m_nAudioFreqency;
    int  m_nAudioFreqSet;

} ScapData_t;

typedef struct AVReqData {

    struct sockaddr_in m_clientAddr;
    int m_mQuant;
    int m_dcQuant;
    int m_sliceSize;
    int m_videoRate;
    int m_audioType;
    int m_avoption;
    int m_slicedrop;
    int m_multicast;
    int m_minComp;
    int m_maxComp;
    float m_burstRatio;
    int m_burstDuration;

    int m_clientAddrSet;
    int m_mQuantSet;
    int m_dcQuantSet;
    int m_sliceSizeSet;
    int m_videoRateSet;
    int m_audioTypeSet;
    int m_avoptionSet;
    int m_slicedropSet;
    int m_multicastSet;
    int m_minCompSet;
    int m_maxCompSet;
    int m_burstRatioSet;
    int m_burstDurationSet;

} AVReqData_t;

typedef struct NetHdrData {

    unsigned short dst_mac[3];
    int video_set;
    unsigned short src_video_port;
    unsigned short dst_video_port;
    unsigned long dst_video_addr;
    int audio_set;
    unsigned short src_audio_port;
    unsigned short dst_audio_port;
    unsigned long dst_audio_addr;
    unsigned char ttl;

} NetHdrData_t;

typedef struct RtpFilter {

    int video_set;
    unsigned short video_port;
    unsigned short video_aes;
    int audio_set;
    unsigned short audio_port;
    unsigned short audio_aes;
    int multicastaddr_set;
    unsigned int multicastaddr;
} RtpFilter_t;

typedef struct NVPair {
    int name;
    char value[80];
} NVPair_t;

typedef struct V2DRx {
    int m_nRxState;
    int m_nAVPort;

    int m_nSvrPort;
    int m_nCtlSock;
    struct sockaddr_in m_Destination;
    unsigned char *m_CtlSnd;
    unsigned char *m_CtlRcv;
    int m_CtlRcvLength;
    int m_nSerialRcvd;
    struct timeval m_tLastStatistics;
    struct timeval m_tLastCtlConnect;
    struct timeval m_tLastFailedConnect;
    struct timeval m_tLastCtlRead;
    int m_nCtrlSndPkts;
    int m_nCtrlSndBytes;
    int m_nCtrlRcvPkts;
    int m_nCtrlRcvBytes;
    int m_nBandwidth;
    int m_nMulticast;
    int m_nVideoPort;
    int m_nAudioPort;
    int m_nAVOption;
    int m_nAudioType;
    char m_MulticastAddress[32];

    int m_nVideoSock;
    int m_nHTTPVideoSock;
    int m_nVideoFlags;
    unsigned char *m_VideoRcv;
    unsigned char *m_RedVideoRcv;
    int m_nVideoLen;
    int m_nVideoRate;
    int m_nVideoPkts;
    int m_nVideoBytes;
    int m_nVideoLoss;
    int m_nVideoOutOfOrder;
    int m_nCurrVideoRate;
    int m_nCurrVideoPkts;
    int m_nCurrVideoBytes;
    int m_nCurrVideoLoss;
    int m_nCurrVideoOutOfOrder;
    int m_nVideoLastRate;
    int m_nVideoLastPkts;
    int m_nVideoLastBytes;
    int m_nVideoLastLoss;
    int m_nVideoLastOutOfOrder;
    unsigned int m_nVideoSeq;

    int m_nAudioSock;
    int m_nHTTPAudioSock;
    int m_nAudioFlags;
    unsigned char *m_AudioRcv;
    int m_nAudioLen;
    int m_nAudioRate;
    int m_nAudioPkts;
    int m_nAudioBytes;
    int m_nAudioLoss;
    int m_nAudioOutOfOrder;
    int m_nCurrAudioRate;
    int m_nCurrAudioPkts;
    int m_nCurrAudioBytes;
    int m_nCurrAudioLoss;
    int m_nCurrAudioOutOfOrder;
    int m_nAudioLastRate;
    int m_nAudioLastPkts;
    int m_nAudioLastBytes;
    int m_nAudioLastLoss;
    int m_nAudioLastOutOfOrder;
    unsigned int m_nAudioSeq;

    int m_nHTTPPort;
    int m_bUseHTTP;

    unsigned long m_nFlag;
    unsigned int m_nErrCode;
    int m_bSendNotify;
    int m_bSendError;
    int m_bSliceRed;
    unsigned long m_nResetCount;
    unsigned short m_version;
    unsigned long m_nSessionId;
    unsigned long m_nQueryId;
    int m_bRFBStatus;
    unsigned long m_RFBMouseSent;
    unsigned long m_RFBKeyboardSent;
    unsigned long m_nCtlPacketsDropped;
    unsigned long m_nCtlFlags;
    int m_statuscode;
    unsigned long m_nReconnectPeriod;

    NVPair_t m_Creq[10], m_Scap[50];
    int      m_nNumCreq, m_nNumScap;
    sliceRedData_t *sliceRdata;

    char m_sAuthToken[80];
    int m_bRecvRTP;
    int m_nPureRTPMode;
    int m_recvInterfaceCount;
    struct in_addr m_recvInterfaces[MAX_RECV_INTERFACES];
    uint64_t m_current_time;
} V2DRx_t;

typedef struct V2DAv {

    int m_nId; // unique id for debugging
    int m_nVideoSock;
    struct sockaddr_in m_VideoDestination;
    int m_isMulticast;
    // unsigned char *m_VideoSnd;
    unsigned char *m_VideoSndPtr;
    int m_nVideoLen;
    int m_nVideoRate;
    int m_nVideoPkts;
    int m_nVideoBytes;
    int m_nVideoLoss;
    int m_nVideoOutOfOrder;
    int m_nCurrVideoRate;
    int m_nCurrVideoPkts;
    int m_nCurrVideoBytes;
    int m_nCurrVideoLoss;
    int m_nCurrVideoOutOfOrder;
    int m_nVideoTxError;
    int m_nVideoTxThrottled;
    int m_nVideoLastRate;
    int m_nVideoLastPkts;
    int m_nVideoLastBytes;
    int m_nVideoLastLoss;
    int m_nVideoLastOutOfOrder;
    unsigned int m_nVideoSeq;

    int m_nAudioSock;
    struct sockaddr_in m_AudioDestination;
    // unsigned char *m_AudioSnd;
    unsigned char *m_AudioSndPtr;
    int m_nAudioLen;
    int m_nAudioRate;
    int m_nAudioPkts;
    int m_nAudioBytes;
    int m_nAudioLoss;
    int m_nAudioOutOfOrder;
    int m_nAudioTxError;
    int m_nCurrAudioRate;
    int m_nCurrAudioPkts;
    int m_nCurrAudioBytes;
    int m_nCurrAudioLoss;
    int m_nCurrAudioOutOfOrder;
    int m_nAudioLastRate;
    int m_nAudioLastPkts;
    int m_nAudioLastBytes;
    int m_nAudioLastLoss;
    int m_nAudioLastOutOfOrder;
    unsigned int m_nAudioSeq;

    struct timeval m_tLastStatistics;

    NVPair_t m_AVdesc[10];
    int      m_nNumAVdesc;

/*     sliceRedData_t *sliceRdata; */

    struct list_head m_TxHead;

    struct list_head  m_AvList;

} V2DAv_t;

typedef struct V2DTx {
    int m_nSvrState;
    int m_nSvrSock;
    int m_nSvrPort;
    int m_nListenPort;

    int m_nTTL;
    char m_McastAddress[32];
    int m_McastEnable;
    void (*m_scapCallback)(void *object, ScapData_t *scap);
    void * m_scapCallbackObject;
    void (*m_listenerCallback)(void *object, int fd);
    void * m_listenerCallbackObject;
    void (*m_errorCallback)(void *object);
    void * m_errorCallbackObject;
    void (*m_notificationCallback)(void *object);
    void * m_notificationCallbackObject;
    int m_nTxState;
    int m_bControlling;
    int m_nCtlSock;
    int m_bUseHTTP;
    int m_nHTTPVideoSock;
    int m_nHTTPAudioSock;
    struct sockaddr_in m_Destination;
    unsigned char *m_CtlSnd;
    unsigned char *m_CtlRcv;
    int m_CtlRcvLength;
    int m_bFirstCtrl;
    struct timeval m_tLastCtlSend;
    int m_nKeyboardRcvd;
    int m_nMouseRcvd;
    int m_nSerialRcvd;
    struct timeval m_tLastCtlRead;
    int m_nCtrlSndPkts;
    int m_nCtrlSndBytes;
    int m_nCtrlRcvPkts;
    int m_nCtrlRcvBytes;
    unsigned long m_nRequestBandwidth;
    unsigned short m_version;
    unsigned short m_requestVersion;
    unsigned long m_nCtlFlags;
    int m_statuscode;
    char m_videoName[80];
    V2DAv_t *m_AV;

    unsigned long m_nFlag;
    unsigned int m_nErrCode;
    int m_bSendNotify;
    int m_bSendError;
    unsigned long m_nResetCount;
    unsigned long m_nCtlPacketsDropped;

    int m_bIsV2DTx;
    int m_bPolicyCheck;
    unsigned long m_nKBMSessionId;
    char * m_sPolicyFileName;

    NVPair_t m_Creq[10];
    int      m_nNumCreq;

    struct list_head m_TxList;

    policy_entry_t *matched_policy_entry;

    int m_bSendRTP;
    int m_bUseSendInterface;
    struct in_addr m_sendInterface;
    struct in_addr m_expectedClient;
    char m_sAuthToken[80];
    char m_sClientAuthToken[80];
    int m_nSimultaneousConnectionsRefused;
    uint64_t m_current_time;
} V2DTx_t;

//enum types

//
// Common Function Prototypes
//

#ifdef __cplusplus
extern "C" {
#endif

    //todo void v2dLibSetReportMode(void);
    void v2dLibDumpHex(unsigned char dat[], int datsize);
    DLLEXPORT int  v2dLibStartup(void);
    DLLEXPORT int  v2dLibCleanup(void);
    DLLEXPORT void v2dLibSetLogger(void (*LoggerCallback)(int logLevel, char * logMessage));
    DLLEXPORT void v2dLibSetTime(unsigned long long currentTime);
    DLLEXPORT int v2dLibIsScap(char *pPayload);
    DLLEXPORT int v2dLibParseScap(ScapData_t *scap, char *pPayload);

#ifndef WINDOWS
    int v2dLibGetClientScap(ScapData_t *scap);
    int v2dLibAudioPack(unsigned char *obuf, unsigned char *ibuf, 
                        int ilen, int atype);
    int v2dLibAudioUnPack(unsigned char *obuf, unsigned char *ibuf,
                          int ilen, int atype);
    int v2dLibFillProtocolUnit(int len, int unit_id, char *buff, 
                               unsigned char *cp);
    int v2dLibGetProtocolUnit(unsigned char *buff, int buff_idx, int buff_size,
                              int *unit_id, unsigned char *cp);
    int v2dLibBuildKbData(char *kbInBuf, char *pKeyboardOutBuffer, int kbLen);
    int v2dLibBuildMsData(char *msInBuf, char *pMouseOutBuffer, int msLen);
    int v2dLibBuildSerialData(char *serInBuf, char *pSerialOutBuffer, int serLen);
#endif
    unsigned short get_protocol_value_short(unsigned char *cp);
    void set_protocol_value_short(unsigned char *cp, unsigned short val);
    unsigned long get_protocol_value_long(unsigned char *cp);
    void set_protocol_value_long(unsigned char *cp, unsigned long val);
    unsigned short get_protocol_packet_length(unsigned char *cp);
    void set_protocol_packet_length(unsigned char *cp, int len);
    void fill_protocol_unit(int blen, int id, unsigned char *buf, unsigned char *cp);
    void fill_protocol_unit_short(int id, unsigned short val, unsigned char *cp);
    void fill_protocol_unit_long(int id, unsigned long val, unsigned char *cp);
    void fill_protocol_unit_string(int slen, int id, char *str, unsigned char *cp);
    unsigned short get_protocol_unit_id(unsigned char *cp);
    void set_protocol_unit_id(unsigned char *cp, unsigned short id);
    unsigned short get_protocol_unit_short(unsigned char *cp);
    unsigned long get_protocol_unit_long(unsigned char *cp);

    DLLEXPORT int v2dLibParseAVReq(AVReqData_t *scap, char *pPayload);
    DLLEXPORT int v2dLibParsePolicy(struct in_addr clientipaddr,
                          struct policy_entry *matched_policy_entry,
                          char * policyfile);
    int v2dLibParseNetHeader(NetHdrData_t *hdr, char *pPayload);
    int v2dLibParseRtpFilter(RtpFilter_t *hdr, char *pPayload);
    int v2dLibDumpNetHeader(unsigned char *pPayload);
    DLLEXPORT  int v2dLibDumpAVReq(unsigned char *pPayload);
    DLLEXPORT int v2dLibDumpScap(char *pPayload);
    DLLEXPORT int nano_sleep(unsigned int nmillisec);
    DLLEXPORT char * v2dLibGetStatusString(char * buffer, int statuscode, int multicast,
            int http, int avoption);
    int getReturnCode(int sErrorCode);
    DLLEXPORT void v2dLibCloseSocket(int nSock);
    DLLEXPORT int v2dLibDisableNagling(int nSock);
    DLLEXPORT char * getV2DProtocolVersion(unsigned short version, char * buf, int len);
    DLLEXPORT unsigned short getV2DProtocolVersionValue(char * buf);
    DLLEXPORT int v2dLibCreateRFBStatus(
            uint8_t * buffer,
            int status,
            uint32_t sessionId,
            uint32_t qyeryId);
    DLLEXPORT int v2dLibCreateRFBStatusQuery(
            uint8_t * buffer,
            uint32_t sessionId,
            uint32_t queryId);
    DLLEXPORT int v2dLibParseRFBStatusQuery(
            uint8_t * buffer,
            uint32_t * sessionId,
            uint32_t * queryId);
    DLLEXPORT int v2dLibParseRFBStatus(
            uint8_t * buffer,
            uint32_t * sessionId,
            uint32_t * queryId,
            int * status);
    int v2dLibDumpDstInfo(unsigned char *pPayload);
    DLLEXPORT int v2dLibParseAbsoluteKeyboard(
            uint8_t * buffer,
            uint32_t * key,
            uint8_t * down,
            uint32_t * sessionId);
    DLLEXPORT int v2dLibParseAbsoluteMouse(
            uint8_t * buffer,
            uint16_t * x,
            uint16_t * y,
            uint8_t * buttonmask,
            uint32_t * sessionId);
    DLLEXPORT int v2dLibCreateAbsoluteKeyboard(
            uint8_t *buffer,
            uint32_t sessionId,
            uint32_t key,
            uint8_t down);
    DLLEXPORT int v2dLibCreateAbsoluteMouse(
            uint8_t *buffer,
            uint32_t sessionId,
            uint16_t x,
            uint16_t y,
            uint8_t buttonmask);

    DLLEXPORT int v2dLibCreateFSRefresh(unsigned char * buffer);
    DLLEXPORT unsigned long v2dLibGetRandomNumber(void);

    struct splash_frame *v2dCreateSplashFrame(int hres,
                                         int vres,
                                         int mquant,
                                         int dcquant,
                                         int slicesize,
                                         int type,
                                         int frame_number);
    DLLEXPORT void v2dDestroySplashFrame(struct splash_frame * sf);

    void write_splash_to_file(struct splash_frame *sf, char * filename);
    int bits_to_bytes(const char *bitstring);

#ifdef __cplusplus
}
#endif

//
// Rx related Function Prototypes
//

#ifdef __cplusplus
extern "C" {
#endif
   DLLEXPORT V2DRx_t *Rx_Create2(char *dest,
                        int srvport,
                        int bandwidth,
                        int multicast,
                        int videoport,
                        int audioport,
                        int avoption,
                        int audiotype,
                        int bsrdreqd,
                        int httpoption,
                        int httpport);

    DLLEXPORT V2DRx_t *Rx_Create(char *dest,
                       int srvport,
                       int bandwidth,
                       int multicast,
                       int videoport,
                       int audioport,
                       int avoption,
                       int audiotype,
                       int bsrdreqd);

    DLLEXPORT void Rx_Destroy(V2DRx_t *s);
    DLLEXPORT int Rx_Go(V2DRx_t *s);
    DLLEXPORT int Rx_SetHTTPPort(V2DRx_t *s, int nHTTPPort);
    DLLEXPORT int Rx_SetRTPMode(V2DRx_t *s, const char * multicastAddress);
    DLLEXPORT int Rx_OpenMediaConnection(V2DRx_t *s);
    DLLEXPORT int Rx_CloseMediaConnection(V2DRx_t *s);
    DLLEXPORT void Rx_PrintStatistics(V2DRx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Rx_SendNotification(V2DRx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Rx_SendError(V2DRx_t *s, char *pBuffer, int nLen);
    DLLEXPORT const char * Rx_GetVideoName(V2DRx_t *s);
    DLLEXPORT int Rx_GetRFBStatus(V2DRx_t *s);
    int Rx_BuildRtpFilter(V2DRx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Rx_SendMouseEvent(V2DRx_t *s, unsigned short x, unsigned short y,
                           unsigned char buttonmask);
    DLLEXPORT void Rx_SendKeyboardEvent(V2DRx_t *s, unsigned long key, int down);
    DLLEXPORT void Rx_SendFSRefresh(V2DRx_t *s);
    DLLEXPORT void Rx_UseMcastRecvInterfaceList(V2DRx_t *s);
    DLLEXPORT int Rx_AddMcastReceiveInterface(V2DRx_t *s, struct in_addr interface_addr);
    DLLEXPORT void Rx_SetAuthToken(V2DRx_t *s, const char * authToken);
    DLLEXPORT const char * Rx_GetAuthToken(V2DRx_t *s);


#ifdef __cplusplus
}
#endif

//
// Tx related Function Prototypes
//

#ifdef __cplusplus
extern "C" {
#endif
    DLLEXPORT V2DTx_t *Tx_Create(int nSvrPort,
                       int nTTL,
                       char *sMCastAddr,
                       int isV2DTx,
                       int policyCheck,
                       char *sPolicyFileName,
                       char *sRequestVersion);
    DLLEXPORT void Tx_Destroy(V2DTx_t *s);
    DLLEXPORT int Tx_GetCreqType(V2DTx_t *s);
    DLLEXPORT int Tx_Go(V2DTx_t *s);
    DLLEXPORT int Tx_SetCtlSocket(V2DTx_t *s, int nSock, int bIsHttp);
    DLLEXPORT int Tx_SetVideoSocket(V2DTx_t *s, int nHTTPSock);
    DLLEXPORT int Tx_SetAudioSocket(V2DTx_t *s, int nHTTPSock);
    DLLEXPORT void Tx_PrintStatistics(V2DTx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Tx_SendNotification(V2DTx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Tx_SendError(V2DTx_t *s, char *pBuffer, int nLen);
    DLLEXPORT void Tx_DumpAvList(void);
    DLLEXPORT void Tx_DumpTxList(V2DAv_t *s);
    DLLEXPORT void Tx_SetScapCallback(V2DTx_t *s,
                            void (*scapCallback)(void *object, ScapData_t *scap),
                            void *object);
    DLLEXPORT void Tx_SetListenerCallback(V2DTx_t *s,
                            void (*listenerCallback)(void *object, int fd),
                            void *object);
    DLLEXPORT void Tx_SetErrorCallback(V2DTx_t *s,
                             void (*errorCallback)(void *object),
                             void *object);
    DLLEXPORT void Tx_SetNotificationCallback(V2DTx_t *s,
                                    void (*notificationCallback)(void *object),
                                    void *object);
    int Tx_BuildNetHeader(V2DTx_t *s, unsigned short mac[], char *pBuffer, int nLen);
    void Tx_ResetConnection(V2DTx_t *s);
    DLLEXPORT void Tx_SendFSRefresh(V2DTx_t *s);
    DLLEXPORT void Tx_DisconnectClients(V2DTx_t *s);
    DLLEXPORT int Tx_CheckForVideoLoss(V2DTx_t *s, char *pBuffer, const int payloadType);
    DLLEXPORT int Tx_CheckForAudioLoss(V2DTx_t *s, char *pBuffer, const int payloadType);
    DLLEXPORT int Tx_IsHTTPMode(V2DTx_t *s);
    DLLEXPORT void Tx_SetMcastSendInterface(V2DTx_t *s, struct in_addr interface_addr);
    DLLEXPORT void Tx_SetExpectedClient(V2DTx_t *s, const char * expectedClient);
    DLLEXPORT void Tx_SetAuthToken(V2DTx_t *s, const char * authToken);
    DLLEXPORT const char * Tx_GetAuthToken(V2DTx_t *s);
    DLLEXPORT const char * Tx_GetClientAuthToken(V2DTx_t *s);
    DLLEXPORT int Tx_GetControllingClientIP(V2DTx_t *s, char * buffer, int buffer_len);

#ifdef __cplusplus
}
#endif


#endif /*__HH_V2D_H__*/
