#ifndef __HH_SRD_H__
#define __HH_SRD_H__


// Hash defines/macros

#define SRD_VIDEO_START_R_FRAME_CODE            0x00000100
#define SRD_VIDEO_START_L_FRAME_CODE            0x000001A0
#define SRD_VIDEO_END_FRAME_CODE                0x000001B0
#define SRD_VIDEO_START_SLICE_CODE              0x0001B2
#define SRD_VIDEO_END_SLICE_CODE                0x0001B9

#define SRD_VIDEO_END_FRAME_CODE_LEN            4

#define SRD_FRAME_SEQ_NUM_LEN                   8
#define SRD_VIDEO_PACKET_SIZE                   1024
#define SIZE_OF_AV_PACKET                       1040
#define SRD_MAX_FRAME_WIDTH                     1920
#define SRD_MAX_FRAME_HEIGHT                    1440
#define START_SLICE_CODE                        434
#define END_SLICE_CODE                          441
#define MAX_SLICE_NO                            2048
#define MAX_SLICE_SIZE                          (1024*20)
#define NOMINAL_SLICE_SIZE                      (1024*4)
#define EOF_MARKER_LEN                          8

#define MAX_INPUT_BUFFERSIZE                    2048000


#define TOKEN_START_LEFT_FRAME                  500
#define TOKEN_START_RIGHT_FRAME                 501
#define TOKEN_END_FRAME                         502
#define TOKEN_START_SLICE                       503
#define TOKEN_START_SVC_FRAME                   504
#define TOKEN_START_SVC_SLICE                   505
#define TOKEN_END_SVC_FRAME                     506
#define TOKEN_NONE                              555

#define SP_SOURCE                               0
#define SP_MONO                                 1
#define SP_LEFT_ONLY                            2
#define SP_RIGHT_ONLY                           3

#define SRD_SVC_INVALID_MODE                    -1
#define SRD_SVC_NULL_MODE                       0
#define SRD_SVC_STRIP_REM_LAYER                 1
#define SRD_SVC_STRIP_QUAD_LAYER                2

#define MAX_SRD_TRACKS     4

// Structures

enum  parseMode_t {
    LOOK_FOR_SOF = 1,
    LOOK_FOR_SOS
};

typedef struct sliceData_t {
    unsigned int sizeofSlice;
    int sliceNumber;
    int refcount;
    unsigned long flags[MAX_SRD_TRACKS];
    unsigned char* sliceData;
} slice_t;

#define SLICEFLAG_FLUSHED              0x01
#define SLICEFLAG_REFRESHSLICE         0x02
#define SLICEFLAG_REALLOCED            0x04
#define SLICEFLAG_DIRTY                0x08

#define SET_SLICE_FLUSHED(slicebuffer) ((slicebuffer)->flags[trackId] |= SLICEFLAG_FLUSHED)
#define CLEAR_SLICE_FLUSHED(slicebuffer) ((slicebuffer)->flags[trackId] &= ~(SLICEFLAG_FLUSHED))
#define IS_SLICE_FLUSHED(slicebuffer) ((slicebuffer)->flags[trackId] & SLICEFLAG_FLUSHED)
#define SET_REFRESH_SLICE(slicebuffer) ((slicebuffer)->flags[0] |= SLICEFLAG_REFRESHSLICE)
#define IS_REFRESH_SLICE(slicebuffer) ((slicebuffer)->flags[0] & SLICEFLAG_REFRESHSLICE)
#define SET_REALLOCED(slicebuffer) ((slicebuffer)->flags[trackId] |= SLICEFLAG_REALLOCED)
#define GET_REALLOCED(slicebuffer) ((slicebuffer)->flags[trackId] & SLICEFLAG_REALLOCED)
#define SET_DIRTY_SLICE(slicebuffer) ((slicebuffer)->flags[trackId] |= SLICEFLAG_DIRTY)
#define CLEAR_DIRTY_SLICE(slicebuffer) ((slicebuffer)->flags[trackId] &= ~(SLICEFLAG_DIRTY))
#define GET_DIRTY_SLICE(slicebuffer) ((slicebuffer)->flags[trackId] & SLICEFLAG_DIRTY)

#define SLICE_PRESENT(slicebuffer) ((slicebuffer) != NULL)
#define SLICE_ABSENT(slicebuffer) ((slicebuffer) == NULL)


