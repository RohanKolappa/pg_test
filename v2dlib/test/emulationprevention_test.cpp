/*
 * emulationprevention_test.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: User
 */

#include "v2dlib_compatibility.h"
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"
#include <gtest/gtest.h>
#include "EmulationPrevention.hh"
#include "layerUtils.hh"


class EmulationPreventionTest : public testing::Test {
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    void SetEmu01Bytes(uint8_t *a_buff, uint32_t byteoffset)
    {
        *(a_buff + byteoffset)     = 0x00;
        *(a_buff + byteoffset + 1) = 0x00;
        *(a_buff + byteoffset + 2) = 0x01;
    }

    void SetEmu03Bytes(uint8_t *a_buff, uint32_t byteoffset)
    {
        *(a_buff + byteoffset)     = 0x00;
        *(a_buff + byteoffset + 1) = 0x00;
        *(a_buff + byteoffset + 2) = 0x03;
    }

    bool  checkLocationEmu(uint8_t *a_buff)
    {
        if (*(a_buff) == 0x00) {
            if (*(a_buff + 1) == 0x00) {
                if (*(a_buff + 2) == 0x03) {
                    return true;
                }
            }
        }
        else {
            return false;
        }
        return false;
    }

};


TEST_F(EmulationPreventionTest, SliceLengthTooShort) {
    uint8_t sliceBuffer[64];
    ::memset(sliceBuffer, 0x00, 2);
    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 2);
    EXPECT_EQ(-1, rc);
}

TEST_F(EmulationPreventionTest, SliceLengthBoundaryCheck) {
    uint8_t sliceBuffer[64];
    ::memset(sliceBuffer, 0x00, 2);
    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 6);
    EXPECT_EQ(-1, rc);
}

TEST_F(EmulationPreventionTest, WithSliceLayer1) {
    uint8_t sliceBuffer[64];
    ::memset(sliceBuffer, 0x00, 7);

    CLayerUtils::SetLayersize((sliceBuffer + 4), 7);
    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 7);
    EXPECT_EQ(0, rc);
}

TEST_F(EmulationPreventionTest, WithSliceLayer12) {
    uint8_t sliceBuffer[64];
    ::memset(sliceBuffer, 0x00, 11);

    CLayerUtils::SetLayersize(sliceBuffer + 4, 3);
    CLayerUtils::SetLayersize(sliceBuffer + 7, 4);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 11);
    EXPECT_EQ(0, rc);
}

TEST_F(EmulationPreventionTest, WithSliceLayer123) {
    uint8_t sliceBuffer[64];
    ::memset(sliceBuffer, 0x00, 13);

    CLayerUtils::SetLayersize(sliceBuffer + 4, 3);
    CLayerUtils::SetLayersize(sliceBuffer + 7, 4);
    CLayerUtils::SetLayersize(sliceBuffer + 11, 3);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 13);
    EXPECT_EQ(0, rc);
}

TEST_F(EmulationPreventionTest, EmulationinLayer1) {

    int Lsize;
    uint8_t sliceBuffer[64];

    ::memset(sliceBuffer, 0x00, 65);

    CLayerUtils::SetLayersize(sliceBuffer + 4, 59);
    SetEmu01Bytes(sliceBuffer, 21);
    SetEmu03Bytes(sliceBuffer, 55);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);

    EXPECT_EQ(2, rc);
    Lsize = CLayerUtils::GetLayerSize(sliceBuffer + 4);
    EXPECT_EQ(61, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 21);
    EXPECT_TRUE(pos);

    pos = checkLocationEmu(sliceBuffer + 56);
    EXPECT_TRUE(pos);
}


