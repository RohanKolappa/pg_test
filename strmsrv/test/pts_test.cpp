/*
 * pts_test.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: durga
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "strmsrv_compatibility.h"
#include "PTSGenerator.hh"

class PTSTest : public testing::Test {
    virtual void SetUp() {
        pPTSHandler = new CPTSGenerator(1000, 90000);
    }

    virtual void TearDown() {
        delete pPTSHandler;
    }

protected:
    CPTSGenerator *pPTSHandler;
};

TEST_F(PTSTest, TestInitialPTSValue) {
    uint32_t nRTPTimestamp = 100;
    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(0ULL, nPTSVal);
}

TEST_F(PTSTest, TestSequentialPTSValues) {
    uint32_t nRTPTimestamp = 100;
    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(0ULL, nPTSVal);

    nRTPTimestamp = 110;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(10ULL, nPTSVal);

    nRTPTimestamp = 120;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(20ULL, nPTSVal);

    nRTPTimestamp = 130;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(30ULL, nPTSVal);

    nRTPTimestamp = 140;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(40ULL, nPTSVal);

    nRTPTimestamp = 150;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(50ULL, nPTSVal);

    nRTPTimestamp = 160;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(60ULL, nPTSVal);

    nRTPTimestamp = 170;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(70ULL, nPTSVal);

    nRTPTimestamp = 180;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(80ULL, nPTSVal);

    nRTPTimestamp = 190;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(90ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSValueWithSingleGap) {
    uint64_t nPTSVal = 0;

    uint32_t nRTPTimestamp = 100;
    for(; nRTPTimestamp <= 1500; nRTPTimestamp += 10) {
        nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    }
    EXPECT_EQ(1400ULL, nPTSVal);

    pPTSHandler->SetDiscontinuity();

    nRTPTimestamp = 20000;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(1400ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSWithMultipleGaps) {
    uint64_t nPTSVal = 0;

    uint32_t nRTPTimestamp = 1000;
    for(; nRTPTimestamp <= 25000; nRTPTimestamp += 10) {
        nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    }
    EXPECT_EQ(24000ULL, nPTSVal);

    pPTSHandler->SetDiscontinuity();

    nRTPTimestamp = 200000;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(24000ULL, nPTSVal);

    for(;nRTPTimestamp <= 230000; nRTPTimestamp += 10) {
        nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    }
    EXPECT_EQ(54000ULL, nPTSVal);

    pPTSHandler->SetDiscontinuity();

    nRTPTimestamp = 500000;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(54000ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSRoundAverage) {
    uint64_t nPTSVal = 0;

    uint32_t nRTPTimestamp = 100;
    for(; nRTPTimestamp <= 500; nRTPTimestamp += 10) {
        nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    }
    EXPECT_EQ(400ULL, nPTSVal);

    for(; nRTPTimestamp <= 550; nRTPTimestamp += 9) {
        nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    }
    EXPECT_EQ(446ULL, nPTSVal);

    pPTSHandler->SetDiscontinuity();

    nRTPTimestamp = 20000;
    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(446ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSValueWithInitialGap) {
    pPTSHandler->SetDiscontinuity();

    uint32_t nRTPTimestamp = 10000;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);

    EXPECT_EQ(0ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSRollover) {
    pPTSHandler->SetDiscontinuity();

    uint32_t nRTPTimestamp = 4294967280U;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);

    EXPECT_EQ(0ULL, nPTSVal);

    nRTPTimestamp += 10;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(10ULL, nPTSVal);

    nRTPTimestamp += 10;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp);
    EXPECT_EQ(20ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSWithSilence) {
    pPTSHandler->SetDiscontinuity();

    uint32_t nRTPTimestamp = 100U;
    uint64_t nWallClockTime = 1000;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                          nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nRTPTimestamp += 10;
    nWallClockTime += 12;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(10ULL, nPTSVal);

    nRTPTimestamp += 10;
    nWallClockTime += 12;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(20ULL, nPTSVal);

    nRTPTimestamp = 220;
    nWallClockTime = 1100;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(120ULL, nPTSVal);

    nRTPTimestamp = 450;
    nWallClockTime = 1330;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(350ULL, nPTSVal);
}

//Silence and then gap.
TEST_F(PTSTest, TestPTSWithWithSilenceFollowedByGap) {
    uint64_t nWallClockTime = 10;
    uint32_t nRTPTimestamp  = 900;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nWallClockTime = 20;
    nRTPTimestamp  = 1800;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(900ULL, nPTSVal);

    nWallClockTime = 200;
    nRTPTimestamp = 18000;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(17100ULL, nPTSVal);

    nWallClockTime = 500;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 200);//Gap from 200 to 500

    nWallClockTime = 500;
    nRTPTimestamp = 45000;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    //Silence = (500 - 200) * 90 = 27000; // (CurrentWallClock - PrevWallClock).
    //Gaps = (500 - 200) * 90 = 27000;
    //PTS = 17100 + 27000 - 27000 = 17100.// PTS += Silence - gaps.
    EXPECT_EQ(17100ULL, nPTSVal);
}

//Gap and then Silence
TEST_F(PTSTest, TestPTSWithWithGapFollowedBySilence) {
    uint64_t nWallClockTime = 10;
    uint32_t nRTPTimestamp  = 90;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nWallClockTime = 20;
    nRTPTimestamp  = 180;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(90ULL, nPTSVal);

    nWallClockTime = 200;
    nRTPTimestamp = 1800;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(1710ULL, nPTSVal);

    nWallClockTime = 500;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 200);//Gap from 200 to 500

    nWallClockTime = 1000;
    nRTPTimestamp = 90000;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    //Silence = (1000 - 200) * 90 = 72000; //(CurrentWallClock - PrevWallClock).
    //Gaps = ((500 - 200) * 90) = 27000; //(CurrentWallClock - DiscontinuityStartTime).
    //PTS = 1710 + 72000 - 27000 = 46710.// PTS += Silence - gaps.
    EXPECT_EQ(46710ULL, nPTSVal);
}

//Gaps in the Silence
TEST_F(PTSTest, TestPTSWithGapsInSilence) {
    uint64_t nWallClockTime = 10;
    uint32_t nRTPTimestamp  = 900;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nWallClockTime = 20;
    nRTPTimestamp  = 1800;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(900ULL, nPTSVal);

    nWallClockTime = 50;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 40);//Gap from 40 to 50 = 10

    nWallClockTime = 80;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 60);//Gap from 60 to 80 = 20

    nWallClockTime = 120;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 90);//Gap from 90 to 120 = 30

    nWallClockTime = 200;
    nRTPTimestamp = 30000;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    //Silence = (200 - 20) * 90 = 16200; //(CurrentWallClock - PrevWallClock).
    //Gap1 = (50 - 40) * 90 = 900;
    //Gap2 = (80 - 60) * 90 = 1800;
    //Gap3 = (120 - 90) * 90 = 2700;
    //Gaps = 900 + 1800 + 2700 = 5400;
    //PTS = 900 + 16200 - 5400 = 11700.// PTS += Silence - gaps.
    EXPECT_EQ(11700ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSWithDifferentSampleRate) {
    delete pPTSHandler;

    pPTSHandler = new CPTSGenerator(1, 1);

    uint64_t nWallClockTime = 10;
    uint32_t nRTPTimestamp  = 10;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                         nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nWallClockTime = 150;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 10);//Gap from 10 to 150 = 140

    nWallClockTime = 160;
    nRTPTimestamp  = 160;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(10ULL, nPTSVal);

    nWallClockTime = 1600;
    nRTPTimestamp  = 1600;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(1450ULL, nPTSVal);
}

TEST_F(PTSTest, TestPTSWithDifferentTicks) {
    delete pPTSHandler;

    pPTSHandler = new CPTSGenerator(10, 1);

    uint64_t nWallClockTime = 100;
    uint32_t nRTPTimestamp  = 10;

    uint64_t nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                         nWallClockTime);
    EXPECT_EQ(0ULL, nPTSVal);

    nWallClockTime = 1500;
    pPTSHandler->SetDiscontinuity(nWallClockTime - 100);//Gap from 10 to 150 = 140

    nWallClockTime = 1600;
    nRTPTimestamp  = 160;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(10ULL, nPTSVal);

    nWallClockTime = 16000;
    nRTPTimestamp  = 1600;

    nPTSVal = pPTSHandler->GetPTSForRTPTimestamp(nRTPTimestamp,
                                                 nWallClockTime);
    EXPECT_EQ(1450ULL, nPTSVal);
}
