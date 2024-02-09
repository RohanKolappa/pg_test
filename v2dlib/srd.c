#include "v2dlib_compatibility.h"
#include <math.h>

#include "v2dcommon.h"
#include "srd.h"
#include "v2d.h"
#include "comp_utils.h"
#include "codecdefs.h"
#include "memory_tracker.h"
//#define NO_MEM_POOL

#define UNKNOWNFRAME                            -1
#define LEFTFRAME                               0
#define RIGHTFRAME                              1

#define SM_STEREO                               0
#define SM_LEFT_ONLY                            1
#define SM_RIGHT_ONLY                           2

#define STEREO_DETECT_WINDOW                    10
#define SAMPLINGINTERVAL                        500

#define DEFAULT_TRACK    0

//#define ddprintf(...) printf(__VA_ARGS__)
#define ddprintf(...)

// Defines used by fill_hoFrame
//#define FILL_NORMAL          0
#define FILL_SD_FIRST        1
#define FILL_SD_PROGRESS     2
#define FILL_SD_LAST         3

#define _BYTE_SWAP(x) (uint32_t)(((x) << 24) + (((x)&0xff00) << 8) \
        + (((x) >> 8)&0xff00) + ((x) >> 24))

// Function prototypes for internal functions
static int getNextSlice(sliceRedData_t *srd, int sliceNo, int frameindex, int trackId);
static void fillMarkerSlice(sliceRedData_t *srd,
                            int sliceNumber, int frameindex);
static int prefill_hoFrame(sliceRedData_t *srd, int frameindex, int prefill);
static int fill_hoFrame(sliceRedData_t *srd, int frameindex, int trackId);

static inline int GETSLICENO(unsigned char *buff)
{
    int sNo = (unsigned char)(*((buff)+3) & 0xFF);
    sNo <<= 8;
    sNo |= (unsigned char)(*((buff)+4) & 0xFF);
    sNo >>= 5;

    if (sNo == 0) // V2D Tx sometimes sends corrupted slice numbers
        return -1;

    return sNo;
}

static inline int GETSVCSLICENO(unsigned char *buff)
{
    unsigned long sNo = (unsigned char)(*((buff) + 6));
    sNo <<= 8;
    sNo |= (unsigned char)(*((buff) + 7));
    sNo = (sNo >> 1) & 0x3FFF;

   // printf("Slice info %x %x %lu\n",
      //   *(buff + 6), *(buff + 7), sNo);

    return sNo;
}

static inline int ISREFRESHSLICE(unsigned char *buff)
{
    return (((unsigned char)(*((buff)+4) & 0xFF)) & 0x08);
}


// Simple buffer pool manager. Allocates memory on demand but puts it
// back into a "free" list when memory is released. Maintains a linked
// list of released memory. The "next" pointer for the linked list is
// part of the data itself since it is only used when it is in the
// "free" list
static inline slice_t * getSliceBuffer(sliceRedData_t *srd) {

    slice_t * slicebuffer;
    int trackId;

    if (srd->buffer_pool_head != NULL) {
        // We have something in our stack
        slicebuffer = (slice_t *) srd->buffer_pool_head;
        srd->buffer_pool_head = srd->buffer_pool_head->m_next;
    }
    else {
        // Allocate a new buffer and return it. Only when it is released,
        // we will put it in our stack
        slicebuffer  = (slice_t *) malloc(sizeof(slice_t));
        if (slicebuffer == NULL)
            return NULL;
        slicebuffer->sliceData  = (unsigned char *) malloc(NOMINAL_SLICE_SIZE);
        if (slicebuffer->sliceData == NULL) {
            free(slicebuffer);
            return NULL;
        }
        srd->buffer_pool_count++;
    }
    slicebuffer->refcount = 1;
    slicebuffer->sizeofSlice = 0;
    for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
        slicebuffer->flags[trackId] = 0;
    }
    return slicebuffer;
}

static inline void releaseSliceBuffer(sliceRedData_t *srd,
                                      slice_t *slicebuffer) {
    int trackId = DEFAULT_TRACK;

    if (slicebuffer == NULL)
        return;

    // Decrease refcount and put it back into our pool if it goes to zero
    slicebuffer->refcount--;

    if (slicebuffer->refcount <= 0) {

        if (GET_REALLOCED(slicebuffer)) {
            // This was a larger than nominal size buffer, restore it to original
            // size before we put it back in to our pool
            free(slicebuffer->sliceData);
            slicebuffer->sliceData  = (unsigned char *) malloc(NOMINAL_SLICE_SIZE);
            if (slicebuffer->sliceData == NULL) {
                free(slicebuffer);
                return;
            }
        }

        {
            struct buffer_pool_list *last_head = srd->buffer_pool_head;
            srd->buffer_pool_head = (struct buffer_pool_list *) slicebuffer;
            srd->buffer_pool_head->m_next = last_head;
        }
    }
}

static inline  int growSliceBuffer(sliceRedData_t *srd,
                                      slice_t *slicebuffer)
{
    int trackId = DEFAULT_TRACK;
    slicebuffer->sliceData =
            (unsigned char *) realloc(slicebuffer->sliceData, MAX_SLICE_SIZE);
    if (slicebuffer->sliceData == NULL)
        return -1;
    srd->realloc_count++;
    SET_REALLOCED(slicebuffer);
    return 0;
}

static inline void fillSliceBuffer(slice_t *slicebuffer,
                                   unsigned char *inbuffer, int length)
{
    if (slicebuffer != NULL) {
        memcpy(slicebuffer->sliceData, inbuffer, length);
        slicebuffer->sizeofSlice = length;
    }

}

static inline void copySliceBuffer(slice_t *dstSlicebuffer,
                                   slice_t *srcSlicebuffer)
{
    if (dstSlicebuffer != NULL && srcSlicebuffer != NULL) {
        memcpy(dstSlicebuffer->sliceData,
               srcSlicebuffer->sliceData,
               srcSlicebuffer->sizeofSlice);
        dstSlicebuffer->sizeofSlice = srcSlicebuffer->sizeofSlice;
    }

}

/**
 * parsePackets - parses a packet to find frame boundaries
 * and create a new frame
 * Input:
 *   rcvbuf - pointer to packet data
 *
 * Output:
 *   number of packets parsed.
 */
int parsePackets(sliceRedData_t *srd, unsigned char* pTotalData, int iTotalLen)
{
    unsigned char* pData;
    int iPackets = 0;
    int iOffset = 0;
    unsigned long nSeqNum;

    do
    {
        // parse one packet at a time
        iOffset = (iPackets*SIZE_OF_AV_PACKET) + 2*SRD_FRAME_SEQ_NUM_LEN;
        pData = pTotalData + iOffset;

        nSeqNum = *(unsigned long *) (pData - 8);

        if (srd->m_nVideoSeq && (srd->m_nVideoSeq != nSeqNum)) {
            if (srd->m_nVideoSeq < nSeqNum) {
                srd->m_nVideoLoss += (nSeqNum - srd->m_nVideoSeq);
                srd->m_nVideoSeq = nSeqNum + 1;
            }
            else {
                srd->m_nVideoOutOfOrder++;
                if (srd->m_nVideoOutOfOrder % 10 == 0)
                    srd->m_nVideoSeq = 0;
            }
        }
        else {
            srd->m_nVideoSeq = nSeqNum + 1;
        }

        // parse the data for frame
        findFramesinPacket(srd, pData);

        iPackets++;
        if ( SIZE_OF_AV_PACKET * iPackets >= iTotalLen  )
            break;

    } while(1);

    return iPackets;

}

static void setStereoMode(sliceRedData_t *srd) {
    if (srd->isStereo) {
        switch (srd->stereo_policy) {
        case SP_SOURCE:
            srd->stereo_mode = SM_STEREO;
            break;
        case SP_MONO:
            srd->stereo_mode = SM_LEFT_ONLY;
            break;
        case SP_LEFT_ONLY:
            srd->stereo_mode = SM_LEFT_ONLY;
            break;
        case SP_RIGHT_ONLY:
            srd->stereo_mode = SM_RIGHT_ONLY;
            break;
        default:
            V2DASSERT(0);
        }
    }
    else {
        // Mono video is always stored in RIGHT bank
        srd->stereo_mode = SM_RIGHT_ONLY;
    }
}

static void computeFrameStats(sliceRedData_t *srd, int frameindex,
        int refreshSlices, int byteCount,
        int sliceCount, int slice_sent_count,
        int totalrefreshSlice) {
    srd->inFrameCount++;


    totalrefreshSlice += refreshSlices;

    srd->isInputFrameStatic = 0;
    if (totalrefreshSlice >= srd->MaxSliceNo - 1)
        srd->isInputFrameStatic = 1;

    // Compute average slice size
#define ALPHA 0.1
    srd->avgSliceSize = (double)ALPHA * ((double)byteCount/sliceCount) +
            (double)(1.0 - ALPHA) * srd->avgSliceSize;

    srd->avgFrameSize = (double)ALPHA * byteCount +
            (double)(1.0 - ALPHA) * srd->avgFrameSize;

    srd->avgRefreshSlices = (double)ALPHA * refreshSlices +
            (double)(1.0 - ALPHA) * srd->avgRefreshSlices;

    srd->nRefreshSliceRate = (int)RINT(srd->avgRefreshSlices);

    if (srd->nRefreshSliceRate > 30)
        srd->nRefreshSliceRate = 30;

    if (srd->MaxSliceNo > 0) {

        // Compute average frame fullness
        srd->avgFrameCoverage =
                (double)ALPHA * (100.0 * sliceCount)/srd->MaxSliceNo +
                (double)(1.0 - ALPHA) * srd->avgFrameCoverage;

        // Was this an "Iframe"
        if (sliceCount >= srd->MaxSliceNo) {
            srd->IFrameCount++;
            srd->IFrameSize = byteCount;
            //            LOG_SYS(V2DLIB_INFO, "V2DSRD: Received an IFrame %ld @ %ld\n",
            //                    srd->IFrameCount, srd->inFrameCount);
            LOG_SYS(V2DLIB_INFO, "V2DSRD: Received an %s IFRAME with %d Refresh slices @ %ld\n",
                    srd->isRightFrame ? "RIGHT" : "LEFT", refreshSlices, srd->inFrameCount);
        }
        else if (sliceCount >= (srd->MaxSliceNo - 2)) {
            LOG_SYS(V2DLIB_INFO, "V2DSRD: Received an ALMOST %s IFRAME with %d Refresh slices out of %d @ %ld\n",
                    srd->isRightFrame ? "RIGHT" : "LEFT", refreshSlices, sliceCount, srd->inFrameCount);
        }

        // Did we get a full frame yet? If so, reset and start accumulating
        // full frame stats again
        if (slice_sent_count >= srd->MaxSliceNo) {
            memset(&(srd->fullframe_slice_sent[frameindex][0]), 0x00,
                    sizeof(char) * MAX_SLICE_NO);
            srd->FullFrameCount++;
            srd->FullFrameInterval = srd->inFrameCount - srd->lastInFrameCount;
            srd->lastInFrameCount = srd->inFrameCount;
            if (srd->FirstFullFrameInterval == 0)
                srd->FirstFullFrameInterval = srd->FullFrameInterval;
        }

    }

    if (srd->isRightFrame) {
        srd->inRightFrameCount++;
        if (srd->mono_frameindex != RIGHTFRAME && !srd->isStereo) {
            LOG_SYS(V2DLIB_INFO, "V2DSRD: Switched to Right @ %ld \n", srd->inFrameCount);
            srd->mono_frameindex = RIGHTFRAME;
            setStereoMode(srd);
        }
    }
    else {
        srd->inLeftFrameCount++;
        if (srd->mono_frameindex != LEFTFRAME && !srd->isStereo) {
            LOG_SYS(V2DLIB_INFO, "V2DSRD: Switched to Left @ %ld\n", srd->inFrameCount);
            srd->mono_frameindex = LEFTFRAME;
            setStereoMode(srd);
        }
    }
}


