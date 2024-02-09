/*
 * extrac_test.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: rkale
 */


#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "gtest_extra_utils.hh"
#include "libmediaconverter.hh"
#include "MinuteFileWriter.hh"
#include "EditList.hh"
#include "MediaProcessorInterface.hh"

using namespace mediaindex;

using namespace std;

typedef struct md5SumInfo {
    std::string m_strPath;
    std::string m_strMD5Sum;
} md5SumInfo_t;


class CMediaProcessorInterfaceDummyImpl: public CMediaProcessorInterface {
    public:
        CMediaProcessorInterfaceDummyImpl() {
            m_nTotalProcessedFrameCount = 0;
            m_nTotalStartFrameCount = 0;
            m_nTotalStopFrameCount = 0;
        }

        virtual ~CMediaProcessorInterfaceDummyImpl() {};

        virtual void StartMedia(uint64_t a_nStartTime) { m_nTotalStartFrameCount++; }
        virtual void StopMedia(uint64_t a_nStopTime) { m_nTotalStopFrameCount++; }
        virtual void ProcessMediaFrame(mediaindex::CCFrame & a_rFrame) { m_nTotalProcessedFrameCount++; }

        virtual uint64_t GetTotalProcessedFrameCount() { return m_nTotalProcessedFrameCount; }
        virtual uint64_t GetTotalStartFrameCount(){ return m_nTotalStartFrameCount; }
        virtual uint64_t GetTotalStopFrameCount(){ return m_nTotalStopFrameCount; }

    private:
        uint64_t m_nTotalProcessedFrameCount;
        uint64_t m_nTotalStartFrameCount;
        uint64_t m_nTotalStopFrameCount;
};


class ExtractTest :
public testing::Test {
protected:

    virtual void SetUp() {
        m_pMinuteFileWriter = NULL;

        m_sDataDir = GTestUtils::GetDataDir("..");
        m_sDataDir.append("/video/matroska/");

        m_sTestAreaDir = "tmp_test_area/";
        GTestUtils::DeleteDirectory(m_sTestAreaDir);

        GTestUtils::CreateDirectory(m_sTestAreaDir);
        m_sTestAreaBlobDir = "tmp_test_area/blob/";
        GTestUtils::CreateDirectory(m_sTestAreaBlobDir);
        m_sTestAreaDBDir = "tmp_test_area/db/";
        GTestUtils::CreateDirectory(m_sTestAreaDBDir);
    }

    virtual void TearDown() {
        delete m_pMinuteFileWriter;

        GTestUtils::ConditionalDeleteDirectory(m_sTestAreaDir);
    }

    void CreateMinuteFileWriter(string aInFile,
            string aDbDir,
            string aBlobDir) {
        m_pMinuteFileWriter = new CMinuteFileWriter(aInFile, aDbDir, aBlobDir);
    }

    void CheckMD5Sums(std::string basePath, md5SumInfo_t md5SumInfo[]) {
        char md5sum[40];
        for (int i = 0; ; i++) {
            if (md5SumInfo[i].m_strPath == "")
                break;
            GTestUtils::MD5SumFile(basePath + md5SumInfo[i].m_strPath, md5sum);
            EXPECT_STREQ(md5SumInfo[i].m_strMD5Sum.c_str(), md5sum);
        }

    }

    void PrintMD5Sums(std::string basePath, md5SumInfo_t md5SumInfo[]) {
        char md5sum[40];
        printf("{\n");
        for (int i = 0; ; i++) {
            if (md5SumInfo[i].m_strPath == "")
                break;
            GTestUtils::MD5SumFile(basePath + md5SumInfo[i].m_strPath, md5sum);
            printf("    {\"%s\", \"%s\"},\n", md5SumInfo[i].m_strPath.c_str(), md5sum);
        }
        printf("    {\"\", \"\"},\n");
        printf("};\n");

    }

protected:
    CMinuteFileWriter *m_pMinuteFileWriter;
    string m_sDataDir;
    string m_sTestAreaDir;
    string m_sTestAreaBlobDir;
    string m_sTestAreaDBDir;
};


