#include "v2dlib_compatibility.h"
#include <gtest_utils/gtest_utils.hh>
#include "codecdefs.h"
#include "bitstream.h"
#include "test_utils.hh"
#include <gtest/gtest.h>

TEST(Bitstream, Sanity) {

    std::string strDataDir = GTestUtils::GetDataDir("..");

    std::string strSliceFile = strDataDir + "/video/slices/frame1-slice-1.dat";

    unsigned char buff[20000];
    memset(buff, 0x00, 20000);

    int nBufLen = 0;

    FILE *fp = fopen(strSliceFile.c_str(), "rb");
    if (fp) {
        nBufLen = fread(buff, 1, 20000, fp);
        fclose(fp);
    }
    else {
        FAIL() << "Unable to open input file";
        return;
    }

    // If we do not specify + 4 bitstream runs out of bits and throws exception
    CBitStream bs(buff, nBufLen);

    EXPECT_EQ(0, bs.totalBitsRead());

    EXPECT_EQ(nBufLen, bs.getRemainingBytes());

    unsigned int val1 = 0, val2 = 0, val3 = 0, firstByte = 0, code = 0;
    bs.getBits(8, val1);
    bs.getBits(8, val2);
    bs.getBits(8, val3);

    EXPECT_EQ(nBufLen - 3, bs.getRemainingBytes());

    code = ((firstByte << 24) | (val1 << 16) | (val2 << 8) | val3);   
    EXPECT_EQ(static_cast<unsigned int>(V2D_VIDEO_START_SLICE_CODE), code);

    unsigned int nSliceNum = 0;
    bs.getBits(11, nSliceNum);
    EXPECT_EQ(1U, nSliceNum);

    unsigned int i_frame_d1 = 0;
    bs.getBits(1, i_frame_d1);
    EXPECT_EQ(1U, i_frame_d1);
    
    unsigned int iRSlice = 0;
    bs.getBits(1, iRSlice);
    EXPECT_EQ(1U, iRSlice);
 
    unsigned int nMQuant = 0;
    bs.getBits(3, nMQuant);
    EXPECT_EQ(8, (1 << nMQuant));

    unsigned int nDCQuant = 0;
    bs.getBits(3, nDCQuant);
    EXPECT_EQ(4, (1 << nDCQuant));

    EXPECT_EQ(nBufLen - 5, bs.getRemainingBytes());

    bs.showBits(24, code);
    EXPECT_TRUE(code != V2D_VIDEO_END_SLICE_CODE);

    EXPECT_EQ(nBufLen - 5, bs.getRemainingBytes());

    EXPECT_EQ(43, bs.totalBitsRead());

    bs.flushBits(15);

    bs.showBits(16, code);
    EXPECT_EQ(25297U, code);

    bs.flushBits(7);

    bs.showBits(16, code);
    EXPECT_EQ(26826U, code);
    EXPECT_EQ(65, bs.totalBitsRead());

    bs.flushBitsToStartFourByteBoundary();
    EXPECT_EQ(96, bs.totalBitsRead());

    bs.flushBitsToStartFourByteBoundary();
    EXPECT_EQ(96, bs.totalBitsRead());

    while(bs.totalBitsRead() != (63*8))
        bs.flushBits(1);

    EXPECT_EQ(nBufLen - 63, bs.getRemainingBytes());

    bs.getBits(8, val1);
    bs.getBits(8, val2);
    bs.getBits(8, val3);

    code = ((firstByte << 24) | (val1 << 16) | (val2 << 8) | val3);  
    EXPECT_EQ(static_cast<unsigned int>(V2D_VIDEO_END_SLICE_CODE), code);
}

