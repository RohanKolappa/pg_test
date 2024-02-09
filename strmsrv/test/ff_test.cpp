/*
 * ff_test.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: rkale
 */



#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "strmsrv_compatibility.h"

#include "FFMediaProcessorErrorHandler.hh"
#include "FFMediaProcessor.hh"
#include "MediaServer.hh"
#include "FFUtils.hh"
#include <fstream>

#ifdef CreateDirectory
#undef CreateDirectory
#endif

using namespace std;
using namespace mediaindex;

typedef struct md5SumInfo {
    std::string m_strPath;
    std::string m_strMD5Sum;
} md5SumInfo_t;

class FFTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_pMediaProcessor = NULL;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/clips/v2d_media/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaMediaDir = "tmp_test_area/medianame/";
        GTestUtils::CreateDirectory(m_sTestAreaMediaDir);

        m_sStreamType = "V2D";
        m_sSDPFilePath = "";
        m_pMediaServer = NULL;
    }

    virtual void TearDown() {
        delete m_pMediaProcessor;
        delete m_pMediaServer;

        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }

    virtual void CreateMediaProcessor(string mediaDirectory, string destMediaDirectory) {
        srandom(1234);
        std::vector<int >trackSpeeds;
        trackSpeeds.push_back(8);
        trackSpeeds.push_back(64);
        trackSpeeds.push_back(256);
        m_pMediaProcessor = new CFFMediaProcessor(mediaDirectory, destMediaDirectory,
                trackSpeeds, m_sStreamType, 0, false, false, m_sSDPFilePath);
    }

    virtual void CreateMediaProcessor(string mediaDirectory) {
        CreateMediaProcessor(mediaDirectory, m_sTestAreaMediaDir);
    }

    virtual void CreateRealtimeMediaProcessor() {
        CreateMediaProcessor("", m_sTestAreaMediaDir);
    }

    virtual void CreateMediaServer(std::string aMediaPath) {
        m_pMediaServer = CMediaServer::Create(aMediaPath.c_str());
        m_pMediaServer->Init();
        m_pMediaServer->Start(0);
    }

    virtual void ProcessMediaServerFrames(int numberOfFramesToProcess) {
        CCFrame cframe;
        int normalFrameCount = 0;
        while (true) {
            int rc = m_pMediaServer->GetFrame(cframe);
            if (rc != MSRC_OK) {
                break;
            }
            if (cframe.GetMessageType() == CCFrame::Normal) {
                CSFrame * sFrame = new CSFrame(
                        (const char *) cframe.GetData(),
                        cframe.GetLength(), cframe.GetMediaFlag(),
                        (timestamp_t) cframe.GetTimeStamp());
                m_pMediaProcessor->PutFrame(sFrame);
                normalFrameCount++;
                delete sFrame;
            }
            else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
                m_pMediaProcessor->Start(cframe.GetTimeStamp());
            }
            else if (cframe.GetMessageType() == CCFrame::MediaStopped) {
                m_pMediaProcessor->Stop(cframe.GetTimeStamp());
            }
            if (normalFrameCount >= numberOfFramesToProcess)
                break;
        }
    }

    virtual void CheckMD5Sums(std::string basePath, md5SumInfo_t md5SumInfo[]) {
        char md5sum[40];
        for (int i = 0; ; i++) {
            if (md5SumInfo[i].m_strPath == "")
                break;
            GTestUtils::MD5SumFile(basePath + md5SumInfo[i].m_strPath, md5sum);
            //printf("Checking %s\n", md5SumInfo[i].m_strPath.c_str());
            EXPECT_STREQ(md5SumInfo[i].m_strMD5Sum.c_str(), md5sum);
        }
    }

    virtual void PrintMD5Sums(std::string basePath, md5SumInfo_t md5SumInfo[]) {
        char md5sum[40];
        printf("{\n");
        for (int i = 0; ; i++) {
            if (md5SumInfo[i].m_strPath == "")
                break;
            GTestUtils::MD5SumFile(basePath + md5SumInfo[i].m_strPath, md5sum);
            printf("    {");
            printf("\"%s\", \"%s\"", md5SumInfo[i].m_strPath.c_str(), md5sum);
            printf("},\n");
        }
        printf("    {\"\", \"\"}\n");
        printf("};\n");
    }

protected:
    CFFMediaProcessor * m_pMediaProcessor;
    CMediaServer * m_pMediaServer;
    string m_sDataDir;
    string m_sTestAreaDir;
    string m_sTestAreaMediaDir;
    string m_sStreamType;
    string m_sSDPFilePath;
    bool m_result;
};

