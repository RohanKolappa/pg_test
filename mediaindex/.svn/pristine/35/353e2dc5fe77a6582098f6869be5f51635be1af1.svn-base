/*
 * create_test.cpp
 *
 *  Created on: Jan 22, 2011
 *      Author: rkale
 */

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"
#include "libmediaconverter.hh"
#include "MatroskaFileWriter.hh"
#include "EditList.hh"

using namespace mediaindex;


class CreateTest :
public testing::Test {
protected:

    virtual void SetUp() {
        m_pMatroskaWriter = NULL;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/matroska/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::CreateDirectory(m_sTestAreaDir);
    }

    virtual void TearDown() {
        delete m_pMatroskaWriter;

        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }


protected:
    CMatroskaFileWriter *m_pMatroskaWriter;
    string m_sDataDir;
    string m_sTestAreaDir;
};


// Create full matroska file using both blob and metadata
TEST_F(CreateTest, CreateFromBlobAndDb) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2.mkd",
                m_sDataDir + "clip2_db",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "clip2.mkd", md5sum);
        EXPECT_STREQ("ded041c671a83fbff8a31c3dd75192e9", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file using only minute files form blob dir
TEST_F(CreateTest, CreateFromBlob) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2.mkd",
                "",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "clip2.mkd", md5sum);
        EXPECT_STREQ("93d14cc9d8c4c2f408f6e5066171507e", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file from a given cutlist using both blob and metadata
TEST_F(CreateTest, CreateEditListFromBlobAndDb) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2_cutlist.mkd",
                m_sDataDir + "clip2_db",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        CEditList * pEditList = new CEditList();
        pEditList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");

        m_pMatroskaWriter->SetEditList(pEditList);

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        delete pEditList;

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "clip2_cutlist.mkd", md5sum);
        EXPECT_STREQ("519b60d1bd12fb4a1daa7ead102844b5", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file from a given cutlist using only minute files form blob dir
TEST_F(CreateTest, CreateEditListFromBlob) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2_cutlist.mkd",
                "",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        CEditList * pEditList = new CEditList();
        pEditList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");

        m_pMatroskaWriter->SetEditList(pEditList);

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        delete pEditList;

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "clip2_cutlist.mkd", md5sum);
        EXPECT_STREQ("89d93883e39c342cf28e5ab54c03892c", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file from a given cutlist using both blob and metadata with gaps.
TEST_F(CreateTest, CreateEditListFromBlobHavingGaps) {
    try {
        string sDataDir = GTestUtils::GetDataDir("..");
        sDataDir.append("/video/clips/v2d_media/");

        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "v2d_media_gaps.mkd",
                sDataDir + "v2d_splash_gaps",
                sDataDir + "v2d_splash_gaps");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        CEditList * pEditList = new CEditList();
        pEditList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,150000:160000,190000:210000");

        m_pMatroskaWriter->SetEditList(pEditList);

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        delete pEditList;

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "v2d_media_gaps.mkd", md5sum);
        EXPECT_STREQ("a52bae98cfb229c551955a1eae9997d6", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file from a blob with initial gaps.
TEST_F(CreateTest, CreateFromBlobWithInitialGap) {
    try {
        string sDataDir = GTestUtils::GetDataDir("..");
        sDataDir.append("/video/clips/v2d_media/");

        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "v2d_splash_initial_gap.mkd",
                "",
                sDataDir + "v2d_splash_with_initial_gap");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "v2d_splash_initial_gap.mkd", md5sum);
        EXPECT_STREQ("9a9ca1490db87cb991cc04bd01b38e53", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Create matroska file from a blob with spurious frame.
TEST_F(CreateTest, CreateFromBlobWithSpuriousFrame) {
    try {
        string sDataDir = GTestUtils::GetDataDir("..");
        sDataDir.append("/video/clips/v2d_media/");

        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "v2d_with_spurious_frame.mkd",
                "",
                sDataDir + "v2d_with_spurious_frame");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "v2d_with_spurious_frame.mkd", md5sum);
        EXPECT_STREQ("b3d93c42e15e20cf268ef3c35a865e80", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// Use non-existing blob dir and ensure we get an error code
TEST_F(CreateTest, MissingBlobDir) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2.mkd",
                "",
                m_sDataDir + "blah_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_ERROR, rc);
    }
    catch(...) {
        EXPECT_FALSE(true);
    }
}

// Use non-existing DB dir and ensure we get an error code
TEST_F(CreateTest, MissingDbDir) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "clip2.mkd",
                m_sDataDir + "blah_db",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_ERROR, rc);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

// If output file cannot be created (bad path), we should get an error code
TEST_F(CreateTest, NoPathToOutputFile) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "blah/blah.mkd",
                m_sDataDir + "clip2_db",
                m_sDataDir + "clip2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_ERROR, rc);

    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

TEST_F(CreateTest, CreateFromBlobWithSnapshot) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "snapshot1.mkd",
                "",
                m_sDataDir + "snapshot1_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "snapshot1.mkd", md5sum);
        EXPECT_STREQ("c6f13f0b4bbd85a9d7e4d2009731520a", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

TEST_F(CreateTest, CreateFromBlobWithThumbnails) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "thumbnails1.mkd",
                "",
                m_sDataDir + "thumbnails1_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "thumbnails1.mkd", md5sum);

#ifdef WINDOWS
        //We are sending cue/track information in the same order in both windows
       // and linux. But Matroska library is internally writing cue data in 
       //different order somehow.
       //If there are multiple tracks at the same time stamp, matroska is
       // writing track data in different order in cue information section.
        EXPECT_STREQ("4660d6f507a4df71d25e703584e0b932", md5sum);
#else
        EXPECT_STREQ("90766504c27433cd2941c444838f310a", md5sum);
#endif

    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

TEST_F(CreateTest, CreateEditListFromBlobWithThumbnails) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "thumbnails1.mkd",
                "",
                m_sDataDir + "thumbnails1_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        CEditList * pEditList = new CEditList();
        pEditList->Set("30000:101000,290000:330000");

        m_pMatroskaWriter->SetEditList(pEditList);

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        delete pEditList;

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "thumbnails1.mkd", md5sum);
        EXPECT_STREQ("bf984ca9063c875a7451fcefd27d7495", md5sum);

    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}

TEST_F(CreateTest, CreateFromBlobAndDBWithSnapshot) {
    try {
        m_pMatroskaWriter = new CMatroskaFileWriter(
                m_sTestAreaDir + "snapshot2.mkd",
                m_sDataDir + "snapshot2_db",
                m_sDataDir + "snapshot2_blob");
        m_pMatroskaWriter->Silent();
        m_pMatroskaWriter->FixedUUIDs();

        int rc = m_pMatroskaWriter->Write();
        EXPECT_EQ(ELIBMC_OK, rc);

        char md5sum[40];
        GTestUtils::MD5SumFile(m_sTestAreaDir + "snapshot2.mkd", md5sum);
        EXPECT_STREQ("5e764f1534cf303806e7a1c8ee2822c7", md5sum);
    }
    catch (...) {
        EXPECT_FALSE(true);
    }
}
