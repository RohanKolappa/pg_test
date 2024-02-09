
#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "imagemanip.h"
#include "cvt.h"

namespace V2DLib
{
void transposeAndAdd128AndPutBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock)
{
    for(unsigned int i=0; i<8; i++)
    {
        for(unsigned int j=0; j<8; j++)
        {
            a_pucBlock[8*i+j] = _S(a_psBlock[8*j+i]+128);
        }
    }

}

void add128AndPutTransposedBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock,
        const unsigned int a_nStride)
{
    unsigned int nOffset = 0;
    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
            a_pucBlock[nOffset + j] = _S(a_psBlock[8 * j + i] + 128);
        }
        nOffset += a_nStride;
    }

}

void add128AndPutBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock,
        const unsigned int a_nStride)
{
    unsigned int nOffset = 0;
    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
            a_pucBlock[nOffset + j] = _S(a_psBlock[8 * i + j] + 128);
        }
        nOffset += a_nStride;
    }

}

void copyBlockToImage(
        const unsigned char* a_pSrc,
        unsigned char* a_pDst,
        const unsigned int a_nStride)
{
    unsigned int nOffset = 0;

    for(int i=0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            a_pDst[nOffset + j] = a_pSrc[8 * i + j];
        }
        nOffset += a_nStride;
    }
}

void copyImagePlane(
        const unsigned char* a_pSrc,
        const unsigned int a_nSrcStride,
        unsigned int width,
        unsigned char* a_pDst,
        const unsigned int a_nDstStride)
{
    for(int i=0; i < 8; i++)
    {
        memcpy(a_pDst, a_pSrc, width);
        a_pDst += a_nDstStride;
        a_pSrc += a_nSrcStride;
    }
}

void copyBlockToImageInverted(
        const unsigned char* a_pSrc,
        unsigned char* a_pDst,
        const unsigned int a_nStride)
{
    unsigned int nOffset = 7*a_nStride;

    for(int i=0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            a_pDst[nOffset + j] = a_pSrc[8 * i + j];
        }
        nOffset -= a_nStride;
    }
}

void markBlockInImage(unsigned char* a_pDst,
        const int rgbStride,
        unsigned char red,
        unsigned char green,
        unsigned char blue,
        int bIs24Bit)
{
    a_pDst += rgbStride * 3;
    for (int i = 3; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            if (j == 3 || j == 4) {
                *a_pDst++ = blue;
                *a_pDst++ = green;
                *a_pDst++ = red;
                if (!bIs24Bit)
                    a_pDst++;
            }
            else {
                if (bIs24Bit)
                    a_pDst += 3;
                else
                    a_pDst += 4;
            }
        }
        if (bIs24Bit)
            a_pDst += rgbStride - 24;
        else
            a_pDst += rgbStride - 32;
    }
}


void markSliceInImage(unsigned char* a_pDst,
        int sliceWidth,
        int sliceHeight,
        int regionWidth,
        int regionHeight,
        int bIsTail,
        int rgbStride,
        unsigned char red,
        unsigned char green,
        unsigned char blue,
        int bIs24Bit)
{
    for (int j = 0; j < 2; j++) {
        unsigned char *dst = a_pDst + j * rgbStride * (sliceHeight - 1);
        for (int i = 0; i < regionWidth; i++) {
            *dst++ = blue;
            *dst++ = green;
            *dst++ = red;
            if (!bIs24Bit)
                dst++;
        }
        if (regionHeight < sliceHeight)
            break;
    }

    int bytesPerPixel = bIs24Bit ? 3 : 4;
    for (int j = 0; j < 2; j++) {
        unsigned char *dst = a_pDst + j * (sliceWidth - 1) * bytesPerPixel;
        if (bIsTail && j == 0) {
            sliceWidth = regionWidth;
            continue;
        }
        for (int i = 0; i < regionHeight; i++) {
            *dst++ = blue;
            *dst++ = green;
            *dst++ = red;
            if (!bIs24Bit)
                dst++;
            dst += rgbStride - bytesPerPixel;
        }
        if (regionWidth < sliceWidth)
            break;
    }

}

