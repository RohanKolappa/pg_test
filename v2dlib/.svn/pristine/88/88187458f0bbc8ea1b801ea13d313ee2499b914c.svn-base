/*****************************************************************
 Description : Defines constants used in decoding v2d frames, 
 coefficient tables etc.
 *****************************************************************/

#if !defined __V2D_CODECDEFS_H__
#define __V2D_CODECDEFS_H__


#define PASS_TESTS
#define NEW_PB
//#define COLLECT_STATS
// enable this macro for progressive or zig-zag scan
//#define USE_PROGRESSIVE_SCAN
#define H264_SLICE_WIDTH 4
#define H264_SLICE_HEIGHT 2

#define SVC_SLICE_WIDTH 8
#define SVC_SLICE_HEIGHT 4
#define SVC_MAX_SLICE_SIZE 128
#define SVC_MAX_FRAME_WIDTH (1920 * 8)
#define SVC_MAX_FRAME_HEIGHT (1200 * 8)

#define V2D_VIDEO_START_R_FRAME_CODE            0x00000100
#define V2D_VIDEO_START_L_FRAME_CODE            0x000001A0
#define V2D_VIDEO_END_FRAME_CODE                0x000001B0
#define V2D_VIDEO_START_SLICE_CODE              0x0001B2
#define V2D_VIDEO_END_SLICE_CODE                0x0001B9
#define V2D_VIDEO_START_CHROMA_SLICE_CODE       0x0001A2

#define V2D_VIDEO_SVC_START_FRAME_CODE          0x00000100
#define V2D_VIDEO_SVC_END_FRAME_CODE            0x000001B0
#define V2D_VIDEO_SVC_START_SLICE_CODE          0x000001B2

#define V2D_MAX_SLICE_SIZE                      256
#define V2D_BLOCK_SIZE                          (8*8)
#define H264_BLOCK_SIZE                          (16*16)

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

#define W1 2841 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565  /* 2048*sqrt(2)*cos(7*pi/16) */

#define ERROR1 (-1)

#define _S(a)	                (a)>255 ? 255 : (a)<0 ? 0 : (a)
#define _CLIP(a,min,max)        (a)>max ? max: (a)<min ? min : (a)

#define CODEC_VERSION_UNKOWN          0
#define CODEC_VERSION_STANDARD        1
#define CODEC_VERSION_SVC             2
#define FIX_CODEC_VERSION(codec_version) (codec_version) == 2 ? 2 : 1

typedef struct
{
    unsigned char code; /* right justified */
    char len;
} VLCtable;

typedef struct
{
    unsigned short code; /* right justified */
    char len;
} sVLCtable;


typedef struct 
{
    char val, len;
}VLCtab;

typedef struct 
{
    char run, level, len;
} DCTtab;

#define SVC_LAYERSIZE_BITS            15
#define SVC_SLICENUMBER_BITS          14

#define SVC_LAYERTYPE_DCLAYER          1
#define SVC_LAYERTYPE_MIDAC4x4LAYER    2
#define SVC_LAYERTYPE_REMAC4x4LAYER    3
#define SVC_LAYERTYPE_QUALITYLAYER     4
#define SVC_LAYERTYPE_CHROMALAYER      5
#define SVC_LAYERTYPE_RESIDUALLAYER    6
#define SVC_LAYERTYPE_FULLLAYER        7
#define SVC_LAYERTYPE_ZEROQUANTLAYER   8
#define SVC_LAYERTYPE_LOSSLESSLAYER    9
#define SVC_LAYERTYPE_NULLLAYER       10

#define SVC_LAYERTYPE_MIDAC2x2LAYER      11
#define SVC_LAYERTYPE_MIDAC3x3LAYER      12
#define SVC_LAYERTYPE_MIDAC5x5LAYER      13
#define SVC_LAYERTYPE_MIDAC6x6LAYER      14
#define SVC_LAYERTYPE_MIDAC7x7LAYER      15

