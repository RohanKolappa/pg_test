
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "bitmap.hh"
#include "test_utils.hh"

#define BLUEMASK  0x00FF0000
#define GREENMASK 0x0000FF00
#define REDMASK   0x000000FF
#define ALPHAMASK 0xFF000000

class CBitmapTest : public ::testing::TestWithParam<const char*> {
};


INSTANTIATE_TEST_CASE_P(LoadSave,
                        CBitmapTest,
                        ::testing::Values(
                        "installshieldwizard", "fuelinjector"));

TEST_P(CBitmapTest, LoadSave) {

    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strCurrFileName = GetParam();
    std::string strBitmapDir = strDataDir + "/video/bitmaps/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += strCurrFileName;
    strSrcFile += ".bmp";

    std::string strDstFile = strBitmapDir;
    strDstFile += strCurrFileName;
    strDstFile += "-copy.bmp";

    CBitmap oSrcBmp;
    EXPECT_EQ(0, oSrcBmp.Load(strSrcFile.c_str()) );

    unsigned char* pPixelData = static_cast<unsigned char *>
        ( oSrcBmp.GetBits() );
    ASSERT_TRUE( pPixelData );

    CBitmap oSaveBmp;
    oSaveBmp.SetBits(pPixelData, oSrcBmp.GetWidth(), oSrcBmp.GetHeight(),
        REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);

    EXPECT_TRUE( oSaveBmp.Save(strDstFile.c_str(), oSrcBmp.GetBitCount()) );

    CBitmap oCopyBmp(strDstFile.c_str());

    unsigned char* pCopyData =
        static_cast<unsigned char*>( oCopyBmp.GetBits() );
    ASSERT_TRUE( pCopyData );

    EXPECT_EQ(0, ::memcmp(pPixelData, pCopyData, (oSrcBmp.GetHeight()*
            oSrcBmp.GetWidth()*oSrcBmp.GetBitCount())/8) );

}

TEST(CBitmapTest, BGRATopRowFirst) {

    std::string strOutFile = "stripes.bmp";

    const unsigned int nWidth = 640, nHeight = 480;
    unsigned nSize = nWidth*nHeight*4;

    unsigned char* pBGRA = new unsigned char[nSize];
    unsigned char* pBGRA2 = new unsigned char[nSize];

    if(!pBGRA || !pBGRA2) {
        FAIL() << "Unable to proceed with test, out of memory";
        return;
    }

    for(unsigned int n = 0; n < nSize; n += 4)
    {
        if(n < (nWidth*nHeight))
        {
            pBGRA[n] = 255;
            pBGRA[n+1] = 0;
            pBGRA[n+2] = 0;
            pBGRA[n+3] = 0;
        }
        else if(n < nWidth*nHeight*2)
        {
            pBGRA[n] = 0;
            pBGRA[n+1] = 255;
            pBGRA[n+2] = 0;
            pBGRA[n+3] = 0;
        }
        else if(n < nWidth*nHeight*3)
        {
            pBGRA[n] = 0;
            pBGRA[n+1] = 0;
            pBGRA[n+2] = 255;
            pBGRA[n+3] = 0;
        }
        else
        {
            pBGRA[n] = 255;
            pBGRA[n+1] = 255;
            pBGRA[n+2] = 255;
            pBGRA[n+3] = 0;
        }

    }

    CBitmap obmp;
    obmp.SetBitsBGRA(pBGRA, nWidth, nHeight);
    ASSERT_TRUE( obmp.Save(strOutFile.c_str(), 32) );
    ASSERT_TRUE( obmp.GetBitsBGRA(pBGRA2, nSize) );

    ASSERT_EQ( 0, ::memcmp(pBGRA, pBGRA2, nSize) );

    delete [] pBGRA;
    delete [] pBGRA2;

    // Comment out the below code if test case fails and you want to debug
    // the bitmap image
    ::remove(strOutFile.c_str());

}

TEST_P(CBitmapTest, BitmapBufferToRaw) {

    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strCurrFileName = "fuelinjector";
    std::string strBitmapDir = strDataDir + "/video/bitmaps/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += strCurrFileName;
    strSrcFile += ".bmp";

    std::string strDstFile = strBitmapDir;
    strDstFile += strCurrFileName;
    strDstFile += "-copy.bmp";

    FILE *pFile = fopen (strSrcFile.c_str(), "rb");
    if (pFile == NULL)
    {
    	FAIL() << "Failed in opening source file";
        return;
    }

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);

    unsigned char *pBuffer = (unsigned char*)malloc (sizeof(unsigned char)*lSize);
    if (pBuffer == NULL)
    {
        FAIL() << "Failed in creating buffer";
        return;
    }

    size_t nRead = fread (pBuffer, 1 ,lSize, pFile);
    if ((size_t)lSize != nRead)
    {
        FAIL() << "Failed in reading from the file";
        return;
    }

    fclose(pFile);

    CBitmap oSrcBmp;
    ASSERT_TRUE( oSrcBmp.LoadFromBuffer(pBuffer, nRead) == 0);

    unsigned char* pPixelData = static_cast<unsigned char *>
        ( oSrcBmp.GetBits() );
    ASSERT_TRUE( pPixelData );

    CBitmap oSaveBmp;
    oSaveBmp.SetBits(pPixelData, oSrcBmp.GetWidth(), oSrcBmp.GetHeight(),
        REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);

    EXPECT_TRUE( oSaveBmp.Save(strDstFile.c_str(), oSrcBmp.GetBitCount()) );

    CBitmap oCopyBmp(strDstFile.c_str());

    unsigned char* pCopyData =
        static_cast<unsigned char*>( oCopyBmp.GetBits() );
    ASSERT_TRUE( pCopyData );

    free(pBuffer);
    EXPECT_EQ(0, ::memcmp(pPixelData, pCopyData, (oSrcBmp.GetHeight()*
            oSrcBmp.GetWidth()*oSrcBmp.GetBitCount())/8) );

}