static md5SumInfo_t md5InfoFF8[] =
{
    {"0", "f8bfb01e219f72d694fd788610f1f457"},
    {"0.idx", "680ee375332c210236349d045eb20149"},
    {"1", "2c365d9d94f119de5c04e3dd468851de"},
    {"1.idx", "f8d2fccd6a8eb75136ceeec505c434ef"},
    {"2", "f50b6bce85bbc61439c14eebc16ef478"},
    {"2.idx", "0430a8189980e7d7c2acc84e9aca3c82"},
    {"3", "b18841da19ba89fc40756296ee74334b"},
    {"3.idx", "e0e6e29b2b10798c4c91e3e1a225660e"},
    {"4", "a0b3f28bee1ab6905f431eeae2507447"},
    {"4.idx", "b67e6d9a54ae41fa00fd9dd501820364"},
    {"5", "2dadef801ae003d498d521400e904aaa"},
    {"5.idx", "1098ef02968ef821bb2c7ef0f38dc35e"},
    {"INDEX", "01f7a6282ee44ee12449444401af1b93"},
    {"", ""}
};
static md5SumInfo_t md5InfoFF64[] =
{
    {"0", "3ceefcdda99e098cef3d0d063bda945a"},
    {"0.idx", "bf559ed85af42a5265d9d586ad13ba1d"},
    {"1", "4ca6d6de881e2a20389fec9d65ee651c"},
    {"1.idx", "ff9a39fbd62988751da9e18e911de65e"},
    {"2", "8567519c4dcd873391809075af6d7dcd"},
    {"2.idx", "3426ebda996b361de903381d5e60d2e1"},
    {"3", "a27eb64a605f36d85359202cbc5f4eb3"},
    {"3.idx", "1417b260c9df7b689110c8ecd91f1257"},
    {"4", "56466eb718992af2099d293d4e6b3ff9"},
    {"4.idx", "765b92c09962b5499671e00cf930b6ec"},
    {"5", "6f301e4d01bfeb12854d5179ae670c21"},
    {"5.idx", "e9ea67dd69113a816265624686ec3789"},
    {"INDEX", "01f7a6282ee44ee12449444401af1b93"},
    {"", ""}
};
static md5SumInfo_t md5InfoFF256[] =
{
    {"0", "c6b050c5a8a61f0522bd7afed4d7a458"},
    {"0.idx", "e31b2321d5170cc9759e04a11ff39ec6"},
    {"1", "9c48ba367b077f9a03c872d556961421"},
    {"1.idx", "dbaaae8c703f0d673cfce57499efb67c"},
    {"2", "81f6f74cbe93c1b4dab2e8c0d2c7d81a"},
    {"2.idx", "9c272ad93634dce9a78bd88c60bbd56a"},
    {"3", "530916dba271ab435e54ce7e6fe0f9d0"},
    {"3.idx", "06c4cad0f4e1d825c444dba8c4a03a15"},
    {"4", "01add9d16debef2cbb40ea7bf75e4474"},
    {"4.idx", "e3a462e12d51def712853aed64ae4f45"},
    {"5", "1e6c5e8656aae9add110f8f228d898f4"},
    {"5.idx", "1fbbe98a4883262338d1237f31970db3"},
    {"INDEX", "01f7a6282ee44ee12449444401af1b93"},
    {"", ""}
};



static md5SumInfo_t md5InfoFF8Trimmed[] =
{
    {"2", "f50b6bce85bbc61439c14eebc16ef478"},
    {"2.idx", "0430a8189980e7d7c2acc84e9aca3c82"},
    {"3", "b18841da19ba89fc40756296ee74334b"},
    {"3.idx", "e0e6e29b2b10798c4c91e3e1a225660e"},
    {"4", "a0b3f28bee1ab6905f431eeae2507447"},
    {"4.idx", "b67e6d9a54ae41fa00fd9dd501820364"},
    {"5", "2dadef801ae003d498d521400e904aaa"},
    {"5.idx", "1098ef02968ef821bb2c7ef0f38dc35e"},
    {"INDEX", "b918021efa7038d31fe5484c985c0c8c"},
    {"", ""}
};
static md5SumInfo_t md5InfoFF64Trimmed[] =
{
    {"2", "8567519c4dcd873391809075af6d7dcd"},
    {"2.idx", "3426ebda996b361de903381d5e60d2e1"},
    {"3", "a27eb64a605f36d85359202cbc5f4eb3"},
    {"3.idx", "1417b260c9df7b689110c8ecd91f1257"},
    {"4", "56466eb718992af2099d293d4e6b3ff9"},
    {"4.idx", "765b92c09962b5499671e00cf930b6ec"},
    {"5", "6f301e4d01bfeb12854d5179ae670c21"},
    {"5.idx", "e9ea67dd69113a816265624686ec3789"},
    {"INDEX", "18832ff7bd182aa4fda3681e328a5d80"},
    {"", ""}
};
static md5SumInfo_t md5InfoFF256Trimmed[] =
{
    {"2", "81f6f74cbe93c1b4dab2e8c0d2c7d81a"},
    {"2.idx", "9c272ad93634dce9a78bd88c60bbd56a"},
    {"3", "530916dba271ab435e54ce7e6fe0f9d0"},
    {"3.idx", "06c4cad0f4e1d825c444dba8c4a03a15"},
    {"4", "01add9d16debef2cbb40ea7bf75e4474"},
    {"4.idx", "e3a462e12d51def712853aed64ae4f45"},
    {"5", "1e6c5e8656aae9add110f8f228d898f4"},
    {"5.idx", "1fbbe98a4883262338d1237f31970db3"},
    {"INDEX", "df85f1e9ef3b538d8d12a3f62a3a50ff"},
    {"", ""}
};


