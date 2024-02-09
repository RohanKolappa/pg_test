/*
 * sliceencodedecode_test.cpp
 *
 *  Created on: Mar 16, 2011
 *      Author: rkale
 */


#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"

#include "v2dlib_compatibility.h"
#include "comp_utils.h"
#include "bitmap.hh"
#include "sliceencoder.hh"
#include "slicedecoder.hh"
#include "svcsliceencoder.hh"
#include "svcslicedecoder.hh"
#include "vframeencoder.hh"
#include "vframedecoder.hh"
#include "imagemanip.h"
#include "memory_tracker.h"

#define MAXENCODEFRAMELEN 50000

using namespace std;


class CSliceEncodeDecodeTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_pRawBuf = NULL;
        m_pEncodeBuf = new unsigned char[MAXENCODEFRAMELEN];
        m_pDecodedImageBuf = NULL;
        m_sliceEncoder = NULL;
        m_sliceDecoder = NULL;
        m_svcSliceEncoder = NULL;
        m_mquant = 3;
        m_dcquant = 2;
        m_nSliceSize = 32;
        m_sliceNum = 10;
    }

    virtual void TearDown() {
        delete m_sliceEncoder;
        delete m_sliceDecoder;
        delete [] m_pRawBuf;
        delete [] m_pEncodeBuf;
        delete [] m_pDecodedImageBuf;
    }


    void CreateInputImage(int width, int height) {
        m_width = width;
        m_height = height;
        m_nRawBufLen = m_width * m_height * 4;
        m_pRawBuf = new unsigned char[m_nRawBufLen];
        unsigned char *cp = m_pRawBuf;
        for (int i = 0; i < m_height; i++) {
            for (int j = 0; j < m_width; j++) {
                *cp++ = (int)((255.0 * i)/m_height);
                *cp++ = 255 - (int)((255.0 * i)/m_height);
                *cp++ = (int)((255.0 * j)/m_width);
                *cp++ = 0;
            }
        }
    }

    void CreateSliceEncoder(eEncodeFormat a_eEncodeFormat) {
        if (a_eEncodeFormat == eSVC) {
            m_sliceEncoder = CSliceEncoder::CreateSliceEncoder(m_mquant, m_dcquant, m_width/8, m_height/8);
            m_svcSliceEncoder = dynamic_cast<CSVCSliceEncoder *>(m_sliceEncoder);
        }
        else {
            m_sliceEncoder = CSliceEncoder::CreateSliceEncoder(m_mquant, m_dcquant, m_nSliceSize);
        }

        m_sliceEncoder->SetTestable(true);
    }

    void EncodeSlice() {
        m_nEncodedBufLen = m_sliceEncoder->EncodeSlice(m_pEncodeBuf, m_pRawBuf, m_width, m_sliceNum);
        //printf("Encoded Length %d\n", m_nEncodedBufLen);
    }

    void CreateSliceDecoder(eEncodeFormat a_eEncodeFormat) {
        m_nDecodedBufLen = m_width * m_height * 4;
        m_pDecodedImageBuf = new unsigned char[m_nDecodedBufLen];

        m_sliceDecoder = CSliceDecoder::CreateSliceDecoder(a_eEncodeFormat);
        CSVCSliceDecoder *svcSliceDecoder = dynamic_cast<CSVCSliceDecoder *>(m_sliceDecoder);
        if (svcSliceDecoder != NULL)
            svcSliceDecoder->SetSliceDimensions(m_width/8, m_height/8);
    }


    void DecodeSlice() {
        unsigned char *m_pY = m_pDecodedImageBuf;
        unsigned char *m_pU = m_pY + (m_width * m_height);
        unsigned char *m_pV = m_pU + (m_width * m_height);
        int sliceNum = 0;
        memset(m_pDecodedImageBuf, 0x00, m_nDecodedBufLen);
        bool bRet = m_sliceDecoder->Decode(m_pEncodeBuf, m_nEncodedBufLen,
                m_nSliceSize, m_pY, m_pU, m_pV, sliceNum);
        EXPECT_TRUE(bRet);
        EXPECT_EQ(m_sliceNum, sliceNum);
    }

    void VerifyDecodedSlice(const char *md5sum_expected) {
        char md5sum_real[40];
        GTestUtils::MD5Sum(m_pDecodedImageBuf, m_nDecodedBufLen, md5sum_real);
        //printf("md5sum = \"%s\"\n", md5sum_real);
        EXPECT_STREQ(md5sum_expected, md5sum_real);
    }

    void AnalyzeLayers() {
        SVCSliceHeader sliceHeader;
        ParseSVCSliceLayers(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader);

        printf("Number of Layers = %d\n", sliceHeader.nNumberOfLayers);
        for (int i = 0; i < (int)sliceHeader.nNumberOfLayers; i++) {
            printf("Layer %d: Type = %d\n", i, sliceHeader.layerHeaders[i].nLayerType);
            printf("Layer %d: Size = %d\n", i, sliceHeader.layerHeaders[i].nLayerSize);
        }
    }

    unsigned char* m_pRawBuf;
    unsigned char* m_pEncodeBuf;
    unsigned char* m_pDecodedImageBuf;

    CSliceEncoder *m_sliceEncoder;
    CSliceDecoder *m_sliceDecoder;
    CSVCSliceEncoder *m_svcSliceEncoder;

    int m_nRawBufLen;
    int m_nEncodedBufLen;
    int m_nDecodedBufLen;
    int m_mquant;
    int m_dcquant;
    int m_width;
    int m_height;
    int m_nSliceSize;
    int m_sliceNum;
};

