/*
 * mediawriter_test.cpp
 *
 *  Created on: Oct 12, 2012
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "MediaServer.hh"
#include "MediaWriter.hh"

using namespace std;

using namespace mediaindex;

class MediaWriterTest : public testing::Test {
protected:

    virtual void SetUp() {

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);
        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaBlobDir = "tmp_test_area/blob/";
        GTestUtils::CreateDirectory(m_sTestAreaBlobDir);

        m_nStartTime = 1277402988850ULL; // Some fixed time for testing

        m_pMediaServer = NULL;
        m_nFrameNumber = 0;
        m_nMediaFramesAdded = 0;
        m_nThumbnailsAdded = 0;
    }

    virtual void TearDown() {
        delete m_pMediaWriter;
        delete m_pMediaServer;

        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }

    virtual void CreateMediaWriter(std::string aMediaPath) {
        m_pMediaWriter = new CMediaWriter(aMediaPath.c_str());
    }


    virtual void CreateInitializedMediaWriter(std::string aMediaPath,
            bool bCreateThumbnails = true) {
        CreateMediaWriter(aMediaPath);
        m_pMediaWriter->EnableThumbnails(bCreateThumbnails);
        m_pMediaWriter->Init();
    }

    virtual void CreateMediaServer(std::string aMediaPath) {
        m_pMediaServer = CMediaServer::Create(aMediaPath.c_str());
        m_pMediaServer->ServeThumbnails(true);
        m_pMediaServer->Init();
    }

    virtual void AddFrame(uint64_t nTimestamp) {
        uint8_t buff[32];
        memset(buff, 0x0, 32);
        sprintf((char *)buff, "Frame %d\n", m_nFrameNumber);
        m_nFrameNumber++;
        m_nMediaFramesAdded++;
        CCFrame aFrame;
        aFrame.SetMessageType(CCFrame::Normal);
        aFrame.SetTimeStamp(nTimestamp);
        aFrame.SetMediaFlag(0);
        aFrame.SetData(buff);
        aFrame.SetLength(32);
        m_pMediaWriter->PutFrame(aFrame);
    }

    virtual void AddThumbnail(uint64_t nTimestamp,
            bool isPrimary = false,
            int imageType = IMAGETYPE_JPEG) {
        uint8_t buff[32];
        memset(buff, 0x0, 32);
        if (isPrimary) {
            sprintf((char *)buff, "Primary Thumbnail %d\n", 1);
        }
        else {
            sprintf((char *)buff, "Stream Thumbnail %d\n", m_nFrameNumber);
            m_nFrameNumber++;
        }
        m_nThumbnailsAdded++;
        CCFrame aFrame;
        aFrame.SetMessageType(CCFrame::Thumbnail);
        aFrame.SetTimeStamp(nTimestamp);
        uint32_t mediaFlag = imageType & IMAGETYPE_MASK;
        mediaFlag |= (isPrimary ? TNTYPE_PRIMARY : TNTYPE_STREAM);
        aFrame.SetMediaFlag(mediaFlag);
        aFrame.SetData(buff);
        aFrame.SetLength(32);
        m_pMediaWriter->AddThumbnail(aFrame);
    }

    virtual void ValidateRetrievalOfFrames(
            int framesToExpect,
            int startsToExpect,
            int thumbNailsToExpect) {

        m_pMediaServer->Start(0);

        int frameCount = 0;
        int startCount = 0;
        int mediaFrameCount = 0;
        int thumbnailCount = 0;
        while (true) {
            int rc = m_pMediaServer->GetFrame(m_cframe);
            if (rc != MSRC_OK)
                break;
            if (m_cframe.GetMessageType() == CCFrame::MediaStarted)
                startCount++;
            if (m_cframe.GetMessageType() == CCFrame::Normal) {
                int frameNumber = -1;
                char * data = (char *) m_cframe.GetData();
                sscanf(data, "Frame %d", &frameNumber);
                // printf("Got a frame %d : %d\n", frameCount, frameNumber);
                EXPECT_EQ(frameCount, frameNumber);
                if (frameCount != frameNumber)
                    return;
                frameCount++;
                mediaFrameCount++;
            }
            if (m_cframe.GetMessageType() == CCFrame::Thumbnail) {
                int frameNumber = -1;
                char * data = (char *) m_cframe.GetData();
                if ((m_cframe.GetMediaFlag() & TNTYPE_MASK) == TNTYPE_PRIMARY) {
                    sscanf(data, "Primary Thumbnail %d", &frameNumber);
                    EXPECT_EQ(1, frameNumber);
                    if (frameNumber != 1)
                        return;
                }
                else {
                    sscanf(data, "Stream Thumbnail %d", &frameNumber);
                    // printf("Got a Stream TN %d : %d\n", frameCount, frameNumber);
                    EXPECT_EQ(frameCount, frameNumber);
                    if (frameCount != frameNumber)
                        return;
                    frameCount++;
                }
                thumbnailCount++;
            }
        }
        EXPECT_EQ(framesToExpect, mediaFrameCount);
        EXPECT_EQ(startsToExpect, startCount);
        EXPECT_EQ(thumbNailsToExpect, thumbnailCount);
    }

    int MinuteOffset(int minuteNumber) {
        return minuteNumber * 60000;
    }

protected:
    CCFrame m_cframe;
    CMediaWriter * m_pMediaWriter;
    CMediaServer * m_pMediaServer;
    string m_sTestAreaDir;
    string m_sTestAreaBlobDir;
    int m_nFrameNumber;
    int m_nMediaFramesAdded;
    int m_nThumbnailsAdded;
    uint64_t    m_nStartTime;
};


TEST_F(MediaWriterTest, NonExistantDirReturnsError) {
    CreateMediaWriter(m_sTestAreaDir + "nosuchdir");
    int rc = m_pMediaWriter->Init();
    EXPECT_EQ(MSRC_MEDIA_ERROR, rc);
}


TEST_F(MediaWriterTest, InitWithValidDirectory) {
    CreateMediaWriter(m_sTestAreaBlobDir);
    int rc = m_pMediaWriter->Init();
    EXPECT_EQ(MSRC_OK, rc);
}


TEST_F(MediaWriterTest, StartStopAtSameTime) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->Start(m_nStartTime);
    m_pMediaWriter->Stop(m_nStartTime);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_EQ(0ULL, m_pMediaServer->GetMediaDuration());
}


TEST_F(MediaWriterTest, StartStopWithoutPackets) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->Start(m_nStartTime);
    m_pMediaWriter->Stop(m_nStartTime + 12345);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_EQ(12345ULL, m_pMediaServer->GetMediaDuration());
}

TEST_F(MediaWriterTest, RecordSomeFrames) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir, false);
    m_pMediaWriter->Start(m_nStartTime);
    for (int i = 0; i < 10; i++)
        AddFrame(m_nStartTime + i * 100);
    m_pMediaWriter->Stop(m_nStartTime + 10000);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_EQ(10000ULL, m_pMediaServer->GetMediaDuration());
    std::list<CGap> gaps = m_pMediaServer->GetGaps();
    EXPECT_EQ(0, (int) gaps.size());
    EXPECT_FALSE(m_pMediaServer->HasThumbnails());

    ValidateRetrievalOfFrames(10, 1, 0);
}

TEST_F(MediaWriterTest, CreateGapsWithoutFrames) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir, false);
    for (int i = 0; i < 10; i++) {
        m_pMediaWriter->Start(m_nStartTime);
        m_pMediaWriter->Stop(m_nStartTime + 10000);
        m_nStartTime += 70000;
    }

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_EQ(100000ULL, m_pMediaServer->GetMediaDuration());
    std::list<CGap> gaps = m_pMediaServer->GetGaps();
    EXPECT_EQ(9, (int) gaps.size());
    EXPECT_FALSE(m_pMediaServer->HasThumbnails());

    ValidateRetrievalOfFrames(0, 10, 0);
}

TEST_F(MediaWriterTest, CreateGapsWithFrames) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir, false);
    for (int i = 0; i < 10; i++) {
        m_pMediaWriter->Start(m_nStartTime);
        for (int i = 0; i < 10; i++)
            AddFrame(m_nStartTime + i * 100);
        m_pMediaWriter->Stop(m_nStartTime + 10000);
        m_nStartTime += 70000;
    }

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_EQ(100000ULL, m_pMediaServer->GetMediaDuration());
    std::list<CGap> gaps = m_pMediaServer->GetGaps();
    EXPECT_EQ(9, (int) gaps.size());
    EXPECT_FALSE(m_pMediaServer->HasThumbnails());

    ValidateRetrievalOfFrames(100, 10, 0);
}

TEST_F(MediaWriterTest, AddPrimaryJPGThumbnail) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);

    m_pMediaWriter->Start(m_nStartTime);
    for (int i = 0; i < 10; i++)
        AddFrame(m_nStartTime + i * 100);
    AddThumbnail(m_nStartTime, true, IMAGETYPE_JPEG);
    m_pMediaWriter->Stop(m_nStartTime + 10000);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_TRUE(m_pMediaServer->HasThumbnails());
    ValidateRetrievalOfFrames(10, 1, 1);
}

TEST_F(MediaWriterTest, AddPrimaryPNGThumbnail) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->Start(m_nStartTime);
    for (int i = 0; i < 10; i++)
        AddFrame(m_nStartTime + i * 100);
    AddThumbnail(m_nStartTime, true, IMAGETYPE_PNG);
    m_pMediaWriter->Stop(m_nStartTime + 10000);

    CreateMediaServer(m_sTestAreaBlobDir);
    ValidateRetrievalOfFrames(10, 1, 1);
}

TEST_F(MediaWriterTest, OnlyThumbnailMode) {
    CreateMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->EnableThumbnails(true);
    m_pMediaWriter->EnableMedia(false);
    m_pMediaWriter->Init();

    m_pMediaWriter->Start(m_nStartTime);

    for (int i = 0; i < 10; i++)
        AddFrame(m_nStartTime + i * 100);
    AddThumbnail(m_nStartTime, true, IMAGETYPE_JPEG);
    m_pMediaWriter->Stop(m_nStartTime + 10000);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_TRUE(m_pMediaServer->HasThumbnails());

    // In thumbnail only mode we cannot (yet) get any frames
    // We just ensure minute files are not created, but tn.jpg exists
    EXPECT_FALSE(GTestUtils::FileExists(m_sTestAreaBlobDir + "INDEX"));
    EXPECT_TRUE(GTestUtils::FileExists(m_sTestAreaBlobDir + "tn/tn.jpg"));
}

TEST_F(MediaWriterTest, AddPrimaryAndStreamThumbnails) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->Start(m_nStartTime);
    AddThumbnail(m_nStartTime, true, IMAGETYPE_JPEG);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10; i++) {
            m_nStartTime += 100;
            AddFrame(m_nStartTime);
        }
        m_nStartTime += 100;
        AddThumbnail(m_nStartTime, false, IMAGETYPE_JPEG);
    }
    m_nStartTime += 100;
    m_pMediaWriter->Stop(m_nStartTime);

    CreateMediaServer(m_sTestAreaBlobDir);
    EXPECT_TRUE(m_pMediaServer->HasThumbnails());
    ValidateRetrievalOfFrames(m_nMediaFramesAdded, 1, m_nThumbnailsAdded);
}


TEST_F(MediaWriterTest, AddStreamThumbnailsSparse) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    m_pMediaWriter->Start(m_nStartTime);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10; i++) {
            m_nStartTime += 100;
            AddFrame(m_nStartTime);
        }
        m_nStartTime += 100;
        AddThumbnail(m_nStartTime, false, IMAGETYPE_JPEG);
        for (int i = 0; i < 10; i++) {
            m_nStartTime += 100;
            AddFrame(m_nStartTime);
        }
        m_nStartTime += 65000;
    }
    m_nStartTime += 100;
    m_pMediaWriter->Stop(m_nStartTime);

    CreateMediaServer(m_sTestAreaBlobDir);

    ValidateRetrievalOfFrames(m_nMediaFramesAdded, 1, m_nThumbnailsAdded);
}

TEST_F(MediaWriterTest, AddStreamThumbnailsWithGaps) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);
    for (int j = 0; j < 10; j++) {
        m_pMediaWriter->Start(m_nStartTime);
        for (int i = 0; i < 10; i++) {
            m_nStartTime += 100;
            AddFrame(m_nStartTime);
        }
        m_nStartTime += 100;
        AddThumbnail(m_nStartTime);
        for (int i = 0; i < 10; i++) {
            m_nStartTime += 100;
            AddFrame(m_nStartTime);
        }
        m_nStartTime += 100;
        AddThumbnail(m_nStartTime);

        m_nStartTime += 100;
        m_pMediaWriter->Stop(m_nStartTime);
        m_nStartTime += 120000;
    }

    CreateMediaServer(m_sTestAreaBlobDir);
    ValidateRetrievalOfFrames(m_nMediaFramesAdded, 10, m_nThumbnailsAdded);
}

TEST_F(MediaWriterTest, AddStreamThumbnailsMixed) {
    CreateInitializedMediaWriter(m_sTestAreaBlobDir);

    m_pMediaWriter->Start(m_nStartTime);

    // Only thumbnail in first minute
    AddThumbnail(m_nStartTime + MinuteOffset(0) + 200);

    // Thumbnail between two frames
    AddFrame(m_nStartTime + MinuteOffset(1) + 100);
    AddThumbnail(m_nStartTime + MinuteOffset(1) + 200);
    AddFrame(m_nStartTime + MinuteOffset(1) + 300);

    // Thumbnail before first frame
    AddThumbnail(m_nStartTime + MinuteOffset(2) + 100);
    AddFrame(m_nStartTime + MinuteOffset(2) + 100);

    // Thumbnail after first frame
    AddFrame(m_nStartTime + MinuteOffset(3) + 100);
    AddThumbnail(m_nStartTime + MinuteOffset(3) + 200);

    // Only thumbnails
    AddThumbnail(m_nStartTime + MinuteOffset(4) + 200);
    AddThumbnail(m_nStartTime + MinuteOffset(4) + 300);
    AddThumbnail(m_nStartTime + MinuteOffset(4) + 400);

    // Thumbnails before a stop
    AddFrame(m_nStartTime + MinuteOffset(5) + 100);
    AddThumbnail(m_nStartTime + MinuteOffset(5) + 200);
    AddThumbnail(m_nStartTime + MinuteOffset(5) + 300);

    m_pMediaWriter->Stop(m_nStartTime + MinuteOffset(6) + 300);

    CreateMediaServer(m_sTestAreaBlobDir);
    ValidateRetrievalOfFrames(m_nMediaFramesAdded, 1, m_nThumbnailsAdded);
}
