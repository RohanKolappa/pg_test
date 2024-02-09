#include "v2dlib_compatibility.h"

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "colortransform.h"

#include "test_utils.hh"

using namespace V2dLibColorTransform;

class ColorTransformTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_strDataDir = GTestUtils::GetDataDir("..");
        m_strDataDir += "/video/yuv2rgb";
        m_YUVBuffer = NULL;
        m_RGBBuffer = NULL;
    }

    virtual void TearDown() {
        delete [] m_YUVBuffer;
        delete [] m_RGBBuffer;
    }

    void CreateYUVandRGBBuffers(int width, int height) {

        m_width = width;
        m_height = height;
        m_YUVBufferLength = m_width * m_height * 3;
        m_RGBBufferLength = m_width * m_height * 4;

        m_YUVBuffer = new uint8_t[m_YUVBufferLength];
        m_RGBBuffer = new uint8_t[m_RGBBufferLength];

        for (int i = 0; i < m_YUVBufferLength; i++) {
            m_YUVBuffer[i] = i % 255;
        }

        memset(m_RGBBuffer, 0x00, m_RGBBufferLength);

        m_pY = m_YUVBuffer;
        m_pU = m_YUVBuffer + (m_width * m_height);
        m_pV = m_YUVBuffer + (m_width * m_height * 2);

        m_slicePlanes[0] = m_pY;
        m_slicePlanes[1] = m_pU;
        m_slicePlanes[2] = m_pV;
        m_sourceStride[0] = m_width;
        m_sourceStride[1] = m_width / 2;
        m_sourceStride[2] = m_width / 2;

    }

    void CopyUVEvenToUVOdd() {
        memcpy(m_pU + (m_width * m_height)/2, m_pU, (m_width * m_height)/2);
        memcpy(m_pV + (m_width * m_height)/2, m_pV, (m_width * m_height)/2);
    }

    void PrintMD5Sum() {
        char md5sum[GTestUtils::MD5SUMLEN];
        GTestUtils::MD5Sum(m_RGBBuffer, m_RGBBufferLength, md5sum);
        printf("md5 = %s\n", md5sum);
    }

    bool CheckMD5Sum(std::string ref_md5sum) {
        char md5sum[GTestUtils::MD5SUMLEN];
        GTestUtils::MD5Sum(m_RGBBuffer, m_RGBBufferLength, md5sum);
        if (strcmp(ref_md5sum.c_str(), md5sum) != 0) {
            printf("MD5 Mismatch\nExpected: %s\nComputed: %s\n",
                    ref_md5sum.c_str(), md5sum);
            return false;
        }
        return true;
    }


protected:
    std::string m_strDataDir;
    int m_width;
    int m_height;
    uint8_t * m_YUVBuffer;
    uint8_t * m_RGBBuffer;
    int m_YUVBufferLength;
    int m_RGBBufferLength;
    uint8_t * m_pY;
    uint8_t * m_pU;
    uint8_t * m_pV;
    const uint8_t *m_slicePlanes[3];
    int m_sourceStride[3];
    YUVToRGBConverter m_YUVConverter;
};

#define BGR24_MD5_SUM "df418b32523b7c7829324e0806cd991c"
#define BGRX_MD5_SUM "30fd17752f3287565978867f19e60a54"
#define RGB24_MD5_SUM "e610344d2cfd3e830cdd440bbad6eccd"

TEST_F(ColorTransformTest, YUV444ToRGB) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuvToRgb(m_pY, m_pU, m_pV, m_RGBBuffer, m_width, m_height, YUVToRGBConverter::SubSample_444);
    ASSERT_TRUE(CheckMD5Sum("b0ba0ee088ec6585d35cec04d15d681f"));
}

TEST_F(ColorTransformTest, YUV422ToRGB) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuvToRgb(m_pY, m_pU, m_pV, m_RGBBuffer, m_width, m_height, YUVToRGBConverter::SubSample_422);
    ASSERT_TRUE(CheckMD5Sum(BGR24_MD5_SUM));
}