#define SVC_DEF_SLICE_WIDTH     8
#define SVC_DEF_SLICE_HEIGHT    4

#define STANDARD_OUTPUT_MD5SUM "18a9dff4e6b236728cd71b41533f6676"
#define STANDARD_SVC_OUTPUT_MD5SUM "18bfb33600cb1761cb59c612800aac01"

TEST_F(CSliceEncodeDecodeTest, Standard) {
    CreateInputImage(m_nSliceSize * 8, 8);
    CreateSliceEncoder(eYUV422P);
    EncodeSlice();
    CreateSliceDecoder(eYUV422P);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_OUTPUT_MD5SUM);
}

TEST_F(CSliceEncodeDecodeTest, SVC) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}


TEST_F(CSliceEncodeDecodeTest, SVCLongSlice) {
    CreateInputImage(m_nSliceSize * 8, 8);
    CreateSliceEncoder(eSVC);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_OUTPUT_MD5SUM);
}

TEST_F(CSliceEncodeDecodeTest, SVCParseSliceLayers) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    EncodeSlice();


    SVCSliceHeader sliceHeader;
    ParseSVCSliceLayers(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader);

    ASSERT_EQ(3, (int) sliceHeader.nNumberOfLayers);

    EXPECT_EQ(0, (int) sliceHeader.layerHeaders[0].bLastLayer);
    EXPECT_EQ(0, (int) sliceHeader.layerHeaders[1].bLastLayer);
    EXPECT_EQ(1, (int) sliceHeader.layerHeaders[2].bLastLayer);

    EXPECT_EQ(91, (int) sliceHeader.layerHeaders[0].nLayerSize);
    EXPECT_EQ(177, (int) sliceHeader.layerHeaders[1].nLayerSize);
    EXPECT_EQ(35, (int) sliceHeader.layerHeaders[2].nLayerSize);

    EXPECT_EQ(SVC_LAYERTYPE_DCLAYER, (int) sliceHeader.layerHeaders[0].nLayerType);
    EXPECT_EQ(SVC_LAYERTYPE_MIDAC4x4LAYER, (int) sliceHeader.layerHeaders[1].nLayerType);
    EXPECT_EQ(SVC_LAYERTYPE_REMAC4x4LAYER, (int) sliceHeader.layerHeaders[2].nLayerType);
}