TEST_F(EmulationPreventionTest, EmulationinLayer2) {
    int Lsize;
    uint8_t *pdata;
    uint8_t sliceBuffer[65];

    ::memset(sliceBuffer, 0x00, 65);

    pdata = sliceBuffer + 4;
    CLayerUtils::SetLayersize(pdata, 4);
    pdata += 4;
    CLayerUtils::SetLayersize(pdata, 32);
    pdata += 32;
    CLayerUtils::SetLayersize(pdata, 23);

    SetEmu01Bytes(sliceBuffer, 17);
    SetEmu03Bytes(sliceBuffer, 32);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);
    EXPECT_EQ(2, rc);

    pdata = sliceBuffer + 4;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(4, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(34, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(23, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 17);
    EXPECT_TRUE(pos);

    pos = checkLocationEmu(sliceBuffer + 33);
    EXPECT_TRUE(pos);

}

TEST_F(EmulationPreventionTest, EmulationinLayer3) {
    int Lsize;
    uint8_t *pdata;
    uint8_t sliceBuffer[65];

    ::memset(sliceBuffer, 0x00, 65);

    pdata = sliceBuffer + 4;
    CLayerUtils::SetLayersize(pdata, 4);
    pdata += 4;
    CLayerUtils::SetLayersize(pdata, 32);
    pdata += 32;
    CLayerUtils::SetLayersize(pdata, 23);

    SetEmu03Bytes(sliceBuffer, 56);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);
    EXPECT_EQ(1, rc);

    pdata = sliceBuffer + 4;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(4, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(32, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(24, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 56);
    EXPECT_TRUE(pos);


}

TEST_F(EmulationPreventionTest, EmulationinAllLayers) {
    int Lsize;
    uint8_t *sliceBuffer = new uint8_t[634];
    uint8_t sliceData[628] =
            { 0x00, 0x00, 0x01, 0xB2, 0x80, 0x3B, 0x00, 0x00, 0x34, 0xD2, 0x77,
                0x7D, 0xDF, 0x67, 0xCF, 0x2C, 0x64, 0xA8, 0x1A, 0x44, 0x92,
                0x3B, 0x1F, 0x3B, 0xFA, 0xCF, 0x79, 0xF0, 0xEF, 0x00, 0x00,
                0x03, 0x47, 0x5E, 0xFC, 0x8C, 0xEC, 0x02, 0x9F, 0x1B, 0xBF,
                0xB8, 0xE4, 0xF7, 0xFD, 0x5F, 0x31, 0xA2, 0xF1, 0x5B, 0x9E,
                0xFD, 0x00, 0x00, 0x01, 0x0E, 0xDD, 0x08, 0xB5, 0x22, 0x19,
                0x12, 0x48, 0x81, 0x56, 0x3D, 0xFA, 0xD1, 0x02, 0x05, 0xF6,
                0x5D, 0x82, 0xB3, 0x5B, 0xAE, 0x32, 0x8B, 0xEB, 0x35, 0xD5,
                0xEB, 0x63, 0x75, 0xB2, 0xE5, 0x36, 0x66, 0xC6, 0x79, 0x33,
                0x69, 0xCD, 0xEC, 0x73, 0x6D, 0x4A, 0xB0, 0xA4, 0x6F, 0x82,
                0x63, 0x0C, 0x6F, 0x82, 0x63, 0x3C, 0xF6, 0xF8, 0x26, 0x34,
                0x39, 0xBE, 0x09, 0x8C, 0xF3, 0xDB, 0xE0, 0x98, 0xD0, 0xE6,
                0x00, 0x00, 0x01, 0x97, 0xBB, 0xCF, 0x7D, 0xA7, 0x12, 0xD6,
                0xF7, 0xFE, 0x7C, 0x3B, 0xB8, 0xEE, 0x80, 0x1E, 0x79, 0xC4,
                0x8D, 0xCB, 0xC5, 0x0B, 0x92, 0x00, 0xE0, 0xF6, 0xE3, 0xF3,
                0x8B, 0x44, 0xF9, 0x56, 0xEF, 0xFD, 0xF4, 0x23, 0x8B, 0xD2,
                0x39, 0x17, 0x44, 0xBF, 0x61, 0x8D, 0x80, 0x0E, 0xFE, 0xE8,
                0xE7, 0x41, 0x37, 0x0C, 0x61, 0x24, 0xF2, 0x06, 0x80, 0x59,
                0xAE, 0x76, 0xB5, 0xF6, 0x14, 0xB7, 0x7C, 0x8F, 0x6F, 0xA9,
                0x00, 0x00, 0x03, 0x90, 0x0B, 0x73, 0x5F, 0x85, 0x37, 0x04,
                0xD8, 0x5B, 0x7D, 0x66, 0x90, 0x00, 0x00, 0x03, 0xDD, 0xF6,
                0xC4, 0x8D, 0xA7, 0x31, 0x17, 0xA0, 0x0B, 0x7E, 0x3B, 0xE2,
                0xBE, 0x70, 0xB1, 0xB8, 0x29, 0xF8, 0x5A, 0x31, 0x17, 0x8C,
                0x17, 0xED, 0xCF, 0x0F, 0x79, 0x3A, 0x56, 0xD3, 0x98, 0x8C,
                0x00, 0xE7, 0xB2, 0x2E, 0xFB, 0xE4, 0x35, 0xC0, 0x07, 0xCE,
                0x60, 0x07, 0xBF, 0x1F, 0x61, 0x04, 0x61, 0x57, 0x08, 0x9B,
                0xED, 0xDF, 0x75, 0x80, 0x1F, 0x70, 0x2B, 0xA0, 0x06, 0x8A,
                0xEB, 0x8F, 0xDE, 0x5F, 0xC3, 0x7C, 0x6B, 0xDE, 0x2F, 0x9C,
                0x20, 0xE7, 0x79, 0xC3, 0x4D, 0xE3, 0xA0, 0x09, 0xBD, 0x85,
                0x64, 0x8E, 0x6E, 0x44, 0xFD, 0xF5, 0xD6, 0x76, 0x27, 0xBE,
                0x72, 0xB0, 0x8F, 0xEB, 0xFB, 0xEC, 0x02, 0xAE, 0x7A, 0xC5,
                0x8D, 0x9C, 0x08, 0xF3, 0x9B, 0x9C, 0x3A, 0x58, 0xDF, 0x4C,
                0x22, 0xBB, 0x50, 0x9D, 0xBD, 0x0A, 0x6E, 0x4D, 0x31, 0x6E,
                0x63, 0x08, 0xF7, 0x27, 0x8B, 0x00, 0x00, 0x01, 0x80, 0x1F,
                0xAA, 0xB3, 0x91, 0x46, 0xB7, 0x4F, 0x22, 0xDB, 0xC1, 0x3D,
                0xFF, 0xCD, 0xCD, 0x67, 0xC8, 0x62, 0xA2, 0x2B, 0x08, 0xC2,
                0x33, 0xFE, 0x4D, 0x11, 0xF2, 0x6C, 0xE0, 0xD6, 0xC3, 0xD9,
                0xC4, 0x7E, 0xF6, 0x80, 0xE7, 0x5A, 0xE4, 0x1C, 0xDD, 0x50,
                0xF6, 0x00, 0x00, 0x03, 0x6B, 0x56, 0xE0, 0x9A, 0xD5, 0xBA,
                0x26, 0xB5, 0x6E, 0x89, 0xAD, 0x4E, 0x6E, 0x89, 0xAD, 0x5B,
                0xA2, 0x6B, 0x53, 0x98, 0x80, 0xD7, 0x93, 0x50, 0x79, 0x91,
                0x58, 0xA8, 0x7D, 0x2C, 0xE0, 0xB6, 0x59, 0x00, 0x00, 0x03,
                0x60, 0xD8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD, 0x0B,
                0x16, 0x34, 0x02, 0x20, 0xD8, 0xC5, 0x80, 0x53, 0x54, 0x02,
                0x2D, 0x8C, 0x90, 0x72, 0xBF, 0x00, 0x00, 0x01, 0x4A, 0x2D,
                0x54, 0xD6, 0x28, 0x3A, 0x10, 0xE5, 0x68, 0xEE, 0xB8, 0x30,
                0x57, 0x49, 0x00, 0x00, 0x01, 0x2F, 0x55, 0x7F, 0x00, 0x00,
                0x03, 0xD6, 0x8B, 0xE2, 0x34, 0xB6, 0x4B, 0x4E, 0x0B, 0x0A,
                0x66, 0x2A, 0x1E, 0x7C, 0x44, 0x0B, 0x61, 0xEB, 0xE8, 0xC7,
                0x45, 0xB2, 0x9B, 0x0B, 0x28, 0x3C, 0x07, 0x88, 0x16, 0xCC,
                0x58, 0x41, 0x65, 0x86, 0x0F, 0x66, 0x5A, 0x39, 0x2A, 0xB3,
                0x6D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x2A, 0x87, 0xA9,
                0xC8, 0x90, 0xF6, 0x29, 0x5E, 0x4C, 0x1E, 0x1F, 0x7F, 0x63,
                0x07, 0x34, 0x58, 0xA0, 0x38, 0x77, 0x01, 0xB2, 0x1F, 0xBA,
                0x70, 0xEC, 0x8C, 0x38, 0xF0, 0xA6, 0x74, 0xC0, 0xF4, 0x90,
                0xB0, 0x1E, 0xAE, 0xD5, 0xAA, 0x6E, 0xDB, 0xA1, 0xAC, 0x7E,
                0x9B, 0x18, 0x3D, 0xD4, 0xDC, 0x02, 0xBC, 0x4F, 0x61, 0x85,
                0xB1, 0x4F, 0x43, 0x7D, 0x24, 0xA7, 0x8F, 0x18, 0xC3, 0x35,
                0x7C, 0x6C, 0x60, 0xE0, 0x0B, 0x0C, 0x52, 0x87, 0xFA, 0xB0,
                0xE6, 0x01, 0xC5, 0xD5, 0xA1, 0x03, 0x02, 0x5B, 0xF4, 0xD7,
                0x38, 0x44, 0x43, 0x4A, 0x00, 0x00, 0x03, 0x06, 0x30, 0xD6,
                0x1A, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };

    memcpy(sliceBuffer, sliceData, 620);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 620);
    EXPECT_EQ(14, rc);

    uint8_t *tempbuff = sliceBuffer;

    Lsize = CLayerUtils::GetLayerSize(tempbuff + 4);
    EXPECT_EQ(61, Lsize);

    tempbuff += 4 + Lsize;
    Lsize = CLayerUtils::GetLayerSize(tempbuff);
    EXPECT_EQ(347, Lsize);

    tempbuff += Lsize;
    Lsize = CLayerUtils::GetLayerSize(tempbuff);
    EXPECT_EQ(222, Lsize);

    char *md5sum = new char [40];
    GTestUtils::MD5Sum(sliceBuffer , 634, md5sum);
    EXPECT_STREQ("be029eaeb96c52de0016b89249a5ac9a", (const char *)md5sum);

}

TEST_F(EmulationPreventionTest, EmulationAtEndofSlice) {

    int Lsize;
    uint8_t sliceBuffer[65];

    ::memset(sliceBuffer, 0x00, 65);

    CLayerUtils::SetLayersize(sliceBuffer + 4, 59);
    SetEmu01Bytes(sliceBuffer, 60);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);

    EXPECT_EQ(1, rc);
    Lsize = CLayerUtils::GetLayerSize(sliceBuffer + 4);
    EXPECT_EQ(60, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 60);
    EXPECT_TRUE(pos);

}

