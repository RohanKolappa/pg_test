/*
 * FFmpegTest.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: rkale
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "BasicUsageEnvironment.hh"

#include "CommonIncludes.hh"
#include "MediaServer.hh"

#include "H264VideoRTPParser.hh"
#include "MPEG4AudioRTPParser.hh"
#include "MPEG4VideoRTPParser.hh"
#include "aligned_malloc.h"

using namespace mediaindex;

static int packetCount = 0;
static int videoPacketCount = 0;
static int audioPacketCount = 0;

void dumpBytes(unsigned char *buf, int bytes) {
    int offset = 0;
    while (bytes > 0) {
        printf("0x%02x ", *(buf + offset));
        bytes--;
        offset++;
        if (offset % 8 == 0 || bytes == 0)
            printf("\n");
    }
}


void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;


    f = fopen(filename, "wb");
    if (f == NULL)
        return;


    fprintf(f,"P5\n%d %d\n%d\n", xsize, ysize, 255);
    for(i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

void ppm_save(unsigned char *buf, int stride, int width, int height, char *filename)
{
    FILE *f;
    int y;

    f = fopen(filename, "wb");
    if (f == NULL)
        return;

    // Write header
    fprintf(f, "P6\n%d %d\n%d\n", width, height, 255);

    // Write pixel data
    for(y=0; y < height; y++)
        fwrite(buf + y * stride, 1, width * 3, f);

    // Close file
    fclose(f);
}

void raw_append_save(unsigned char *buf, int len, char *filename)
{
    FILE *f = NULL;

    // open file in append mode
    f = fopen(filename, "ab");
    if (f == NULL)
        return;

    // Write buffer to file
    fwrite(buf, len, 1, f);

    // Close file
    fclose(f);

}


static AVFrame *picture = NULL;
static AVFrame *dstPicture = NULL;
static struct SwsContext *pSwsCtx = NULL;

void decode_video_frame(AVCodecContext *pCodecCtx,
        uint8_t *frame_buffer, int frame_size)
{
    AVPacket avFramePkt;
    static int frameCount = 0;
    static int pictureCount = 0;
    static int lastVideoPacketCount = 0;

    int got_picture;

    if (picture == NULL)
        picture = avcodec_alloc_frame();
    if (dstPicture == NULL)
        dstPicture = avcodec_alloc_frame();

    av_init_packet(&avFramePkt);
    avFramePkt.data = frame_buffer;
    avFramePkt.size = frame_size;

    frameCount++;
    while (avFramePkt.size > 0) {

        printf("Decoding frame %d of size %d\n", frameCount, avFramePkt.size);
        int len = avcodec_decode_video2(pCodecCtx, picture, &got_picture, &avFramePkt);
        if (len < 0) {
            printf("Error while decoding frame\n");
            break;
            //exit(1);
        }
        if (got_picture) {
            pictureCount++;
            printf("*** Got Picture %d using len %d at frame %d using %d packets ***\n",
                    pictureCount, len, frameCount, videoPacketCount - lastVideoPacketCount);
            lastVideoPacketCount = videoPacketCount;
            if ((pictureCount < 10 || pictureCount % 10 == 0) &&
                    pCodecCtx->width > 0 && pCodecCtx->height > 0) {
                char buf[64];
                snprintf(buf, sizeof(buf), "picture_%04d.pgm", pictureCount);
//                pgm_save(picture->data[0], picture->linesize[0],
//                        pCodecCtx->width, pCodecCtx->height, buf);
                // Convert to RGB format
                if (pSwsCtx == NULL) {
                    int width = pCodecCtx->width;
                   int height = pCodecCtx->height;
                    pSwsCtx = sws_getContext(
                            width, height, pCodecCtx->pix_fmt,
                            width, height, PIX_FMT_RGB24,
                            SWS_BICUBIC, NULL, NULL, NULL);

                    // Determine required buffer size and allocate buffer
                    int numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                        pCodecCtx->height);

                    uint8_t * buffer = (uint8_t *) malloc(numBytes);

                    // Assign appropriate parts of buffer to image planes in pFrameRGB
                    avpicture_fill((AVPicture *)dstPicture, buffer, PIX_FMT_RGB24,
                            pCodecCtx->width, pCodecCtx->height);
                }

                sws_scale(pSwsCtx, (const uint8_t * const*)picture->data,
                        picture->linesize, 0, pCodecCtx->height, dstPicture->data,
                        dstPicture->linesize);
                snprintf(buf, sizeof(buf), "picture_%04d.ppm", pictureCount);
//                ppm_save(dstPicture->data[0], dstPicture->linesize[0],
//                        pCodecCtx->width, pCodecCtx->height, buf);
            }
            //exit(0);
        }
        avFramePkt.size -= len;
        avFramePkt.data += len;
        if (avFramePkt.size > 0)
            printf("More processing for current packet %d, processed %d, remaining %d\n",
                    frameCount, len, avFramePkt.size);
    }
}

void decode_audio_frame(AVCodecContext *pCodecCtx,
        uint8_t *frame_buffer, int frame_size)
{
    AVPacket avFramePkt;
    static int frameCount = 0;
    static int sampleCount = 0;

    av_init_packet(&avFramePkt);
    avFramePkt.data = frame_buffer;
    avFramePkt.size = frame_size;

    frameCount++;
    uint8_t *outbuf = (uint8_t *) aligned_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE, DEFAULT_ALIGNMENT);


    int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    while (avFramePkt.size > 0) {
        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;


        printf("Decoding audio frame %d of size %d\n", frameCount, avFramePkt.size);
        //int len = avcodec_decode_video2(pCodecCtx, picture, &got_picture, &avFramePkt);
        int len = avcodec_decode_audio3(pCodecCtx, (short *)outbuf, &out_size, &avFramePkt);

        if (len < 0) {
            printf("Error while decoding audio frame\n");
            //exit(1);
            break;
        }
        if (out_size) {
            sampleCount++;
            printf("**** Got Sample %d of size %d using len %d at frame %d ****\n",
                    sampleCount, out_size, len, frameCount);
            //raw_append_save(outbuf, out_size, "audio.pcm");
        }
        avFramePkt.size -= len;
        avFramePkt.data += len;
        if (avFramePkt.size > 0)
            printf("More processing for current packet %d\n", frameCount);
    }
    aligned_free(outbuf);
}

//#define PARSE_PRINTF printf
#define PARSE_PRINTF(...) do {} while (0)

int parse_packet(AVCodecContext *pCodecCtx, AVCodecParserContext *pParserContext,
        uint8_t * packet_buffer, int packet_size)
{
    uint8_t *in_data = packet_buffer;
    int in_len = packet_size;
    uint8_t *out_data;
    int out_size;
    uint64_t pts = 0, dts = 0, pos = 0;

    PARSE_PRINTF("Begin Parse Packet with length %d\n", in_len);
    while (in_len) {
        PARSE_PRINTF("Invoking parser with length %d\n", in_len);
        int len = av_parser_parse2(pParserContext, pCodecCtx, &out_data, &out_size,
                in_data, in_len,
                pts, dts, pos);
        if (len < 0) {
            PARSE_PRINTF("Parser returned error %d!!\n", len);
        }
        if (len == 0) {
            PARSE_PRINTF("Parser did not consume input!!\n");
        }
        in_data += len;
        in_len  -= len;
        if (in_len > 0) {
            PARSE_PRINTF("Partially consumed %d\n", in_len);
        }

        if (out_size > 0) {
           PARSE_PRINTF("Parsed a frame of size %d, so far in=%d\n",
                   out_size, totalParsedInputBytes);

           if (pCodecCtx->codec_type == AVMEDIA_TYPE_VIDEO)
               decode_video_frame(pCodecCtx, out_data, out_size);
           else {
               decode_audio_frame(pCodecCtx, out_data, out_size);
           }
        }
    }
    return 0;
}

int main(int argc, const char * argv[])
{

    if (argc < 2) {
        printf("Usage: %s <MediaDirectory>\n", argv[0]);
        exit(1);
    }

    char mediaDir[256];
    strcpy(mediaDir, argv[1]);

    /* must be called before using avcodec lib */
    avcodec_init();

    // Register all formats and codecs
    av_register_all();

    av_log_set_level(99);


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
    filebuffer[filesize] = 0;
    fclose(fp);

    if (filesize <= 0) {
        printf("Could not read input file %s\n", sdp_file_name);
        return -1;
    }


    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);

    MediaSubsession *videoSubsession = NULL;
    MediaSubsession *audioSubsession = NULL;

    MediaSession *session = MediaSession::createNew(*env, filebuffer);

    MediaSubsessionIterator iter(*session);
    MediaSubsession *subsession;
    while ((subsession = iter.next()) != NULL) {
        printf("Payload Fomat: %d\n", subsession->rtpPayloadFormat());
        printf("Medium Name: %s\n", subsession->mediumName());
        printf("Codec Name: %s\n", subsession->codecName());
        printf("Protocol Name: %s\n", subsession->protocolName());
        printf("Control Path: %s\n", subsession->controlPath());
        //printf("Saved Lines: %s\n", subsession->savedSDPLines());
        //printf("PPS: %s\n", subsession->fmtp_spropparametersets());
        if (strcmp(subsession->mediumName(), "video") == 0) {
            if (videoSubsession == NULL)
                videoSubsession = subsession;
        }
        else if (strcmp(subsession->mediumName(), "audio") == 0) {
            if (audioSubsession == NULL)
                audioSubsession = subsession;
        }
    }


    AVCodecContext *pVideoCodecCtx = avcodec_alloc_context();

    AVCodecContext *pAudioCodecCtx = avcodec_alloc_context();


    pVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pVideoCodecCtx->codec_id = CODEC_ID_NONE;

    pAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;

    bool bVideoNeedsParsing = false;
    bool bAudioNeedsParsing = false;

    CRTPParser * pVideoRtpParser = NULL;
    if (strcasecmp(videoSubsession->codecName(), "H264") == 0) {
        pVideoCodecCtx->codec_id = CODEC_ID_H264;
        pVideoRtpParser = new CH264VideoRTPParser(videoSubsession->rtpPayloadFormat());
        bVideoNeedsParsing = true;
    }
    else if (strcasecmp(videoSubsession->codecName(), "MP4V-ES") == 0) {
        pVideoCodecCtx->codec_id = CODEC_ID_MPEG4;
        pVideoRtpParser = new CMPEG4VideoRTPParser(videoSubsession->rtpPayloadFormat());
    }

    CRTPParser * pAudioRtpParser = NULL;
    if (strcasecmp(audioSubsession->codecName(), "MPEG4-GENERIC") == 0) {
        pAudioCodecCtx->codec_id = CODEC_ID_AAC;
        pAudioRtpParser = new CMPEG4AudioRTPParser(audioSubsession->rtpPayloadFormat());
    }

    if (pVideoRtpParser != NULL)
        pVideoRtpParser->RetrieveCodecConfig(videoSubsession, pVideoCodecCtx);
    if (pAudioRtpParser != NULL)
        pAudioRtpParser->RetrieveCodecConfig(audioSubsession, pAudioCodecCtx);

    // Find the decoder for the video stream

    AVCodec *pVideoCodec = NULL;
    pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
    if (pVideoCodec == NULL) {
        printf("Video Codec not found.\n");
        return -1;
    }

    // Open video codec
    if (avcodec_open(pVideoCodecCtx, pVideoCodec) < 0) {
        printf("Could not open video codec.\n");
        return -1;
    }

    // Find the decoder for the audio stream

    AVCodec *pAudioCodec = NULL;
    pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
    if (pAudioCodec == NULL) {
        printf("Audio Codec not found.\n");
    }
    else {
        // Open audio codec
        if (avcodec_open(pAudioCodecCtx, pAudioCodec) < 0) {
            printf("Could not open audio codec.\n");
            return -1;
        }
    }


    printf("Video Dimension = %d x %d\n", pVideoCodecCtx->width, pVideoCodecCtx->height);
    printf("Audio fmt = %d frame size = %d sample rate = %d channels = %d\n",
            pAudioCodecCtx->sample_fmt,
            pAudioCodecCtx->frame_size,
            pAudioCodecCtx->sample_rate,
            pAudioCodecCtx->channels);

    int width = pVideoCodecCtx->width;
    int height = pVideoCodecCtx->height;

    if (pVideoCodec->capabilities & CODEC_CAP_TRUNCATED) {
        pVideoCodecCtx->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    }


    // Get video codec parser
    AVCodecParserContext *pVideoParserContext = av_parser_init(pVideoCodecCtx->codec_id);

    if (pVideoParserContext == NULL) {
        printf("Video Parser Context not found\n");
        exit(0);
    }

    // Get audio codec parser
    AVCodecParserContext *pAudioParserContext = av_parser_init(pAudioCodecCtx->codec_id);

    if (pAudioParserContext == NULL) {
        printf("Audio Parser Context not found\n");
        exit(0);
    }

    // Open minute file for RTP media
    CMediaServer *ms = CMediaServer::Create(mediaDir);
    if (ms->Init() != MSRC_OK) {
        printf("MediaServer Init Failed: %s\n", ms->GetErrorString());
        exit(1);
    }


    if (ms->Start(0) != MSRC_OK) {
        printf("MediaServer Start Failed: %s\n",
                ms->GetErrorString());
        exit(1);
    }

    int rc;
    CCFrame cframe;
    int errorPacketCount = 0;

    AVPacket avpkt;

    bool done = false;
    while (!done) {
        // Get a frame from media server
        if ((rc = ms->GetFrame(cframe)) != MSRC_OK) {
            if (rc != MSRC_END_OF_MEDIA) {
                printf("MediaServer Get Frame Failed: %s\n",
                        ms->GetErrorString());
            }
            break;
        }

        if (cframe.GetTimeOffset() > 3000) {
            done = true;
        }

        if (cframe.GetMessageType() == CCFrame::Normal) {
            packetCount++;

            unsigned long nStreamType = cframe.GetStreamType();

            if (nStreamType == STREAMTYPE_RTP) {
                CRTPStreamMediaFlag rtpSMediaFlag(cframe.GetMediaFlag());

                switch(rtpSMediaFlag.GetEncaps()) {
                case FRAMETYPE_RTP_ENCAPS_RTP:
                    switch (rtpSMediaFlag.GetChannel()) {
                    case FRAMETYPE_RTP_CHAN_VIDEO:

                        videoPacketCount++;

                        if (pVideoRtpParser != NULL &&
                                pVideoRtpParser->GetPacket(cframe.GetData(), cframe.GetLength(), &avpkt)) {
//                            if (pVideoRtpParser->IsMarked())
//                                printf("Packet %d is marked\n", videoPacketCount);
                            if (bVideoNeedsParsing)
                                parse_packet(pVideoCodecCtx, pVideoParserContext, avpkt.data, avpkt.size);
                            else
                                decode_video_frame(pVideoCodecCtx, avpkt.data, avpkt.size);
                            av_free_packet(&avpkt);
                        }
                        break;

                    case FRAMETYPE_RTP_CHAN_AUDIO:
                        audioPacketCount++;
                        if (pAudioRtpParser != NULL &&
                                pAudioRtpParser->GetPacket(cframe.GetData(), cframe.GetLength(), &avpkt)) {
                            if (bAudioNeedsParsing)
                                parse_packet(pAudioCodecCtx, pAudioParserContext, avpkt.data, avpkt.size);
                            else
                                decode_audio_frame(pAudioCodecCtx, avpkt.data, avpkt.size);
                            av_free_packet(&avpkt);
                        }

                        break;

                    default:
                        errorPacketCount++;
                        break;
                    }
                    break;
                default:
                    errorPacketCount++;
                    break;
                }
            }

        }
        else {
            if (cframe.GetMessageType() == CCFrame::MediaStopped) {
                printf("MediaReader Stopped at %llu\n", cframe.GetTimeStamp());
            }
            else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
                printf("MediaReader Started at %llu\n", cframe.GetTimeStamp());
            }
            else {
                printf("Unexpected frame at %llu\n", cframe.GetTimeStamp());
                exit(1);
            }
        }

        if (width != pVideoCodecCtx->width || height != pVideoCodecCtx->height) {
            printf("Resolution changed at %dx%d -> %dx%d\n",
                    width, height,
                    pVideoCodecCtx->width, pVideoCodecCtx->height);
            width = pVideoCodecCtx->width;
            height = pVideoCodecCtx->height;
        }
    }

    printf("Audio fmt = %d frame size = %d sample rate = %d channels = %d\n",
            pAudioCodecCtx->sample_fmt,
            pAudioCodecCtx->frame_size,
            pAudioCodecCtx->sample_rate,
            pAudioCodecCtx->channels);

    printf("Processed %d packets. Video = %d, Audio = %d, Error = %d\n",
            packetCount, videoPacketCount, audioPacketCount, errorPacketCount);

    // Cleanup
    av_parser_close(pVideoParserContext);
    avcodec_close(pVideoCodecCtx);
    av_free(pVideoCodecCtx->extradata);
    av_free(pVideoCodecCtx);
//    if (pAudioCodecCtx != NULL)
//        avcodec_close(pAudioCodecCtx);


//    av_close_input_file(pFormatCtx);


    if (picture != NULL)
        av_free(picture);
    if (dstPicture != NULL) {
        free(dstPicture->data[0]);
        av_free(dstPicture);
    }
    if (pSwsCtx != NULL)
        sws_freeContext(pSwsCtx);
    delete ms;

    Medium::close(session);
    env->reclaim();
    delete scheduler;
    delete pVideoRtpParser;
    delete pAudioRtpParser;
}

