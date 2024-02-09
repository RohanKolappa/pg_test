
#include <math.h>

#include <v2dcommon.h>
#include <ScapProcessor.hh>
#include <srd.h>
#include <vframedecoder.hh>
#include <bitmap.hh>
#include <pgetopt.h>

#ifdef WINDOWS
#define usleep Sleep
#define ROUND windows_round

int windows_gettimeofday(struct timeval* tp, int* tz)
{
    struct timeb tb;
    ftime(&tb);
    tp->tv_sec = (long)tb.time;
    tp->tv_usec = 1000*tb.millitm;
    return 0;
}

double windows_round(double d)
{
  return floor(d + 0.5);
}

#else

#define ROUND round

//#define USE_THREADS

#endif

const int  RTP_HEADER_SIZE = 12;

struct Rx_Params {
    char pHostName[32];
    int  bMulticast;
    int  nAVOption;
    int  nBitrate;
    int  nSvrPort;
    int  nVideoPort;
    int  nAudioPort;
    int  nWidth;
    int  nHeight;
    int  nSliceSize;
};

struct Status {
    struct timeval tStart;
    struct timeval tPrev;

    int id;
    int nCtrlPkts;
    int nVideoPkts;
    int nAudioPkts;

    int nVideoFramesParsed;
    int nVideoFramesReduced;

    int nVideoFramesDecoded;
    int nAudioFramesDecoded;
    int nFullFrameCount;
    int nLastFullFrameCount;

    int nFileSaveFrequency;
    int nFileSaveCount;
    int nFileSaveMaxCount;
    int nVideoPacketLoss;
    int nVideoRate;
    int nAudioPacketLoss;
    int nMultiple;
    int nRTPOnly;
    
    FILE *pVideoRawFile; 
    FILE *pAudioRawFile;
    int nVideoFramesToWrite;
    int nAudioSamplesToWrite;
    int nAudioBytesWritten;

    int nTotalDuration;
    int nPreviousDuration;

    int nAudioRate;
    bool nGotResolution;
    bool bFrameFoldingDisable;
    bool bVideoDecodingDisable;
    struct CallBackData *callbackdata;
    uint32_t rtpTimestamp;
    bool bCreateBitmapRawData;
    bool bIsRawDataHeaderCreated;
};

struct RxParameters {
    struct Rx_Params stRxParams;
    struct Status status;
};

struct CallBackData {
    CallBackData(Status & a_pStatus, unsigned char *&a_pEncodedVideoBuffer, unsigned char *&a_pDecodedVideoBuffer,
        sliceRedData_t *a_pSrd, CVideoFrameDecoder *a_pV2DFrameDecoder, CScapProcessor *a_pScapProcessor, long a_nDecodedVideoBufferSize) : 
        status(a_pStatus),
        pSrd(a_pSrd),
        pV2DFrameDecoder(a_pV2DFrameDecoder),
        pScapProcessor(a_pScapProcessor),
        pEncodedVideoBuffer(a_pEncodedVideoBuffer),
        pDecodedVideoBuffer(a_pDecodedVideoBuffer),
        nDecodedVideoBufferSize(a_nDecodedVideoBufferSize)
        {};
    Status & status;
    sliceRedData_t *pSrd;
    CVideoFrameDecoder *pV2DFrameDecoder;
    CScapProcessor *pScapProcessor;
    unsigned char *&pEncodedVideoBuffer;
    unsigned char *&pDecodedVideoBuffer;
    long  nDecodedVideoBufferSize;
};


 bool DecodeVideoData(sliceRedData_t *a_pSrd,
        CVideoFrameDecoder *a_pV2DFrameDecoder,
        unsigned char *&a_pEncodedBuffer,
        unsigned char *&a_pDecodedBuffer,
        long &a_nDecodedBufferSize,
        int a_nSrcWidth,
        int a_nSrcHeight,
        int a_nSrcOrigWidth,
        int a_nSrcOrigHeight,
        struct Status &a_status);

bool DecodeAudioBuffer(unsigned char* a_pInData, unsigned char* a_pOutData,
                       const int a_nDataLen, struct Status &a_status)
{
#ifdef USE_THREADS
	return true;
#endif
    for (int i = 0; i < a_nDataLen; i += 2) {
        *a_pOutData = *(a_pInData + 1);
        *(a_pOutData + 1) = *a_pInData;

        if (a_status.pAudioRawFile != NULL) {
            if (a_status.nAudioBytesWritten < (a_status.nAudioSamplesToWrite * 4)) {
                fwrite(a_pOutData, sizeof(unsigned char), 2, a_status.pAudioRawFile);
                a_status.nAudioBytesWritten += 2;
            }
            else {
                fclose(a_status.pAudioRawFile);
                a_status.pAudioRawFile = NULL;
            }
        }

        a_pInData += 2;
        a_pOutData += 2;
    }

    return true;
}

int ParseBitRate(const char * bitratestring) {
    char *cc, *br;
    double rate;
    int brate;

    br = strdup(bitratestring);
    if (((cc = strchr(br, 'k')) != NULL) || ((cc = strchr(br, 'K')) != NULL)) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = (int) ROUND(rate * 1024.0);
    }
    else if (((cc = strchr(br, 'm')) != NULL) || ((cc = strchr(br, 'M'))
            != NULL)) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = (int) ROUND(rate * 1024.0 * 1024.0);
    }
    else {
        sscanf(br, "%lf", &rate);
        brate = (int) ROUND(rate);
    }

    free(br);
    return brate;
}