static md5SumInfo_t md5InfoPrimaryTN[] =
{
    {"tn.jpg", "c38b63daed3abaf1222cf3ea0d4779a8"},
    {"", ""}
};
static md5SumInfo_t md5InfoStreamTNs[] =
{
    {"1328801647105.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801705676.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801765677.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801825674.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801885670.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801945675.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"", ""}
};

static md5SumInfo_t md5Info1280x1024TN[] =
{
    {"tn.jpg", "40e9d007332fe5ec2f405b0e799221e2"},
    {"", ""}
};

static md5SumInfo_t md5InfoFinalTN[] =
{
    {"tn.jpg", "c38b63daed3abaf1222cf3ea0d4779a8"},
    {"", ""}
};

static md5SumInfo_t md5InfoFinalStreamTNs[] =
{
    {"1328801647105.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"1328801962223.jpg", "d02bff719c78c034abcfc87d5a27757d"},
    {"", ""}
};

static md5SumInfo_t md5InfoSnapshotTN[] =
{
    {"tn.jpg", "0a5d96faf490ccf9134016a7932dd219"},
    {"", ""}
};

static md5SumInfo_t md5InfoBmpSnapshotTN[] =
{
    {"tn.jpg", "d52325d33e9081280fce09b022469ff2"},
    {"", ""}
};

static md5SumInfo_t md5InfoStreamTNs256x256[] =
{
    {"1268674716518.jpg", "6663fe266f005fac3debc220b2f6bf46"},
    {"1268674774722.jpg", "6b8c27c6c9cd2411b06341d0351820cc"},
    {"1268674834715.jpg", "e3b4967fdb9315c63d79a88e4d4beda6"},
    {"1268674894712.jpg", "5726a176d30a2c9e210c13e4f778fcaf"},
    {"1268674954741.jpg", "6f8ed786b11ae69cb02c5b74b9a9866e"},
    {"1268675014661.jpg", "627f57e1107aac1c5a875cc704853ebc"},
    {"1268675074673.jpg", "815ff499c739709eb38ec15bd2e28db3"},
    {"", ""}
};

static md5SumInfo_t md5InfoRTPPrimaryTN[] =
{
    {"tn.jpg", "4d5ce98ef3eb59b2c1958298c372c02d"},
    {"", ""}
};

static md5SumInfo_t md5InfoAntricaPrimaryTN[] =
{
    {"tn.jpg", "d423c2903754ccfd99a5ff185e179d29"},
    {"", ""}
};

static md5SumInfo_t md5InfoMPEG4PrimaryTN[] =
{
    {"tn.jpg", "2b94198ebeb5b3c8fd7462c5b0c0a83c"},
    {"", ""}
};

static md5SumInfo_t md5InfoRTPPrimaryTNBeforeStop[] =
{
    {"tn.jpg", "c9513c94fe74d8b327d0cd3dbd2ff183"},
    {"", ""}
};

static md5SumInfo_t md5InfoRTPStreamTNs[] =
{
    {"1297892811000.jpg", "8a81e021c0c4ce8a5301cc9fd9cb04cc"},
    {"1297892825916.jpg", "6a3ebf9c98e097674a40c39c9516f153"},
    {"1297892840867.jpg", "079fee8b3ef9d430df7b132fc50c0d68"},
    {"1297892855819.jpg", "f85a630b6b5ed099b5e8448a9a2c6200"},
    {"1297892870732.jpg", "858d469964466ce199482069fdf85079"},
    {"1297892885682.jpg", "dfdce503c9e1bbde85a227072f2e1434"},
    {"1297892900597.jpg", "dcc6c4934f0aa165fbfe4295f40b6778"},
    {"1297892915554.jpg", "a6b96cab6e674f8494f900be1c68d5d0"},
    {"1297892930473.jpg", "7d975bfda2964069dd7c274e8369b790"},
    {"1297892945413.jpg", "ba4eafe32238d72d680f6ac975b49bdd"},
    {"1297892960329.jpg", "0d4d3f31762f7a0d18798f79dfeda5fb"},
    {"", ""}
};

static md5SumInfo_t md5InfoAntricaStreamTNs[] =
{
    {"1353469799283.jpg", "c61a9d94248fcea3ce9e9712841cd956"},
    {"1353469815282.jpg", "32bd7007de2bad9932fadc65f7940160"},
    {"1353469831282.jpg", "683635062b4de4d32a9bf500360136be"},
    {"1353469843283.jpg", "ff8e2503ae84c6b33b71e9688c760828"},
    {"1353469859281.jpg", "702fdc770ced03e468abaa2548a19403"},
    {"1353469875284.jpg", "fdb65ef526ed428021dbc479cca2f467"},
    {"1353469891282.jpg", "527d6dfe63ff12813aa05264d5b251c1"},
    {"1353469903281.jpg", "c19facd6a64a8a92665700180c3dd45c"},
    {"", ""}
};

static md5SumInfo_t md5InfoBlackTN[] =
{
    {"tn.jpg", "cf2f00b59f9de26aa8574d44a3fdb69c"},
    {"", ""}
};

static md5SumInfo_t md5InfoLiveTNs[] =
{
    {"tn_big.jpg", "80249c02f1cb10a7ea723205320fa92c"},
    {"tn_small.jpg", "e07b43d8150bed934c015a3246365ab6"},
    {"snapshot.jpg", "0571881146f94d2b15d7bfc2ea7a2ae9"},
    {"", ""}
};

static md5SumInfo_t md5InfoRTPPrimaryTNForGapsMedia[] =
{
    {"tn.jpg", "800149e4efc7dd88a97671c0c466537b"},
    {"", ""}
};

static md5SumInfo_t md5InfoBoeingMP4File[] =
{
    {"output.mp4", "543523cb8eeea99c266c04aed30019a5"},
    {"", ""}
};

static md5SumInfo_t md5InfoAntricaMP4File[] =
{
    {"output.mp4", "5767605a7f23c6758a85f56815176a01"},
    {"", ""}
};

static md5SumInfo_t md5InfoAntricaMP4FileWithGaps[] =
{
    {"output.mp4", "1b040972c77076b426b6174d5f7657cc"},
    {"", ""}
};

static md5SumInfo_t md5InfoNetviz1920x1080x30MP4File[] =
{
    {"output.mp4", "8748b8e78a3d6f5d295c9684f73f40b5"},
    {"", ""}
};

static md5SumInfo_t md5InfoNetviz1280x720x60MP4File[] =
{
    {"output.mp4", "65cc6fa43d987c6251cc4b9c46f300fb"},
    {"", ""}
};

static md5SumInfo_t md5InfoNetviz640x360x60MP4File[] =
{
    {"output.mp4", "eac805955cb560e15dad5409ca011087"},
    {"", ""}
};

static md5SumInfo_t md5InfoNetvizWithGapsMP4File[] =
{
    {"output.mp4", "bf869748eeb1c6f41ad093d714359900"},
    {"", ""}
};

static md5SumInfo_t md5InfoMP4File[] =
{
    {"output.mp4", "9013abea61fd4fcee599e256bca8db04"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisMP4File[] =
{
    {"output.mp4", "50936a9cda13892fd5227e3314339a9e"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisMP4File_Skip[] =
{
    {"output.mp4", "edfa34c50037c7ce0697e603fa566d5b"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisMP4File_Init_Silence[] =
{
    {"output.mp4", "c365f6e4afc865e13f57e085cb4d60d5"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisMP4File_Silence_Same_SSRC[] =
{
    {"output.mp4", "93c38c263743e40bc9e162d44415f8c5"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisMP4File_Reboot[] =
{
    {"output.mp4", "b1dd0e037af1669d170b02f55cc671ab"},
    {"", ""}
};

static md5SumInfo_t md5InfoAxisH264MP4File[] =
{
    {"output.mp4", "3ac133c008a6e12d3708ffbf1b6d9104"},
    {"", ""}
};

static md5SumInfo_t md5InfoAudioOnlyMP4File[] =
{
    {"output.mp4", "a7a04fd886daec3ef7fed6fe3df67f5e"},
    {"", ""}
};

static md5SumInfo_t md5InfoMPEGTSFile[] =
{
    {"output.ts", "8a7cbb1698f90d1fba71d6bf9c7565d6"},
    {"", ""}
};

static md5SumInfo_t md5InfoMPEGTSGapsFile[] =
{
    {"output.ts", "7c39e26ca9686ce79668b6076828d0ce"},
    {"", ""}
};

static md5SumInfo_t md5InfoStandardSnapshot[] =
{
    {"output.jpg", "66c36303ded60b22ac9ffd2bc4142a2b"},
    {"output_1.bmp", "a078ffa2ef397c0feaba1b437d8a6ef2"},
    {"output_2.png", "3bd8f6ad32bab1ae84f594d24007d36b"},
    {"", ""}
};

TEST_F(FFTest, NonExistingDestinationDirectory) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio", m_sTestAreaDir + "non_existing_dir");
    m_result = m_pMediaProcessor->ProcessMedia();
    EXPECT_FALSE(m_result);
    EXPECT_STREQ("Not a directory: tmp_test_area/non_existing_dir",
            m_pMediaProcessor->GetErrorString().c_str());
}


TEST_F(FFTest, BadDestinationDirectory) {
    GTestUtils::CreateFileFromBuffer(m_sTestAreaDir + "bad_dir", NULL, 0);
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio", m_sTestAreaDir + "bad_dir");
    m_result = m_pMediaProcessor->ProcessMedia();
    EXPECT_FALSE(m_result);
    EXPECT_STREQ("Not a directory: tmp_test_area/bad_dir",
            m_pMediaProcessor->GetErrorString().c_str());
}

TEST_F(FFTest, NonExistingMedia) {
    CreateMediaProcessor(m_sDataDir + "no_such_media");
    m_result = m_pMediaProcessor->ProcessMedia();
    EXPECT_FALSE(m_result);
    EXPECT_STREQ("MediaServer Init failed for ../avdata/video/clips/v2d_media/no_such_media",
            m_pMediaProcessor->GetErrorString().c_str());
}

TEST_F(FFTest, BasicWithThreeTracks) {
#ifndef WINDOWS
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");
    m_result = m_pMediaProcessor->ProcessMedia();
    EXPECT_TRUE(m_result);

    CheckMD5Sums(m_sTestAreaMediaDir + "ff_8/", md5InfoFF8);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_64/", md5InfoFF64);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_256/", md5InfoFF256);

    uint64_t bytesWritten = m_pMediaProcessor->GetBytesWritten();
    EXPECT_EQ(1426372ULL, bytesWritten);
#endif
}

TEST_F(FFTest, TrimLengthWithThreeTracks) {
#ifndef WINDOWS
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->SetTrimLength(180000);

    m_result = m_pMediaProcessor->ProcessMedia();
    EXPECT_TRUE(m_result);

    CheckMD5Sums(m_sTestAreaMediaDir + "ff_8/", md5InfoFF8Trimmed);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_64/", md5InfoFF64Trimmed);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_256/", md5InfoFF256Trimmed);

    uint64_t bytesWritten = m_pMediaProcessor->GetBytesWritten();
    EXPECT_EQ(1426372ULL, bytesWritten);

    uint64_t bytesDeleted = m_pMediaProcessor->GetBytesDeleted();
    EXPECT_EQ(547573ULL, bytesDeleted);
#endif
}

TEST_F(FFTest, CreateAllThumbnailsWithTracks) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetStreamTNResolution(160, 160);
    m_pMediaProcessor->SetStreamTNTimeInterval(60000);

    if (!m_pMediaProcessor->ProcessMedia()) {
    	EXPECT_FALSE(true);
    }

    CheckMD5Sums(m_sTestAreaMediaDir + "ff_8/", md5InfoFF8);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_64/", md5InfoFF64);
    CheckMD5Sums(m_sTestAreaMediaDir + "ff_256/", md5InfoFF256);

    uint64_t bytesWritten = m_pMediaProcessor->GetBytesWritten();
    EXPECT_EQ(1426372ULL, bytesWritten);

    EXPECT_EQ(8, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(19272ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoPrimaryTN);
    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoStreamTNs);
}

TEST_F(FFTest, CreateAllThumbnailsWithoutTracks) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetStreamTNResolution(160, 160);
    m_pMediaProcessor->SetStreamTNTimeInterval(60000);

    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(8, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(19272ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoPrimaryTN);
    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoStreamTNs);
}

TEST_F(FFTest, CreatePrimaryThumbnail) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(5657ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoPrimaryTN);
}

TEST_F(FFTest, CreateStreamThumbnails) {
    CreateMediaProcessor(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);

    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetStreamTNTimeInterval(60000);
    m_pMediaProcessor->SetStreamTNResolution(256, 256);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(7, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(21985ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoStreamTNs256x256);
}

TEST_F(FFTest, CreateHighResolutionPrimaryThumbnail) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(1400, 1280);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(34839ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5Info1280x1024TN);
}

TEST_F(FFTest, CreateThumbnailsBeforeStop) {
    CreateMediaProcessor(m_sDataDir + "v2d_with_audio");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetPrimaryTNOffset(3166660);
    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetStreamTNTimeInterval(3166660);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(3, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(9547ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoFinalTN);
    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoFinalStreamTNs);
}

TEST_F(FFTest, CreatePrimaryThumbnailForSnapshot) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/matroska/");

    m_sStreamType = "SNAPSHOT";

    CreateMediaProcessor(m_sDataDir + "snapshot2_blob");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(27922ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoSnapshotTN);
}

TEST_F(FFTest, CreatePrimaryThumbnailForBMPSnapshot) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/matroska/");

    m_sStreamType = "SNAPSHOT";

    CreateMediaProcessor(m_sDataDir + "snapshot3_blob");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(15676ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoBmpSnapshotTN);
}

TEST_F(FFTest, CreatePrimaryThumbnailForBMPSnapshotMalformed) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/matroska/");

    string strTmpBlob = m_sTestAreaDir + "snapshot3_blob-copy";
    GTestUtils::CreateDirectory(strTmpBlob);

    //Copying SSINDEX and bitmap into temporary directory.
    string srcIndex = m_sDataDir + "/snapshot3_blob/SSINDEX";
    string dstIndex = strTmpBlob + "/SSINDEX";

    std::ifstream fInIndex(srcIndex.c_str(), std::ifstream::in);
    std::ofstream fOutIndex(dstIndex.c_str(), std::ofstream::out);
    fOutIndex << fInIndex.rdbuf();
    fInIndex.close();
    fOutIndex.close();

    string srcBmp = m_sDataDir + "/snapshot3_blob/snapshot_0.bmp";
    string dstBmp = strTmpBlob + "/snapshot_0.bmp";

    GTestUtils::CreateTruncatedFile(srcBmp, dstBmp, 2000);

    m_sStreamType = "SNAPSHOT";

    CreateMediaProcessor(strTmpBlob);

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(0, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(0ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    EXPECT_EQ(9, CFFMediaProcessorErrorHandler::GetThumbnailCreationErrorForSnapshot());
}

TEST_F(FFTest, CreateRTPPrimaryThumbnail) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "boeing/boeing.sdp";

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(15321ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTN);
}

TEST_F(FFTest, CreateRTPPrimaryThumbnailBeforeStop) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "boeing/boeing.sdp";

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetPrimaryTNOffset(356789000);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(15094ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTNBeforeStop);
}

TEST_F(FFTest, CreateRTPPrimaryThumbnailFromBufferSDP) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    string strSDPBuffer = "v=0\n\n";
    strSDPBuffer += "o=- 1282670704600425 1282670704600425 IN IP4 10.51.51.11\n\n";
    strSDPBuffer += "s=Media Presentation\n\n";
    strSDPBuffer += "e=NONE\n\n";
    strSDPBuffer += "c=IN IP4 192.168.1.12\n\n";
    strSDPBuffer += "b=AS:240032\n\n";
    strSDPBuffer += "t=0 0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp?videocodec=h264\n\n";
    strSDPBuffer += "a=range:npt=0.000000-\n\n";
    strSDPBuffer += "m=video 6060 RTP/AVP 96\n\n";
    strSDPBuffer += "b=AS:240000\n\n";
    strSDPBuffer += "a=framerate:2147483647.0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=1?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:96 H264/90000\n\n";
    strSDPBuffer += "a=fmtp:96 packetization-mode=1; profile-level-id=420033;sprop-parameter-sets=Z0IAM+KQDwBE/LgLcBAQHh4kRUA=,aM48gA==\n\n";
    strSDPBuffer += "m=audio 6062 RTP/AVP 97\n\n";
    strSDPBuffer += "b=AS:32\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=2?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:97 mpeg4-generic/16000/1\n\n";
    strSDPBuffer += "a=fmtp:97 profile-level-id=15; mode=AAC-hbr;config=1408; SizeLength=13; IndexLength=3;IndexDeltaLength=3; Profile=1; bitrate=32000;\n\n";

    string strTempFile = m_sTestAreaDir + "boeing.sdp";

    if (!GTestUtils::CreateFileFromBuffer(strTempFile, (unsigned char*)strSDPBuffer.c_str(), strSDPBuffer.length())) {
        EXPECT_FALSE(true);
    }

    if (!GTestUtils::FileExists(strTempFile)) {
        EXPECT_FALSE(true);
    }

    m_sStreamType = "RTP";
    m_sSDPFilePath = strTempFile;

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(15321ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTN);
}

//Missing video session parameters.
TEST_F(FFTest, CreateRTPPrimaryThumbnailWithWrongSDP) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    string strSDPBuffer = "v=0\n\n";
    strSDPBuffer += "o=- 1282670704600425 1282670704600425 IN IP4 10.51.51.11\n\n";
    strSDPBuffer += "s=Media Presentation\n\n";
    strSDPBuffer += "e=NONE\n\n";
    strSDPBuffer += "c=IN IP4 192.168.1.12\n\n";
    strSDPBuffer += "b=AS:240032\n\n";
    strSDPBuffer += "t=0 0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp?videocodec=h264\n\n";
    strSDPBuffer += "a=range:npt=0.000000-\n\n";
    strSDPBuffer += "b=AS:240000\n\n";
    strSDPBuffer += "a=framerate:2147483647.0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=1?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:96 H264/90000\n\n";
    strSDPBuffer += "a=fmtp:96 packetization-mode=1; profile-level-id=420033;sprop-parameter-sets=Z0IAM+KQDwBE/LgLcBAQHh4kRUA=,aM48gA==\n\n";
    strSDPBuffer += "m=audio 6062 RTP/AVP 97\n\n";
    strSDPBuffer += "b=AS:32\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=2?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:97 mpeg4-generic/16000/1\n\n";
    strSDPBuffer += "a=fmtp:97 profile-level-id=15; mode=AAC-hbr;config=1408; SizeLength=13; IndexLength=3;IndexDeltaLength=3; Profile=1; bitrate=32000;\n\n";

    string strTempFile = m_sTestAreaDir + "boeing.sdp";

    if (!GTestUtils::CreateFileFromBuffer(strTempFile, (unsigned char*)strSDPBuffer.c_str(), strSDPBuffer.length())) {
        EXPECT_FALSE(true);
    }

    if (!GTestUtils::FileExists(strTempFile)) {
        EXPECT_FALSE(true);
    }

    m_sStreamType = "RTP";
    m_sSDPFilePath = strTempFile;

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
}

//Wrong video codec type "H264-1234".
TEST_F(FFTest, CreateRTPPrimaryThumbnailWithWrongCodec) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    string strSDPBuffer = "v=0\n\n";
    strSDPBuffer += "o=- 1282670704600425 1282670704600425 IN IP4 10.51.51.11\n\n";
    strSDPBuffer += "s=Media Presentation\n\n";
    strSDPBuffer += "e=NONE\n\n";
    strSDPBuffer += "c=IN IP4 192.168.1.12\n\n";
    strSDPBuffer += "b=AS:240032\n\n";
    strSDPBuffer += "t=0 0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp?videocodec=h264\n\n";
    strSDPBuffer += "a=range:npt=0.000000-\n\n";
    strSDPBuffer += "m=video 6060 RTP/AVP 96\n\n";
    strSDPBuffer += "b=AS:240000\n\n";
    strSDPBuffer += "a=framerate:2147483647.0\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=1?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:96 H264-1234/90000\n\n";
    strSDPBuffer += "a=fmtp:96 packetization-mode=1; profile-level-id=420033;sprop-parameter-sets=Z0IAM+KQDwBE/LgLcBAQHh4kRUA=,aM48gA==\n\n";
    strSDPBuffer += "m=audio 6062 RTP/AVP 97\n\n";
    strSDPBuffer += "b=AS:32\n\n";
    strSDPBuffer += "a=control:rtsp://10.1.5.155/axis-media/media.amp/trackID=2?videocodec=h264\n\n";
    strSDPBuffer += "a=rtpmap:97 mpeg4-generic/16000/1\n\n";
    strSDPBuffer += "a=fmtp:97 profile-level-id=15; mode=AAC-hbr;config=1408; SizeLength=13; IndexLength=3;IndexDeltaLength=3; Profile=1; bitrate=32000;\n\n";

    string strTempFile = m_sTestAreaDir + "boeing.sdp";

    if (!GTestUtils::CreateFileFromBuffer(strTempFile, (unsigned char*)strSDPBuffer.c_str(), strSDPBuffer.length())) {
        EXPECT_FALSE(true);
    }

    if (!GTestUtils::FileExists(strTempFile)) {
        EXPECT_FALSE(true);
    }

    m_sStreamType = "RTP";
    m_sSDPFilePath = strTempFile;

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
}