static int flipFrameBuffersForTrack(sliceRedData_t *srd, int trackId)
{
    int count;
    int frameindex;
    int mark_this_slice = FALSE;
    int was_dirty = FALSE;
    int slice_sent_count = 0;
    int byteCount = 0;
    int bRefreshSlice;
    int b444Slice;


    int sliceCount = 0;
    int refreshSlices = 0; /* this variable gives refresh slices in current frame */
    int totalrefreshSlice = 0 ; /* this variable keeps track of prev+current frame refresh slices */

    if (srd->isStereo) {
        if (srd->isRightFrame)
            frameindex = RIGHTFRAME;
        else
            frameindex = LEFTFRAME;
    }
    else {
        // For mono video, always use the RIGHT frame bank
        frameindex = RIGHTFRAME;
    }

    // A frame always has slice zero in normal mode
    V2DASSERT(srd->isSVC || trackId != 0 ||
            srd->m_wrFrame[frameindex].slicebuffer[0] != NULL);

    if (srd->hoSliceIdx[frameindex] >= 0) {
        // We were in the middle of slice-drain mode with
        // some slices already written. Fill in the slices remaining
        // in this frame and close it out.
        prefill_hoFrame(srd, frameindex, FILL_SD_LAST);

        // Since this frame went directly to hoBuff, mark this
        // frame as already flushed.
        srd->m_rdFrameFlushed[trackId][frameindex] = TRUE;
    }
    else if (srd->m_rdFrameFlushed[trackId][frameindex] == FALSE) {
        // Previous frame still in in buffer so will
        // be overwriting (i.e. reducing) a frame
        if (trackId == 0)
            srd->reducedFrameCount++;
        else
            srd->reducedFrameCountPerTrack[trackId]++;
    }
    else {
        // New frame has come in that needs to be flushed
        srd->m_rdFrameFlushed[trackId][frameindex] = FALSE;
    }

    // Set marker for slice-drain mode
    if (trackId == 0)
        srd->wrSliceIdx = -1;

    // Copy the Frame Header slice to the appropriate (left or right)
    // rdFrame
    if (srd->isSVC && srd->m_wrFrame[frameindex].headerbuffer != NULL) {
        if (srd->m_rdFrame[trackId][frameindex].headerbuffer != NULL) {
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][frameindex].headerbuffer);
            srd->m_rdFrame[trackId][frameindex].headerbuffer = NULL;
        }
        srd->m_rdFrame[trackId][frameindex].headerbuffer =
                srd->m_wrFrame[frameindex].headerbuffer;
        srd->m_rdFrame[trackId][frameindex].headerbuffer->refcount++;
    }


    // Copy slices from wrFrame to the appropriate (left or right)
    // rdFrame. Slice reduction occurs if there is already an older
    // slice in the rdFrame slot
    for (count = 0; count < MAX_SLICE_NO; count++)
    {
        was_dirty = FALSE;
        if (srd->m_wrFrame[frameindex].slicebuffer[count] != NULL) {
            bRefreshSlice = IS_REFRESH_SLICE(srd->m_wrFrame[frameindex].slicebuffer[count]);

            // In 4:4:4 mode, a refresh slice has additional color info, that we
            // need to treat differently
            b444Slice = (srd->is444 && bRefreshSlice) ? 1 : 0;

            // 'Slice Reduction' about to happen
            if (!b444Slice) {
                if (srd->m_rdFrame[trackId][frameindex].slicebuffer[count] != NULL) {
                    // m_rdFrame always keeps all the latest slices (one full frame
                    // is always available). Only slices marked as flushed have
                    // actually been sent out, so the remaining are what we are
                    // 'reducing'
                    if (!IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].slicebuffer[count])) {
                        //V2DASSERT(srd->m_rdFrameFlushed[frameindex] == FALSE);
                        if (trackId == 0) {
                            srd->reducedSlices++;
                            srd->reducedBytes +=
                                    srd->m_rdFrame[trackId][frameindex].slicebuffer[count]->sizeofSlice;
                        }
                        else {
                            srd->reducedSlicesPerTrack[trackId]++;
                            srd->reducedBytesPerTrack[trackId] +=
                                    srd->m_rdFrame[trackId][frameindex].slicebuffer[count]->sizeofSlice;

                        }
                        if (srd->mark_slices)
                            mark_this_slice = TRUE;
                        if (GET_DIRTY_SLICE(srd->m_rdFrame[trackId][frameindex].slicebuffer[count]))
                            was_dirty = TRUE;
                    }
                    releaseSliceBuffer(srd, srd->m_rdFrame[trackId][frameindex].slicebuffer[count]);
                    srd->m_rdFrame[trackId][frameindex].slicebuffer[count] = NULL;
                }
            }
            else {
                // Cannot overwrite 4:4:4 slice over the previous slice.
                // We copy to altSliceBuffer instead.
                if (srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count] != NULL) {
                    if (!IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count])) {
                        if (trackId == 0) {
                            srd->reducedSlices++;
                            srd->reducedAltSlices++;
                            srd->reducedBytes +=
                                    srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count]->sizeofSlice;
                        }
                        else {
                            srd->reducedSlicesPerTrack[trackId]++;
                            srd->reducedAltSlicesPerTrack[trackId]++;
                            srd->reducedBytesPerTrack[trackId] +=
                                    srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count]->sizeofSlice;

                        }
                        if (srd->mark_slices)
                            mark_this_slice = TRUE;
                    }
                    releaseSliceBuffer(srd, srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count]);
                    srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count] = NULL;
                }
            }
            V2DASSERT(srd->m_wrFrame[frameindex].slicebuffer[count]->sizeofSlice > 0);

            // Copy new slice into the rdFrame slot
            if (!b444Slice) {
                V2DASSERT(srd->m_rdFrame[trackId][frameindex].slicebuffer[count] == NULL);
                srd->m_rdFrame[trackId][frameindex].slicebuffer[count] =
                        srd->m_wrFrame[frameindex].slicebuffer[count];
                srd->m_rdFrame[trackId][frameindex].slicebuffer[count]->refcount++;
                if (!bRefreshSlice || was_dirty) {
                    SET_DIRTY_SLICE(srd->m_rdFrame[trackId][frameindex].slicebuffer[count]);
                }
            }
            else {
                // Refresh slice in 4:4:4 mode goes into alt slice buffer
                V2DASSERT(srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count] == NULL);
                srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count] =
                        srd->m_wrFrame[frameindex].slicebuffer[count];
                srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count]->refcount++;
            }

            // If a new regular slice comes in 4:4:4 mode, we can invalidate the
            // older refresh slice
            if (srd->is444 && !b444Slice) {
                releaseSliceBuffer(srd, srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count]);
                srd->m_rdFrame[trackId][frameindex].altSlicebuffer[count] = NULL;
            }

            // How many bytes seen in this one frame?
            byteCount += srd->m_wrFrame[frameindex].slicebuffer[count]->sizeofSlice;

            // How many refresh slices seen in this frame?
            if (bRefreshSlice)
                refreshSlices++;

            // If in special "marker" mode, mark all reduced slices
            if (srd->mark_slices && !srd->isSVC) {
                if (mark_this_slice && count != 0) {
                    fillMarkerSlice(srd, count, frameindex);
                }
                mark_this_slice = FALSE;
            }

            //srd->m_wrFrame[frameindex].slicebuffer[count] = NULL; // TODO delete this

            sliceCount++; // how many slices seen in this one frame;

            if (trackId == 0)
                srd->fullframe_slice_sent[frameindex][count] = 1;

            // Is this an unexpected slice?
            if (srd->MaxSliceNo && count >= srd->MaxSliceNo) {
                srd->parse_errors_bad_frame++;
            }
        }
        else {
            if (srd->m_rdFrame[trackId][frameindex].slicebuffer[count] != NULL) {
                if (IS_REFRESH_SLICE(srd->m_rdFrame[trackId][frameindex].slicebuffer[count]))
                    totalrefreshSlice++;
            }
        }

        // Track slices for full frame detection
        if (trackId == 0) {
            if (srd->fullframe_slice_sent[frameindex][count] == 1) {
                slice_sent_count++;
            }
        }
    }

    // Copy over the timestamp to rdFrame
    srd->m_rdFrame[trackId][frameindex].timestamp = srd->m_wrFrame[frameindex].timestamp;
    ddprintf("To rdFrame: %u\n", srd->m_rdFrame[trackId][frameindex].timestamp);

    if (srd->isSVC && srd->m_wrFrame[frameindex].footerbuffer != NULL) {
        if (srd->m_rdFrame[trackId][frameindex].footerbuffer != NULL) {
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][frameindex].footerbuffer);
            srd->m_rdFrame[trackId][frameindex].footerbuffer = NULL;
        }
        srd->m_rdFrame[trackId][frameindex].footerbuffer
                = srd->m_wrFrame[frameindex].footerbuffer;
        srd->m_wrFrame[frameindex].footerbuffer = NULL;
    }

    if (srd->is444 && srd->isRightFrame == 0) {
        LOG_SYS(V2DLIB_INFO, "V2DSRD: Received a LEFT frame @ %ld\n",
                    srd->inFrameCount);
    }

    if (trackId == 0) {
        computeFrameStats(srd, frameindex, refreshSlices, byteCount,
                sliceCount, slice_sent_count, totalrefreshSlice);

        // Reset the prevSliceNo since we will now start collecting a new frame
        srd->prevSliceNo = -1;
    }
    return 0;
}

static void flipFrameBuffers(sliceRedData_t *srd)
{
    // To measure average frame arrival period
    int delta = 0;
    struct timeval tCurrFrameArrival = {0, 0};
    int trackId;
    int frameindex;
    int count;

    // Call flip frame for each track
    for (trackId = 0; trackId < srd->track_count; trackId++) {
        flipFrameBuffersForTrack(srd, trackId);
    }

    // We are done with wrFrame and can release all slice buffers in it
    for (frameindex = 0; frameindex < 2; frameindex++) {
        if (srd->isSVC && srd->m_wrFrame[frameindex].headerbuffer != NULL) {
            releaseSliceBuffer(srd, srd->m_wrFrame[frameindex].headerbuffer);
            srd->m_wrFrame[frameindex].headerbuffer = NULL;
        }
        for (count = 0; count < MAX_SLICE_NO; count++) {
            releaseSliceBuffer(srd, srd->m_wrFrame[frameindex].slicebuffer[count]);
            srd->m_wrFrame[frameindex].slicebuffer[count] = NULL;
        }
    }

    // Compute inter frame arrival time
    if (srd->inFrameCount == 1) {
        GETTIMEOFDAY(&srd->tPrevFrameArrival, NULL);
        srd->lastInFrameCnt = srd->inFrameCount;
    }

    GETTIMEOFDAY(&tCurrFrameArrival, NULL);
    delta = ((tCurrFrameArrival.tv_sec - srd->tPrevFrameArrival.tv_sec) * 1000)  +
        ((tCurrFrameArrival.tv_usec - srd->tPrevFrameArrival.tv_usec)/1000 );


    if (delta > SAMPLINGINTERVAL) {
        unsigned int avgFrameArrPeriod =
            delta / (srd->inFrameCount - srd->lastInFrameCnt);
        srd->tPrevFrameArrival = tCurrFrameArrival;
        srd->lastInFrameCnt = srd->inFrameCount;

        srd->avgFrameArrPeriod =
            (80 * avgFrameArrPeriod + 20 * srd->avgFrameArrPeriod)/100;
    }


    // Callback user function to indicate new frame arrival
    if (srd->m_newFrameCallback != NULL)
        srd->m_newFrameCallback(srd->m_newFrameCallbackObject);

}