static char *GetBitRate(int bitrate, char * buffer)
{
    if (bitrate < 0)
        bitrate = 0;

    double bits = (double) (bitrate * 1.0);

    if (bits < 1024.0)
        sprintf(buffer, "%.2f", bits);

    else if (bits < 1024.0 * 1024.0)
        sprintf(buffer, "%.2fK", bits / 1024.0);

    else
        sprintf(buffer, "%.2fM", bits / (1024.0 * 1024.0));

    return buffer;
}

static void print_help(void) {
    printf("Usage: rx -s IP_ADDR [optional-arguments]\n");
    printf("Optional arguments:\n");
    printf("\t-a 1|0\t\tMake audio/video connection if 1 else 0\n");
    printf("\t-m 1|0\t\tMake multicast connection if 1 else 0\n");
    printf("\t-p Value\tUse server port number 'Value'\n");
    printf("\t-P Value\tSet local Audio/Video port number to 'Value'\n");
    printf("\t-b Value\tSet bit rate to 'Value' (add suffix K or M for Kbps/Mbps\n");
    printf("\t-F Value\tSave one image per \"F\" full frames (default 0)\n");
    printf("\t-C Value\tMax number of images to save (default 1)\n");
    printf("\t-d Value\tTotal duration to run in Secs (default infinite)\n");
    printf("\t-S Value\tTotal number of (4 byte) audio samples to write\n\t\t\tinto file audio_16384_2.pcm (default 0)\n");
    printf("\t-R Value\tUse RTP only mode. Provide video parameters\n\t\t\tFormat: WidthxHeight@Slicesize (e.g. 1024x768@32)\n");
    printf("\t-w Value\twrites number of Raw video frames(without RTP header)\n\t\t\tcomming from network\n");
    printf("\t-v Value\tTo disable frame folding, set this value to 1,\n\t\t\t(default 0)\n");
    printf("\t-D Value\tTo disable decoding, set this value to 1(default 0)\n");
    printf("\t-B 1|0\t\tCreate \"BitmapData.dat\" with raw bitmap data if 1 else 0\n");
    printf("\t-h\t\tPrint this message\n");
}

#define eos(s) ((s)+strlen(s))

static void GetTimeOfDay(struct timeval &a_tNow) {
    GETTIMEOFDAY(&a_tNow, NULL);
}

int getDuration(struct Status & a_status) {
    struct timeval tNow;
    GetTimeOfDay(tNow);
    unsigned long long nDuration = ((tNow.tv_sec - a_status.tStart.tv_sec) * 1000)  +
    		((tNow.tv_usec - a_status.tStart.tv_usec) / 1000 );
    return nDuration;
}

void Print_Status(struct Status & a_status, struct timeval &a_tNow) {

    char printbuffer[512];
    char videorate[32];
    char audiorate[32];

    unsigned long long nDuration = ((a_tNow.tv_sec - a_status.tStart.tv_sec) * 1000)  +
    		((a_tNow.tv_usec - a_status.tStart.tv_usec) / 1000 );

    if ((int) nDuration < a_status.nPreviousDuration + 1000)
    	return;

    a_status.nPreviousDuration = ((int) nDuration / 1000) * 1000;

    strcpy(printbuffer, "");

#ifdef USE_THREADS
    sprintf(eos(printbuffer), "** Thread %d Duration %d **\n",
    		a_status.id, (int)nDuration);
#endif

    sprintf(eos(printbuffer), "Ctrl : %d\n  Video : %d (%s) \n  Audio : %d (%s) \n",
    		a_status.nCtrlPkts,
    		a_status.nVideoPkts,
    		GetBitRate(a_status.nVideoRate, videorate),
    		a_status.nAudioPkts,
    		GetBitRate(a_status.nAudioRate, audiorate));

#ifdef USE_THREADS
    printf("%s\n", printbuffer);
    return;
#endif

    sprintf(eos(printbuffer), " Packet Loss : \n  Video : %d\n  Audio : %d\n",
            a_status.nVideoPacketLoss,
            a_status.nAudioPacketLoss);

    sprintf(eos(printbuffer), " Parsed Frames : \n");
    sprintf(eos(printbuffer), "  Video : %d (Reduced: %d)\n",
            a_status.nVideoFramesParsed,
            a_status.nVideoFramesReduced);

    sprintf(eos(printbuffer), " Decoded Frames : \n");
    sprintf(eos(printbuffer), "  Video : %d\n  Audio : %d \n", a_status.nVideoFramesDecoded,
                                        a_status.nAudioFramesDecoded);
    sprintf(eos(printbuffer), " Video Full Frames %d\n", a_status.nFullFrameCount);


    printf("%s\n\n", printbuffer);
}

void InitParams(struct Rx_Params &a_RxParams) {
    a_RxParams.pHostName[0] = '\0';
    a_RxParams.bMulticast   = FALSE;
    a_RxParams.nAVOption    = AVOPTION_AUDIO_VIDEO;
    a_RxParams.nBitrate     = 1024000;
    a_RxParams.nSvrPort     = 6060;
    a_RxParams.nVideoPort   = 0;
    a_RxParams.nAudioPort   = 0;
    a_RxParams.nWidth       = 0;
    a_RxParams.nHeight      = 0;
    a_RxParams.nSliceSize   = 0;
}

