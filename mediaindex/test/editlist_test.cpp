/*
 * editlist_test.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "libmediaconverter.hh"
#include "EditList.hh"

using namespace mediaindex;

class EditListTest :
public testing::Test {
protected:

    virtual void SetUp() {
        m_pEditList = new CEditList();
    }

    virtual void TearDown() {
        delete m_pEditList;
    }

    virtual bool CheckOffsets(int index, uint64_t startOffset, uint64_t endOffset) {
        if (m_pEditList->StartOffset(index) != startOffset)
            return false;
        if (m_pEditList->EndOffset(index) != endOffset)
            return false;
        return true;
    }
    CEditList *m_pEditList;
};


TEST_F(EditListTest, EmptyStringFails) {
    EXPECT_FALSE(m_pEditList->Set(""));
}

TEST_F(EditListTest, SingleItemListIsOk) {
    EXPECT_TRUE(m_pEditList->Set("1:2"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
}

TEST_F(EditListTest, SpacesNotAllowed) {
    EXPECT_FALSE(m_pEditList->Set("1: 2"));
}

TEST_F(EditListTest, EntryWithoutColon) {
    EXPECT_FALSE(m_pEditList->Set("1"));
}

TEST_F(EditListTest, NoCharsAfterColon) {
    EXPECT_FALSE(m_pEditList->Set("1:"));
}

TEST_F(EditListTest, NonIntegerAfterColon) {
    EXPECT_FALSE(m_pEditList->Set("1:foo"));
}

TEST_F(EditListTest, NonIntegerBeforeColon) {
    EXPECT_FALSE(m_pEditList->Set("foo:2"));
}

TEST_F(EditListTest, EmptyStringBeforeColon) {
    EXPECT_FALSE(m_pEditList->Set(":2"));
}

TEST_F(EditListTest, NonIntegersBeforeAndAfterColon) {
    EXPECT_FALSE(m_pEditList->Set("foo:bar"));
}

TEST_F(EditListTest, TrailingCommaIsOk) {
    EXPECT_TRUE(m_pEditList->Set("1:2,"));
}

TEST_F(EditListTest, TwoItemListIsOk) {
    EXPECT_TRUE(m_pEditList->Set("1:2,3:4"));
    EXPECT_EQ(2, m_pEditList->GetItemCount());
}

TEST_F(EditListTest, MalformedSecondItems) {
    EXPECT_FALSE(m_pEditList->Set("1:2,3"));
    EXPECT_FALSE(m_pEditList->Set("1:2,3:bar"));
    EXPECT_FALSE(m_pEditList->Set("1:2,foo:4"));
    EXPECT_FALSE(m_pEditList->Set("1:2,:4"));
}

TEST_F(EditListTest, TrailingCommasFail) {
    EXPECT_FALSE(m_pEditList->Set("1:2,,"));
}

TEST_F(EditListTest, EmptyItemsFail) {
    EXPECT_FALSE(m_pEditList->Set("1:2,,3:4"));
}

TEST_F(EditListTest, ToLessThanFrom) {
    EXPECT_FALSE(m_pEditList->Set("10:5"));
    EXPECT_EQ(0, m_pEditList->GetItemCount());
}

TEST_F(EditListTest, ToEqualsFrom) {
    EXPECT_FALSE(m_pEditList->Set("10:10"));
    EXPECT_EQ(0, m_pEditList->GetItemCount());
}

TEST_F(EditListTest, SecondItemIncorrect) {
    EXPECT_TRUE(m_pEditList->Set("1:2,3:2"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
}

TEST_F(EditListTest, GetOffsets) {
    EXPECT_TRUE(m_pEditList->Set("1:2,3:4,5:6"));
    EXPECT_EQ(3, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 2));
    EXPECT_TRUE(CheckOffsets(1, 3, 4));
    EXPECT_TRUE(CheckOffsets(2, 5, 6));
}

TEST_F(EditListTest, ReverseOrder) {
    EXPECT_TRUE(m_pEditList->Set("5:6,3:4,1:2"));
    EXPECT_EQ(3, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 2));
    EXPECT_TRUE(CheckOffsets(1, 3, 4));
    EXPECT_TRUE(CheckOffsets(2, 5, 6));
}

TEST_F(EditListTest, SecondItemOverlapsBefore) {
    EXPECT_TRUE(m_pEditList->Set("3:5,1:4"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 5));
}

TEST_F(EditListTest, SecondItemOverlapsAfter) {
    EXPECT_TRUE(m_pEditList->Set("3:5,4:6"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 3, 6));
}

TEST_F(EditListTest, SecondItemSubsumesFirst) {
    EXPECT_TRUE(m_pEditList->Set("3:5,1:6"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 6));
}

TEST_F(EditListTest, ThirdItemOverlapsFirst) {
    EXPECT_TRUE(m_pEditList->Set("3:5,7:9,4:6"));
    EXPECT_EQ(2, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 3, 6));
    EXPECT_TRUE(CheckOffsets(1, 7, 9));
}

TEST_F(EditListTest, ThirdItemOverlapsFirstAndSecond) {
    EXPECT_TRUE(m_pEditList->Set("3:5,7:9,4:8"));
    EXPECT_EQ(1, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 3, 9));
}

TEST_F(EditListTest, MultipleOverlaps) {
    EXPECT_TRUE(m_pEditList->Set("1:2,3:5,7:9,10:12,13:15,9:11,14:17"));
    EXPECT_EQ(4, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 2));
    EXPECT_TRUE(CheckOffsets(1, 3, 5));
    EXPECT_TRUE(CheckOffsets(2, 7, 12));
    EXPECT_TRUE(CheckOffsets(3, 13, 17));
}

TEST_F(EditListTest, MultipleOverlapsUnsortedAndDups) {
    EXPECT_TRUE(m_pEditList->Set("9:11,7:9,1:2,13:15,3:5,14:17,3:5,10:12,15:16"));
    EXPECT_EQ(4, m_pEditList->GetItemCount());
    EXPECT_TRUE(CheckOffsets(0, 1, 2));
    EXPECT_TRUE(CheckOffsets(1, 3, 5));
    EXPECT_TRUE(CheckOffsets(2, 7, 12));
    EXPECT_TRUE(CheckOffsets(3, 13, 17));
}
