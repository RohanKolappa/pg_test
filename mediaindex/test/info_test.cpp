/*
 * info_test.cpp
 *
 *  Created on: Jan 22, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "libmediaconverter.hh"
#include "MatroskaFileInfo.hh"
#include "EditList.hh"

using namespace mediaindex;
using namespace std;

class InfoTest :
public testing::Test {
protected:

    virtual void SetUp() {
        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/matroska/");

        m_pFileInfo = NULL;
    }

    virtual void TearDown() {
        delete m_pFileInfo;
    }

protected:
    char m_md5sum[40];
    string m_sDataDir;
    CMatroskaFileInfo * m_pFileInfo;
};


TEST_F(InfoTest, GetInfoBadFile) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "blah.mkd", true);
    bool rc = m_pFileInfo->PrintInfo("all");
    EXPECT_FALSE(rc);
}

TEST_F(InfoTest, GetMetaDataInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    m_pFileInfo->PrintInfo("metadata");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("14cfc2833e50b030a82b5927b66c1e92", m_md5sum);
}

TEST_F(InfoTest, GetBlobDbInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    m_pFileInfo->PrintInfo("blobdb");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("0ba5d5d313c1e3f869ba5797cdb2dcf8", m_md5sum);
}

TEST_F(InfoTest, GetMSFRLInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    m_pFileInfo->PrintInfo("msfrl");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("2f3d1e56f2274e2d146b0e54d0094b14", m_md5sum);
}

TEST_F(InfoTest, GetBookmarksInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    m_pFileInfo->PrintInfo("bookmarks");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("8cc7aaef09f00f39ff740a1e08284837", m_md5sum);
}

TEST_F(InfoTest, GetBookmarksInfoWithEditList) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");
    m_pFileInfo->SetEditList(editList);

    m_pFileInfo->PrintInfo("bookmarks");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("3c156cd487d6c53ce2ea93fa68417fee", m_md5sum);

    delete editList;

}

TEST_F(InfoTest, GetSizeInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    m_pFileInfo->PrintInfo("size");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("2f994b2fc8e8462bf47410ecead6be3b", m_md5sum);
}

TEST_F(InfoTest, GetSizeInfoFromMinuteFile) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip2_blob", true);
    m_pFileInfo->PrintInfo("size");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("860d5b7f1e2c119ce2f3fc5f19b6f97d", m_md5sum);
}

TEST_F(InfoTest, GetGapInfoFromMinuteFile) {
    string sDataDir = GTestUtils::GetDataDir("..");
    sDataDir.append("/video/clips/v2d_media/");

    m_pFileInfo = new CMatroskaFileInfo(sDataDir + "v2d_splash_gaps", true);

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,150000:160000,190000:210000");
    m_pFileInfo->SetEditList(editList);

    m_pFileInfo->PrintInfo("gaps");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("eb353842460da8564471300d9c880c51", m_md5sum);

    delete editList;
}

TEST_F(InfoTest, GetGapInfoFromMatroskaFile) {
    string sDataDir = GTestUtils::GetDataDir("..");
    sDataDir.append("/video/clips/v2d_media/");

    m_pFileInfo = new CMatroskaFileInfo(sDataDir + "v2d_splash_gaps.mkd", true);

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,150000:160000,190000:210000");
    m_pFileInfo->SetEditList(editList);

    m_pFileInfo->PrintInfo("gaps");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("eb353842460da8564471300d9c880c51", m_md5sum);

    delete editList;
}

TEST_F(InfoTest, GetSizeInfoWithEditList) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");
    m_pFileInfo->SetEditList(editList);

    m_pFileInfo->PrintInfo("size");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("a09f508733a7823efeab21c4060d43f6", m_md5sum);

    delete editList;
}

TEST_F(InfoTest, GetSizeInfoWithEditListFromMinuteFile) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip2_blob", true);

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,190000:210000");
    m_pFileInfo->SetEditList(editList);

    m_pFileInfo->PrintInfo("size");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("10aefd0028d0c1decd6dba7871dd817b", m_md5sum);

    delete editList;
}

TEST_F(InfoTest, GetTimeCodeAtOffset) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);

    m_pFileInfo->PrintInfo("gettimecodeatoffset", "1000");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("2a54f32065e797326a2eab503c7b8245", m_md5sum);
}

TEST_F(InfoTest, GetOffsetAtTimeCode) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);

    m_pFileInfo->PrintInfo("getoffsetattimecode", "1280362628710");
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("6c9aebe1a0d285799f4ba4e480477aac", m_md5sum);
}

TEST_F(InfoTest, GetAllInfo) {
    m_pFileInfo = new CMatroskaFileInfo(m_sDataDir + "clip1.mkd", true);
    bool rc = m_pFileInfo->PrintInfo("all");
    EXPECT_TRUE(rc);
    string resultString = m_pFileInfo->GetResultString();
    GTestUtils::MD5Sum((unsigned char *)resultString.c_str(), resultString.length(), m_md5sum);
    EXPECT_STREQ("47bb0c4e1d38c036d3dab5138cce035b", m_md5sum);
}