int ParseArguments(int a_argc, char* a_argv[], struct Rx_Params &a_RxParams, struct Status & a_status) {
    int nRetval = TRUE;
    int nOpt = 0;

    InitParams(a_RxParams);

    while ((nOpt = pgetopt(a_argc, a_argv, (char *) "s:h:p:P:a:b:m:w:F:C:B:d:S:M:R:v:D:")) > 0) {
           switch (nOpt) {
            case 'a':
                if (atoi(poptarg))
                    a_RxParams.nAVOption = AVOPTION_AUDIO_VIDEO;
                else
                    a_RxParams.nAVOption = AVOPTION_VIDEO;
            break;

            case 'm':
                a_RxParams.bMulticast = atoi(poptarg);
                break;

            case 'p':
                a_RxParams.nSvrPort = atoi(poptarg);
                break;

            case 'P':
                a_RxParams.nVideoPort = atoi(poptarg);
                a_RxParams.nAudioPort = atoi(poptarg) + 1;
                break;

            case 'F':
                a_status.nFileSaveFrequency = atoi(poptarg);
                break;

            case 'R':
                a_status.nRTPOnly = 1;
                sscanf(poptarg, "%dx%d@%d",
                        &a_RxParams.nWidth, &a_RxParams.nHeight, &a_RxParams.nSliceSize);
                break;

            case 'C':
                a_status.nFileSaveMaxCount = atoi(poptarg);
                break;

            case 'b':
                a_RxParams.nBitrate = ParseBitRate(poptarg);
                break;

            case 's':
                strncpy(a_RxParams.pHostName, poptarg, sizeof(a_RxParams.pHostName));
                break;

            case 'd':
                a_status.nTotalDuration = (atoi(poptarg) * 1000);
                break;

            case 'S':
                a_status.nAudioSamplesToWrite = atoi(poptarg);
                break;

            case 'M':
                a_status.nMultiple = atoi(poptarg);
                if (a_status.nMultiple <= 0)
                	a_status.nMultiple = 1;
                break;
            case 'w':
                a_status.nVideoFramesToWrite = atoi(poptarg);
                break;
            case 'v':
                a_status.bFrameFoldingDisable = atoi(poptarg);
                break;
            case 'D':
                a_status.bVideoDecodingDisable = atoi(poptarg);
                break;

            case 'B':
                a_status.bCreateBitmapRawData = atoi(poptarg) == 1 ? true : false;
                break;

            case 'h':
            case '?':
                print_help();
                nRetval = FALSE;
        }
    }

    if (a_RxParams.pHostName[0] == '\0') {
        print_help();
        nRetval = FALSE;
    }

    return nRetval;
}

void Init_Status(struct Status &a_status, struct timeval &a_tNow) {

    a_status.tPrev.tv_sec = a_tNow.tv_sec;
    a_status.tPrev.tv_usec = a_tNow.tv_usec;

    a_status.tStart.tv_sec = a_tNow.tv_sec;
    a_status.tStart.tv_usec = a_tNow.tv_usec;

    a_status.id = 0;
    a_status.nCtrlPkts  = 0;
    a_status.nVideoPkts = 0;
    a_status.nAudioPkts = 0;

    a_status.nVideoFramesParsed = 0;
    a_status.nVideoFramesReduced = 0;

    a_status.nVideoFramesDecoded = 0;
    a_status.nAudioFramesDecoded = 0;
    a_status.nFullFrameCount = 0;
    a_status.nLastFullFrameCount = 0;
    a_status.nFileSaveFrequency = 0;
    a_status.nFileSaveCount = 0;
    a_status.nFileSaveMaxCount = 1;
    a_status.nVideoPacketLoss = 0;
    a_status.nVideoRate = 0;
    a_status.nAudioPacketLoss = 0;
    a_status.pVideoRawFile = NULL;
    a_status.pAudioRawFile = NULL;
    a_status.nVideoFramesToWrite = 0;
    a_status.nAudioSamplesToWrite = 0;
    a_status.nAudioBytesWritten = 0;
    a_status.nMultiple = 1;
    a_status.nTotalDuration = -1;
    a_status.nPreviousDuration = 0;
    a_status.nRTPOnly = 0;

    a_status.nAudioRate = 0;
    a_status.nGotResolution = false;
    a_status.bFrameFoldingDisable = false;
    a_status.bVideoDecodingDisable = false;

    a_status.bCreateBitmapRawData = false;
    a_status.bIsRawDataHeaderCreated = false;
}

void ReCreateSRD(sliceRedData_t *&a_pSrd) {
    if (a_pSrd != NULL) {
        destroySRD(a_pSrd);
        free(a_pSrd);
        a_pSrd = NULL;
    }

    a_pSrd = (sliceRedData_t *) malloc(sizeof(sliceRedData_t));
    initSRD(a_pSrd);
}

static void SRDNewFrameCallback(void *a_object) {
  
    struct CallBackData *callbackdata = (struct CallBackData *) a_object;
    
    /* Handle the callback which is installed in SRD module. 
       This is called back here for every frame SRD module collects 
       during packet parsing. Even if there are a multiple frames 
       in a packet, each frame can be retrieved here for decoding 
       and thus frame folding can be avoided */
   
       DecodeVideoData(callbackdata->pSrd, callbackdata->pV2DFrameDecoder, callbackdata->pEncodedVideoBuffer,
                       callbackdata->pDecodedVideoBuffer,callbackdata->nDecodedVideoBufferSize, 
                       callbackdata->pScapProcessor->GetWidth(), callbackdata->pScapProcessor->GetHeight(),
                       callbackdata->pScapProcessor->GetOriginalWidth(), callbackdata->pScapProcessor->GetOriginalHeight(),
                       callbackdata->status);
    
}

