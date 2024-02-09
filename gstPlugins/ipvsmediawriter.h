#ifndef __IPVS_MEDIA_WRITER_H__
#define __IPVS_MEDIA_WRITER_H__

#include <stdio.h>

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

#define GST_TYPE_IPVS_MEDIA_WRITER \
  (ipvs_media_writer_get_type())

#define GST_IPVS_MEDIA_WRITER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPVS_MEDIA_WRITER,IpvsMediaWriter))

#define GST_IPVS_MEDIA_WRITER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPVS_MEDIA_WRITER,IpvsMediaWriterClass))

#define GST_IS_IPVS_MEDIA_WRITER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPVS_MEDIA_WRITER))

#define GST_IS_IPVS_MEDIA_WRITER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPVS_MEDIA_WRITER))

typedef struct _IpvsMediaWriter IpvsMediaWriter;
typedef struct _IpvsMediaWriterClass IpvsMediaWriterClass;

struct _IpvsMediaWriter {
    GstBaseSink parent;

    gboolean bUseFixedTS;
    gchar *pBlobDir;
    gchar *pSDPFilePath;
    gchar *pTmpSDPFile;
    gchar *pTmpSDPFilePath;
    gboolean bHLS;
    gboolean bTN;
    GstCaps *pVideoCaps;
    GstCaps* pAudioCaps;

    const gchar *pSDPData;

    struct mediawriterWrapper_t *pWriter;
    guint64 nStartTimeStamp;
    guint64 nStopTimeStamp;

    gboolean bCreatedSDPFile;
    gboolean bCreatedHLSGenerator;
    gboolean bCreatedTNGenerator;
};

struct _IpvsMediaWriterClass {
    GstBaseSinkClass parent_class;
};

GType ipvs_media_writer_get_type (void);

G_END_DECLS

#endif /* __IPVS_MEDIA_WRITER_H__ */