static inline int updateSlice(sliceRedData_t *srd, int sliceNo, int sliceSize, int bRefreshSlice)
{
    int frameindex;
    int i;

    V2DASSERT(sliceSize != 0);
    V2DASSERT(sliceSize <= MAX_SLICE_SIZE);
    V2DASSERT(!(srd->isSliceZero == TRUE && sliceNo != 0));
    V2DASSERT(srd->hbuff != NULL);

    // V2D Tx sometimes sends corrupted slice numbers. Check for these
    // and ignore the slices
    if (sliceNo == -1) {
        srd->parse_errors_bad_slice++;
        return -1;
    }
    if (srd->MaxSliceNo && sliceNo >= srd->MaxSliceNo) {
        srd->parse_errors_bad_slice++;
        return -1;
    }

    // Update stats
    srd->inBytes += sliceSize;
    srd->inSlices++;

    // Copy slice data into the appropriate slot of the wrFrame
    if (srd->isStereo) {
        if (srd->isRightFrame)
            frameindex = RIGHTFRAME;
        else
            frameindex = LEFTFRAME;
    }
    else {
        // For mono video, always use the RIGHT frame bank
        frameindex = RIGHTFRAME;
    }

    V2DASSERT(srd->m_wrFrame[frameindex].slicebuffer[sliceNo] == NULL);
    srd->hslicebuffer->sizeofSlice = sliceSize;
    srd->hslicebuffer->sliceNumber = sliceNo;
    srd->m_wrFrame[frameindex].slicebuffer[sliceNo] = srd->hslicebuffer;

    if (sliceNo == 0) {
        srd->m_wrFrame[frameindex].timestamp = srd->currentFrameTimestamp;
        ddprintf("To wrFrame: %u\n", srd->m_wrFrame[frameindex].timestamp);
    }
    if (bRefreshSlice)
        SET_REFRESH_SLICE(srd->hslicebuffer);

    if (srd->isSVC) {
        SVCSliceHeader sliceHeader;
        ParseSVCSliceLayers(srd->m_wrFrame[frameindex].slicebuffer[sliceNo]->sliceData,
                srd->m_wrFrame[frameindex].slicebuffer[sliceNo]->sizeofSlice, &sliceHeader);
        if (sliceHeader.nNumberOfLayers > 0) {
            for (i = 0; i < (int)sliceHeader.nNumberOfLayers; i++) {
                if (sliceHeader.layerHeaders[i].nLayerType == SVC_LAYERTYPE_DCLAYER)
                    srd->svcDCBytes += sliceHeader.layerHeaders[i].nLayerSize;
                else if (IsMidACLayerType(sliceHeader.layerHeaders[i].nLayerType))
                    srd->svcQuadBytes += sliceHeader.layerHeaders[i].nLayerSize;
                else if (IsRemACLayerType(sliceHeader.layerHeaders[i].nLayerType))
                    srd->svcRemBytes += sliceHeader.layerHeaders[i].nLayerSize;
            }
        }
    }
    // Get ready for new slice
    srd->hslicebuffer = getSliceBuffer(srd);
    V2DASSERT(srd->hslicebuffer != NULL);
    V2DASSERT(srd->hslicebuffer->sliceData != NULL);
    srd->hbuff = srd->hslicebuffer->sliceData;

    // wrSliceIdx is the current slice number just written into the
    // wrFrame. Used by slice-drain mode logic to track where we are at.
    if (srd->wrSliceIdx == -1 && !srd->isSVC) {
        // The first slice updated better be slice Zero
        V2DASSERT(sliceNo == 0);
    }
    srd->wrSliceIdx = sliceNo;

    return 0;
}

static inline int updateFrameHeader(sliceRedData_t *srd, int headerSize)
{
    int frameindex;
    int rc;
    SVCFrameHeader frameHeader;

    V2DASSERT(headerSize != 0);
    V2DASSERT(srd->hbuff != NULL);
    V2DASSERT(srd->isSVC == 1);

    // Update stats
    srd->inBytes += headerSize;

    // Find the appropriate slot of the wrFrame
    if (srd->isStereo) {
        if (srd->isRightFrame)
            frameindex = RIGHTFRAME;
        else
            frameindex = LEFTFRAME;
    }
    else {
        // For mono video, always use the RIGHT frame bank
        frameindex = RIGHTFRAME;
    }


    // Copy header data into the appropriate slot of the wrFrame
    V2DASSERT(srd->m_wrFrame[frameindex].headerbuffer == NULL);
    srd->hslicebuffer->sizeofSlice = headerSize;
    srd->hslicebuffer->sliceNumber = 0;
    srd->m_wrFrame[frameindex].headerbuffer = srd->hslicebuffer;

    // Get ready for new slice
    srd->hslicebuffer = getSliceBuffer(srd);
    V2DASSERT(srd->hslicebuffer != NULL);
    V2DASSERT(srd->hslicebuffer->sliceData != NULL);
    srd->hbuff = srd->hslicebuffer->sliceData;

    // TODO parse the  header

    rc = ParseSVCFrameHeader(srd->m_wrFrame[frameindex].headerbuffer->sliceData,
            srd->m_wrFrame[frameindex].headerbuffer->sizeofSlice, &frameHeader);
    if (rc) {
        if (frameHeader.bFrameDimensions &&
                frameHeader.nSliceWidth > 0 && frameHeader.nSliceHeight > 0) {
            int maxSliceNo = (frameHeader.nEncodedWidth * frameHeader.nEncodedHeight) /
                    (frameHeader.nSliceWidth * frameHeader.nSliceHeight * 64);
            if (maxSliceNo > 0 && maxSliceNo != srd->MaxSliceNo) {
                srd->MaxSliceNo = maxSliceNo;
            }
        }
    }
    V2DASSERT(srd->wrSliceIdx == -1);

    return 0;
}

static inline int updateEndOfFrame(sliceRedData_t *srd, int footerSize)
{
    int frameindex;

    V2DASSERT(footerSize != 0);
    V2DASSERT(srd->hbuff != NULL);
    V2DASSERT(srd->isSVC == 1);

    // Update stats
    srd->inBytes += footerSize;

    // Find the appropriate slot of the wrFrame
    if (srd->isStereo) {
        if (srd->isRightFrame)
            frameindex = RIGHTFRAME;
        else
            frameindex = LEFTFRAME;
    }
    else {
        // For mono video, always use the RIGHT frame bank
        frameindex = RIGHTFRAME;
    }

    V2DASSERT(srd->m_wrFrame[frameindex].footerbuffer == NULL);
    srd->hslicebuffer->sizeofSlice = footerSize;
    srd->hslicebuffer->sliceNumber = 0;
    srd->m_wrFrame[frameindex].footerbuffer = srd->hslicebuffer;

    // Get ready for new slice
    srd->hslicebuffer = getSliceBuffer(srd);
    V2DASSERT(srd->hslicebuffer != NULL);
    V2DASSERT(srd->hslicebuffer->sliceData != NULL);
    srd->hbuff = srd->hslicebuffer->sliceData;

    return 0;

}

void SRDSetPacketTimestamp(sliceRedData_t *srd, uint32_t timeStamp)
{
    srd->currentPacketTimestamp = timeStamp;
}

int findFramesinPacket(sliceRedData_t *srd, unsigned char* pData)
{

    int tok = TOKEN_NONE;
    int sliceNo;
    int bRefreshSlice;
    int len = 0;
    int readLen = 0;
    unsigned char * readPtr = pData;
    unsigned char * buff;
    int trackId = DEFAULT_TRACK;

    if (srd->isSVC) {
        return findSVCFramesinPacket(srd, pData, SRD_VIDEO_PACKET_SIZE);
    }
    srd->inPktCount++;

    // Out of memory check
    if (srd->hbuff == NULL)
        return 0;

    // First check if we have enough room in the current hbuff to
    // write at least one packet worth
    if((srd->hbuff_wroffset + SRD_VIDEO_PACKET_SIZE) >= NOMINAL_SLICE_SIZE) {
        if (!GET_REALLOCED(srd->hslicebuffer)) {
            if (growSliceBuffer(srd, srd->hslicebuffer) == -1)
                return 0;
            srd->hbuff = srd->hslicebuffer->sliceData;
        }
    }
    if((srd->hbuff_wroffset + SRD_VIDEO_PACKET_SIZE) >= MAX_SLICE_SIZE) {
        srd->hbuff_overflow++;
        srd->hbuff_wroffset = 0;
        srd->isSliceZero = FALSE;
        srd->isRightFrame = 0;
        srd->parseMode = LOOK_FOR_SOF;
    }

    // Parse the input 4 bytes at a time looking for SOF or SOS
    // (start of frame or start of slice codes)
    while (len < SRD_VIDEO_PACKET_SIZE)
    {
        buff = pData + len;
        if (*(buff) == 0) {
            if (*((buff)+1) == 0) {
                if (*((buff)+2) == 0x01) {
                    if (*((buff)+3) == 0xa0) {
                        tok = TOKEN_START_LEFT_FRAME;
                    }
                    else if(*((buff)+3) == 0) {
                        tok = TOKEN_START_RIGHT_FRAME;
                    }
                }
            }
            else if (*((buff)+1) == 0x01) {
                if (*((buff)+2) == 0xb2)
                    tok = TOKEN_START_SLICE;
            }
        }

        if (tok != TOKEN_NONE) {

            if (srd->parseMode == LOOK_FOR_SOF) {

                switch(tok) {

                case TOKEN_START_RIGHT_FRAME:
                    srd->isRightFrame = 1;
                    // fall through
                    /* no break */
                case TOKEN_START_LEFT_FRAME:
                    srd->hbuff_wroffset = 0;
                    srd->isSliceZero = TRUE;
                    srd->parseMode = LOOK_FOR_SOS;
                    readPtr = pData + len;
                    readLen = 0;
                    srd->currentFrameTimestamp = srd->currentPacketTimestamp;
                    ddprintf("To currentFrameTimestamp (SOF): %u\n", srd->currentFrameTimestamp);
                    break;

                case TOKEN_START_SLICE:

                    if (srd->inSlices > 0) // skip the first partial frame
                        srd->parse_errors_sof++;
                    srd->hbuff_wroffset = 0;
                    readPtr = pData + len;
                    readLen = 0;
                    break;

                }

            }

            else { // srd->parseMode == LOOK_FOR_EOSL

                switch(tok) {

                case TOKEN_START_RIGHT_FRAME:
                    // fall through
                case TOKEN_START_LEFT_FRAME:
                    memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr, readLen);
                    srd->hbuff_wroffset += readLen;
                    readPtr = pData + len;
                    readLen = 0;

                    bRefreshSlice = 0;
                    if (srd->isSliceZero == FALSE) {
                        sliceNo = GETSLICENO(&srd->hbuff[0]);
                        bRefreshSlice = ISREFRESHSLICE(&srd->hbuff[0]);
                    }
                    else {
                        sliceNo = 0;
                    }
                    if (sliceNo > srd->prevSliceNo) {
                        if (updateSlice(srd, sliceNo, srd->hbuff_wroffset, bRefreshSlice) != -1)
                            srd->prevSliceNo = sliceNo;
                    }
                    else {
                        srd->parse_errors_ooo_slice++;
                    }


                    flipFrameBuffers(srd);

                    srd->hbuff_wroffset = 0;

                    srd->isSliceZero = TRUE;

                    srd->currentFrameTimestamp = srd->currentPacketTimestamp;
                    ddprintf("To currentFrameTimestamp: %u\n", srd->currentFrameTimestamp);

                    // set the left/right frame marker for the next frame
                    if (tok == TOKEN_START_RIGHT_FRAME)
                        srd->isRightFrame = 1;
                    else
                        srd->isRightFrame = 0;

                    break;

                case TOKEN_START_SLICE:

                    memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr, readLen);
                    srd->hbuff_wroffset += readLen;
                    readPtr = pData + len;
                    readLen = 0;


                    bRefreshSlice = 0;
                    if (srd->isSliceZero == FALSE) {
                        sliceNo = GETSLICENO(&srd->hbuff[0]);
                        bRefreshSlice = ISREFRESHSLICE(&srd->hbuff[0]);
                    }
                    else {
                        // Slice 0 is always send to no refresh flag
                        sliceNo = 0;
                    }

                    if (sliceNo > srd->prevSliceNo)
                    {
                        if (updateSlice(srd, sliceNo, srd->hbuff_wroffset, bRefreshSlice) != -1)
                            srd->prevSliceNo = sliceNo;

                        srd->hbuff_wroffset = 0;

                        srd->isSliceZero = FALSE;

                        srd->parseMode = LOOK_FOR_SOS;
                    }
                    else {
                        // Missed EOF and next SOF so start over
                        flipFrameBuffers(srd);

                        srd->parse_errors_ooo_slice++;

                        srd->hbuff_wroffset = 0;

                        srd->isRightFrame = 0;
                        srd->parseMode = LOOK_FOR_SOF;
                    }

                    break;

                }

            }
            tok = TOKEN_NONE; // reset for next round -- optimization
        }

        len += 4;
        readLen += 4;

    }

    /* Copy the remaining bytes into our hbuff */
    memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr, readLen);
    srd->hbuff_wroffset += readLen;


    return 0;
}

void CopyFirst4bytesData(sliceRedData_t *srd, unsigned int bytedata, int len)
{
	unsigned int temp = (_BYTE_SWAP(bytedata));
	memcpy(&srd->hbuff[srd->hbuff_wroffset], &(temp), 4 - len);
	srd->hbuff_wroffset += 4 - len;

}

