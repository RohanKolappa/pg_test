
#include "v2dlib_compatibility.h"
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include <sstream>
#include <fstream>
#include "vframedecoder.hh"
#include "codecdefs.h"
#include "colortransform.h"
#include "imagemanip.h"
#include "bitmap.hh"
#include "test_utils.hh"
#include "memory_tracker.h"

using GTestUtils::MD5SUMLEN;

class CVFrameDecoderTest : public testing::Test {
protected:
    
    virtual void SetUp() {

        m_pEncodeBuf = NULL;
        m_pDecodeBuf = NULL;

        m_strDataDir = GTestUtils::GetDataDir("..");

        m_strDataDir += "/video/frames2/";
        m_pFrameDecoder = NULL;
    }

    void ReadEncodedData(const std::string encodedFile) {

        std::string strEncFile = m_strDataDir + encodedFile;
        int nEncodedFileSize   = GTestUtils::GetFileSize(strEncFile);

        m_pEncodeBuf = new unsigned char[nEncodedFileSize + 8];

        FILE *fp = ::fopen(strEncFile.c_str(), "rb");
        ASSERT_TRUE(fp != NULL);
        m_nEncLen = ::fread(m_pEncodeBuf, 1, nEncodedFileSize, fp);
        ::fclose(fp);
    }

    void CreateDecoder(int width, int height, int sliceSize,
            enum ePixelFormat decodeFormat = eYUV422,
            enum eEncodeFormat encodeFormat = eYUV422P) {

        // Create Decoder
        v2dDecConfiguration decConfig;
        decConfig.m_nWidth = width;
        decConfig.m_nHeight = height;
        decConfig.m_nSliceSize = sliceSize;
        decConfig.m_eEncodeFormat = encodeFormat;
        decConfig.m_eDecodeFormat = decodeFormat;

        m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);

        m_nDecLen = decConfig.m_nDecodeBufLen;

        m_pDecodeBuf = new unsigned char[m_nDecLen];
    }

    void PrintMD5Sum() {
        char md5sum[MD5SUMLEN];
        GTestUtils::MD5Sum(m_pDecodeBuf, m_nDecLen, md5sum);
        printf("md5 = %s\n", md5sum);
    }

    bool CheckMD5Sum(std::string ref_md5sum) {
        char md5sum[MD5SUMLEN];
        GTestUtils::MD5Sum(m_pDecodeBuf, m_nDecLen, md5sum);
        return (strcmp(ref_md5sum.c_str(), md5sum) == 0);
    }

    virtual void TearDown() {
        delete [] m_pDecodeBuf;
        delete [] m_pEncodeBuf;
        delete m_pFrameDecoder;
    }

protected:

    std::string m_strDataDir;
    CVideoFrameDecoder *m_pFrameDecoder;

    unsigned char* m_pDecodeBuf;    
    unsigned char* m_pEncodeBuf;

    size_t m_nEncLen;
    size_t m_nDecLen;
};

TEST_F(CVFrameDecoderTest, DecodeEMRSS) {
    ReadEncodedData("emrss_enc_1920x1200_30_422.dat");
    CreateDecoder(1920, 1200, 30, eYUV422);

    int rc =  m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);
    EXPECT_EQ(0, rc);
    EXPECT_TRUE(CheckMD5Sum("cf012f51b9189dc833f0ea2f7575a6df"));
}

TEST_F(CVFrameDecoderTest, DecodeEAB) {
    ReadEncodedData("eab_enc_1280x720_32_422.dat");
    CreateDecoder(1280, 720, 32, eYUV422);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);

    EXPECT_TRUE(CheckMD5Sum("471e37d48beeb52d7c292dc5c691fd36"));
}

TEST_F(CVFrameDecoderTest, DecodeEAB_RGB) {
    ReadEncodedData("eab_enc_1280x720_32_422.dat");
    CreateDecoder(1280, 720, 32, eBGR24);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);

    EXPECT_TRUE(CheckMD5Sum("807a0ebd369f699e4427e48103a46cd2"));
}

TEST_F(CVFrameDecoderTest, DecodeAUTHSTATION) {
    ReadEncodedData("authstation_enc_1280x1024_32_422.dat");
    CreateDecoder(1280, 1024, 32, eYUV422);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);

    EXPECT_TRUE(CheckMD5Sum("5e1d84515f2cb44b1d3fe6d842b5ce31"));
}

TEST_F(CVFrameDecoderTest, DecodeFIGHTERPILOT) {
    ReadEncodedData("fp_enc_1600x1200_30_422.dat");
    CreateDecoder(1600, 1200, 30, eYUV422);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);

    EXPECT_TRUE(CheckMD5Sum("fbe65dbba58444227d1edaef443d1310"));
}

TEST_F(CVFrameDecoderTest, DecodeFIGHTERPILOT_RGB) {
    ReadEncodedData("fp_enc_1600x1200_30_422.dat");
    CreateDecoder(1600, 1200, 30, eRGB24);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);
    EXPECT_TRUE(CheckMD5Sum("bc6ea852dd17fc2ceaef086046736f7c"));
}

TEST_F(CVFrameDecoderTest, DecodeFIGHTERPILOT_BGR) {
    ReadEncodedData("fp_enc_1600x1200_30_422.dat");
    CreateDecoder(1600, 1200, 30, eBGR24);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);

    EXPECT_TRUE(CheckMD5Sum("bff2414fd5a3fdd56a29b73d72b4dbbe"));
}

TEST_F(CVFrameDecoderTest, DecodeAUTHSTATION_444) {
    ReadEncodedData("authstation_enc_1280x1024_32_422.dat");
    CreateDecoder(1280, 1024, 32, eBGR24, eYUV444S);

    m_pFrameDecoder->Decode(m_pEncodeBuf, m_nEncLen, m_pDecodeBuf, m_nDecLen);
    EXPECT_TRUE(CheckMD5Sum("4602417e4a19fca6d3abf37ef57209c7"));
}
