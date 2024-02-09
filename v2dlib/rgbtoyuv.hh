/*
 * rgbtoyuv.hh
 *
 *  Created on: Oct 28, 2009
 *      Author: rkale
 */

#ifndef RGBTOYUV_HH_
#define RGBTOYUV_HH_

// TODO: Put this in a namespace

class CRGBToYUV {
public:

    static void RGBToYUV(unsigned char * a_BmpPos, unsigned char *a_yArrayPos,
            unsigned char *a_uArrayPos, unsigned char *a_vArrayPos, int a_nFrameWidth,
            int a_nFrameHeight, int a_nStride, int a_bpp,
            int a_nSplit, int a_nSubsample);
    static void YUV422ToYUV420(
            unsigned char *a_yArrayPos,
            unsigned char *a_uArrayPos,
            unsigned char *a_vArrayPos,
            int a_nStride,
            unsigned char *a_yArrayOutPos,
            unsigned char *a_uArrayOutPos,
            unsigned char *a_vArrayOutPos,
            int a_nOutStride,
            int a_nWidth, int a_nHeight);
private:
    static bool m_bIsInitialized;

    static int Y_R[256];
    static int Y_B[256];
    static int Y_G[256];
    static int U_R[256];
    static int U_G[256];
    static int U_B[256];
    static int V_R[256];
    static int V_G[256];
    static int V_B[256];

    static void InitYuvRgbLookupTable();

};


#endif /* RGBTOYUV_HH_ */
