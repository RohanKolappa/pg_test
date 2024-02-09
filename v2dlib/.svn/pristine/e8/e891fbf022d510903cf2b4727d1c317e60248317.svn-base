
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "comp_utils.h"
#include "bitmap.hh"
#include "sliceencoder.hh"
#include "test_utils.hh"
#include "memory_tracker.h"

//#define MAXRAWSLICELEN 10000
#define MAXENCODESLICELEN 10000

typedef struct SliceEncoderInfo {
    std::string m_strBMPFile;
    size_t m_nSliceSize;
    int m_nMQuant;
    int m_nDCQuant;
    std::string m_strOrigMD5;
    std::string m_strRealMD5;
} SliceEncoderInfo_t;



// The fixture for testing encodeOneSlice.
class CV2DSliceEncoderTest :
    public testing::TestWithParam<SliceEncoderInfo_t> {
protected:
    
    virtual void SetUp() {
        m_bSetupError = false;
        m_strError = "";
        m_pRawBuf = NULL;
        m_pEncodeBuf = new unsigned char[MAXENCODESLICELEN];
    }
    
    virtual void TearDown() {
        delete [] m_pRawBuf;
        delete [] m_pEncodeBuf;
    }

    virtual int ReadDataFile(std::string rawFile)
    {

        std::string strDataDir = GTestUtils::GetDataDir("..");
        std::string strRawFile = strDataDir + "/video/slices/" + rawFile;

        CBitmap oRawBmp(strRawFile.c_str());
        unsigned int nRawBmpSize =
            oRawBmp.GetWidth() * oRawBmp.GetHeight() * 4;

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

    size_t m_nRawBufLen;
};

static SliceEncoderInfo_t slices[] =
{
    {"debug_30.bmp", 30, 6, 6, "e3204d3681c2396a56eb80d705f5ef93", "e8bf53d5fe9ab9e928e5f60d55d79a86"},
    {"debug_30.bmp", 30, 4, 2, "4b0172723e3c723a52d3bdb9bc45f1dd", "3292d617479e866e8cd483531acdd749"},
    {"debug_30.bmp", 30, 2, 4, "f750218b1465a12b1ef6948e04dfb20e", "b4a768e7832a43b17879432256e8f6ec"},
    {"debug_30.bmp", 30, 1, 1, "d5b66ba4066885cf864f4f336e0adb77", "d9fa965aa59c5882380293ce933b3a62"},
    {"debug_32.bmp", 32, 6, 6, "ff6cbdacdeb7f4a23c0eca12eb920b1c", "57f922df13d1fdac43a3583f58c1a8c6"},
    {"debug_32.bmp", 32, 4, 2, "ff4b1bd1d41f84d9c6c8ab07f4026a64", "6e6a981b22f40d893ea8bcf5398570b2"},
    {"debug_32.bmp", 32, 2, 4, "aad0baa7a6785cea157cabd6d3dfaab6", "07f3ecd55ea15657af1102677607c1f7"},
    {"debug_32.bmp", 32, 1, 1, "a95bc9163f14627856daa1fd247c73b3", "2df77c46194389539786c63510321dbb"},
    {"random_5_32.bmp", 32, 6, 6, "74e6ed34ac0b108328be09989fbba5c8", "f24f927cdc824416d1bfd203fac9713e"},
    {"random_5_32.bmp", 32, 4, 2, "b88deb4356db1f1c5be7442a05ec2f97", "fac784d543e149a78b8f3a20d0061cad"},
    {"random_5_32.bmp", 32, 2, 4, "5abd0a0773f61c9191596eea818caccb", "4e48f1d874dc6ecdffc7016c978f2cc0"},
    {"random_5_32.bmp", 32, 1, 1, "61abdd61e7f9433ad4fbfbc9e8f71f5a", "7d19b6ddb3e5541e22ec0614ce4f9bb0"},
    {"random_10_32.bmp", 32, 6, 6, "d5b2750cb1cc08358d81ffdaee02fa76", "2a84fcc9653bea0918a7d339c3186460"},
    {"random_10_32.bmp", 32, 4, 2, "742af9f06dbfc366628a54140f85a628", "d7c31bab3e105789a363f8fea9db3732"},
    {"random_10_32.bmp", 32, 2, 4, "c86f10a06d23a00af44e0b99ed21144d", "eb787e5c952f5ef05557dd5f35a91df2"},
    {"random_10_32.bmp", 32, 1, 1, "da362bc1765573fe906ad4d49c007ad0", "3dde921c8cf1c0afbd461d2fdaf96d78"},
    {"random_15_32.bmp", 32, 6, 6, "53b20b8fa0e6aa23dbb25ba734fb96a6", "47efa6adedbf665097171c8f7415d345"},
    {"random_15_32.bmp", 32, 4, 2, "938436362493a2f2a36315df8f2813e2", "41b2ac4a184a6507aefef64b1ad27431"},
    {"random_15_32.bmp", 32, 2, 4, "103d7497b7ff11de83bc9c76ccc152eb", "6596cad76c4f9a47312f41a66e1e3b03"},
    {"random_15_32.bmp", 32, 1, 1, "91631f1bfc66181c3869df253b83fe75", "8d986cca18645521e13e808ec664e814"},
    {"random_20_32.bmp", 32, 6, 6, "1c32571fb8b1efe0a563dd8ab6e7dbc3", "ee30f1cf3016c6ed144b2a02cc4abdf3"},
    {"random_20_32.bmp", 32, 4, 2, "66d375bf48e809168855a7494d02b0d9", "fbd561165463d1a5f1a2e05bf4baabcd"},
    {"random_20_32.bmp", 32, 2, 4, "d880cce9987409eae56090f52117ca0a", "7ce8b60a3dceaf72a2a82d325277754a"},
    {"random_20_32.bmp", 32, 1, 1, "7ec5cc61520f6ee29c0f6787c75d8651", "32bfe6d1f732b21117fb834648354369"},

};


INSTANTIATE_TEST_CASE_P(EncodeSlice,
        CV2DSliceEncoderTest,
        ::testing::ValuesIn(slices));


TEST_P(CV2DSliceEncoderTest, EncodeSlice) {

    if (m_bSetupError) {
        FAIL() << m_strError.c_str();
        return;
    }

    SliceEncoderInfo_t info = GetParam();

    if (ReadDataFile(info.m_strBMPFile) < 0) {
        FAIL() << m_strError.c_str();
        return;
    }

    int mquant = info.m_nMQuant;
    int dcquant = info.m_nDCQuant;
    int nSliceSize = info.m_nSliceSize;

    int nHres = nSliceSize*8;
    int nVres = 8;

    // Test using the original comp_utils implementation
    initFrameParams(nHres, nVres, mquant, dcquant, nSliceSize);
    size_t nEncodeLen = encodeOneSlice(m_pRawBuf, (char *)m_pEncodeBuf);

    char md5sum[40], md5sum_real[40];
    strcpy(md5sum_real, "");
    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum);

    EXPECT_STREQ(md5sum, info.m_strOrigMD5.c_str());

    CSliceEncoder *sliceEncoder = CSliceEncoder::CreateSliceEncoder(mquant, dcquant, nSliceSize);
    sliceEncoder->SetTestable(true);
    ::memset(m_pEncodeBuf, 0x00, MAXENCODESLICELEN);
    nEncodeLen = sliceEncoder->EncodeSlice(m_pEncodeBuf, m_pRawBuf, nHres);
    GTestUtils::MD5Sum(m_pEncodeBuf, nEncodeLen, md5sum_real);

    EXPECT_STREQ(md5sum_real, info.m_strRealMD5.c_str());

    // To recreate checksums of samples, un-comment following lines
    // and run test. Then cut-paste the generated printf's
    // into above slices[] array

//        printf("{\"%s\", %d, %d, %d, \"%s\", \"%s\"},\n",
//                info.m_strBMPFile.c_str(),
//                info.m_nSliceSize, info.m_nMQuant, info.m_nDCQuant,
//                md5sum, md5sum_real);
    delete sliceEncoder;
}

