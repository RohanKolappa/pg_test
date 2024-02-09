/*
 * frame_creator.cpp
 *
 *  Created on: Oct 30, 2009
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include <string>
#include <sstream>

#include "bitmap.hh"
#include "png.hh"

#include "ipputils.h"
#include "comp_utils.h"
#include "codecdefs.h"
#include "vframeencoder.hh"
#include "vframedecoder.hh"
#include "pgetopt.h"

#ifdef WINDOWS 
    #define srandom srand
    #define random rand
#else
    #define srandom srandom
    #define random random
#endif 

#ifdef WINDOWS
#define usleep Sleep

int windows_gettimeofday(struct timeval* tp, int* tz)
{
    struct timeb tb;
    ftime(&tb);
    tp->tv_sec = (long)tb.time;
    tp->tv_usec = 1000*tb.millitm;
    return 0;
}
#endif

#ifdef WINDOWS
   #define GETTIMEOFDAY windows_gettimeofday
#else 
   #define GETTIMEOFDAY(timevalp, tz) gettimeofday((timevalp), (tz))
#endif 

using namespace std;

int save_output = 0;
int do_decode = 0;
int scaleFactor;
int quant_parameter = 24;
eEncodeFormat encodeFormat;
bool bTestable = false;

// Simple utility to create samples for testing
// frame encoding/decoding functionality
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

void createRandomPattern(unsigned char * inBuff, int width, int height,
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

    CPng png("inset.png");
    png.Tile(inBuff, width, height);
    png.Overlay(inBuff, width, height);
    return;
}

void overlayOnPattern(unsigned char * inBuff, int width, int height,
        char *fileName)
{
    CPng *png = new CPng(fileName);
    png->Tile(inBuff, width, height);
    png->Overlay(inBuff, width, height);
    delete png;
}

int compress_pattern(unsigned char *outBuff, unsigned char *inBuff, int width,
        int height, int mquant, int dcquant)
{
    printf("Compressing Frame Legacy mquant = %d, dcquant = %d...\n",
             mquant, dcquant);
    initFrameParams(width, height, mquant, dcquant, 32);
    return encodeOneFrameToFile(inBuff, (char *) outBuff, 0);
}

int compress_pattern_real(unsigned char *outBuff, unsigned char *inBuff, int width,
        int height, int mquant, int dcquant, int *sliceSize, char *imageFiles)
{
    int len = 0;
    if (imageFiles == NULL) {
        printf("Compressing Frame Real mquant = %d, dcquant = %d...\n",
                mquant, dcquant);
        CVideoFrameEncoder *frameEncoder =
                new CVideoFrameEncoder(width, height, mquant, dcquant, encodeFormat);
        frameEncoder->SetTestable(bTestable);
        //frameEncoder->SetSliceSize(8,4);
        frameEncoder->SetQuantizationParameter(quant_parameter);
        *sliceSize = frameEncoder->GetSliceSize();
        //len = frameEncoder->EncodeFrame(outBuff, inBuff);
        frameEncoder->AllocateBitmap();
        frameEncoder->UpdateFrameFromBuffer(inBuff);
        frameEncoder->UpdateAll();
        frameEncoder->EncodeFrame();
        len = frameEncoder->GetNextFrame(outBuff);

        delete frameEncoder;
    }
    else {
        // Compress a series of frames using image files
        CVideoFrameEncoder *frameEncoder =
                new CVideoFrameEncoder(width, height, mquant, dcquant, encodeFormat);
        frameEncoder->SetTestable(bTestable);
        frameEncoder->SetQuantizationParameter(quant_parameter);
        *sliceSize = frameEncoder->GetSliceSize();
        frameEncoder->SetBitmap(inBuff);
        printf("Compressing Frame Real mquant = %d, dcquant = %d...\n",
            mquant, dcquant);
        printf("Using images from set %s\n", imageFiles);
        char filename[256];
        for (int i = 0; i < 24; i++) {
            sprintf(filename, "%s%02d.png", imageFiles, i + 1);
            overlayOnPattern(inBuff, width, height, filename);
            frameEncoder->UpdateFrameFromBuffer(inBuff);
            frameEncoder->UpdateAll();
            frameEncoder->EncodeFrame();
            len = frameEncoder->GetNextFrame(outBuff);
            printf("Using image %s len = %d\n", filename, len);
        }
        printf("\n");
        delete frameEncoder;
    }
    return len;
}

static unsigned long long getmillisec()
{
    struct timeval tv;
    GETTIMEOFDAY(&tv, NULL);
    return ((unsigned long long) tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

static void measure_performance(int loops, int width, int height,
        int mquant, int dcquant, int random_seed)
{
    unsigned char *inBuff;
    unsigned char *outBuff;
    int i;
    int len;
    int number_of_loops = loops;
    unsigned long long starttime;
    unsigned long long endtime;
    uint32_t totalSize = 0;
    CVideoFrameEncoder *frameEncoder = NULL;

    // Allocate space for input/output buffers
    inBuff = new unsigned char[width * height * 4 + 32];
    outBuff = new unsigned char[width * height * 4 + 32];
    while ((int)inBuff % 32 != 0) inBuff++;
    while ((int)outBuff % 32 != 0) outBuff++;

    // Create pattern for encoding
    if (random_seed > 0)
        createRandomPattern(inBuff, width, height, random_seed++);
    else
        createGradientPattern(inBuff, width, height);



    if (save_output) {
        CBitmap oSaveBmp;
        oSaveBmp.SetBitsBGRA(inBuff, width, height);
        ostringstream ss;
        ss << "perf";
        ss << "_" << width << "_" << height << ".bmp";
        oSaveBmp.Save(ss.str().c_str(), 32);
    }

    if (do_decode) {
        // First encode the frame and then repeatedly decode
        printf("Decode Test for %d frames resolution: %dx%d\n", number_of_loops, width, height);
        frameEncoder = new CVideoFrameEncoder(width, height, mquant, dcquant, encodeFormat);
        frameEncoder->SetTestable(bTestable);
        frameEncoder->SetQuantizationParameter(quant_parameter);
        //len = frameEncoder->EncodeFrame(outBuff, inBuff);

        frameEncoder->AllocateBitmap();
        frameEncoder->UpdateFrameFromBuffer(inBuff);
        frameEncoder->UpdateAll();
        frameEncoder->EncodeFrame();
        len = frameEncoder->GetNextFrame(outBuff);

        v2dDecConfiguration decConfig;
        decConfig.m_nWidth = width;
        decConfig.m_nHeight = height;
        decConfig.m_nSliceSize = frameEncoder->GetSliceSize();
        decConfig.m_eEncodeFormat = encodeFormat;
        if (!save_output)
            decConfig.m_eDecodeFormat = eYUV422;
        else
            decConfig.m_eDecodeFormat = eBGRX32;

        CVideoFrameDecoder *pFrameDecoder = CVideoFrameDecoder::CreateObj(
                &decConfig);
        pFrameDecoder->SetScaleFactor(scaleFactor);
        size_t nSize = decConfig.m_nDecodeBufLen;
        memset(inBuff, 0x00, width * height * 4);

        starttime = getmillisec();
        for (i = 0; i < number_of_loops; i++) {
            pFrameDecoder->Decode(outBuff, len, inBuff, nSize);
            totalSize += len;
        }
        endtime = getmillisec();

        if (save_output) {
            int scaledWidth = pFrameDecoder->GetScaledWidth();
            int scaledHeight = pFrameDecoder->GetScaledHeight();
            CBitmap oSaveBmp;
            oSaveBmp.SetBitsBGRA(inBuff, scaledWidth, scaledHeight);
            ostringstream ss;
            ss << "perf";
            ss << "_" << scaledWidth << "_" << scaledHeight << "_decoded.bmp";
            oSaveBmp.Save(ss.str().c_str(), 32);
        }

    }
    else {
        printf("Encode Test for %d frames\n", number_of_loops);
        frameEncoder = new CVideoFrameEncoder(width, height, mquant, dcquant, encodeFormat);
        frameEncoder->SetTestable(bTestable);
        frameEncoder->SetQuantizationParameter(quant_parameter);
        frameEncoder->AllocateBitmap();
        frameEncoder->UpdateFrameFromBuffer(inBuff);
        starttime = getmillisec();
        for (i = 0; i < number_of_loops; i++) {
            //len = frameEncoder->EncodeFrame(outBuff, inBuff);
            //totalSize += len;
            frameEncoder->UpdateAll();
            frameEncoder->EncodeFrame();
            len = frameEncoder->GetNextFrame(outBuff);

        }
        endtime = getmillisec();
    }

    // Report time spent
    double time_per_frame = (1.0 * (int) (endtime - starttime))
            / number_of_loops;
    printf("Time Spent = %0.2f per frame\n", time_per_frame);
    printf("Size = %u, FPS = %0.2f, BW = %.2f M\n",
            totalSize/number_of_loops,
            1000.0/time_per_frame,
            ((double)(totalSize * 8.0 * 1000.0))/((int)(endtime - starttime) * (1024.0 * 1024.0)));

    // Release resources
    delete frameEncoder;
}

static void frame_update_test()
{
    CVideoFrameEncoder *frameEncoder = new CVideoFrameEncoder(1280, 1024, 3, 3);
    frameEncoder->SetTestable(bTestable);

    frameEncoder->SetBitmap(NULL);

    frameEncoder->UpdatePixel(0, 0);
    frameEncoder->UpdatePixel(3, 0);
    frameEncoder->UpdatePixel(0, 7);
    frameEncoder->UpdatePixel(0, 8);
    frameEncoder->UpdatePixel(1279, 0);
    frameEncoder->UpdatePixel(1280, 0);

    frameEncoder->ClearUpdates();
    frameEncoder->UpdatePixel(25, 25);
    frameEncoder->ClearUpdates();
    frameEncoder->UpdatePixel(25, 50);

    frameEncoder->ClearUpdates();
    frameEncoder->UpdateRectangle(25, 25, 50, 50);


}

int decoderonly(unsigned char *outbuff, char * filename)
{
    unsigned int encodeLength;
    unsigned int length;
    FILE *pfile;

    pfile = fopen((char *)filename, "rb");
    if (pfile != NULL) {
    fseek (pfile, 0, SEEK_END);
    encodeLength = ftell(pfile);
    rewind(pfile);
    length = fread(outbuff, 1, encodeLength, pfile);
    printf("\nfile length:%d", length);
    fclose(pfile);
    return (encodeLength = length);
    }
    else
    {
      printf("\n error in opening file");
      return 0;
    }
}

#define MAX_NOBPSLICE           36
#define MIN_NOBPSLICE           20
#define MAX_SLICES_PER_FRAME    2048

static int compute_slice_size(int width, int height) {
    int slice_size = MAX_NOBPSLICE;
    int size;
    int found = 0;
    int slices_per_frame;

    // Encoded width should be multiple of 16 and
    // Encoded height should be multiple of 8
    int encoded_width = ((width + 15) / 16) * 16;
    int encoded_height = ((height + 7) / 8) * 8;

    int retry_count = 0;
    while (retry_count++ < 16) {
        for (size = MAX_NOBPSLICE; size >= MIN_NOBPSLICE; size -= 2) {
            if ((((encoded_width * encoded_height)/128) % size) == 0) {
                slice_size = size;
                found = 1;
                break;
            }
        }

        slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);

        if (!found && slices_per_frame < MAX_SLICES_PER_FRAME) {
            // Retry with expanded height
            encoded_height += 8;
            continue;
        }
    }

    slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);
    if (slices_per_frame >= MAX_SLICES_PER_FRAME) {
        int retry_count = 0;
        while (retry_count++ < 16) {
            // We need a bigger slice size so that number of slices
            // per frame is 2047 or less
            int min_slice_size = (int)((1.0 * encoded_width * encoded_height)/(2047.0 * 64.0)) + 1;
            if (min_slice_size % 2 != 0)
                min_slice_size++;

            // Search upwards from min slice size to double of min size
            found = 0;
            for (size = min_slice_size; size < min_slice_size * 2; size += 2) {
                if ((((encoded_width * encoded_height)/128) % size) == 0) {
                    slice_size = size;
                    slices_per_frame = (encoded_width * encoded_height)/(64 * slice_size);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                encoded_height += 8;
                continue;
            }
        }
    }

    // Assert on the constraints
    if (encoded_width % 16 != 0)
        printf("  Failed width constraint\n");
    if (encoded_height % 8 != 0)
        printf("  Failed height constraint\n");

    if (slice_size % 2 != 0)
        printf("  Failed even slice size constraint\n");

    if (slices_per_frame >= 2048)
        printf("  Failed slices per frame constraint\n");

    printf("%d x %d : %d, %d %s",
            width, height, slice_size, slices_per_frame, found ? "ok" : "default");
    if (width != encoded_width || height != encoded_height) {
        printf(" expanded %d x %d %d", encoded_width, encoded_height, encoded_height - height);
    }
    printf("\n");
    if (!found) {
        printf("Bummer\n");
        exit(1);
    }
    return slice_size;
}

static void slice_size_test() {

#if 1
    compute_slice_size(320, 240);
    compute_slice_size(640, 480);
    compute_slice_size(800, 600);
    compute_slice_size(1024, 768);
    compute_slice_size(1280, 1024);
    compute_slice_size(1400, 1050);
    compute_slice_size(1600, 1200);
    compute_slice_size(1920, 1080);
    compute_slice_size(1920, 1200);
    compute_slice_size(2048, 1152);
    compute_slice_size(2560, 1600);
    compute_slice_size(3360, 1050);
    compute_slice_size(3840, 1200);
    compute_slice_size(3840, 2160);
    compute_slice_size(3840, 2400);
    compute_slice_size(5040, 1050);
    compute_slice_size(5040, 2100);
    compute_slice_size(2540, 1426);
#endif

    for (int i = 0; i < 5000; i++) {
        int width = 80 + rand() % 5000;
        int height = 80 + rand() % 5000;
        compute_slice_size(width, height);
    }
}

static void usage(char *command)
{
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    printf("\t-n String\tName to use for generated files (default \"out\")\n");
    printf("\t-W Number\tFrame width (default 1280)\n");
    printf("\t-H Number\tFrame height (default 1024)\n");
    printf("\t-m Number\tMquant to use (default 6)\n");
    printf("\t-d Number\tDCquant to use (default 6)\n");
    printf("\t-r Number\tUse random pattern with given seed as argument\n");
    printf("\t-p Number\tMeasure performance using these many loops\n");
    printf("\t-q Number\tCompresion quality (decides mquant and dcquant)\n");
    printf("\t-f String\tTo use only decoder(specify file name to decode)\n");
    printf("\t-D \t\tDo decode\n");
    printf("\t-s \t\tSave compressed frame to data file\n");
    printf("\t-V \t\tUse SVC (Version 2 codec)\n");
    printf("\t-S Number\tScale Factor 1, 2, 4 or 8\n");
    printf("\t-I String\tPath to an image set\n");
    printf("\t-h\t\tShow this help\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    unsigned char *inBuff;
    unsigned char *outBuff;
    char ch;
    string sampleName;
    char *filename = NULL;
    int width, height;
    int mquant, dcquant;
    int random_seed;
    int sliceSize;
    int do_perf = 0;
    int quality;
    char *imageFiles;
    int use_h264;
    int use_svc;
    int decodeonly = 0;
    FILE * fp;

    // Setup default values
    mquant = 6;
    dcquant = 6;
    width = 1280;
    height = 1024;
    sampleName = "out";
    random_seed = 0;
    quality = -1;
    save_output = 0;
    do_decode = 0;
    imageFiles = NULL;
    scaleFactor = 1;
    use_h264 = 0;
    use_svc = 0;

    InitIPPLib();

    // Check command line options
    while ((ch = pgetopt(argc, argv, (char *)"n:W:H:m:d:r:p:f:eq:sDI:S:Q:V")) > 0) {
        switch (ch) {
        case 'n':
            sampleName = poptarg;
            break;
        case 'W':
            width = atoi(poptarg);
            break;
        case 'H':
            height = atoi(poptarg);
            break;
        case 'm':
            mquant = atoi(poptarg);
            break;
        case 'd':
            dcquant = atoi(poptarg);
            break;
        case 'q':
            quality = atoi(poptarg);
            break;
        case 'f':
            filename = poptarg;
            decodeonly = 1;
            break;
        case 'r':
            random_seed = atoi(poptarg);
            break;
        case 'p':
            do_perf = atoi(poptarg);
            break;
        case 's':
            save_output = 1;
            break;
        case 'D':
            do_decode = 1;
            break;
        case 'Q':
            use_h264 = 1;
            quant_parameter = atoi(poptarg);
            break;
        case 'V':
            use_svc = 1;
            break;
        case 'S':
            scaleFactor = atoi(poptarg);
            break;
        case 'I':
            imageFiles = strdup(poptarg);
            break;
        case 'e':
            // Experimental one off code
            slice_size_test();
            exit(0);
            frame_update_test();
            exit(0);
            break;
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }

    // If quality is set, use that to compute mquant and dcquant values
    if (quality >= 0) {
        mquant = mquant_from_compression(quality);
        dcquant = dcquant_from_compression(quality);
    }
    printf("mquant = %d, dcquant = %d\n", mquant, dcquant);

    encodeFormat = eYUV422P;
    if (use_h264) {
        encodeFormat = eH264;
    }
    else if (use_svc) {
        encodeFormat = eSVC;
    }

    // Allocate space for input/output buffers
    inBuff = new unsigned char[width * height * 4];
    outBuff = new unsigned char[width * height * 4];

    if (do_perf > 0) {
        measure_performance(do_perf, width, height, mquant, dcquant, random_seed);
        exit(0);
    }

    // Create pattern
    if (random_seed <= 0) {
        //createDebugPattern(inBuff, width, height);
        createGradientPattern(inBuff, width, height);
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
    ss << "_" << width << "_" << height << ".bmp";
    oSaveBmp.Save(ss.str().c_str(), 32);

    int encodeLength = 0;


#if 0
    // Compress pattern using legacy encoder
    encodeLength = compress_pattern(outBuff, inBuff, width, height, mquant,
            dcquant);

    // Save compressed output
    // No longer need to do this since we use md5sums of the data in the test cases

    if (save_output) {
        ss.str("");
        ss << sampleName;
        if (random_seed > 0)
            ss << "_" << random_seed;
        ss << "_" << width << "_" << height <<  "_" << mquant << "_" << dcquant << ".dat";
        fp = fopen(ss.str().c_str(), "w");
        if (fp != NULL) {
            fwrite(outBuff, encodeLength, 1, fp);
            fclose(fp);
        }
    }
#endif

    // Encode using new CVideoFrameEncoder class
    encodeLength = compress_pattern_real(outBuff, inBuff, width, height, mquant,
            dcquant, &sliceSize, imageFiles);

    printf("Encode length = %d, compression ratio = %.1f\n",
            encodeLength, (width * height * 3.0)/encodeLength);

    if (encodeFormat == eYUV422P) {
        printf("Slice Size = %d\n", sliceSize);
    }
    if (save_output) {
        ss.str("");
        ss << sampleName;
        if (random_seed > 0)
            ss << "_" << random_seed;
        ss << "_" <<  width << "_" << height << "_" << mquant << "_" << dcquant << "_real" << ".dat";
        fp = fopen(ss.str().c_str(), "w");
        if (fp != NULL) {
            fwrite(outBuff, encodeLength, 1, fp);
            fclose(fp);
        }
    }

    if (do_decode) {
        // Decode the encoded frame
        v2dDecConfiguration decConfig;
        decConfig.m_nWidth        = width;
        decConfig.m_nHeight       = height;
        decConfig.m_nSliceSize    = sliceSize;
        decConfig.m_eDecodeFormat = eBGRX32;
        decConfig.m_eEncodeFormat = encodeFormat;

        CVideoFrameDecoder *pFrameDecoder =
                CVideoFrameDecoder::CreateObj(&decConfig);
        size_t nSize = decConfig.m_nDecodeBufLen;
        pFrameDecoder->SetScaleFactor(scaleFactor);
        //pFrameDecoder->SetSliceDebug(true);
          if(decodeonly)
          {
            encodeLength = decoderonly(outBuff,filename);
            if(encodeLength == 0)
            return 0;
          }

        memset(inBuff, 0x00, width * height * 4);
        pFrameDecoder->Decode(outBuff, encodeLength, inBuff, nSize);

        // Write out the decoded file
        int scaledWidth = pFrameDecoder->GetScaledWidth();
        int scaledHeight = pFrameDecoder->GetScaledHeight();
        oSaveBmp.SetBitsBGRA(inBuff, scaledWidth, scaledHeight);
        ss.str("");
        ss << sampleName;
        if (random_seed > 0)
            ss << "_" << random_seed;
        ss << "_" << scaledWidth << "_" << scaledHeight << "_decoded.bmp";
        oSaveBmp.Save(ss.str().c_str());

        delete pFrameDecoder;
    }

    delete [] inBuff;
    delete [] outBuff;

    return 0;
}
