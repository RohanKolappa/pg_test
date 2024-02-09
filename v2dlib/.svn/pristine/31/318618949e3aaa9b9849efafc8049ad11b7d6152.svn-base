/*
 * rgbtoyuv.cpp
 *
 *  Created on: Oct 28, 2009
 *      Author: rkale
 */

#include "rgbtoyuv.hh"

#define AVERAGE_SUBSAMPLE

bool CRGBToYUV::m_bIsInitialized = false;

int CRGBToYUV::Y_R[256];
int CRGBToYUV::Y_B[256];
int CRGBToYUV::Y_G[256];
int CRGBToYUV::U_R[256];
int CRGBToYUV::U_G[256];
int CRGBToYUV::U_B[256];
int CRGBToYUV::V_R[256];
int CRGBToYUV::V_G[256];
int CRGBToYUV::V_B[256];

#define RGB2Y(red, green, blue) (short)(((  Y_R[red] + Y_G[green] + Y_B[blue]) >> 15) +  16)
#define RGB2U(red, green, blue) (short)(((- U_R[red] - U_G[green] + U_B[blue]) >> 15) + 128)
#define RGB2V(red, green, blue) (short)(((  V_R[red] - V_G[green] - V_B[blue]) >> 15) + 128)
#define _S(a)     (a)>255 ? 255 : (a)<0 ? 0 : (a)


// This function is used to do a RGB to YUV conversion for a slice within a broader
// frame.
// Parameter a_BmpPos points to the pixel of the top left corner of the slice
// a_nStride is the overall width so that we know how much to jump to get
//   at the next row of pixels
// Parameter a_nSplit specifies the pixel value from the beginning where the slice splits
//   in case the slice spans across a row
void CRGBToYUV::RGBToYUV(unsigned char * a_BmpPos, unsigned char *a_yArrayPos,
        unsigned char *a_uArrayPos, unsigned char *a_vArrayPos,
        int a_nWidth, int a_nHeight, int a_nStride, int a_bpp,
        int a_nSplit, int a_nSubsample)
{
    // There is probably a better way to do this...
    if (!m_bIsInitialized) {
        InitYuvRgbLookupTable();
        m_bIsInitialized = true;
    }

    unsigned char * rowPos;
    unsigned char blue, green, red;
#ifdef AVERAGE_SUBSAMPLE
    unsigned char lastblue = 0, lastgreen = 0, lastred = 0;
#endif
    int y;
    int row, col;

    if (a_nSubsample == 0) // 4:4:4
    {
        for (row = 0; row < a_nHeight; row++) {
            rowPos = a_BmpPos;
            for (col = 0; col < a_nWidth; col++) {
                if (col == a_nSplit)
                    rowPos += (a_nStride * 7);
                blue = *rowPos;
                green = *(rowPos + 1);
                red = *(rowPos + 2);
                *(a_yArrayPos++) = _S(RGB2Y(red, green, blue));
                *(a_uArrayPos++) = _S(RGB2U(red, green, blue));
                *(a_vArrayPos++) = _S(RGB2V(red, green, blue));
                rowPos += a_bpp;
            }
            a_BmpPos += a_nStride;
        }
    }
    else if (a_nSubsample == 1) // 4:2:2
    {
        for (row = 0; row < a_nHeight; row++) {
            rowPos = a_BmpPos;
            for (col = 0; col < a_nWidth; col++) {
                if (col == a_nSplit)
                    rowPos += (a_nStride * 7);
                blue = *rowPos;
                green = *(rowPos + 1);
                red = *(rowPos + 2);

                *(a_yArrayPos++) = y = _S(RGB2Y(red, green, blue));

#ifdef AVERAGE_SUBSAMPLE
                if (col % 2 == 0) {
                    lastred = red >> 1;
                    lastgreen = green >> 1;
                    lastblue = blue >> 1;
                }
                else {
                    *(a_uArrayPos++) = _S(RGB2U((red >> 1) + lastred,
                            (green >> 1) + lastgreen,
                            (blue >> 1) + lastblue));
                    *(a_vArrayPos++) = _S(RGB2V((red >> 1) + lastred,
                            (green >> 1) + lastgreen,
                            (blue >> 1) + lastblue));
                }
#else
                if (col % 2 == 0) {
                    *(a_uArrayPos++) = _S(RGB2U(red, green, blue));
                    *(a_vArrayPos++) = _S(RGB2V(red, green, blue));
                }
#endif
                rowPos += a_bpp;
            }
            a_BmpPos += a_nStride;
        }
    }
    else // 4:2:0
    {
        for (row = 0; row < a_nHeight; row++) {
            rowPos = a_BmpPos;
            if (row % 2 == 1) {
                for (col = 0; col < a_nWidth; col++) {
                    if (col == a_nSplit)
                        rowPos += (a_nStride * 7);
                    blue = *rowPos;
                    green = *(rowPos + 1);
                    red = *(rowPos + 2);

                    *(a_yArrayPos++) = y = _S(RGB2Y(red, green, blue));
                    if (col % 2 == 0) {
                        *(a_uArrayPos++) = _S(RGB2U(red, green, blue));
                        *(a_vArrayPos++) = _S(RGB2V(red, green, blue));
                    }
                    a_BmpPos += a_bpp;
                }
            }
            else {
                for (col = 0; col < a_nWidth; col++) {
                    if (col == a_nSplit)
                        rowPos += (a_nStride * 7);
                    blue = *rowPos;
                    green = *(rowPos + 1);
                    red = *(rowPos + 2);

                    *(a_yArrayPos++) = y = _S(RGB2Y(red, green, blue));
                    a_BmpPos += a_bpp;
                }
            }
            a_BmpPos += a_nStride;
        }
    }
}

