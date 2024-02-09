/*
 * standardsliceencoder.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: rkale
 */

#include "standardsliceencoder.hh"
#include "ipputils.h"
#include "imagemanip.h"
#include "bitstreamwriter.hh"
#include "memory_tracker.h"


CStandardSliceEncoder::CStandardSliceEncoder(int a_nMQuant, int a_nDCQuant,
        int a_nSliceSize) :
    CSliceEncoder(a_nMQuant, a_nDCQuant, a_nSliceSize, 0, 0)
{
}


CStandardSliceEncoder::~CStandardSliceEncoder()
{

}

void CStandardSliceEncoder::EncodeData()
{
    int yStride = m_nWidth;
    int uvStride = m_nWidth >> 1;
    short mQuant = m_nMMult;
    short dc_dct_pred[3] = {0, 0, 0};

    unsigned char * yArrayPos = m_yArray;
    unsigned char * uArrayPos = m_uArray;
    unsigned char * vArrayPos = m_vArray;

    int flag = 1;
    for (int blkNo = 0; blkNo < m_nSliceSize / 2; blkNo++) {
        EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, flag);
        yArrayPos += 8;
        EncodeOneBlock(m_block, uArrayPos, uvStride, mQuant, 1, dc_dct_pred, flag);
        uArrayPos += 8;

        EncodeOneBlock(m_block, yArrayPos, yStride, mQuant, 0, dc_dct_pred, 0);
        yArrayPos += 8;
        EncodeOneBlock(m_block, vArrayPos, uvStride, mQuant, 2, dc_dct_pred, flag);
        vArrayPos += 8;

        flag = 0;
        if ((yArrayPos - m_yArray) % m_nWidth == 0) {
            yArrayPos += yStride * 7;
        }
        if ((uArrayPos - m_uArray) % (m_nWidth / 2) == 0) {
            uArrayPos += uvStride * 7;
            vArrayPos += uvStride * 7;
        }
    }

}
