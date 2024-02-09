#include "strmsrv_compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>

//#define USE_SDL

#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#endif

#include "MediaServer.hh"
#include "squeue.hh"
#include "AudioRenderer.hh"

using namespace mediaindex;

const int PACKET_SIZE = 1024;
const int RTP_HEADER_SIZE = 12;
const int DATA_SIZE = PACKET_SIZE - RTP_HEADER_SIZE;

static int packetCount = 0;
static int videoPacketCount = 0;
static int audioPacketCount = 0;

const int MAX_BUFFERS = 10;
int g_nReadIndex = 0;
int g_nWriteIndex = 0;
int g_samplesFilled = 0;

timestamp_t g_beginTime;

CSQueueSource *g_audioSource = NULL;

bool	g_bSDLInitialized = false;


bool ConvertPCMSamplesToNetorkOrder(unsigned char* a_pPCMData,
                                    const int a_nDataLen)
{
    for (int i = 0; i < a_nDataLen; i += 2) {
        unsigned short *nSample = (unsigned short*)(a_pPCMData + i);
        unsigned short nSampleSwapped = htons(*nSample);

        memcpy(a_pPCMData + i, &nSampleSwapped, 2);
    }

    return true;
}

unsigned char g_sampleBuffer[4096 * 8];
int g_offset = 0;
#ifdef USE_SDL
void AudioCallback(void *a_pUserData, Uint8 *a_pStream, int a_nLen)
{

    timestamp_t now = CTimeKeeper::GetTime();
//    printf("AudioCallback %d of size %d timeTaken = %d offset = %d\n",
//            g_samplesFilled, a_nLen,
//            (int) (now - g_beginTime), g_offset);

    // Drain the queue till we collect at least a_nLen worth of data
    if (now - g_beginTime < 10) {
        printf("Called too fast!!\n");
    }
    while (true) {
        CSFrame *sFrame = g_audioSource->GetFrame();
        if (sFrame == NULL)
            break;
        //printf("Copying %d at offset %d\n", sFrame->GetLength(), g_offset);
        memcpy(g_sampleBuffer + g_offset, sFrame->GetData(), sFrame->GetLength());
        g_offset += sFrame->GetLength();
        g_audioSource->ReleaseFrame(sFrame);
        if (g_offset >= a_nLen)
            break;
    }

    // Do we have enough for one sample ?
    if (g_offset >= a_nLen) {
        // Copy a_nLen worth of data into SDL's buffer
        // and move the remaining up into our sample buffer
        memcpy(a_pStream, g_sampleBuffer, a_nLen);
        memmove(g_sampleBuffer, g_sampleBuffer + a_nLen, g_offset - a_nLen);
        g_offset = g_offset - a_nLen;
    } else {
        // Insert silence
        printf("Inserting silence\n");
        memset(a_pStream, 0x00, a_nLen);
    }
    g_samplesFilled++;

}

bool InitializeSDL()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Unable to Init SDL : %s\n ", SDL_GetError());
        return false;
    }

    SDL_AudioSpec *pDesired, *pObtained;

    pDesired = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
    pObtained = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));

    pDesired->freq = 16384;
    pDesired->format = AUDIO_S16MSB;
    pDesired->silence = 0;
    pDesired->samples = 2048;
    pDesired->channels = 2;
    pDesired->callback = AudioCallback;
    pDesired->userdata = NULL;

    if (SDL_OpenAudio(pDesired, pObtained) < 0) {
        fprintf(stderr, "Unable to Open audio : %s\n",SDL_GetError());
        return false;
    }
    fprintf(stderr, "Acquired Audio Device\n");
    printf("pObtained freq = %d\n", pObtained->freq);
    printf("pObtained format = %d\n", pObtained->format);
    printf("pObtained silence = %d\n", pObtained->silence);
    printf("pObtained samples = %d\n", pObtained->samples);
    printf("pObtained channels = %d\n", pObtained->channels);
    printf("pObtained size = %d\n", pObtained->size);

    g_beginTime = CTimeKeeper::GetTime();

    SDL_PauseAudio(0);

    return true;
}

void UnInitializeSDL()
{
    SDL_CloseAudio();
    SDL_Quit();
}
#endif