void SetupSRDNewFrameCallback(sliceRedData_t *a_pSrd, CVideoFrameDecoder *&a_pV2DFrameDecoder,
                              CScapProcessor *a_pScap, unsigned char *&a_pEncodedBuffer, 
                              unsigned char *&a_pDecodedBuffer,long a_nDecodedBufferSize,
                               struct Status &a_status) {
  
    if (a_status.callbackdata != NULL)
        delete a_status.callbackdata;
    
    a_status.callbackdata  = new CallBackData(a_status, a_pEncodedBuffer, 
                              a_pDecodedBuffer, a_pSrd, a_pV2DFrameDecoder, a_pScap, a_nDecodedBufferSize);

    /* We install a callback in SRD module and pass it a data structure 
       encapsulating all the parameters we will need for decoding the frame.*/
    SRDSetNewFrameCallback(a_pSrd, SRDNewFrameCallback, a_status.callbackdata);
   // a_status.callbackdata = callbackdata;
   

}
void InitializeSRD(sliceRedData_t *a_pSrd, CScapProcessor *a_pScap) {
    a_pSrd->sliceSize = a_pScap->GetSliceSize();
    a_pSrd->MaxSliceNo = a_pScap->GetMaxSliceNumber();
    setStereoVideo(a_pSrd, a_pScap->GetStereoMode());
    setStereoPolicy(a_pSrd, SP_MONO);
    setCompMode(a_pSrd, a_pScap->GetVideoCompMode());
    setMotionCompMode(a_pSrd, a_pScap->GetVideoMotionCompMode());
    
}

void ReCreateV2DFrameDecoder(CVideoFrameDecoder *&a_pFrameDecoder,
                             CScapProcessor *a_pScap) {


    if (a_pFrameDecoder != NULL) {
        delete a_pFrameDecoder;
    }

    v2dDecConfiguration decConfig;

    decConfig.m_nWidth = a_pScap->GetWidth();
    decConfig.m_nHeight = a_pScap->GetHeight();
    decConfig.m_nSliceSize = a_pScap->GetSliceSize();
    decConfig.m_eEncodeFormat = a_pScap->GetEncodeFormat();
    decConfig.m_eDecodeFormat = eBGR24;

    a_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);
}

void ReCreateDecoderBuffer(unsigned char *&a_pDecodedBuffer,
                           CVideoFrameDecoder *a_pV2DFrameDecoder, long &a_nDecodedBufferSize) {
#ifdef USE_THREADS
	return;
#endif
    if (a_pDecodedBuffer != NULL) {
        delete [] a_pDecodedBuffer;
    }

    a_nDecodedBufferSize = a_pV2DFrameDecoder->GetDecodeBufLen();
    a_pDecodedBuffer = new unsigned char[a_nDecodedBufferSize];
}


bool GetEncodedVideoFrame(sliceRedData_t *a_pSrd, unsigned char *&a_pEncodedBuffer,
                     long &a_lEncodedFrameSize, struct Status &a_status) {

    // Does the frame parser have anything for us?
    int nFrameAvailable = get_hoFrame(a_pSrd);
    if (nFrameAvailable == -1)
        return false; // no frame ready yet

    // Ok we have some frame that we can retrieve
    // Allocate a buffer for it first
    if (a_pEncodedBuffer != NULL)
        delete [] a_pEncodedBuffer;
    a_lEncodedFrameSize = getHoFrameSize(a_pSrd); // find how big a buffer we need
    a_pEncodedBuffer = new unsigned char[a_lEncodedFrameSize];

    // Grab the latest encoded frame from the frame parser
    copyHoFrameToBuffer(a_pSrd, a_pEncodedBuffer, a_lEncodedFrameSize);

    // Grab the RTP timestamp associated with the frame just retrieved
    a_status.rtpTimestamp = SRDGetFrameTimeStamp(a_pSrd);

    a_status.nVideoFramesParsed++;

    
    // SRD parser keeps track of how many frames got reduced ("folded")
    a_status.nVideoFramesReduced = a_pSrd->reducedFrameCount;
    

    return true;
}

bool FlipImageData (unsigned char *a_pBuffer,
          int a_nSrcWidth, int a_nSrcHeight) {
    unsigned char *pTmpBuff = new unsigned char[3 * a_nSrcWidth];
    int nLineLength = 3 * a_nSrcWidth;
    int nSize = sizeof(unsigned char) * nLineLength;

    //Flipping the image data
    for(int i = 0; i < a_nSrcHeight / 2; i++)
    {
        memcpy(pTmpBuff, a_pBuffer + (i * nLineLength), nSize);
        memcpy(a_pBuffer + (i * nLineLength), a_pBuffer + (( a_nSrcHeight - 1 - i) * nLineLength), nSize);
        memcpy(a_pBuffer + ((a_nSrcHeight - 1 - i) * nLineLength), pTmpBuff, nSize);
    }
    delete [] pTmpBuff;

    return true;
}

void SaveVideoFrameToFile(unsigned char *a_pDecodedBuffer,
        int a_nSrcWidth, int a_nSrcHeight,
        int a_nSrcOrigWidth, int a_nSrcOrigHeight,
        int a_nFileNumber) {
    CBitmap oSaveBmp;
    char filename[32];
    sprintf(filename, "image_%d.bmp", a_nFileNumber);

    if (a_nSrcWidth == a_nSrcOrigWidth && a_nSrcHeight == a_nSrcOrigHeight) {

        oSaveBmp.SetBitsBGR(a_pDecodedBuffer, a_nSrcWidth, a_nSrcHeight);
        oSaveBmp.Save(filename, 24);
    }
    else {
        // Need to crop out pixels on the bottom and right
        uint8_t * croppedBuffer = new uint8_t[a_nSrcOrigWidth * a_nSrcOrigHeight * 3];
        uint8_t * srcBufferLine = a_pDecodedBuffer;
        uint8_t * dstBufferLine = croppedBuffer;
        for (int j = 0; j < a_nSrcOrigHeight; j++) {
            memcpy(dstBufferLine, srcBufferLine, a_nSrcOrigWidth * 3);
            srcBufferLine += a_nSrcWidth * 3;
            dstBufferLine += a_nSrcOrigWidth * 3;
        }

        oSaveBmp.SetBitsBGR(croppedBuffer, a_nSrcOrigWidth, a_nSrcOrigHeight);
        oSaveBmp.Save(filename, 24);
        delete [] croppedBuffer;
    }

    printf("Saved Image File %s\n\n", filename);
}

