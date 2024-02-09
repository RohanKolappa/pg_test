#ifndef __V2D_IMAGEMANIPUTILS_H__
#define __V2D_IMAGEMANIPUTILS_H__

namespace V2DLib
{
    void transposeAndAdd128AndPutBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock);

    void add128AndPutTransposedBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock,
        const unsigned int a_nStride);

    void add128AndPutBlock (
        const short* a_psBlock,
        unsigned char* a_pucBlock,
        const unsigned int a_nStride);

    void copyImagePlane(
        const unsigned char* a_pSrc,
        const unsigned int a_nSrcStride,
        unsigned int width,
        unsigned char* a_pDst,
        const unsigned int a_nDstStride);

    void copyBlockToImage(
        const unsigned char* a_pSrc,
        unsigned char* a_pDst,
        const unsigned int a_nStride);

    void copyBlockToImageInverted(
        const unsigned char* a_pSrc,
        unsigned char* a_pDst,
        const unsigned int a_nStride);

    void markBlockInImage(unsigned char* a_pDst,
            const int rgbStride,
            unsigned char red,
            unsigned char green,
            unsigned char blue,
            int bIs24Bit);

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
            int bIs24Bit);

    void computeSliceSize(int width, int height, int & slice_size,
            int & encoded_width, int & encoded_height, int & slices_per_frame);

    void computeVideoTimings(int width, int height,
            float & refresh_rate, float & pixel_clock,
            int & hfp, int & hbp, int & hsync,
            int & vfp, int & vbp, int & vsync);


    void dumpBuffer(const unsigned char *buffer, const char *message = 0);
    void dumpBuffer4x4(const unsigned char *buffer, int istride, const char *message = 0);
    void dumpBuffer8x8(const unsigned char *buffer, int istride, const char *message = 0);
    void dumpResidual8x8(const unsigned char *buffer, const unsigned char *buffer2,
            int stride, const char *message);
    void dumpBlock(const short *block, const char *message = 0);
    void dumpBlock16(const int *block, const char *message = 0);

    void tileImageBGRA(unsigned char* outBuff, int width, int height,
            unsigned char* insetBuff, int inset_width, int inset_height);
} //namespace


#endif //__V2D_IMAGEMANIPUTILS_H__