TEST_F(FFTest, CreateMPEG4PrimaryThumbnail) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "mpeg4/mpeg4.sdp";

    CreateMediaProcessor(m_sDataDir + "mpeg4");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(18113ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoMPEG4PrimaryTN);
}

TEST_F(FFTest, CreateAntricaPrimaryThumbnail) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "antrica/antrica.sdp";

    CreateMediaProcessor(m_sDataDir + "antrica");

    //Using default Primary resolution.
    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(16668ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoAntricaPrimaryTN);
}

TEST_F(FFTest, CreateRTPStreamThumbnails) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "boeing/boeing.sdp";

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(12, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(33715ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPStreamTNs);
}

TEST_F(FFTest, CreateAntricaStreamThumbnails) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "antrica/antrica.sdp";

    CreateMediaProcessor(m_sDataDir + "antrica");

    m_pMediaProcessor->EnableStreamTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(9, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(30146ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoAntricaStreamTNs);
}

TEST_F(FFTest, CreateBlackThumbnail) {
	CreateMediaProcessor(m_sDataDir + "v2d_silence");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(2667ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoBlackTN);
}

TEST_F(FFTest, CreateLiveThumbnails) {
    CreateMediaProcessor(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->EnableStreamTN(false);
    m_pMediaProcessor->EnableLiveTN(true, true, true, true);

    m_pMediaProcessor->SetStreamTNTimeInterval(10000);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(117, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(1618228ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoLiveTNs);
}

TEST_F(FFTest, CreateNoLiveThumbnailsRealtime) {
    CreateRealtimeMediaProcessor();
    CreateMediaServer(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->EnableStreamTN(false);
    m_pMediaProcessor->EnableLiveTN(true, false, false, false);

    m_pMediaProcessor->SetStreamTNTimeInterval(2000);

    m_pMediaProcessor->InitMedia();

    ProcessMediaServerFrames(1000);

    EXPECT_EQ(0, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(0ULL, m_pMediaProcessor->GetBytesWrittenForTN());
}

TEST_F(FFTest, CreateLiveThumbnailsRealtime) {
    CreateRealtimeMediaProcessor();
    CreateMediaServer(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->EnableStreamTN(false);
    m_pMediaProcessor->EnableLiveTN(true, true, true, true);

    m_pMediaProcessor->SetStreamTNTimeInterval(2000);

    m_pMediaProcessor->InitMedia();

    ProcessMediaServerFrames(100);

    EXPECT_EQ(6, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(56244ULL, m_pMediaProcessor->GetBytesWrittenForTN());
}

TEST_F(FFTest, CreateSmallLiveThumbnailsRealtime) {
    CreateRealtimeMediaProcessor();
    CreateMediaServer(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->EnableStreamTN(false);
    m_pMediaProcessor->EnableLiveTN(true, false, false, true);

    m_pMediaProcessor->SetStreamTNTimeInterval(2000);

    m_pMediaProcessor->InitMedia();

    ProcessMediaServerFrames(100);

    EXPECT_EQ(2, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(2762ULL, m_pMediaProcessor->GetBytesWrittenForTN());
}

TEST_F(FFTest, ChangeLiveThumbnailsParametersRealtime) {
    CreateRealtimeMediaProcessor();
    CreateMediaServer(m_sDataDir + "v2d_splash_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->EnableStreamTN(false);
    m_pMediaProcessor->SetStreamTNTimeInterval(2000);

    // To start with, enable only small thumbnails
    m_pMediaProcessor->EnableLiveTN(true, false, false, true);

    m_pMediaProcessor->InitMedia();

    ProcessMediaServerFrames(100);
    EXPECT_EQ(2, m_pMediaProcessor->GetTotalThumbnailCount());
    EXPECT_EQ(2762ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    // After processing some frames, enable large thumbnails too
    m_pMediaProcessor->EnableLiveTN(true, false, true, true);
    ProcessMediaServerFrames(100);
    EXPECT_EQ(4, m_pMediaProcessor->GetTotalThumbnailCount()); // 2 more created
    EXPECT_EQ(10622ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    // After processing some more frames, enable only native thumbnails
    m_pMediaProcessor->EnableLiveTN(true, true, false, false);
    ProcessMediaServerFrames(100);
    EXPECT_EQ(5, m_pMediaProcessor->GetTotalThumbnailCount()); // 1 more created
    EXPECT_EQ(44109ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    // After processing some more frames, disable all thumbnails
    m_pMediaProcessor->EnableLiveTN(true, false, false, false);
    ProcessMediaServerFrames(1000);
    EXPECT_EQ(5, m_pMediaProcessor->GetTotalThumbnailCount()); // no more created
    EXPECT_EQ(44109ULL, m_pMediaProcessor->GetBytesWrittenForTN());
}

TEST_F(FFTest, CreatePrimaryThumbnailWithGapsMedia) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "netviz_with_gaps/netviz.sdp";

    CreateMediaProcessor(m_sDataDir + "netviz_with_gaps");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTNForGapsMedia);
}

TEST_F(FFTest, CreateSnapshotNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/matroska/");

    m_sStreamType = "SNAPSHOT";

    CreateMediaProcessor(m_sDataDir + "snapshot1_blob");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(3995765ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoStandardSnapshot);
}

TEST_F(FFTest, CreateBoeingNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "boeing/boeing.sdp";

    CreateMediaProcessor(m_sDataDir + "boeing");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }


    EXPECT_EQ(36172554ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoBoeingMP4File);
}

TEST_F(FFTest, CreateAntricaNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "antrica/antrica.sdp";

    CreateMediaProcessor(m_sDataDir + "antrica");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }


    EXPECT_EQ(76252575ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAntricaMP4File);
}

TEST_F(FFTest, CreateAntricaNativeOutputWithGaps) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "antrica_with_gaps/antrica.sdp";

    CreateMediaProcessor(m_sDataDir + "antrica_with_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(21897827ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAntricaMP4FileWithGaps);
}

TEST_F(FFTest, CreateNetviz1920x1080x30NativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "netviz_1920x1080x30/netviz.sdp";

    CreateMediaProcessor(m_sDataDir + "netviz_1920x1080x30");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(18408966ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoNetviz1920x1080x30MP4File);
}

TEST_F(FFTest, CreateNetviz1280x720x60FpsNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "netviz_1280x720x60/netviz.sdp";

    CreateMediaProcessor(m_sDataDir + "netviz_1280x720x60");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(9704935ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoNetviz1280x720x60MP4File);
}

TEST_F(FFTest, CreateNetviz640x360x60FpsNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "netviz_640x360x60/netviz_low.sdp";

    CreateMediaProcessor(m_sDataDir + "netviz_640x360x60");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(4558653ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoNetviz640x360x60MP4File);
}

TEST_F(FFTest, CreateNetvizNativeOutputWithGaps) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "netviz_with_gaps/netviz.sdp";

    CreateMediaProcessor(m_sDataDir + "netviz_with_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(18662985ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoNetvizWithGapsMP4File);
}

TEST_F(FFTest, CreateMpeg4NativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "mpeg4/mpeg4.sdp";

    CreateMediaProcessor(m_sDataDir + "mpeg4");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(80413886ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoMP4File);
}

TEST_F(FFTest, CreateAxisNativeOutputWithGaps) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_640x480x30_with_gaps/mpeg4.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_640x480x30_with_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(18156166ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisMP4File);
}

TEST_F(FFTest, CreateAxisH264NativeOutputWithGaps) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_h264_with_gaps/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_h264_with_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(8817091ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisH264MP4File);
}

TEST_F(FFTest, CreateAxisNativeOutputWithReboot) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_reboot/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_reboot");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(13159938ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisMP4File_Reboot);
}

TEST_F(FFTest, CreateAxisNativeOutputWithSkipBackFwd) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_skip_back_fwd/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_skip_back_fwd");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(16593913ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisMP4File_Skip);
}

TEST_F(FFTest, CreateAxisAudioOnlyNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "audioonly/audioonly.sdp";

    CreateMediaProcessor(m_sDataDir + "audioonly");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }


    EXPECT_EQ(697721ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAudioOnlyMP4File);
}

TEST_F(FFTest, CreateMpegTSNativeOutput) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/mpegts/");

    m_sStreamType = "MPEGTS";

    CreateMediaProcessor(m_sDataDir + "mpegts_1min");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }


    EXPECT_EQ(16486848ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoMPEGTSFile);
}

TEST_F(FFTest, CreateMpegTSNativeOutputWithGaps) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/mpegts/");

    m_sStreamType = "MPEGTS";

    CreateMediaProcessor(m_sDataDir + "mpegts_with_gaps");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(23151072ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoMPEGTSGapsFile);
}

