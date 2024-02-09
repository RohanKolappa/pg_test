/*
 * vframeimager.cpp
 *
 *  Created on: Jul 2, 2010
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"

#ifndef WINDOWS
#include <jpeglib.h>
#endif
#include <setjmp.h>

#include "vframeimager.hh"
#include "vframedecoder.hh"
#include "colortransform.h"
#include "bitmap.hh"
CVideoFrameImager::CVideoFrameImager(int width, int height, int sliceSize)
{
    m_nWidth = width;
    m_nHeight = height;
    m_nSliceSize = sliceSize;

    v2dDecConfiguration decConfig;
    decConfig.m_nWidth = width;
    decConfig.m_nHeight = height;
    decConfig.m_nSliceSize = sliceSize;
    decConfig.m_eEncodeFormat = eYUV422P;

    m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);
    m_nDecodeSize = decConfig.m_nDecodeBufLen;
    m_pDecodeBuffer = new unsigned char[m_nDecodeSize];

}

CVideoFrameImager::~CVideoFrameImager()
{
    delete[] m_pDecodeBuffer;
    delete m_pFrameDecoder;
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

// Creates a scaled JPEG image from an encoded V2D frames in the given buffer
// Can be used, for example, to create thumbnails out of frames picked up from
// srd module.
int CVideoFrameImager::CreateImage(unsigned char *aBuffer, int aBufferLength,
        unsigned char *aOutputBuffer, int aScaleFactor) {

    int rc = m_pFrameDecoder->Decode(aBuffer, aBufferLength, m_pDecodeBuffer, m_nDecodeSize);
    if (rc == -1)
        return -1;

    const unsigned char* pY = m_pDecodeBuffer;
    const unsigned char* pU = pY + (m_nWidth * m_nHeight);
    const unsigned char* pV = pU + (m_nWidth * m_nHeight) / 2;

    // Scale down the image by the given factor
    int scalefactor = aScaleFactor;
    int width = m_nWidth/scalefactor;
    int height = m_nHeight/scalefactor;

    unsigned char* pRGB = new unsigned char[width * height * 3];
    V2dLibColorTransform::YUVToRGBConverter oRGB;
    oRGB.yuv422ToRgbScaled(pY, pU, pV, pRGB, m_nWidth, m_nHeight, scalefactor);

#ifdef CREATE_BMP
    // Saves the image as a BMP file
    unsigned char* pRGBA = new unsigned char[width * height * 4];
    V2dLibColorTransform::ConvertRGBToRGBA(pRGB, pRGBA, width, height);

    CBitmap oSaveBmp;
    oSaveBmp.SetBitsBGRA(pRGBA, width, height);
    oSaveBmp.Save("image.bmp", 32);

    delete[] pRGBA;
#endif


#ifndef WINDOWS
    // Create a jpeg image
    struct jpeg_compress_struct cinfo;


    /* Initialize mem based destination manager */
    struct jpeg_destination_mgr dmgr;
    dmgr.init_destination    = init_buffer;
    dmgr.empty_output_buffer = empty_buffer;
    dmgr.term_destination    = term_buffer;
    dmgr.next_output_byte    = aOutputBuffer;
    dmgr.free_in_buffer      = width * height * 3;

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
    // jpeg_stdio_dest(&cinfo, outfile);

    cinfo.dest = &dmgr;

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, 75, true);

    jpeg_start_compress(&cinfo, true);

    JSAMPROW row_pointer;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = (JSAMPROW) &pRGB[cinfo.next_scanline * 3 * cinfo.image_width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    delete[] pRGB;

    jpeg_finish_compress(&cinfo);

    int imageSize = cinfo.dest->next_output_byte - aOutputBuffer;
    if (imageSize == 0)
        imageSize = -1;

    jpeg_destroy_compress(&cinfo);
    return imageSize;
#endif
    return 0;
}
