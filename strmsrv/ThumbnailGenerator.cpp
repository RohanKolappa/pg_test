/*
 * CThumbnailGenerator.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: durga
 */

#include "strmsrv_compatibility.h"
#include "FFmpegHeaders.hh"

#ifndef WINDOWS
#include <jpeglib.h>
#endif
#include <setjmp.h>

#include "ThumbnailGenerator.h"
#include "vframedecoder.hh"
#include "bitmap.hh"

CThumbnailGenerator::CThumbnailGenerator(int a_nSrcWidth,
                                         int a_nSrcHeight,
                                         ePixelFormat eSrcFormat)
: m_nScaledWidth(0)
, m_nScaledHeight(0)
, m_pSwsCtx(NULL)
, m_pScaledFrame(NULL)
, m_pScaledData(NULL)
{
    m_nSrcWidth = a_nSrcWidth;
    m_nSrcHeight = a_nSrcHeight;

    m_eSrcPixelFormat = eSrcFormat;
    m_eDstPixelFormat = eRGB24;

    if (m_eSrcPixelFormat == eRGB24) {
    	m_nBytesPerPixel = 3;
    }
    else if(m_eSrcPixelFormat == eBGRX32) {
    	m_nBytesPerPixel = 4;
    }
    else {
    	m_nBytesPerPixel = 0;
    }

    m_nScaledDataSize = 0;

    m_pScaledFrame = avcodec_alloc_frame();
}

CThumbnailGenerator::~CThumbnailGenerator()
{
    if (m_pScaledFrame != NULL) {
        av_free(m_pScaledFrame);
    }
    m_pScaledFrame = NULL;

    if (m_pScaledData != NULL) {
        delete [] m_pScaledData;
    }
    m_pScaledData = NULL;

    if (m_pSwsCtx != NULL) {
        sws_freeContext(m_pSwsCtx);
    }
    m_pSwsCtx = NULL;
}

#ifndef WINDOWS

// Callback functions needed by the jpeg library so that it
// can write to our user defined memory buffer instead of
// to a file
static void init_buffer(jpeg_compress_struct* cinfo) {}

static boolean empty_buffer(jpeg_compress_struct* cinfo) {
    return TRUE;
}

static void term_buffer(jpeg_compress_struct* cinfo) {}

struct my_error_mgr {
    struct jpeg_error_mgr pub;    /* "public" fields */

    jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}
#endif

int CThumbnailGenerator::CreateImage(unsigned char **aBuffer,
                                     unsigned char **aOutputBuffer,
                                     const int a_nOutBufferSize) {
    ScaleImage(aBuffer);

#ifndef WINDOWS
    // Create a jpeg image
    struct jpeg_compress_struct cinfo;

    /* Initialize mem based destination manager */
    struct jpeg_destination_mgr dmgr;
    dmgr.init_destination    = init_buffer;
    dmgr.empty_output_buffer = empty_buffer;
    dmgr.term_destination    = term_buffer;
    dmgr.next_output_byte    = *aOutputBuffer;
    dmgr.free_in_buffer      = a_nOutBufferSize;

    /* We set up the normal JPEG error routines, then override error_exit. */
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
      /* If we get here, the JPEG code has signaled an error.
       * We need to clean up the JPEG object, close the input file, and return.
       */
      jpeg_destroy_compress(&cinfo);
      return -1;
    }

    jpeg_create_compress(&cinfo);

    cinfo.dest = &dmgr;

    cinfo.image_width = m_nScaledWidth;
    cinfo.image_height = m_nScaledHeight;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, 75, true);

    jpeg_start_compress(&cinfo, true);

    JSAMPROW row_pointer;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = (JSAMPROW) &m_pScaledData[cinfo.next_scanline * 3 * cinfo.image_width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    int imageSize = cinfo.dest->next_output_byte - *aOutputBuffer;
    if (imageSize == 0)
        imageSize = -1;

    jpeg_destroy_compress(&cinfo);
    return imageSize;
#endif
    return 0;
}