TEST_F(CSliceEncodeDecodeTest, SVCStripRemLayer) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    EncodeSlice();

    SVCSliceHeader sliceHeader;
    ParseSVCSliceLayers(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader);

    ASSERT_EQ(3, (int) sliceHeader.nNumberOfLayers);

    unsigned char pOutBuf[1024];
    int strippedLength = StripSVCLayer(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader,
            pOutBuf, SVC_LAYERTYPE_REMAC4x4LAYER);
    EXPECT_EQ(272, strippedLength);  
    SVCSliceHeader strippedSliceHeader;
    ParseSVCSliceLayers(pOutBuf, strippedLength, &strippedSliceHeader);
    ASSERT_EQ(2, (int) strippedSliceHeader.nNumberOfLayers);
    EXPECT_EQ(0, (int) strippedSliceHeader.layerHeaders[0].bLastLayer);
    EXPECT_EQ(1, (int) strippedSliceHeader.layerHeaders[1].bLastLayer); 

    EXPECT_EQ(91, (int) strippedSliceHeader.layerHeaders[0].nLayerSize);
    EXPECT_EQ(177, (int) strippedSliceHeader.layerHeaders[1].nLayerSize);

    EXPECT_EQ(SVC_LAYERTYPE_DCLAYER, (int) strippedSliceHeader.layerHeaders[0].nLayerType);
    EXPECT_EQ(SVC_LAYERTYPE_MIDAC4x4LAYER, (int) strippedSliceHeader.layerHeaders[1].nLayerType);

}

TEST_F(CSliceEncodeDecodeTest, SVCStripQuadLayer) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    EncodeSlice();

    SVCSliceHeader sliceHeader;
    ParseSVCSliceLayers(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader);

    ASSERT_EQ(3, (int) sliceHeader.nNumberOfLayers);

    unsigned char pOutBuf[1024];
    int strippedLength = StripSVCLayer(m_pEncodeBuf, m_nEncodedBufLen, &sliceHeader,
            pOutBuf, SVC_LAYERTYPE_MIDAC4x4LAYER);
    EXPECT_EQ(95, strippedLength);

    SVCSliceHeader strippedSliceHeader;
    ParseSVCSliceLayers(pOutBuf, strippedLength, &strippedSliceHeader);
    ASSERT_EQ(1, (int) strippedSliceHeader.nNumberOfLayers);
    EXPECT_EQ(1, (int) strippedSliceHeader.layerHeaders[0].bLastLayer);

    EXPECT_EQ(91, (int) strippedSliceHeader.layerHeaders[0].nLayerSize);

    EXPECT_EQ(SVC_LAYERTYPE_DCLAYER, (int) strippedSliceHeader.layerHeaders[0].nLayerType);
}

TEST_F(CSliceEncodeDecodeTest, SVCLayerDimension2) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    m_svcSliceEncoder->SetLayerDimension(2);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}

TEST_F(CSliceEncodeDecodeTest, SVCLayerDimension3) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    m_svcSliceEncoder->SetLayerDimension(3);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}


TEST_F(CSliceEncodeDecodeTest, SVCLayerDimension5) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    m_svcSliceEncoder->SetLayerDimension(5);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}

TEST_F(CSliceEncodeDecodeTest, SVCLayerDimension6) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    m_svcSliceEncoder->SetLayerDimension(6);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}

TEST_F(CSliceEncodeDecodeTest, SVCLayerDimension7) {
    CreateInputImage(SVC_DEF_SLICE_WIDTH * 8, SVC_DEF_SLICE_HEIGHT * 8);
    CreateSliceEncoder(eSVC);
    m_svcSliceEncoder->SetLayerDimension(7);
    EncodeSlice();
    CreateSliceDecoder(eSVC);
    DecodeSlice();
    VerifyDecodedSlice(STANDARD_SVC_OUTPUT_MD5SUM);
}