/* Save an image file when the "full frame" counter bumps up, i.e. the
 * refresh slices cover the frame fully. The frequency and max count
 * parameters allow us to control how many of these images are created */
bool ShouldSaveVideoFrame(struct Status &a_status) {
    if (a_status.nFullFrameCount <= a_status.nLastFullFrameCount)
        return false;

    a_status.nLastFullFrameCount = a_status.nFullFrameCount;

    if (a_status.nFileSaveFrequency <= 0)
        return false;

    if (a_status.nFileSaveFrequency > 1 &&
            a_status.nFullFrameCount % a_status.nFileSaveFrequency != 1)
        return false;

    if (a_status.nFileSaveCount >= a_status.nFileSaveMaxCount)
        return false;

    a_status.nFileSaveCount++;
    return true;
}

//Saving the bitmap raw data into local file "BitmapData.dat"
void SaveBitmapData(struct Status &a_status, unsigned char *a_pDecodedBuffer,
                    int a_nSrcWidth, int a_nSrcHeight,
                    int a_nSrcOrigWidth, int a_nSrcOrigHeight)
{
    FILE *pFile = NULL;
    char filename[32];
    sprintf(filename,"%s", "BitmapData.dat");

    unsigned char *pBitmapBuffer = new unsigned char[3 * a_nSrcOrigWidth * a_nSrcOrigHeight];

    if (a_status.bIsRawDataHeaderCreated == false) {
        pFile = fopen(filename, "wb");
        fwrite((void*)&a_nSrcOrigWidth, 1, sizeof(int), pFile);
        fwrite((void*)&a_nSrcOrigHeight, 1, sizeof(int), pFile);

        a_status.bIsRawDataHeaderCreated = true;
    }
    else {
        pFile = fopen(filename, "ab");
    }

    if (a_nSrcWidth == a_nSrcOrigWidth && a_nSrcHeight == a_nSrcOrigHeight) {
        memcpy(pBitmapBuffer, a_pDecodedBuffer, (a_nSrcOrigWidth * a_nSrcOrigHeight * 3));
    }
    else {
        // Need to crop out pixels on the bottom and right
        uint8_t * srcBufferLine = a_pDecodedBuffer;
        uint8_t * dstBufferLine = pBitmapBuffer;
        for (int j = 0; j < a_nSrcOrigHeight; j++) {
            memcpy(dstBufferLine, srcBufferLine, a_nSrcOrigWidth * 3);
            srcBufferLine += a_nSrcWidth * 3;
            dstBufferLine += a_nSrcOrigWidth * 3;
        }
    }

    //Flipping the image data
    FlipImageData(pBitmapBuffer, a_nSrcOrigWidth, a_nSrcOrigHeight);

    // Writing into output file
    fwrite (pBitmapBuffer, 1, (a_nSrcOrigWidth * a_nSrcOrigHeight * 3), pFile);

    fclose(pFile);

    delete [] pBitmapBuffer;
}

void DecodeVideoFrame(CVideoFrameDecoder *a_pV2DFrameDecoder, unsigned char *a_pDecodedBuffer,
                     long &a_nDecodedBufferSize, unsigned char *a_pEncodedBuffer,
                     long &a_nEncodedBufferSize, int a_nSrcWidth, int a_nSrcHeight,
                     struct Status &a_status) {

    if (a_status.bVideoDecodingDisable)
        return;

    a_pV2DFrameDecoder->Decode(a_pEncodedBuffer, a_nEncodedBufferSize,
                               a_pDecodedBuffer, a_nDecodedBufferSize);

    a_status.nVideoFramesDecoded++;
}


void ConfigureVideoParameters(CScapProcessor *a_pScap,
                        sliceRedData_t *&a_pSrd,
                        CVideoFrameDecoder *&a_pV2DFrameDecoder,
                        unsigned char *&a_pEncodedBuffer,
                        unsigned char *&a_pDecodedBuffer,
                        long &a_nDecodedBufferSize,
                        struct Status &a_status)
{
    ReCreateSRD(a_pSrd);

    InitializeSRD(a_pSrd, a_pScap);
     if (!a_status.bVideoDecodingDisable) {
       ReCreateV2DFrameDecoder(a_pV2DFrameDecoder, a_pScap);
       ReCreateDecoderBuffer(a_pDecodedBuffer, a_pV2DFrameDecoder, a_nDecodedBufferSize);
    }
    
    /* If frame folding has to be disabled, 
       we install a callback in the SRD module.*/
    if (a_status.bFrameFoldingDisable)
       SetupSRDNewFrameCallback(a_pSrd, a_pV2DFrameDecoder, a_pScap, 
                              a_pEncodedBuffer, a_pDecodedBuffer, a_nDecodedBufferSize, a_status);

}

