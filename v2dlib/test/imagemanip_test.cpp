#include "v2dlib_compatibility.h"
#include "codecdefs.h"
#include "imagemanip.h"
#include "test_utils.hh"
#include <gtest/gtest.h>
#include "memory_tracker.h"

using namespace std;
using namespace V2DLib;

TEST(ImageManip, TransposeAndAdd128AndPutBlock) {

    unsigned char cblk[V2D_BLOCK_SIZE];
    ::memset(cblk, 0, V2D_BLOCK_SIZE);

    short sblk[V2D_BLOCK_SIZE];
    for(size_t i = 0; i < V2D_BLOCK_SIZE; i++) sblk[i] = i;

    transposeAndAdd128AndPutBlock(sblk, cblk);

    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
            unsigned char x = cblk[8*i + j] - 128;
            ASSERT_EQ( 8*j + i, x);
#if 0 // Turn on for debugging
            std::cout << "cblk[" << (8*i + j) << "]" << "=" << (int)x 
                << std::endl; 
#endif
        }
    }

}

TEST(ImageManip, CopyBlockToImageInverted) {

    const size_t nWidth = 800, nHeight = 600;
    unsigned char* pImgBuff = new unsigned char[nWidth*nHeight];
    ::memset(pImgBuff, 0, nWidth*nHeight);

    unsigned char cblk[V2D_BLOCK_SIZE];
    for(size_t i = 0; i < V2D_BLOCK_SIZE; i++) cblk[i] = i;

    copyBlockToImageInverted(cblk, pImgBuff, nWidth);

    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
#if 0  // Turn on for debugging
            std::cout << "pImg[" << (nWidth*(7-i) + j) << "]" << " = " <<
                (short)pImgBuff[nWidth*(7-i) + j] << std::endl;
#endif
            short nDiff = pImgBuff[nWidth*(7-i) + j] - cblk[8*i + j];
            ASSERT_EQ(0, nDiff);
        }

    }
    delete [] pImgBuff;
}

TEST(ImageManip, CopyBlockToImage) {

    const size_t nWidth = 800, nHeight = 600;
    unsigned char* pImgBuff = new unsigned char[nWidth*nHeight];

    ::memset(pImgBuff, 0, nWidth*nHeight);

    unsigned char cblk[V2D_BLOCK_SIZE];
    for(size_t i = 0; i < V2D_BLOCK_SIZE; i++) cblk[i] = i;

    copyBlockToImage(cblk, pImgBuff, nWidth);

    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
#if 0  // Turn on for debugging
            std::cout << "pImg[" << (nWidth*i + j) << "]" << " = " <<
                (short)pImgBuff[nWidth*i + j] << std::endl;
#endif
            short nDiff = pImgBuff[nWidth*i + j] - cblk[8*i + j];
            ASSERT_EQ(0, nDiff);
        }

    }
    delete [] pImgBuff;
}


