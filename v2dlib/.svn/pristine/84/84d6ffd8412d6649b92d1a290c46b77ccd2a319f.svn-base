/*
 * slice_creator.cpp
 *
 *  Created on: Oct 23, 2009
 *      Author: rkale
 */

//#include "comp_utils.h"

#include "v2dlib_compatibility.h"
#include <string>
#include <sstream>

#include "bitmap.hh"
#include "sliceencoder.hh"
#include "slicedecoder.hh"
#include "comp_utils.h"
#include "dct.hh"
#include "ipputils.h"
#include "colortransform.h"
#include "imagemanip.h"

using namespace std;

// Simple utility to create samples for testing
// slice encoding/decoding functionality

void createDebugPattern(unsigned char * inBuff, int width, int height)
{
    int i, j, k, l;
    unsigned char *cp;

    int hres = width;
    int vres = height;

    // Input is in BGRA format
    cp = inBuff;
    for (i = 0; i < vres; i++) {
        k = 0;
        for (j = 0; j < hres; j++) {
            // alternating pixels of black and white
            l = 1 << 3;
            if (j % l == 0)
                k = ~k;
            if (k) {
                *cp++ = 255;
                *cp++ = 255;
                *cp++ = 255;
            }
            else {
                *cp++ = 0;
                *cp++ = 0;
                *cp++ = 0;
            }

            *cp++ = 0;

        }
    }

    return;
}

void createGradientPattern(unsigned char * inBuff, int width, int height)
{
    int i, j;
    unsigned char *cp;

    int hres = width;
    int vres = height;

    // Input is in BGRA format
    cp = inBuff;
    for (i = 0; i < vres; i++) {
        for (j = 0; j < hres; j++) {
            *cp++ = (int)((255.0 * i)/vres);
            *cp++ = 255 - (int)((255.0 * i)/vres);
            *cp++ = (int)((255.0 * j)/hres);
            *cp++ = 0;

        }
    }
}

static void createRandomPattern(unsigned char * inBuff, int width, int height,
        int random_seed)
{
    int i, j;
    unsigned char *cp;

    int hres = width;
    int vres = height;

    // Input is in BGRA format

    srandom(random_seed);

    cp = inBuff;
    for (i = 0; i < vres; i++) {
        for (j = 0; j < hres; j++) {
            *cp++ = random() % 255;
            *cp++ = random() % 255;
            *cp++ = random() % 255;

            *cp++ = 0;
        }
    }
}


static int compress_pattern(unsigned char *outBuff, unsigned char *inBuff, int width,
        int height, int mquant, int dcquant, int sliceSize)
{
    printf("Compressing Slice Legacy SliceSize = %d, mquant = %d, dcquant = %d...\n",
            sliceSize, mquant, dcquant);
    initFrameParams(width, height, mquant, dcquant, sliceSize);
    return encodeOneSlice(inBuff, (char *) outBuff);
}

static int compress_pattern_real(unsigned char *outBuff, unsigned char *inBuff, int width,
        int height, int mquant, int dcquant, int sliceSize)
{
    printf("Compressing Slice Real SliceSize = %d, mquant = %d, dcquant = %d...\n",
            sliceSize, mquant, dcquant);
    CSliceEncoder *sliceEncoder = CSliceEncoder::CreateSliceEncoder(mquant, dcquant, sliceSize);
    int nEncodeLen = sliceEncoder->EncodeSlice(outBuff, inBuff, width);
    delete sliceEncoder;
    return nEncodeLen;
}

static unsigned long long getmillisec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long) tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

