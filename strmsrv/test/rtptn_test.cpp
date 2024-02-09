/*
 * rtptn_test.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: durga
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>

#include "strmsrv_compatibility.h"

#include "FFMediaProcessor.hh"

using namespace std;

typedef struct md5SumInfo {
    std::string m_strPath;
    std::string m_strMD5Sum;
} md5SumInfo_t;

class RTPTNTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_pMediaProcessor = NULL;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/clips/rtp_media/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaMediaDir = "tmp_test_area/medianame/";
        GTestUtils::CreateDirectory(m_sTestAreaMediaDir);

    }

    virtual void TearDown() {
        delete m_pMediaProcessor;

        GTestUtils::DeleteDirectory(m_sTestAreaDir);
    }

    virtual void CreateMediaProcessor(string mediaDirectory, string destMediaDirectory, string strSDPFile) {
    	std::vector<int >trackSpeeds;
		trackSpeeds.push_back(8);
		trackSpeeds.push_back(64);
		trackSpeeds.push_back(256);
        m_pMediaProcessor = new CFFMediaProcessor(mediaDirectory, destMediaDirectory,
                trackSpeeds, "RTP", 0, false, strSDPFile);
    }

    virtual void CreateMediaProcessor(string mediaDirectory, string strSDPFile) {
        CreateMediaProcessor(mediaDirectory, m_sTestAreaMediaDir, strSDPFile);
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
protected:
    CFFMediaProcessor * m_pMediaProcessor;
    string m_sDataDir;
    string m_sTestAreaDir;
    string m_sTestAreaMediaDir;
};

static md5SumInfo_t md5InfoRTPPrimaryTN[] =
{
    {"tn.jpg", "c88e557e47d7e1ab16ebbb3b3b4d9912"},
    {"", ""}
};

TEST_F(RTPTNTest, CreatePrimaryThumbnail) {
    CreateMediaProcessor(m_sDataDir + "boeing", m_sDataDir + "boeing/boeing.sdp");

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }

    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTN);
}

TEST_F(RTPTNTest, CreatePrimaryThumbnailFromBufferSDP) {
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

	CreateMediaProcessor(m_sDataDir + "boeing", strTempFile);

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }
    CheckMD5Sums(m_sTestAreaMediaDir + "tn/", md5InfoRTPPrimaryTN);
}

//Missing video session parameters.
TEST_F(RTPTNTest, CreatePrimaryThumbnailWithWrongSDP) {
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

	CreateMediaProcessor(m_sDataDir + "boeing", strTempFile);

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }
}

//Wrong video codec type "H264-1234".
TEST_F(RTPTNTest, CreatePrimaryThumbnailWithWrongCodec) {
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

	CreateMediaProcessor(m_sDataDir + "boeing", strTempFile);

    m_pMediaProcessor->EnablePrimaryTN(true);
    m_pMediaProcessor->SetPrimaryTNResolution(640, 320);
    m_pMediaProcessor->EnableFFTracks(false);

    if (!m_pMediaProcessor->ProcessMedia()) {
        EXPECT_FALSE(true);
    }
}
