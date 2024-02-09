#include "fpga.h"
#include "slice_size.h"

static int slices_per_frame(int width, int height, int sliceSize) {
    return ((width * height) >> 6 )/(sliceSize);
}

static int slices_fit_in_frame(int width, int height, int sliceSize) {
    if ((((width * height) >> 7) % sliceSize) == 0)
        return 1;
    return 0;
}

static int compute_min_slice_size(int width, int height) {
    int min_slice_size = (width * height + 2047 * 64 - 1)/(2047 * 64);
    min_slice_size  = (min_slice_size >> 1) << 1;
    return min_slice_size;
}

static int compute_slice_size_internal(int width, int height, int * slice_size,
        int * encoded_width, int * encoded_height, int isI50)
{
    int sliceSize = MAX_NOBPSLICE;
    int max_nobp_slice;
    int min_nobp_slice;
    int size;
    int found = 0;
    int encodedWidth;
    int encodedHeight;
    int slicesPerFrame;
    int retry_count = 0;

    if (isI50) {
        sliceSize = I50_MAX_NOBPSLICE;
        max_nobp_slice = I50_MAX_NOBPSLICE;
        min_nobp_slice = I50_MIN_NOBPSLICE;
    }
    else {
        sliceSize = MAX_NOBPSLICE;
        max_nobp_slice = MAX_NOBPSLICE;
        min_nobp_slice = MIN_NOBPSLICE;
    }

    // Encoded width should be multiple of 16 and
    // Encoded height should be multiple of 8
    encodedWidth = ((width + 15) >> 4 ) << 4;
    encodedHeight = ((height + 7) >> 3 ) << 3;

    while (retry_count++ < 16) {
        for (size = max_nobp_slice; size >= min_nobp_slice; size -= 2) {
            if (slices_fit_in_frame(encodedWidth, encodedHeight, size)) {
                sliceSize = size;
                found = 1;
                break;
            }
        }

        slicesPerFrame = slices_per_frame(encodedWidth, encodedHeight, sliceSize);

        if (!found && slicesPerFrame <= MAX_NUM_SLICES_PER_FRAME) {
            // Retry with expanded height
            encodedHeight += 8;
            continue;
        }
    }

    slicesPerFrame = slices_per_frame(encodedWidth, encodedHeight, sliceSize);
    if (slicesPerFrame > MAX_NUM_SLICES_PER_FRAME) {
        int retry_count = 0;
        while (retry_count++ < 16) {
            // We need a bigger slice size so that number of slices
            // per frame is 2047 or less
            int min_slice_size = compute_min_slice_size(encodedWidth, encodedHeight);

            // Search upwards from min slice size to double of min size
            found = 0;
            for (size = min_slice_size; size < min_slice_size * 2; size += 2) {
                if (slices_fit_in_frame(encodedWidth, encodedHeight, size)) {
                    sliceSize = size;
                    slicesPerFrame = slices_per_frame(encodedWidth, encodedHeight, sliceSize);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                encodedHeight += 8;
                continue;
            }
        }
    }

    // Setup return values
    *slice_size = sliceSize;
    *encoded_width = encodedWidth;
    *encoded_height = encodedHeight;

    return (found) ? 0 : -1;
}

void compute_svc_slice_size(int width, int height, int *slice_size, int *slice_width, int *slice_height,
        int * encoded_width, int * encoded_height) {

    int encodedWidth;
    int encodedHeight;

    *slice_width = 8;
    *slice_height = 4;
    *slice_size = 32;// dummy value assigment ;

    encodedWidth =  width   ;// (int) (width / 16) * 16;
    encodedHeight = height  ;// (int) (height / 8) * 8;

    while (encodedWidth % (*slice_width * 8) != 0)
        encodedWidth += 1;

    while (encodedHeight % (*slice_height * 8) != 0)
        encodedHeight += 1;

    *encoded_width = encodedWidth;
    *encoded_height = encodedHeight;
}

int compute_i50_slice_size(int width, int height, int * slice_size,
        int * encoded_width, int * encoded_height) {
    return compute_slice_size_internal(width, height, slice_size, encoded_width, encoded_height, 1);
}

int compute_slice_size(int width, int height, int * slice_size,
        int * encoded_width, int * encoded_height) {
    return compute_slice_size_internal(width, height, slice_size, encoded_width, encoded_height, 0);
}
