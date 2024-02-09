/*
 * ffcreator.cpp
 *
 *  Created on: Jun 19, 2010
 *      Author: rkale
 */



#include "strmsrv_compatibility.h"

#include <string>
#include "FFMediaProcessor.hh"
#include "FFUtils.hh"
#include "FFMediaProcessorErrorHandler.hh"

using namespace std;

static void usage(char *command )
{
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\t-s String\tMedia Source Directory\n");
    printf("\t-d String\tOutput Directory (default same as source)\n");
    printf("\t-f \t\tEnable fast forward tracks.\n");
    printf("\t-t \t\tEnable primary thumbnail creation.\n");
    printf("\t-i \t\tEnable stream thumbnail creation.\n");
    printf("\t-o Value\tTime offset for primary thumbnail.\n");
    printf("\t-I Value\tInterval for stream thumbnail.\n");
    printf("\t-T String\tComma separated list of tracks to create\n");
    printf("\t-F Stream Type [V2D | RTP | UDP | MPEGTS | SNAPSHOT] \n");
    printf("\t-p SDP file path in case of RTP media \n");
    printf("\t-X \t\tDelete all tracks\n");
    printf("\t-h \t\tShow this help\n");
    exit(1);
}

int main(int argc, char **argv) {

    char   ch;
    string source_media_dir;
    string dest_media_dir;
    string trackList;
    string strStreamType = "";
    string strSDPFilePath = "";
    bool   bEnableFFTracks = false;
    bool   bEnableThumbnails = false;
    bool   bEnableStreamThumbnails = false;
    bool   bDeleteDirectories = false;
    int    nTimeOffsetForThumbnail = 0;
    int    nStreamThumbnailInterval = 0;
    int    rc = 0;

    // Check command line options
    while ((ch = getopt(argc, argv, "s:d:p:fto:T:F:XiI:h")) != -1) {
        switch (ch) {
        case 's':
            source_media_dir = string(optarg);
            break;

        case 'd':
            dest_media_dir = string(optarg);
            break;

        case 'f':
            bEnableFFTracks = true;
            break;

        case 't':
            bEnableThumbnails = true;
            break;

        case 'i':
            bEnableStreamThumbnails = true;
            break;

        case 'o':
            nTimeOffsetForThumbnail = atoi(string(optarg).c_str()) * 1000;
            break;

        case 'I':
            nStreamThumbnailInterval = atoi(string(optarg).c_str()) * 1000;
            break;

        case 'T':
            trackList = string(optarg);
            break;

        case 'F':
            strStreamType = string(optarg);
            break;

        case 'X':
            bDeleteDirectories = true;
            break;

        case 'p':
            strSDPFilePath = string(optarg);
            break;

        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }

    if (source_media_dir.length() == 0) {
        usage(argv[0]);
    }

    if (strStreamType.empty()) {
        usage(argv[0]);
        exit(2);
    }
    else if (strStreamType == "RTP") {
        if (bDeleteDirectories == false) {
            if (strSDPFilePath.empty()) {
                usage(argv[0]);
                exit(3);
            }
        }
    }

    // If destination is not provided it is the same as source
    // This is the usual case.
    // For testing, the tracks can be created in a different directory
    if (dest_media_dir.length() == 0) {
        dest_media_dir = source_media_dir;
    }

    if (bDeleteDirectories) {
        if (bEnableFFTracks == true) {
            CFFUtils::DeleteAllTrackDirectories(dest_media_dir);
        }

        if (bEnableThumbnails == true) {
            CFFUtils::DeleteThumbnailDirectory(dest_media_dir);
        }
        exit(0);
    }

    std::vector<int> trackSpeeds;
    if (trackList.size() > 0) {
        if (!CFFUtils::ParseTrackList(trackList, trackSpeeds)) {
            printf("Error: Invalid track list specified.\n");
            printf("Should be comma separated list of increasing speeds.\n");
            exit(1);
        }
    }
    else {
        // Use default tracks
        trackSpeeds.push_back(2);
        trackSpeeds.push_back(10);
        trackSpeeds.push_back(60);
        trackSpeeds.push_back(256);
    }

    CFFMediaProcessor *mediaProcessor = new CFFMediaProcessor(source_media_dir, dest_media_dir,
            trackSpeeds, strStreamType, 0, false, true, strSDPFilePath);

    if (mediaProcessor == NULL) {
        printf("Error: While creating FFMediaProcessor.\n");
        exit(1);
    }

    mediaProcessor->EnableFFTracks(bEnableFFTracks);

    mediaProcessor->EnablePrimaryTN(bEnableThumbnails);

    mediaProcessor->EnableStreamTN(bEnableStreamThumbnails);

    if (nTimeOffsetForThumbnail  > 0) {
        mediaProcessor->SetPrimaryTNOffset(nTimeOffsetForThumbnail);
    }

    if (nStreamThumbnailInterval > 0) {
        mediaProcessor->SetStreamTNTimeInterval(nStreamThumbnailInterval);
    }

    if (bEnableFFTracks || bEnableThumbnails) {
        if (!mediaProcessor->ProcessMedia()) {
            printf("Error: %s\n", mediaProcessor->GetErrorString().c_str());
            rc = 1;
        }
    }

    if (CFFMediaProcessorErrorHandler::GetThumbnailCreationErrorForSnapshot() != 0)
    	rc = CFFMediaProcessorErrorHandler::GetThumbnailCreationErrorForSnapshot();

    return rc;
}

