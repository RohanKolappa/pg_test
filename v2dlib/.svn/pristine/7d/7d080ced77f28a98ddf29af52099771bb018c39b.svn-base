/*
 * vframeimager.hh
 *
 *  Created on: Jul 2, 2010
 *      Author: rkale
 */

#ifndef VFRAMEIMAGER_HH_
#define VFRAMEIMAGER_HH_

class CVideoFrameDecoder;

class CVideoFrameImager
{
public:
    CVideoFrameImager(int width, int height, int sliceSize);
    ~CVideoFrameImager();
    int CreateImage(unsigned char *aBuffer, int aBufferLength,
            unsigned char *aOutputBuffer, int aScaleFactor);

private:
    int m_nWidth;
    int m_nHeight;
    int m_nSliceSize;
    int m_nDecodeSize;
    CVideoFrameDecoder *m_pFrameDecoder;
    unsigned char * m_pDecodeBuffer;

};

#endif /* VFRAMEIMAGER_HH_ */
