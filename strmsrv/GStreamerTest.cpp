/*
 * GStreamerTest.cpp
 *
 *  Created on: Apr 14, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gst/gst.h>
#include <gst/sdp/gstsdpmessage.h>


int main(int argc, char * argv[])
{

    // Init Gstreamer
    gst_init (&argc, &argv);

    if (argc < 2) {
        printf("Usage: %s <MediaDirectory>\n", argv[0]);
        exit(1);
    }

    char mediaDir[256];
    strcpy(mediaDir, argv[1]);

    char sdp_file_name[64];
    sprintf(sdp_file_name, "%s/input.sdp", mediaDir);

    // Read the whole file into a buffer
    FILE *fp = fopen(sdp_file_name, "r");
    if (fp == NULL) {
        printf("Could not open input file %s\n", sdp_file_name);
        return -1;
    }
    char filebuffer[16384];
    int filesize = fread(filebuffer, 1, 16384, fp);
    fclose(fp);

    if (filesize <= 0) {
        printf("Could not read input file %s\n", sdp_file_name);
        return -1;
    }
    printf("Read file %s of size %d\n", sdp_file_name, filesize);

    GstSDPMessage * sdpMessage;
    gst_sdp_message_new(&sdpMessage);

    if (gst_sdp_message_parse_buffer((const guint8 *) filebuffer, filesize, sdpMessage) != GST_SDP_OK) {
        printf("Failed to parse SDP file %s\n", sdp_file_name);
        return -1;
    }

    printf("Session Name: %s\n", gst_sdp_message_get_session_name(sdpMessage));

    int numberOfStreams = gst_sdp_message_medias_len(sdpMessage);

    for (int streamIndex = 0; streamIndex < numberOfStreams; streamIndex++) {
        const GstSDPMedia * pMedia = gst_sdp_message_get_media(sdpMessage, streamIndex);
        printf("%d Media: %s\n", streamIndex,
                 gst_sdp_media_get_media(pMedia));
        printf("%d Protocol : %s\n", streamIndex,
                 gst_sdp_media_get_proto(pMedia));
        printf("%d Format Len : %d\n", streamIndex,
                 gst_sdp_media_formats_len(pMedia));
        printf("%d Format : %s\n", streamIndex,
                 gst_sdp_media_get_format(pMedia, 0));

        int numbAttributes = gst_sdp_media_attributes_len(pMedia);
        for (int attrIdx = 0; attrIdx < numbAttributes; attrIdx++) {
            const GstSDPAttribute * attribute =
                    gst_sdp_media_get_attribute(pMedia, attrIdx);
            printf("Attribute %s => %s\n", attribute->key, attribute->value);
        }
    }

    return 0;
}
