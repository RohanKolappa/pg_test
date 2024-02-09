/*
 * vframeencoder_test.cpp
 *
 *  Created on: Oct 30, 2009
 *      Author: rkale
 */


#include "v2dlib_compatibility.h"
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "comp_utils.h"
#include "bitmap.hh"
#include "vframeencoder.hh"
#include "test_utils.hh"
#include "memory_tracker.h"

#define MAXENCODEFRAMELEN 5000000

// These are used by frame differencing tests below
static const char * md5array[] = {
    "a23ea7f0d9855ecaa6e6a2b9d6039832",
    "3ae36ce27d74fc96080f803ead6b7ee7",
    "192d93c77d2ecc7299a419f772810f29",
    "4ae4517d431e971304e33254a5934587",
    "d5cdcb9013260bed38474cc9a49ac254",
    "f112266f17b0729094abddc270c66bc8",
    "53099ad29047e1ef50e3a96868099ed4",
    "984a75c4dda95a619d4a9f7cb6415aec",
    "32664cef3efe801e83d23a4ccce717ad",
    "2c79d6e614ccdce39d931043fd6eeeee",
};



using namespace std;

typedef struct FrameEncoderInfo {
    std::string m_strBMPFile;
    int m_nWidth;
    int m_nHeight;
    size_t m_nSliceSize;
    int m_nMQuant;
    int m_nDCQuant;
    std::string m_strOrigMD5;
    std::string m_strRealMD5;
} FrameEncoderInfo_t;



// The fixture for testing encoding of video frames.
class CV2DVideoFrameEncoderTest :
    public testing::TestWithParam<FrameEncoderInfo_t> {
protected:

    virtual void SetUp() {

        m_bSetupError = false;
        m_strError = "";
        m_pRawBuf = NULL;
        m_frameEncoder = NULL;

        m_pEncodeBuf = new unsigned char[MAXENCODEFRAMELEN];
    }

    virtual void TearDown() {
        delete m_frameEncoder;
        delete [] m_pRawBuf;
        delete [] m_pEncodeBuf;
    }

    virtual int ReadDataFile(std::string rawFile)
    {

        std::string strDataDir = GTestUtils::GetDataDir("..");
        std::string strRawFile = strDataDir + "/video/frames/encoder/" + rawFile;

        CBitmap oRawBmp(strRawFile.c_str());
        unsigned int nRawBmpSize =
            oRawBmp.GetWidth() * oRawBmp.GetHeight() * 4;

        delete [] m_pRawBuf;
        m_pRawBuf = new unsigned char[nRawBmpSize];
        if(m_pRawBuf == NULL) {
            m_strError = "Failed due to insufficient memory";
            return -1;
        }

        if (oRawBmp.GetBitsBGRA(m_pRawBuf, nRawBmpSize) == false) {
            m_strError = "Error in CBitmap::GetBits()";
            return -1;
        }

       return 0;
    }


protected:
    bool m_bSetupError;
    std::string m_strError;

    unsigned char* m_pRawBuf;
    unsigned char* m_pEncodeBuf;

    CVideoFrameEncoder *m_frameEncoder;
};

// Warning: ensure that the length of all your samples fits
// the size MAXENCODEFRAMELEN defined above. Else bump it up
static FrameEncoderInfo_t frames[] =
{
    {"debug_1280_1024.bmp",
      1280, 1024, 32, 3, 3,
      "5ae0f700b3e6114705fbbc7badece7f9", "a8d10aac5ee8616720c624f249d419ac",
    },
    {"random_5_1280_1024.bmp",
     1280, 1024, 32, 3, 3,
     "b07cfe9373c38678190e1165b8ec2980", "39be0e44e926cd25f5fd9d72f162275b",
    },
};


INSTANTIATE_TEST_CASE_P(EncodeFrame,
        CV2DVideoFrameEncoderTest,
        ::testing::ValuesIn(frames));



