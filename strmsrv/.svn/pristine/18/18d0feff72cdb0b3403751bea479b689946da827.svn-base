/*
 * FFmpegUtils.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/opt.h"
}

#include "FFmpegUtils.hh"

CMutex FFmpegUtils::m_mutex;
bool FFmpegUtils::m_bInitialized = false;

void FFmpegUtils::Initialize()
{
    if (m_bInitialized)
        return;

    CScopedLock lock(m_mutex);

    if (!m_bInitialized) {
        // Initialize FFmpeg
        avcodec_init();

        // Register all formats and codecs
        av_register_all();

        av_log_set_level(12);

        m_bInitialized = true;
    }

}


bool FFmpegUtils::OpenCodec(AVCodecContext *a_pCodecCtx, AVCodec *a_pCodec)
{
    CScopedLock lock(m_mutex);

    if (avcodec_open(a_pCodecCtx, a_pCodec) < 0)
        return false;

    return true;
}

void FFmpegUtils::CloseCodec(AVCodecContext *a_pCodecCtx)
{
    CScopedLock lock(m_mutex);

    avcodec_close(a_pCodecCtx);
}