static md5SumInfo_t md5InfoBlob1[] =
{
    {"0",    "455141efd7f768ed3c9d7acc9333e5fd"},
    {"0.idx", "a1e13a4784e23c5a6bb85a19a09a421a"},
    {"1", "2ca32de878e38e98eca52b5cd3a528fb"},
    {"1.idx", "a308f8668924d50254bd16346a8120cd"},
    {"2", "067d8efdb618e043244125800eaf8a7f"},
    {"2.idx", "106b1ce309b9f087fc7188d916f210aa"},
    {"3", "0085a58897d96f64615db3b4da2b9070"},
    {"3.idx", "623aa85d3c0395406cbeea9048deee81"},
    {"INDEX", "0680153d67658f7e7fb84940bce8ff46"},
    {"", ""},
};

static md5SumInfo_t md5InfoDB1[] =
{
    {"db.xml", "b72e4c79b3b37c3dc1ba1bf4b5a586d5"},
    {"db/_index", "4352bb7b00d9d9cd4d439b1e556db9b1"},
    {"db/IPVSc_default/msfrl_cb34c6d0-cec0-48bb-9b88-9a46b70eb754",
        "92734ef9f1f5eae984656cef4a3cbc3c"},
    {"db/IPVSc_default/msfrl_c2f655e3-50c0-4eab-97d7-21e74026e57f",
        "353a8ad0ecb5474bc8ad3770b399cec4"},
    {"db/IPVSc_default/bookmark_0.xml", "9ead85a2925b1097a89e10e4fd099400"},
    {"db/IPVSc_default/bookmark_1.xml", "5a2a332845159c2a69d501a4ed691bfa"},
    {"db/IPVSc_default/bookmark_2.xml", "147c721bce11c95c01d3f696c4e709d6"},
    {"db/IPVSc_default/bookmark_3.xml", "4e36420047f7305bdd665296eb4e133e"},
    {"db/IPVSc_default/bookmark_4.xml", "4938e9d9eb65f348e8c64122c200c00d"},
    {"db/IPVSc_default/bookmark_5.xml", "0f8c8408c7baa092947175f2a038c52f"},
    {"db/IPVSc_default/bookmark_6.xml", "a06320da9f1ca6e38692d556a372fd24"},
    {"db/IPVSc_default/bookmark_7.xml", "516b51566bef36e6b085935d300e349d"},
    {"db/IPVSc_default/bookmark_8.xml", "b9a769305a91ffe500276ab8cbe4537b"},
    {"", ""},
};

static md5SumInfo_t md5InfoBlob2[] =
{
    {"0",    "15d117a9f58ebf258a5029a4bfb22a10"},
    {"0.idx", "6a2637527f0ae2a5ae46d953c3e109cd"},
    {"1", "d6f33bbe1fc54ffa636b7ee321b90934"},
    {"1.idx", "b29c8bd3fb16163560413e7cacfdc039"},
    {"3", "b55b18315acd1e869d92c4345faedbbc"},
    {"3.idx", "57a8f94128b3f37fd13db27b5250f39a"},
    {"INDEX", "0189b9a68e25eba13da29ff50dc17242"},
    {"", ""},
};

static md5SumInfo_t md5InfoDB2[] =
{
    {"db.xml", "b72e4c79b3b37c3dc1ba1bf4b5a586d5"},
    {"db/_index", "4352bb7b00d9d9cd4d439b1e556db9b1"},
    {"db/IPVSc_default/msfrl_cb34c6d0-cec0-48bb-9b88-9a46b70eb754",
        "92734ef9f1f5eae984656cef4a3cbc3c"},
    {"db/IPVSc_default/msfrl_c2f655e3-50c0-4eab-97d7-21e74026e57f",
        "353a8ad0ecb5474bc8ad3770b399cec4"},
    {"db/IPVSc_default/bookmark_0.xml", "5a2a332845159c2a69d501a4ed691bfa"},
    {"db/IPVSc_default/bookmark_1.xml", "4e36420047f7305bdd665296eb4e133e"},
    {"db/IPVSc_default/bookmark_2.xml", "4938e9d9eb65f348e8c64122c200c00d"},
    {"db/IPVSc_default/bookmark_3.xml", "0f8c8408c7baa092947175f2a038c52f"},
    {"db/IPVSc_default/bookmark_4.xml", "a06320da9f1ca6e38692d556a372fd24"},
    {"db/IPVSc_default/bookmark_5.xml", "516b51566bef36e6b085935d300e349d"},
    {"", ""},
};

