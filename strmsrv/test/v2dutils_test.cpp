/*
 * v2dutils_test.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rkale
 */

#include "V2DUtils.hh"
#include <gtest/gtest.h>

using namespace std;

TEST(V2DUtilsTest, CreateLutEntry1024x768) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(1024, 768, &lut);

    EXPECT_EQ(1024, lut.a_hres);
    EXPECT_EQ(768, lut.a_vres);
    EXPECT_EQ(1024, lut.a_orig_hres);
    EXPECT_EQ(768, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(60.0, lut.a_refresh);
    EXPECT_FLOAT_EQ(65.0, lut.a_pcr);
    EXPECT_EQ(24, lut.a_hfp);
    EXPECT_EQ(160, lut.a_hbp);
    EXPECT_EQ(136, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(29, lut.a_vbp);
    EXPECT_EQ(6, lut.a_vsync);
    EXPECT_STREQ("VESA 1024x768@60.0Hz", lut.comment);

}


TEST(V2DUtilsTest, CreateLutEntry1920x1200) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(1920, 1200, &lut);

    EXPECT_EQ(1920, lut.a_hres);
    EXPECT_EQ(1200, lut.a_vres);
    EXPECT_EQ(1920, lut.a_orig_hres);
    EXPECT_EQ(1200, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.95, lut.a_refresh);
    EXPECT_FLOAT_EQ(154.0, lut.a_pcr);
    EXPECT_EQ(48, lut.a_hfp);
    EXPECT_EQ(80, lut.a_hbp);
    EXPECT_EQ(32, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(26, lut.a_vbp);
    EXPECT_EQ(6, lut.a_vsync);
    EXPECT_STREQ("CVT-RB 1920x1200@60.0Hz", lut.comment);

}

TEST(V2DUtilsTest, CreateLutEntry2560x1600) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(2560, 1600, &lut);

    EXPECT_EQ(2560, lut.a_hres);
    EXPECT_EQ(1600, lut.a_vres);
    EXPECT_EQ(2560, lut.a_orig_hres);
    EXPECT_EQ(1600, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.972, lut.a_refresh);
    EXPECT_FLOAT_EQ(268.5, lut.a_pcr);
    EXPECT_EQ(48, lut.a_hfp);
    EXPECT_EQ(80, lut.a_hbp);
    EXPECT_EQ(32, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(37, lut.a_vbp);
    EXPECT_EQ(6, lut.a_vsync);
    EXPECT_STREQ("CVT-RB 2560x1600@60.0Hz", lut.comment);
}

TEST(V2DUtilsTest, CreateLutEntry3840x1080) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(3840, 1080, &lut);

    EXPECT_EQ(3840, lut.a_hres);
    EXPECT_EQ(1080, lut.a_vres);
    EXPECT_EQ(3840, lut.a_orig_hres);
    EXPECT_EQ(1080, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.93, lut.a_refresh);
    EXPECT_FLOAT_EQ(277.0, lut.a_pcr);
    EXPECT_EQ(64, lut.a_hfp);
    EXPECT_EQ(192, lut.a_hbp);
    EXPECT_EQ(64, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(19, lut.a_vbp);
    EXPECT_EQ(9, lut.a_vsync);
    EXPECT_STREQ("MTX0511 3840x1080@59.9Hz", lut.comment);
}

TEST(V2DUtilsTest, CreateLutEntry3840x1200) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(3840, 1200, &lut);

    EXPECT_EQ(3840, lut.a_hres);
    EXPECT_EQ(1200, lut.a_vres);
    EXPECT_EQ(3840, lut.a_orig_hres);
    EXPECT_EQ(1200, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.95, lut.a_refresh);
    EXPECT_FLOAT_EQ(308.0, lut.a_pcr);
    EXPECT_EQ(64, lut.a_hfp);
    EXPECT_EQ(192, lut.a_hbp);
    EXPECT_EQ(64, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(22, lut.a_vbp);
    EXPECT_EQ(10, lut.a_vsync);
    EXPECT_STREQ("MTX0511 3840x1200@60.0Hz", lut.comment);
}

TEST(V2DUtilsTest, CreateLutEntry3360x1050) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(3360, 1050, &lut);

    EXPECT_EQ(3360, lut.a_hres);
    EXPECT_EQ(1056, lut.a_vres);
    EXPECT_EQ(3360, lut.a_orig_hres);
    EXPECT_EQ(1050, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.88, lut.a_refresh);
    EXPECT_FLOAT_EQ(238.0, lut.a_pcr);
    EXPECT_EQ(64, lut.a_hfp);
    EXPECT_EQ(192, lut.a_hbp);
    EXPECT_EQ(64, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(19, lut.a_vbp);
    EXPECT_EQ(8, lut.a_vsync);
    EXPECT_STREQ("MTX0511 3360x1050@59.9Hz", lut.comment);
}

TEST(V2DUtilsTest, CreateLutEntry2048x1152) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(2048, 1152, &lut);

    EXPECT_EQ(2048, lut.a_hres);
    EXPECT_EQ(1152, lut.a_vres);
    EXPECT_EQ(2048, lut.a_orig_hres);
    EXPECT_EQ(1152, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(59.909, lut.a_refresh);
    EXPECT_FLOAT_EQ(156.75, lut.a_pcr);
    EXPECT_EQ(48, lut.a_hfp);
    EXPECT_EQ(80, lut.a_hbp);
    EXPECT_EQ(32, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(25, lut.a_vbp);
    EXPECT_EQ(5, lut.a_vsync);
    EXPECT_STREQ("MAC 2048x1152@59.9Hz", lut.comment);
}


TEST(V2DUtilsTest, CreateLutEntryCloseMatch) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(1020, 765, &lut);

    EXPECT_EQ(1024, lut.a_hres);
    EXPECT_EQ(768, lut.a_vres);
    EXPECT_EQ(1020, lut.a_orig_hres);
    EXPECT_EQ(765, lut.a_orig_vres);
    EXPECT_FLOAT_EQ(60.0, lut.a_refresh);
    EXPECT_FLOAT_EQ(65.0, lut.a_pcr);
    EXPECT_EQ(24, lut.a_hfp);
    EXPECT_EQ(160, lut.a_hbp);
    EXPECT_EQ(136, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(29, lut.a_vbp);
    EXPECT_EQ(6, lut.a_vsync);
    EXPECT_STREQ("VESA 1020x765@60.0Hz", lut.comment);
}

TEST(V2DUtilsTest, CreateLutEntryNonStandard) {
    LUT_struct lut;
    V2DUtils::CreateLutEntry(1300, 1000, &lut);

    EXPECT_EQ(1312, lut.a_hres);
    EXPECT_EQ(1008, lut.a_vres);
    EXPECT_EQ(1300, lut.a_orig_hres);
    EXPECT_EQ(1000, lut.a_orig_vres);
    EXPECT_NEAR(59.94, lut.a_refresh, 0.01);
    EXPECT_NEAR(91.5, lut.a_pcr, 0.01);
    EXPECT_EQ(48, lut.a_hfp);
    EXPECT_EQ(80, lut.a_hbp);
    EXPECT_EQ(32, lut.a_hsync);
    EXPECT_EQ(3, lut.a_vfp);
    EXPECT_EQ(16, lut.a_vbp);
    EXPECT_EQ(10, lut.a_vsync);
    EXPECT_STREQ("CVT 1300x1000@59.9Hz", lut.comment);
}
