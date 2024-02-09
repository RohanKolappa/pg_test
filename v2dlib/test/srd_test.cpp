/*
 * srd_test.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: rkale
 */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"

#include "v2dlib_compatibility.h"
#include "srd.h"
#include "memory_tracker.h"
#include "codecdefs.h"

using namespace std;

#define VIDEO_PACKET_SIZE 1024

struct PacketTimestampInfo {
    uint32_t m_nPacketNumber;
    uint32_t m_nTimestamp;
};

// SRD tests
class SRD: public testing::Test {
protected:

    virtual void SetUp()
    {
        m_strDataDir = GTestUtils::GetDataDir("..");
        ASSERT_NE(m_strDataDir, "");
        m_strDataDir.append("/video/clips/v2d_raw");
        //m_strDataDir = ".";

        CreateSRD();
        setStereoPolicy(m_srd, SP_SOURCE);

        m_nSRDPacketOffset = 0;
        m_nDropRate = 0;
        m_bitstream = NULL;
        m_bitstreamOut = NULL;
        m_nOutputPacketCount = 0;
    }

    virtual void TearDown() {
        if (m_srd != NULL) {
            destroySRD(m_srd);
            free(m_srd);
        }
        CloseDataFile();

        if(m_bitstreamOut != NULL)
        m_bitstreamOut->close();
    }

    void CreateSRD()
    {
        m_srd = (sliceRedData_t *) malloc(sizeof(sliceRedData_t));
        ASSERT_TRUE(m_srd != NULL);
        ASSERT_TRUE(initSRD(m_srd) >= 0);
    }

    void CloseDataFile()
    {
        if (m_bitstream != NULL) {
          m_bitstream->close();
      //    delete m_bitstream;
        }
    }

    bool OpenDataFile(string fileName) {
        string strDataFile = m_strDataDir + "/" + fileName;
        m_bitstream = new ifstream(strDataFile.c_str(), ios_base::in | ios_base::binary);
        return !m_bitstream->fail();
    }

    bool WriteDataFile(string fileName) {
        string strDataFile = m_strDataDir + "/" + fileName;
        m_bitstreamOut = new ofstream(strDataFile.c_str(), ios_base::out | ios_base::binary);
        return !m_bitstreamOut->fail();
    }

    static void SRDNewFrameCallback(void *a_object) {
        SRD * This = (SRD *) a_object;
        This->ProcessSRDFrame(true);
    }

    static void SRDNewFrameCallbackEOF(void *a_object) {
        SRD * This = (SRD *) a_object;
        This->RetrieveOneFrameFromSRD();
    }

    void ProcessDataFile() {
        uint32_t timeStamp = 1;
        while (!m_bitstream->eof() &&
                m_bitstream->read((char *)m_chReadBuf, VIDEO_PACKET_SIZE)) {

            size_t nBytesRead = m_bitstream->gcount();
            if (nBytesRead != 1024)
                break;
            SRDSetPacketTimestamp(m_srd, timeStamp);
            findFramesinPacket(m_srd, m_chReadBuf);
            timeStamp += 1;
            ProcessSRDFrame(false);
        }
    }

    void ProcessDataVarChunkSize(int size1, unsigned int size2) {
        unsigned char *m_chReadBuff = new unsigned char[size1 * 3];
        unsigned int pak_len = size1;
        unsigned int len = size1;
        while ((len <= size2) &&
                m_bitstream->read((char *)m_chReadBuff, pak_len)) {

            size_t nBytesRead = m_bitstream->gcount();
            if (nBytesRead != pak_len)
            break;

            findSVCFramesinPacket(m_srd, m_chReadBuff, pak_len);
            RetrieveOneFrameFromSRD();

            if ((m_srd->inPktCount > 0) && (m_srd->inPktCount < 150)) {
                pak_len = 2048;
            }
            else if ((m_srd->inPktCount > 150) && (m_srd->inPktCount < 300)) {
                pak_len = 768;
            }
            else if ((m_srd->inPktCount > 300) && (m_srd->inPktCount < 400)) {
                pak_len = 512;
            }
            else if ((m_srd->inPktCount > 400) && (m_srd->inPktCount < 500)) {
                pak_len = 12;
            }
            len += pak_len;
           /* if(len > size2) {
                int temp = len - size2;
                len -= len - temp;
            }*/

        }
        delete [] m_chReadBuff;
    }