bool HandleControlData(V2DRx_t *a_pV2DRx, CScapProcessor *a_pScap,
                        sliceRedData_t *&a_pSrd,
                        CVideoFrameDecoder *&a_pV2DFrameDecoder,
                        unsigned char *&a_pEncodedBuffer,
                        unsigned char *&a_pDecodedBuffer,
                        long &a_nDecodedBufferSize,
                        struct Status &a_status) {
    char *pCtrlData = (char *)(a_pV2DRx->m_CtlRcv);

    if (a_pScap->Process(pCtrlData)) {
        if (a_pScap->ResolutionChangeDetected()) {


#ifdef USE_THREADS
            printf("[%d] ", a_status.id);
#endif
            printf("New Resolution: %dx%d@%d\n",
                    a_pScap->GetWidth(), a_pScap->GetHeight(), a_pScap->GetSliceSize());
            if (a_pScap->GetOriginalWidth() != a_pScap->GetWidth() ||
                    a_pScap->GetOriginalHeight() != a_pScap->GetHeight()) {
                printf("Original Resolution: %dx%d\n",
                        a_pScap->GetOriginalWidth(), a_pScap->GetOriginalHeight());
            }
            a_status.nGotResolution = true;

            if (a_pV2DRx->m_nMulticast) {
                printf("Multicast Address: %s\n", a_pV2DRx->m_MulticastAddress);
            }
            ConfigureVideoParameters(a_pScap, a_pSrd, a_pV2DFrameDecoder,
                    a_pEncodedBuffer, a_pDecodedBuffer, a_nDecodedBufferSize, a_status);

        }
    }

    return true;
}

bool DecodeVideoData(sliceRedData_t *a_pSrd,
        CVideoFrameDecoder *a_pV2DFrameDecoder,
        unsigned char *&a_pEncodedBuffer,
        unsigned char *&a_pDecodedBuffer,
        long &a_nDecodedBufferSize,
        int a_nSrcWidth,
        int a_nSrcHeight,
        int a_nSrcOrigWidth,
        int a_nSrcOrigHeight,
        struct Status &a_status)
{
    if (a_pSrd == NULL)
        return false;

    long nEncodedBufferSize = 0;

    // Get a frame from the frame parser
    if (!GetEncodedVideoFrame(a_pSrd, a_pEncodedBuffer, nEncodedBufferSize, a_status))
        return false;

    a_status.nFullFrameCount = a_pSrd->FullFrameCount;

    if (a_pV2DFrameDecoder == NULL)
        return false;

    if (a_pDecodedBuffer == NULL)
        return false;

    if (!a_status.bVideoDecodingDisable) {
      // Decode the video frame
       DecodeVideoFrame(a_pV2DFrameDecoder, a_pDecodedBuffer, a_nDecodedBufferSize,
            a_pEncodedBuffer, nEncodedBufferSize, a_nSrcWidth, a_nSrcHeight, a_status);
    }

    if (a_status.bCreateBitmapRawData) {
        SaveBitmapData(a_status, a_pDecodedBuffer, a_nSrcWidth, a_nSrcHeight,
                       a_nSrcOrigWidth, a_nSrcOrigHeight);
    }

    // Now we can render the video frame
    if (ShouldSaveVideoFrame(a_status)) {
        SaveVideoFrameToFile(a_pDecodedBuffer,
                    a_nSrcWidth, a_nSrcHeight,
                    a_nSrcOrigWidth, a_nSrcOrigHeight,
                    a_status.nFileSaveCount);
    }
    return true;
}

void DumpRawVideoData(unsigned char *pData, int len, struct Status &a_status ) {

    if (a_status.pVideoRawFile != NULL ) {
       if (a_status.nVideoFramesParsed <= a_status.nVideoFramesToWrite) {
          int ret = fwrite(pData, 1, len, a_status.pVideoRawFile);
          if (ret <= 0){
            printf("Error in writing raw video data \n");
            fclose(a_status.pVideoRawFile);
            a_status.pVideoRawFile = NULL;
            return;
          }
       }
       else {
          fclose(a_status.pVideoRawFile);
          a_status.pVideoRawFile = NULL;
       }
    }
    
}

bool HandleVideoData(V2DRx_t *a_pV2DRx, sliceRedData_t *a_pSrd,
                     CVideoFrameDecoder *a_pV2DFrameDecoder,
                     unsigned char *&a_pEncodedBuffer,
                     unsigned char *&a_pDecodedBuffer,
                     CScapProcessor *a_pScap,
                     long &a_nDecodedBufferSize,
                     struct Status &a_status) {

    if (a_pSrd == NULL)
        return false;

    // Retrieve RTP timestamp from the packet header
    uint32_t rtpTimeStamp = ntohl(*((uint32_t *)(a_pV2DRx->m_VideoRcv + 4)));

    // Set timestamp for this packet in the SRD frame parser
    SRDSetPacketTimestamp(a_pSrd, rtpTimeStamp);

    // Send the packet to the SRD frame parser */
    findFramesinPacket(a_pSrd, a_pV2DRx->m_VideoRcv + RTP_HEADER_SIZE);
    DumpRawVideoData(a_pV2DRx->m_VideoRcv + RTP_HEADER_SIZE, a_pV2DRx->m_nVideoLen - RTP_HEADER_SIZE, a_status );

    return true;
}