int main(int argc, const char * argv[])
{
    if (argc < 2) {
        printf("Usage: %s <MediaDirectory>\n", argv[0]);
        exit(1);
    }


#ifdef USE_SDL
    if (!InitializeSDL()) {
        exit(-1);
    }
#else

    CAudioRenderer *pAudioRenderer = new CAudioRenderer(16384, 2, 253, CAudioRenderer::AFSigned16);
    pAudioRenderer->StartStreaming();
#endif


//    CAudioRenderer *pAudioRendererToo = new CAudioRenderer();
//    pAudioRendererToo->StartStreaming();


    char * mediaDir = strdup(argv[1]);
    // Open minute file for RTP media
    CMediaServer *ms = CMediaServer::Create(mediaDir);
    if (ms->Init() != MSRC_OK) {
        printf("MediaServer Init Failed: %s\n", ms->GetErrorString());
        exit(1);
    }


    // start 10 secs into media
    timestamp_t startTimecode = ms->GetTimecodeAtOffset(10000);

    if (ms->Start(startTimecode) != MSRC_OK) {
        printf("MediaServer Start Failed: %s\n",
                ms->GetErrorString());
        exit(1);
    }


    // Create Queues
#ifdef USE_SDL
    CSQueueSink *audioSink = CSQueueManager::Instance()->GetQueueSink("AudioQueue");
    g_audioSource = CSQueueManager::Instance()->GetQueueSource("AudioQueue");
#endif

    int rc;
    CCFrame cframe;
    int errorPacketCount = 0;

    bool done = false;
    while (!done) {
        // Get a frame from media server
        if ((rc = ms->GetFrame(cframe)) != MSRC_OK) {
            if (rc != MSRC_END_OF_MEDIA) {
                printf("MediaServer Get Frame Failed: %s\n",
                        ms->GetErrorString());
            }
            break;
        }

        if (cframe.GetMessageType() == CCFrame::Normal) {
            packetCount++;
            if (cframe.GetMediaFlag() & FRAMETYPE_V2D_AUDIO) {
                audioPacketCount++;
                //printf("Got Audio packet %d of len %d\n", audioPacketCount, cframe.GetLength());

                // Wait till queue is drained
#ifdef USE_SDL
                while (audioSink->Size() > 8) {
                    //usleep(1);
                    SDL_Delay(10);
                }

                CSFrame *sFrame = NULL;

#else
//                while (!pAudioRenderer->HasRoom() || !pAudioRendererToo->HasRoom()) {
//                    usleep(1);
//                }

                while (!pAudioRenderer->HasRoom()) {
                    usleep(1);
                }

#endif


                if (cframe.GetLength() != 1040) {
                    // Assume RTP
#ifdef USE_SDL
                    sFrame = new CSFrame((char *) cframe.GetData() + RTP_HEADER_SIZE,
                            cframe.GetLength() - RTP_HEADER_SIZE);
#else
                    printf("Bummer %d\n", cframe.GetLength());
                    exit(1);
                    ConvertPCMSamplesToNetorkOrder(cframe.GetData() + RTP_HEADER_SIZE,
                            cframe.GetLength() - RTP_HEADER_SIZE);
                    pAudioRenderer->RenderSample(cframe.GetData() + RTP_HEADER_SIZE,
                            cframe.GetLength() - RTP_HEADER_SIZE);
//                    pAudioRendererToo->RenderSample(cframe.GetData() + RTP_HEADER_SIZE,
//                            cframe.GetLength() - RTP_HEADER_SIZE);
#endif

                }
                else {
                    // Legacy recording
#ifdef USE_SDL
                    sFrame = new CSFrame((char *) cframe.GetData() + (16 + 8),
                            cframe.GetLength() - (16 + 8 + 4));
#else
                    ConvertPCMSamplesToNetorkOrder(cframe.GetData() + (16 + 8),
                            cframe.GetLength() - (16 + 8 + 4));
                    pAudioRenderer->RenderSample(cframe.GetData() + (16 + 8),
                            cframe.GetLength() - (16 + 8 + 4));
#endif
                }
#ifdef USE_SDL
                audioSink->InsertFrame(sFrame);
#endif
           }
        }
        else {
            if (cframe.GetMessageType() == CCFrame::MediaStopped) {
                printf("MediaReader Stopped at %llu\n", cframe.GetTimeStamp());
            }
            else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
                printf("MediaReader Started at %llu\n", cframe.GetTimeStamp());
            }
            else {
                printf("Unexpected frame at %llu\n", cframe.GetTimeStamp());
                exit(1);
            }
        }


    }

    printf("Processed %d packets. Video = %d, Audio = %d, Error = %d\n",
            packetCount, videoPacketCount, audioPacketCount, errorPacketCount);

    delete ms;
    free(mediaDir);

#ifdef USE_SDL
    UnInitializeSDL();
#else
    pAudioRenderer->StopStreaming();
    delete pAudioRenderer;
#endif

    return 0;
}