    void RetrieveOneFrameFromSRD() {
        int total_len = 0;

        while (true) {
            int len = getPackets(m_srd, m_chOutBuf, 1024 , 0);
            if (len <= 0)
                break;
           // m_bitstreamOut->write((char *)m_chOutBuf,len);
            total_len += len;
            GTestUtils::MD5SumUpdate(m_chOutBuf, len);
        }
    }

    void ProcessSRDFrame(bool inProgress) {

        // Randomly decide not to pick the next frame
        if (GTestUtils::GetRandomNumber() % 100 < m_nDropRate) {
            return;
        }

        int len = getPackets(m_srd, m_chOutBuf + m_nSRDPacketOffset,
                VIDEO_PACKET_SIZE - m_nSRDPacketOffset, inProgress ? 0 : 1);

        if (len > 0) {
            m_nSRDPacketOffset += len;
        }

        if (m_nSRDPacketOffset >= VIDEO_PACKET_SIZE) {
            GTestUtils::MD5SumUpdate(m_chOutBuf, VIDEO_PACKET_SIZE);
            m_nSRDPacketOffset = 0;


            uint32_t timestamp = SRDGetFrameTimeStamp(m_srd);
            m_packetInfoList.push_back(timestamp);
            m_nOutputPacketCount++;

        }

    }

    void DumpTimeStampInfo()
    {
        for (int i = 0; i < (int) m_packetInfoList.size(); ++i) {
            printf("Packet = %d, Timestamp = %u\n",
                    i, m_packetInfoList[i]);
        }
    }

    void CheckMD5SumOfTimestamps(string referenceMD5Sum)
    {
        // Write the timestamp info out as a string and compare md5sum
        std::stringstream ss;
        for (int i = 0; i < (int) m_packetInfoList.size(); ++i) {
            ss << i << ", " << m_packetInfoList[i] << "\n";
        }
        char md5sum[40];
        GTestUtils::MD5Sum((unsigned char *)ss.str().c_str(), ss.str().size(), md5sum);
        EXPECT_STREQ(referenceMD5Sum.c_str(), md5sum);
    }

    void ProcessSRDTrack(int trackId) {

        getPacketsForTrack(m_srd, m_chTrackBuf[trackId], VIDEO_PACKET_SIZE, 1, trackId);

    }

    void CheckMD5SumOfResult(string referenceMD5Sum) {
        // Check result using md5sum of output
        char md5sum[40];
        GTestUtils::MD5SumResult(md5sum);
        EXPECT_STREQ(referenceMD5Sum.c_str(), md5sum);
    }

    virtual void PrintSRDStats() {
        printf("SRDInPackets %lu\n", m_srd->inPktCount);
        printf("SRDOutPackets %lu\n", m_srd->outPktCount);
        printf("SRDInFrames %lu\n", m_srd->inFrameCount);
        printf("SRDOutFrames %lu\n", m_srd->outFrameCount);
        printf("SRDRightFrames %lu\n", m_srd->inRightFrameCount);
        printf("SRDLeftFrames %lu\n", m_srd->inLeftFrameCount);
        printf("SRDReducedFrames %lu\n", m_srd->reducedFrameCount);
        printf("SRDInSlices %lu\n", m_srd->inSlices);
        printf("SRDOutSlices %lu\n", m_srd->outSlices);
        printf("SRDReducedSlices %lu\n", m_srd->reducedSlices);
        printf("SRDInBytes %lu\n", m_srd->inBytes);
        printf("SRDOutBytes %lu\n", m_srd->outBytes);
        printf("SRDReducedBytes %lu\n", m_srd->reducedBytes);
        printf("SRDPrefillFrames %lu\n", m_srd->prefillFrameCount);
        printf("SRDReducedAltSlices %lu\n", m_srd->reducedAltSlices);
        printf("SRDPadBytes %lu\n", m_srd->padBytes);
        printf("SRDIFrameCount %lu\n", m_srd->IFrameCount);
        printf("SRDOutputIFrameCount %lu\n", m_srd->OutputIFrameCount);
        printf("SRDFullFrameCount %lu\n", m_srd->FullFrameCount);
        printf("SRDFullFrameInterval %lu\n", m_srd->FullFrameInterval);
        printf("SRDMaxSliceNumber %d\n", m_srd->MaxSliceNo);
        printf("SRDSliceSize %d\n", m_srd->sliceSize);
        printf("SRDSliceOutOfOrder %lu\n", m_srd->parse_errors_ooo_slice);
        printf("SRDBadSlice %lu\n", m_srd->parse_errors_bad_slice);
        printf("SRDBadFrame %lu\n", m_srd->parse_errors_bad_frame);
        printf("SRDSOFError %lu\n", m_srd->parse_errors_sof);
        printf("SRDFrameOverflow %lu\n", m_srd->hbuff_overflow);

    }

