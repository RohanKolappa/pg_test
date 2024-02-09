/*
 * ScreenTest.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "common.hh"
#include "winscreenscraper.hh"
#include "bitmap.hh"
#include "vframeencoder.hh"
#include "geometry.hh"
#include "timekeeper.hh"

int main(int argc, char **argv) {

    // Create screen scraper object
    InitSSSyslog(true);

    CWindowsScreenScraper *scraper;
    scraper = new CWindowsScreenScraper(NULL);
    int width = scraper->FrameDimension().width;
    int height = scraper->FrameDimension().height;
    printf("Width %d Height %d\n", width, height);



    // Create V2D Encoder
    CVideoFrameEncoder *pFrameEncoder = new CVideoFrameEncoder(width, height, 6, 6,
            eYUV422P, scraper->GetPixelFormat());
    pFrameEncoder->AllocateBitmap();

    // Get ready for first image
    pFrameEncoder->UpdateAll();
    if (!scraper->Grab()) {
        printf("Error Grabbing first frame\n");
        exit(1);
    }

    // Grab and encode frames
    timestamp_t begintime = CTimeKeeper::GetHighResTime();
    int numbloops = 100;
    for (int i = 0; i < numbloops; i++) {
        if (scraper->Grab()) {
            pFrameEncoder->UpdateFrameFromBuffer((uint8_t *)scraper->GetBuffer());
            //pFrameEncoder->UpdateAll();
            pFrameEncoder->EncodeFrame();
        }
    }
    printf("EncodedFrames = %d\n", (int)pFrameEncoder->GetEncodedFrameCount());
    printf("EncodedSlices = %d\n", (int)pFrameEncoder->GetEncodedSliceCount());
    int timespent = CTimeKeeper::GetHighResTime() - begintime;
    double timetaken = (1.0 * timespent)/HIGHRES_TICKS_PER_SECOND;
    printf("Timetaken = %f\n", timetaken);
    printf("FPS = %f\n", numbloops/timetaken);

    // Save current frame to a file
    //    CBitmap oSaveBmp;
    //    oSaveBmp.SetBitsBGRA(scraper->GetBuffer(), scraper->Width(), scraper->Height());
    //    oSaveBmp.Save("out.bmp", 32);


    delete scraper;
}
