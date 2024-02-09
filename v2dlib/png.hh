/*
 * png.hh
 *
 *  Created on: Nov 8, 2009
 *      Author: rkale
 */

#ifndef PNG_HH_
#define PNG_HH_

#ifndef WINDOWS
#include "png.h"
#endif

class DLLEXPORT CPng {

public:
    CPng(const char * a_FileName);
    ~CPng();

    int Overlay(unsigned char *inBuff, int frame_width, int frame_height);
    int Tile(unsigned char *inBuff, int frame_width, int frame_height);

    int GetBGRA(unsigned char *inBuff);

    void DisplayInfo();

    bool Ok() {
        return ! m_bError;
    }

    int GetWidth() {
        return m_nWidth;
    }
    int GetHeight() {
        return m_nHeight;
    }

private:
    FILE *m_pFP;
#ifndef WINDOWS
    png_structp m_pPngPtr;
    png_infop m_pInfoPtr;
    png_infop m_pEndInfo;
#endif
    bool m_bError;
#ifdef __XPI__
    png_uint_32 m_nWidth;
    png_uint_32 m_nHeight;
#else
    unsigned long m_nWidth;
    unsigned long m_nHeight;
#endif
    int m_nBitDepth;
    int m_nColorType;
    int m_nInterlaceType;
    int m_nCompressionType;
    int m_nFilterMethod;

    int Position(unsigned char *inBuff, int frame_width, int frame_height,
            int v_offset, int v_extent, int h_offset, int h_extent);

};

#endif /* PNG_HH_ */
