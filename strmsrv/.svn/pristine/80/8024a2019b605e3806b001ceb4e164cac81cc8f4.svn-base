/*
 * idprovider_test.cpp
 *
 *  Created on: Apr 4, 2013
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "IdProvider.hh"
#include <gtest/gtest.h>

using namespace std;

// The fixture for testing class CQueue.
class CIdProviderTest : public testing::Test {
protected:

    virtual void SetUp() {
        m_pIdProvider = new CIdProvider(1000);
    }

    virtual void TearDown() {
        delete m_pIdProvider;
    }

protected:

    CIdProvider * m_pIdProvider;
    int m_id;
};

TEST_F(CIdProviderTest, GetFirst) {
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
}

TEST_F(CIdProviderTest, GetSeries) {
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1001, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1002, m_id);
}


TEST_F(CIdProviderTest, ReleaseFirst) {
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
    m_pIdProvider->ReleaseId(1000);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
}

TEST_F(CIdProviderTest, TestHoles) {
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1001, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1002, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1003, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1004, m_id);
    m_pIdProvider->ReleaseId(1002);
    m_pIdProvider->ReleaseId(1000);
    m_pIdProvider->ReleaseId(1004);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1000, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1002, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1004, m_id);
    m_id = m_pIdProvider->GetId();
    EXPECT_EQ(1005, m_id);
}