typedef struct frameData_t {
    slice_t *slicebuffer[MAX_SLICE_NO];
    slice_t *altSlicebuffer[MAX_SLICE_NO];
    slice_t *headerbuffer;
    slice_t *footerbuffer;
    uint32_t timestamp;
} frame_t;

struct buffer_pool_list {
    struct buffer_pool_list * m_next;
};

typedef struct sliceRedData {

    enum parseMode_t parseMode;

    unsigned long hbuff_wroffset;
    slice_t *hslicebuffer;
    uint32_t currentPacketTimestamp;
    uint32_t currentFrameTimestamp;
    unsigned char * hbuff;
    int isSliceZero;
    int isRightFrame;
    int isStereo;
    int is444;
    int isMotion444;
    int prevSliceNo;
    int isFirstSlice;
    int isInputFrameStatic;
    unsigned long parse_errors_sof;
    unsigned long parse_errors_ooo_slice;
    unsigned long parse_errors_bad_slice;
    unsigned long parse_errors_bad_frame;

    unsigned long videoSeqNum;
    unsigned long audioSeqNum;

    unsigned long inBytes;
    unsigned long reducedBytes;
    unsigned long inSlices;
    unsigned long reducedSlices;
    unsigned long reducedAltSlices;
    unsigned long outBytes;
    unsigned long outSlices;
    unsigned long padBytes;
    unsigned long hbuff_overflow;
    unsigned long realloc_count;

    unsigned long reducedBytesPerTrack[MAX_SRD_TRACKS];
    unsigned long reducedSlicesPerTrack[MAX_SRD_TRACKS];
    unsigned long reducedAltSlicesPerTrack[MAX_SRD_TRACKS];
    unsigned long outBytesPerTrack[MAX_SRD_TRACKS];
    unsigned long outSlicesPerTrack[MAX_SRD_TRACKS];
    unsigned long reducedFrameCountPerTrack[MAX_SRD_TRACKS];
    unsigned long outFrameCountPerTrack[MAX_SRD_TRACKS];
    unsigned long outPktCountPerTrack[MAX_SRD_TRACKS];
    unsigned long padBytesPerTrack[MAX_SRD_TRACKS];
    unsigned long OutputIFrameCountPerTrack[MAX_SRD_TRACKS];
    int nRefreshSlicePointer[MAX_SRD_TRACKS];
    int iframe_slice_sent_count[MAX_SRD_TRACKS];

    void (*m_newFrameCallback)(void *object);
    void * m_newFrameCallbackObject;

    int isLastSlice;
    int MaxSliceNo;
    int sliceSize;
    int wrSliceIdx;
    int current_frameindex;
    int mono_frameindex;
    int stereo_mode;
    int stereo_policy;
    int slice_drain;
    int track_count;
    int interleave_left;
    int interleave_right;
    int nRefreshSliceRate;
    int bInternalRefresh;
    int isSVC;
    int svc_processing_mode;
    int hoSliceIdx[2];
    int fill_svc_header[MAX_SRD_TRACKS];
    int hoBytes[MAX_SRD_TRACKS][2];
    int currSliceNo[MAX_SRD_TRACKS];
    unsigned char * currSlicePtr[MAX_SRD_TRACKS];

    unsigned long inPktCount;
    unsigned long inFrameCount;
    unsigned long lastInFrameCount;
    unsigned long inRightFrameCount;
    unsigned long inLeftFrameCount;
    unsigned long outFrameCount;
    unsigned long prefillFrameCount;
    unsigned long outPktCount;
    unsigned long reducedFrameCount;
    unsigned long lastInFrameCnt;
    unsigned long IFrameCount;
    unsigned long IFrameSize;
    unsigned long OutputIFrameCount;
    unsigned long FullFrameCount;
    unsigned long FullFrameInterval;
    unsigned long FirstFullFrameInterval;
    unsigned long svcDCBytes;
    unsigned long svcQuadBytes;
    unsigned long svcRemBytes;

    struct timeval tPrevFrameArrival;

    unsigned long m_nVideoLoss;
    unsigned long m_nVideoOutOfOrder;
    unsigned long m_nVideoSeq;
    unsigned long assert2;


    int iBufferSize;
    int m_rdFrameFlushed[MAX_SRD_TRACKS][2];

    frame_t m_wrFrame[2];
    frame_t m_rdFrame[MAX_SRD_TRACKS][2];
    frame_t m_hoFrame[MAX_SRD_TRACKS][2];

    unsigned int avgFrameArrPeriod;
    unsigned int last4bytes;
    double avgSliceSize;
    double avgFrameSize;
    double avgFrameCoverage;
    double avgRefreshSlices;
    int debug_slices;
    slice_t *debug_slicebuffer;
    char debug_slice_sent[2][MAX_SLICE_NO];
    char fullframe_slice_sent[2][MAX_SLICE_NO];
    int mark_slices;
    slice_t *marker_slicebuffer;
    int buffer_pool_count;
    struct buffer_pool_list * buffer_pool_head;
} sliceRedData_t;