TEST(ImageManip, SliceSize) {
    int slice_size;
    int encoded_width;
    int encoded_height;
    int slices_per_frame;

    // Standard resolutions needing slice size 36 or less
    computeSliceSize(320, 240, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(40, slices_per_frame);
    EXPECT_EQ(320, encoded_width);
    EXPECT_EQ(240, encoded_height);

    computeSliceSize(640, 480, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(150, slices_per_frame);
    EXPECT_EQ(640, encoded_width);
    EXPECT_EQ(480, encoded_height);

    computeSliceSize(800, 600, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(250, slices_per_frame);
    EXPECT_EQ(800, encoded_width);
    EXPECT_EQ(600, encoded_height);

    computeSliceSize(1024, 768, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(384, slices_per_frame);
    EXPECT_EQ(1024, encoded_width);
    EXPECT_EQ(768, encoded_height);

    computeSliceSize(1280, 1024, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(640, slices_per_frame);
    EXPECT_EQ(1280, encoded_width);
    EXPECT_EQ(1024, encoded_height);

    computeSliceSize(1400, 1052, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(726, slices_per_frame);
    EXPECT_EQ(1408, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    computeSliceSize(1600, 1200, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(1000, slices_per_frame);
    EXPECT_EQ(1600, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    computeSliceSize(1920, 1080, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(900, slices_per_frame);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1080, encoded_height);

    computeSliceSize(1920, 1200, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1000, slices_per_frame);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    computeSliceSize(2048, 1152, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1024, slices_per_frame);
    EXPECT_EQ(2048, encoded_width);
    EXPECT_EQ(1152, encoded_height);

    computeSliceSize(2560, 1600, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(2000, slices_per_frame);
    EXPECT_EQ(2560, encoded_width);
    EXPECT_EQ(1600, encoded_height);

    computeSliceSize(3360, 1050, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1540, slices_per_frame);
    EXPECT_EQ(3360, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    computeSliceSize(3840, 1200, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(2000, slices_per_frame);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    // Large resolutions that need larger than 36 slice size
    computeSliceSize(3840, 2160, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(72, slice_size);
    EXPECT_EQ(1800, slices_per_frame);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2160, encoded_height);

    computeSliceSize(3840, 2400, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(72, slice_size);
    EXPECT_EQ(2000, slices_per_frame);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2400, encoded_height);

    computeSliceSize(5040, 1050, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(42, slice_size);
    EXPECT_EQ(1980, slices_per_frame);
    EXPECT_EQ(5040, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    computeSliceSize(5040, 2100, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(84, slice_size);
    EXPECT_EQ(1980, slices_per_frame);
    EXPECT_EQ(5040, encoded_width);
    EXPECT_EQ(2112, encoded_height);

    // Weird/random resolutions

    // This one needs expanded height to match in less than 36 range
    computeSliceSize(2540, 1426, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1590, slices_per_frame);
    EXPECT_EQ(2544, encoded_width);
    EXPECT_EQ(1440, encoded_height);

    // This random dimension needs expansion by 43 pixels
    computeSliceSize(3340, 3077, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(110, slice_size);
    EXPECT_EQ(1482, slices_per_frame);
    EXPECT_EQ(3344, encoded_width);
    EXPECT_EQ(3120, encoded_height);

    // This random dimension needs expansion by 62 pixels in the less than 36 range
    computeSliceSize(969, 3266, slice_size,
            encoded_width, encoded_height, slices_per_frame);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(1586, slices_per_frame);
    EXPECT_EQ(976, encoded_width);
    EXPECT_EQ(3328, encoded_height);

}

TEST(ImageManip, ComputeTimings) {
    float pixel_clock, refresh_rate;
    int hfp, hbp, hsync;
    int vfp, vbp, vsync;

    computeVideoTimings(1920, 1200,
            refresh_rate, pixel_clock,
            hfp, hbp, hsync,
            vfp, vbp, vsync);
    EXPECT_NEAR(59.95, refresh_rate, .01);
    EXPECT_NEAR(154.0, pixel_clock, .01);
    EXPECT_EQ(48, hfp);
    EXPECT_EQ(80, hbp);
    EXPECT_EQ(32, hsync);
    EXPECT_EQ(3,  vfp);
    EXPECT_EQ(26, vbp);
    EXPECT_EQ(6,  vsync);


    computeVideoTimings(2560, 1600,
            refresh_rate, pixel_clock,
            hfp, hbp, hsync,
            vfp, vbp, vsync);
    EXPECT_NEAR(59.972, refresh_rate, .01);
    EXPECT_NEAR(268.5, pixel_clock, .01);
    EXPECT_EQ(48, hfp);
    EXPECT_EQ(80, hbp);
    EXPECT_EQ(32, hsync);
    EXPECT_EQ(3,  vfp);
    EXPECT_EQ(37, vbp);
    EXPECT_EQ(6,  vsync);

}