static md5SumInfo_t md5InfoBlob3[] =
{
    {"0",    "52205f55ffc1394befc832ab1f102842"},
    {"0.idx", "e48e352802ed87c931d5721b9fc54b5b"},
    {"INDEX", "4a486c98d273399ce371062dd050f65f"},
    {"", ""},
};

static md5SumInfo_t md5InfoDB3[] =
{
    {"db.xml", "b72e4c79b3b37c3dc1ba1bf4b5a586d5"},
    {"db/_index", "4352bb7b00d9d9cd4d439b1e556db9b1"},
    {"db/IPVSc_default/msfrl_cb34c6d0-cec0-48bb-9b88-9a46b70eb754",
        "92734ef9f1f5eae984656cef4a3cbc3c"},
    {"db/IPVSc_default/msfrl_c2f655e3-50c0-4eab-97d7-21e74026e57f",
        "353a8ad0ecb5474bc8ad3770b399cec4"},
    {"db/IPVSc_default/bookmark_0.xml", "5a2a332845159c2a69d501a4ed691bfa"},
    {"db/IPVSc_default/bookmark_1.xml", "147c721bce11c95c01d3f696c4e709d6"},
    {"db/IPVSc_default/bookmark_2.xml", "4e36420047f7305bdd665296eb4e133e"},
    {"db/IPVSc_default/bookmark_3.xml", "4938e9d9eb65f348e8c64122c200c00d"},
    {"", ""},
};

static md5SumInfo_t md5InfoSnapshot1Blob[] =
{
    {"snapshot_0.jpg",    "66c36303ded60b22ac9ffd2bc4142a2b"},
    {"snapshot_1.bmp",    "a078ffa2ef397c0feaba1b437d8a6ef2"},
    {"snapshot_2.png",    "3bd8f6ad32bab1ae84f594d24007d36b"},
    {"SSINDEX",    "c5ddcab156b9c02bc9f6d619f5dd31e8"},
    {"tn/tn.jpg",  "1660d9701d4d293fa8d40cdf79943289"},
    {"", ""},
};

static md5SumInfo_t md5InfoSnapshot2Blob[] =
{
    {"snapshot_0.jpg",    "9fcadade4593d7e9b53e60acf1aa32e3"},
    {"SSINDEX",    "617c0148b8cfede492078e5d14359583"},
    {"", ""},
};

static md5SumInfo_t md5InfoSnapshot2DB[] =
{
    {"db.xml", "12f68398e986ff2f9ec7511bedae1a31"},
    {"db/_index", "ee7485955593404e02fb1755d491a4b8"},
    {"db/IPVSc_default/msfrl_61c7e513-41d0-4fb7-a513-369de3698adc",
        "7a1d640eafb31935290a5a1cc9183e8c"},
    {"", ""},
};