int findSVCFramesinPacket(sliceRedData_t *srd, unsigned char* pData, int pkt_len)
{
    int tok = TOKEN_NONE;
    int sliceNo;
    int bRefreshSlice;
    int len = 0;
    int readLen = 0;
    unsigned char * readPtr = pData;
    unsigned char * buff;
    unsigned int bytedata = srd->last4bytes;
    int bytecount = 0;
    unsigned char cdata;
    int trackId = DEFAULT_TRACK;

    srd->inPktCount++;

    // Out of memory check
    if (srd->hbuff == NULL)
        return 0;

    // First check if we have enough room in the current hbuff to
    // write at least one packet worth
    if ((srd->hbuff_wroffset + pkt_len) >= NOMINAL_SLICE_SIZE) {
        if (!GET_REALLOCED(srd->hslicebuffer)) {
            if (growSliceBuffer(srd, srd->hslicebuffer) == -1)
                return 0;
            srd->hbuff = srd->hslicebuffer->sliceData;
        }
    }

    if ((srd->hbuff_wroffset + pkt_len) >= MAX_SLICE_SIZE) {
        srd->hbuff_overflow++;
        srd->hbuff_wroffset = 0;
        srd->isFirstSlice = FALSE;
        srd->isRightFrame = 0;
        srd->parseMode = LOOK_FOR_SOF;
    }

    // Parse the input 4 bytes at a time looking for SOF or SOS
    // (start of frame or start of slice codes)
    while (len < pkt_len - 4) {
        cdata = (unsigned char) (bytedata >> 24);
        if ((cdata) == 0) {
            cdata = (unsigned char) ((bytedata >> 16) & 0x000000FF);
            if (((cdata)) == 0) {
                cdata = (unsigned char) ((bytedata >> 8) & 0x000000FF);
                if (((cdata)) == 0x01) {
                    cdata = (unsigned char) ((bytedata) & 0x000000FF);
                    if (((cdata)) == 0) {
                        tok = TOKEN_START_SVC_FRAME;
                    }
                    else if (((cdata)) == 0xb2) {
                        tok = TOKEN_START_SVC_SLICE;
                    }
                    else if (((cdata)) == 0xb0) {
                        tok = TOKEN_END_SVC_FRAME;
                    }
                }
            }
        }

        if (tok != TOKEN_NONE) {

            if (srd->parseMode == LOOK_FOR_SOF) {

                switch (tok) {

                case TOKEN_START_SVC_FRAME:
                    srd->isRightFrame = 1;
                    srd->hbuff_wroffset = 0;
                    srd->parseMode = LOOK_FOR_SOS;
                    if (bytecount < 4) {
                        CopyFirst4bytesData(srd, bytedata, len);
                    }

                    readPtr = pData + len;
                    //printf("Found a SOF @%d\n", len);
                    readLen = 0;

                    // Next slice would be a frame header
                    srd->isFirstSlice = TRUE;
                    break;

                case TOKEN_START_SVC_SLICE:

                    // printf("Found an unexpected SOS (still looking for SOF) @%d\n", len);
                    if (srd->inSlices > 0) // skip the first partial frame
                        srd->parse_errors_sof++;
                    srd->hbuff_wroffset = 0;
                    readPtr = pData + len;
                    readLen = 0;
                    break;
                }
            }

            else { // srd->parseMode == LOOK_FOR_SOS

                switch (tok) {

                case TOKEN_START_SVC_FRAME:
                case TOKEN_END_SVC_FRAME:
                    if (bytecount < 4) {
                        srd->hbuff_wroffset -= 4 - readLen;
                        readLen = 0;
                    }
                    else {
                        memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr,
                                readLen);
                        srd->hbuff_wroffset += readLen;
                        readPtr = pData + len;
                        readLen = 0;
                    }

                    if (srd->isFirstSlice == TRUE) {
                        //printf("Found next Frame Header: No slices in previous frame!\n");
                        updateFrameHeader(srd, srd->hbuff_wroffset);
                        flipFrameBuffers(srd);
                        srd->hbuff_wroffset = 0;
                        break;
                    }
                    sliceNo = GETSVCSLICENO(&srd->hbuff[0]);
                    bRefreshSlice = ISREFRESHSLICE(&srd->hbuff[0]);
                    if (sliceNo > srd->prevSliceNo) {
                        if (updateSlice(srd, sliceNo, srd->hbuff_wroffset,
                                bRefreshSlice) != -1)
                            srd->prevSliceNo = sliceNo;
                        srd->hbuff_wroffset = 0;
                        if (bytecount < 4) {
                            CopyFirst4bytesData(srd, bytedata, len);
                            readPtr = pData;
                        }
                    }
                    else {
                        srd->parse_errors_ooo_slice++;
                        //    printf("Out of order slice 1: %d <= %d\n", sliceNo, srd->prevSliceNo);
                    }

                    memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr,
                                                    4 - srd->hbuff_wroffset);
                    updateEndOfFrame(srd, 4);
                    srd->hbuff_wroffset = 0;
                    srd->parseMode = LOOK_FOR_SOF;
                    // Current frame is done
                    flipFrameBuffers(srd);

                  //  srd->hbuff_wroffset = 0;

                    //  TODO: mark left/right frame

                    // Next slice would be a frame header
                    srd->isFirstSlice = TRUE;

                    break;

                case TOKEN_START_SVC_SLICE:

                    if (bytecount < 4) {
                        srd->hbuff_wroffset -= 4 - readLen;
                        readLen = 0;
                    }
                    else {
                        memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr,
                                readLen);
                        srd->hbuff_wroffset += readLen;
                        readPtr = pData + len;
                        readLen = 0;
                    }

                    if (srd->isFirstSlice == TRUE) {
                        // Data collected so far is a frame header
                        //printf("Found a Frame Header\n");
                        updateFrameHeader(srd, srd->hbuff_wroffset);
                        srd->isFirstSlice = FALSE;
                        srd->hbuff_wroffset = 0;
                        if (bytecount < 4) {
                            CopyFirst4bytesData(srd, bytedata, len);
                            readPtr = pData;
                        }
                        break;
                    }
                    else {
                        // Data collected so far is a regular slice
                        // Found a frame header
                        sliceNo = GETSVCSLICENO(&srd->hbuff[0]);
                        bRefreshSlice = ISREFRESHSLICE(&srd->hbuff[0]);
                        //printf("Found a SOS: %d\n", sliceNo);
                        srd->isFirstSlice = FALSE;
                    }

                    if (sliceNo > srd->prevSliceNo) {
                        if (updateSlice(srd, sliceNo, srd->hbuff_wroffset,
                                bRefreshSlice) != -1)
                            srd->prevSliceNo = sliceNo;

                        srd->hbuff_wroffset = 0;
                        if (bytecount < 4) {
                            CopyFirst4bytesData(srd, bytedata, len);
                            readPtr = pData;
                        }
                        srd->parseMode = LOOK_FOR_SOS;
                    }
                    else {
                        // Missed EOF and next SOF so start over
                        //printf("Out of order slice 2: %d <= %d\n", sliceNo, srd->prevSliceNo);

                        flipFrameBuffers(srd);

                        srd->parse_errors_ooo_slice++;

                        srd->hbuff_wroffset = 0;

                        srd->parseMode = LOOK_FOR_SOF;
                    }

                    break;

                }

            }
            tok = TOKEN_NONE; // reset for next round -- optimization
        }

        buff = pData + bytecount;
        bytedata = (bytedata << 8) | (*(buff));
        bytecount += 1;
        len = (bytecount == 4) ? 0 : len + 1;
        readLen = (bytecount == 4) ? 0 : readLen + 1;

    }

    /* Copy the remaining bytes into our hbuff */
    memcpy(&srd->hbuff[srd->hbuff_wroffset], readPtr, readLen + 4);
    srd->hbuff_wroffset += readLen + 4;
    srd->last4bytes = (unsigned int) (pData[len] << 24 | pData[len + 1] << 16
            | pData[len + 2] << 8 | pData[len + 3]);

    return 0;
}

void setStereoPolicy(sliceRedData_t *srd, int policy) {

    srd->stereo_policy = policy;
    setStereoMode(srd);
}


void SRDSetCodecVersion(sliceRedData_t *srd, int codecVersion) {
    if (codecVersion == CODEC_VERSION_SVC)
        srd->isSVC = 1;
    else
        srd->isSVC = 0;
}

void SRDSetSVCProcessingMode(sliceRedData_t *srd, int svcProcessingMode)
{
    switch (svcProcessingMode) {
    case SRD_SVC_NULL_MODE:
    case SRD_SVC_STRIP_REM_LAYER:
    case SRD_SVC_STRIP_QUAD_LAYER:
        srd->svc_processing_mode = svcProcessingMode;
        break;
    default:
        break;
    }
}

void setStereoVideo(sliceRedData_t *srd, int is_stereo) {
    if (is_stereo) {
        srd->isStereo = TRUE;
    }
    else {
        srd->isStereo = FALSE;
    }
    setStereoMode(srd);
}

void setCompMode(sliceRedData_t *srd, int compMode) {
    if (compMode) {
        srd->is444 = TRUE;
        srd->bInternalRefresh = 0;
    }
    else {
        srd->is444 = FALSE;
    }
}

void setMotionCompMode(sliceRedData_t *srd, int motionCompMode) {
    if (motionCompMode) {
        srd->isMotion444 = TRUE;
        srd->bInternalRefresh = 0;
    }
    else {
        srd->isMotion444 = FALSE;
    }
}

void SRDSetNumberOfTracks(sliceRedData_t *srd, int numberOfTracks) {
    srd->track_count = numberOfTracks;
    if (srd->track_count > MAX_SRD_TRACKS)
        srd->track_count = MAX_SRD_TRACKS;
    if (srd->track_count < 1)
        srd->track_count = 1;

    // No support for multi-track in slice_drain mode yet.
    if (srd->slice_drain)
        srd->track_count = 1;
}

void SRDEnableInternalRefresh(sliceRedData_t *srd) {
    // Internal Refresh cannot be accomplished for static or dyanamic 444 case
    if (srd->is444 == TRUE || srd->isMotion444 == TRUE)
        return;

    srd->bInternalRefresh = 1;
}

const char * StereoDetect(sliceRedData_t *srd) {
    if (srd->isStereo)
        return "Stereo";
    if (srd->mono_frameindex == LEFTFRAME)
        return "MonoLeft";
    return "MonoRight";
}

int initSRD(sliceRedData_t *srd)
{
    int trackId;

    /* First set all parameters to zero */
    memset(srd, 0, sizeof(sliceRedData_t));

    /* Set all non zero parameters */
    srd->current_frameindex  = UNKNOWNFRAME;
    srd->isRightFrame = FALSE;
    srd->isStereo = FALSE;
    srd->stereo_policy = SP_SOURCE;
    srd->stereo_mode = SM_LEFT_ONLY;
    srd->is444 = FALSE;
    srd->isMotion444 = FALSE;
    srd->parseMode = LOOK_FOR_SOF;
    srd->isSliceZero = FALSE;
    srd->isFirstSlice = FALSE;
    srd->prevSliceNo = -1;
    for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
        srd->m_rdFrameFlushed[trackId][LEFTFRAME] = TRUE;
        srd->m_rdFrameFlushed[trackId][RIGHTFRAME] = TRUE;
        srd->currSliceNo[trackId] = -1;
        srd->fill_svc_header[trackId] = 0;
        srd->nRefreshSlicePointer[trackId] = 0;
    }
    srd->hoSliceIdx[LEFTFRAME] = -1;
    srd->hoSliceIdx[RIGHTFRAME] = -1;
    srd->wrSliceIdx = -1;
    srd->slice_drain = 0;
    srd->track_count = 1;
    srd->bInternalRefresh = 0;
    srd->nRefreshSliceRate = 5;
    srd->isSVC = 0;
    srd->svc_processing_mode = SRD_SVC_NULL_MODE;
    srd->last4bytes = 0xFFFFFFFF;
    srd->isInputFrameStatic = 0;

    srd->hslicebuffer = getSliceBuffer(srd);
    if (srd->hslicebuffer == NULL) {
        return -1;
    }
    srd->hbuff = srd->hslicebuffer->sliceData;

    srd->m_newFrameCallback = NULL;
    srd->m_newFrameCallbackObject = NULL;

    return 0;
}