//It doesn't create any mp4 file, because media contains only silence.
TEST_F(FFTest, CreateAxisNativeOutputWithOnlySilence) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_dummy/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_dummy");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(0ULL, m_pMediaProcessor->GetBytesWrittenForTN());
}

TEST_F(FFTest, CreateAxisNativeOutputWithInitialSilence) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_initial_silence/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_initial_silence");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(1580415ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisMP4File_Init_Silence);
}

TEST_F(FFTest, CreateAxisNativeOutputWithSilenceAndSameSSRC) {
    m_sDataDir = GTestUtils::GetDataDir("..");
    m_sDataDir.append("/video/clips/rtp_media/");

    m_sStreamType = "RTP";
    m_sSDPFilePath = m_sDataDir + "axis_silence_same_ssrc/axis.sdp";

    CreateMediaProcessor(m_sDataDir + "axis_silence_same_ssrc");

    m_pMediaProcessor->EnableFFTracks(false);
    m_pMediaProcessor->SetNativeOutputFileName(m_sTestAreaMediaDir + "output");
    m_pMediaProcessor->SetNativeOutputMode();

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    EXPECT_EQ(10897973ULL, m_pMediaProcessor->GetBytesWrittenForTN());

    CheckMD5Sums(m_sTestAreaMediaDir, md5InfoAxisMP4File_Silence_Same_SSRC);
}