    virtual void PrintSRDExpectedString() {
        printf("EXPECT_EQ(%d, (int) m_srd->inPktCount);\n", (int) m_srd->inPktCount);
        printf("EXPECT_EQ(%d, (int) m_srd->outPktCount);\n", (int) m_srd->outPktCount);
        printf("EXPECT_EQ(%d, (int) m_srd->inFrameCount);\n", (int) m_srd->inFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->outFrameCount);\n", (int) m_srd->outFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->inRightFrameCount);\n", (int) m_srd->inRightFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->inLeftFrameCount);\n", (int) m_srd->inLeftFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->reducedFrameCount);\n", (int) m_srd->reducedFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->inSlices);\n", (int) m_srd->inSlices);
        printf("EXPECT_EQ(%d, (int) m_srd->outSlices);\n", (int) m_srd->outSlices);
        printf("EXPECT_EQ(%d, (int) m_srd->reducedSlices);\n", (int) m_srd->reducedSlices);
        printf("EXPECT_EQ(%d, (int) m_srd->inBytes);\n", (int) m_srd->inBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->outBytes);\n", (int) m_srd->outBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->reducedBytes);\n", (int) m_srd->reducedBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->prefillFrameCount);\n", (int) m_srd->prefillFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->reducedAltSlices);\n", (int) m_srd->reducedAltSlices);
        printf("EXPECT_EQ(%d, (int) m_srd->padBytes);\n", (int) m_srd->padBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->IFrameCount);\n", (int) m_srd->IFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->OutputIFrameCount);\n", (int) m_srd->OutputIFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->FullFrameCount);\n", (int) m_srd->FullFrameCount);
        printf("EXPECT_EQ(%d, (int) m_srd->FullFrameInterval);\n", (int) m_srd->FullFrameInterval);
        printf("EXPECT_EQ(%d, (int) m_srd->svcDCBytes);\n", (int) m_srd->svcDCBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->svcQuadBytes);\n", (int) m_srd->svcQuadBytes);
        printf("EXPECT_EQ(%d, (int) m_srd->svcRemBytes);\n", (int) m_srd->svcRemBytes);
    }

protected:
    string m_strDataDir;
    uint8_t m_chReadBuf[VIDEO_PACKET_SIZE];
    uint8_t m_chOutBuf[VIDEO_PACKET_SIZE];
    uint8_t m_chTrackBuf[4][VIDEO_PACKET_SIZE];
    sliceRedData_t *        m_srd;
    unsigned long m_nSRDPacketOffset;
    int m_nDropRate;
    ifstream * m_bitstream;
    ofstream * m_bitstreamOut;
    int m_nOutputPacketCount;
    std::vector<uint32_t> m_packetInfoList;
};


// All SRD tests sends frames to SRD and at the same time picks some frames
// with some random frequency. Some frames get reduced and some gets pulled
// out.