#define ARRAYSIZE 16
static void measure_performance(int loops, int sliceSize, int mquant,
        int dcquant)
{
    unsigned char *inBuffArray[ARRAYSIZE];
    unsigned char *outBuff;
    int width = sliceSize * 8;
    int height = 8;
    int i;
    int random_seed = 5;
    int len;
    int number_of_loops = loops;
    unsigned long long starttime;
    unsigned long long endtime;

    (void)random_seed; // suppress warnings

    // Allocate space for input/output buffers
    for (i = 0; i < ARRAYSIZE; i++) {
        inBuffArray[i] = new unsigned char[width * height * 4];
    }
    outBuff = new unsigned char[width * height * 24];

    // Create patterns for encoding
    for (i = 0; i < ARRAYSIZE; i++) {
        //createRandomPattern(inBuffArray[i], width, height, random_seed++);
        //createDebugPattern(inBuffArray[i], width, height);
        createGradientPattern(inBuffArray[i], width, height);
    }

    CSliceEncoder *sliceEncoder = CSliceEncoder::CreateSliceEncoder(mquant, dcquant, sliceSize);

    printf("Performance test: Slice size: %d, mquant=%d, dcquant=%d\n",
            sliceSize, mquant, dcquant);
#define DECODE_TEST
    uint32_t totalSize = 0;
#ifdef DECODE_TEST
    len = sliceEncoder->EncodeSlice(outBuff, inBuffArray[0], width);
    printf("Encoded slice to size %d\n", len);
    printf("Decode Performance: Loops: %d\n", number_of_loops);
    unsigned char *pY = new unsigned char[width * height + 32];
    unsigned char *pU = new unsigned char[width * height + 32];
    unsigned char *pV = new unsigned char[width * height + 32];
    while ((int)pY % 32 != 0) pY++;
    while ((int)pU % 32 != 0) pU++;
    while ((int)pV % 32 != 0) pV++;
    CSliceDecoder SliceDecoder;
    int scaleFactor = 2;
    SliceDecoder.SetScaleFactor(scaleFactor);
    int nSliceNumber = 0;
    starttime = getmillisec();
    for (i = 0; i < number_of_loops; i++) {
        SliceDecoder.Decode(outBuff, len, sliceSize,
                pY, pU, pV, nSliceNumber);
        totalSize += len;
    }
    endtime = getmillisec();
#else
    printf("Encode Performance: Loops: %d\n", number_of_loops);
    totalSize = 0;
    starttime = getmillisec();
    for (i = 0; i < number_of_loops; i++) {
        len = sliceEncoder->EncodeSlice(outBuff, inBuffArray[i% ARRAYSIZE], width);
        totalSize += len;
    }
    endtime = getmillisec();
#endif

    // Report time spent. Per frame cost is approximated by
    // 640 slices which would be equivalent to a 1280 x 1024 full frame
    double time_per_slice = (1.0 * (int) (endtime - starttime))
            / (1.0 * number_of_loops);
    printf("Total Time Spent = %d\n", (int)(endtime - starttime));
    printf("Time Spent = %0.2f per slice, %0.2f per frame\n", time_per_slice,
            time_per_slice * 640);
    printf("Size = %u, FPS = %0.2f, BW = %.2f M\n",
            totalSize,
            1000.0/(time_per_slice * 640.0),
            ((double)(totalSize * 8.0 * 1000.0))/((int)(endtime - starttime) * (1024.0 * 1024.0)));

    // Release resources
    delete sliceEncoder;
    for (i = 0; i < ARRAYSIZE; i++) {
        delete[] inBuffArray[i];
    }
    delete[] outBuff;
}

// Experimental testfdct function
static void testfdct()
{
    // Testing against known FDCT result from
    // http://en.wikipedia.org/wiki/JPEG

    short input[64] = {
        -76, -73, -67, -62, -58, -67, -64, -55,
        -65, -69, -73, -38, -19, -43, -59, -56,
        -66, -69, -60, -15,  16, -24, -62, -55,
        -65, -70, -57, -6,   26, -22, -58, -59,
        -61, -67, -60, -24, -2,  -40, -60, -58,
        -49, -63, -68, -58, -51, -60, -70, -53,
        -43, -57, -64, -69, -73, -67, -63, -45,
        -41, -49, -59, -60, -63, -52, -50, -34,
    };

    short output[64] = {
        -415, -30, -61,  27,  56, -20,  -2,   0,
           4, -22, -61,  10,  13,  -7,  -9,   5,
         -47,   7,  77, -25, -29,  10,   5,  -6,
         -49,  12,  34, -15, -10,   6,   2,   2,
          12,  -7, -13,  -4,  -2,   2,  -3,   3,
          -8,   3,   2,  -6,  -2,   1,   4,   2,
          -1,   0,   0,  -2,  -1,  -3,   4,  -1,
           0,   0,  -1,  -4,  -1,   0,   1,   2,
    };

    // Increase following loop count for quick performance measurements
    for (int i = 0; i < 1; i++) {
        CFDCT::FDCT(input);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("% 4d, ", input[i * 8 + j]);
//            printf("% 4d, ", (int)round((int)(input[i * 8 + j] * 1.0)/8.0));
        }
        printf("\n");
    }

    if (memcmp(input, output, sizeof(short) * 64 ) == 0) {
        printf("FDCT Test Ok\n");
    }
    else {
        printf("FDCT Test Failed\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("% 4d, ", output[i * 8 + j]);
            }
            printf("\n");
        }

    }
}


