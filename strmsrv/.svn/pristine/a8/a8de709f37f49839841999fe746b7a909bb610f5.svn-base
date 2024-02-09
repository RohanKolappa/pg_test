/*
 * FFmpegUtils.hh
 *
 *  Created on: Apr 23, 2011
 *      Author: rkale
 */

#ifndef FFMPEGUTILS_HH_
#define FFMPEGUTILS_HH_

#include "mutex.hh"

struct AVCodecContext;
struct AVCodec;

class FFmpegUtils {
public:
    static void Initialize();
    static bool OpenCodec(AVCodecContext *a_pCodecCtx, AVCodec *a_pCodec);
    static void CloseCodec(AVCodecContext *a_pCodecCtx);

private:
    static CMutex m_mutex;
    static bool m_bInitialized;

};

#endif /* FFMPEGUTILS_HH_ */