static md5SumInfo_t md5InfoBlobTN1[] =
{
    {"0", "19f9ddcbacea8095211505582db93ae3"},
    {"0.idx", "9babd563c758a87ea604da8835151e60"},
    {"1", "94e6cbf9abff7d817749e2fa581e9f0c"},
    {"1.idx", "fbad66385a85cca9236608a6e2961fd1"},
    {"2", "c9397d9af0df760481eeafa10fa70c72"},
    {"2.idx", "658a6333d3bba4c8e5f6fc5dec878bb9"},
    {"3", "8cafad199e51828aa96d8fab946fb3e0"},
    {"3.idx", "72f89e4c932f460bb7762bd2038bd8d0"},
    {"4", "d4cac3f2d009a455a7926dc2a0f4afd1"},
    {"4.idx", "67e3450f1ef5719a8b661020f4fbd769"},
    {"5", "76d5db04a2884d4c4aac41c48faec4bb"},
    {"5.idx", "32b6e1410d56cc8ae9341cd023c53fe6"},
    {"6", "e1a5ffe5db61bc8c0c87b5994a5b0cf7"},
    {"6.idx", "8364e72f33d2a831a66fd0f09be70fd4"},
    {"INDEX", "6e282b794cc68b66979d44bb6475e829"},
    {"tn/0.tdx", "b0051e1d16b2059c34e37d1c8263e253"},
    {"tn/1.tdx", "b9465344ff67fd102a95ec1a71f14db4"},
    {"tn/2.tdx", "27ecaff8068c085b7ddfd08297f80455"},
    {"tn/3.tdx", "389e27d969de7771b6a2cad31bd41493"},
    {"tn/4.tdx", "a56ef86319b6e29d12e9f71fff623d28"},
    {"tn/5.tdx", "dcc74ec2ce081d88fc0807f851e79c0b"},
    {"tn/6.tdx", "d8022e4a6a66d673fabba3e1ecf3ad17"},
    {"tn/INDEX", "6e282b794cc68b66979d44bb6475e829"},
    {"tn/tn.jpg", "c756c25c7a329f218723c49a3a68aa6a"},
    {"tn/1268674716518.jpg", "8b0cb8909216e274e43589d63d9f287d"},
    {"tn/1268674739662.jpg", "3474d4990465ee8eb3d667d6708f02fe"},
    {"tn/1268674764706.jpg", "6b5524dad07f048c95fe689847d5243e"},
    {"tn/1268674789659.jpg", "efcbec2d68dc0e78fca8a2392de37e9d"},
    {"tn/1268674814669.jpg", "8840dc60a3ccbff3c4ae0c3ad3b3c61b"},
    {"tn/1268674839668.jpg", "2675c718484a70960ea9e1cf06847d73"},
    {"tn/1268674864676.jpg", "23418fc0ed694fd1a94e1bb5e366f589"},
    {"tn/1268674889712.jpg", "9ff866936ac4fd0ce876ad88b0c4b10c"},
    {"tn/1268674914729.jpg", "cea5dc3ef004b562257e45f8ecda24c3"},
    {"tn/1268674939730.jpg", "4acbff0a85122aa76cd8b3532895d5d3"},
    {"tn/1268674964731.jpg", "08dc597ac7a2d16a6c8c3ce66975b0d1"},
    {"tn/1268674989734.jpg", "ce9858bd7bc183a1ffcb40eb3a09aaa6"},
    {"tn/1268675014661.jpg", "9d5ee38fa9835f912dced08a90b2053f"},
    {"tn/1268675039681.jpg", "74ba5a69e5c0ade1a079f9f5d6814179"},
    {"tn/1268675064677.jpg", "3f00603b1cd11c4c113b540416327be4"},
    {"tn/1268675089697.jpg", "081c9927da934334b3040e5ca81ca424"},
    {"", ""},
};

TEST_F(ExtractTest, BadInputFileReturnsErrorCode) {
    CreateMinuteFileWriter("blah.mkd", "", "");
    m_pMinuteFileWriter->Silent();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_ERROR, rc);
}

TEST_F(ExtractTest, NoDBDirReturnsErrorCode) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", "blah_db_dir", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_ERROR, rc);
}

TEST_F(ExtractTest, NoBlobDirReturnsErrorCode) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", m_sTestAreaDBDir, "blah_blob_dir");
    m_pMinuteFileWriter->Silent();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_ERROR, rc);
}

// Extract blob and ensure everything is retrieved
TEST_F(ExtractTest, ExtractBlob) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob1);
}

// Extract both db and blob and ensure everything is retrieved
TEST_F(ExtractTest, ExtractAll) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", m_sTestAreaDBDir, m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob1);
    CheckMD5Sums(m_sTestAreaDBDir, md5InfoDB1);
}

TEST_F(ExtractTest, ExtractBlobWithThumbnails) {
    CreateMinuteFileWriter(m_sDataDir + "thumbnails1.mkd", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlobTN1);
}


TEST_F(ExtractTest, ExtractEditList) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", m_sTestAreaDBDir, m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    // This edit list includes size of the nine bookmarks present in the media
    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");
    m_pMinuteFileWriter->SetEditList(editList);
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob2);
    CheckMD5Sums(m_sTestAreaDBDir, md5InfoDB2);
}

TEST_F(ExtractTest, ExtractEditListWithBookmarks) {
    CreateMinuteFileWriter(m_sDataDir + "clip1.mkd", m_sTestAreaDBDir, m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    // This edit list includes four of the nine bookmarks present in the media
    CEditList * editList = new CEditList();
    editList->Set("3000:11000");
    m_pMinuteFileWriter->SetEditList(editList);
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob3);
    CheckMD5Sums(m_sTestAreaDBDir, md5InfoDB3);
}