int destroySRD(sliceRedData_t *srd)
{
    int count;
    int trackId;

    releaseSliceBuffer(srd, srd->m_wrFrame[LEFTFRAME].headerbuffer);
    releaseSliceBuffer(srd, srd->m_wrFrame[RIGHTFRAME].headerbuffer);
    for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
        releaseSliceBuffer(srd, srd->m_rdFrame[trackId][LEFTFRAME].headerbuffer);
        releaseSliceBuffer(srd, srd->m_rdFrame[trackId][RIGHTFRAME].headerbuffer);
    }
    for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
        releaseSliceBuffer(srd, srd->m_hoFrame[trackId][LEFTFRAME].headerbuffer);
        releaseSliceBuffer(srd, srd->m_hoFrame[trackId][RIGHTFRAME].headerbuffer);
    }

    if (srd->isSVC) {
      releaseSliceBuffer(srd, srd->m_wrFrame[LEFTFRAME].footerbuffer);
      releaseSliceBuffer(srd, srd->m_wrFrame[RIGHTFRAME].footerbuffer);
      for(trackId = 0 ;trackId < MAX_SRD_TRACKS; trackId++) {
          releaseSliceBuffer(srd, srd->m_rdFrame[trackId][LEFTFRAME].footerbuffer);
          releaseSliceBuffer(srd, srd->m_rdFrame[trackId][RIGHTFRAME].footerbuffer);
      }
      for(trackId = 0 ; trackId < MAX_SRD_TRACKS; trackId++) {
          releaseSliceBuffer(srd, srd->m_hoFrame[trackId][LEFTFRAME].footerbuffer);
          releaseSliceBuffer(srd, srd->m_hoFrame[trackId][RIGHTFRAME].footerbuffer);
      }
    }

    for (count = 0; count < MAX_SLICE_NO; count++) {
        releaseSliceBuffer(srd, srd->m_wrFrame[LEFTFRAME].slicebuffer[count]);
        releaseSliceBuffer(srd, srd->m_wrFrame[RIGHTFRAME].slicebuffer[count]);
        for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][LEFTFRAME].slicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][RIGHTFRAME].slicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_hoFrame[trackId][LEFTFRAME].slicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_hoFrame[trackId][RIGHTFRAME].slicebuffer[count]);
        }

        releaseSliceBuffer(srd, srd->m_wrFrame[LEFTFRAME].altSlicebuffer[count]);
        releaseSliceBuffer(srd, srd->m_wrFrame[RIGHTFRAME].altSlicebuffer[count]);
        for (trackId = 0; trackId < MAX_SRD_TRACKS; trackId++) {
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][LEFTFRAME].altSlicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_rdFrame[trackId][RIGHTFRAME].altSlicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_hoFrame[trackId][LEFTFRAME].altSlicebuffer[count]);
            releaseSliceBuffer(srd, srd->m_hoFrame[trackId][RIGHTFRAME].altSlicebuffer[count]);
        }
    }

    releaseSliceBuffer(srd, srd->hslicebuffer);

    releaseSliceBuffer(srd, srd->debug_slicebuffer);

    releaseSliceBuffer(srd, srd->marker_slicebuffer);


    // Free the buffer pool
    count  = 0;
    while (srd->buffer_pool_head != NULL) {
        struct buffer_pool_list * last_head = srd->buffer_pool_head;
        srd->buffer_pool_head = srd->buffer_pool_head->m_next;
        if (last_head != NULL) {
            slice_t * slicebuffer = (slice_t *) last_head;
            free(slicebuffer->sliceData);
            free(slicebuffer);
            count++;
        }
    }

    V2DASSERT(count == srd->buffer_pool_count);
    return 0;
}

void SRDSetNewFrameCallback(sliceRedData_t *srd,
                        void (*newFrameCallback)(void *object),
                        void *object)
{
    srd->m_newFrameCallback = newFrameCallback;
    srd->m_newFrameCallbackObject = object;
}

static void createDebugSlice(sliceRedData_t *srd)
{
    int mquant = 6;
    int dcquant = 6;

    int hres, vres;
    unsigned char * inbuff;
    char * outbuff;
    int inlen, outlen;
    int i,j,k,l;
    unsigned char *cp;

    if (srd->sliceSize <= 0)
        return;

    hres = srd->sliceSize * 8;
    vres = 8;

    inlen = hres * vres * 4;
    inbuff = (unsigned char *) malloc(inlen);
    if (inbuff == NULL)
        return;
    outbuff = (char *) malloc(inlen);
    if (outbuff == NULL) {
        free(inbuff);
        return;
    }
    cp = inbuff;
    for (i = 0; i < vres; i++) {
        k = 0;
	for (j = 0; j < hres; j++) {
            // alternating pixels of black and white
            l = 1 << 3;
            if (j%l == 0)
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
            cp++;
        }
    }

    initFrameParams(hres, vres, mquant, dcquant, srd->sliceSize);
    outlen = encodeOneSlice(inbuff, outbuff);
    if (outlen > 0) {
        releaseSliceBuffer(srd, srd->debug_slicebuffer);
        srd->debug_slicebuffer = getSliceBuffer(srd);
        fillSliceBuffer(srd->debug_slicebuffer,
                        (unsigned char *)outbuff, outlen);
    }
    free(inbuff);
    free(outbuff);
    return;
}

static slice_t *getDebugSlice(sliceRedData_t *srd,
    int sliceNumber, int frameindex)

{
    unsigned char *buf = NULL;
    slice_t *slicebuffer;

    if (srd->debug_slicebuffer == NULL)
        createDebugSlice(srd);

    if (srd->debug_slicebuffer == NULL) {
        return NULL;
    }
    V2DASSERT(srd->debug_slicebuffer->sizeofSlice > 0);

    slicebuffer = getSliceBuffer(srd);
    if (slicebuffer == NULL) {
        return NULL;
    }
    copySliceBuffer(slicebuffer, srd->debug_slicebuffer);
    slicebuffer->sliceNumber = sliceNumber;

    buf = slicebuffer->sliceData;

    buf[3] = (unsigned char) (sliceNumber >> 3);
    buf[4] &= 0x1F;

    buf[4] |= (unsigned char) ((sliceNumber & 0x7) << 5);

    if (frameindex == LEFTFRAME)
        buf[4] &= ~0x10;
    else
        buf[4] |= 0x10;

    return slicebuffer;

}

void enableSliceDebug(sliceRedData_t *srd) {

    int i;
    srd->debug_slices = 1;
    for(i = 1; i < MAX_SLICE_NO; i++) {
        srd->debug_slice_sent[LEFTFRAME][i] = 0;
        srd->debug_slice_sent[RIGHTFRAME][i] = 0;
    }
}

void disableSliceDebug(sliceRedData_t *srd) {
    srd->debug_slices = 0;
    if (!srd->isSVC)
        requestFullFrame(srd);
}

static void createMarkerSlice(sliceRedData_t *srd)
{
    int mquant = 6;
    int dcquant = 6;

    int hres, vres;
    unsigned char * inbuff;
    char * outbuff;
    int inlen, outlen;
    int i,j,k,l;
    unsigned char *cp;

    if (srd->sliceSize <= 0)
        return;

    hres = srd->sliceSize * 8;
    vres = 8;

    inlen = hres * vres * 4;
    inbuff = (unsigned char *) malloc(inlen);
    if (inbuff == NULL)
        return;
    outbuff = (char *) malloc(inlen);
    if (outbuff == NULL) {
        free(inbuff);
        return;
    }
    cp = inbuff;
    for (i = 0; i < vres; i++) {
        k = 0;
	for (j = 0; j < hres; j++) {
            // alternating pixels of black and grey
            l = 1 << 3;
            if (j%l == 0)
                k = ~k;
            if (k) {
                *cp++ = 128;
                *cp++ = 128;
                *cp++ = 128;
            }
            else {
                *cp++ = 0;
                *cp++ = 0;
                *cp++ = 0;
            }
            cp++;
        }
    }

    initFrameParams(hres, vres, mquant, dcquant, srd->sliceSize);
    outlen = encodeOneSlice(inbuff, outbuff);
    if (outlen > 0) {
        releaseSliceBuffer(srd, srd->marker_slicebuffer);
        srd->marker_slicebuffer = getSliceBuffer(srd);
        fillSliceBuffer(srd->marker_slicebuffer,
                        (unsigned char *)outbuff, outlen);
    }
    free(inbuff);
    free(outbuff);
    return;
}

static void fillMarkerSlice(sliceRedData_t *srd,
    int sliceNumber, int frameindex)

{
    unsigned char * buf =
        srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[sliceNumber]->sliceData;
    unsigned int sliceSize =
        srd->m_wrFrame[frameindex].slicebuffer[sliceNumber]->sizeofSlice;

    if (srd->marker_slicebuffer == NULL)
        createMarkerSlice(srd);

    if (srd->marker_slicebuffer == NULL) {
        return;
    }
    V2DASSERT(srd->marker_slicebuffer->sizeofSlice > 0);
    // Create a marker slice of the exact same size as the original
    // slice as far as possible so that we do not perturb the
    // generated traffic bandwidth
    if (srd->marker_slicebuffer->sizeofSlice <= sliceSize) {
        // reuse existing slice
        memcpy(buf, srd->marker_slicebuffer->sliceData,
               srd->marker_slicebuffer->sizeofSlice);
        memset(buf + srd->marker_slicebuffer->sizeofSlice, 0x00,
               sliceSize - srd->marker_slicebuffer->sizeofSlice);

    }
    else {
        // slice too big, create a new one

        slice_t *slicebuffer = getSliceBuffer(srd);
        if (slicebuffer != NULL) {
            copySliceBuffer(slicebuffer, srd->marker_slicebuffer);
            releaseSliceBuffer(srd, srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[sliceNumber]);
            srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[sliceNumber] = slicebuffer;
            slicebuffer->sliceNumber = sliceNumber;
            buf = slicebuffer->sliceData;
        }
    }
    if (buf != NULL) {
        buf[3] = (unsigned char) (sliceNumber >> 3);
        buf[4] &= 0x1F;
        buf[4] |= (unsigned char) ((sliceNumber & 0x7) << 5);

        if (frameindex == LEFTFRAME)
            buf[4] &= ~0x10;
        else
            buf[4] |= 0x10;
    }
}

void enableSliceMarking(sliceRedData_t *srd) {
    srd->mark_slices = 1;
}

void disableSliceMarking(sliceRedData_t *srd) {
    srd->mark_slices = 0;
}


void requestFullFrame(sliceRedData_t *srd) {

    int i;
    int frameindex;
    int count;
    int trackId = DEFAULT_TRACK;

    for (frameindex = 0; frameindex < 2; frameindex++) {

        if (srd->m_rdFrameFlushed[DEFAULT_TRACK][frameindex] == TRUE) {
            srd->m_rdFrameFlushed[DEFAULT_TRACK][frameindex] = FALSE;
        }
        // For SVC mode, mark the header as not flushed
        if (srd->isSVC && SLICE_PRESENT(srd->m_rdFrame[DEFAULT_TRACK][frameindex].headerbuffer)) {
            CLEAR_SLICE_FLUSHED(srd->m_rdFrame[DEFAULT_TRACK][frameindex].headerbuffer);
            count++;
        }

        // mark all slices in rdFrame as not flushed
        count = 0;
        for (i = 0; i < MAX_SLICE_NO; i++) {
            if (srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[i] != NULL) {
                CLEAR_SLICE_FLUSHED(srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[i]);
                count++;
            }
        }
        LOG_SYS(
			V2DLIB_VERBOSE,
			"V2DSRD: RequestFullFrame touched %d slices for %d, frame=%ld\n",
			count,frameindex, srd->outFrameCount);

        // Also mark the first slice in wrFrame as not flushed
        // In slice drain mode, this will ensure that the next frame
        // is picked up from the hoFrame
        if (SLICE_PRESENT(srd->m_wrFrame[frameindex].slicebuffer[0])) {
            CLEAR_SLICE_FLUSHED(srd->m_wrFrame[frameindex].slicebuffer[0]);
        }
        if (srd->isSVC && SLICE_PRESENT(srd->m_wrFrame[frameindex].headerbuffer)) {
            CLEAR_SLICE_FLUSHED(srd->m_wrFrame[frameindex].headerbuffer);
        }
    }
}

// In debug mode send special debug slice for slices not present
// in the current frame. Remember that we sent one so that we can
// save bandwidth by sending them only as needed
static void fillDebugSlices(sliceRedData_t *srd, int frameindex,
                            int startindex, int endindex)
{
    int idx;
    if (srd->isSVC)
        return; // debug slice not (yet) supported for SVC mode
    if (startindex == 0)
        startindex = 1;
    for(idx = startindex; idx < endindex; idx++) { // not for slice zero

        if (SLICE_ABSENT(srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[idx])) {
            if (srd->debug_slice_sent[frameindex][idx] == 0) {
                srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[idx] =
                    getDebugSlice(srd, idx, frameindex);
                if (SLICE_PRESENT(srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[idx])) {
                    srd->hoBytes[DEFAULT_TRACK][frameindex] +=
                        srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[idx]->sizeofSlice;
                    srd->debug_slice_sent[frameindex][idx] = 1;
                }
            }
        }
        else {
            srd->debug_slice_sent[frameindex][idx] = 0;
        }
    }
}