TEST_F(ColorTransformTest, YUV420ToRGB) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuvToRgb(m_pY, m_pU, m_pV, m_RGBBuffer, m_width, m_height, YUVToRGBConverter::SubSample_420);
    ASSERT_TRUE(CheckMD5Sum("25cf91bba1064a10ba7307c0512491a5"));
}


TEST_F(ColorTransformTest, YUV422ToBGR24) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuv422ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_BGR);
    ASSERT_TRUE(CheckMD5Sum(BGR24_MD5_SUM));
}

TEST_F(ColorTransformTest, YUV444ToBGR24) {
    CreateYUVandRGBBuffers(200, 100);

    CopyUVEvenToUVOdd();

    m_YUVConverter.yuv444ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_BGR);
    ASSERT_TRUE(CheckMD5Sum(BGR24_MD5_SUM));
}

TEST_F(ColorTransformTest, YUV422ToRGB24) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuv422ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_RGB);
    ASSERT_TRUE(CheckMD5Sum(RGB24_MD5_SUM));
}

TEST_F(ColorTransformTest, YUV444ToRGB24) {
    CreateYUVandRGBBuffers(200, 100);

    CopyUVEvenToUVOdd();

    m_YUVConverter.yuv444ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_RGB);
    ASSERT_TRUE(CheckMD5Sum(RGB24_MD5_SUM));
}

TEST_F(ColorTransformTest, YUV422ToBGRX) {
    CreateYUVandRGBBuffers(200, 100);

    m_YUVConverter.yuv422ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_BGRX);
    ASSERT_TRUE(CheckMD5Sum(BGRX_MD5_SUM));
}

TEST_F(ColorTransformTest, YUV444ToBGRX) {
    CreateYUVandRGBBuffers(200, 100);

    CopyUVEvenToUVOdd();

    m_YUVConverter.yuv444ToRGBFormat(m_slicePlanes, m_sourceStride, m_width, m_height, m_RGBBuffer,
            m_width * 3, YUVToRGBConverter::ImageFormat_BGRX);
    ASSERT_TRUE(CheckMD5Sum(BGRX_MD5_SUM));
}


// Tests using real image  data
TEST(ColorTransformRealDataTests, YUVToRGB) {

    std::string strDataDir = GTestUtils::GetDataDir("..");

    strDataDir += "/video/yuv2rgb/";
    std::string strYUVFile = strDataDir + "authstation_yuv_1280_1024_422.dat";
    std::string strRGBFile = strDataDir + "authstation_rgb_1280_1024.dat";

    const size_t nWidth = 1280;
    const size_t nHeight = 1024;
    const size_t nYUVSize = nWidth * nHeight * 2;
    const size_t nRGBSize = nWidth * nHeight * 3;

    uint8_t * pYUV    = new uint8_t[nYUVSize];
    uint8_t * pRGB    = new uint8_t[nRGBSize];
    uint8_t * pRGBRef = new uint8_t[nRGBSize];

    FILE *fp = ::fopen(strYUVFile.c_str(), "rb");
    ASSERT_TRUE(fp != NULL);
    ::fread(pYUV, 1, nYUVSize, fp);
    ::fclose(fp);

    fp = ::fopen(strRGBFile.c_str(), "rb");
    ASSERT_TRUE(fp != NULL);
    ::fread(pRGBRef, 1, nRGBSize, fp);
    ::fclose(fp);

    uint8_t * pY = pYUV;
    uint8_t * pU = pYUV + (nWidth * nHeight);
    uint8_t * pV = pYUV + (nWidth * nHeight*3)/2;

    YUVToRGBConverter oRGB;
    oRGB.yuvToRgb(pY, pU, pV, pRGB, nWidth, nHeight, YUVToRGBConverter::SubSample_422);

    ASSERT_EQ(0, ::memcmp(pRGB, pRGBRef, nRGBSize));

    delete [] pYUV;
    delete [] pRGB;
    delete [] pRGBRef;
}
