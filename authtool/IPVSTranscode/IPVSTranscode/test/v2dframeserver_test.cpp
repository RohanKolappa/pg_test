/*
 * v2dframeserver_test.cpp
 *
 *  Created on: Sep 22, 2011
 *      Author: rkale
 */


#include "IPVSTranscode_compatibility.h"

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "V2DFrameServer.hh"

#include "bitmap.hh"

using namespace std;

class CV2DFrameServerTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_strDataDir = GTestUtils::GetDataDir("../../..");
        ASSERT_NE(m_strDataDir,  "");
        m_strDataDir.append("/video/clips/");

        m_pFrameServer = NULL;
    }

    virtual void TearDown() {
        delete m_pFrameServer;
    }

    virtual void CreateFrameServer(string a_path, int a_fps = 30)
    {
        string path = m_strDataDir + a_path;
        m_pFrameServer = new CV2DFrameServer(path, a_fps);
    }

    virtual void ComputeMd5Sum() {
        GTestUtils::MD5Sum(m_frameData, m_pFrameServer->GetVideoFrameSize(), m_md5sum);
    }

    virtual void PrintMd5Sum() {
        printf("md5sum = %s\n", m_md5sum);
    }

    virtual void VerifyMd5Sum(const char * md5Str, const char * dummy) {
        EXPECT_STREQ(md5Str, m_md5sum);
    }

    virtual void SaveImage() {
        CBitmap oSaveBmp;
        oSaveBmp.SetBitsBGRA(m_frameData,
                m_pFrameServer->GetFrameWidth(), m_pFrameServer->GetFrameHeight());
        oSaveBmp.Save("image.bmp");
    }

protected:
    string m_strDataDir;
    CV2DFrameServer * m_pFrameServer;
    unsigned char * m_frameData;
    char m_md5sum[40];
    int m_frameNo;

};


TEST_F(CV2DFrameServerTest, CreateNonExistant) {

    CreateFrameServer("v2d_XXX");
    int retVal = m_pFrameServer->Initialize();
    EXPECT_EQ(FS_MEDIASERVER_INIT_FAILED, retVal);
}

TEST_F(CV2DFrameServerTest, NonV2DMedia) {

    CreateFrameServer("udp_media/udp_media_gaps");
    int retVal = m_pFrameServer->Initialize();
    EXPECT_EQ(FS_STREAMTYPE_NOTSUPPORTED, retVal);
}

TEST_F(CV2DFrameServerTest, Initialize) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    int retVal = m_pFrameServer->Initialize();
    EXPECT_EQ(FS_SUCCESS, retVal);
    EXPECT_EQ(1280, m_pFrameServer->GetFrameWidth());
    EXPECT_EQ(1024, m_pFrameServer->GetFrameHeight());
    EXPECT_EQ(1280 * 1024 * 4, m_pFrameServer->GetVideoFrameSize());
    EXPECT_EQ(11074ULL, m_pFrameServer->GetVideoFrameCount());
    EXPECT_EQ(0ULL, m_pFrameServer->GetVideoFramesDecoded());
}

TEST_F(CV2DFrameServerTest, DoubleInitializationShouldFail) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();
    int retVal = m_pFrameServer->Initialize();
    EXPECT_EQ(FS_ALREADY_INITIALIZED, retVal);
}

TEST_F(CV2DFrameServerTest, FrameCountFPS_20) {
    CreateFrameServer("v2d_media/v2d_splash_gaps", 20);
    m_pFrameServer->Initialize();
    EXPECT_EQ(7383ULL, m_pFrameServer->GetVideoFrameCount());
}

TEST_F(CV2DFrameServerTest, FrameCountFPS_1) {
    CreateFrameServer("v2d_media/v2d_splash_gaps", 1);
    m_pFrameServer->Initialize();
    EXPECT_EQ(369ULL, m_pFrameServer->GetVideoFrameCount());
}

