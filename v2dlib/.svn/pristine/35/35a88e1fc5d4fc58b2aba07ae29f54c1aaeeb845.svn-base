#pragma once

#define IPVS_ENCODE_TYPE_YUV422P 1
#define IPVS_ENCODE_TYPE_YUV444S 2
#define IPVS_ENCODE_TYPE_YUV444D 3
#define IPVS_ENCODE_TYPE_SVC     4

struct v2dLibWrapper_t;

#ifdef __cplusplus
extern "C" {
#endif

    v2dLibWrapper_t* createV2dLibDecObj(int nEncodedFormat, int a_nWidth,
                                        int a_nHeight, int a_nSliceSize);
    void destroyV2dLibDecObj(v2dLibWrapper_t *a_pV2dLibWrapper);

    int  decodeFrame(v2dLibWrapper_t *a_pV2dLibWrapper, 
                     unsigned char *a_pInBuffer, int a_nInLen,
                     unsigned char* a_pOutBuffer);

    void setSliceDebug(v2dLibWrapper_t *a_pV2dLibWrapper, int a_bEnableSliceDebug);

#ifdef __cplusplus
}
#endif