// Encode and decode a slice filled with random patterns.
// Add printfs or (use gdb etc) in the slice encoder/decoder to
// study/debug what is happening
static void debug_encode_decode()
{
    int sliceSize = 32;
    int mquant = 1;
    int dcquant = 1;
    int width = sliceSize * 8;
    int height = 8;
    int random_seed = 5;
    int len;

    (void)random_seed;

    // Allocate space for input/output buffers
    unsigned char *inBuff = new unsigned char[width * height * 4];
    unsigned char *outBuff = new unsigned char[width * height * 4];

    // Create pattern for encoding
    createRandomPattern(inBuff, width, height, random_seed);
    //createGradientPattern(inBuff, width, height);
    //createDebugPattern(inBuff, width, height);

    CBitmap oSaveBmp;
    oSaveBmp.SetBitsBGRA(inBuff, width, height);
    ostringstream ss;
    ss << "slice";
    ss << "_" << width << "_" << height << ".bmp";
    oSaveBmp.Save(ss.str().c_str(), 32);

    CSliceEncoder *sliceEncoder = CSliceEncoder::CreateSliceEncoder(mquant, dcquant, sliceSize);

    len = sliceEncoder->EncodeSlice(outBuff, inBuff, width, 23);
    printf("Done Encoding to size %d\n", len);


#if 1
    // Decode the just encoded slice
    unsigned char *pY = new unsigned char[width * height];
    unsigned char *pU = new unsigned char[width * height];
    unsigned char *pV = new unsigned char[width * height];
    memset(pY, 0x00, width * height);
    memset(pU, 0x00, width * height);
    memset(pV, 0x00, width * height);
    CSliceDecoder SliceDecoder;
    int scaleFactor = 1;
    SliceDecoder.SetScaleFactor(scaleFactor);
    int nSliceNumber = 0;
    bool bRet = SliceDecoder.Decode(outBuff, len, sliceSize,
                pY, pU, pV, nSliceNumber);
    printf("Done Decoding (%d) sliceNumber : %d\n", bRet, nSliceNumber);


#ifdef NO_LONGER_NEEDED
    unsigned char *pY2 = new unsigned char[width * height];
    unsigned char *pU2 = new unsigned char[width * height];
    unsigned char *pV2 = new unsigned char[width * height];
    memset(pY2, 0, width * height);
    memset(pU2, 0, width * height);
    memset(pV2, 0, width * height);

    unsigned char * yArrayPos = pY2;
    unsigned char * uArrayPos = pU2;
    unsigned char * vArrayPos = pV2;
    using V2dLibImageManip::copyBlockToImage;

    for (int block = 0; block < 32; block++) {
        yArrayPos = pY2 + block * 8;
        uArrayPos = pU2 + block * 4;
        vArrayPos = pV2 + block * 4;
        copyBlockToImage(&pY[block * 64], yArrayPos, width);
        if (block % 2 == 0) {
            copyBlockToImage(&pU[(block/2) * 64], uArrayPos, width/2);
            copyBlockToImage(&pV[(block/2) * 64], vArrayPos, width/2);
        }
    }
#endif

    // Write out the decoded buffer
    int scaledWidth = width/scaleFactor;
    int scaledHeight = height/scaleFactor;
    V2dLibColorTransform::YUVToRGBConverter oRGB;
    memset(inBuff, 0, scaledWidth * scaledHeight * 4);
    oRGB.yuv422ToRgbScaled(pY, pU, pV, inBuff, scaledWidth, scaledHeight,
            1, V2dLibColorTransform::YUVToRGBConverter::ImageFormat_BGRX);

    //V2dLibImageManip::dumpBuffer(inBuff, "color2");

    oSaveBmp.SetBitsBGRA(inBuff, scaledWidth, scaledHeight);
    ostringstream sss;
    sss << "slice";
    sss << "_" << scaledWidth << "_" << scaledHeight << "_decoded.bmp";
    oSaveBmp.Save(sss.str().c_str(), 32);

#endif

    // Release resources
    delete sliceEncoder;
    delete [] inBuff;
    delete [] outBuff;

}