#define SVC_LAYERTYPE_REMAC2x2LAYER      16
#define SVC_LAYERTYPE_REMAC3x3LAYER      17
#define SVC_LAYERTYPE_REMAC5x5LAYER      18
#define SVC_LAYERTYPE_REMAC6x6LAYER      19
#define SVC_LAYERTYPE_REMAC7x7LAYER      20

typedef struct
{
    unsigned int nFtimestamp;
    unsigned int bIFrame;
    unsigned int bFFrame;
    unsigned int bIsStereo;
    unsigned int bIsLeft;
    unsigned int bFrameDimensions;
    unsigned int nFrameWidth;
    unsigned int nFrameHeight;
    unsigned int nEncodedOffsetX;
    unsigned int nEncodedOffsetY;
    unsigned int nEncodedWidth;
    unsigned int nEncodedHeight;
    unsigned int nSliceWidth;
    unsigned int nSliceHeight;
    unsigned int bChromaSamplingMode;
} SVCFrameHeader;

typedef struct
{
    unsigned int nLayerSize;
    unsigned int nLayerType;
    unsigned int bLastLayer;
} SVCLayerHeader;

#define SVC_MAX_DECODER_LAYERS 16

typedef struct
{
    unsigned int bRefreshSlice;
    unsigned int bSliceType;
    unsigned int nSliceNumber;
    unsigned int nMQuant;
    unsigned int nDCQuant;
    unsigned int nAltnMQuant;
    unsigned int nAltnDCQuant;
    unsigned int nNumberOfLayers;
    SVCLayerHeader layerHeaders[SVC_MAX_DECODER_LAYERS];
} SVCSliceHeader;

#ifdef __cplusplus
extern "C" {
#endif

extern int ParseSVCFrameHeader(uint8_t * a_pData, int a_nDataLen,
        SVCFrameHeader * frameHeader);
extern int ParseSVCSliceLayers(uint8_t * a_pData, int a_nDataLen, SVCSliceHeader * sliceHeader);

extern int StripSVCLayer(uint8_t * a_pData, int a_nDataLen,
        const SVCSliceHeader * sliceHeader, uint8_t * a_outBuf, int stripLayerType);

extern int IsRemACLayerType(int layerType);
extern int IsMidACLayerType(int layerType);

#ifdef __cplusplus
}
#endif

extern const unsigned char    quant_mat[64];
extern const unsigned char    quant_mat_mult[64];
extern const sVLCtable        DClumtabHW[12];
extern const VLCtab           DClumtabHW0[32];
extern const VLCtab           DClumtabHW1[16];
extern const DCTtab           DCTtab0[60];
extern const DCTtab           DCTtab0a[252];
extern const DCTtab           DCTtab1[8];
extern const DCTtab           DCTtab1a[8];
extern const DCTtab           DCTtab2[16];
extern const DCTtab           DCTtab3[16];
extern const DCTtab           DCTtab4[16];
extern const DCTtab           DCTtab5[16];
extern const DCTtab           DCTtab6[16];
extern const unsigned char    scan[64];
extern const unsigned char    progressive_scan[64];

enum eEncodeFormat {eYUV422P, eYUV444S, eYUV444D, eH264, eSVC};
enum ePixelFormat {eYUV422, eRGB24, eBGR24, eBGRX32};

enum eSVCEncodingType {
    ScalableEncoding,
    FullEncoding,
    LosslessEncoding,
};

enum eSVCEncodingSubType {
    ScalableStandard,
    ScalableQuad,
    ScalableDC,
};

#define ISSOFCODE(x) \
    ((x)[0] == 0x0) && ((x)[1] == 0x0) && ((x)[2] == 0x01) && \
    ( ((x)[3] == 0x0) || ((x)[3] == 0xA0) )

#endif //__V2D_CODECDEFS_H__

