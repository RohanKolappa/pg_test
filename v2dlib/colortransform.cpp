#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "colortransform.h"

namespace V2dLibColorTransform
{
    YUVToRGBConverter::YUVToRGBConverter()
    {
        InitYuvRgbLookupTable();
    }

    YUVToRGBConverter::~YUVToRGBConverter()
    {

    }

    void YUVToRGBConverter::InitYuvRgbLookupTable()
	{
		int i;
		for (i = -16; i < 240; i++)
			RGB_Y[i+16] = 9535 * i;
		for (i = -128; i < 128; i++) {
			R_V[i+128] = 13074  * i;
			G_U[i+128] = 3203  * i;
			G_V[i+128] = 6660  * i;
			B_U[i+128] = 16531 * i;
		}
	}

    void YUVToRGBConverter::yuvToRgb(
        const unsigned char* a_yArrayPos,
        const unsigned char* a_uArrayPos, 
        const unsigned char* a_vArrayPos,
        unsigned char* a_BmpPos, 
        const size_t a_nFrameWidth,
        const size_t a_nFrameHeight, 
        SubSample a_nSubsample)
    {
        unsigned char _r, _g, _b; 
        register unsigned char _y, _u, _v;

        if (a_nSubsample == SubSample_444) {
            for (size_t row = 0; row < a_nFrameHeight; row++) {
                for (size_t col = 0; col < a_nFrameWidth; col++) {
                    _y = a_yArrayPos[col];
                    _u = a_uArrayPos[col];
                    _v = a_vArrayPos[col];

                    _r = _R(_y, _u, _v);
                    _g = _G(_y, _u, _v);
                    _b = _B(_y, _u, _v);

                    a_BmpPos[0] = _b;
                    a_BmpPos[1] = _g;
                    a_BmpPos[2] = _r;
                    a_BmpPos += 3;
                }
                a_yArrayPos += a_nFrameWidth;
                a_uArrayPos += a_nFrameWidth;
                a_vArrayPos += a_nFrameWidth;
            }

        }
        else if (a_nSubsample == SubSample_422) {
            for (size_t row = 0; row < a_nFrameHeight; row++) {
                for (size_t col = 0; col < a_nFrameWidth; col++) {
                    _y = a_yArrayPos[col];
                    _u = a_uArrayPos[col >> 1];
                    _v = a_vArrayPos[col >> 1];

                    _r = _R(_y, _u, _v);
                    _g = _G(_y, _u, _v);
                    _b = _B(_y, _u, _v);

                    a_BmpPos[0] = _b;
                    a_BmpPos[1] = _g;
                    a_BmpPos[2] = _r;
                    a_BmpPos += 3;
                }

                a_yArrayPos += a_nFrameWidth;
                a_uArrayPos += a_nFrameWidth / 2;
                a_vArrayPos += a_nFrameWidth / 2;
            }
        }
        else {
            for (size_t row = 0; row < a_nFrameHeight; row++) {
                for (size_t col = 0; col < a_nFrameWidth; col++) {
                    _y = a_yArrayPos[col];
                    _u = a_uArrayPos[col >> 1];
                    _v = a_vArrayPos[col >> 1];

                    _r = _R(_y, _u, _v);
                    _g = _G(_y, _u, _v);
                    _b = _B(_y, _u, _v);

                    a_BmpPos[0] = _b;
                    a_BmpPos[1] = _g;
                    a_BmpPos[2] = _r;
                    a_BmpPos += 3;
                }

                a_yArrayPos += a_nFrameWidth;
                if (row % 2) {
                    a_uArrayPos += a_nFrameWidth/2;
                    a_vArrayPos += a_nFrameWidth/2;
                }
            }
        }
    }

    void YUVToRGBConverter::yuv422ToRgbScaled(
            const unsigned char* a_yArrayPos,
            const unsigned char* a_uArrayPos,
            const unsigned char* a_vArrayPos,
            unsigned char *a_outBuffer,
            const size_t a_nFrameWidth,
            const size_t a_nFrameHeight,
            unsigned int a_nScaleFactor,
            unsigned int a_eFormat)
    {
        int yy, uu, vv;
        int red, green, blue;
        if (!(a_nScaleFactor == 8 ||
                a_nScaleFactor == 4 ||
                a_nScaleFactor == 2 ||
                a_nScaleFactor == 1))
                return;

        unsigned int nScaleFactorSquare = a_nScaleFactor * a_nScaleFactor;

        for (size_t row = 0; row < a_nFrameHeight; row++) {
            if (row % a_nScaleFactor == (a_nScaleFactor - 1)) {
                for (size_t block = 0; block < a_nFrameWidth; block += a_nScaleFactor) {
                    yy = uu = vv = 0;
                    for (size_t irow = 0; irow < a_nScaleFactor; irow++) {
                        int stride = ((row - a_nScaleFactor + 1) + irow) * a_nFrameWidth;
                        for (size_t icol = 0; icol < a_nScaleFactor; icol++) {
                            yy += a_yArrayPos[stride + block + icol];
                            uu += a_uArrayPos[(stride + block + icol) >> 1];
                            vv += a_vArrayPos[(stride + block + icol) >> 1];
                        }
                    }
                    red = _R(yy/nScaleFactorSquare,
                            uu/nScaleFactorSquare, vv/nScaleFactorSquare);
                    green = _G(yy/nScaleFactorSquare,
                            uu/nScaleFactorSquare, vv/nScaleFactorSquare);
                    blue = _B(yy/nScaleFactorSquare,
                            uu/nScaleFactorSquare, vv/nScaleFactorSquare);
                    if (a_eFormat == ImageFormat_RGB) {
                        a_outBuffer[0] = red;
                        a_outBuffer[1] = green;
                        a_outBuffer[2] = blue;
                        a_outBuffer += 3;
                    }
                    else if (a_eFormat == ImageFormat_BGRX) {
                        a_outBuffer[0] = blue;
                        a_outBuffer[1] = green;
                        a_outBuffer[2] = red;
                        a_outBuffer += 4;
                    }

                }
            }
        }
    }