static inline void fill_hoHeaderSlice(sliceRedData_t *srd, frame_t sourceFrame[],
        int frameindex, int trackId)
{
    SET_SLICE_FLUSHED(sourceFrame[frameindex].headerbuffer);
    srd->m_hoFrame[trackId][frameindex].headerbuffer =
            sourceFrame[frameindex].headerbuffer;
    sourceFrame[frameindex].headerbuffer->refcount++;
    srd->hoBytes[trackId][frameindex] +=
            sourceFrame[frameindex].headerbuffer->sizeofSlice;
}

static inline void fill_hoEndOfFrameSlice(sliceRedData_t *srd,
        frame_t sourceFrame[], int frameindex, int trackId)
{
    srd->m_hoFrame[trackId][frameindex].footerbuffer
            = sourceFrame[frameindex].footerbuffer;
    sourceFrame[frameindex].footerbuffer->refcount++;
    srd->hoBytes[trackId][frameindex]
            += sourceFrame[frameindex].footerbuffer->sizeofSlice;
}

static inline void fill_hoSlice(sliceRedData_t *srd, frame_t sourceFrame[],
        int frameindex, int sliceindex, int trackId)
{
    if (srd->isSVC && srd->svc_processing_mode != SRD_SVC_NULL_MODE) {
        SVCSliceHeader sliceHeader;
        int strippedLength;
        int strippedLayer = 0;
        SVCSliceHeader strippedSliceHeader;

        SET_SLICE_FLUSHED(sourceFrame[frameindex].slicebuffer[sliceindex]);
        srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex] = getSliceBuffer(srd);
        ParseSVCSliceLayers(sourceFrame[frameindex].slicebuffer[sliceindex]->sliceData,
                sourceFrame[frameindex].slicebuffer[sliceindex]->sizeofSlice, &sliceHeader);
        strippedLayer = srd->svc_processing_mode == SRD_SVC_STRIP_QUAD_LAYER ?
            SVC_LAYERTYPE_MIDAC4x4LAYER : SVC_LAYERTYPE_REMAC4x4LAYER;
        strippedLength = StripSVCLayer(sourceFrame[frameindex].slicebuffer[sliceindex]->sliceData,
                sourceFrame[frameindex].slicebuffer[sliceindex]->sizeofSlice, &sliceHeader,
                srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex]->sliceData, strippedLayer);

        ParseSVCSliceLayers(srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex]->sliceData,
                strippedLength, &strippedSliceHeader);

        srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex]->sizeofSlice = strippedLength;
        srd->hoBytes[trackId][frameindex] += strippedLength;
        srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex]->sliceNumber =
                sourceFrame[frameindex].slicebuffer[sliceindex]->sliceNumber;
        return;
    }

    SET_SLICE_FLUSHED(sourceFrame[frameindex].slicebuffer[sliceindex]);
    CLEAR_DIRTY_SLICE(sourceFrame[frameindex].slicebuffer[sliceindex]);
    srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex] =
            sourceFrame[frameindex].slicebuffer[sliceindex];
    sourceFrame[frameindex].slicebuffer[sliceindex]->refcount++;
    srd->hoBytes[trackId][frameindex] +=
            sourceFrame[frameindex].slicebuffer[sliceindex]->sizeofSlice;
}

static inline void fill_hoSliceFromAltBuffer(sliceRedData_t *srd, frame_t sourceFrame[],
        int frameindex, int sliceindex, int trackId)
{
    SET_SLICE_FLUSHED(sourceFrame[frameindex].altSlicebuffer[sliceindex]);
    srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex] =
            sourceFrame[frameindex].altSlicebuffer[sliceindex];
    sourceFrame[frameindex].altSlicebuffer[sliceindex]->refcount++;
    srd->hoBytes[trackId][frameindex] +=
            sourceFrame[frameindex].altSlicebuffer[sliceindex]->sizeofSlice;
}

static inline void freeSliceBufferData(sliceRedData_t *srd, slice_t **pSliceBuffer)
{
    if (SLICE_PRESENT(*pSliceBuffer)) {
        releaseSliceBuffer(srd, *pSliceBuffer);
        *pSliceBuffer = NULL;
    }
}

static inline int isSliceWithinRefreshWindow(sliceRedData_t *srd, int sliceNumber, int trackId)
{
    int nMappedSliceNo =  sliceNumber;
    if (srd->MaxSliceNo <= 0)
        return 1;
    if (nMappedSliceNo < srd->nRefreshSlicePointer[trackId])
        nMappedSliceNo += srd->MaxSliceNo;
    if ((nMappedSliceNo >= srd->nRefreshSlicePointer[trackId]) &&
            (nMappedSliceNo < srd->nRefreshSlicePointer[trackId] + srd->nRefreshSliceRate)) {
       return 1;
    }
    return 0;
}
// Handle the slice-drain mode
// This tries to read ahead and look in the wrFrame to see if we
// can pick up slices to send on the wire
static int prefill_hoFrame(sliceRedData_t *srd, int frameindex, int prefill) {
    int i;
    int slices_collected = 0;
    int lsp;

    V2DASSERT(prefill == FILL_SD_FIRST || prefill == FILL_SD_PROGRESS || prefill == FILL_SD_LAST);
    V2DASSERT(!(prefill == FILL_SD_FIRST && srd->hoSliceIdx[frameindex] != -1));
    V2DASSERT(!(prefill == FILL_SD_PROGRESS && srd->hoSliceIdx[frameindex] == -1));
    V2DASSERT(!(prefill == FILL_SD_LAST && srd->hoSliceIdx[frameindex] == -1));
    V2DASSERT(!(srd->hoSliceIdx[frameindex] > 0 && srd->wrSliceIdx == -1));


    // On first call, check if we have enough slice bytes available to fill up
    // an output packet of 1024 bytes
    if (prefill == FILL_SD_FIRST) {
        int bytesAvailable = 0;
        if (srd->isSVC) {
            if (SLICE_PRESENT(srd->m_wrFrame[frameindex].headerbuffer)) {
                bytesAvailable +=
                        srd->m_wrFrame[frameindex].headerbuffer->sizeofSlice;
                if (SLICE_PRESENT(srd->m_wrFrame[frameindex].footerbuffer))
                    bytesAvailable
                            += srd->m_wrFrame[frameindex].footerbuffer->sizeofSlice;
            }
            else {
                return -1;
            }
        }
        for (i = 0; i < srd->wrSliceIdx + 1; i++) {
            if (SLICE_PRESENT(srd->m_wrFrame[frameindex].slicebuffer[i])) {
                bytesAvailable +=
                        srd->m_wrFrame[frameindex].slicebuffer[i]->sizeofSlice;
            }
            if (bytesAvailable >= SRD_VIDEO_PACKET_SIZE + 1) {
                break;
            }
        }
        if (bytesAvailable < SRD_VIDEO_PACKET_SIZE + 1) {
            return -1;
        }
    }


    // On first call, delete all slices in hoFrame
    if (srd->hoSliceIdx[frameindex] == -1) { // same as prefill == FILL_SD_FIRST
        if (srd->isSVC) {
            freeSliceBufferData(srd, &srd->m_hoFrame[DEFAULT_TRACK][frameindex].headerbuffer);
        }
        for (i = 0; i < MAX_SLICE_NO; i++) {
            freeSliceBufferData(srd, &srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[i]);
        }
        srd->iframe_slice_sent_count[DEFAULT_TRACK] = 0; // reset IFrame detection count

        // Also update the timestamp in hoFrame directly from wrFrame
        srd->m_hoFrame[DEFAULT_TRACK][frameindex].timestamp = srd->m_wrFrame[frameindex].timestamp;
    }

    // This section executes for FIRST, PROGRESS, or LAST call.

    // For SVC case, on first call, copy the header slice into hoFrame
    if (srd->isSVC) {
        if (srd->hoSliceIdx[frameindex] == -1) { // same as prefill == FILL_SD_FIRST
            if (SLICE_PRESENT(srd->m_wrFrame[frameindex].headerbuffer)) {
                fill_hoHeaderSlice(srd, srd->m_wrFrame, frameindex, DEFAULT_TRACK);
            }
            else {
                V2DASSERT(0); // header buffer better be there
            }
        }
    }
    // Copy slices from wrFrame to hoFrame in the narrow range
    // from srd->hoSliceIdx to srd->wrSliceIdx
    slices_collected = 0;
    lsp = srd->hoSliceIdx[frameindex] + 1;
    for (i = lsp; i < srd->wrSliceIdx + 1; i++) {

        if (SLICE_PRESENT(srd->m_wrFrame[frameindex].slicebuffer[i])) {
            slices_collected++;

            // Copy slice from wrFrame to hoFrame
            fill_hoSlice(srd, srd->m_wrFrame, frameindex, i, DEFAULT_TRACK);
            srd->iframe_slice_sent_count[DEFAULT_TRACK]++; // track IFrame detection


        }
        else {
            // If we look for slice zero, it should should have been there
            // (for the standard codec)
            if (!srd->isSVC) {
                V2DASSERT(i != 0 );
            }
        }

    }
    if (srd->isSVC) { /* this check is for filling of End of frame slice in slice drain mode */
        if (SLICE_PRESENT(srd->m_wrFrame[frameindex].footerbuffer)) {
            fill_hoEndOfFrameSlice(srd, srd->m_wrFrame, frameindex, DEFAULT_TRACK);
        }
    }
    // Insert debug slices in this narrow range if needed
    if (srd->debug_slices) {
        fillDebugSlices(srd, frameindex, lsp, srd->wrSliceIdx + 1);
    }
    // Assert that if slice range is positive, we have to pick up
    // at least one slice
    V2DASSERT(srd->wrSliceIdx < lsp || slices_collected > 0);

    // Update hoSliceIdx with the last slice written
    srd->hoSliceIdx[frameindex] = srd->wrSliceIdx;

    // Things to do on last call. Called from flipFrameBuffers, so
    // we know no more slices expected in this frame
    if (prefill == FILL_SD_LAST) {
        if (srd->debug_slices) {
            fillDebugSlices(srd, frameindex,
                    srd->wrSliceIdx + 1, srd->MaxSliceNo);
        }
        // Reset hoSliceIdx to indicate we have reached the end
        srd->hoSliceIdx[frameindex] = -1;

        srd->outFrameCount++;
        srd->prefillFrameCount++;

        if ((srd->MaxSliceNo > 0) &&
                (srd->iframe_slice_sent_count[DEFAULT_TRACK] >= srd->MaxSliceNo)) {
            srd->OutputIFrameCount++;
            LOG_SYS(V2DLIB_INFO,
                    "V2DSRD: Sent an IFrame (slice-drain mode) %ld at %ld\n",
                    srd->OutputIFrameCount, srd->outFrameCount);
        }
    }

    // Slice-Drain mode handling done so return
    return slices_collected;
}

