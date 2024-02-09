#ifndef __V2D_COLORTRANSFORMUTILS_H__
#define __V2D_COLORTRANSFORMUTILS_H__

#include "codecdefs.h"
#include <stddef.h>

#ifdef V2DLIBRARY_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

namespace V2dLibColorTransform
{
    class DLLEXPORT YUVToRGBConverter
    {
    public:
        enum ImageFormat {
            ImageFormat_RGB,
            ImageFormat_BGRX,
            ImageFormat_BGR,
        };

        enum SubSample {
            SubSample_444,
            SubSample_422,
            SubSample_420,
        };

        YUVToRGBConverter();
        ~YUVToRGBConverter();

        void yuvToRgb(
            const unsigned char* a_yArrayPos,
            const unsigned char* a_uArrayPos, 
            const unsigned char* a_vArrayPos,
            unsigned char* a_BmpPos, 
            const size_t a_nFrameWidth,
            const size_t a_nFrameHeight, 
            SubSample a_nSubsample);


        void yuv422ToRgbScaled(
                const unsigned char* a_yArrayPos,
                const unsigned char* a_uArrayPos,
                const unsigned char* a_vArrayPos,
                unsigned char *a_outBuffer,
                const size_t a_nFrameWidth,
                const size_t a_nFrameHeight,
                unsigned int a_nScaleFactor,
                unsigned int a_eFormat = ImageFormat_RGB);

        void yuv422ToRGBFormat(
                const unsigned char *a_slicePlanes[3],
                int a_sourceStride[],
                unsigned int a_width,
                unsigned int a_height,
                unsigned char *a_destBuffer,
                int a_destStride,
                ImageFormat a_eFormat);

        void yuv444ToRGBFormat(
                const unsigned char *a_slicePlanes[3],
                int a_sourceStride[],
                unsigned int a_width,
                unsigned int a_height,
                unsigned char *a_destBuffer,
                int a_destStride,
                ImageFormat a_eFormat);


        void yuv422ToBGRBlock(
            const unsigned char* a_pY,
            const unsigned char* a_pU,
            const unsigned char* a_pV,
            const int a_YUVStride,
            unsigned char* a_pDst,
            const int rgbStride, const int bytesPerPixel)
        {
            register unsigned char _y, _u, _v;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    _y = a_pY[j];
                    _u = a_pU[j >> 1];
                    _v = a_pV[j >> 1];

                    *a_pDst++ = _B(_y, _u, _v);
                    *a_pDst++ = _G(_y, _u, _v);
                    *a_pDst++ = _R(_y, _u, _v);
                    a_pDst += bytesPerPixel - 3;
                }
                a_pY += a_YUVStride;
                a_pU += a_YUVStride >> 1;
                a_pV += a_YUVStride >> 1;
                a_pDst += rgbStride - (8 * bytesPerPixel);
            }
        }


    private:
        int RGB_Y[256];
        int R_V[256];
        int G_U[256];
        int G_V[256];
        int B_U[256];

        unsigned char _R(unsigned char a_y, unsigned char a_u, unsigned char a_v);
        unsigned char _G(unsigned char a_y, unsigned char a_u, unsigned char a_v);
        unsigned char _B(unsigned char a_y, unsigned char a_u, unsigned char a_v);

        void InitYuvRgbLookupTable();
    };

    inline unsigned char YUVToRGBConverter::_R(unsigned char a_y, unsigned char a_u, unsigned char a_v)
    {
        return _S( (RGB_Y[a_y] + R_V[a_v]) >> 13 );
    }

    inline unsigned char YUVToRGBConverter::_G(unsigned char a_y, unsigned char a_u, unsigned char a_v)
    {
        return _S( (RGB_Y[a_y] - G_U[a_u] - G_V[a_v]) >> 13 );
    }

    inline unsigned char YUVToRGBConverter::_B(unsigned char a_y, unsigned char a_u, unsigned char a_v)
    {
        return _S( (RGB_Y[a_y] + B_U[a_u] ) >> 13 );
    }

    void ConvertRGBToRGBA(const unsigned char *a_pRGB,
            unsigned char* a_pRGBA, 
            const size_t a_nWidth,
            const size_t a_nHeight);

} //namespace 

#endif //__V2D_COLORTRANSFORMUTILS_H__
