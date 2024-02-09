
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include <sstream>
#include "slicedecoder.hh"
#include "codecdefs.h"
#include "colortransform.h"
#include "imagemanip.h"
#include "bitmap.hh"
#include "test_utils.hh"
#include "memory_tracker.h"

using namespace V2DLib;

const size_t NMAXENCODELEN = 20000;
const size_t NMAXDECODELEN = V2D_MAX_SLICE_SIZE * V2D_BLOCK_SIZE * 3;

#define DEBUGSLICESIZE 30
#define RANDOMSLICESIZE 32

class CSliceDecoderTest : public testing::TestWithParam<const char*> {
protected:
    
    virtual void SetUp() {
        m_strDataDir = GTestUtils::GetDataDir("..");
        m_strDataDir += "/video/slices/";

        m_pEncodeBuf = new unsigned char[NMAXENCODELEN];
        memset(m_pEncodeBuf, 0x00, NMAXENCODELEN);
        m_nSliceLen = 0;
        m_nSliceNumber = -1;
        m_pSliceDecoder = NULL;
        SetupDecodeBuffer();
    }
    
    virtual void TearDown() {
        delete [] m_pDecodeBuf;
        delete [] m_pEncodeBuf;
        delete m_pSliceDecoder;
    }

    virtual void CreateSliceDecoder() {
        m_pSliceDecoder = new CSliceDecoder();
    }

    virtual void SetupDecodeBuffer() {
        m_pDecodeBuf = new unsigned char[NMAXDECODELEN];
        memset(m_pDecodeBuf, 0x00, NMAXDECODELEN);
        m_pY = m_pDecodeBuf;
        m_pU = m_pDecodeBuf + (V2D_MAX_SLICE_SIZE * V2D_BLOCK_SIZE);
        m_pV = m_pDecodeBuf + (V2D_MAX_SLICE_SIZE * V2D_BLOCK_SIZE * 2);
    }

    virtual void ReadEncodedData(std::string fileName) {
        std::string strEncFile = m_strDataDir + fileName;
        FILE *fp = ::fopen(strEncFile.c_str(), "rb");
        ASSERT_TRUE(fp != NULL) << "Could not open file " << strEncFile;
        int nSliceLen = ::fread(m_pEncodeBuf, 1, NMAXENCODELEN, fp);
        m_nSliceLen = nSliceLen;
        ::fclose(fp);
    }

    virtual void ComputeMD5Sum() {
        GTestUtils::MD5Sum(m_pDecodeBuf, NMAXDECODELEN, md5sum_real);
    }

protected:
    std::string m_strDataDir;
    CSliceDecoder * m_pSliceDecoder;
    unsigned char* m_pEncodeBuf;
    int m_nSliceLen;
    int m_nSliceNumber;
    unsigned char* m_pDecodeBuf;    
    unsigned char *m_pY;
    unsigned char *m_pU;
    unsigned char *m_pV;
    char md5sum_real[40];
};


#define MD5ALLZEROS  "f4f35d60b3cc18aaa6d8d92f0cd3708a"

TEST_F(CSliceDecoderTest, ZeroSliceLengthFailsToDecode) {

    ReadEncodedData("enc.dat");
    CreateSliceDecoder();
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, 0, DEBUGSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_FALSE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ(MD5ALLZEROS, md5sum_real);
}

TEST_F(CSliceDecoderTest, SliceLength5FailsToDecode) {

    ReadEncodedData("enc.dat");
    CreateSliceDecoder();
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, 5, DEBUGSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_FALSE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ(MD5ALLZEROS, md5sum_real);
}


TEST_F(CSliceDecoderTest, ZeroedOutSliceFailsToDecode) {

    ReadEncodedData("enc.dat");
    CreateSliceDecoder();

    memset(m_pEncodeBuf, 0x00, 256);
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, m_nSliceLen, DEBUGSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_FALSE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ(MD5ALLZEROS, md5sum_real);
}

TEST_F(CSliceDecoderTest, ZeroedOutBitstreamDoesNotDecode) {

    ReadEncodedData("random_5_32_2_4.dat");
    CreateSliceDecoder();

    // Wipe out the bitstream after initial header
    // We expect decode to return true but the yuv to remain untouched
    memset(m_pEncodeBuf + 6, 0x00, m_nSliceLen);
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, m_nSliceLen, DEBUGSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_TRUE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ(MD5ALLZEROS, md5sum_real);
}

TEST_F(CSliceDecoderTest, PartialZeroedOutBitstreamDecodes) {

    ReadEncodedData("random_5_32_2_4.dat");
    CreateSliceDecoder();

    // Wipe out the bitstream after 500 bytes
    memset(m_pEncodeBuf + 500, 0x00, m_nSliceLen);
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, m_nSliceLen, RANDOMSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_TRUE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ("711ed537f33392655affb0a38feb5691", md5sum_real);
}

TEST_F(CSliceDecoderTest, TruncatedBitstreamDecodes) {

    ReadEncodedData("random_5_32_2_4.dat");
    CreateSliceDecoder();

    // Ask decoder to deal with only first 500 bytes
    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, 500, RANDOMSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_FALSE(bRet);

    ComputeMD5Sum();
    EXPECT_STREQ("7142c608685cb98a0fe207b9c45ac0cb", md5sum_real);
}

TEST_F(CSliceDecoderTest, DecodeDebugSlice) {

    ReadEncodedData("enc.dat");
    CreateSliceDecoder();

    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, m_nSliceLen, DEBUGSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_TRUE(bRet);
    EXPECT_EQ(1, m_nSliceNumber);

    ComputeMD5Sum();
    EXPECT_STREQ("6f5d3ca4c377f14d748c01f8b31c9596", md5sum_real);
}

TEST_F(CSliceDecoderTest, DecodeRandomSlice) {

    ReadEncodedData("random_5_32_2_4.dat");
    CreateSliceDecoder();

    bool bRet = m_pSliceDecoder->Decode(m_pEncodeBuf, m_nSliceLen, RANDOMSLICESIZE,
            m_pY, m_pU, m_pV, m_nSliceNumber);
    EXPECT_TRUE(bRet);
    EXPECT_EQ(1, m_nSliceNumber);

    ComputeMD5Sum();
    EXPECT_STREQ("f87e78d2f4457b486c56fc36247e4235", md5sum_real);
}


TEST_F(CSliceDecoderTest, DecodeRandomSliceStress) {

    ReadEncodedData("random_5_32_2_4.dat");
    CreateSliceDecoder();

    // Decoding truncated slices of various lengths
    // and ensure decoder does not crash.
    // Exactly 8 bytes of extra data is allocated and
    // fill with zeros. Verified with valgrind.
    for (int i = 10; i < m_nSliceLen; i += 7) {
        uint8_t * buffer = new uint8_t[i + 8];
        memcpy(buffer, m_pEncodeBuf, i);
        memset(buffer + i, 0x00, 8);
        m_pSliceDecoder->Decode(buffer, i, RANDOMSLICESIZE,
                m_pY, m_pU, m_pV, m_nSliceNumber);
        delete [] buffer;
    }
}