    void YUVToRGBConverter::yuv422ToRGBFormat(
            const unsigned char *a_slicePlanes[3],
            int a_sourceStride[],
            unsigned int a_width,
            unsigned int a_height,
            unsigned char *a_destBuffer,
            int a_destStride,
            ImageFormat a_eFormat)
    {
        unsigned char _r, _g, _b;
        register unsigned char _y, _u, _v;

        const unsigned char* yArrayPos = a_slicePlanes[0];
        const unsigned char* uArrayPos = a_slicePlanes[1];
        const unsigned char* vArrayPos = a_slicePlanes[2];

        for (size_t row = 0; row < a_height; row++) {
            unsigned char * pDest = a_destBuffer + row * a_destStride;
            for (size_t col = 0; col < a_width; col++) {
                _y = yArrayPos[col];
                _u = uArrayPos[col >> 1];
                _v = vArrayPos[col >> 1];

                _r = _R(_y, _u, _v);
                _g = _G(_y, _u, _v);
                _b = _B(_y, _u, _v);

                if (a_eFormat == ImageFormat_BGRX) {
                    pDest[0] = _b;
                    pDest[1] = _g;
                    pDest[2] = _r;
                    pDest += 4;
                }
                else if (a_eFormat == ImageFormat_RGB) {
                    pDest[0] = _r;
                    pDest[1] = _g;
                    pDest[2] = _b;
                    pDest += 3;
                }
                else if (a_eFormat == ImageFormat_BGR) {
                    pDest[0] = _b;
                    pDest[1] = _g;
                    pDest[2] = _r;
                    pDest += 3;
                }
            }

            yArrayPos += a_sourceStride[0];
            uArrayPos += a_sourceStride[1];
            vArrayPos += a_sourceStride[2];

        }
    }

    void YUVToRGBConverter::yuv444ToRGBFormat(
            const unsigned char *a_slicePlanes[3],
            int a_sourceStride[],
            unsigned int a_width,
            unsigned int a_height,
            unsigned char *a_destBuffer,
            int a_destStride,
            ImageFormat a_eFormat)
    {
        uint8_t _r, _g, _b;
        register uint8_t _y, _u, _v;

        const uint8_t * yArrayPos = a_slicePlanes[0];
        const uint8_t * uEvenArrayPos = a_slicePlanes[1];
        const uint8_t * vEvenArrayPos = a_slicePlanes[2];
        const uint8_t * uOddArrayPos = a_slicePlanes[1] + (a_sourceStride[1] * a_height);
        const uint8_t * vOddArrayPos = a_slicePlanes[2] + (a_sourceStride[2] * a_height);

        for(size_t row = 0; row < a_height; row++) {
            unsigned char * pDest = a_destBuffer + row * a_destStride;
            for(size_t col = 0; col < a_width; col++) {
                _y = yArrayPos[col];
                if (col % 2 == 0) {
                    _u = uEvenArrayPos[col >> 1];
                    _v = vEvenArrayPos[col >> 1];
                }
                else {
                    _u = uOddArrayPos[col >> 1];
                    _v = vOddArrayPos[col >> 1];
                }
                _r = _R(_y, _u, _v);
                _g = _G(_y, _u, _v);
                _b = _B(_y, _u, _v);

                if (a_eFormat == ImageFormat_BGRX) {
                    pDest[0] = _b;
                    pDest[1] = _g;
                    pDest[2] = _r;
                    pDest   += 4;
                }
                else if (a_eFormat == ImageFormat_RGB) {
                    pDest[0] = _r;
                    pDest[1] = _g;
                    pDest[2] = _b;
                    pDest   += 3;
                }
                else if (a_eFormat == ImageFormat_BGR) {
                    pDest[0] = _b;
                    pDest[1] = _g;
                    pDest[2] = _r;
                    pDest   += 3;
                }
            }

            yArrayPos += a_sourceStride[0];
            uEvenArrayPos += a_sourceStride[1];
            vEvenArrayPos += a_sourceStride[2];
            uOddArrayPos += a_sourceStride[1];
            vOddArrayPos += a_sourceStride[2];
        }
    }

    void ConvertRGBToRGBA(const unsigned char *a_pRGB,
            unsigned char* a_pRGBA, 
            const size_t a_nWidth,
            const size_t a_nHeight)
    {
        for(size_t i = 0, j = 0; i < a_nWidth*a_nHeight*3; i += 3)
        {
            a_pRGBA[j++] = a_pRGB[i+2];
            a_pRGBA[j++] = a_pRGB[i+1];
            a_pRGBA[j++] = a_pRGB[i];
            a_pRGBA[j++] = 0;
        }
    }

} //namespace