TEST_F(EmulationPreventionTest, EmulationAtEndofLayer2) {

    int Lsize;
    uint8_t *pdata;
    uint8_t sliceBuffer[65];

    ::memset(sliceBuffer, 0x00, 65);

    pdata = sliceBuffer + 4;
    CLayerUtils::SetLayersize(pdata, 4);
    pdata += 4;
    CLayerUtils::SetLayersize(pdata, 32);
    pdata += 32;
    CLayerUtils::SetLayersize(pdata, 23);

    SetEmu01Bytes(sliceBuffer, 17);
    SetEmu03Bytes(sliceBuffer, 29);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);
    EXPECT_EQ(2, rc);

    pdata = sliceBuffer + 4;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(4, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(34, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(23, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 17);
    EXPECT_TRUE(pos);

    pos = checkLocationEmu(sliceBuffer + 30);
    EXPECT_TRUE(pos);
}

TEST_F(EmulationPreventionTest, EmulationAtEndofLayer3) {
    int Lsize;
    uint8_t *pdata;
    uint8_t sliceBuffer[65];

    ::memset(sliceBuffer, 0x00, 65);

    pdata = sliceBuffer + 4;
    CLayerUtils::SetLayersize(pdata, 4);
    pdata += 4;
    CLayerUtils::SetLayersize(pdata, 32);
    pdata += 32;
    CLayerUtils::SetLayersize(pdata, 23);

    SetEmu03Bytes(sliceBuffer, 60);

    int rc = CEmulationPrevention::EmulateSlice(sliceBuffer, 63);
    EXPECT_EQ(1, rc);

    pdata = sliceBuffer + 4;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(4, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(32, Lsize);

    pdata += Lsize;
    Lsize = CLayerUtils::GetLayerSize(pdata);
    EXPECT_EQ(24, Lsize);

    bool pos = checkLocationEmu(sliceBuffer + 60);
    EXPECT_TRUE(pos);

}


