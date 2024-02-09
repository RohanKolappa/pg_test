
#include "CommonIncludes.hh"
#include "MediaServer.hh"
#include "MediaWriter.hh"

using namespace mediaindex;

int main(int argc, char **argv) {

    unsigned long long starttimecode = 0;
    int media_size;
    CMediaWriter *mw = NULL;
    int rc;

    if(argc < 2) {
        printf("Usage: %s <MediaDirectory> [StartTimecode] [DestMediaDirectory]\n", argv[0]);
        exit(1);
    }

    if (argc > 2) {
        starttimecode = STRTOULL(argv[2], NULL, 0);
    }
    CMediaServer *ms = CMediaServer::Create(argv[1]);
    if (ms == NULL) {
        printf("Could not create MediaServer object!!\n");
        return 1;
    }

    if (ms->Init() != MSRC_OK) {
        printf("MediaServer Init Failed: %s\n",
               ms->GetErrorString());
        exit(1);
    }


    // Assuming smaller numbers mean offset
    if (starttimecode < 30000000) {
        starttimecode = ms->GetTimecodeAtOffset(starttimecode);
    }

    if (ms->Start(starttimecode) != MSRC_OK) {
        printf("MediaServer Start Failed: %s\n",
                ms->GetErrorString());
        exit(1);
    }

    if (argc > 3) {
        mw = new CMediaWriter(argv[3]);
    }

    // Initialize media writer if needed
    if (mw != NULL) {
        if (mw->Init() != MSRC_OK) {
            printf("MediaWriter Init Failed: %s\n",
                    mw->GetErrorString());
            exit(1);
        }
    }

#if 0
    if (mw != NULL) {
        if (mw->Start(ms->GetMediaStartTimestamp()) != MSRC_OK) {
            printf("MediaWriter Start Failed: %s\n",
                    mw->GetErrorString());
            exit(1);
        }

    }
#endif

    // Read frames (and write if needed)
    uint64_t lastoffset = 0;
    int frames = 0;
    CCFrame cframe;
    media_size = 0;

    while(1) {
        // Get a frame from media server
        if ((rc = ms->GetFrame(cframe)) != MSRC_OK) {
            if (rc != MSRC_END_OF_MEDIA) {
                printf("MediaServer Get Frame Failed: %s\n",
                        ms->GetErrorString());
            }
            break;
        }

        // Assert Timecodes, offsets are correct
        if (ms->GetOffsetFromTimecode(cframe.GetTimeStamp()) !=
                cframe.GetTimeOffset()) {
            printf("MediaServer Returned inconsistent Offset %"PRIu64" %"PRIu64" at %"PRIu64"\n",
                    cframe.GetTimeOffset(),
                    ms->GetOffsetFromTimecode(cframe.GetTimeStamp()),
                    cframe.GetTimeStamp());
           exit(1);
        }

        if (cframe.GetMessageType() != CCFrame::MediaStopped) {
            // Cannot assert at stop since the timecode returned will be
            // the next start of media
            if (ms->GetTimecodeAtOffset(cframe.GetTimeOffset()) !=
                    cframe.GetTimeStamp()) {
                printf("MediaServer Returned inconsistent Timecode %"PRIu64" %"PRIu64" at %"PRIu64"\n",
                        cframe.GetTimeStamp(),
                        ms->GetTimecodeAtOffset(cframe.GetTimeOffset()),
                        cframe.GetTimeOffset());
                exit(1);
            }
        }

        // Write the frame to MediaWriter
        if (cframe.GetMessageType() == CCFrame::Normal) {
            if (mw != NULL) {
                int rc = mw->PutFrame(cframe);
                if (rc != MSRC_OK) {
                    printf("MediaWriter Write Failed: %s\n",
                            mw->GetErrorString());
                    break;
                }
            }

        }
        else {
            if (cframe.GetMessageType() == CCFrame::MediaStopped) {
                printf("MediaReader Stopped at %"PRIu64"\n", cframe.GetTimeStamp());
                if (mw != NULL && mw->Stop(cframe.GetTimeStamp()) != MSRC_OK) {
                    printf("MediaWriter Stop Failed: %s\n",
                            mw->GetErrorString());
                }
            }
            else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
                printf("MediaReader Started at %"PRIu64"\n", cframe.GetTimeStamp());
                if (mw != NULL && mw->Start(cframe.GetTimeStamp()) != MSRC_OK) {
                    printf("MediaWriter Start Failed: %s\n",
                            mw->GetErrorString());
                }
            }
            else {
                printf("Unexpected frame at %"PRIu64"\n", cframe.GetTimeStamp());
                exit(1);
            }
        }

        if ((cframe.GetTimeOffset() - lastoffset) > 30000) {
            printf("Time Offset = %"PRIu64" TimeStamp = %"PRIu64"\n",
                    cframe.GetTimeOffset(),
                    cframe .GetTimeStamp());
            lastoffset = cframe.GetTimeOffset();
        }

        frames++;
        media_size += cframe.GetLength();
    }


    printf("Found %d frames. Size = %d, TimeSpan = %"PRIu64"\n",
            frames, media_size,
            ms->GetMediaEndTimestamp() - ms->GetMediaStartTimestamp());

    ms->Stop();


    if (mw != NULL) {
        printf("Created media at %s of Size %"PRIu64"\n",
                mw->GetMediaDirectory(), mw->GetBytesWritten());
    }

    delete ms;
    delete mw;
}
