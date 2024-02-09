/*
 * scapprocessor_test.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"

#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "ScapProcessor.hh"

class CScapProcessorTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_scapProcessor = new CScapProcessor();
        ResetScap();
    }

    virtual void TearDown() {
        delete m_scapProcessor;
    }

    virtual void StartScap() {
        if (m_cp != NULL)
            return;
        m_cp = (unsigned char *)m_pScapBuffer + 2;
        m_nLen += 2;
        *(unsigned short *) m_cp = htons(IDENT_SERVER_CAP);
        m_cp += 2;
        m_nLen += 2;
    }

    virtual void ResetScap() {
        memset(m_pScapBuffer, 0x00, 256);
        m_nLen = 0;
        m_cp = NULL;
    }

    virtual void UpdateScapLength(int pduLength) {
        m_nLen += pduLength;
        m_cp += pduLength;
        char * cp = m_pScapBuffer;
        *(unsigned short *)  cp = htons(m_nLen);
    }

    virtual void AddVideoChanged(bool bVideoChanged) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CHANGED, (int)bVideoChanged, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddVideoResolution(int width, int height) {
        StartScap();
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_HRES, width, m_cp);
        UpdateScapLength(8);
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_VRES, height, m_cp);
        UpdateScapLength(8);
    }

    virtual void AddOriginalVideoResolution(int width, int height) {
        StartScap();
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_HRES, width, m_cp);
        UpdateScapLength(8);
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_ORIG_VRES, height, m_cp);
        UpdateScapLength(8);
    }

    virtual void AddSliceSize(int sliceSize) {
        StartScap();
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_VIDEO_SLICESIZE, sliceSize, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddStereoMode(bool bStreoMode) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_STEREO, (int)bStreoMode, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddCompMode(bool bCompMode) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_COMPMODE, (int)bCompMode, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddMotionCompMode(bool bMotionCompMode) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_MOTIONCOMPMODE, (int)bMotionCompMode, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddCodecVersion(int nCodecVersion) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_VIDEO_CODEC_VERSION, nCodecVersion, m_cp);
        UpdateScapLength(6);
    }

    virtual void AddAudioType(int nAudioType) {
        StartScap();
        FILL_PROTOCOL_UNIT_SHORT(SCAP_IDENT_AUDIO_TYPE, nAudioType,m_cp);
        UpdateScapLength(6);
    }
	
    virtual void AddAudioFreq(long nAudiFreq) {
        StartScap();
        FILL_PROTOCOL_UNIT_LONG(SCAP_IDENT_AUDIO_FREQ, nAudiFreq, m_cp);
        UpdateScapLength(8);
    }

    CScapProcessor * m_scapProcessor;
    char m_pScapBuffer[256];
    int m_nLen;
    unsigned char * m_cp;
};

TEST_F(CScapProcessorTest, NoScapProcessedShowsZeroCount)
{
    EXPECT_EQ(0, (int)m_scapProcessor->GetScapProcessedCount());
}

TEST_F(CScapProcessorTest, NonScapPayloadReturnsFalse)
{
    bool result = m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(result);
}

TEST_F(CScapProcessorTest, NonScapPayloadShowsZeroCount)
{
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(0, (int)m_scapProcessor->GetScapProcessedCount());
}

TEST_F(CScapProcessorTest, CorrectScapPayloadReturnsTrue)
{
    AddVideoChanged(false);
    bool result = m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(result);
}

TEST_F(CScapProcessorTest, MultipleScapProcessedCountIsCorrect)
{
    AddVideoChanged(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, (int)m_scapProcessor->GetScapProcessedCount());

    m_scapProcessor->Process(m_pScapBuffer);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(3, (int)m_scapProcessor->GetScapProcessedCount());
}

TEST_F(CScapProcessorTest, VideoRateChangeDetectsResChange)
{
    AddVideoChanged(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    ResetScap();
    AddVideoChanged(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

    // Repeat the mode change signal and it should detect res change
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());
}

TEST_F(CScapProcessorTest, VideoResolutionChangeDetectsResChange)
{
    AddVideoResolution(1024, 768);
    AddSliceSize(32);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1024, m_scapProcessor->GetWidth());
    EXPECT_EQ(768, m_scapProcessor->GetHeight());
    EXPECT_EQ(1024, m_scapProcessor->GetOriginalWidth());
    EXPECT_EQ(768, m_scapProcessor->GetOriginalHeight());
    EXPECT_EQ(32, m_scapProcessor->GetSliceSize());
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

    // Process same scap
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1024, m_scapProcessor->GetWidth());
    EXPECT_EQ(768, m_scapProcessor->GetHeight());
    EXPECT_EQ(1024, m_scapProcessor->GetOriginalWidth());
    EXPECT_EQ(768, m_scapProcessor->GetOriginalHeight());
    EXPECT_EQ(32, m_scapProcessor->GetSliceSize());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    // Change resolution
    ResetScap();
    AddVideoResolution(1280, 768);
    AddSliceSize(30);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1280, m_scapProcessor->GetWidth());
    EXPECT_EQ(768, m_scapProcessor->GetHeight());
    EXPECT_EQ(1280, m_scapProcessor->GetOriginalWidth());
    EXPECT_EQ(768, m_scapProcessor->GetOriginalHeight());
    EXPECT_EQ(30, m_scapProcessor->GetSliceSize());
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

    // Process same scap
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1280, m_scapProcessor->GetWidth());
    EXPECT_EQ(768, m_scapProcessor->GetHeight());
    EXPECT_EQ(1280, m_scapProcessor->GetOriginalWidth());
    EXPECT_EQ(768, m_scapProcessor->GetOriginalHeight());
    EXPECT_EQ(30, m_scapProcessor->GetSliceSize());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    // Change resolution, same as previous but with different original resolution
    ResetScap();
    AddVideoResolution(1280, 768);
    AddOriginalVideoResolution(1272, 762);
    AddSliceSize(30);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1280, m_scapProcessor->GetWidth());
    EXPECT_EQ(768, m_scapProcessor->GetHeight());
    EXPECT_EQ(1272, m_scapProcessor->GetOriginalWidth());
    EXPECT_EQ(762, m_scapProcessor->GetOriginalHeight());
    EXPECT_EQ(30, m_scapProcessor->GetSliceSize());
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

}

TEST_F(CScapProcessorTest, SliceSizeChangeDetectsResChange)
{
    AddVideoResolution(1024, 768);
    AddSliceSize(32);
    m_scapProcessor->Process(m_pScapBuffer);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    ResetScap();
    AddVideoResolution(1024, 768);
    AddSliceSize(30);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());
}

TEST_F(CScapProcessorTest, CheckStereoMode)
{
    AddVideoResolution(1024, 768);
    AddStereoMode(0);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(0, m_scapProcessor->GetStereoMode());

    AddVideoResolution(1024, 768);
    AddStereoMode(1);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, m_scapProcessor->GetStereoMode());

}

TEST_F(CScapProcessorTest, MaxSliceNumber)
{
    AddVideoResolution(1280, 1024);
    AddSliceSize(32);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(640, m_scapProcessor->GetMaxSliceNumber());
}

TEST_F(CScapProcessorTest, MaxSliceNumberForSVCIsZero)
{
    AddCodecVersion(CODEC_VERSION_SVC);
    AddVideoResolution(1280, 1024);
    AddSliceSize(32);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(0, m_scapProcessor->GetMaxSliceNumber());
}

TEST_F(CScapProcessorTest, CodecVersionChangeDetectsResChange)
{
    AddCodecVersion(CODEC_VERSION_STANDARD);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());
    EXPECT_EQ(CODEC_VERSION_STANDARD, m_scapProcessor->GetCodecVersion());
    EXPECT_FALSE(m_scapProcessor->IsSVC());

    ResetScap();
    AddCodecVersion(CODEC_VERSION_SVC);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());
    EXPECT_EQ(CODEC_VERSION_SVC, m_scapProcessor->GetCodecVersion());
    EXPECT_TRUE(m_scapProcessor->IsSVC());

    // Repeat codec version change and it should not detect res change
    ResetScap();
    AddCodecVersion(CODEC_VERSION_SVC);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());
    EXPECT_EQ(CODEC_VERSION_SVC, m_scapProcessor->GetCodecVersion());

    // Not sending codec version should assume CODEC_VERSION_STANDARD
    // hence should detect res change
    ResetScap();
    AddVideoChanged(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());
    EXPECT_EQ(CODEC_VERSION_STANDARD, m_scapProcessor->GetCodecVersion());

}

TEST_F(CScapProcessorTest, CompModeChangeDetectsResChange)
{
    AddCompMode(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(0, m_scapProcessor->GetVideoCompMode());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    ResetScap();
    AddCompMode(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, m_scapProcessor->GetVideoCompMode());
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

    // Repeat last mode and it should not detect res change
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, m_scapProcessor->GetVideoCompMode());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());
}

TEST_F(CScapProcessorTest, MotionCompModeChangeDetectsResChange)
{
    AddMotionCompMode(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(0, m_scapProcessor->GetVideoMotionCompMode());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());

    ResetScap();
    AddMotionCompMode(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, m_scapProcessor->GetVideoMotionCompMode());
    EXPECT_TRUE(m_scapProcessor->ResolutionChangeDetected());

    // Repeat last mode and it should not detect res change
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(1, m_scapProcessor->GetVideoMotionCompMode());
    EXPECT_FALSE(m_scapProcessor->ResolutionChangeDetected());
}

TEST_F(CScapProcessorTest, CheckEncodeFormat)
{
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_SVC);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(eSVC, m_scapProcessor->GetEncodeFormat());

    ResetScap();
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_STANDARD);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(eYUV422P, m_scapProcessor->GetEncodeFormat());

    ResetScap();
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_STANDARD);
    AddCompMode(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(eYUV444S, m_scapProcessor->GetEncodeFormat());

    ResetScap();
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_STANDARD);
    AddCompMode(true);
    AddMotionCompMode(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(eYUV444D, m_scapProcessor->GetEncodeFormat());

    // Setting motion comp mode to true when comp mode is false is malformed scap
    // and should default to 422
    ResetScap();
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_STANDARD);
    AddCompMode(false);
    AddMotionCompMode(true);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_EQ(eYUV422P, m_scapProcessor->GetEncodeFormat());

}

TEST_F(CScapProcessorTest, GetCachedSCAP)
{
    AddVideoResolution(1024, 768);
    AddCodecVersion(CODEC_VERSION_STANDARD);
    AddSliceSize(30);
    AddCompMode(false);
    AddMotionCompMode(true);

    uint8_t buffer[256];
    bool result = m_scapProcessor->GetCachedSCAP(buffer);
    EXPECT_FALSE(result);

    m_scapProcessor->Process(m_pScapBuffer);
    result = m_scapProcessor->GetCachedSCAP(buffer);
    EXPECT_TRUE(result);
    EXPECT_EQ(0, memcmp(m_pScapBuffer, buffer, m_nLen));

    // Verify direct buffer access API
    EXPECT_EQ(0, memcmp(m_pScapBuffer,
            m_scapProcessor->GetCachedSCAPBuffer(),
            m_scapProcessor->GetCachedSCAPLength()));

}

TEST_F(CScapProcessorTest, GetAudioParams)
{
    AddAudioType(AUDIOTYPE_DEFAULT);
    m_scapProcessor->Process(m_pScapBuffer);
    int nChannels = m_scapProcessor->GetAudioChannels();
    EXPECT_EQ(2, nChannels);
    int nBitsPerSample = m_scapProcessor->GetAudioBitsPerSample();
    EXPECT_EQ(16, nBitsPerSample);

    ResetScap();
    AddAudioType(AUDIOTYPE_MONO);
    m_scapProcessor->Process(m_pScapBuffer);
    nChannels = m_scapProcessor->GetAudioChannels();
    EXPECT_EQ(1, nChannels);
    nBitsPerSample = m_scapProcessor->GetAudioBitsPerSample();
    EXPECT_EQ(16, nBitsPerSample);

    ResetScap();
    AddAudioType(AUDIOTYPE_8BIT);
    m_scapProcessor->Process(m_pScapBuffer);
    nChannels = m_scapProcessor->GetAudioChannels();
    EXPECT_EQ(2, nChannels);
    nBitsPerSample = m_scapProcessor->GetAudioBitsPerSample();
    EXPECT_EQ(8, nBitsPerSample);

    ResetScap();
    AddAudioType(AUDIOTYPE_MONO_8BIT);
    m_scapProcessor->Process(m_pScapBuffer);
    nChannels = m_scapProcessor->GetAudioChannels();
    EXPECT_EQ(1, nChannels);
    nBitsPerSample = m_scapProcessor->GetAudioBitsPerSample();
    EXPECT_EQ(8, nBitsPerSample);
}

TEST_F(CScapProcessorTest, AudioResolutionChangeDetected)
{
     
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->AudioResolutionChangeDetected());
    EXPECT_EQ(LEG_AUDIO_FREQ , m_scapProcessor->GetAudioFrequency());
  
    ResetScap();
    AddAudioFreq(NGS_AUDIO_FREQ);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->AudioResolutionChangeDetected());
    EXPECT_EQ(NGS_AUDIO_FREQ, m_scapProcessor->GetAudioFrequency());

    // Repeat Audio freq with 16000 and it should not detect res change
    ResetScap();
    AddAudioFreq(NGS_AUDIO_FREQ);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_FALSE(m_scapProcessor->AudioResolutionChangeDetected());
    EXPECT_EQ(NGS_AUDIO_FREQ, m_scapProcessor->GetAudioFrequency());

    ResetScap();
    AddVideoChanged(false);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->AudioResolutionChangeDetected());
    EXPECT_EQ(LEG_AUDIO_FREQ, m_scapProcessor->GetAudioFrequency());

    ResetScap();
    AddAudioFreq(2000);
    m_scapProcessor->Process(m_pScapBuffer);
    EXPECT_TRUE(m_scapProcessor->AudioResolutionChangeDetected());
    EXPECT_EQ(2000, m_scapProcessor->GetAudioFrequency());


}
