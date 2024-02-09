/*
 * parser_test.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"
#include "libmediaconverter.hh"
#include "MatroskaFileParser.hh"
#include "XMLUtils.hh"

using namespace mediaindex;

using namespace std;

class ParserTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/matroska/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_pParser = NULL;
    }

    virtual void TearDown() {
        delete m_pParser;
        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }

    virtual void CreateMatroskaFileParser(std::string filePath) {
        m_pParser = new CMatroskaFileParser(filePath);
    }

    virtual bool GetMediaStartedFrame() {
        CCFrame aFrame;
        m_pParser->GetFrame(aFrame);
        return (CCFrame::MediaStarted == aFrame.GetMessageType());
    }

protected:
    string m_sDataDir;
    CMatroskaFileParser * m_pParser;
    string m_sTestAreaDir;
};

// Test with non existing file
TEST_F(ParserTest, BadInputFileReturnsError) {
    CreateMatroskaFileParser(m_sDataDir + "blah.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "Can't open stdio file"));
}

TEST_F(ParserTest, BadInputFileFailsToInitialize) {
    CreateMatroskaFileParser(m_sDataDir + "blah.mkd");
    m_pParser->Parse();
    EXPECT_FALSE(m_pParser->InitializeMediaToTimeOffset(0));
}

TEST_F(ParserTest, BadInputFileFailsToGetFrame) {
    CreateMatroskaFileParser(m_sDataDir + "blah.mkd");
    m_pParser->Parse();
    CCFrame aFrame;
    EXPECT_FALSE(m_pParser->GetFrame(aFrame));
}

TEST_F(ParserTest, MissingCallToParserFailsOtherCalls) {
    CreateMatroskaFileParser(m_sDataDir + "blah.mkd");
    EXPECT_FALSE(m_pParser->InitializeMediaToTimeOffset(0));
    CCFrame aFrame;
    EXPECT_FALSE(m_pParser->GetFrame(aFrame));
}


TEST_F(ParserTest, NoParseCalledReturnsZeroAsFileSize) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    EXPECT_EQ(0, (int)m_pParser->GetFileSize());
}

// Test malformed file
TEST_F(ParserTest, MalformedFileZeroLength) {
    unsigned char buffer[256];
    memset(buffer, 0xab, 256);
    GTestUtils::CreateFileFromBuffer(m_sTestAreaDir + "malformed.mkd", buffer, 0);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    //cout << m_pParser->GetErrorString() << endl;
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No EBML head found"));
}

TEST_F(ParserTest, MalformedFileShort) {
    unsigned char buffer[256];
    memset(buffer, 0xab, 256);
    GTestUtils::CreateFileFromBuffer(m_sTestAreaDir + "malformed.mkd", buffer, 7);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_EQ(7, (int)m_pParser->GetFileSize());
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No segment/level 0 element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedToTwoBytes) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 2);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_EQ(2, (int)m_pParser->GetFileSize());
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No EBML head found"));
}

TEST_F(ParserTest, MalformedFileTruncatedToSegmentHeader) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 27);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_EQ(27, (int)m_pParser->GetFileSize());
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No segment/level 0 element found"));
}

// For some reason 28 is not working in some PCs.
TEST_F(ParserTest, MalformedFileTruncatedToWithinSegmentHeader) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 29);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No seekhead element found"));
}


TEST_F(ParserTest, MalformedFileTruncatedToWithinSeekHead) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 39);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No seekhead element found"));
}


TEST_F(ParserTest, MalformedFileTruncatedToWithinSeekEntry) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 43);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No seekhead element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedToEndOfSeekHead) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 119);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No segment info element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedStartOfSegmentInfo) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 4135);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No segment info element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedWithinSegmentInfo) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 4280);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No segment info element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedToTagsStart) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 3920717);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No tags element found"));
}

TEST_F(ParserTest, MalformedFileTruncatedOneByteShort) {
    GTestUtils::CreateTruncatedFile(m_sDataDir + "clip1.mkd",
            m_sTestAreaDir + "malformed.mkd", 3934907);
    CreateMatroskaFileParser(m_sTestAreaDir + "malformed.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_ERROR, rc);
    EXPECT_TRUE(GTestUtils::StringContains(m_pParser->GetErrorString(),
            "No tags element found"));
}

TEST_F(ParserTest, FileSize) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_OK, rc);
    EXPECT_EQ(3938840, (int)m_pParser->GetFileSize());
}


TEST_F(ParserTest, MediaCreateDate) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    EXPECT_EQ(1280362627710ULL, m_pParser->GetMediaCreateDate());
}

TEST_F(ParserTest, MediaTitle) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    EXPECT_STREQ("ANUJ - tx10711", m_pParser->GetMediaTitle().c_str());
}

TEST_F(ParserTest, MediaDuration) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    EXPECT_EQ(216371ULL, m_pParser->GetMediaDuration());
}

TEST_F(ParserTest, StreamType) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    EXPECT_STREQ("V2D", m_pParser->GetStreamType().c_str());
}

// Compute md5sum of all the metadata xmls concatenated and verify result
TEST_F(ParserTest, ParseGetTags) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    int rc = m_pParser->Parse();
    EXPECT_EQ(ELIBMC_OK, rc);
    list<CMetadataHolder> metaDataList = m_pParser->GetMetadata("all");
    EXPECT_EQ(4, (int)metaDataList.size());
    string resultString;
    for (list<CMetadataHolder>::iterator iter = metaDataList.begin();
            iter != metaDataList.end(); ++iter) {
        DomDocToString(iter->m_pXmlDoc, resultString);
        GTestUtils::MD5SumUpdate((unsigned char *)resultString.c_str(), resultString.length());
    }
    char md5sum[40];
    GTestUtils::MD5SumResult(md5sum);
    //printf("md5sum = %s\n", md5sum);
    EXPECT_STREQ("7b96838304678013785a1861cf108f76", md5sum);
}

TEST_F(ParserTest, InitializeMediaToTimeOffsetSucceeds) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    bool retVal;
    retVal = m_pParser->InitializeMediaToTimeOffset(0);
    EXPECT_TRUE(retVal);
    retVal = m_pParser->InitializeMediaToTimeOffset(13123);
    EXPECT_TRUE(retVal);
    retVal = m_pParser->InitializeMediaToTimeOffset(215863);
    EXPECT_TRUE(retVal);
}

TEST_F(ParserTest, GetFirstFrame) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(0);
    CCFrame aFrame;
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::MediaStarted, aFrame.GetMessageType());
    EXPECT_EQ(0UL, aFrame.GetLength());
    EXPECT_EQ(1280362627710ULL, aFrame.GetTimeStamp());
}

TEST_F(ParserTest, GetLastFrame) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(216371);
    CCFrame aFrame;
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::MediaStopped, aFrame.GetMessageType());
    EXPECT_EQ(0UL, aFrame.GetLength());
    EXPECT_EQ(1280362844081ULL, aFrame.GetTimeStamp());
}

TEST_F(ParserTest, GetAnyFrameReturnsMediaStartFirst) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(12000);

    EXPECT_TRUE(GetMediaStartedFrame());

    CCFrame aFrame;
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::Normal, aFrame.GetMessageType());
    EXPECT_EQ(1036UL, aFrame.GetLength());
    EXPECT_EQ(1280362639726ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(12016ULL, aFrame.GetTimeOffset());
}

TEST_F(ParserTest, GetFirstFrameInCluster) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(14918);
    EXPECT_TRUE(GetMediaStartedFrame());

    CCFrame aFrame;
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::Normal, aFrame.GetMessageType());
    EXPECT_EQ(1280362642628ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(14918ULL, aFrame.GetTimeOffset());
}

TEST_F(ParserTest, GetLastFrameInCluster) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(19803);
    CCFrame aFrame;

    EXPECT_TRUE(GetMediaStartedFrame());

    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::Normal, aFrame.GetMessageType());
    EXPECT_EQ(1280362647513ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(19803ULL, aFrame.GetTimeOffset());
}

TEST_F(ParserTest, GetFrameAfterLastFrameInCluster) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(19804);
    EXPECT_TRUE(GetMediaStartedFrame());

    CCFrame aFrame;
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(CCFrame::Normal, aFrame.GetMessageType());
    EXPECT_EQ(1280362647639ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(19929ULL, aFrame.GetTimeOffset());
}

TEST_F(ParserTest, GetFramesAfterSkippingMedia) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    CCFrame aFrame;

    m_pParser->InitializeMediaToTimeOffset(12000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1280362639726ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(12016ULL, aFrame.GetTimeOffset());

    m_pParser->InitializeMediaToTimeOffset(100000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1280362727748ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(100038ULL, aFrame.GetTimeOffset());

    m_pParser->InitializeMediaToTimeOffset(12000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1280362639726ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(12016ULL, aFrame.GetTimeOffset());
}

TEST_F(ParserTest, GetFramesTillEndOfMediaDetected) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(216000);
    EXPECT_TRUE(GetMediaStartedFrame());
    for (int i = 0; i < 5; i++) {
        CCFrame aFrame;
        EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    }
    CCFrame aFrame;
    EXPECT_FALSE(m_pParser->GetFrame(aFrame));
}

TEST_F(ParserTest, GetConsecutiveFrames) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(21000);

    EXPECT_TRUE(GetMediaStartedFrame());

    for (int i = 0; i < 500; i++) {
        CCFrame aFrame;
        m_pParser->GetFrame(aFrame);
        if (i == 0) {
            EXPECT_EQ(1280362648739ULL, aFrame.GetTimeStamp());
            EXPECT_EQ(21029ULL, aFrame.GetTimeOffset());
            EXPECT_EQ(1036, (int)aFrame.GetLength());
        }
        if (i == 78) {
            EXPECT_EQ(1280362652932ULL, aFrame.GetTimeStamp());
            EXPECT_EQ(25222ULL, aFrame.GetTimeOffset());
            EXPECT_EQ(220, (int)aFrame.GetLength());
        }

        if (i == 499) {
            EXPECT_EQ(1280362677903ULL, aFrame.GetTimeStamp());
            EXPECT_EQ(50193ULL, aFrame.GetTimeOffset());
            EXPECT_EQ(1036, (int)aFrame.GetLength());
        }
    }
}

TEST_F(ParserTest, GetAllFrames) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(0);
    EXPECT_FALSE(m_pParser->HasThumbnails());
    EXPECT_FALSE(m_pParser->IsSnapshot());

    int numberOfFrames = 0;
    while (numberOfFrames < 3800) {
        CCFrame aFrame;
        if (!m_pParser->GetFrame(aFrame)) {
            break;
        }
        numberOfFrames++;
    }
    EXPECT_EQ(3751, numberOfFrames);
}

TEST_F(ParserTest, GetAllBookMarks) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->NeedBookmarkFrames(true);
    m_pParser->InitializeMediaToTimeOffset(0);
    int numberOfFrames = 0;
    while (numberOfFrames < 10) {
        CCFrame aFrame;
        if (!m_pParser->GetFrame(aFrame)) {
            break;
        }
        char md5sum[40];
        GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
        numberOfFrames++;
        if (numberOfFrames == 1) {
            EXPECT_STREQ("9ead85a2925b1097a89e10e4fd099400", md5sum);
        }
        else if (numberOfFrames == 2) {
            EXPECT_STREQ("5a2a332845159c2a69d501a4ed691bfa", md5sum);
        }
        else if (numberOfFrames == 3) {
            EXPECT_STREQ("147c721bce11c95c01d3f696c4e709d6", md5sum);
        }
        else if (numberOfFrames == 4) {
            EXPECT_STREQ("4e36420047f7305bdd665296eb4e133e", md5sum);
        }
        else if (numberOfFrames == 5) {
            EXPECT_STREQ("4938e9d9eb65f348e8c64122c200c00d", md5sum);
        }
        else if (numberOfFrames == 6) {
            EXPECT_STREQ("0f8c8408c7baa092947175f2a038c52f", md5sum);
        }
        else if (numberOfFrames == 7) {
            EXPECT_STREQ("a06320da9f1ca6e38692d556a372fd24", md5sum);
        }
        else if (numberOfFrames == 8) {
            EXPECT_STREQ("516b51566bef36e6b085935d300e349d", md5sum);
        }
        else if (numberOfFrames == 9) {
            EXPECT_STREQ("b9a769305a91ffe500276ab8cbe4537b", md5sum);
        }
    }
    EXPECT_EQ(9, numberOfFrames);
}

TEST_F(ParserTest, GetBookmarksAtSpecificOffsets) {
    CreateMatroskaFileParser(m_sDataDir + "clip1.mkd");
    m_pParser->Parse();
    m_pParser->NeedBookmarkFrames(true);

    CCFrame aFrame;
    char md5sum[40];

    // At first bookmark
    m_pParser->InitializeMediaToTimeOffset(32128);
    EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
    EXPECT_STREQ("0f8c8408c7baa092947175f2a038c52f", md5sum);

    // Just first bookmark, should get second
    m_pParser->InitializeMediaToTimeOffset(32129);
    EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
    EXPECT_STREQ("516b51566bef36e6b085935d300e349d", md5sum);

    // Before second bookmark
    m_pParser->InitializeMediaToTimeOffset(60000);
    EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
    EXPECT_STREQ("516b51566bef36e6b085935d300e349d", md5sum);

    // At second bookmark
    m_pParser->InitializeMediaToTimeOffset(114114);
    EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
    EXPECT_STREQ("516b51566bef36e6b085935d300e349d", md5sum);

    // At third bookmark
    m_pParser->InitializeMediaToTimeOffset(158866);
    EXPECT_TRUE(m_pParser->GetFrame(aFrame));
    GTestUtils::MD5Sum(aFrame.GetData(), aFrame.GetLength(), md5sum);
    EXPECT_STREQ("b9a769305a91ffe500276ab8cbe4537b", md5sum);

    // Just after third bookmark
    m_pParser->InitializeMediaToTimeOffset(158867);
    EXPECT_FALSE(m_pParser->GetFrame(aFrame));
}

TEST_F(ParserTest, MediaCreateDateSnapshot) {
    CreateMatroskaFileParser(m_sDataDir + "snapshot1_blob.mkd");
    m_pParser->Parse();
    EXPECT_EQ(1280362627710ULL, m_pParser->GetMediaCreateDate());
}

TEST_F(ParserTest, MediaTitleSnapshot) {
    CreateMatroskaFileParser(m_sDataDir + "snapshot1_blob.mkd");
    m_pParser->Parse();
    EXPECT_STREQ("Unknown", m_pParser->GetMediaTitle().c_str());
}

TEST_F(ParserTest, MediaDurationSnapshot) {
    CreateMatroskaFileParser(m_sDataDir + "snapshot1_blob.mkd");
    m_pParser->Parse();
    EXPECT_EQ(0ULL, m_pParser->GetMediaDuration());
}

TEST_F(ParserTest, StreamTypeSnapshot) {
    CreateMatroskaFileParser(m_sDataDir + "snapshot1_blob.mkd");
    m_pParser->Parse();
    EXPECT_STREQ("SNAPSHOT", m_pParser->GetStreamType().c_str());
}

TEST_F(ParserTest, GetAllFramesSnapshot) {
    CreateMatroskaFileParser(m_sDataDir + "snapshot1_blob.mkd");
    m_pParser->Parse();
    EXPECT_TRUE(m_pParser->IsSnapshot());
    m_pParser->InitializeMediaToTimeOffset(0);
    int numberOfFrames = 0;
    while (numberOfFrames < 10) {
        CCFrame aFrame;
        if (!m_pParser->GetFrame(aFrame)) {
            break;
        }
        numberOfFrames++;
    }
    EXPECT_EQ(3, numberOfFrames);
}

TEST_F(ParserTest, GetAllFramesWithThumbnails) {
    CreateMatroskaFileParser(m_sDataDir + "thumbnails1.mkd");
    m_pParser->ServeThumbnails(true);
    m_pParser->Parse();
    m_pParser->InitializeMediaToTimeOffset(0);

    EXPECT_TRUE(m_pParser->HasThumbnails());

    int numberOfFrames = 0;
    int numberOfThumbnailFrames = 0;
    while (numberOfFrames < 25000) {
        CCFrame aFrame;
        if (!m_pParser->GetFrame(aFrame)) {
            break;
        }

        numberOfFrames++;

        if (aFrame.GetMessageType() == CCFrame::Thumbnail)
            numberOfThumbnailFrames++;
    }
    EXPECT_EQ(24714, numberOfFrames);
    EXPECT_EQ(17, numberOfThumbnailFrames);
}

TEST_F(ParserTest, GetFramesAfterSkippingMediaWithThumbnails) {
    CreateMatroskaFileParser(m_sDataDir + "thumbnails1.mkd");
    m_pParser->ServeThumbnails(true);
    m_pParser->Parse();
    CCFrame aFrame;

    m_pParser->InitializeMediaToTimeOffset(150000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1268674864676ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(150021ULL, aFrame.GetTimeOffset());
    EXPECT_EQ(2022ULL, aFrame.GetLength());  // Thumbnail Size


    m_pParser->InitializeMediaToTimeOffset(160000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1268674887766ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(160073ULL, aFrame.GetTimeOffset());
    EXPECT_EQ(1036ULL, aFrame.GetLength());  // Regular Frame size

    m_pParser->InitializeMediaToTimeOffset(162000);
    EXPECT_TRUE(GetMediaStartedFrame());
    m_pParser->GetFrame(aFrame);
    EXPECT_EQ(1268674889712ULL, aFrame.GetTimeStamp());
    EXPECT_EQ(162019ULL, aFrame.GetTimeOffset());
    EXPECT_EQ(2102ULL, aFrame.GetLength());  // Thumbnail size

}
