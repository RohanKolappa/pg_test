/*
 * mediaserver_test.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "MediaServer.hh"

using namespace std;

using namespace mediaindex;

class MediaServerTest : public testing::TestWithParam<bool> {
protected:

    virtual void SetUp() {
        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/matroska/");

        m_sDataDirClips = GTestUtils::GetDataDir("..");
        m_sDataDirClips.append("/video/clips/");

        m_pMediaServer = NULL;
    }

    virtual void TearDown() {
        delete m_pMediaServer;
    }

    // We use value parameterized tests to ensure all tests pass using
    // both MatroskaMediaServer as well as MinuteMediaServer.
    // In the test data directory, for each minute file directory a
    // corresponding .mkd file exists
    virtual void CreateMediaServer(std::string aMediaPath) {
        bool useMatroska = GetParam();
        std::string mediaPath;
        if (useMatroska) {
            mediaPath = aMediaPath.append(".mkd");
        }
        else {
            mediaPath = aMediaPath;
        }
        m_pMediaServer = CMediaServer::Create(mediaPath.c_str());
    }

    virtual bool IsMatroska() {
        bool useMatroska = GetParam();
        return useMatroska;
    }

    virtual bool ValidateRetrievalOfFrames(std::string expectedMD5Sum, int framesToExpect, bool expectEOF) {
        bool retVal = true;
        int numberOfFrames = 0;
        while (true) {
            int rc = m_pMediaServer->GetFrame(m_cframe);
            if (rc != MSRC_OK) {
                if (expectEOF) {
                    EXPECT_EQ(rc, MSRC_END_OF_MEDIA);
                    EXPECT_STREQ("End of Media Reached", m_pMediaServer->GetErrorString());
                }
                else {
                    EXPECT_TRUE(false);
                }
                break;
            }
            // Ensure offsets and timestamps are correlated
            if (m_pMediaServer->GetOffsetFromTimecode(m_cframe.GetTimeStamp()) !=
                    m_cframe.GetTimeOffset()) {
                printf("Error: MediaServer returned inconsistent Offset %"PRIu64" %"PRIu64" at %"PRIu64"\n",
                        m_cframe.GetTimeOffset(),
                        m_pMediaServer->GetOffsetFromTimecode(m_cframe.GetTimeStamp()),
                        m_cframe.GetTimeStamp());
                EXPECT_TRUE(false);
                retVal = false;
                break;
            }
            if (m_cframe.GetMessageType() != CCFrame::MediaStopped) {
                // Cannot assert at stop since the timecode returned will be
                // the next start of media
                if (m_pMediaServer->GetTimecodeAtOffset(m_cframe.GetTimeOffset()) !=
                        m_cframe.GetTimeStamp()) {
                    printf("Error: MediaServer returned inconsistent Timecode %"PRIu64" %"PRIu64" at %"PRIu64"\n",
                            m_cframe.GetTimeStamp(),
                            m_pMediaServer->GetTimecodeAtOffset(m_cframe.GetTimeOffset()),
                            m_cframe.GetTimeOffset());
                    EXPECT_TRUE(false);
                    retVal = false;
                    break;
                }
            }
            // Ensure first frame signals media started
            if (numberOfFrames == 0) {
                EXPECT_EQ(CCFrame::MediaStarted, m_cframe.GetMessageType());
            }
            // Ensure last frame signals media stopped
            if (expectEOF && numberOfFrames == framesToExpect - 1) {
                EXPECT_EQ(CCFrame::MediaStopped, m_cframe.GetMessageType());
            }
            // Compute running md5sum of data in all frames
            if (m_cframe.GetMessageType() == CCFrame::Normal) {
                GTestUtils::MD5SumUpdate(m_cframe.GetData(), m_cframe.GetLength());
            }

            numberOfFrames++;
            if (numberOfFrames > framesToExpect) {
                // printf("Done at offset %llu\n", m_cframe.GetTimeOffset());
                break;
            }
        }

        // Confirm md5 sum of all frames
        char md5sum[40];
        GTestUtils::MD5SumResult(md5sum);
        EXPECT_STREQ(expectedMD5Sum.c_str(), md5sum);

        if (expectEOF)
            EXPECT_EQ(framesToExpect, numberOfFrames);

        return retVal;
    }

protected:
    string m_sDataDir;
    string m_sDataDirClips;
    CMediaServer * m_pMediaServer;
    CCFrame m_cframe;
};


// Instantiate all tests for using MatroskaMediaServer
INSTANTIATE_TEST_CASE_P(Matroska,
        MediaServerTest,
        ::testing::Values(true));

// Instantiate all tests for using MinuteMediaServer
INSTANTIATE_TEST_CASE_P(Minute,
        MediaServerTest,
        ::testing::Values(false));

TEST_P(MediaServerTest, BadInputDirReturnsError) {
    CreateMediaServer(m_sDataDir + "blah");
    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_MEDIA_ERROR, rc);
    //EXPECT_STREQ("Index file not found.", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, GetFrameWithoutStartReturnsError) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service not yet started", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingInitTwiceReturnsError) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Already Initialized", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStartWithoutInitReturnsError) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    int rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Not Yet Initialized", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStartWithoutStopReturnsError) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    m_pMediaServer->Start(0);
    int rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service already started", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStopWithoutStartReturnsError) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->Stop();
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service already stopped", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, GetMediaStartTimeStamp) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetMediaStartTimestamp());
}

TEST_P(MediaServerTest, GetMediaStopTimeStamp) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(1280362844081ULL, m_pMediaServer->GetMediaEndTimestamp());
}

TEST_P(MediaServerTest, MediaDuration) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(216371ULL, m_pMediaServer->GetMediaDuration());
}

TEST_P(MediaServerTest, MediaSize) {
    if (IsMatroska())
        return;
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();
    uint64_t mediaSize = m_pMediaServer->GetMediaSize(0, m_pMediaServer->GetMediaDuration());
    EXPECT_EQ(3898136ULL, mediaSize);
}

TEST_P(MediaServerTest, GetMediaStartTimeStampForGaps) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");
    m_pMediaServer->Init();
    EXPECT_EQ(1268674714655ULL, m_pMediaServer->GetMediaStartTimestamp());
}

TEST_P(MediaServerTest, GetMediaStopTimeStampForGaps) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");
    m_pMediaServer->Init();
    EXPECT_EQ(1268675113148ULL, m_pMediaServer->GetMediaEndTimestamp());
}

TEST_P(MediaServerTest, GetTimeCodeAtOffsets) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");
    m_pMediaServer->Init();
    EXPECT_EQ(1268674714655ULL, m_pMediaServer->GetTimecodeAtOffset(0));
    EXPECT_EQ(1268674715655ULL, m_pMediaServer->GetTimecodeAtOffset(1000));
    EXPECT_EQ(1268674724655ULL, m_pMediaServer->GetTimecodeAtOffset(10000));
    EXPECT_EQ(1268674814655ULL, m_pMediaServer->GetTimecodeAtOffset(100000));
    EXPECT_EQ(1268674870297ULL, m_pMediaServer->GetTimecodeAtOffset(155642)); // Just before a gap
    EXPECT_EQ(1268674883336ULL, m_pMediaServer->GetTimecodeAtOffset(155643)); // At a gap
    EXPECT_EQ(1268674883337ULL, m_pMediaServer->GetTimecodeAtOffset(155644)); // Just after a gap
    EXPECT_EQ(1268674887693ULL, m_pMediaServer->GetTimecodeAtOffset(160000));
    EXPECT_EQ(1268675044007ULL, m_pMediaServer->GetTimecodeAtOffset(300000));
    EXPECT_EQ(1268675113148ULL, m_pMediaServer->GetTimecodeAtOffset(400000)); // Beyond end of media
}


TEST_P(MediaServerTest, GetOffsetAtTimeCodes) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");
    m_pMediaServer->Init();
    EXPECT_EQ(0ULL, m_pMediaServer->GetOffsetFromTimecode(0ULL));
    EXPECT_EQ(0ULL, m_pMediaServer->GetOffsetFromTimecode(1268674714655ULL));
    EXPECT_EQ(1ULL, m_pMediaServer->GetOffsetFromTimecode(1268674714656ULL));
    EXPECT_EQ(100000ULL, m_pMediaServer->GetOffsetFromTimecode(1268674814655ULL));
    EXPECT_EQ(155642ULL, m_pMediaServer->GetOffsetFromTimecode(1268674870297ULL)); // Just before a gap
    EXPECT_EQ(155643ULL, m_pMediaServer->GetOffsetFromTimecode(1268674870298ULL)); // At a gap
    EXPECT_EQ(155643ULL, m_pMediaServer->GetOffsetFromTimecode(1268674870300ULL)); // Within a gap
    EXPECT_EQ(155643ULL, m_pMediaServer->GetOffsetFromTimecode(1268674883336ULL)); // End of a gap
    EXPECT_EQ(155644ULL, m_pMediaServer->GetOffsetFromTimecode(1268674883337ULL)); // Just after gap
    EXPECT_EQ(160000ULL, m_pMediaServer->GetOffsetFromTimecode(1268674887693ULL));
    EXPECT_EQ(300000ULL, m_pMediaServer->GetOffsetFromTimecode(1268675044007ULL));
    EXPECT_EQ(369141ULL, m_pMediaServer->GetOffsetFromTimecode(1268675200000ULL)); // Beyond end of media
}

TEST_P(MediaServerTest, GetGapListFromMediaWithoutGaps) {
    CreateMediaServer(m_sDataDir + "clip2_blob");
    m_pMediaServer->Init();

    std::list <CGap> gaps = m_pMediaServer->GetGaps();
    EXPECT_EQ(0, (int)gaps.size());
}

TEST_P(MediaServerTest, GetGapListFromMediaWithGaps) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");
    m_pMediaServer->Init();

    std::list <CGap> gaps = m_pMediaServer->GetGaps();
    ASSERT_EQ(2, (int)gaps.size());

    CGap gap = gaps.front();
    EXPECT_EQ(1268674870298ULL, gap.GetBeginTime());
    EXPECT_EQ(1268674883336ULL, gap.GetEndTime());

    gaps.pop_front();
    CGap gap2 = gaps.front();
    EXPECT_EQ(1268675023103ULL, gap2.GetBeginTime());
    EXPECT_EQ(1268675039417ULL, gap2.GetEndTime());
}

TEST_P(MediaServerTest, GetFramesFromBeyondEndOfMedia) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");

    m_pMediaServer->Init();

    int rc = m_pMediaServer->Start(1268675113148ULL + 10000);
    EXPECT_EQ(MSRC_OK, rc);

    // First we should get a "stopped" message
    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OK, rc);
    EXPECT_EQ(CCFrame::MediaStopped, m_cframe.GetMessageType());

    // Then we should get an "end of media"
    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_END_OF_MEDIA, rc);
}

TEST_P(MediaServerTest, GetAllFrames) {
    CreateMediaServer(m_sDataDir + "clip2_blob");

    int rc = m_pMediaServer->Init();

    EXPECT_FALSE(m_pMediaServer->IsSnapshot()); // better not be a snapshot

    EXPECT_EQ(MSRC_OK, rc);

    rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OK, rc);

    EXPECT_TRUE(ValidateRetrievalOfFrames("8d98a6eacc71beda200a061d16aa9801", 3751, true));
}


TEST_P(MediaServerTest, GetPartialFrames) {
    CreateMediaServer(m_sDataDir + "clip2_blob");

    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OK, rc);

    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(1000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("03b9a1bdf7c6c8e0107211d9e98793c8", 500, false));

    m_pMediaServer->Stop();
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(1000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("03b9a1bdf7c6c8e0107211d9e98793c8", 500, false));

    m_pMediaServer->Stop();
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(10000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("f207bc6c7f21b03c8e04ad151a315225", 500, false));

    m_pMediaServer->Stop();
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(15000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("4ab656becd44c7a9d23861656e7e9e62", 500, false));

    m_pMediaServer->Stop();
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(200000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("57ad559d337034975aa56c80f8c193bc", 280, true));

}

TEST_P(MediaServerTest, GetFramesWithGaps) {
    CreateMediaServer(m_sDataDirClips + "v2d_media/v2d_splash_gaps");

    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OK, rc);

    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(150000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("954028955c9624fef4f371bb3eac41d3", 2000, false));

    m_pMediaServer->Stop();
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(290000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("25d5b55c2250175d7db4199f6fe0d9e7", 2000, false));
}

TEST_P(MediaServerTest, GetFrameWithoutStartFailsForSnapshot) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service not yet started", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingInitTwiceFailsForSnapshot) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Already Initialized", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStartWithoutInitFailsForSnapshot) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    int rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Not Yet Initialized", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStartWithoutStopFailsForSnapshot) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    m_pMediaServer->Start(0);
    int rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service already started", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, CallingStopWithoutStartFailsForSnapshot) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    int rc = m_pMediaServer->Stop();
    EXPECT_EQ(MSRC_OBJECT_ERROR, rc);
    EXPECT_STREQ("Service already stopped", m_pMediaServer->GetErrorString());
}

TEST_P(MediaServerTest, GetSnapshotFrames) {

    CreateMediaServer(m_sDataDir + "snapshot1_blob");

    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OK, rc);

    EXPECT_TRUE(m_pMediaServer->IsSnapshot());

    rc = m_pMediaServer->Start(0);
    EXPECT_EQ(MSRC_OK, rc);

    // We should get three snapshot frames and then end of media
    char md5sum[40];

    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OK, rc);
    EXPECT_TRUE(m_cframe.GetMessageType() == CCFrame::Snapshot);
    EXPECT_EQ(34921UL, m_cframe.GetLength());
    EXPECT_EQ(1280362627710ULL, m_cframe.GetTimeStamp());
    EXPECT_EQ(IMAGETYPE_JPEG, (int)m_cframe.GetMediaFlag());
    GTestUtils::MD5Sum(m_cframe.GetData(), m_cframe.GetLength(), md5sum);
    EXPECT_STREQ("66c36303ded60b22ac9ffd2bc4142a2b", md5sum);

    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OK, rc);
    EXPECT_TRUE(m_cframe.GetMessageType() == CCFrame::Snapshot);
    EXPECT_EQ(3932214UL, m_cframe.GetLength());
    EXPECT_EQ(1280362637710ULL, m_cframe.GetTimeStamp());
    EXPECT_EQ(IMAGETYPE_BMP, (int)m_cframe.GetMediaFlag());
    GTestUtils::MD5Sum(m_cframe.GetData(), m_cframe.GetLength(), md5sum);
    EXPECT_STREQ("a078ffa2ef397c0feaba1b437d8a6ef2", md5sum);

    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_OK, rc);
    EXPECT_TRUE(m_cframe.GetMessageType() == CCFrame::Snapshot);
    EXPECT_EQ(28630UL, m_cframe.GetLength());
    EXPECT_EQ(1280362687623ULL, m_cframe.GetTimeStamp());
    EXPECT_EQ(IMAGETYPE_PNG, (int)m_cframe.GetMediaFlag());
    GTestUtils::MD5Sum(m_cframe.GetData(), m_cframe.GetLength(), md5sum);
    EXPECT_STREQ("3bd8f6ad32bab1ae84f594d24007d36b", md5sum);

    rc = m_pMediaServer->GetFrame(m_cframe);
    EXPECT_EQ(MSRC_END_OF_MEDIA, rc);

}

TEST_P(MediaServerTest, GetSnapshotStartTimeStamp) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetMediaStartTimestamp());
}

TEST_P(MediaServerTest, GetSnapshotStopTimeStamp) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetMediaEndTimestamp());
}

TEST_P(MediaServerTest, SnapshotDuration) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    EXPECT_EQ(0ULL, m_pMediaServer->GetMediaDuration());
}

TEST_P(MediaServerTest, SnapshotSize) {
    if (IsMatroska())
        return;
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();
    uint64_t mediaSize = m_pMediaServer->GetMediaSize(0, m_pMediaServer->GetMediaDuration());
    EXPECT_EQ(3995765ULL, mediaSize);
}

TEST_P(MediaServerTest, GetSnapshotTimecodesAndOffsets) {
    CreateMediaServer(m_sDataDir + "snapshot1_blob");
    m_pMediaServer->Init();

    // Any offset returns start time code
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetTimecodeAtOffset(0));
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetTimecodeAtOffset(1000));
    EXPECT_EQ(1280362627710ULL, m_pMediaServer->GetTimecodeAtOffset(5000));

    // Any timecode return 0 as offset
    EXPECT_EQ(0ULL, m_pMediaServer->GetOffsetFromTimecode(0));
    EXPECT_EQ(0ULL, m_pMediaServer->GetOffsetFromTimecode(1280362627710ULL));
    EXPECT_EQ(0ULL, m_pMediaServer->GetOffsetFromTimecode(1280362627710ULL + 1000));
}

TEST_P(MediaServerTest, GetFramesWithThumbnails) {
    if (IsMatroska())
        CreateMediaServer(m_sDataDir + "thumbnails1");
    else
        CreateMediaServer(m_sDataDir + "thumbnails1_blob");

    m_pMediaServer->ServeThumbnails(true);

    int rc = m_pMediaServer->Init();
    EXPECT_EQ(MSRC_OK, rc);


    // The follwoing tests repeat the same sequence twice to ensure that exactly
    // the same frames are served after rewinding media to the same point
    m_pMediaServer->Start(0);
    EXPECT_TRUE(ValidateRetrievalOfFrames("8324cba9764800cf0eb10cfd460ab002", 3000, false));
    m_pMediaServer->Stop();


    m_pMediaServer->Start(0);
    EXPECT_TRUE(ValidateRetrievalOfFrames("8324cba9764800cf0eb10cfd460ab002", 3000, false));
    m_pMediaServer->Stop();


    // Use an offset that would skip the first thumbnail in 3rd min file
    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(150000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("1ed286cc4bd6c578212a628bb71854db", 3000, false));

    m_pMediaServer->Stop();

    m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(150000));
    EXPECT_TRUE(ValidateRetrievalOfFrames("1ed286cc4bd6c578212a628bb71854db", 3000, false));

    m_pMediaServer->Stop();

}