void CRGBToYUV::YUV422ToYUV420(
        unsigned char *a_yArrayPos,
        unsigned char *a_uArrayPos,
        unsigned char *a_vArrayPos,
        int a_nStride,
        unsigned char *a_yArrayOutPos,
        unsigned char *a_uArrayOutPos,
        unsigned char *a_vArrayOutPos,
        int a_nOutStride,
        int a_nWidth, int a_nHeight)
{
    int row, col;
    int y, u, v;
    for (row = 0; row < a_nHeight/2; row++) {
        for (col = 0; col < a_nWidth/2; col++) {
            y = *(a_yArrayPos + col);
            y += *(a_yArrayPos + col + 1);
            y += *(a_yArrayPos + a_nStride + col);
            y += *(a_yArrayPos + a_nStride + col + 1);
            u =  *(a_uArrayPos + col/2);
            u +=  *(a_uArrayPos + a_nStride/2 + col/2);
            v =  *(a_vArrayPos + col/2);
            v +=  *(a_vArrayPos + a_nStride/2 + col/2);
            *(a_yArrayOutPos + col/2) = y >> 2;
            *(a_uArrayOutPos + col/4) = u >> 1;
            *(a_vArrayOutPos + col/4) = v >> 1;
        }
        a_yArrayPos += a_nStride;
        a_uArrayPos += a_nStride/2;
        a_vArrayPos += a_nStride/2;

        if (row % 2 == 0) {
            a_yArrayOutPos += a_nOutStride;
            a_uArrayOutPos += a_nOutStride/2;
            a_vArrayOutPos += a_nOutStride/2;
        }
    }
}

void CRGBToYUV::InitYuvRgbLookupTable()
{
    int i;

    for (i = 0; i < 256; i++) {
        Y_R[i] = 8421 * i;
        Y_G[i] = 16515 * i;
        Y_B[i] = 3211 * i;
        U_R[i] = 4850 * i;
        U_G[i] = 9536 * i;
        U_B[i] = 14385 * i;
        V_R[i] = 14385 * i;
        V_G[i] = 12059 * i;
        V_B[i] = 2327 * i;
    }
}