void CThumbnailGenerator::SetupScaledDimensions()
{
    if (m_nSrcWidth <= 0 || m_nSrcHeight <= 0) {
        return;
    }

    if (m_nScaledWidth <= 0 || m_nScaledHeight <= 0) {
        return;
    }

    // If the thumbnail resolution is greater than the original resolution then the thumbnail
    // resution should be same as the original resolution.
    if (m_nScaledWidth > m_nSrcWidth &&
            m_nScaledHeight > m_nSrcHeight) {
        m_nScaledWidth = m_nSrcWidth;
        m_nScaledHeight = m_nSrcHeight;

        return;
    }

    double dAspectRatio = ((double) m_nSrcWidth / m_nSrcHeight);

    int nTargetHeight = (int)RINT((double) m_nScaledWidth / dAspectRatio);
    if (nTargetHeight <= m_nScaledHeight) {
        m_nScaledHeight = nTargetHeight;
    }
    else {
    	m_nScaledWidth = (int)RINT((double) m_nScaledHeight * dAspectRatio);
    }
}

void CThumbnailGenerator::SetupScaler()
{
	if (m_nSrcWidth <= 0 || m_nSrcHeight <= 0) {
		return;
	}

	if (m_nScaledWidth <= 0 || m_nScaledHeight <= 0) {
		return;
	}

    if (m_pSwsCtx != NULL) {
        sws_freeContext(m_pSwsCtx);
        m_pSwsCtx = NULL;
    }

    enum PixelFormat src_pix_fmt;
    switch (m_eSrcPixelFormat) {
	case eRGB24:
		src_pix_fmt = PIX_FMT_RGB24;
		m_nBytesPerPixel = 3;
		break;

	case eBGRX32:
	default:
		src_pix_fmt = PIX_FMT_RGB32;
		m_nBytesPerPixel = 4;
		break;
	}

    enum PixelFormat dst_pix_fmt;
	switch(m_eDstPixelFormat) {
	case eRGB24:
	case eBGRX32:
	default:
		dst_pix_fmt = PIX_FMT_RGB24;
	}

	int nSwsFlags = SWS_BILINEAR;

    m_pSwsCtx = sws_getContext(m_nSrcWidth, m_nSrcHeight, src_pix_fmt, m_nScaledWidth, m_nScaledHeight,
                               dst_pix_fmt, nSwsFlags, NULL, NULL, NULL);

    m_nScaledDataSize = avpicture_get_size(dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);

    delete [] m_pScaledData;
    m_pScaledData = new unsigned char[m_nScaledDataSize];
    memset(m_pScaledData, 0x00, m_nScaledDataSize);

    avpicture_fill((AVPicture *) m_pScaledFrame, (uint8_t *)m_pScaledData, dst_pix_fmt, m_nScaledWidth, m_nScaledHeight);
}

void CThumbnailGenerator::ScaleImage(unsigned char **a_pInBuffer)
{
	const uint8_t* srcSlice[3] = {*a_pInBuffer, 0, 0};
	const int srcStride[3] = {m_nSrcWidth * m_nBytesPerPixel, 0, 0};

	sws_scale(m_pSwsCtx, srcSlice, srcStride, 0, m_nSrcHeight,
			  m_pScaledFrame->data, m_pScaledFrame->linesize);
}

void CThumbnailGenerator::SetThumbnailResolution(int a_nWidth, int a_nHeight)
{
    m_nScaledWidth = a_nWidth;
    m_nScaledHeight = a_nHeight;

    SetupScaledDimensions();
    SetupScaler();
}

void CThumbnailGenerator::SetSrcResolution(int a_nWidth, int a_nHeight, ePixelFormat eSrcPixelFormat)
{
    m_nSrcWidth = a_nWidth;
    m_nSrcHeight = a_nHeight;

    m_eSrcPixelFormat = eSrcPixelFormat;
}