static int fill_hoFrame(sliceRedData_t *srd, int frameindex, int trackId) {
    int sliceindex;

    // Normal fill mode.
    // Copy new slices (non flushed) from rdFrame into hoFrame
    if (trackId == 0)
        srd->outFrameCount++;
    else
        srd->outFrameCountPerTrack[trackId]++;

    srd->iframe_slice_sent_count[trackId] = 0;

    if (srd->bInternalRefresh && srd->MaxSliceNo > 0) {
        srd->nRefreshSlicePointer[trackId] += srd->nRefreshSliceRate;
        srd->nRefreshSlicePointer[trackId] = srd->nRefreshSlicePointer[trackId] % srd->MaxSliceNo;
    }

    if (srd->isSVC) {
        // Copy header slice
        if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].headerbuffer)) {
            // Free old header data
            freeSliceBufferData(srd, &srd->m_hoFrame[trackId][frameindex].headerbuffer);
            // Copy new header data
            fill_hoHeaderSlice(srd, srd->m_rdFrame[trackId], frameindex, trackId);
        }
    }
    for (sliceindex = 0; sliceindex < MAX_SLICE_NO; sliceindex++)
    {
        // Free old data in m_hoFrame
        freeSliceBufferData(srd, &srd->m_hoFrame[trackId][frameindex].slicebuffer[sliceindex]);

        // Copy slice from rdFrame to hoFrame
        if (!srd->is444) {
            if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex]) &&
                    !IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex])) {
                int bSendSlice = 0;

                // Send refresh slices in round robin fashion if so configured
                if (!srd->bInternalRefresh) {
                    // Internal refresh not configured, send all slices
                    bSendSlice = 1;
                }
                else if (GET_DIRTY_SLICE(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex])) {
                    // Not a refreshed slice, i.e. dirty, so have to send
                    bSendSlice = 1;
                }
                else {
                    // Refresh slice, send only if within window
                    bSendSlice = isSliceWithinRefreshWindow(srd, sliceindex, trackId);
                }

                if (bSendSlice) {
                    fill_hoSlice(srd, srd->m_rdFrame[trackId], frameindex, sliceindex, trackId);
                    srd->iframe_slice_sent_count[trackId]++; // Track output IFrame detection
                }
            }
            else if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex]) &&
                    IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex])) {

                if (srd->bInternalRefresh) {
                    int bSendSlice = isSliceWithinRefreshWindow(srd, sliceindex, trackId);
                    if (bSendSlice) {
                        fill_hoSlice(srd, srd->m_rdFrame[trackId], frameindex, sliceindex, trackId);
                        srd->iframe_slice_sent_count[trackId]++; // Track output IFrame detection
                    }

                }
            }
        }
        else {
            // In 4:4:4 mode, send the normal slice if it exists, else
            // send the refresh slice carrying 4:4:4 color info
            if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex]) &&
                    !IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].slicebuffer[sliceindex])) {
                fill_hoSlice(srd, srd->m_rdFrame[trackId], frameindex, sliceindex, trackId);
                srd->iframe_slice_sent_count[trackId]++;
            }
            else if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].altSlicebuffer[sliceindex]) &&
                    !IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].altSlicebuffer[sliceindex])) {
                fill_hoSliceFromAltBuffer(srd, srd->m_rdFrame[trackId], frameindex, sliceindex, trackId);
                srd->iframe_slice_sent_count[trackId]++;
            }
        }
    }

    // Extract the timestamp from rdFrame into current hoFrame
    srd->m_hoFrame[trackId][frameindex].timestamp = srd->m_rdFrame[trackId][frameindex].timestamp;
    ddprintf("Output %u\n", srd->m_hoFrame[trackId][frameindex].timestamp);

    if (srd->isSVC) {
		/* As the End Of Frame is constant 4 bytes later on can get ride of this step
		 *   one time filling is ok  */
		if (SLICE_PRESENT(srd->m_rdFrame[trackId][frameindex].footerbuffer)) {
			// Free old header data
			freeSliceBufferData(srd, &srd->m_hoFrame[trackId][frameindex].footerbuffer);
			// Copy new header data
			fill_hoEndOfFrameSlice(srd, srd->m_rdFrame[trackId], frameindex, trackId);
		}
	}
    // Count output IFrames
    if ((srd->MaxSliceNo > 0) &&
            (srd->iframe_slice_sent_count[trackId] >= srd->MaxSliceNo)) {
        if (trackId == 0)
            srd->OutputIFrameCount++;
        else
            srd->OutputIFrameCountPerTrack[trackId]++;
        LOG_SYS(V2DLIB_INFO, "V2DSRD: Sent an IFrame %ld at %ld\n",
                srd->OutputIFrameCount, srd->outFrameCount);
    }


    V2DASSERT(srd->m_rdFrameFlushed[trackId][frameindex] == FALSE);
    srd->m_rdFrameFlushed[trackId][frameindex] = TRUE;

    // Send debug slices if needed
    if (srd->debug_slices) {
        fillDebugSlices(srd, frameindex, 1, srd->MaxSliceNo);
    }

    return 0;
}

static int is_frame_ready(sliceRedData_t *srd, int frameindex, int trackId)
{
    if (srd->isSVC) {
        if (SLICE_ABSENT(srd->m_rdFrame[trackId][frameindex].headerbuffer) ||
                IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].headerbuffer)) {
            return 0;
        }
    }
    else {
        if (SLICE_ABSENT(srd->m_rdFrame[trackId][frameindex].slicebuffer[0]) ||
                IS_SLICE_FLUSHED(srd->m_rdFrame[trackId][frameindex].slicebuffer[0])) {
            return 0;
        }
    }
    return 1;
}

static void get_first_slice(sliceRedData_t *srd, int frameindex, int trackId)
{
    if (srd->isSVC) {
        V2DASSERT(SLICE_PRESENT(srd->m_hoFrame[trackId][frameindex].headerbuffer));
        srd->currSlicePtr[trackId] =
                srd->m_hoFrame[trackId][frameindex].headerbuffer->sliceData;
        srd->fill_svc_header[trackId] = 1;
        srd->currSliceNo[trackId] = -1;
    }
    else {
        srd->currSliceNo[trackId] = getNextSlice(srd, -1, frameindex, trackId);
        if (srd->currSliceNo[trackId] < MAX_SLICE_NO) {
            srd->currSlicePtr[trackId] =
                    srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sliceData;
        }
        else {
            srd->currSlicePtr[trackId] = NULL;
        }
    }
    srd->current_frameindex = frameindex;
}

int get_hoFrameForTrack(sliceRedData_t *srd, int trackId)
{
    int frameindex;

    if (srd->stereo_mode == SM_STEREO) {
        // TODO: handle stereo case for SVC mode

        // In stereo mode, we wait for both right and left frame
        // to have some data and then take a snapshot of both
        // frames in left/right hoFrame. current_frameindex tracks
        // from which snapshot we are currently serving data

        if (srd->current_frameindex == RIGHTFRAME ||
            srd->current_frameindex == UNKNOWNFRAME) {
            if (!is_frame_ready(srd, LEFTFRAME, trackId) ||
                    !is_frame_ready(srd, RIGHTFRAME, trackId)) {
                return -1;
            }

            fill_hoFrame(srd, LEFTFRAME, trackId);
            fill_hoFrame(srd, RIGHTFRAME, trackId);

            get_first_slice(srd, LEFTFRAME, trackId);
        }
        else if (srd->current_frameindex == LEFTFRAME) {
            if (SLICE_ABSENT(srd->m_hoFrame[trackId][RIGHTFRAME].slicebuffer[0])) {
                // happens if stereo is detected on the fly
                return -1;
            }

            get_first_slice(srd, RIGHTFRAME, trackId);
        }
    }

    else {
        // Either we have mono video or have been forced to a
        // left only or right only policy with stereo video
        if (srd->stereo_mode == SM_LEFT_ONLY)
            frameindex = LEFTFRAME;
        else
            frameindex = RIGHTFRAME;

        // Do we have another frame ready?
        if (!is_frame_ready(srd, frameindex, trackId))
        {
            // We don't have a new frame ready in our 'reduction'
            // buffer. In slice drain mode, we try to tap into the
            // still getting parsed source
            if (srd->slice_drain == 1 && !srd->isStereo) {
                int sliceCount = prefill_hoFrame(srd, frameindex, FILL_SD_FIRST);
                if (sliceCount > 0) {
                    V2DASSERT(srd->hoSliceIdx[frameindex] != -1);
                    if (!srd->isSVC) {
                        V2DASSERT(SLICE_PRESENT(srd->m_hoFrame[trackId][frameindex].slicebuffer[0]));
                    }
                    get_first_slice(srd, frameindex, trackId);
                    return 0;
                }
                V2DASSERT(srd->hoSliceIdx[frameindex] == -1);
            }
            // We had nothing in rdFrame and even in slice-drain mode we
            // did not find anything
            return -1;
        }

        // We have something waiting for us in rdFrame. Go grab it.
        fill_hoFrame(srd, frameindex, trackId);

        get_first_slice(srd, frameindex, trackId);

    }

    return 0;
}

int get_hoFrame(sliceRedData_t *srd) {
    return get_hoFrameForTrack(srd, 0);
}


