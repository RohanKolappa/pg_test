/*
 * vframeencodedecode_test.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: rkale
 */


#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"

#include "v2dlib_compatibility.h"
#include "comp_utils.h"
#include "bitmap.hh"
#include "vframeencoder.hh"
#include "vframedecoder.hh"
#include "imagemanip.h"
#include "memory_tracker.h"

#define MAXENCODEFRAMELEN 5000000

using namespace std;


class CFrameEncodeDecodeTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_pRawBuf = NULL;
        m_pEncodeBuf = new unsigned char[MAXENCODEFRAMELEN];
        m_frameEncoder = NULL;
        m_pFrameDecoder = NULL;
        m_pDecodedImageBuf = NULL;
        m_mquant = 3;
        m_dcquant = 2;
    }

    virtual void TearDown() {
        delete m_frameEncoder;
        delete m_pFrameDecoder;
        delete [] m_pRawBuf;
        delete [] m_pEncodeBuf;
        delete [] m_pDecodedImageBuf;
    }


    void CreateInputImage(int width, int height) {
        m_width = width;
        m_height = height;
        m_nRawBufLen = m_width * m_height * 4;
        m_pRawBuf = new unsigned char[m_nRawBufLen];
        string insetfile = GTestUtils::GetDataDir("..") + "/video/bitmaps/inset.bmp";
        ASSERT_TRUE(GTestUtils::FileExists(insetfile));
        CBitmap oRawBmp(insetfile.c_str());
        int inset_width = oRawBmp.GetWidth();
        int inset_height = oRawBmp.GetHeight();

        unsigned int insetBufSize = inset_width * inset_height * 4;
        unsigned char *insetBuf = new unsigned char[insetBufSize];

        oRawBmp.GetBitsBGRA(insetBuf, insetBufSize);
        V2DLib::tileImageBGRA(m_pRawBuf, m_width, m_height,
                insetBuf, inset_width, inset_height);
        delete [] insetBuf;
    }

    void CreateEncoder(eEncodeFormat a_eEncodeFormat) {
        m_frameEncoder = new CVideoFrameEncoder(m_width, m_height,
                m_mquant, m_dcquant, a_eEncodeFormat);
        m_frameEncoder->SetTestable(true);
        m_nSliceSize = m_frameEncoder->GetSliceSize();
        m_frameEncoder->AllocateBitmap();
//        V2DLib::Dimension dimension = m_frameEncoder->GetEncodedDimension();
//        printf("Input Dimension = %d x %d\n", m_width, m_height);
//        printf("Encoded Dimension = %d x %d\n", dimension.width, dimension.height);
//        printf("Slice Size = %d\n", m_nSliceSize);
    }

    void EncodeFrame() {
        m_frameEncoder->UpdateFrameFromBuffer(m_pRawBuf);
        //m_frameEncoder->SetBitmap(m_pRawBuf);
        m_frameEncoder->UpdateAll();
        m_frameEncoder->EncodeFrame();
        m_nEncodedBufLen = m_frameEncoder->GetNextFrame(m_pEncodeBuf);
    }

    void CreateDecoder(eEncodeFormat a_eEncodeFormat) {
        v2dDecConfiguration decConfig;
        decConfig.m_nWidth        = m_width;
        decConfig.m_nHeight       = m_height;
        decConfig.m_nSliceSize    = m_nSliceSize;
        decConfig.m_eDecodeFormat = eBGRX32;
        decConfig.m_eEncodeFormat = a_eEncodeFormat;

        m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);

        m_nDecodedBufLen = m_width * m_height * 4;
        EXPECT_EQ(m_nDecodedBufLen, (int)decConfig.m_nDecodeBufLen);

        m_pDecodedImageBuf = new unsigned char[m_nDecodedBufLen];
    }

    void EnableSliceDebug() {
        ASSERT_TRUE(m_pFrameDecoder != NULL);
        m_pFrameDecoder->SetSliceDebug(true);
    }

    void DecodeFrame() {
        ASSERT_TRUE(m_pFrameDecoder != NULL);
        m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncodedBufLen, m_pDecodedImageBuf, m_nDecodedBufLen);
    }

    void VerifyDecodedFrame(const char *md5sum_expected) {
        char md5sum_real[40];
        GTestUtils::MD5Sum(m_pDecodedImageBuf, m_nDecodedBufLen, md5sum_real);
        //printf("md5sum = \"%s\"\n", md5sum_real);
        EXPECT_STREQ(md5sum_expected, md5sum_real);
    }

    void SaveInputImage() {
        CBitmap oSaveBmp;
        oSaveBmp.SetBitsBGRA(m_pRawBuf, m_width, m_height);
        oSaveBmp.Save("input.bmp");
    }

    void SaveOutputImage() {
        CBitmap oSaveBmp;
        oSaveBmp.SetBitsBGRA(m_pDecodedImageBuf, m_width, m_height);
        oSaveBmp.Save("output.bmp");
    }

    unsigned char* m_pRawBuf;
    unsigned char* m_pEncodeBuf;
    unsigned char* m_pDecodedImageBuf;

    int m_nRawBufLen;
    int m_nEncodedBufLen;
    int m_nDecodedBufLen;
    CVideoFrameEncoder *m_frameEncoder;
    CVideoFrameDecoder *m_pFrameDecoder;
    int m_mquant;
    int m_dcquant;
    int m_width;
    int m_height;
    int m_nSliceSize;
};

