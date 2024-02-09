/*
Tested with pipeline:
    gst-launch-1.0 filesrc location=/home/durga/videos/boeing.mp4 ! qtdemux name=demux demux.video_0 ! queue ! rtph264pay pt=96 ! ipvsrtpmuxer name=mux ! ipvsmediawriter location=/home/durga/tmp sdp-file-path=/home/durga/public_html/sdp.sdp create-hls=true create-primary-thumbnail=true demux. ! queue ! rtpmp4gpay pt=97 ! mux.
*/

#include <string.h>
#include <arpa/inet.h>

#include <sys/time.h>

#define GLIB_DISABLE_DEPRECATION_WARNINGS

#include "ipvsrtpmuxer.h"
#include "mediawriter_cwrapper.h"

#ifndef PACKAGE
#define PACKAGE "barco"
#endif


GST_DEBUG_CATEGORY (ipvsrtpmuxer_debug);
#define GST_CAT_DEFAULT ipvsrtpmuxer_debug

enum
{
    PROP_0, //It is necessary. 
    PROP_USE_FIXED_TS
};


static GstStaticPadTemplate ipvsrtpmuxer_sink_factory =
    GST_STATIC_PAD_TEMPLATE ("sink_%d",
    GST_PAD_SINK,
    GST_PAD_REQUEST,
    GST_STATIC_CAPS ("application/x-rtp"));

static GstStaticPadTemplate ipvsrtpmuxer_src_factory =
    GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/ipvs-sframe"));

static void InitializeSDPParams (IpvsRtpMuxer *pMux);
static void FillVideoSDPParams (GstStructure *pStruct, IpvsRtpMuxer *pMux);
static void FillAudioSDPParams (GstStructure *pStruct, IpvsRtpMuxer *pMux);

static void ipvsrtpmuxer_set_property  (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void ipvsrtpmuxer_reset (IpvsRtpMuxer * mux);
static void ipvsrtpmuxer_dispose (GObject * object);

static GstFlowReturn ipvsrtpmuxer_collected_buffer (GstCollectPads * pads,
    GstCollectData * data, GstBuffer * buf, IpvsRtpMuxer * mux);

static gboolean ipvsrtpmuxer_sink_event (GstCollectPads * pads,
    GstCollectData * data, GstEvent * event, gpointer user_data);

static GstPad *ipvsrtpmuxer_request_new_pad (GstElement * element,
    GstPadTemplate * templ, const gchar * name, const GstCaps * caps);
static void ipvsrtpmuxer_release_pad (GstElement * element, GstPad * pad);

static GstStateChangeReturn ipvsrtpmuxer_change_state (GstElement * element,
    GstStateChange transition);

static void ipvsrtpmuxer_set_src_caps (IpvsRtpMuxer * mux);

static gboolean ipvsrtpmuxer_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event);

static GstFlowReturn ipvsrtpmuxer_create_stream (IpvsRtpMuxer * mux, 
                                        IpvsPadData * pPadData);

G_DEFINE_TYPE (IpvsRtpMuxer, ipvsrtpmuxer, GST_TYPE_ELEMENT)

#define parent_class ipvsrtpmuxer_parent_class

static void
InitializeSDPParams (IpvsRtpMuxer *pMux)
{
    if (pMux == NULL)
        return;

    pMux->bVideoPresent = FALSE;
    pMux->bAudioPresent = FALSE;

    pMux->pVideoEncodingName = NULL;
    pMux->nVideoPayload = 96;
    pMux->nVideoClockRate = 90000;
    pMux->pSPS = NULL;

    pMux->pAudioEncodingName = NULL;
    pMux->nAudioPayload = 97;
    pMux->nAudioClockRate = 16000;
    pMux->pAudioMode = NULL;
    pMux->pAudioConfig = NULL;
    pMux->pAudioSizeLen = NULL;
    pMux->pAudioIndexLen = NULL;
    pMux->pAudioDeltaLen = NULL;
}

static void
FillVideoSDPParams (GstStructure *pStruct, IpvsRtpMuxer *pMux)
{
    if (pStruct == NULL)
        return;

    const GValue *pValue = gst_structure_get_value (pStruct, "payload");
    if (pValue != NULL)
        pMux->nVideoPayload = g_value_get_int (pValue);

    pValue = gst_structure_get_value (pStruct, "clock-rate");
    if (pValue != NULL)
        pMux->nVideoClockRate = g_value_get_int (pValue);

    pValue = gst_structure_get_value (pStruct, "sprop-parameter-sets");
    if (pValue != NULL)
        pMux->pSPS = g_value_get_string (pValue);
}