TEST_F(ExtractTest, ExtractBlobFromBlob) {
    CreateMinuteFileWriter(m_sDataDir + "clip2_blob", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob1);
}

TEST_F(ExtractTest, ExtractEditListFromBlob) {
    CreateMinuteFileWriter(m_sDataDir + "clip2_blob", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    CEditList * editList = new CEditList();
    editList->Set("3000:5000,8000:10000,15000:18000,32000:33000,65000:75000,110000:115000,190000:210000");
    m_pMinuteFileWriter->SetEditList(editList);
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoBlob2);
}

TEST_F(ExtractTest, ExtractSnapshotBlob) {
    CreateMinuteFileWriter(m_sDataDir + "snapshot1_blob.mkd", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoSnapshot1Blob);
}

TEST_F(ExtractTest, ExtractSnapshotAll) {
    CreateMinuteFileWriter(m_sDataDir + "snapshot2.mkd", m_sTestAreaDBDir, m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    CheckMD5Sums(m_sTestAreaBlobDir, md5InfoSnapshot2Blob);
    CheckMD5Sums(m_sTestAreaDBDir, md5InfoSnapshot2DB);
}

TEST_F(ExtractTest, TestHLS) {
    m_pMinuteFileWriter = new CMinuteFileWriter(m_sDataDir + "clip1.mkd",
                                                m_sTestAreaDBDir, m_sTestAreaBlobDir);

    CMediaProcessorInterfaceDummyImpl *pProcessor = new CMediaProcessorInterfaceDummyImpl();
    m_pMinuteFileWriter->AddMediaProcessor(pProcessor);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);

    uint64_t nTotalPktsProcessed = pProcessor->GetTotalProcessedFrameCount();
    EXPECT_EQ(3749ULL, nTotalPktsProcessed);

    uint64_t nTotalStartFrameCount = pProcessor->GetTotalStartFrameCount();
    EXPECT_EQ(1ULL, nTotalStartFrameCount);

    uint64_t nTotalStopFrameCount = pProcessor->GetTotalStopFrameCount();
    EXPECT_EQ(1ULL, nTotalStopFrameCount);

    delete pProcessor;
}


#if 0
TEST_F(ExtractTest, ExtractEditListAndAppend) {
    // Extract multiple sections of source files into the same destination blob folder
    // without deleting the folder. A "append to recording" should be simulated
    CreateMinuteFileWriter(m_sDataDir + "clip2_blob", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    CEditList * editList = new CEditList();
    editList->Set("3000:5000");
    m_pMinuteFileWriter->SetEditList(editList);
    int rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;
    delete m_pMinuteFileWriter;

    CreateMinuteFileWriter(m_sDataDir + "clip2_blob", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    editList = new CEditList();
    editList->Set("8000:10000");
    m_pMinuteFileWriter->SetEditList(editList);
    rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;
    delete m_pMinuteFileWriter;

    CreateMinuteFileWriter(m_sDataDir + "clip2_blob", "", m_sTestAreaBlobDir);
    m_pMinuteFileWriter->Silent();
    m_pMinuteFileWriter->FixedUUIDs();

    editList = new CEditList();
    editList->Set("190000:210000");
    m_pMinuteFileWriter->SetEditList(editList);
    rc = m_pMinuteFileWriter->Write();
    EXPECT_EQ(ELIBMC_OK, rc);
    delete editList;

}
#endif

//TEST_F(ExtractTest, ExtractEditListWithGaps) {
//    CreateMinuteFileWriter(m_sDataDir + "v2d_gaps.mkd", m_sTestAreaDBDir, m_sTestAreaBlobDir);
//    m_pMinuteFileWriter->Silent();
//    m_pMinuteFileWriter->FixedUUIDs();
//    CEditList * editList = new CEditList();
//    editList->Set("3000:45000,150000:160000,340000:400000");
//    m_pMinuteFileWriter->SetEditList(editList);
//    int rc = m_pMinuteFileWriter->Write();
//    EXPECT_EQ(ELIBMC_OK, rc);
//}