// Function prototypes

#ifdef __cplusplus
extern "C" {
#endif
    DLLEXPORT int initSRD(sliceRedData_t *srd);
    DLLEXPORT int destroySRD(sliceRedData_t *srd);
    DLLEXPORT void SRDSetNewFrameCallback(sliceRedData_t *srd,
                            void (*newFrameCallback)(void *object),
                            void *object);
    DLLEXPORT int parsePackets(sliceRedData_t *srd,
                     unsigned char* pTotalData,
                     int iTotalLen);

    DLLEXPORT void SRDSetPacketTimestamp(sliceRedData_t *srd, uint32_t timeStamp);
    DLLEXPORT int findFramesinPacket(sliceRedData_t *srd, unsigned char* pData);
    DLLEXPORT int findSVCFramesinPacket(sliceRedData_t *srd, unsigned char* pData, int pkt_len);
    DLLEXPORT int getPackets(sliceRedData_t *srd,
                   unsigned char *outbuff, int outbufflen, int dopad);
    DLLEXPORT int getPacketsForTrack(sliceRedData_t *srd,
                   unsigned char *outbuff, int outbufflen, int dopad, int trackId);
    DLLEXPORT int getMergedPackets(sliceRedData_t *srd,
                         sliceRedData_t *alt_srd,
                         unsigned char *outbuff);
    DLLEXPORT void setStereoPolicy(sliceRedData_t *srd, int policy);
    DLLEXPORT void setStereoVideo(sliceRedData_t *srd, int is_stereo);
    DLLEXPORT void setCompMode(sliceRedData_t *srd, int compMode);
    DLLEXPORT void setMotionCompMode(sliceRedData_t *srd, int motionCompMode);
    DLLEXPORT void SRDSetNumberOfTracks(sliceRedData_t *srd, int numberOfTracks);
    DLLEXPORT void SRDEnableInternalRefresh(sliceRedData_t *srd);
    DLLEXPORT void SRDSetCodecVersion(sliceRedData_t *srd, int codecVersion);
    DLLEXPORT void SRDSetSVCProcessingMode(sliceRedData_t *srd, int svcProcessingMode);
    DLLEXPORT const char * StereoDetect(sliceRedData_t *srd);
    DLLEXPORT void enableSliceDebug(sliceRedData_t *srd);
    DLLEXPORT void disableSliceDebug(sliceRedData_t *srd);
    DLLEXPORT void enableSliceMarking(sliceRedData_t *srd);
    DLLEXPORT void disableSliceMarking(sliceRedData_t *srd);
    DLLEXPORT int get_hoFrame(sliceRedData_t *srd);
    DLLEXPORT int get_hoFrameForTrack(sliceRedData_t *srd, int trackId);
    DLLEXPORT void requestFullFrame(sliceRedData_t *srd);
    DLLEXPORT int getHoFrameSize(sliceRedData_t *srd);
    DLLEXPORT int getMinHoFrameSize(sliceRedData_t *srd);
    DLLEXPORT int copyHoFrameToBuffer(sliceRedData_t *srd,
                            unsigned char *outBuff,
                            const unsigned int outBuffLen);
    DLLEXPORT uint32_t SRDGetFrameTimeStamp(sliceRedData_t *srd);
    DLLEXPORT int getFullFrameSize(sliceRedData_t *srd);
    DLLEXPORT int getFullFrame(sliceRedData_t *srd,
            unsigned char *outBuff,
            const unsigned int outBuffLen);


    // Fill a sequence number into the given buffer (TODO: take byte
    // ordering into account: currently assuming native byte order which
    // works for DMS. On window's client, we dont even look at the sequence numbers

    DLLEXPORT extern void fillSequence(unsigned char *bf, unsigned int sequence);
    DLLEXPORT extern inline void fillSequence(unsigned char *bf, unsigned int sequence)
    {
        memcpy(bf, &sequence, 4);
    }

#ifdef __cplusplus
}
#endif

#endif // __HH_SRC_H__