static void
FillAudioSDPParams (GstStructure *pStruct, IpvsRtpMuxer *pMux)
{
    if (pStruct == NULL)
        return;

     const GValue *pValue = gst_structure_get_value (pStruct, "payload");
    if (pValue != NULL)
        pMux->nAudioPayload = g_value_get_int (pValue);

    pValue = gst_structure_get_value (pStruct, "clock-rate");
    if (pValue != NULL)
        pMux->nAudioClockRate = g_value_get_int (pValue);

    pValue = gst_structure_get_value (pStruct, "encoding-params");
    if (pValue != NULL)
        pMux->pAudioEncodingParams = g_value_get_string (pValue);

    pValue = gst_structure_get_value (pStruct, "mode");
    if (pValue != NULL)
        pMux->pAudioMode = g_value_get_string (pValue);

    pValue = gst_structure_get_value (pStruct, "config");
    if (pValue != NULL)
        pMux->pAudioConfig = g_value_get_string (pValue);

    pValue = gst_structure_get_value (pStruct, "sizelength");
    if (pValue != NULL)
        pMux->pAudioSizeLen = g_value_get_string (pValue);

    pValue = gst_structure_get_value (pStruct, "indexlength");
    if (pValue != NULL)
        pMux->pAudioIndexLen = g_value_get_string (pValue);

    pValue = gst_structure_get_value (pStruct, "indexdeltalength");
    if (pValue != NULL)
        pMux->pAudioDeltaLen = g_value_get_string (pValue);
}

