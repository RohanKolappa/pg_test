/*
 * mpegts_parser_test.cpp
 *
 *  Created on: Jul 25, 2013
 *      Author: durga
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "strmsrv_compatibility.h"
#include "MPEGTSPktParser.hh"
#include "sframe.hh"
#include "MediaServer.hh"


using namespace std;
using namespace mediaindex;

const int TS_PACKET_SIZE = 188;

class MPEGTSPktParserTest : public testing::Test {
protected:
    virtual void SetUp() {
        m_pMediaServer = NULL;

        m_bEndOfMedia = false;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/clips/mpegts/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);

        m_sTestAreaMediaDir = "tmp_test_area/medianame/";
        GTestUtils::CreateDirectory(m_sTestAreaMediaDir);
    }

    virtual void TearDown() {
        delete m_pMediaServer;
    }

    virtual void CreateMediaServer(std::string aMediaPath) {
        m_pMediaServer = CMediaServer::Create(aMediaPath.c_str());
        m_pMediaServer->Init();
        m_pMediaServer->Start(0);
    }

    CSFrame * GetNextNormalFrame() {
        CCFrame cframe;

        while (true) {
            int rc = m_pMediaServer->GetFrame(cframe);
            if (rc != MSRC_OK) {
                if (rc == MSRC_END_OF_MEDIA) {
                    m_bEndOfMedia = true;
                }
                break;
            }

            if (cframe.GetMessageType() == CCFrame::Normal) {
                CSFrame * sFrame = new CSFrame(
                        (const char *) cframe.GetData(),
                        cframe.GetLength(), cframe.GetMediaFlag(),
                        (timestamp_t) cframe.GetTimeStamp());
                return sFrame;
            }
        }
        return NULL;
    }

protected:
    CMPEGTSPktParser m_Parser;
    CMediaServer * m_pMediaServer;
    string m_sDataDir;
    string m_sTestAreaDir;
    string m_sTestAreaMediaDir;
    bool  m_bEndOfMedia;
};

TEST_F(MPEGTSPktParserTest, TestStreamID) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;

    bool bTestedVideo = false;
    bool bTestedAudio = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);
                if (m_Parser.IsItVideoStream()) {
                    unsigned char nVideoStreamID = m_Parser.GetStreamID();
                    EXPECT_EQ(224U, nVideoStreamID);
                    bTestedVideo = true;
                }
                if (m_Parser.IsItAudioStream()) {
                    unsigned char nAudioStreamID = m_Parser.GetStreamID();
                    EXPECT_EQ(192U, nAudioStreamID);
                    bTestedAudio = true;
                }
            }
        }

        if (bTestedVideo & bTestedAudio)
            bContinue = false;

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedVideo);
    EXPECT_TRUE(bTestedAudio);
}

TEST_F(MPEGTSPktParserTest, TestPESPacketLength) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;

    bool bTestedVideo = false;
    bool bTestedAudio = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);
                if (m_Parser.IsItVideoStream()) {
                    unsigned short nVideoPESPacketLen = m_Parser.GetPESPacketLength();
                    EXPECT_EQ(0U, nVideoPESPacketLen);
                    bTestedVideo = true;
                }
                if (m_Parser.IsItAudioStream()) {
                    unsigned short nAudioPESPacketLen = m_Parser.GetPESPacketLength();
                    EXPECT_EQ(1162U, nAudioPESPacketLen);
                    bTestedAudio = true;
                }
            }
        }

        if (bTestedVideo & bTestedAudio)
            bContinue = false;

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedVideo);
    EXPECT_TRUE(bTestedAudio);
}

TEST_F(MPEGTSPktParserTest, TestPCR) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    char *pData = NULL;
    bool bTestedPCR = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);
                if (m_Parser.IsPCRFieldPresent()) {
                    uint64_t nPCRVal = m_Parser.GetPCRValue();
                    EXPECT_EQ(3725914243ULL, nPCRVal);

                    bContinue = false;
                    bTestedPCR = true;
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedPCR);
}

TEST_F(MPEGTSPktParserTest, TestVideoPTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    char *pData = NULL;
    bool bTestedPTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsPTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725960001ULL, nPTSVal);

                        bContinue = false;
                        bTestedPTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedPTS);
}

TEST_F(MPEGTSPktParserTest, TestVideoDTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    char *pData = NULL;
    bool bTestedDTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsDTSPresent()) {
                        uint64_t nDTSVal = m_Parser.GetDTSValue();
                        EXPECT_EQ(3725966007ULL, nDTSVal);

                        bContinue = false;
                        bTestedDTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedDTS);
}

TEST_F(MPEGTSPktParserTest, TestVideoPTSAndDTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    char *pData = NULL;
    bool bTestedPTSAndDTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsPTSPresent() && m_Parser.IsDTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725975016ULL, nPTSVal);

                        uint64_t nDTSVal = m_Parser.GetDTSValue();
                        EXPECT_EQ(3725966007ULL, nDTSVal);

                        bContinue = false;
                        bTestedPTSAndDTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedPTSAndDTS);
}

TEST_F(MPEGTSPktParserTest, TestAudioPTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    char *pData = NULL;
    bool bTestedPTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItAudioStream()) {
                    if (m_Parser.IsPTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725917755ULL, nPTSVal);

                        bContinue = false;
                        bTestedPTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bTestedPTS);
}

TEST_F(MPEGTSPktParserTest, TestUpdatePCRValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;
    bool bUpdatedPCR = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket((unsigned char*)pData + i, TS_PACKET_SIZE);
                if (m_Parser.IsPCRFieldPresent()) {
                    uint64_t nPCRVal = m_Parser.GetPCRValue();
                    EXPECT_EQ(3725914243ULL, nPCRVal);

                    uint64_t nNewPCR = 3725000000ULL;
                    m_Parser.UpdatePCRValue(pData + i, nNewPCR);

                    CMPEGTSPktParser newParser;
                    newParser.ParsePacket(pData + i, TS_PACKET_SIZE);
                    EXPECT_EQ(nNewPCR, newParser.GetPCRValue());

                    bContinue = false;
                    bUpdatedPCR = true;
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bUpdatedPCR);
}

TEST_F(MPEGTSPktParserTest, TestUpdateVideoPTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;
    bool bUpdatedPTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsPTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725960001ULL, nPTSVal);

                        uint64_t nNewPTSVal = 20000ULL;
                        m_Parser.UpdatePTSValue(pData + i, nNewPTSVal);

                        CMPEGTSPktParser newParser;
                        newParser.ParsePacket(pData + i, TS_PACKET_SIZE);
                        EXPECT_EQ(nNewPTSVal, newParser.GetPTSValue());

                        bContinue = false;
                        bUpdatedPTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bUpdatedPTS);
}

TEST_F(MPEGTSPktParserTest, TestUpdateVideoDTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;
    bool bUpdatedDTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsDTSPresent()) {
                        uint64_t nDTSVal = m_Parser.GetDTSValue();
                        EXPECT_EQ(3725966007ULL, nDTSVal);

                        uint64_t nNewDTSVal = 45000ULL;
                        m_Parser.UpdateDTSValue(pData + i, nNewDTSVal);

                        CMPEGTSPktParser newParser;
                        newParser.ParsePacket(pData + i, TS_PACKET_SIZE);
                        EXPECT_EQ(nNewDTSVal, newParser.GetDTSValue());

                        bContinue = false;
                        bUpdatedDTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bUpdatedDTS);
}

TEST_F(MPEGTSPktParserTest, TestUpdateVideoPTSAndDTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;
    bool bUpdatedPTSAndDTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItVideoStream()) {
                    if (m_Parser.IsPTSPresent() && m_Parser.IsDTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725975016ULL, nPTSVal);

                        uint64_t nNewPTSVal = 35000ULL;
                        m_Parser.UpdatePTSValue(pData + i, nNewPTSVal);

                        uint64_t nDTSVal = m_Parser.GetDTSValue();
                        EXPECT_EQ(3725966007ULL, nDTSVal);

                        uint64_t nNewDTSVal = 33000ULL;
                        m_Parser.UpdateDTSValue(pData + i, nNewDTSVal);

                        CMPEGTSPktParser newParser;
                        newParser.ParsePacket(pData + i, TS_PACKET_SIZE);

                        EXPECT_EQ(nNewPTSVal, newParser.GetPTSValue());
                        EXPECT_EQ(nNewDTSVal, newParser.GetDTSValue());

                        bContinue = false;
                        bUpdatedPTSAndDTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bUpdatedPTSAndDTS);
}

TEST_F(MPEGTSPktParserTest, TestUpdateAudioPTSValue) {
    CreateMediaServer(m_sDataDir + "mpegts_with_gaps");

    CSFrame *pCSFrame = NULL;
    unsigned char *pData = NULL;
    bool bUpdatedPTS = false;

    bool bContinue = true;
    while(bContinue) {
        pCSFrame = GetNextNormalFrame();
        if (pCSFrame != NULL) {
            pData = (unsigned char*) pCSFrame->GetData();
            for(int i = 0; i < pCSFrame->GetLength(); i += TS_PACKET_SIZE) {
                m_Parser.ParsePacket(pData + i, TS_PACKET_SIZE);

                if (m_Parser.IsItAudioStream()) {
                    if (m_Parser.IsPTSPresent()) {
                        uint64_t nPTSVal = m_Parser.GetPTSValue();
                        EXPECT_EQ(3725917755ULL, nPTSVal);

                        uint64_t nNewPTSVal = 1725926699ULL;
                        m_Parser.UpdatePTSValue(pData + i, nNewPTSVal);

                        CMPEGTSPktParser newParser;
                        newParser.ParsePacket(pData + i, TS_PACKET_SIZE);
                        EXPECT_EQ(nNewPTSVal, newParser.GetPTSValue());

                        bContinue = false;
                        bUpdatedPTS = true;
                    }
                }
            }
        }

        if (m_bEndOfMedia)
            bContinue = false;
    }

    EXPECT_TRUE(bUpdatedPTS);
}
