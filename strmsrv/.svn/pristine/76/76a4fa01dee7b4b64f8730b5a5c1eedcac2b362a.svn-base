/*
 * advanced_pts_test.cpp
 *
 *  Created on: Aug 6, 2013
 *      Author: durga
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"

#include "strmsrv_compatibility.h"
#include "TimeStampAdjuster.hh"

class TSAdjusterTest : public testing::Test {
    virtual void SetUp() {
        pTSAdjuster = new CTimeStampAdjuster();
    }

    virtual void TearDown() {
        delete pTSAdjuster;
    }

protected:
    CTimeStampAdjuster *pTSAdjuster;
};

TEST_F(TSAdjusterTest, TestInitialTSValue) {
    uint32_t nInputTimestamp = 100;
    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(100ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestSequentialTSValues) {
    uint32_t nInputTimestamp = 100;
    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(100ULL, nAdjustedTSVal);

    nInputTimestamp = 110;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(110ULL, nAdjustedTSVal);

    nInputTimestamp = 120;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(120ULL, nAdjustedTSVal);

    nInputTimestamp = 130;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(130ULL, nAdjustedTSVal);

    nInputTimestamp = 140;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(140ULL, nAdjustedTSVal);

    nInputTimestamp = 150;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(150ULL, nAdjustedTSVal);

    nInputTimestamp = 160;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(160ULL, nAdjustedTSVal);

    nInputTimestamp = 170;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(170ULL, nAdjustedTSVal);

    nInputTimestamp = 180;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(180ULL, nAdjustedTSVal);

    nInputTimestamp = 190;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(190ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSWithDisorderValue) {
    uint64_t nAdjustedTSVal = 0;

    uint32_t nInputTimestamp = 1000;
    for(; nInputTimestamp <= 25000; nInputTimestamp += 1000) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nInputTimestamp, nAdjustedTSVal);
    }

    nInputTimestamp = 10000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    for(;nInputTimestamp <= 23000; nInputTimestamp += 1000) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nInputTimestamp, nAdjustedTSVal);
    }

    nInputTimestamp = 20700;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(20700ULL, nAdjustedTSVal);

    nInputTimestamp = 50000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(50000ULL, nAdjustedTSVal);

    nInputTimestamp = 50300;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(50300ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSWithImmediateDisorderValue) {
    uint64_t nAdjustedTSVal = 0;

    uint32_t nInputTimestamp = 10000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 8000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(8000ULL, nAdjustedTSVal);

    nInputTimestamp = 14000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(14000ULL, nAdjustedTSVal);

    nInputTimestamp = 19000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(19000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithSingleGap) {
    uint64_t nAdjustedTSVal = 0;

    uint32_t nInputTimestamp = 100;
    for(; nInputTimestamp <= 1500; nInputTimestamp += 10) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nInputTimestamp, nAdjustedTSVal);
    }

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 20000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(1500ULL, nAdjustedTSVal);

    nInputTimestamp = 25000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(6500ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithMultipleGaps) {
    uint64_t nAdjustedTSVal = 0;

    uint32_t nInputTimestamp = 35000;
    for(; nInputTimestamp <= 50000; nInputTimestamp += 1500) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nInputTimestamp, nAdjustedTSVal);
    }

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 80000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(50000ULL, nAdjustedTSVal);

    nInputTimestamp = 95000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(65000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 115000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(65000ULL, nAdjustedTSVal);

    nInputTimestamp = 135000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(85000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithInitialGap) {
    pTSAdjuster->SetDiscontinuity();

    uint32_t nInputTimestamp = 10000;

    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);

    EXPECT_EQ(10000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSWithDisorderAndMultipleGaps) {
    uint64_t nAdjustedTSVal = 0;

    uint32_t nInputTimestamp = 1000;
    for(; nInputTimestamp <= 25000; nInputTimestamp += 10) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nInputTimestamp, nAdjustedTSVal);
    }

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 200000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(25000ULL, nAdjustedTSVal);

    uint64_t nPrevTimestamp = nAdjustedTSVal;

    nInputTimestamp = 201000LL;
    for(;nInputTimestamp <= 230000; nInputTimestamp += 1000) {
        nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
        EXPECT_EQ(nPrevTimestamp + 1000, nAdjustedTSVal);
        nPrevTimestamp += 1000;
    }

    nInputTimestamp = 207000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(32000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 500000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(55000ULL, nAdjustedTSVal);

    nInputTimestamp = 503000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(58000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithDTS) {
    uint32_t nInputTimestamp = 10000;

    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 12000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(12000ULL, nAdjustedTSVal);

    nInputTimestamp = 11000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(11000ULL, nAdjustedTSVal);

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(18000ULL, nAdjustedTSVal);

    nInputTimestamp = 14000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(14000ULL, nAdjustedTSVal);

    nInputTimestamp = 22000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(22000ULL, nAdjustedTSVal);

    nInputTimestamp = 17000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(17000ULL, nAdjustedTSVal);

    nInputTimestamp = 20000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(20000ULL, nAdjustedTSVal);

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(18000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithInitialDTS) {
    uint32_t nInputTimestamp = 10000;

    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(18000ULL, nAdjustedTSVal);

    nInputTimestamp = 22000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(22000ULL, nAdjustedTSVal);

    nInputTimestamp = 17000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(17000ULL, nAdjustedTSVal);

    nInputTimestamp = 20000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(20000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithInitialGapAndNonMainStreamTS) {
    pTSAdjuster->SetDiscontinuity();

    uint32_t nInputTimestamp = 10000;
    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 22000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 17000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(5000ULL, nAdjustedTSVal);

    nInputTimestamp = 20000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(8000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithInitialGapAndNonMainStreamTS2) {
    pTSAdjuster->SetDiscontinuity();

    uint32_t nInputTimestamp = 10000;
    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 9000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(9000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 22000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(14000ULL, nAdjustedTSVal);
}

TEST_F(TSAdjusterTest, TestTSValueWithInitialGapAndNonMainStreamTS3) {
    pTSAdjuster->SetDiscontinuity();

    uint32_t nInputTimestamp = 10000;
    uint64_t nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    pTSAdjuster->SetDiscontinuity();

    nInputTimestamp = 18000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(10000ULL, nAdjustedTSVal);

    nInputTimestamp = 22000;
    nAdjustedTSVal = pTSAdjuster->GetAdjustedTS(nInputTimestamp);
    EXPECT_EQ(14000ULL, nAdjustedTSVal);
}