// Get a packet from the current HO frame and fill it into the given
// outbuff
int getPacketsForTrack(sliceRedData_t *srd, unsigned char *outbuff,
        int outbufflen, int dopad, int trackId)
{
    int bytesWritten = 0;
    int bytesRemaining = 0;
    int outbuff_offset = 0; //location to write to outbuff
    int writelen;
    int frameindex;
    int nextSliceNumber;

    V2DASSERT(srd != NULL);
    V2DASSERT(outbuff != NULL);


    // If the current frame (hoFrame) has been emptied out
    // retrieve a new frame from the read buffer (rdFrame)
    if (srd->currSlicePtr[trackId] == NULL)
    {
        if (srd->currSliceNo[trackId] == -1) {
            if (get_hoFrameForTrack(srd, trackId) == -1)
                return -1;
        }
        else {
            frameindex = srd->current_frameindex;
            V2DASSERT(frameindex != UNKNOWNFRAME);
            nextSliceNumber = getNextSlice(srd, srd->currSliceNo[trackId], frameindex, trackId);
            if (nextSliceNumber == -1) {
                return -1;
            }

            LOG_SYS(V2DLIB_WARN, "V2DSRD: Should not get here usually\n");
            V2DASSERT(nextSliceNumber >= 0);
            V2DASSERT(nextSliceNumber != MAX_SLICE_NO);
            V2DASSERT(SLICE_PRESENT(srd->m_hoFrame[trackId][frameindex].slicebuffer[nextSliceNumber]));
            srd->currSliceNo[trackId] = nextSliceNumber;
            srd->currSlicePtr[trackId] =
                srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sliceData;
        }
    }
    else {

        // We have data sitting in our hoFrame. If we are in slice-drain
        // mode we wait till we can fill up a complete packet
        if (srd->hoSliceIdx[srd->current_frameindex] >= 0) {
            if (srd->hoBytes[trackId][srd->current_frameindex] < outbufflen + 1) {
                prefill_hoFrame(srd, srd->current_frameindex, FILL_SD_PROGRESS);
                return -1;
            }
        }
    }

    frameindex = srd->current_frameindex;
    V2DASSERT(frameindex != UNKNOWNFRAME);

    while (bytesWritten < outbufflen)
    {
        if (srd->fill_svc_header[trackId]) {
            V2DASSERT(srd->m_hoFrame[trackId][frameindex].headerbuffer->sizeofSlice > 0);
            bytesRemaining =
                    srd->m_hoFrame[trackId][frameindex].headerbuffer->sizeofSlice -
                    (srd->currSlicePtr[trackId] -
                            srd->m_hoFrame[trackId][frameindex].headerbuffer->sliceData);

        }
        else if (srd->currSliceNo[trackId]  < MAX_SLICE_NO) {
            V2DASSERT(srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sizeofSlice > 0);
            bytesRemaining =
                    srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sizeofSlice -
                    (srd->currSlicePtr[trackId] -
                            srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sliceData);
        }
        else {
            bytesRemaining
                    = srd->m_hoFrame[trackId][frameindex].footerbuffer->sizeofSlice
                            - (srd->currSlicePtr[trackId]
                                    - srd->m_hoFrame[trackId][frameindex].footerbuffer->sliceData);
        }


        if ((bytesRemaining + bytesWritten) > outbufflen)
        {
            // We can only write part of the current slice in the given buffer.
            // The buffer should get filled out completely and the while
            // loop should naturally break out after this
            writelen = outbufflen - bytesWritten;
            memcpy(&outbuff[outbuff_offset], srd->currSlicePtr[trackId], writelen);
            srd->currSlicePtr[trackId] += writelen;
            bytesWritten += writelen;
            outbuff_offset += writelen;
            if (trackId == 0)
                srd->outBytes += writelen;
            else
                srd->outBytesPerTrack[trackId] += writelen;
            srd->hoBytes[trackId][frameindex] -= writelen;
            V2DASSERT(bytesWritten == outbufflen);
            V2DASSERT(outbuff_offset == outbufflen);
        }
        else
        {
            // We have enough room to write out the current slice
            // (or frame header for svc case)
            writelen = bytesRemaining;
            memcpy(&outbuff[outbuff_offset], srd->currSlicePtr[trackId], writelen);
            bytesWritten += writelen;
            outbuff_offset += writelen;
            if (trackId == 0)
                srd->outBytes += writelen;
            else
                srd->outBytesPerTrack[trackId] += writelen;

            srd->hoBytes[trackId][frameindex] -= writelen;

            V2DASSERT(outbuff_offset <= outbufflen);

            // Sync up to the next 4 word boundary just in case the
            // size of last slice written was not an even multiple of four
          if (! srd->isSVC)
          {
            while ((outbuff_offset % 4) != 0) {
                outbuff[outbuff_offset] = 0x00;
                outbuff_offset++;
                bytesWritten++;
                if (trackId == 0)
                    srd->outBytes++;
                else
                    srd->outBytesPerTrack[trackId] += writelen;
                if (outbuff_offset >= outbufflen)
                    break;
            }
          }
            // Get next slice
          if (!srd->fill_svc_header[trackId] && srd->currSliceNo[trackId] < MAX_SLICE_NO) {
                if (trackId == 0)
                    srd->outSlices++;
                else
                    srd->outSlicesPerTrack[trackId]++;
            }
            srd->fill_svc_header[trackId] = 0;
            nextSliceNumber = getNextSlice(srd, srd->currSliceNo[trackId], frameindex, trackId);

           // printf("total out Bytes: %d\n",nextSliceNumber);
            if (srd->isSVC && nextSliceNumber == MAX_SLICE_NO) {
                srd->currSliceNo[trackId] = nextSliceNumber;
                srd->currSlicePtr[trackId]
                        = srd->m_hoFrame[trackId][frameindex].footerbuffer->sliceData;
            }
            else {
            if (nextSliceNumber >= MAX_SLICE_NO)
            {
                // No more slices in the current hoframe. Try
                // to get the next one. If it is not available, pad
                // the remaining bytes in the current packet with
                // zeros and break.
                if (srd->isSVC) {
                        writelen = outbufflen - outbuff_offset;
                        memset(&outbuff[outbuff_offset], 0x00, writelen);
                        outbuff_offset += writelen;
                        bytesWritten += writelen;
                        srd->outBytes += writelen;
                        srd->padBytes += writelen;

                    }
                if (!srd->isStereo) {
                    V2DASSERT(srd->hoBytes[trackId][frameindex] == 0);
                }

                if (get_hoFrameForTrack(srd, trackId) == -1) {

                    // Pad the remaining space with zeros
                    if (dopad) {
                        writelen = outbufflen - outbuff_offset;
                        memset(&outbuff[outbuff_offset], 0x00, writelen);
                        outbuff_offset += writelen;
                        bytesWritten += writelen;
                        if (trackId == 0) {
                            srd->outBytes += writelen;
                            srd->padBytes += writelen;
                        }
                        else {
                            srd->outBytesPerTrack[trackId] += writelen;
                            srd->padBytesPerTrack[trackId] += writelen;
                        }
                        //LOG_SYS("SRD padding %d due to frame underflow\n", writelen);
                    }

                    srd->currSliceNo[trackId] = -1;
                    srd->currSlicePtr[trackId] = NULL;

                    break; // out of while loop
                }
                else {
                    // We got the next hoframe. Continue with our while loop.
                    // currSlicePtr and currSliceNo already set by get_hoFrame
                    frameindex = srd->current_frameindex;
                }
            }
            else if (nextSliceNumber == -1) {
                // This should only happen in slice drain mode.
                // Next slice is not yet ready.
                // Pad the remaining space with zeros and break
                if (dopad) {
                    writelen = outbufflen - outbuff_offset;
                    memset(&outbuff[outbuff_offset], 0x00, writelen);
                    outbuff_offset += writelen;
                    bytesWritten += writelen;
                    if (trackId == 0) {
                        srd->outBytes += writelen;
                        srd->padBytes += writelen;
                    }
                    else {
                        srd->outBytesPerTrack[trackId] += writelen;
                        srd->padBytesPerTrack[trackId] += writelen;
                    }
                }
                srd->currSlicePtr[trackId] = NULL;

                break; // out of while loop
            }
            else {
                // We have a new slice, set the current pointers
                srd->currSliceNo[trackId] = nextSliceNumber;
                srd->currSlicePtr[trackId] =
                    srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sliceData;
            }
            }

        }

        V2DASSERT(srd->currSlicePtr[trackId] != NULL);

        // Our pointer into the current slice should be somewhere
        // within the current slice
        if (srd->fill_svc_header[trackId]) {
            V2DASSERT((srd->currSlicePtr[trackId] -
                       srd->m_hoFrame[trackId][frameindex].headerbuffer->sliceData) <=
                      (int)(srd->m_hoFrame[trackId][frameindex].headerbuffer->sizeofSlice));
        }
        else if(srd->currSliceNo[trackId] < MAX_SLICE_NO) {
            V2DASSERT((srd->currSlicePtr[trackId] -
                    srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sliceData) <=
                    (int)(srd->m_hoFrame[trackId][frameindex].slicebuffer[srd->currSliceNo[trackId]]->sizeofSlice));
        }
        else {
            V2DASSERT((srd->currSlicePtr[trackId] -
                            srd->m_hoFrame[trackId][frameindex].footerbuffer->sliceData) <=
                    (int)(srd->m_hoFrame[trackId][frameindex].footerbuffer->sizeofSlice));
        }
    }

    if (dopad) {
        if (trackId == 0)
            srd->outPktCount++;
        else
            srd->outPktCountPerTrack[trackId]++;
    }
    // If all is well, we should have the packet completely filled out
    // unless we are in non-pad mode
    V2DASSERT(!dopad || outbuff_offset == outbufflen);

    // Get ready for the next request
    if (trackId == 0) {
        srd->audioSeqNum++;
        srd->videoSeqNum++;
    }

    return outbuff_offset;
}

int getPackets(sliceRedData_t *srd, unsigned char *outbuff, int outbufflen, int dopad)
{
    return getPacketsForTrack(srd, outbuff, outbufflen, dopad, 0);
}

static int getNextSlice(sliceRedData_t *srd, int sliceNo, int frameindex, int trackId)
{
    int sNo;
    int lastindex = MAX_SLICE_NO;

    // In normal mode (non-SVC), the first slice Number is always Zero
    if (sliceNo == -1) {
        if (!srd->isSVC) {
            return 0;
        }
    }

    // Are we in slice read ahead phase?
    if (srd->hoSliceIdx[frameindex] != -1) {
        lastindex = srd->hoSliceIdx[frameindex] + 1;
        // Do we need to get more slices?
        if (sliceNo + 1 >= lastindex) {
            prefill_hoFrame(srd, frameindex, FILL_SD_PROGRESS);
            V2DASSERT(srd->hoSliceIdx[frameindex] != -1);
            // Did we get any more slices?
            lastindex = srd->hoSliceIdx[frameindex] + 1;
            if (sliceNo + 1 >= lastindex) {
                return -1;
            }
        }
    }

    for (sNo = sliceNo + 1; sNo < lastindex; sNo++)
    {
        if (SLICE_PRESENT(srd->m_hoFrame[trackId][frameindex].slicebuffer[sNo])) {
            V2DASSERT(sNo ==
                   srd->m_hoFrame[trackId][frameindex].slicebuffer[sNo]->sliceNumber);
            return sNo;
        }
    }

    V2DASSERT(srd->hoSliceIdx[frameindex] == -1);
    return sNo;

}

// Calling code should call this function first to get the HoFrame size,
// allocate a buffer with that size and then call copyHoFrameToBuffer.
int getHoFrameSize(sliceRedData_t *srd)
{
    int i;
    int frameindex;
    int frameSize = 0;

    frameindex = srd->current_frameindex;
    V2DASSERT(frameindex != UNKNOWNFRAME);

    if (srd->isSVC) {
        frameSize += srd->m_hoFrame[DEFAULT_TRACK][frameindex].headerbuffer->sizeofSlice;
    }

    for(i = 0; i < MAX_SLICE_NO; i++)
    {
		if (srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[i])
			frameSize += srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[i]->sizeofSlice;
    }

    if (srd->isSVC) {
		frameSize += srd->m_hoFrame[DEFAULT_TRACK][frameindex].footerbuffer->sizeofSlice;
	}

    return frameSize;
}

int getMinHoFrameSize(sliceRedData_t *srd)
{
    V2DASSERT(srd->current_frameindex != UNKNOWNFRAME);

    return srd->hoBytes[DEFAULT_TRACK][srd->current_frameindex] + 1024;
}

int copyHoFrameToBuffer(sliceRedData_t *srd,
                        unsigned char *outBuff,
                        const unsigned int outBuffLen)
{
    unsigned int bytesCopied = 0, currSliceSize = 0;
    int frameindex;

    frameindex = srd->current_frameindex;

    V2DASSERT(frameindex != UNKNOWNFRAME);
    V2DASSERT(outBuff);

    if (srd->isSVC) {
        // Copy Frame Header
        currSliceSize = srd->m_hoFrame[DEFAULT_TRACK][frameindex].headerbuffer->sizeofSlice;
        memcpy(outBuff,
                srd->m_hoFrame[DEFAULT_TRACK][frameindex].headerbuffer->sliceData,
                currSliceSize);

        srd->outBytes += currSliceSize;
        outBuff += currSliceSize;
        bytesCopied += currSliceSize;
        srd->hoBytes[DEFAULT_TRACK][frameindex] -= currSliceSize;

        // Get the first real slice.
        // In case there are none, we will get MAX_SLICE_NO)
        srd->currSliceNo[DEFAULT_TRACK] = getNextSlice(srd, -1, frameindex, DEFAULT_TRACK);
    }

    while (srd->currSliceNo[DEFAULT_TRACK] < MAX_SLICE_NO) {
        currSliceSize = srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[srd->currSliceNo[DEFAULT_TRACK]]->sizeofSlice;

        if ((bytesCopied + currSliceSize) > outBuffLen)
            break;

        memcpy(outBuff,
            srd->m_hoFrame[DEFAULT_TRACK][frameindex].slicebuffer[srd->currSliceNo[DEFAULT_TRACK]]->sliceData,
            currSliceSize);

        srd->outSlices++;
        srd->outBytes += currSliceSize;

        outBuff += currSliceSize;
        bytesCopied += currSliceSize;
        srd->hoBytes[DEFAULT_TRACK][frameindex] -= currSliceSize;

        srd->currSliceNo[DEFAULT_TRACK] = getNextSlice(srd, srd->currSliceNo[DEFAULT_TRACK], frameindex, DEFAULT_TRACK);
    }
    if (srd->isSVC) {
        currSliceSize += srd->m_hoFrame[DEFAULT_TRACK][frameindex].footerbuffer->sizeofSlice;
        memcpy(outBuff, srd->m_hoFrame[DEFAULT_TRACK][frameindex].footerbuffer->sliceData,
                currSliceSize);
        srd->outBytes += currSliceSize;
        outBuff += currSliceSize;
        bytesCopied += currSliceSize;
        srd->hoBytes[DEFAULT_TRACK][frameindex] -= currSliceSize;
    }


    V2DASSERT(srd->hoBytes[DEFAULT_TRACK][frameindex] == 0);

    return bytesCopied;
}

uint32_t SRDGetFrameTimeStamp(sliceRedData_t *srd) {
    int frameindex;

    frameindex = srd->current_frameindex;
    V2DASSERT(frameindex != UNKNOWNFRAME);

    return srd->m_hoFrame[DEFAULT_TRACK][frameindex].timestamp;
}

int getFullFrameSize(sliceRedData_t *srd)
{
    int i;
    int frameindex;
    int frameSize = 0;

    frameindex = 1;//srd->current_frameindex;
    V2DASSERT(frameindex != UNKNOWNFRAME);

    for (i = 0; i < MAX_SLICE_NO; i++)
    {
        if (srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[i])
        	frameSize += srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[i]->sizeofSlice;
    }
    return frameSize;
}

int getFullFrame(sliceRedData_t *srd,
        unsigned char *outBuff,
        const unsigned int outBuffLen)
{
    unsigned int bytesCopied = 0;
    unsigned int currSliceSize;
    int frameindex;
    int currSliceNo;

    frameindex = 1;//srd->current_frameindex;

    V2DASSERT(frameindex != UNKNOWNFRAME);
    V2DASSERT(outBuff);

    for (currSliceNo = 0; currSliceNo < MAX_SLICE_NO; currSliceNo++) {
        if (SLICE_ABSENT(srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[currSliceNo]))
            continue;
        currSliceSize = srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[currSliceNo]->sizeofSlice;
        if ((bytesCopied + currSliceSize) > outBuffLen)
            break;
        memcpy(outBuff,
                srd->m_rdFrame[DEFAULT_TRACK][frameindex].slicebuffer[currSliceNo]->sliceData,
                currSliceSize);
        outBuff += currSliceSize;
        bytesCopied += currSliceSize;
    }

    return bytesCopied;
}