TEST_P(CBitmapTest, LoadNonExistedBitmapFile) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot1_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += "NonExistedBmp";
    strSrcFile += ".bmp";

    CBitmap oSrcBmp;
    EXPECT_EQ(1, oSrcBmp.Load(strSrcFile.c_str()));
}

TEST_P(CBitmapTest, LoadBitmapWithNullData) {
    unsigned char *pBuffer = NULL;
    CBitmap oSrcBmp;
    EXPECT_EQ(4, oSrcBmp.LoadFromBuffer(pBuffer, 100));
    delete pBuffer;
}

TEST_P(CBitmapTest, LoadBitmapWithCorruptedHeader) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot1_blob/";

    std::string strMalformedBmp = strBitmapDir;
    strMalformedBmp += "malformed.bmp";

    unsigned char buffer[6];
    memset(buffer, 0xab, 6);
    GTestUtils::CreateFileFromBuffer(strMalformedBmp, buffer, 6);

    CBitmap oSrcBmp;
    EXPECT_EQ(5, oSrcBmp.Load(strMalformedBmp.c_str()));
}

TEST_P(CBitmapTest, LoadBitmapZeroLength) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot1_blob/";

    std::string strMalformedBmp = strBitmapDir;
    strMalformedBmp += "malformed.bmp";

    unsigned char buffer[256];
    memset(buffer, 0xab, 256);
    GTestUtils::CreateFileFromBuffer(strMalformedBmp, buffer, 0);

    CBitmap oSrcBmp;
    EXPECT_EQ(5, oSrcBmp.Load(strMalformedBmp.c_str()));
}

TEST_P(CBitmapTest, LoadNonBitmap) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot1_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += "snapshot_0";
    strSrcFile += ".jpg";

    CBitmap oSrcBmp;
    EXPECT_EQ(6, oSrcBmp.Load(strSrcFile.c_str()));
}

TEST_P(CBitmapTest, LoadBitmapWithWrongOffset) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot4_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += "snapshot_0";
    strSrcFile += ".bmp";

    std::string strMalformedBmp = strBitmapDir;
    strMalformedBmp += "malformed.bmp";

    GTestUtils::CreateTruncatedFile(strSrcFile, strMalformedBmp, 122);

    CBitmap oSrcBmp;
    EXPECT_EQ(8, oSrcBmp.Load(strMalformedBmp.c_str()));
}

TEST_P(CBitmapTest, LoadBitmapBufferWithTruncatedData) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot1_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += "snapshot_1";
    strSrcFile += ".bmp";

    std::string strMalformedBmp = strBitmapDir;
    strMalformedBmp += "malformed.bmp";

    GTestUtils::CreateTruncatedFile(strSrcFile, strMalformedBmp, 2000);

    CBitmap oSrcBmp;
    EXPECT_EQ(9, oSrcBmp.Load(strMalformedBmp.c_str()));
}

TEST_P(CBitmapTest, LoadSaveBitfield3Bitmap) {

    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strCurrFileName = "snapshot_0";
    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot4_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += strCurrFileName;
    strSrcFile += ".bmp";

    std::string strDstFile = strBitmapDir;
    strDstFile += strCurrFileName;
    strDstFile += "-copy.bmp";

    CBitmap oSrcBmp;
    EXPECT_EQ(0, oSrcBmp.Load(strSrcFile.c_str()) );

    unsigned char* pPixelData = static_cast<unsigned char *>
        ( oSrcBmp.GetBits() );
    ASSERT_TRUE( pPixelData );

    CBitmap oSaveBmp;
    oSaveBmp.SetBits(pPixelData, oSrcBmp.GetWidth(), oSrcBmp.GetHeight(),
        REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);

    EXPECT_TRUE( oSaveBmp.Save(strDstFile.c_str(), oSrcBmp.GetBitCount()) );

    CBitmap oCopyBmp(strDstFile.c_str());

    unsigned char* pCopyData =
        static_cast<unsigned char*>( oCopyBmp.GetBits() );
    ASSERT_TRUE( pCopyData );

    EXPECT_EQ(0, ::memcmp(pPixelData, pCopyData, (oSrcBmp.GetHeight()*
            oSrcBmp.GetWidth()*oSrcBmp.GetBitCount())/8) );
}

TEST_P(CBitmapTest, LoadSaveBitfield3BitmapWithTruncatedData) {
    std::string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "") {
        FAIL() << "Data directory not found, have you set GTESTDATADIR ?";
        return;
    }

    std::string strBitmapDir = strDataDir + "/video/matroska/snapshot4_blob/";

    std::string strSrcFile = strBitmapDir;
    strSrcFile += "snapshot_0";
    strSrcFile += ".bmp";

    std::string strMalformedBmp = strBitmapDir;
    strMalformedBmp += "malformed.bmp";

    GTestUtils::CreateTruncatedFile(strSrcFile, strMalformedBmp, 2000);

    CBitmap oSrcBmp;
    EXPECT_EQ(9, oSrcBmp.Load(strMalformedBmp.c_str()));
}
