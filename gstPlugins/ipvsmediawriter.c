/**
 * SECTION:element-ipvsmediawriter
 *
 * Write incoming data to a file in the local file system.
 */

/* PIPE :
   gst-launch-1.0 filesrc location=/home/durga/videos/boeing.mp4 ! qtdemux name=demux demux.video_0 ! queue ! rtph264pay pt=96 ! ipvsrtpmuxer name=mux ! ipvsmediawriter location=/home/durga/tmp sdp-file-path=/home/durga/public_html/sdp.sdp create-hls=true create-primary-thumbnail=true demux. ! queue ! rtpmp4gpay pt=97 ! mux.
 */

#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "ipvsmediawriter.h"
#include "mediawriter_cwrapper.h"

#ifndef PACKAGE
#define PACKAGE "barco"
#endif

#include "ipvsmediawriter.h"
#include "mediawriter_cwrapper.h"

enum
{
    PROP_0,
    PROP_USE_FIXED_TS,
    PROP_LOCATION,
    PROP_SDP_FILE_PATH,
    PROP_ENABLE_HLS,
    PROP_ENABLE_TN
};

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/ipvs-sframe"));

static void ipvs_media_writer_dispose (GObject * object);

static gboolean ipvs_media_writer_set_location (IpvsMediaWriter * sink, const gchar * location, GError ** error);
static void ipvs_media_writer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void ipvs_media_writer_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean ipvs_media_writer_start (GstBaseSink * sink);
static gboolean ipvs_media_writer_stop (GstBaseSink * sink);
static gboolean ipvs_media_writer_event (GstBaseSink * sink, GstEvent * event);
static GstFlowReturn ipvs_media_writer_render (GstBaseSink * sink,
    GstBuffer * buffer);

void ipvs_media_writer_parse_caps(IpvsMediaWriter *pMediaSink, GstCaps *pCaps);

static void CreateHLSGenerator(IpvsMediaWriter *pMediaWriter);
static void CreateTNGenerator (IpvsMediaWriter *pMediaWriter);

static int CreateSDPFile(IpvsMediaWriter *pMediaWriter);
static int CreateTmpSDPFile(IpvsMediaWriter *pMediaWriter);

G_DEFINE_TYPE (IpvsMediaWriter, ipvs_media_writer, GST_TYPE_BASE_SINK)

#define parent_class ipvs_media_writer_parent_class

