/*
 * ffutil_test.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "FFMediaProcessor.hh"
#include "FFUtils.hh"
#include "URLFetcher.hh"

#ifdef CreateDirectory
#undef CreateDirectory
#endif

using namespace std;

typedef struct md5SumInfo {
    std::string m_strPath;
    std::string m_strMD5Sum;
} md5SumInfo_t;

class FFUtilsTest : public testing::Test {
protected:

    virtual void SetUp() {

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaMediaDir = "tmp_test_area/medianame/";
        GTestUtils::CreateDirectory(m_sTestAreaMediaDir);

        m_trackDir = "junk";
    }

    virtual void TearDown() {

        GTestUtils::DeleteDirectory(m_sTestAreaDir);
    }

    virtual void CreateFile(string filePath) {
        uint8_t buffer[8];
        GTestUtils::CreateFileFromBuffer(filePath, buffer, 8);
    }

protected:
    string m_sTestAreaDir;
    string m_sTestAreaMediaDir;
    string m_trackDir;
    bool m_result;
};

TEST_F(FFUtilsTest, NonExistingMedia) {
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaDir + "no_such_media", 1.0, m_trackDir);
    ASSERT_FALSE(m_result);
}

TEST_F(FFUtilsTest, NormalSpeed) {
#ifndef WINDOWS
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 1.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
#endif
}

TEST_F(FFUtilsTest, SlowSpeed) {
#ifndef WINDOWS
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 0.5, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
#endif
}

TEST_F(FFUtilsTest, FastSpeedWithoutAnyTracks) {
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
}

TEST_F(FFUtilsTest, SpeedForOneValidTrack) {
#ifndef WINDOWS
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_8");

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_8", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 10.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 1.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
#endif
}

TEST_F(FFUtilsTest, SpeedForTwoValidTracks) {
#ifndef WINDOWS
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_64");
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_8");

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_8", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 10.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_64", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 100.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 1.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
#endif
}

TEST_F(FFUtilsTest, SpeedForThreeValidTracks) {
#ifndef WINDOWS
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_128");
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_64");
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_8");

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_8", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 10.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_64", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 64.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_64", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 64.001, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_128", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 100.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("ff_128", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 200.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 1.0, m_trackDir);
    ASSERT_TRUE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
#endif
}

TEST_F(FFUtilsTest, TrackWithOutOfRangeSpeed) {
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_513");

    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());
}

TEST_F(FFUtilsTest, TrackWithInvalidFormat) {
    // Should not work with spurious trailing characters
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_8_");
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());

    // Should not work with leading zeros
    GTestUtils::CreateDirectory(m_sTestAreaMediaDir + "ff_08");
    m_result = CFFUtils::GetTrackDirForSpeed(m_sTestAreaMediaDir, 2.0, m_trackDir);
    ASSERT_FALSE(m_result);
    ASSERT_STREQ("", m_trackDir.c_str());

}

TEST_F(FFUtilsTest, IsDirectory) {
#ifndef WINDOWS
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc");
    m_result = CFFUtils::IsDirectory(m_sTestAreaDir + "abc");
    ASSERT_TRUE(m_result);

    m_result = CFFUtils::IsDirectory(m_sTestAreaDir + "xyz");
    ASSERT_FALSE(m_result);

    CreateFile(m_sTestAreaDir + "lmn");
    m_result = CFFUtils::IsDirectory(m_sTestAreaDir + "lmn");
    ASSERT_FALSE(m_result);
#endif
}

TEST_F(FFUtilsTest, DeleteDirectoryTree) {
#ifndef WINDOWS
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn/xyz");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn/xyz/pqr");
    CreateFile(m_sTestAreaDir + "abc/file1");
    CreateFile(m_sTestAreaDir + "abc/file2");
    CreateFile(m_sTestAreaDir + "abc/lmn/file1");
    CreateFile(m_sTestAreaDir + "abc/lmn/file2");
    CreateFile(m_sTestAreaDir + "abc/lmn/xyz/file1");
    CreateFile(m_sTestAreaDir + "abc/lmn/xyz/file2");
    CreateFile(m_sTestAreaDir + "abc/lmn/xyz/pqr/file1");
    CreateFile(m_sTestAreaDir + "abc/lmn/xyz/pqr/file2");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/dir1");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn/dir1");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn/xyz/dir1");
    GTestUtils::CreateDirectory(m_sTestAreaDir + "abc/lmn/xyz/pqr/dir1");

    m_result = CFFUtils::IsDirectory(m_sTestAreaDir + "abc");
    ASSERT_TRUE(m_result);

    m_result = CFFUtils::DeleteDirectoryTree(m_sTestAreaDir + "abc");
    ASSERT_TRUE(m_result);

    m_result = CFFUtils::IsDirectory(m_sTestAreaDir + "abc");
    ASSERT_FALSE(m_result);
#endif
}

TEST_F(FFUtilsTest, SpeedListParse) {

    bool status;
    std::vector<int> trackSpeeds;

    status = CFFUtils::ParseTrackList("", trackSpeeds);
    ASSERT_FALSE(status);

    status = CFFUtils::ParseTrackList("2", trackSpeeds);
    ASSERT_TRUE(status);
    ASSERT_EQ(1, (int)trackSpeeds.size());
    ASSERT_EQ(2, trackSpeeds[0]);

    status = CFFUtils::ParseTrackList("2,5", trackSpeeds);
    ASSERT_TRUE(status);
    ASSERT_EQ(2, (int)trackSpeeds.size());
    ASSERT_EQ(2, trackSpeeds[0]);
    ASSERT_EQ(5, trackSpeeds[1]);

    status = CFFUtils::ParseTrackList("2,5,10", trackSpeeds);
    ASSERT_TRUE(status);
    ASSERT_EQ(3, (int)trackSpeeds.size());
    ASSERT_EQ(2, trackSpeeds[0]);
    ASSERT_EQ(5, trackSpeeds[1]);
    ASSERT_EQ(10, trackSpeeds[2]);

    status = CFFUtils::ParseTrackList("2,5,10,", trackSpeeds);
    ASSERT_TRUE(status);
    ASSERT_EQ(3, (int)trackSpeeds.size());
    ASSERT_EQ(2, trackSpeeds[0]);
    ASSERT_EQ(5, trackSpeeds[1]);
    ASSERT_EQ(10, trackSpeeds[2]);

    status = CFFUtils::ParseTrackList("1", trackSpeeds);
    ASSERT_FALSE(status);

    status = CFFUtils::ParseTrackList("5,2", trackSpeeds);
    ASSERT_FALSE(status);

    status = CFFUtils::ParseTrackList("2,-", trackSpeeds);
    ASSERT_FALSE(status);

}

#if 0
TEST(URLTest, FileUploadTest) {

    CFileUploader *fetcher;
    bool rc;

    // this should upload successfully
    fetcher = new CFileUploader("http://10.51.48.170/mediaportal/thumbnail");
    rc = fetcher->UploadFile("somefile.jpg", "image.jpg");
    EXPECT_TRUE(rc);
    delete fetcher;

    // Fail, bad URL
    fetcher = new CFileUploader("");
    rc = fetcher->UploadFile("somefile.jpg", "image.jpg");
    EXPECT_FALSE(rc);
    delete fetcher;

    // Fail, server should return error
    fetcher = new CFileUploader("http://10.51.48.170/mediaportal/tthumbnail");
    rc = fetcher->UploadFile("somefile.jpg", "image.jpg");
    EXPECT_FALSE(rc);
    delete fetcher;
}
#endif
