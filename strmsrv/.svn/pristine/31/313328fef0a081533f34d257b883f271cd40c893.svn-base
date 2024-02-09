/*
 * hls_test.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: durga
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "AdaptiveMediaGenerator.hh"
#include "MediaServer.hh"
#include "MinuteFileWriter.hh"
#include "EditList.hh"
#include "FFMediaProcessor.hh"

#ifdef CreateDirectory
#undef CreateDirectory
#endif

using namespace std;
using namespace mediaindex;

typedef struct md5SumInfo {
    std::string m_strPath;
    std::string m_strMD5Sum;
} md5SumInfo_t;

class HLSTest : public testing::Test {
protected:
#ifdef HAVE_DISKIO
    virtual void SetUp() {
        m_pAdaptiveMediaGenerator = NULL;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/clips/rtp_media/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaMediaDir = "tmp_test_area/medianame/";
        GTestUtils::CreateDirectory(m_sTestAreaMediaDir);
    }

    virtual void TearDown() {
        delete m_pAdaptiveMediaGenerator;

        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }

    virtual void CheckMD5Sums(std::string basePath, md5SumInfo_t md5SumInfo[]) {
        char md5sum[40];
        for (int i = 0; ; i++) {
            if (md5SumInfo[i].m_strPath == "")
                break;
            GTestUtils::MD5SumFile(basePath + md5SumInfo[i].m_strPath, md5sum);

            EXPECT_STREQ(md5SumInfo[i].m_strMD5Sum.c_str(), md5sum);
        }
    }

protected:
    CAdaptiveMediaGenerator * m_pAdaptiveMediaGenerator;
    string m_sDataDir;
    string m_sTestAreaDir;
    string m_sTestAreaMediaDir;
#endif
};

static md5SumInfo_t md5InfoAxisHLSDir[] =
{
    {"output.m3u8", "f988a52b662a7b8dfc21dc5a81a64660"},
//    {"segment-000001.ts", "9402eaf4e06a5859dd4744d998d0f89a"},
    {"", ""}
};

#ifdef HAVE_DISKIO

TEST_F(HLSTest, CreateHLSForAxis) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "axis_with_audio/axis.sdp",
                                                            0, false);

    CMinuteFileWriter writer(m_sDataDir + "axis_with_audio/axis_with_audio.mkd",
                             "", m_sTestAreaMediaDir);

    writer.Silent();
    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();
 
    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_EQ(20014ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_EQ(1ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_EQ(1ULL, nStopFrames);

    uint64_t nVFramesSent = m_pAdaptiveMediaGenerator->GetTotalVFramesSentToHLS();
    EXPECT_EQ(17543ULL, nVFramesSent);

    uint64_t nAFramesSent = m_pAdaptiveMediaGenerator->GetTotalAFramesSentToHLS();
    EXPECT_EQ(2471ULL, nAFramesSent);

//    CheckMD5Sums(m_sTestAreaMediaDir + "hls/", md5InfoAxisHLSDir);
}

/*
TEST_F(HLSTest, CreateHLSForAntrica) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "antrica/antrica.sdp",
                                                            0, false);
    CMinuteFileWriter writer(m_sDataDir + "antrica",
                             "", m_sTestAreaMediaDir);

    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_NE(0ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_NE(0ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_NE(0ULL, nStopFrames);
}

TEST_F(HLSTest, CreateHLSWithClock) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "netviz_with_clock/netviz.sdp",
                                                            0, false);
    CMinuteFileWriter writer(m_sDataDir + "netviz_with_clock/netviz_with_clock.mkd",
                             "", m_sTestAreaMediaDir);

    CEditList * editList = new CEditList();
    editList->Set("15000:75000,135000:175000");
    writer.SetEditList(editList);

    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_EQ(35961ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_EQ(2ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_EQ(2ULL, nStopFrames);

    uint64_t nBytesWritten = m_pAdaptiveMediaGenerator->GetBytesWritten();
    EXPECT_GE(50900000ULL, nBytesWritten);
}

TEST_F(HLSTest, CreateHLSForBoeing) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "boeing/boeing.sdp",
                                                            0, false);
    CMinuteFileWriter writer(m_sDataDir + "boeing",
                             "", m_sTestAreaMediaDir);

    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_NE(0ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_NE(0ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_NE(0ULL, nStopFrames);
}

TEST_F(HLSTest, CreateHLSAndTNWithClock) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "netviz_with_clock/netviz.sdp",
                                                            0, false);
    std::vector<int >trackSpeeds;
    trackSpeeds.push_back(8);
    trackSpeeds.push_back(64);
    trackSpeeds.push_back(256);

    CFFMediaProcessor *pMediaProcessor = new CFFMediaProcessor(m_sDataDir + "netviz_with_clock/netviz_with_clock.mkd",
                                       m_sTestAreaMediaDir, trackSpeeds, "RTP",
                                       0, false, true, m_sDataDir + "netviz_with_clock/netviz.sdp");

    pMediaProcessor->EnablePrimaryTN(true);
    pMediaProcessor->EnableFFTracks(false);
    pMediaProcessor->InitMedia();

    CMinuteFileWriter writer(m_sDataDir + "netviz_with_clock/netviz_with_clock.mkd",
                             "", m_sTestAreaMediaDir);

    CEditList * editList = new CEditList();
    editList->Set("15000:38000");
    writer.SetEditList(editList);


    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);
    writer.AddMediaProcessor(pMediaProcessor);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_EQ(8273ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_EQ(1ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_EQ(1ULL, nStopFrames);


    delete pMediaProcessor;
}
*/
TEST_F(HLSTest, CreateHLSForAxisWithDrops) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "axis_with_audio/axis.sdp",
                                                            0, false);

    m_pAdaptiveMediaGenerator->SetVideoDropPercentage(1.0f, 5, 1234);
    m_pAdaptiveMediaGenerator->SetAudioDropPercentage(1.0f, 25, 1234);

    CMinuteFileWriter writer(m_sDataDir + "axis_with_audio/axis_with_audio.mkd",
                             "", m_sTestAreaMediaDir);

    writer.Silent();
    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_EQ(20014ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_EQ(1ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_EQ(1ULL, nStopFrames);

    uint64_t nVFramesSent = m_pAdaptiveMediaGenerator->GetTotalVFramesSentToHLS();
    EXPECT_EQ(16718ULL, nVFramesSent);

    uint64_t nAFramesSent = m_pAdaptiveMediaGenerator->GetTotalAFramesSentToHLS();
    EXPECT_EQ(1846ULL, nAFramesSent);
}

/*
TEST_F(HLSTest, CreateHLSForNetvizWithGaps) {
    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(m_sTestAreaMediaDir,
                                                            m_sDataDir + "netviz_with_gaps/netviz.sdp",
                                                            0, false);

    CMinuteFileWriter writer(m_sDataDir + "netviz_with_gaps",
                             "", m_sTestAreaMediaDir);

    writer.AddMediaProcessor(m_pAdaptiveMediaGenerator);

    writer.Write();

    m_pAdaptiveMediaGenerator->StopProcess();

    uint64_t nFramesProcessed = m_pAdaptiveMediaGenerator->GetTotalProcessedFrameCount();
    EXPECT_EQ(20014ULL, nFramesProcessed);

    uint64_t nStartFrames = m_pAdaptiveMediaGenerator->GetTotalStartFrameCount();
    EXPECT_EQ(1ULL, nStartFrames);

    uint64_t nStopFrames = m_pAdaptiveMediaGenerator->GetTotalStopFrameCount();
    EXPECT_EQ(1ULL, nStopFrames);

    uint64_t nVFramesSent = m_pAdaptiveMediaGenerator->GetTotalVFramesSentToHLS();
    EXPECT_EQ(17543ULL, nVFramesSent);

    uint64_t nAFramesSent = m_pAdaptiveMediaGenerator->GetTotalAFramesSentToHLS();
    EXPECT_EQ(2471ULL, nAFramesSent);

    CheckMD5Sums(m_sTestAreaMediaDir + "hls/", md5InfoAxisHLSDir);
}
*/
#endif