bool HandleAudioData(V2DRx_t *a_pV2DRx,
                     unsigned char *&a_pDecodedAudioBuffer, struct Status &a_status) {
    int nAvailableData = a_pV2DRx->m_nAudioLen - RTP_HEADER_SIZE;

    if (a_pDecodedAudioBuffer == NULL)
    {
        a_pDecodedAudioBuffer = new unsigned char[nAvailableData];
    }

    DecodeAudioBuffer(a_pV2DRx->m_AudioRcv + RTP_HEADER_SIZE,
                      a_pDecodedAudioBuffer,
                      nAvailableData, a_status);

    a_status.nAudioFramesDecoded++;

    return true;
}

int check_error(V2DRx_t *rx_session) {
    if (rx_session->m_bSendError == true) {
        int errCode = (rx_session->m_nErrCode & 0xffff0000) >> 16;
        switch (errCode) {
        case RC_CRITICAL_ERR:
            // Do Critical error handling;
            printf("Critical error\n");
            return 2;
        case RC_SOCKERR:
            // recoverable error;
            break;
        }
        char errorString[128];
        Rx_SendError(rx_session, errorString, 128);
        v2dLibGetStatusString(errorString,
                              rx_session->m_statuscode,
                              rx_session->m_nMulticast,
                              rx_session->m_bUseHTTP,
                              rx_session->m_nAVOption);
       printf("Error: %s\n", errorString);
       return 1;
    }

    return 0;
}

void RestartRxSession(RxParameters * rxParameters,
		V2DRx_t * &pV2DRx, sliceRedData_t * &pSrd,
		CScapProcessor * &pScapProcessor, struct Status &a_status) {



    Rx_Destroy(pV2DRx);
    delete pScapProcessor;


    struct timeval tNow;
    GetTimeOfDay(tNow);
    a_status.tPrev.tv_sec = tNow.tv_sec;
    a_status.tPrev.tv_usec = tNow.tv_usec;
    a_status.tStart.tv_sec = tNow.tv_sec;
    a_status.tStart.tv_usec = tNow.tv_usec;

	pV2DRx = Rx_Create(rxParameters->stRxParams.pHostName,
    		rxParameters->stRxParams.nSvrPort,
    		rxParameters->stRxParams.nBitrate,
    		rxParameters->stRxParams.bMulticast,
    		rxParameters->stRxParams.nVideoPort,
    		rxParameters->stRxParams.nAudioPort,
    		rxParameters->stRxParams.nAVOption,
    		AUDIOTYPE_DEFAULT, 0);

	ReCreateSRD(pSrd);

	pScapProcessor = new CScapProcessor();

}

