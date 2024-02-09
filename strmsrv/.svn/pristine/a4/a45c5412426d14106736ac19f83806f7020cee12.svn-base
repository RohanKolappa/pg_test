/*
 * CThumbnailGenerator.h
 *
 *  Created on: Sep 24, 2012
 *      Author: durga
 */

#ifndef CTHUMBNAILGENERATOR_H_
#define CTHUMBNAILGENERATOR_H_

#include <string>

struct SwsContext;
struct AVFrame;

#include "codecdefs.h"


class CThumbnailGenerator {
public:
    CThumbnailGenerator(int a_nSrcWidth, int a_nSrcHeight, ePixelFormat eSrcFormat);
    ~CThumbnailGenerator();

    void SetSrcResolution(int a_nWidth, int a_nHeight, ePixelFormat eSrcFormat);
    void SetThumbnailResolution(int a_nWidth, int a_nHeight);

    int CreateImage(unsigned char **aBuffer, unsigned char **aOutputBuffer,
                    const int a_nOutBufferSize);

    int CreateImageFile(unsigned char **a_pInBuffer, FILE *a_pOutFile);

private:
    void SetupScaledDimensions();
    void SetupScaler();
    void ScaleImage(unsigned char **a_pInBuffer);

    int m_nSrcWidth;
    int m_nSrcHeight;
    int m_nScaledWidth;
    int m_nScaledHeight;
    int m_nSliceSize;
    int m_nDecodeSize;

    ePixelFormat m_eSrcPixelFormat;
    ePixelFormat m_eDstPixelFormat;

    int m_nBytesPerPixel;

    SwsContext * m_pSwsCtx;
    AVFrame * m_pScaledFrame;
    unsigned char * m_pScaledData;
    int m_nScaledDataSize;
};

#endif /* CTHUMBNAILGENERATOR_H_ */