#define STANDARD_OUTPUT_MD5SUM "fa5a4ecf67740a083094d9dc07de0204"

TEST_F(CFrameEncodeDecodeTest, Standard) {
    CreateInputImage(1024, 768);
    //SaveInputImage();
    CreateEncoder(eYUV422P);
    EncodeFrame();
    CreateDecoder(eYUV422P);
    //EnableSliceDebug();
    DecodeFrame();
    VerifyDecodedFrame(STANDARD_OUTPUT_MD5SUM);
    //SaveOutputImage();
}


TEST_F(CFrameEncodeDecodeTest, StandardSplitSlices) {
    CreateInputImage(1200, 768);
    CreateEncoder(eYUV422P);
    EncodeFrame();
    CreateDecoder(eYUV422P);
    DecodeFrame();
    VerifyDecodedFrame("0fe2543d25e1a5ebc8af43652dbb22c8");
}

TEST_F(CFrameEncodeDecodeTest, StandardSplitSlicesWithSliceDebug) {
    CreateInputImage(1200, 768);
    CreateEncoder(eYUV422P);
    EncodeFrame();
    CreateDecoder(eYUV422P);
    EnableSliceDebug();
    DecodeFrame();
    VerifyDecodedFrame("e10b36fed95cdc6cb8753986f06a7ca4");
}

TEST_F(CFrameEncodeDecodeTest, SVC) {
    CreateInputImage(1024, 768);
    CreateEncoder(eSVC);
    EncodeFrame();
    CreateDecoder(eSVC);
    DecodeFrame();
    VerifyDecodedFrame(STANDARD_OUTPUT_MD5SUM);
}


TEST_F(CFrameEncodeDecodeTest, H264) {
    CreateInputImage(1024, 768);
    CreateEncoder(eH264);
    EncodeFrame();
    CreateDecoder(eH264);
    DecodeFrame();
    VerifyDecodedFrame("892cefdf0c0ac938f08a69cf5288130b");
}

TEST_F(CFrameEncodeDecodeTest, SVCDifferentSliceSize) {
    CreateInputImage(1024, 768);
    //SaveInputImage();
    CreateEncoder(eSVC);
    m_frameEncoder->SetSliceSize(16, 2);
    EncodeFrame();
    CreateDecoder(eSVC);
    //EnableSliceDebug();
    DecodeFrame();
    VerifyDecodedFrame(STANDARD_OUTPUT_MD5SUM);
    //SaveOutputImage();
}

TEST_F(CFrameEncodeDecodeTest, SVCPaddedFrameDimensions) {
    CreateInputImage(1034, 770);
    CreateEncoder(eSVC);
    EncodeFrame();
    CreateDecoder(eSVC);
    DecodeFrame();
    VerifyDecodedFrame("2961cf6947963fd270adbd6b5363a271");
}

TEST_F(CFrameEncodeDecodeTest, SVCFullEncoding) {
    CreateInputImage(1024, 768);
    CreateEncoder(eSVC);
    m_frameEncoder->SetEncodingType(FullEncoding);
    EncodeFrame();
    CreateDecoder(eSVC);
    DecodeFrame();
    VerifyDecodedFrame(STANDARD_OUTPUT_MD5SUM);
}