void * RxProcess(void * data) {

    RxParameters * rxParameters = (RxParameters *) data;
   
#ifdef USE_THREADS
    printf("[%d] Starting thread.\n", rxParameters->status.id);
#endif

    struct timeval tNow;
    sliceRedData_t *pSrd = NULL;
    CVideoFrameDecoder *pV2DFrameDecoder = NULL;
    CScapProcessor *pScapProcessor = NULL;
    rxParameters->status.callbackdata = NULL;
    
    unsigned char* pEncodedVideoBuffer = NULL;
    unsigned char* pDecodedVideoBuffer = NULL;
    long  nDecodedVideoBufferSize = 0;
    unsigned char* pDecodedAudioBuffer = NULL;
    
    V2DRx_t *pV2DRx = Rx_Create(rxParameters->stRxParams.pHostName,
    		rxParameters->stRxParams.nSvrPort,
    		rxParameters->stRxParams.nBitrate,
    		rxParameters->stRxParams.bMulticast,
    		rxParameters->stRxParams.nVideoPort,
    		rxParameters->stRxParams.nAudioPort,
    		rxParameters->stRxParams.nAVOption,
    		AUDIOTYPE_DEFAULT, 0);


    if (pV2DRx == NULL) {
        printf ("Could not create session\n");
        exit(-2);
    }

    if (rxParameters->status.nRTPOnly == 1) {
        // Pure RTP mode: use the configured server IP as the multicast IP
        if (Rx_SetRTPMode(pV2DRx, rxParameters->stRxParams.pHostName) == -1) {
            printf("Failed configuring RTP mode\n");
            exit(1);
        }
    }

    if (rxParameters->status.nAudioSamplesToWrite > 0 && rxParameters->status.id == 0) {
    	rxParameters->status.pAudioRawFile = fopen("audio_16384_2.pcm", "wb");
        if (rxParameters->status.pAudioRawFile == NULL) {
            printf("Could not create audio raw samples file.\n");
            exit(-3);
        }
    }

    if (rxParameters->status.nVideoFramesToWrite > 0) {
        rxParameters->status.pVideoRawFile = fopen("video_noRtp_data.dat", "wb");
        if (rxParameters->status.pVideoRawFile == NULL) {
            printf("Could not create video raw samples file.\n");
            exit(-3);
        }
    }

    GetTimeOfDay(tNow);


    
    // In RTP only mode configure the Video parameters directly
    if (rxParameters->status.nRTPOnly == 1 &&
            rxParameters->stRxParams.nWidth > 0 &&
            rxParameters->stRxParams.nHeight > 0 &&
            rxParameters->stRxParams.nSliceSize > 0) {
        pScapProcessor = new CScapProcessor(rxParameters->stRxParams.nWidth,
                rxParameters->stRxParams.nHeight,
                rxParameters->stRxParams.nSliceSize);
        ConfigureVideoParameters(pScapProcessor, pSrd, pV2DFrameDecoder,
                pEncodedVideoBuffer, pDecodedVideoBuffer, nDecodedVideoBufferSize, rxParameters->status);
    }
    else {
        pScapProcessor =  new CScapProcessor();
    }
              
    while (1) {
        if (pV2DRx->m_nCtlSock == 0) {
            Rx_OpenMediaConnection(pV2DRx);
        }

        Rx_Go(pV2DRx);

        if (check_error(pV2DRx)) {
        	exit(1);
        	break;
        }


        int operations = 0;

        if (pV2DRx->m_nFlag & I_CTL) {
        	rxParameters->status.nCtrlPkts++;

            HandleControlData(pV2DRx, pScapProcessor, pSrd, pV2DFrameDecoder,
                              pEncodedVideoBuffer, pDecodedVideoBuffer, nDecodedVideoBufferSize,
                              rxParameters->status);

            pV2DRx->m_nFlag &= ~I_CTL;
            operations++;
        }


        if (pV2DRx->m_nFlag & I_VIDEO) {
        	rxParameters->status.nVideoPkts++;

            HandleVideoData(pV2DRx, pSrd, pV2DFrameDecoder,
                            pEncodedVideoBuffer, pDecodedVideoBuffer,
                            pScapProcessor, nDecodedVideoBufferSize,
                            rxParameters->status);

            pV2DRx->m_nFlag &= ~I_VIDEO;
            rxParameters->status.nVideoPacketLoss = pV2DRx->m_nVideoLoss;
            operations++;
        }


        if (pV2DRx->m_nFlag & I_AUDIO) {
        	rxParameters->status.nAudioPkts++;

            HandleAudioData(pV2DRx, pDecodedAudioBuffer, rxParameters->status);

            pV2DRx->m_nFlag &= ~I_AUDIO;
            rxParameters->status.nAudioPacketLoss = pV2DRx->m_nAudioLoss;
            operations++;
        }


        if (operations == 0) {
            if (DecodeVideoData(pSrd, pV2DFrameDecoder, pEncodedVideoBuffer,
                pDecodedVideoBuffer, nDecodedVideoBufferSize,
                pScapProcessor->GetWidth(), pScapProcessor->GetHeight(),
                pScapProcessor->GetOriginalWidth(), pScapProcessor->GetOriginalHeight(),
                rxParameters->status)) {
                operations++;
            }
        }


        GetTimeOfDay(tNow);

#if 0
        // Restart as soon as resolution is retrieved
        if (rxParameters->status.nGotResolution) {
        	RestartRxSession(rxParameters, pV2DRx,
        				pSrd, pScapProcessor, rxParameters->status);
        	rxParameters->status.nGotResolution = false;
        	continue;
        }
#endif

        if (rxParameters->status.nTotalDuration > 0) {
            unsigned long long nDuration =
            		((tNow.tv_sec - rxParameters->status.tStart.tv_sec) * 1000)  +
            ((tNow.tv_usec - rxParameters->status.tStart.tv_usec) / 1000 );
            if (nDuration >= (unsigned long long) rxParameters->status.nTotalDuration)
                break;
        }

        rxParameters->status.nVideoRate = pV2DRx->m_nCurrVideoRate;
        rxParameters->status.nAudioRate = pV2DRx->m_nCurrAudioRate;

        Print_Status(rxParameters->status, tNow);
        if (operations == 0) {
            usleep(1);
        }

    }

    if (rxParameters->status.pAudioRawFile != NULL) {
        fclose(rxParameters->status.pAudioRawFile);
    }

    if (rxParameters->status.pVideoRawFile != NULL) {
        fclose(rxParameters->status.pVideoRawFile);
    }

    if (pSrd != NULL) {
        destroySRD(pSrd);
        free(pSrd);
    }

    GetTimeOfDay(tNow);

    unsigned long long nTotalTime = ((tNow.tv_sec - rxParameters->status.tStart.tv_sec) * 1000)  +
                        ((tNow.tv_usec - rxParameters->status.tStart.tv_usec) / 1000 );
    printf("[%d] Total Duration : %llu msecs\n\n", rxParameters->status.id, nTotalTime);

    Print_Status(rxParameters->status, tNow);

    Rx_Destroy(pV2DRx);

    delete pScapProcessor;

    delete [] pDecodedAudioBuffer;

    delete [] pEncodedVideoBuffer;
    delete [] pDecodedVideoBuffer;

    delete pV2DFrameDecoder;
    delete rxParameters->status.callbackdata;
    
    return NULL;
}

#ifndef WINDOWS
void sighandler(int sig)
{
    switch (sig) {
    case SIGPIPE:
        break;
    default:
        break;
    }
}
#endif

int main(int argc, char **argv)
{
	RxParameters rxParameters;

    struct timeval tNow;
    GetTimeOfDay(tNow);
    Init_Status(rxParameters.status, tNow);

#ifndef WINDOWS
    signal (SIGPIPE, sighandler);
#endif

    if (!ParseArguments(argc, argv, rxParameters.stRxParams, rxParameters.status))
        return -1;

#define MAX_THREADS 64


#ifdef USE_THREADS

	RxParameters rxParametersArray[MAX_THREADS];
    pthread_t thread[MAX_THREADS];
	void *threadstatus[MAX_THREADS];

	int numbInstances = rxParameters.status.nMultiple;
    for (int i = 0; i < numbInstances; i++) {
        memcpy(&rxParametersArray[i], &rxParameters, sizeof(struct RxParameters));
        rxParametersArray[i].status.id = i;
		if (pthread_create(&thread[i], NULL, RxProcess, (void *)&rxParametersArray[i])) {
			printf("Failed to create thread\n");
			exit(1);
		}
    }

    for (int i = 0; i < numbInstances; i++) {
    	pthread_join(thread[i], &threadstatus[i]);
    }
#else
    /* Not using threads. Only one client is supported */
    RxProcess((void *)&rxParameters);
#endif

    exit(0);

}