TEST_F(CV2DFrameServerTest, GetConsecutiveFrames) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();

    // Get first 128 frames, and verify count of actual
    // frames decoded matches known numbers
    for (m_frameNo = 0; m_frameNo < 132; m_frameNo++) {
        m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
        if (m_frameNo >= 0 && m_frameNo <= 55)
            EXPECT_EQ(0ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 56 && m_frameNo <= 58)
            EXPECT_EQ(1863ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 59 && m_frameNo <= 62)
            EXPECT_EQ(1948ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 78 && m_frameNo <= 80)
            EXPECT_EQ(2584ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 120 && m_frameNo <= 123)
            EXPECT_EQ(3980ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 124 && m_frameNo <= 125)
            EXPECT_EQ(4107ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo >= 126 && m_frameNo <= 127)
            EXPECT_EQ(4193ULL, m_pFrameServer->GetCurrentVideoServedOffset());
        if (m_frameNo == 0) {
            ComputeMd5Sum();
            EXPECT_STREQ("5f363e0e58a95f06cbe9bbc662c5dfb6", m_md5sum);
        }

        if (m_frameNo == 59) {
            ComputeMd5Sum();
            EXPECT_STREQ("d00661847761b8e4bf14b14408cc8e69", m_md5sum);
        }

        if (m_frameNo == 127) {
            ComputeMd5Sum();
            EXPECT_STREQ("ded3b79ba98b804661ac8f6d30ec4213", m_md5sum);
        }
    }
}


TEST_F(CV2DFrameServerTest, GetConsecutiveFramesAfterJump) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();


    int startFrame = 2000 - 132;
    for (m_frameNo = startFrame; m_frameNo < startFrame + 132 + 32; m_frameNo++) {
        m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
        if (m_frameNo == 1991) {
            ComputeMd5Sum();
            EXPECT_STREQ("1c5c5c90810ea2a4fafd1f30e1086d1a", m_md5sum);
        }

        if (m_frameNo == 1992) {
            ComputeMd5Sum();
            EXPECT_STREQ("c86e583bd41b1ea532e125711f960eaf", m_md5sum);
        }

        if (m_frameNo == 1995) {
            ComputeMd5Sum();
            EXPECT_STREQ("a6f479007ee88e1aade7786f157e62c2", m_md5sum);
        }

    }


}