static void usage(char *command)
{
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    printf("\t-n String\tName to use for generated files (default \"out\")\n");
    printf("\t-s Number\tSlice size to use (default 32)\n");
    printf("\t-m Number\tMquant to use (default 6)\n");
    printf("\t-s Number\tDCquant to use (default 6)\n");
    printf("\t-r Number\tUse random pattern with given seed as argument\n");
    printf("\t-p Number\tMeasure performance using these many loops\n");
    printf("\t-h\t\tShow this help\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    unsigned char *inBuff;
    unsigned char *outBuff;
    char ch;
    string sampleName;
    int mquant, dcquant;
    int sliceSize;
    int random_seed;
    int do_perf = 0;
    FILE *fp;

    // Setup default values
    sliceSize = 30;
    mquant = 6;
    dcquant = 6;
    sampleName = "out";
    random_seed = 0;

    InitIPPLib();

    // Check command line options
    while ((ch = getopt(argc, argv, "n:s:m:d:r:p:e")) != -1) {
        switch (ch) {
        case 'n':
            sampleName = optarg;
            break;
        case 's':
            sliceSize = atoi(optarg);
            break;
        case 'm':
            mquant = atoi(optarg);
            break;
        case 'd':
            dcquant = atoi(optarg);
            break;
        case 'r':
            random_seed = atoi(optarg);
            break;
        case 'p':
            do_perf = atoi(optarg);
            break;
        case 'e':
            // Experimental one off code
            if (0) {
                testfdct();
            }
            if (1) {
                debug_encode_decode();
            }
            exit(0);
            break;
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }


    int width = sliceSize * 8;
    int height = 8;

    // Allocate space for input/output buffers
    inBuff = new unsigned char[width * height * 4];
    outBuff = new unsigned char[width * height * 4];

    if (do_perf > 0) {
        measure_performance(do_perf, sliceSize, mquant, dcquant);
        exit(0);
    }

    // Create pattern
    if (random_seed <= 0) {
        createDebugPattern(inBuff, width, height);
    }
    else {
        createRandomPattern(inBuff, width, height, random_seed);
    }

    // Save pattern as a bitmap file
    CBitmap oSaveBmp;
    oSaveBmp.SetBitsBGRA(inBuff, width, height);
    ostringstream ss;
    ss << sampleName;
    if (random_seed > 0)
        ss << "_" << random_seed;
    ss << "_" << sliceSize << ".bmp";
    oSaveBmp.Save(ss.str().c_str(), 32);

#if 0
    {
        // Code to verify BMP file written
        CBitmap oRawBmp(ss.str().c_str());

        unsigned int nRawBmpSize = oRawBmp.GetWidth() * oRawBmp.GetHeight() * 4;

        unsigned char * m_pRawBuf = new unsigned char[nRawBmpSize];
        if (m_pRawBuf == NULL) {
            return -1;
        }

        if (oRawBmp.GetBits(m_pRawBuf, nRawBmpSize, 32) == false) {
            return -1;
        }

        int rc = memcmp(inBuff, m_pRawBuf, nRawBmpSize);
        printf("Memcmp result = %d for %d, %d\n", rc, nRawBmpSize, width
                * height * 4);
    }
#endif

    // Compress pattern using legacy encoder
    int encodeLength = compress_pattern(outBuff, inBuff, width, height, mquant,
            dcquant, sliceSize);

    // Save compressed output
    // No longer need to do this since we use md5sums of the data in the test cases
    int save_output = 0;

    if (save_output) {
        ss.str("");
        ss << sampleName;
        if (random_seed > 0)
            ss << "_" << random_seed;
        ss << "_" << sliceSize << "_" << mquant << "_" << dcquant << ".dat";
        fp = fopen(ss.str().c_str(), "w");
        if (fp != NULL) {
            fwrite(outBuff, encodeLength, 1, fp);
            fclose(fp);
        }
    }
    // Re-encode using new CSliceEncoder class
    encodeLength = compress_pattern_real(outBuff, inBuff, width, height, mquant,
            dcquant, sliceSize);

    // Save compressed output
    if (save_output) {
        ss.str("");
        ss << sampleName;
        if (random_seed > 0)
            ss << "_" << random_seed;
        ss << "_" << sliceSize << "_" << mquant << "_" << dcquant << "_real" << ".dat";
        fp = fopen(ss.str().c_str(), "w");
        if (fp != NULL) {
            fwrite(outBuff, encodeLength, 1, fp);
            fclose(fp);
        }
    }

    delete[] inBuff;
    delete[] outBuff;

    return 0;
}