static void
ipvs_media_writer_class_init (IpvsMediaWriterClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);
    GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS (klass);

    gobject_class->dispose = ipvs_media_writer_dispose;

    gobject_class->set_property = ipvs_media_writer_set_property;
    gobject_class->get_property = ipvs_media_writer_get_property;

    g_object_class_install_property (gobject_class, PROP_USE_FIXED_TS,
             g_param_spec_boolean ("use-fixed-start-time", 
                                    "Use Fixed Start Timestamp",
                                    "Use fixed start timestamp", FALSE,
                                    G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_LOCATION,
             g_param_spec_string ("location", "File Location",
             "Location of the Minute files to write", NULL,
             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_SDP_FILE_PATH,
             g_param_spec_string ("sdp-file-path", "SDP File Path",
             "Complete path to SDP File", NULL,
             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_ENABLE_HLS,
             g_param_spec_boolean ("create-hls", "Enable/Disable HLS",
             "Enable or Disable HLS creation", FALSE,
             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_ENABLE_TN,
             g_param_spec_boolean ("create-primary-thumbnail", "Primary Thumbnail",
             "Enable or Disable of Primary thumbnail creation", FALSE,
             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    gst_element_class_set_static_metadata (gstelement_class,
             "IPVS Media Writer",
             "IPVS Media Writer Sink", "Creates Minute files",
             "Durga <durga.davuluri@barco.com>");

    gst_element_class_add_pad_template (gstelement_class,
              gst_static_pad_template_get (&sinktemplate));

    gstbasesink_class->start = GST_DEBUG_FUNCPTR (ipvs_media_writer_start);
    gstbasesink_class->stop = GST_DEBUG_FUNCPTR (ipvs_media_writer_stop);
    gstbasesink_class->render = GST_DEBUG_FUNCPTR (ipvs_media_writer_render);
    gstbasesink_class->event = GST_DEBUG_FUNCPTR (ipvs_media_writer_event);
}

static void
ipvs_media_writer_init (IpvsMediaWriter * pMediaWriter)
{
    pMediaWriter->bUseFixedTS = FALSE;
    pMediaWriter->pBlobDir = NULL;
    pMediaWriter->pSDPFilePath = NULL;
    pMediaWriter->pTmpSDPFilePath = NULL;
    pMediaWriter->bHLS = FALSE;
    pMediaWriter->bTN = FALSE;

    pMediaWriter->pTmpSDPFile = "__temp_sdp_.sdp";
    pMediaWriter->pVideoCaps = NULL;
    pMediaWriter->pAudioCaps = NULL;

    pMediaWriter->bCreatedSDPFile = FALSE;
    pMediaWriter->bCreatedHLSGenerator = FALSE;
    pMediaWriter->bCreatedTNGenerator = FALSE;

    pMediaWriter->pWriter = NULL;

    gst_base_sink_set_sync (GST_BASE_SINK (pMediaWriter), FALSE);
}

static void
ipvs_media_writer_dispose (GObject * object)
{
    IpvsMediaWriter *sink = GST_IPVS_MEDIA_WRITER (object);

    G_OBJECT_CLASS (parent_class)->dispose (object);

    g_free (sink->pBlobDir);
    sink->pBlobDir = NULL;

    g_free (sink->pSDPFilePath);
    sink->pSDPFilePath = NULL;

    g_free (sink->pTmpSDPFilePath);
    sink->pTmpSDPFilePath = NULL;

    sink->pTmpSDPFile = NULL;

    sink->bHLS = FALSE;
    sink->bTN = FALSE;
    sink->bUseFixedTS = FALSE;

    if (sink->pVideoCaps)
        gst_caps_unref(sink->pVideoCaps);

    if (sink->pAudioCaps)
        gst_caps_unref(sink->pAudioCaps);
}

static gboolean
ipvs_media_writer_set_location (IpvsMediaWriter * sink, const gchar * location,
    GError ** error)
{
    g_free (sink->pBlobDir);
    if (location != NULL)
    {
        sink->pBlobDir = g_strdup (location);
        GST_INFO ("BlobDir : %s", sink->pBlobDir);
    }
    else
    {
        sink->pBlobDir = NULL;
    }

    return TRUE;
}

static void
ipvs_media_writer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
    IpvsMediaWriter *sink = GST_IPVS_MEDIA_WRITER (object);

    switch (prop_id) 
    {
        case PROP_USE_FIXED_TS:
            sink->bUseFixedTS = TRUE;
            sink->nStartTimeStamp = 10000;
            break;

        case PROP_LOCATION:
            ipvs_media_writer_set_location (sink, g_value_get_string (value), NULL);
            break;

        case PROP_SDP_FILE_PATH:
            g_free(sink->pSDPFilePath);
            sink->pSDPFilePath = g_strdup(g_value_get_string(value));
            break;

        case PROP_ENABLE_HLS:
            sink->bHLS = g_value_get_boolean(value);
            break;

        case PROP_ENABLE_TN:
            sink->bTN = g_value_get_boolean(value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
ipvs_media_writer_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
    IpvsMediaWriter *sink = GST_IPVS_MEDIA_WRITER (object);

    switch (prop_id)
    {
        case PROP_LOCATION:
            g_value_set_string (value, sink->pBlobDir);
            break;

        case PROP_SDP_FILE_PATH:
            g_value_set_string (value, sink->pSDPFilePath);
            break;

        case PROP_ENABLE_HLS:
            g_value_set_boolean (value, sink->bHLS);
            break;

        case PROP_ENABLE_TN:
            g_value_set_boolean (value, sink->bTN);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static gboolean
ipvs_media_writer_event (GstBaseSink * sink, GstEvent * event)
{
    GstEventType type;
    IpvsMediaWriter *pMediaSink;

    pMediaSink = GST_IPVS_MEDIA_WRITER (sink);

    type = GST_EVENT_TYPE (event);

    switch (type)
    {
        case GST_EVENT_CAPS:
        {
            GstCaps *pCaps;
            gst_event_parse_caps(event, &pCaps);
            ipvs_media_writer_parse_caps(pMediaSink, pCaps); 
            break;
        }

        case GST_EVENT_FLUSH_STOP:
        {
            break;
        }

        case GST_EVENT_EOS:
        {
            break;
        }
        default:
            return GST_BASE_SINK_CLASS (parent_class)->event (sink, event);
    }

    return GST_BASE_SINK_CLASS (parent_class)->event (sink, event);
}

static int 
CreateSDPFile (IpvsMediaWriter *pMediaWriter)
{
    if (pMediaWriter == NULL)
        return 0;

    FILE *pSDPFile = fopen(pMediaWriter->pSDPFilePath, "w");
    if (pSDPFile == NULL)
    {
        g_print("\n\n Failed in creating SDP File.\n\n");
        return 0;
    }

    fprintf (pSDPFile, "%s", pMediaWriter->pSDPData);

    fclose(pSDPFile);

    return 1;
}

//Creating tmp sdp file for HLS and thumbnails.
static int
CreateTmpSDPFile (IpvsMediaWriter *pMediaWriter)
{
    if (pMediaWriter == NULL)  
        return 0;

    //If tmp-sdp file is already been created, then do nothing.
    if (pMediaWriter->pTmpSDPFilePath != NULL)
        return 1;

    gchar pTmpSDPFilePath[255];
    sprintf (pTmpSDPFilePath, "%s/%s", pMediaWriter->pBlobDir, pMediaWriter->pTmpSDPFile);
    pMediaWriter->pTmpSDPFilePath = g_strdup (pTmpSDPFilePath);

    FILE *pSDPFile = fopen(pMediaWriter->pTmpSDPFilePath, "w");
    if (pSDPFile == NULL)
    {
        GST_DEBUG_OBJECT(pMediaWriter, "Failed in creating SDP File.\n");
        return 0;
    }

    fprintf(pSDPFile, "%s", pMediaWriter->pSDPData);

    fclose(pSDPFile);

    return 1;
}

static void 
CreateHLSGenerator (IpvsMediaWriter *pMediaWriter)
{
    if (pMediaWriter->bHLS)
    {
        CreateTmpSDPFile(pMediaWriter);

        startMediaWriterForHLS(pMediaWriter->pWriter, pMediaWriter->nStartTimeStamp, pMediaWriter->pBlobDir, pMediaWriter->pTmpSDPFilePath);

        pMediaWriter->bCreatedHLSGenerator = TRUE;
    }
}

static void
CreateTNGenerator (IpvsMediaWriter *pMediaWriter)
{
    if (pMediaWriter == NULL)
        return;

    if (pMediaWriter->bTN)
    {
        CreateTmpSDPFile(pMediaWriter);

        startMediaWriterForTN(pMediaWriter->pWriter, pMediaWriter->nStartTimeStamp, pMediaWriter->pBlobDir, pMediaWriter->pTmpSDPFilePath);

        pMediaWriter->bCreatedTNGenerator = TRUE;
    }
}

static GstFlowReturn
ipvs_media_writer_render (GstBaseSink * sink, GstBuffer * buffer)
{
    IpvsMediaWriter *pMediaSink;
    GstMapInfo info;
    struct IPVSHeaderInfo headerInfo;
    int nHeaderSize = 0;
    int nTotalBuffSize = 0;
    guint64 nWallClock = 0;
    guint32 nFlags = 0;

    pMediaSink = GST_IPVS_MEDIA_WRITER (sink);

    if (pMediaSink->bCreatedHLSGenerator == 0)
    {
        CreateHLSGenerator(pMediaSink);
    }

    if (pMediaSink->bCreatedTNGenerator == 0)
    {
        CreateTNGenerator(pMediaSink);
    }

    gst_buffer_map (buffer, &info, GST_MAP_READ);

    if (info.size > 0 && info.data != NULL) {
        nHeaderSize = sizeof(headerInfo);
        nTotalBuffSize = gst_buffer_get_size(buffer);

        gst_buffer_extract(buffer, 0, (gpointer*)&headerInfo, nHeaderSize);

        nWallClock = ntohll (headerInfo.nWallClock);
        nFlags = ntohl (headerInfo.nFlags);

        putMediaFrame(pMediaSink->pWriter, info.data + nHeaderSize , info.size - nHeaderSize, nWallClock, nFlags);

        putMediaFrameForHLS(pMediaSink->pWriter, info.data + nHeaderSize , info.size - nHeaderSize, nWallClock, nFlags);

        putMediaFrameForTN(pMediaSink->pWriter, info.data + nHeaderSize, info.size - nHeaderSize, nWallClock, nFlags);

        pMediaSink->nStopTimeStamp = nWallClock;
    }

    gst_buffer_unmap (buffer, &info);

    return GST_FLOW_OK;
}

void ipvs_media_writer_parse_caps (IpvsMediaWriter *pMediaSink, GstCaps *pCaps)
{
    if (pCaps == NULL)
        return;

    GstStructure *pStruct = gst_caps_get_structure(pCaps, 0);
    const GValue *pVal = gst_structure_get_value(pStruct, "sdp");
    if (pVal == NULL)
        return;

    pMediaSink->pSDPData = g_value_get_string (pVal);

    if (pMediaSink->bCreatedSDPFile == 0)
    {
        pMediaSink->bCreatedSDPFile = CreateSDPFile(pMediaSink);
    }
}

static gboolean
ipvs_media_writer_start (GstBaseSink * basesink)
{
    IpvsMediaWriter *pMediaWriter;

    pMediaWriter = GST_IPVS_MEDIA_WRITER (basesink);

    //For test cases we can use fixed start of time
    if (pMediaWriter->bUseFixedTS == FALSE) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        pMediaWriter->nStartTimeStamp = (((tv.tv_sec * 1000000LL) + (tv.tv_usec)) / 1000);
    }

    pMediaWriter->pWriter = createMediaWriter(pMediaWriter->pBlobDir);
    startMediaWriter(pMediaWriter->pWriter, pMediaWriter->nStartTimeStamp, 
           pMediaWriter->bHLS == TRUE ? 1 : 0, 
           pMediaWriter->bTN == TRUE ? 1 : 0);

    return TRUE;
}

static gboolean
ipvs_media_writer_stop (GstBaseSink * basesink)
{
    IpvsMediaWriter *pMediaWriter;

    pMediaWriter = GST_IPVS_MEDIA_WRITER (basesink);
   
    stopMediaWriter(pMediaWriter->pWriter, pMediaWriter->nStopTimeStamp);
    stopMediaWriterForHLS(pMediaWriter->pWriter, pMediaWriter->nStopTimeStamp);
    stopMediaWriterForTN(pMediaWriter->pWriter, pMediaWriter->nStopTimeStamp);

    destroyMediaWriter(pMediaWriter->pWriter);

    return TRUE;
}

static gboolean
plugin_init (GstPlugin *plugin)
{
    if (!gst_element_register (plugin, "ipvsmediawriter", GST_RANK_PRIMARY,
                ipvs_media_writer_get_type()))
        return FALSE;

    return TRUE;
}

GST_PLUGIN_DEFINE (1, 0, ipvsmediawriter, "IPVS Media Writer Sink Element", plugin_init, "1.0", "GPL", PACKAGE , "www.barco.com");