TEST_F(CV2DFrameServerTest, GetSpecificFrames) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();


    m_frameNo = 1991;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("1c5c5c90810ea2a4fafd1f30e1086d1a", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(3000);

    m_frameNo = 1992;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("c86e583bd41b1ea532e125711f960eaf", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(3000);

    m_frameNo = 1993;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("c86e583bd41b1ea532e125711f960eaf", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(3000);

    m_frameNo = 1995;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("a6f479007ee88e1aade7786f157e62c2", m_md5sum);

    m_frameNo = 2008;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("a6f479007ee88e1aade7786f157e62c2", m_md5sum);

    m_frameNo = 127;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("ded3b79ba98b804661ac8f6d30ec4213", m_md5sum);

    m_frameNo = 0;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("5f363e0e58a95f06cbe9bbc662c5dfb6", m_md5sum);

}

TEST_F(CV2DFrameServerTest, GetLastFrame) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();

    m_frameNo = 11073;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ASSERT_TRUE(m_frameData != NULL);
    ComputeMd5Sum();
    EXPECT_STREQ("bf75083555fabeac794cf57a3efc183d", m_md5sum);
}

TEST_F(CV2DFrameServerTest, GetOutOfRangeFrame) {
    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize();

    m_frameNo = 11074;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ASSERT_TRUE(m_frameData == NULL);
}

TEST_F(CV2DFrameServerTest, GetScaledFrame) {

    CreateFrameServer("v2d_media/v2d_splash_gaps");
    m_pFrameServer->Initialize(1024, 768);
    EXPECT_EQ(1024, m_pFrameServer->GetFrameWidth());
    EXPECT_EQ(768, m_pFrameServer->GetFrameHeight());
    EXPECT_EQ(1280, m_pFrameServer->GetWidth());
    EXPECT_EQ(1024, m_pFrameServer->GetHeight());
    EXPECT_EQ(1024 * 768 * 4, m_pFrameServer->GetVideoFrameSize());


    m_frameNo = 1991;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("bec77e9d40865cd157ad8347392dbf3a", m_md5sum);

}


TEST_F(CV2DFrameServerTest, ResolutionChangeConsecutiveFrames) {
    CreateFrameServer("v2d_media/v2d_res_change");
    m_pFrameServer->Initialize();
    EXPECT_EQ(1024, m_pFrameServer->GetFrameWidth());
    EXPECT_EQ(768, m_pFrameServer->GetFrameHeight());
    EXPECT_EQ(1024 * 768 * 4, m_pFrameServer->GetVideoFrameSize());
    EXPECT_EQ(5703ULL, m_pFrameServer->GetVideoFrameCount());

    // We go through three change in resolutions
    // 1024x768 -> 1280x1024 -> 1024x768 -> 1920x1080
    for (m_frameNo = 350; m_frameNo < 630; m_frameNo++) {
        m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
        if (m_frameNo >= 356 && m_frameNo <= 401) {
            ComputeMd5Sum();
            EXPECT_STREQ("3b170e699036aba7a9396b164dc96306", m_md5sum);
        }
        if (m_frameNo >= 402 && m_frameNo <= 406) {
            ComputeMd5Sum();
            EXPECT_STREQ("814ff1a8872b8bdbeef9b9e1d9dafd76", m_md5sum);
        }
        if (m_frameNo == 407) {
            ComputeMd5Sum();
            EXPECT_STREQ("5f8c0ccf5a391044ada16d5e06332b1c", m_md5sum);
        }
        if (m_frameNo >= 455  && m_frameNo <= 491) {
            ComputeMd5Sum();
            EXPECT_STREQ("d3ed2a90e6aa4029726af2286cd50a84", m_md5sum);
        }
        if (m_frameNo == 492) {
            ComputeMd5Sum();
            EXPECT_STREQ("ddb7df166f0ac92e1206ac24b2dfcd94", m_md5sum);
        }
        if (m_frameNo >= 558 && m_frameNo <= 622) {
            ComputeMd5Sum();
            EXPECT_STREQ("3ca951671ffdeca8dc8343ba47ef56af", m_md5sum);
        }
        if (m_frameNo == 623) {
            ComputeMd5Sum();
            EXPECT_STREQ("5140ed2a46be1e5bad9dc0eb18456f49", m_md5sum);
        }

    }
}

TEST_F(CV2DFrameServerTest, ResolutionChangeSpecificFrames) {

    // Jump to specific frames before and after resolution change
    // and verify each frame is the same that we get when we
    // go consecutively
    CreateFrameServer("v2d_media/v2d_res_change");
    m_pFrameServer->Initialize();

    m_frameNo = 356;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("3b170e699036aba7a9396b164dc96306", m_md5sum);

    m_frameNo = 359;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("3b170e699036aba7a9396b164dc96306", m_md5sum);

    m_frameNo = 401;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("3b170e699036aba7a9396b164dc96306", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(200);

    m_frameNo = 402;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("814ff1a8872b8bdbeef9b9e1d9dafd76", m_md5sum);

    m_frameNo = 406;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("814ff1a8872b8bdbeef9b9e1d9dafd76", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(200);

    m_frameNo = 407;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("5f8c0ccf5a391044ada16d5e06332b1c", m_md5sum);

    m_frameNo = 455;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("d3ed2a90e6aa4029726af2286cd50a84", m_md5sum);

    m_frameNo = 491;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("d3ed2a90e6aa4029726af2286cd50a84", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(200);

    m_frameNo = 492;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("ddb7df166f0ac92e1206ac24b2dfcd94", m_md5sum);

    m_frameNo = 558;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("3ca951671ffdeca8dc8343ba47ef56af", m_md5sum);

    m_frameNo = 622;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("3ca951671ffdeca8dc8343ba47ef56af", m_md5sum);

    m_frameData = m_pFrameServer->GetVideoFrame(200);

    m_frameNo = 623;
    m_frameData = m_pFrameServer->GetVideoFrame(m_frameNo);
    ComputeMd5Sum();
    EXPECT_STREQ("5140ed2a46be1e5bad9dc0eb18456f49", m_md5sum);

}