static void 
ipvsrtpmuxer_set_property  (GObject * object, guint prop_id,
                   const GValue * value, GParamSpec * pspec)
{
    IpvsRtpMuxer *pMuxer = GST_IPVS_RTP_MUXER (object);

    switch (prop_id)
    {
        case PROP_USE_FIXED_TS:
            pMuxer->bUseFixedTS = TRUE;
            pMuxer->g_nStartTime = 10000;
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
ipvsrtpmuxer_class_init (IpvsRtpMuxerClass * klass)
{
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gst_element_class_add_pad_template (gstelement_class,
              gst_static_pad_template_get (&ipvsrtpmuxer_sink_factory));

    gst_element_class_add_pad_template (gstelement_class,
              gst_static_pad_template_get (&ipvsrtpmuxer_src_factory));

    gst_element_class_set_static_metadata (gstelement_class,
              "IPVS RTP Muxer", "Codec/Muxer",
              "Multiplexes media streams into an IPVS Stream",
              "Durga <durga.davuluri@barco.com>");

    gobject_class->dispose = ipvsrtpmuxer_dispose;

    gobject_class->set_property = ipvsrtpmuxer_set_property;

    g_object_class_install_property (gobject_class, PROP_USE_FIXED_TS,
    g_param_spec_boolean ("use-fixed-start-time", 
                          "Use Fixed Start Timestamp",
                          "Use fixed start timestamp", FALSE,
                          G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

    gstelement_class->request_new_pad = ipvsrtpmuxer_request_new_pad;
    gstelement_class->release_pad = ipvsrtpmuxer_release_pad;
    gstelement_class->change_state = ipvsrtpmuxer_change_state;
}

static void
ipvsrtpmuxer_init (IpvsRtpMuxer * mux)
{
    mux->pSrcPad =
          gst_pad_new_from_static_template (&ipvsrtpmuxer_src_factory, "src");
    gst_pad_use_fixed_caps (mux->pSrcPad);
    gst_pad_set_event_function (mux->pSrcPad,
                   GST_DEBUG_FUNCPTR (ipvsrtpmuxer_src_event));
    gst_element_add_pad (GST_ELEMENT (mux), mux->pSrcPad);

    mux->collect = gst_collect_pads_new ();
    gst_collect_pads_set_buffer_function (mux->collect,
                        (GstCollectPadsBufferFunction)
                        GST_DEBUG_FUNCPTR (ipvsrtpmuxer_collected_buffer), mux);

    gst_collect_pads_set_event_function (mux->collect,
      (GstCollectPadsEventFunction) GST_DEBUG_FUNCPTR (ipvsrtpmuxer_sink_event),
      mux);

    ipvsrtpmuxer_reset (mux);
}

static void
ipvsrtpmuxer_pad_reset (IpvsPadData * pad_data)
{
    pad_data->nPID = 0;
    
    if (pad_data->pCaps)
        gst_caps_unref(pad_data->pCaps);

    pad_data->pCaps = NULL;
}

static void
ipvsrtpmuxer_reset (IpvsRtpMuxer * mux)
{
    GSList *walk;

    mux->bUseFixedTS = FALSE;
    mux->bFirst = TRUE;

    //Init SDP Parameters
    InitializeSDPParams(mux);

    GST_COLLECT_PADS_STREAM_LOCK (mux->collect);
    for (walk = mux->collect->data; walk != NULL; walk = g_slist_next (walk))
        ipvsrtpmuxer_pad_reset ((IpvsPadData *) walk->data);
    GST_COLLECT_PADS_STREAM_UNLOCK (mux->collect);
}

static void
ipvsrtpmuxer_dispose (GObject * object)
{
    IpvsRtpMuxer *mux = GST_IPVS_RTP_MUXER (object);

    ipvsrtpmuxer_reset (mux);

    if (mux->collect)
    {
        gst_object_unref (mux->collect);
        mux->collect = NULL;
    }

    GST_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

static GstFlowReturn
ipvsrtpmuxer_create_stream (IpvsRtpMuxer * mux, IpvsPadData * pPadData)
{
    GstPad *pad;
    GstStructure *s;

    pad = pPadData->collect.pad;

    pPadData->pCaps = gst_pad_get_current_caps (pad);
    if (pPadData->pCaps == NULL)
    {
        goto not_negotiated;
    }

    GST_DEBUG_OBJECT (pad, "Creating stream with PID 0x%04x for caps %"
                            GST_PTR_FORMAT, pPadData->nPID, pPadData->pCaps);
    s = gst_caps_get_structure (pPadData->pCaps, 0);
    g_return_val_if_fail (s != NULL, FALSE);

    const gchar *pStrMedia = g_value_get_string( 
                               gst_structure_get_value(s, "media"));
    if (strcmp (g_ascii_strup(pStrMedia, -1), "VIDEO") == 0)
    {
        mux->bVideoPresent = TRUE;
        mux->pVideoEncodingName = g_value_get_string( 
                               gst_structure_get_value(s, "encoding-name"));
        if (strcmp (g_ascii_strup( mux->pVideoEncodingName, -1), "H264") == 0)
        {
            pPadData->bIsVideo = TRUE;
            FillVideoSDPParams(s, mux);
        }
    }
    else if (strcmp (g_ascii_strup(pStrMedia, -1), "AUDIO") == 0)
    {
        mux->bAudioPresent = TRUE;
        mux->pAudioEncodingName = g_value_get_string( 
                               gst_structure_get_value(s, "encoding-name"));
        if (strcmp (g_ascii_strup(mux->pAudioEncodingName, -1), "MPEG4-GENERIC") == 0)
        {
            pPadData->bIsVideo = FALSE;
            FillAudioSDPParams(s, mux);
        }
    }
    else
    {
        pPadData->bIsVideo = FALSE;
    }

    return GST_FLOW_OK;

not_negotiated:
    {
        GST_DEBUG_OBJECT (pad, "Sink pad caps were not set before pushing");
        if (pPadData->pCaps)
            gst_caps_unref (pPadData->pCaps);
        return GST_FLOW_NOT_NEGOTIATED;
    }
}

static GstFlowReturn
ipvsrtpmuxer_create_streams (IpvsRtpMuxer * mux)
{
    GstFlowReturn ret = GST_FLOW_OK;
    GSList *walk = mux->collect->data;

    /* Create the streams */
    while (walk)
    {
        IpvsPadData *pPadData = (IpvsPadData *) walk->data;

        walk = g_slist_next (walk);

        ret = ipvsrtpmuxer_create_stream (mux, pPadData);
        if (ret != GST_FLOW_OK)
            goto no_stream;
    }

    return ret;

no_stream:
    {
        GST_ELEMENT_ERROR (mux, STREAM, MUX,
                    ("Could not create handler for stream"), (NULL));
        return ret;
    }
}


#define COLLECT_DATA_PAD(collect_data) (((GstCollectData *)(collect_data))->pad)

static gboolean
ipvsrtpmuxer_sink_event (GstCollectPads * pads, GstCollectData * data,
    GstEvent * event, gpointer user_data)
{
    return gst_collect_pads_event_default (pads, data, event, FALSE);
}

static gboolean
ipvsrtpmuxer_src_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
    return gst_pad_event_default (pad, parent, event);
}

static GstFlowReturn
ipvsrtpmuxer_collected_buffer (GstCollectPads * pads, GstCollectData * data,
    GstBuffer * buf, IpvsRtpMuxer * mux)
{
    gsize nBufSize = 0;
    GstBuffer *pOutBuff = NULL;
    GstBuffer *pExtraBuff = NULL;
    gint64 nPos;
    static gint64 nPrevTS;
    gint nExtraSize;
    struct IPVSHeaderInfo headerInfo;
    GstFlowReturn ret = GST_FLOW_OK;
    IpvsPadData *pPadData = (IpvsPadData *) data;

    GST_DEBUG_OBJECT (mux, "Pads collected");

    if (G_UNLIKELY (mux->bFirst))
    {
        ret = ipvsrtpmuxer_create_streams (mux);
        if (G_UNLIKELY (ret != GST_FLOW_OK))
            return ret;

        ipvsrtpmuxer_set_src_caps(mux);
        mux->bFirst = FALSE;

        //For test cases, we can use fixed start of time.
        if (mux->bUseFixedTS == FALSE) {
            struct timeval tv;
            gettimeofday(&tv, NULL);
 
            mux->g_nStartTime = (((tv.tv_sec * 1000000LL) + (tv.tv_usec)) / 1000) ;
        }
    }

    if (G_UNLIKELY ( pPadData == NULL)) 
    {
        /* EOS */
        gst_pad_push_event (mux->pSrcPad, gst_event_new_eos ());

        return GST_FLOW_OK;
    }

    g_assert (buf != NULL);
    
    nBufSize = gst_buffer_get_size(buf);
    nExtraSize = sizeof(headerInfo);

    if (GST_BUFFER_DTS_IS_VALID(buf))
        nPos = GST_BUFFER_DTS(buf) / 1000000;
    else
        nPos = GST_BUFFER_PTS(buf) / 1000000;

    if (nPrevTS > nPos)
    {
        nPos = nPrevTS;
    }

    headerInfo.nWallClock = htonll (nPos + mux->g_nStartTime); 
    headerInfo.nFlags = htonl(getRTPMediaFlags((pPadData->bIsVideo == TRUE) ? 1 : 0));

    pExtraBuff = gst_buffer_new_allocate(NULL, nExtraSize, NULL);
    gst_buffer_fill (pExtraBuff, 0, (gpointer*)&headerInfo, sizeof(headerInfo));
    
    pOutBuff = gst_buffer_append(pExtraBuff, buf);

    gst_pad_push (mux->pSrcPad, pOutBuff);

    nPrevTS = nPos;

    return GST_FLOW_OK;
}

static GstPad *
ipvsrtpmuxer_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * name, const GstCaps * caps)
{
    IpvsRtpMuxer *mux = GST_IPVS_RTP_MUXER (element);
    gint pid = ++(mux->nPID);
    gchar *pad_name = NULL;
    GstPad *pad = NULL;
    IpvsPadData *pad_data = NULL;

    pad_name = g_strdup_printf ("sink_%d", pid);
    pad = gst_pad_new_from_template (templ, pad_name);
    g_free (pad_name);
  
    pad_data = (IpvsPadData *)
             gst_collect_pads_add_pad (mux->collect, pad, sizeof (IpvsPadData),
             (GstCollectDataDestroyNotify) (ipvsrtpmuxer_pad_reset), TRUE);
    if (pad_data == NULL)
    {
        goto pad_failure;
    }

    ipvsrtpmuxer_pad_reset (pad_data);
    pad_data->nPID = pid;

    if (G_UNLIKELY (!gst_element_add_pad (element, pad)))
    {
        goto could_not_add;
    }

    return pad;

could_not_add:
    {
        GST_ELEMENT_ERROR (element, STREAM, FAILED,
             ("Internal data stream error."), ("Could not add pad to element"));
        gst_collect_pads_remove_pad (mux->collect, pad);
        gst_object_unref (pad);
    
        return NULL;
    }
pad_failure:
    {
        GST_ELEMENT_ERROR (element, STREAM, FAILED,
         ("Internal data stream error."), ("Could not add pad to collectpads"));
    gst_object_unref (pad);

        return NULL;
    }
}

static void
ipvsrtpmuxer_release_pad (GstElement * element, GstPad * pad)
{
    IpvsRtpMuxer *mux = GST_IPVS_RTP_MUXER (element);

    GST_DEBUG_OBJECT (mux, "Pad %" GST_PTR_FORMAT " being released", pad);

    if (mux->collect)
    {
        gst_collect_pads_remove_pad (mux->collect, pad);
    }

    gst_element_remove_pad (element, pad);
}

static void
ipvsrtpmuxer_set_src_caps (IpvsRtpMuxer * mux)
{
    gchar *pSDPData = NULL;

    //Session Data
    pSDPData = g_strdup_printf("v=0\n\no=- 0 0 IN IP4 127.0.0.1\n\ns=Media Presentation\n\nc=IN IP4 127.0.0.1\n\nt=0 0\n");

    //If Video Present
    if (mux->bVideoPresent == TRUE)
    {
        pSDPData = g_strdup_printf("%s\nm=video 6060 RTP/AVP %d\n\na=rtpmap:%d %s/%d\n\na=fmtp:%d sprop-parameter-sets=%s\n", pSDPData, mux->nVideoPayload, mux->nVideoPayload, g_ascii_strup(mux->pVideoEncodingName, -1), mux->nVideoClockRate, mux->nVideoPayload, mux->pSPS);
    }

    //If Audio Present
    if (mux->bAudioPresent == TRUE)
    {
        pSDPData = g_strdup_printf("%s\nm=audio 6062 RTP/AVP %d\n\na=rtpmap:%d %s/%d/%s\n\na=fmtp:%d mode=%s;config=%s;SizeLength=%s;IndexLength=%s;IndexDeltaLength=%s;\n\n", pSDPData, mux->nAudioPayload, mux->nAudioPayload, g_ascii_strdown(mux->pAudioEncodingName, -1), mux->nAudioClockRate, mux->pAudioEncodingParams, mux->nAudioPayload, mux->pAudioMode, mux->pAudioConfig, mux->pAudioSizeLen, mux->pAudioIndexLen, mux->pAudioDeltaLen); 
    }

    GstCaps *caps = gst_caps_new_simple("application/ipvs-sframe", "sdp", G_TYPE_STRING, pSDPData, NULL);

    gst_pad_push_event(mux->pSrcPad, gst_event_new_stream_start("ipvsmux_0"));

    if (!gst_pad_set_caps (mux->pSrcPad, caps))
    {
        GST_DEBUG_OBJECT(mux, "\n\n SETTING CAPS FAILED ON SRCPAD \n\n");
    }

    gst_caps_unref (caps);
}

static GstStateChangeReturn
ipvsrtpmuxer_change_state (GstElement * element, GstStateChange transition)
{
    IpvsRtpMuxer *mux = GST_IPVS_RTP_MUXER (element);
    GstStateChangeReturn ret;

    switch (transition)
    {
        case GST_STATE_CHANGE_NULL_TO_READY:
            break;

        case GST_STATE_CHANGE_READY_TO_PAUSED:
            gst_collect_pads_start (mux->collect);
            break;

        case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
            break;

        case GST_STATE_CHANGE_PAUSED_TO_READY:
            gst_collect_pads_stop (mux->collect);
            break;

        case GST_STATE_CHANGE_READY_TO_NULL:
            break;

        default:
            break;
    }

    ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

    switch (transition)
    {
        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            break;

        case GST_STATE_CHANGE_PAUSED_TO_READY:
            ipvsrtpmuxer_reset (mux);
            break;

        case GST_STATE_CHANGE_READY_TO_NULL:
            break;

        default:
            break;
    }

    return ret;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
    if (!gst_element_register (plugin, "ipvsrtpmuxer", GST_RANK_PRIMARY,
                               ipvsrtpmuxer_get_type ()))
        return FALSE;

    GST_DEBUG_CATEGORY_INIT (ipvsrtpmuxer_debug, "ipvsrtpmuxer", 0,
                             "IPVS RTP muxer");

    return TRUE;
}

GST_PLUGIN_DEFINE (1, 0,
    ipvsrtpmuxer, "IPVS RTP Muxer",
    plugin_init, "1.0", "GPL", PACKAGE, "www.barco.com");
