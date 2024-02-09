#include <gtest/gtest.h>
#include "slice_size.h"
#include "custmenu.h"


TEST(SliceSizeTest, SliceSize) {
    int slice_size;
    int encoded_width;
    int encoded_height;
    int slices_per_frame;

    // Standard resolutions needing slice size 36 or less
    compute_slice_size(320, 240, &slice_size,
            &encoded_width ,&encoded_height);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(320, encoded_width);
    EXPECT_EQ(240, encoded_height);

    compute_slice_size(640, 480, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(640, encoded_width);
    EXPECT_EQ(480, encoded_height);

    compute_slice_size(800, 600, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(800, encoded_width);
    EXPECT_EQ(600, encoded_height);

    compute_slice_size(1024, 768, &slice_size,
           &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(1024, encoded_width);
    EXPECT_EQ(768, encoded_height);

    compute_slice_size(1280, 1024, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(1280, encoded_width);
    EXPECT_EQ(1024, encoded_height);

    compute_slice_size(1400, 1052, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(1408, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_slice_size(1600, 1200, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(1600, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    compute_slice_size(1920, 1080, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1080, encoded_height);

    // Test for I50, slice size should be 30
    compute_i50_slice_size(1920, 1080, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(30, slice_size);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1080, encoded_height);

    compute_slice_size(1920, 1200, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    compute_slice_size(2048, 1152, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(2048, encoded_width);
    EXPECT_EQ(1152, encoded_height);

    compute_slice_size(2560, 1600, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(2560, encoded_width);
    EXPECT_EQ(1600, encoded_height);

    compute_slice_size(3360, 1050, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(3360, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_slice_size(3840, 1200, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(1200, encoded_height);

    // Large resolutions that need larger than 36 slice size
    compute_slice_size(3840, 2160, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(72, slice_size);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2160, encoded_height);

    compute_slice_size(3840, 2400, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(72, slice_size);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2400, encoded_height);

    compute_slice_size(5040, 1050, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(42, slice_size);
    EXPECT_EQ(5040, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_slice_size(5040, 2100, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(84, slice_size);
    EXPECT_EQ(5040, encoded_width);
    EXPECT_EQ(2112, encoded_height);

    compute_slice_size(3200, 2400, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(60, slice_size);
    EXPECT_EQ(3200, encoded_width);
    EXPECT_EQ(2400, encoded_height);

    // Weird/random resolutions

    // This one needs expanded height to match in less than 36 range
    compute_slice_size(2540, 1426, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(36, slice_size);
    EXPECT_EQ(2544, encoded_width);
    EXPECT_EQ(1440, encoded_height);

    // This random dimension needs expansion by 43 pixels
    compute_slice_size(3340, 3077, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(110, slice_size);
    EXPECT_EQ(3344, encoded_width);
    EXPECT_EQ(3120, encoded_height);

    // This random dimension needs expansion by 62 pixels in the less than 36 range
    compute_slice_size(969, 3266, &slice_size,
            &encoded_width, &encoded_height);
    EXPECT_EQ(32, slice_size);
    EXPECT_EQ(976, encoded_width);
    EXPECT_EQ(3328, encoded_height);


}

TEST(SvcSliceSizeTest, EncodedWidthHeight) {
    int slice_size;
    int encoded_width;
    int encoded_height;
    int slices_per_frame;
    int slice_width; 
    int slice_height;

    compute_svc_slice_size(320, 240, &slice_size, &slice_width, &slice_height,
            &encoded_width ,&encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(320, encoded_width);
    EXPECT_EQ(256, encoded_height);

    compute_svc_slice_size(640, 480, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(640, encoded_width);
    EXPECT_EQ(480, encoded_height);

    compute_svc_slice_size(800, 600, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(832, encoded_width);
    EXPECT_EQ(608, encoded_height);

    compute_svc_slice_size(1024, 768, &slice_size, &slice_width, &slice_height,
           &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1024, encoded_width);
    EXPECT_EQ(768, encoded_height);

    compute_svc_slice_size(1280, 1024, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1280, encoded_width);
    EXPECT_EQ(1024, encoded_height);

    compute_svc_slice_size(1400, 1052, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1408, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_svc_slice_size(1600, 1200, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1600, encoded_width);
    EXPECT_EQ(1216, encoded_height);

    compute_svc_slice_size(1920, 1080, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1088, encoded_height);

    compute_svc_slice_size(1920, 1200, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1920, encoded_width);
    EXPECT_EQ(1216, encoded_height);

    compute_svc_slice_size(2048, 1152, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(2048, encoded_width);
    EXPECT_EQ(1152, encoded_height);

    compute_svc_slice_size(2560, 1600, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(2560, encoded_width);
    EXPECT_EQ(1600, encoded_height);

    compute_svc_slice_size(3360, 1050, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3392, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_svc_slice_size(3840, 1200, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(1216, encoded_height);

    compute_svc_slice_size(3840, 2160, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2176, encoded_height);

    compute_svc_slice_size(3840, 2400, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3840, encoded_width);
    EXPECT_EQ(2400, encoded_height);

    compute_svc_slice_size(5040, 1050, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(5056, encoded_width);
    EXPECT_EQ(1056, encoded_height);

    compute_svc_slice_size(5040, 2100, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(5056, encoded_width);
    EXPECT_EQ(2112, encoded_height);

    compute_svc_slice_size(3200, 2400, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3200, encoded_width);
    EXPECT_EQ(2400, encoded_height);


    compute_svc_slice_size(2540, 1426, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(2560, encoded_width);
    EXPECT_EQ(1440, encoded_height);

    compute_svc_slice_size(3340, 3077, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(3392, encoded_width);
    EXPECT_EQ(3104, encoded_height);

    compute_svc_slice_size(969, 3266, &slice_size, &slice_width, &slice_height,
            &encoded_width, &encoded_height);
    EXPECT_EQ(8, slice_width);
    EXPECT_EQ(4, slice_height);
    EXPECT_EQ(1024, encoded_width);
    EXPECT_EQ(3296, encoded_height);


}

