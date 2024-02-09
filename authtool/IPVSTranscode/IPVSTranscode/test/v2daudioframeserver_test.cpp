/*
 * v2daudioframeserver_test.cpp
 *
 *  Created on: Feb 7, 2012
 *      Author: rkale
 */


#include "IPVSTranscode_compatibility.h"

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "V2DAudioFrameServer.hh"

using namespace std;

class CV2DAudioFrameServerTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_strDataDir = GTestUtils::GetDataDir("../../..");
        ASSERT_NE(m_strDataDir,  "");
        m_strDataDir.append("/video/clips/");

        m_pAudioFrameServer = NULL;
        m_pAudioBuffer = new uint8_t[1024 * 32];
    }

    virtual void TearDown() {
        delete m_pAudioFrameServer;
        delete [] m_pAudioBuffer;
    }

    virtual void CreateFrameServer(string a_path)
    {
        string path = m_strDataDir + a_path;
        m_pAudioFrameServer = new CV2DAudioFrameServer(path);
    }

    virtual void CreateInitializedFrameServer(string a_path)
    {
        CreateFrameServer(a_path);
        m_pAudioFrameServer->Initialize();
    }

    virtual void ComputeMd5Sum(int size) {
        GTestUtils::MD5Sum(m_frameData, size, m_md5sum);
    }

    virtual void PrintMd5Sum() {
        printf("md5sum = %s\n", m_md5sum);
    }

    virtual void VerifyMd5Sum(const char * md5Str) {
        EXPECT_STREQ(md5Str, m_md5sum);
    }

protected:
    string m_strDataDir;
    CV2DAudioFrameServer * m_pAudioFrameServer;
    unsigned char * m_frameData;
    uint8_t *m_pAudioBuffer;
    char m_md5sum[40];
    int m_frameNo;

};


TEST_F(CV2DAudioFrameServerTest, CreateNonExistant) {

    CreateFrameServer("v2d_XXX");
    int retVal = m_pAudioFrameServer->Initialize();
    EXPECT_EQ(FS_MEDIASERVER_INIT_FAILED, retVal);
}

TEST_F(CV2DAudioFrameServerTest, NonV2DMedia) {

    CreateFrameServer("udp_media/udp_media_gaps");
    int retVal = m_pAudioFrameServer->Initialize();
    EXPECT_EQ(FS_STREAMTYPE_NOTSUPPORTED, retVal);
}

TEST_F(CV2DAudioFrameServerTest, Initialize) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    int retVal = m_pAudioFrameServer->Initialize();
    EXPECT_EQ(FS_SUCCESS, retVal);
}

TEST_F(CV2DAudioFrameServerTest, DoubleInitializationShouldFail) {
    CreateInitializedFrameServer("v2d_media/v2d_splash_gaps");
    int retVal = m_pAudioFrameServer->Initialize();
    EXPECT_EQ(FS_ALREADY_INITIALIZED, retVal);
}


TEST_F(CV2DAudioFrameServerTest, AudioUnavailable) {
    CreateInitializedFrameServer("v2d_media/v2d_splash_gaps");
    bool hasAudio = m_pAudioFrameServer->HasAudio();
    EXPECT_FALSE(hasAudio);
}

TEST_F(CV2DAudioFrameServerTest, AudioAvailable) {
    CreateInitializedFrameServer("v2d_media/v2d_with_audio");
    bool hasAudio = m_pAudioFrameServer->HasAudio();
    EXPECT_TRUE(hasAudio);
}


TEST_F(CV2DAudioFrameServerTest, AudioFrameCount) {
    CreateInitializedFrameServer("v2d_media/v2d_with_audio");
    EXPECT_EQ(5064880ULL, m_pAudioFrameServer->GetSampleCount());
}

TEST_F(CV2DAudioFrameServerTest, AudioFrameCountWithoutAudio) {
    CreateInitializedFrameServer("v2d_media/v2d_splash_gaps");
    EXPECT_EQ(0ULL, m_pAudioFrameServer->GetSampleCount());
}

TEST_F(CV2DAudioFrameServerTest, AudioParameters) {
    CreateInitializedFrameServer("v2d_media/v2d_with_audio");
    EXPECT_EQ(16000, m_pAudioFrameServer->GetFrequency());
    EXPECT_EQ(2, m_pAudioFrameServer->GetChannels());
    EXPECT_EQ(16, m_pAudioFrameServer->GetBitsPerSample());
}


TEST_F(CV2DAudioFrameServerTest, GetSamplesWithDifferentSizes) {
    CreateInitializedFrameServer("v2d_media/v2d_with_audio");

    int step_array[] = { 5, 10, 25, 50, 100, 200, 250, 500, 1000, 2000, 4000, 8000};
    for (int index = 0; index < 12; index++) {
        int start = 1600;
        int size = 8000;
        int step = step_array[index];
        for (int i = start; i < start + size; i += step) {
            m_pAudioFrameServer->GetSamples(m_pAudioBuffer, i, step);
            GTestUtils::MD5SumUpdate(m_pAudioBuffer, step * 4);
        }
        GTestUtils::MD5SumResult(m_md5sum);
        EXPECT_STREQ("151b81848615a1a3ab3599af3473ff4d", m_md5sum);
    }
}

TEST_F(CV2DAudioFrameServerTest, GetAllSamples) {
    CreateInitializedFrameServer("v2d_media/v2d_with_audio");
    m_pAudioFrameServer->SetFastResample();

    int start = 0;
    int step = 200;
    while (true) {
        if (!m_pAudioFrameServer->GetSamples(m_pAudioBuffer, start, step))
            break;
        start += step;
        if (start > 6000000)
            break;
    }
    EXPECT_EQ(5064600, start);
}

#if 1
TEST_F(CV2DAudioFrameServerTest, GetAllSamplesToo) {
    std::string media = "bloblink";
    m_pAudioFrameServer = new CV2DAudioFrameServer(media);
    m_pAudioFrameServer->Initialize();
    m_pAudioFrameServer->SetFastResample();

    int start = 0;
    int step = 200;
    while (true) {
        if (!m_pAudioFrameServer->GetSamples(m_pAudioBuffer, start, step))
            break;
        start += step;
        if (start > 60000000)
            break;
    }
    //printf("Processed %d samples\n", start);
}
#endif