#define MAX_NOBPSLICE           36
#define MIN_NOBPSLICE           20
#define MAX_SLICES_PER_FRAME    2048

// Compute slice size for version one codec
void computeSliceSize(int width, int height, int & slice_size,
        int & encoded_width, int & encoded_height, int & slices_per_frame)
{
    slice_size = MAX_NOBPSLICE;
    int size;
    int found = 0;

    // Encoded width should be multiple of 16 and
    // Encoded height should be multiple of 8
    encoded_width = ((width + 15) / 16) * 16;
    encoded_height = ((height + 7) / 8) * 8;

    int retry_count = 0;
    while (retry_count++ < 16) {
        for (size = MAX_NOBPSLICE; size >= MIN_NOBPSLICE; size -= 2) {
            if ((((encoded_width * encoded_height)/128) % size) == 0) {
                slice_size = size;
                found = 1;
                break;
            }
        }

        slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);

        if (!found && slices_per_frame < MAX_SLICES_PER_FRAME) {
            // Retry with expanded height
            encoded_height += 8;
            continue;
        }
    }

    slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);
    if (slices_per_frame >= MAX_SLICES_PER_FRAME) {
        int retry_count = 0;
        while (retry_count++ < 16) {
            // We need a bigger slice size so that number of slices
            // per frame is 2047 or less
            int min_slice_size = (int)((1.0 * encoded_width * encoded_height)/(2047.0 * 64.0)) + 1;
            if (min_slice_size % 2 != 0)
                min_slice_size++;

            // Search upwards from min slice size to double of min size
            found = 0;
            for (size = min_slice_size; size < min_slice_size * 2; size += 2) {
                if ((((encoded_width * encoded_height)/128) % size) == 0) {
                    slice_size = size;
                    slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                encoded_height += 8;
                continue;
            }
        }
    }

}

void computeVideoTimings(int width, int height,
        float & refresh_rate, float & pixel_clock,
        int & hfp, int & hbp, int & hsync,
        int & vfp, int & vbp, int & vsync)
{
    // Forward to implementation in cvt.c
    computeModeTimings(width, height,
            &refresh_rate, &pixel_clock,
            &hfp, &hbp, &hsync,
            &vfp, &vbp, &vsync);
}

void dumpBuffer(const unsigned char *buffer, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    int idx = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            printf("%02x ", buffer[idx]);
            ++idx;
        }
        printf("\n");
    }
}

void dumpBuffer4x4(const unsigned char *buffer, int stride, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%02x ", buffer[j]);
        }
        printf("\n");
        buffer += stride;
    }
}

void dumpBuffer8x8(const unsigned char *buffer, int stride, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%02x ", buffer[j]);
        }
        printf("\n");
        buffer += stride;
    }
}

void dumpResidual8x8(const unsigned char *buffer, const unsigned char *buffer2, int stride, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%02d ", buffer[j] - buffer2[j]);
        }
        printf("\n");
        buffer += stride;
        buffer2 += stride;
    }
}

void dumpBlock(const short *block, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    int idx = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("% 4d ", block[idx]);
            ++idx;
        }
        printf("\n");
    }
}

void dumpBlock16(const int *block, const char *message) {
    if (message != NULL)
        printf("%s:\n", message);
    for (int i = 0; i < 16; i++) {
        printf("%d ", block[i]);
    }
    printf("\n");
}


void tileImageBGRA(unsigned char* outBuff, int width, int height,
        unsigned char* insetBuff, int inset_width, int inset_height)
{

    for (int v_offset = 0; v_offset < height; v_offset += inset_height) {
        int copy_height = inset_height;
        if ((height - v_offset) < inset_height)
            copy_height = (height - v_offset);
        for (int h_offset = 0; h_offset < width; h_offset += inset_width) {
            int copy_width = inset_width;
            if ((width - h_offset) < inset_width)
                copy_width = (width - h_offset);

            for (int i = 0; i < copy_height; i++) {
                memcpy(outBuff + ((v_offset + i) * width * 4) + (h_offset * 4),
                        insetBuff + (i * inset_width * 4),
                        copy_width * 4);
            }
        }
    }

}


} //namespace