// Test default behavior with no SRDNewFrameCallback installed
TEST_F(SRD, Normal)
{

    ASSERT_TRUE(OpenDataFile("codec_1_basic.dat"));

    // Following values set based on input recording of
    // 1024 x 768 frame dimension
    m_srd->MaxSliceNo = 384;
    m_srd->sliceSize = 32;

    ProcessDataFile();


    CheckMD5SumOfResult("a778ac04bf2e989c87baf4df73dc4752");

    //DumpTimeStampInfo();
    //PrintSRDStats();

    // Check SRD counters
    EXPECT_EQ(4605, (int) m_srd->inPktCount);
    EXPECT_EQ(4349, (int) m_srd->outPktCount);
    EXPECT_EQ(444, (int) m_srd->inFrameCount);
    EXPECT_EQ(238, (int) m_srd->outFrameCount);
    EXPECT_EQ(444, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(206, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(18723, (int) m_srd->inSlices);
    EXPECT_EQ(18035, (int) m_srd->outSlices);
    EXPECT_EQ(627, (int) m_srd->reducedSlices);
    EXPECT_EQ(4715128, (int) m_srd->inBytes);
    EXPECT_EQ(4453376, (int) m_srd->outBytes);
    EXPECT_EQ(257436, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(11804, (int) m_srd->padBytes);
    EXPECT_EQ(1, (int) m_srd->IFrameCount);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(6, (int) m_srd->FullFrameCount);
    EXPECT_EQ(77, (int) m_srd->FullFrameInterval);

    // Check timestamp info
    CheckMD5SumOfTimestamps("8edd33013664f6c130022d93c1436311");
}


// Test with SRDNewFrameCallback installed to pickup every frame
TEST_F(SRD, NormalWithCallback)
{

    ASSERT_TRUE(OpenDataFile("codec_1_basic.dat"));

    // Install callback handler
    SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);

    // Following values set based on input recording of
    // 1024 x 768 frame dimension
    m_srd->MaxSliceNo = 384;
    m_srd->sliceSize = 32;

    ProcessDataFile();

    //DumpTimeStampInfo();
    //PrintSRDStats();

    CheckMD5SumOfResult("03c607cf9bf4129b56b80f6d05c54868");

    // Check SRD counters
    EXPECT_EQ(4605, (int) m_srd->inPktCount);
    EXPECT_EQ(4062, (int) m_srd->outPktCount);
    EXPECT_EQ(444, (int) m_srd->inFrameCount);
    EXPECT_EQ(306, (int) m_srd->outFrameCount);
    EXPECT_EQ(444, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(138, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(18723, (int) m_srd->inSlices);
    EXPECT_EQ(18428, (int) m_srd->outSlices);
    EXPECT_EQ(258, (int) m_srd->reducedSlices);
    EXPECT_EQ(4715128, (int) m_srd->inBytes);
    EXPECT_EQ(4614144, (int) m_srd->outBytes);
    EXPECT_EQ(121048, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(29912, (int) m_srd->padBytes);
    EXPECT_EQ(1, (int) m_srd->IFrameCount);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(6, (int) m_srd->FullFrameCount);
    EXPECT_EQ(77, (int) m_srd->FullFrameInterval);

    // Check timestamp info
    CheckMD5SumOfTimestamps("b9fe8a5ea3e77e78082af2b12b75c8be");

}

// Turn on Slice Drain and use non zero drop rate
TEST_F(SRD, SliceDrain)
{

    ASSERT_TRUE(OpenDataFile("codec_1_basic.dat"));

    // Turn on slice drain
    m_srd->slice_drain = 1;

    // Set drop rate
    GTestUtils::SetRandomSeed(5);
    m_nDropRate = 10;

    // Install callback handler
    SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);

    // Following values set based on input recording of
    // 1024 x 768 frame dimension
    m_srd->MaxSliceNo = 384;
    m_srd->sliceSize = 32;

    ProcessDataFile();

    //DumpTimeStampInfo();
    //PrintSRDStats();

    CheckMD5SumOfResult("b40f3776761b23c2ec62350d4f1e9715");

    // Check SRD counters
    EXPECT_EQ(4605, (int) m_srd->inPktCount);
    EXPECT_EQ(4073, (int) m_srd->outPktCount);
    EXPECT_EQ(444, (int) m_srd->inFrameCount);
    EXPECT_EQ(317, (int) m_srd->outFrameCount);
    EXPECT_EQ(444, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(127, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(18723, (int) m_srd->inSlices);
    EXPECT_EQ(18485, (int) m_srd->outSlices);
    EXPECT_EQ(184, (int) m_srd->reducedSlices);
    EXPECT_EQ(4715128, (int) m_srd->inBytes);
    EXPECT_EQ(4595712, (int) m_srd->outBytes);
    EXPECT_EQ(105560, (int) m_srd->reducedBytes);
    EXPECT_EQ(116, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(0, (int) m_srd->padBytes);
    EXPECT_EQ(1, (int) m_srd->IFrameCount);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(6, (int) m_srd->FullFrameCount);
    EXPECT_EQ(77, (int) m_srd->FullFrameInterval);

    // Check timestamp info
    CheckMD5SumOfTimestamps("fcc7b94120a73e873e539320204b726b");
}

TEST_F(SRD, NormalFullFrameRequest)
{

    ASSERT_TRUE(OpenDataFile("codec_1_basic.dat"));

    // Following values set based on input recording of
    // 1024 x 768 frame dimension
    m_srd->MaxSliceNo = 384;
    m_srd->sliceSize = 32;

    int count = 0;
    while (!m_bitstream->eof() &&
            m_bitstream->read((char *)m_chReadBuf, VIDEO_PACKET_SIZE)) {

        size_t nBytesRead = m_bitstream->gcount();
        if (nBytesRead != 1024)
            break;
        findFramesinPacket(m_srd, m_chReadBuf);
        ProcessSRDFrame(false);
        if (count % 100 == 1) {
            requestFullFrame(m_srd);
        }
        count++;
    }

    CheckMD5SumOfResult("e624e9ea5397ec8873233caca56d78aa");

    // Check SRD counters
    EXPECT_EQ(4605, (int) m_srd->inPktCount);
    EXPECT_EQ(4537, (int) m_srd->outPktCount);
    EXPECT_EQ(444, (int) m_srd->inFrameCount);
    EXPECT_EQ(65, (int) m_srd->outFrameCount);
    EXPECT_EQ(444, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(388, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(18723, (int) m_srd->inSlices);
    EXPECT_EQ(20782, (int) m_srd->outSlices);
    EXPECT_EQ(10506, (int) m_srd->reducedSlices);
    EXPECT_EQ(4715128, (int) m_srd->inBytes);
    EXPECT_EQ(4645888, (int) m_srd->outBytes);
    EXPECT_EQ(2890500, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(0, (int) m_srd->padBytes);
    EXPECT_EQ(1, (int) m_srd->IFrameCount);
    EXPECT_EQ(46, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(6, (int) m_srd->FullFrameCount);
    EXPECT_EQ(77, (int) m_srd->FullFrameInterval);
}

TEST_F(SRD, NormalMultiTrack) {


    ASSERT_TRUE(OpenDataFile("codec_1_basic.dat"));

    SRDEnableInternalRefresh(m_srd);
    SRDSetNumberOfTracks(m_srd, 4);

    // Following values set based on input recording of
    // 1024 x 768 frame dimension
    m_srd->MaxSliceNo = 384;
    m_srd->sliceSize = 32;

    int count = 0;
    while (!m_bitstream->eof() &&
            m_bitstream->read((char *)m_chReadBuf, VIDEO_PACKET_SIZE)) {

        size_t nBytesRead = m_bitstream->gcount();
        if (nBytesRead != 1024)
            break;
        findFramesinPacket(m_srd, m_chReadBuf);

        // Drain each track at different rates

        ProcessSRDTrack(0);   // Drain every time

        if (count % 10 != 0)
            ProcessSRDTrack(1); // 90% of the time

        if (count % 2 != 0)
            ProcessSRDTrack(2); // 50% the time

        if (count % 15 == 0)  // Every 15th packet
            ProcessSRDTrack(3);

        count++;
    }


    // Check SRD counters for Track 1
    EXPECT_EQ(4605, (int) m_srd->inPktCount);
    EXPECT_EQ(4166, (int) m_srd->outPktCount);
    EXPECT_EQ(444, (int) m_srd->inFrameCount);
    EXPECT_EQ(260, (int) m_srd->outFrameCount);
    EXPECT_EQ(444, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(184, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(18723, (int) m_srd->inSlices);
    EXPECT_EQ(17372, (int) m_srd->outSlices);
    EXPECT_EQ(1752, (int) m_srd->reducedSlices);
    EXPECT_EQ(4715128, (int) m_srd->inBytes);
    EXPECT_EQ(4265984, (int) m_srd->outBytes);
    EXPECT_EQ(528068, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(37596, (int) m_srd->padBytes);
    EXPECT_EQ(1, (int) m_srd->IFrameCount);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(6, (int) m_srd->FullFrameCount);
    EXPECT_EQ(77, (int) m_srd->FullFrameInterval);

    // Check SRD counters for Track 2
    EXPECT_EQ(3905, (int) m_srd->outPktCountPerTrack[1]);
    EXPECT_EQ(221, (int) m_srd->outFrameCountPerTrack[1]);
    EXPECT_EQ(223, (int) m_srd->reducedFrameCountPerTrack[1]);
    EXPECT_EQ(16554, (int) m_srd->outSlicesPerTrack[1]);
    EXPECT_EQ(2389, (int) m_srd->reducedSlicesPerTrack[1]);
    EXPECT_EQ(3998720, (int) m_srd->outBytesPerTrack[1]);
    EXPECT_EQ(736576, (int) m_srd->reducedBytesPerTrack[1]);
    EXPECT_EQ(16084, (int) m_srd->padBytesPerTrack[1]);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCountPerTrack[1]);

    // Check SRD counters for Track 3
    EXPECT_EQ(2268, (int) m_srd->outPktCountPerTrack[2]);
    EXPECT_EQ(94, (int) m_srd->outFrameCountPerTrack[2]);
    EXPECT_EQ(350, (int) m_srd->reducedFrameCountPerTrack[2]);
    EXPECT_EQ(10338, (int) m_srd->outSlicesPerTrack[2]);
    EXPECT_EQ(8333, (int) m_srd->reducedSlicesPerTrack[2]);
    EXPECT_EQ(2322432, (int) m_srd->outBytesPerTrack[2]);
    EXPECT_EQ(2358276, (int) m_srd->reducedBytesPerTrack[2]);
    EXPECT_EQ(0, (int) m_srd->padBytesPerTrack[2]);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCountPerTrack[2]);

    // Check SRD counters for Track 4
    EXPECT_EQ(302, (int) m_srd->outPktCountPerTrack[3]);
    EXPECT_EQ(6, (int) m_srd->outFrameCountPerTrack[3]);
    EXPECT_EQ(437, (int) m_srd->reducedFrameCountPerTrack[3]);
    EXPECT_EQ(1615, (int) m_srd->outSlicesPerTrack[3]);
    EXPECT_EQ(16603, (int) m_srd->reducedSlicesPerTrack[3]);
    EXPECT_EQ(309248, (int) m_srd->outBytesPerTrack[3]);
    EXPECT_EQ(4294196, (int) m_srd->reducedBytesPerTrack[3]);
    EXPECT_EQ(0, (int) m_srd->padBytesPerTrack[3]);
    EXPECT_EQ(1, (int) m_srd->OutputIFrameCountPerTrack[3]);
}

// Test default behavior for Codec2 with no SRDNewFrameCallback installed
TEST_F(SRD, Codec2Normal)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));
    
    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

    ProcessDataFile();

    // Check result using md5sum of output
    CheckMD5SumOfResult("16fdcca1915faee45ef0c54ff3c5b3b5");

    // Check SRD counters

    EXPECT_EQ(3647, (int) m_srd->inPktCount);
    EXPECT_EQ(3578, (int) m_srd->outPktCount);
    EXPECT_EQ(1831, (int) m_srd->inFrameCount);
    EXPECT_EQ(63, (int) m_srd->outFrameCount);
    EXPECT_EQ(1831, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(1767, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(19421, (int) m_srd->inSlices);
    EXPECT_EQ(19101, (int) m_srd->outSlices);
    EXPECT_EQ(20, (int) m_srd->reducedSlices);
    EXPECT_EQ(3733435, (int) m_srd->inBytes);
    EXPECT_EQ(3663872, (int) m_srd->outBytes);
    EXPECT_EQ(3632, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(32239, (int) m_srd->padBytes);
    EXPECT_EQ(3, (int) m_srd->IFrameCount);
    EXPECT_EQ(3, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(50, (int) m_srd->FullFrameCount);
    EXPECT_EQ(39, (int) m_srd->FullFrameInterval);

}

// Test with SRDNewFrameCallback installed to pickup every frame
TEST_F(SRD, Codec2NormalWithCallback)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

    // Install callback handler
    SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);

    ProcessDataFile();

    CheckMD5SumOfResult("7b1e42a90be99f617976ec9625c1b7c2");

    // Check SRD counters
    EXPECT_EQ(3647, (int) m_srd->inPktCount);
    EXPECT_EQ(2708, (int) m_srd->outPktCount);
    EXPECT_EQ(1831, (int) m_srd->inFrameCount);
    EXPECT_EQ(1772, (int) m_srd->outFrameCount);
    EXPECT_EQ(1831, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(59, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(19421, (int) m_srd->inSlices);
    EXPECT_EQ(19394, (int) m_srd->outSlices);
    EXPECT_EQ(20, (int) m_srd->reducedSlices);
    EXPECT_EQ(3733435, (int) m_srd->inBytes);
    EXPECT_EQ(4647936, (int) m_srd->outBytes);
    EXPECT_EQ(3632, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(921413, (int) m_srd->padBytes);
    EXPECT_EQ(3, (int) m_srd->IFrameCount);
    EXPECT_EQ(3, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(50, (int) m_srd->FullFrameCount);
    EXPECT_EQ(39, (int) m_srd->FullFrameInterval);
    EXPECT_EQ(917312, (int) m_srd->svcDCBytes);
    EXPECT_EQ(1990557 , (int) m_srd->svcQuadBytes);
    EXPECT_EQ(702086, (int) m_srd->svcRemBytes);
}


// Turn on Slice Drain and use non zero drop rate
TEST_F(SRD, Codec2SliceDrain)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

    // Turn on slice drain
    m_srd->slice_drain = 1;

    // Set drop rate
    GTestUtils::SetRandomSeed(5);
    m_nDropRate = 10;

    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

    // Install callback handler
    SRDSetNewFrameCallback(m_srd, SRDNewFrameCallback, this);

    ProcessDataFile();

    CheckMD5SumOfResult("ec73ae6a82829e2f0323a5362ffc2584");

    // Check SRD counters
    EXPECT_EQ(3647, (int) m_srd->inPktCount);
    EXPECT_EQ(2920, (int) m_srd->outPktCount);
    EXPECT_EQ(1831, (int) m_srd->inFrameCount);
    EXPECT_EQ(1788, (int) m_srd->outFrameCount);
    EXPECT_EQ(1831, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(43, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(19421, (int) m_srd->inSlices);
    EXPECT_EQ(19398, (int) m_srd->outSlices);
    EXPECT_EQ(20, (int) m_srd->reducedSlices);
    EXPECT_EQ(3733435, (int) m_srd->inBytes);
    EXPECT_EQ(4655104, (int) m_srd->outBytes);
    EXPECT_EQ(3632, (int) m_srd->reducedBytes);
    EXPECT_EQ(961, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(927157, (int) m_srd->padBytes);
    EXPECT_EQ(3, (int) m_srd->IFrameCount);
    EXPECT_EQ(3, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(50, (int) m_srd->FullFrameCount);
    EXPECT_EQ(39, (int) m_srd->FullFrameInterval);
    EXPECT_EQ(917312, (int) m_srd->svcDCBytes);
    EXPECT_EQ(1990557 , (int) m_srd->svcQuadBytes);
    EXPECT_EQ(702086, (int) m_srd->svcRemBytes);
}



TEST_F(SRD, Codec2StripRemLayer)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);
    SRDSetSVCProcessingMode(m_srd, SRD_SVC_STRIP_REM_LAYER);

    ProcessDataFile();

    //PrintSRDStats();

    // Check result using md5sum of output
    CheckMD5SumOfResult("1f4bdfa0fe6125187a2ef34af4f700dd");

    // Check SRD counters
    EXPECT_EQ(3647, (int) m_srd->inPktCount);
    EXPECT_EQ(3550, (int) m_srd->outPktCount);
    EXPECT_EQ(1831, (int) m_srd->inFrameCount);
    EXPECT_EQ(1379, (int) m_srd->outFrameCount);
    EXPECT_EQ(1831, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(452, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(19421, (int) m_srd->inSlices);
    EXPECT_EQ(19391, (int) m_srd->outSlices);
    EXPECT_EQ(20, (int) m_srd->reducedSlices);
    EXPECT_EQ(3733435, (int) m_srd->inBytes);
    EXPECT_EQ(3635200, (int) m_srd->outBytes);
    EXPECT_EQ(3632, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(620098, (int) m_srd->padBytes);
    EXPECT_EQ(3, (int) m_srd->IFrameCount);
    EXPECT_EQ(3, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(50, (int) m_srd->FullFrameCount);
    EXPECT_EQ(39, (int) m_srd->FullFrameInterval);
    EXPECT_EQ(917312, (int) m_srd->svcDCBytes);
    EXPECT_EQ(1990557 , (int) m_srd->svcQuadBytes);
    EXPECT_EQ(702086, (int) m_srd->svcRemBytes);
}



TEST_F(SRD, Codec2StripQuadLayer)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);
    SRDSetSVCProcessingMode(m_srd, SRD_SVC_STRIP_QUAD_LAYER);

    ProcessDataFile();

    // Check result using md5sum of output
    CheckMD5SumOfResult("6882de8e138e0009b76f895b397594af");

    // Check SRD counters
    EXPECT_EQ(3647, (int) m_srd->inPktCount);
    EXPECT_EQ(1890, (int) m_srd->outPktCount);
    EXPECT_EQ(1831, (int) m_srd->inFrameCount);
    EXPECT_EQ(1817, (int) m_srd->outFrameCount);
    EXPECT_EQ(1831, (int) m_srd->inRightFrameCount);
    EXPECT_EQ(0, (int) m_srd->inLeftFrameCount);
    EXPECT_EQ(14, (int) m_srd->reducedFrameCount);
    EXPECT_EQ(19421, (int) m_srd->inSlices);
    EXPECT_EQ(19414, (int) m_srd->outSlices);
    EXPECT_EQ(0, (int) m_srd->reducedSlices);
    EXPECT_EQ(3733435, (int) m_srd->inBytes);
    EXPECT_EQ(1935360, (int) m_srd->outBytes);
    EXPECT_EQ(0, (int) m_srd->reducedBytes);
    EXPECT_EQ(0, (int) m_srd->prefillFrameCount);
    EXPECT_EQ(0, (int) m_srd->reducedAltSlices);
    EXPECT_EQ(895361, (int) m_srd->padBytes);
    EXPECT_EQ(3, (int) m_srd->IFrameCount);
    EXPECT_EQ(3, (int) m_srd->OutputIFrameCount);
    EXPECT_EQ(50, (int) m_srd->FullFrameCount);
    EXPECT_EQ(39, (int) m_srd->FullFrameInterval);
    EXPECT_EQ(917312, (int) m_srd->svcDCBytes);
    EXPECT_EQ(1990557 , (int) m_srd->svcQuadBytes);
    EXPECT_EQ(702086, (int) m_srd->svcRemBytes);

}

/*  Boundary check for Start , Middle & End of SOS in packet */
TEST_F(SRD, Codec2NormalVarPakLen)
{

    for (int k = 0; k < 8; k++) {
        ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

        SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

        /* size1 is the size  for checking SOS at start/mid/end of the packet
         * size2 is the size of next SOF+ some bytes extra*/

        ProcessDataVarChunkSize(2382 + k, 343048);
        CheckMD5SumOfResult("da8897d1574e22d89a4b93339a7f4a3d");

        destroySRD(m_srd);
        CloseDataFile();
        CreateSRD();
    }
}


/*  Boundary check for Start , Middle & End of SOF in packet */
TEST_F(SRD, BoundaryCheckForSOF)
{

    for (int k = 0; k < 8; k++) {
        ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));
        SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

        /* size1 is the size  for checking SOS at start/mid/end of the packet
         * size2 is the size of next SOF+ some bytes extra*/

        ProcessDataVarChunkSize(2428 + k, 4924);
        CheckMD5SumOfResult("545dc8db4f3063657dfedc24835c6a91");

        destroySRD(m_srd);
        CloseDataFile();
        CreateSRD();
    }
}


/* Before a new frame starts a End of Frame(EOF) and necessary
 * 4 byte boundary is padded for new frame to start
 */
TEST_F(SRD, EOFCheckWithCallback)
{

    ASSERT_TRUE(OpenDataFile("codec_2_basic.dat"));

    SRDSetCodecVersion(m_srd, CODEC_VERSION_SVC);

    // Install callback handler
    SRDSetNewFrameCallback(m_srd, SRDNewFrameCallbackEOF, this);

    ProcessDataFile();
    CheckMD5SumOfResult("9dbf60cefd734b728798711a186e727d");

}
