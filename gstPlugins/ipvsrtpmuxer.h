#ifndef __IPVS_RTP_MUXER_H__
#define __IPVS_RTP_MUXER_H__

#include <gst/gst.h>
#include <gst/base/gstcollectpads.h>

G_BEGIN_DECLS

#define GST_TYPE_IPVS_RTP_MUXER  (ipvsrtpmuxer_get_type())
#define GST_IPVS_RTP_MUXER(obj)  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_IPVS_RTP_MUXER, IpvsRtpMuxer))

typedef struct IpvsRtpMuxer IpvsRtpMuxer;
typedef struct IpvsRtpMuxerClass IpvsRtpMuxerClass;
typedef struct IpvsPadData IpvsPadData;

typedef GstBuffer * (*IpvsPadDataPrepareFunction) (GstBuffer * buf,
    IpvsPadData * data, IpvsRtpMuxer * mux);

typedef void (*IpvsPadDataFreePrepareDataFunction) (gpointer prepare_data);

struct IpvsRtpMuxer {
    GstElement parent;

    GstPad *pSrcPad;

    GstCollectPads *collect;

    gboolean bFirst;
    gint nPID;

    gboolean bUseFixedTS;

    guint64 g_nStartTime;

    //SDP Data
    gboolean bVideoPresent;
    gboolean bAudioPresent;

    const gchar *pVideoEncodingName;
    int nVideoPayload;
    int nVideoClockRate;
    const gchar *pSPS;
    
    const gchar *pAudioEncodingName;
    const gchar *pAudioEncodingParams;
    int nAudioPayload;
    int nAudioClockRate;
    const gchar *pAudioMode;
    const gchar *pAudioConfig;
    const gchar *pAudioSizeLen;
    const gchar *pAudioIndexLen;
    const gchar *pAudioDeltaLen;
};

struct IpvsRtpMuxerClass {
    GstElementClass parent_class;
};

struct IpvsPadData {
    GstCollectData collect;
    GstCaps *pCaps;
    gboolean bIsVideo;
    gint nPID;
};

GType ipvsrtpmuxer_get_type (void);

G_END_DECLS

#endif //__IPVS_RTP_MUXER_H__