TEST_P(CV2DVideoFrameEncoderTest, EncodeFrame) {


    if(m_bSetupError) {
        FAIL() << m_strError.c_str();
        return;
    }

    FrameEncoderInfo_t info = GetParam();

    if (ReadDataFile(info.m_strBMPFile) < 0) {
        FAIL() << m_strError.c_str();
        return;
    }

    int mquant = info.m_nMQuant;
    int dcquant = info.m_nDCQuant;
    int nSliceSize = info.m_nSliceSize;
    int width = info.m_nWidth;
    int height = info.m_nHeight;

    // Test using the original comp_utils implementation
    initFrameParams(width, height, mquant, dcquant, nSliceSize);
    size_t nEncodeLen = encodeOneFrameToFile(m_pRawBuf, (char *)m_pEncodeBuf, 0);

    char md5sum[40];
    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum);

    EXPECT_STREQ(md5sum, info.m_strOrigMD5.c_str());


    // Also test using new CVideoFrameEncoder class
    // Since the class uses a new DCT implementation, the results are not exactly
    // the same.

    m_frameEncoder = new CVideoFrameEncoder(width, height, mquant, dcquant);
    m_frameEncoder->SetTestable(true);
    ::memset(m_pEncodeBuf, 0x00, MAXENCODEFRAMELEN);
    m_frameEncoder->SetBitmap(m_pRawBuf);
    m_frameEncoder->UpdateAll();
    m_frameEncoder->EncodeFrame();
    nEncodeLen = m_frameEncoder->GetNextFrame(m_pEncodeBuf);
    //nEncodeLen = frameEncoder->EncodeFrame(m_pEncodeBuf, m_pRawBuf);
    char md5sum_real[40];
    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum_real);


    //printf("md5sum = \"%s\", \"%s\"\n", md5sum, md5sum_real);

    EXPECT_STREQ(md5sum_real, info.m_strRealMD5.c_str());

}

TEST_F(CV2DVideoFrameEncoderTest, FrameDifferencing) {

    if(m_bSetupError) {
        FAIL() << m_strError.c_str();
        return;
    }

    if (ReadDataFile("random_5_1280_1024.bmp") < 0) {
        FAIL() << m_strError.c_str();
        return;
    }

    int mquant = 3;
    int dcquant = 3;
    int width = 1280;
    int height = 1024;

    m_frameEncoder = new CVideoFrameEncoder(width, height, mquant, dcquant);
    m_frameEncoder->SetTestable(true);
    ::memset(m_pEncodeBuf, 0x00, MAXENCODEFRAMELEN);
    m_frameEncoder->SetBitmap(m_pRawBuf);
    m_frameEncoder->UpdateAll();
    m_frameEncoder->EncodeFrame();
    int nEncodeLen = m_frameEncoder->GetNextFrame(m_pEncodeBuf);

    char md5sum[40];
    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum);
    // printf("md5sum = %s\n", md5sum);
    EXPECT_STREQ("39be0e44e926cd25f5fd9d72f162275b", md5sum);

    m_frameEncoder->UpdatePixel(0, 0);
    m_frameEncoder->UpdatePixel(3, 0);
    m_frameEncoder->UpdatePixel(0, 7);
    m_frameEncoder->UpdatePixel(0, 8);
    m_frameEncoder->UpdatePixel(1279, 0);
    m_frameEncoder->UpdatePixel(1280, 0);
    m_frameEncoder->EncodeFrame();

    nEncodeLen = m_frameEncoder->GetNextFrame(m_pEncodeBuf);

    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum);
    // printf("md5sum = %s\n", md5sum);
    EXPECT_STREQ("bcafc02ce3dff2003023a80efd97dba7", md5sum);


    // Test with 10 randomly generated rectangles
    // The encoded output is known and recorded as md5 checksums
    GTestUtils::SetRandomSeed(5);
    for (int i = 0; i < 10; i++) {
        int x = GTestUtils::GetRandomNumber() % 640;
        int y = GTestUtils::GetRandomNumber() % 512;
        int width = GTestUtils::GetRandomNumber() % 640;
        int height = GTestUtils::GetRandomNumber() % 512;


        m_frameEncoder->UpdateRectangle(x, y, width, height);

        m_frameEncoder->EncodeFrame();

        nEncodeLen = m_frameEncoder->GetNextFrame(m_pEncodeBuf);

        GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum);

        EXPECT_STREQ(md5array[i], md5sum);

        // To recreate checksums of samples, un-comment next line
        // and run test. Then cut-paste into above md5array[]
        //  printf("        \"%s\",\n", md5sum);

    }
}


