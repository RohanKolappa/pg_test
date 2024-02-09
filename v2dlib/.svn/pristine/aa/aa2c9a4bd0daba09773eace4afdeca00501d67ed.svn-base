/*
 * svc.h
 *
 *  Created on: Sep 16, 2010
 *      Author: rkale
 */

#ifndef SVC_H_
#define SVC_H_

#include "v2dlib_compatibility.h"

#define SVC_MIN(a,b) ((a)<(b) ? (a) : (b))

// Prediction modes
typedef void (*x264_predict_t)( uint8_t *src, int i_stride );
#ifdef __cplusplus
extern "C" {
#endif

extern x264_predict_t predict_4x4[9+3];
extern x264_predict_t predict_8x8c[4+3];

#ifdef __cplusplus
}
#endif
// Block indexes

static const uint8_t block_idx_x[16] = {
    0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3
};
static const uint8_t block_idx_y[16] = {
    0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3
};


// Zig Zag scan functions
#define ZIG(i,y,x) level[i] = dct[y][x];
static inline void scan_zigzag_8x8full( int level[64], int16_t dct[8][8] )
{
    ZIG( 0,0,0) ZIG( 1,0,1) ZIG( 2,1,0) ZIG( 3,2,0)
    ZIG( 4,1,1) ZIG( 5,0,2) ZIG( 6,0,3) ZIG( 7,1,2)
    ZIG( 8,2,1) ZIG( 9,3,0) ZIG(10,4,0) ZIG(11,3,1)
    ZIG(12,2,2) ZIG(13,1,3) ZIG(14,0,4) ZIG(15,0,5)
    ZIG(16,1,4) ZIG(17,2,3) ZIG(18,3,2) ZIG(19,4,1)
    ZIG(20,5,0) ZIG(21,6,0) ZIG(22,5,1) ZIG(23,4,2)
    ZIG(24,3,3) ZIG(25,2,4) ZIG(26,1,5) ZIG(27,0,6)
    ZIG(28,0,7) ZIG(29,1,6) ZIG(30,2,5) ZIG(31,3,4)
    ZIG(32,4,3) ZIG(33,5,2) ZIG(34,6,1) ZIG(35,7,0)
    ZIG(36,7,1) ZIG(37,6,2) ZIG(38,5,3) ZIG(39,4,4)
    ZIG(40,3,5) ZIG(41,2,6) ZIG(42,1,7) ZIG(43,2,7)
    ZIG(44,3,6) ZIG(45,4,5) ZIG(46,5,4) ZIG(47,6,3)
    ZIG(48,7,2) ZIG(49,7,3) ZIG(50,6,4) ZIG(51,5,5)
    ZIG(52,4,6) ZIG(53,3,7) ZIG(54,4,7) ZIG(55,5,6)
    ZIG(56,6,5) ZIG(57,7,4) ZIG(58,7,5) ZIG(59,6,6)
    ZIG(60,5,7) ZIG(61,6,7) ZIG(62,7,6) ZIG(63,7,7)
}
static inline void scan_zigzag_4x4full( int level[16], int16_t dct[4][4] )
{
    ZIG( 0,0,0) ZIG( 1,0,1) ZIG( 2,1,0) ZIG( 3,2,0)
    ZIG( 4,1,1) ZIG( 5,0,2) ZIG( 6,0,3) ZIG( 7,1,2)
    ZIG( 8,2,1) ZIG( 9,3,0) ZIG(10,3,1) ZIG(11,2,2)
    ZIG(12,1,3) ZIG(13,2,3) ZIG(14,3,2) ZIG(15,3,3)
}
static inline void scan_zigzag_4x4( int level[15], int16_t dct[4][4] )
{
                ZIG( 0,0,1) ZIG( 1,1,0) ZIG( 2,2,0)
    ZIG( 3,1,1) ZIG( 4,0,2) ZIG( 5,0,3) ZIG( 6,1,2)
    ZIG( 7,2,1) ZIG( 8,3,0) ZIG( 9,3,1) ZIG(10,2,2)
    ZIG(11,1,3) ZIG(12,2,3) ZIG(13,3,2) ZIG(14,3,3)
}

static inline void scan_zigzag_2x2_dc( int level[4], int16_t dct[2][2] )
{
    ZIG(0,0,0)
    ZIG(1,0,1)
    ZIG(2,1,0)
    ZIG(3,1,1)
}
#undef ZIG

#define ZIG(i,y,x) {\
    int o = x+y*i_stride;\
    level[i] = p_src[o] - p_dst[o];\
    p_dst[o] = p_src[o];\
}
static inline void sub_zigzag_4x4full( int level[16], const uint8_t *p_src, uint8_t *p_dst, int i_stride )
{
    ZIG( 0,0,0) ZIG( 1,0,1) ZIG( 2,1,0) ZIG( 3,2,0)
    ZIG( 4,1,1) ZIG( 5,0,2) ZIG( 6,0,3) ZIG( 7,1,2)
    ZIG( 8,2,1) ZIG( 9,3,0) ZIG(10,3,1) ZIG(11,2,2)
    ZIG(12,1,3) ZIG(13,2,3) ZIG(14,3,2) ZIG(15,3,3)
}
static inline void sub_zigzag_4x4( int level[15], const uint8_t *p_src, uint8_t *p_dst, int i_stride )
{
                ZIG( 0,0,1) ZIG( 1,1,0) ZIG( 2,2,0)
    ZIG( 3,1,1) ZIG( 4,0,2) ZIG( 5,0,3) ZIG( 6,1,2)
    ZIG( 7,2,1) ZIG( 8,3,0) ZIG( 9,3,1) ZIG(10,2,2)
    ZIG(11,1,3) ZIG(12,2,3) ZIG(13,3,2) ZIG(14,3,3)
}
#undef ZIG

static const int scan_zigzag_x[16]={0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 3, 2, 3};
static const int scan_zigzag_y[16]={0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 3, 2, 1, 2, 3, 3};

static inline void unscan_zigzag_4x4full( int16_t dct[4][4], int level[16] )
{
    int i;

    for( i = 0; i < 16; i++ )
    {
        dct[scan_zigzag_y[i]][scan_zigzag_x[i]] = level[i];
    }
}
static inline void unscan_zigzag_4x4( int16_t dct[4][4], int level[15] )
{
    int i;

    for( i = 1; i < 16; i++ )
    {
        dct[scan_zigzag_y[i]][scan_zigzag_x[i]] = level[i - 1];
    }
}

static inline void unscan_zigzag_2x2_dc( int16_t dct[2][2], int level[4] )
{
    dct[0][0] = level[0];
    dct[0][1] = level[1];
    dct[1][0] = level[2];
    dct[1][1] = level[3];
}


// Prediction
enum intra_chroma_pred_e
{
    I_PRED_CHROMA_DC = 0,
    I_PRED_CHROMA_H  = 1,
    I_PRED_CHROMA_V  = 2,
    I_PRED_CHROMA_P  = 3,

    I_PRED_CHROMA_DC_LEFT = 4,
    I_PRED_CHROMA_DC_TOP  = 5,
    I_PRED_CHROMA_DC_128  = 6
};

static const int x264_mb_pred_mode8x8c_fix[7] =
{
    I_PRED_CHROMA_DC, I_PRED_CHROMA_H, I_PRED_CHROMA_V, I_PRED_CHROMA_P,
    I_PRED_CHROMA_DC, I_PRED_CHROMA_DC,I_PRED_CHROMA_DC
};


enum intra4x4_pred_e
{
    I_PRED_4x4_V  = 0,
    I_PRED_4x4_H  = 1,
    I_PRED_4x4_DC = 2,
    I_PRED_4x4_DDL= 3,
    I_PRED_4x4_DDR= 4,
    I_PRED_4x4_VR = 5,
    I_PRED_4x4_HD = 6,
    I_PRED_4x4_VL = 7,
    I_PRED_4x4_HU = 8,

    I_PRED_4x4_DC_LEFT = 9,
    I_PRED_4x4_DC_TOP  = 10,
    I_PRED_4x4_DC_128  = 11,
};

enum macroblock_position_e
{
    MB_LEFT     = 0x01,
    MB_TOP      = 0x02,
    MB_TOPRIGHT = 0x04,
    MB_TOPLEFT  = 0x08,

    MB_PRIVATE  = 0x10,
};

static const int x264_mb_pred_mode4x4_fix[13] =
{
    -1,
    I_PRED_4x4_V,   I_PRED_4x4_H,   I_PRED_4x4_DC,
    I_PRED_4x4_DDL, I_PRED_4x4_DDR, I_PRED_4x4_VR,
    I_PRED_4x4_HD,  I_PRED_4x4_VL,  I_PRED_4x4_HU,
    I_PRED_4x4_DC,  I_PRED_4x4_DC,  I_PRED_4x4_DC
};

//static const char* mode4x4_string[13] =
//{
//    "V", "H", "DC", "DDL",
//    "DDR", "VR", "HD", "VL",
//    "HU", "DC_LEFT", "DC_TOP", "DC_128"
//};

#define SCAN8_SIZE (6*8)

#define x264_mb_pred_mode4x4_fix(t) x264_mb_pred_mode4x4_fix[(t)+1]

#ifdef __cplusplus
extern "C" {
#endif
extern int mb_pred_mode4x4_valid(int first_block, int first_row, int idx, int i_mode);
extern int mb_pred_chromamode_valid(int first_block, int first_row, int idx, int i_mode);
extern int mb_predict_intra4x4_mode(int idx, int intra4x4_pred_mode[SCAN8_SIZE]);
extern int mb_predict_non_zero_code(int idx, int non_zero_count[]);
extern void predict_4x4_mode_available(unsigned int i_neighbour, int *mode,
        int *pi_count);
extern void predict_8x8chroma_mode_available(unsigned int i_neighbour,
        int *mode, int *pi_count);
extern void create_neighbour_list4(int i_neighbour, int i_neighbour4[16]);

#ifdef __cplusplus
}
#endif

#define SCAN8_SIZE (6*8)
#define SCAN8_0 (4+1*8)

static const int x264_scan8[16+2*4] =
{
    /* Luma */
    4+1*8, 5+1*8, 4+2*8, 5+2*8,
    6+1*8, 7+1*8, 6+2*8, 7+2*8,
    4+3*8, 5+3*8, 4+4*8, 5+4*8,
    6+3*8, 7+3*8, 6+4*8, 7+4*8,

    /* Cb */
    1+1*8, 2+1*8,
    1+2*8, 2+2*8,

    /* Cr */
    1+4*8, 2+4*8,
    1+5*8, 2+5*8,
};

/*
   0 1 2 3 4 5 6 7
 0
 1   B B   L L L L
 2   B B   L L L L
 3         L L L L
 4   R R   L L L L
 5   R R
*/

#ifdef __cplusplus
extern "C" {
#endif

// SAD
extern int pixel_sad_4x4(uint8_t *pix1, int i_stride_pix1,
                 uint8_t *pix2, int i_stride_pix2);
extern int pixel_sad_8x8(uint8_t *pix1, int i_stride_pix1,
                 uint8_t *pix2, int i_stride_pix2);


// DCT
extern void dct2x2dc(int16_t d[2][2]);
extern void sub4x4_dct(int16_t dct[4][4], uint8_t *pix1, int i_pix1,
        uint8_t *pix2, int i_pix2);
extern void add4x4_idct(uint8_t *p_dst, int i_dst, int16_t dct[4][4]);
extern void sub8x8_dct( int16_t dct[4][4][4], uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 );
extern void sub16x16_dct( int16_t dct[16][4][4], uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 );
extern void add8x8_idct( uint8_t *p_dst, int i_dst, int16_t dct[4][4][4] );
extern void add16x16_idct( uint8_t *p_dst, int i_dst, int16_t dct[16][4][4] );


// Quant
extern void create_quant_tables_4x4(int quant4_mf[6][4][4], int dequant4_mf[6][4][4]);
extern void quant_4x4(int16_t dct[4][4], int quant_mf[6][4][4], int i_qscale,
        int b_intra);
extern void quant_2x2_dc( int16_t dct[2][2], int quant_mf[6][4][4], int i_qscale, int b_intra );
extern void dequant_4x4(int16_t dct[4][4], int dequant_mf[6][4][4],
        int i_qscale);
extern void dequant_2x2_dc( int16_t dct[2][2], int dequant_mf[6][4][4], int i_qscale);

#ifdef __cplusplus
}
#endif

// Coded Block Pattern
static const uint8_t intra4x4_cbp_to_golomb[48]=
{
  3, 29, 30, 17, 31, 18, 37,  8, 32, 38, 19,  9, 20, 10, 11,  2,
 16, 33, 34, 21, 35, 22, 39,  4, 36, 40, 23,  5, 24,  6,  7,  1,
 41, 42, 43, 25, 44, 26, 46, 12, 45, 47, 27, 13, 28, 14, 15,  0
};
static const int golomb_to_intra4x4_cbp[48]=
{
    47, 31, 15,  0, 23, 27, 29, 30,  7, 11, 13, 14, 39, 43, 45, 46,
    16,  3,  5, 10, 12, 19, 21, 26, 28, 35, 37, 42, 44,  1,  2,  4,
     8, 17, 18, 20, 24,  6,  9, 22, 25, 32, 33, 34, 36, 40, 38, 41
};


// VLC encode

#define BLOCK_INDEX_CHROMA_DC   (-1)
#define BLOCK_INDEX_LUMA_DC     (-2)

typedef struct
{
    int i_bits;
    int i_size;
} vlc_t;

#define MKVLC( a, b ) { a, b }
static const vlc_t x264_coeff_token[5][17*4] =
{
    /* table 0 */
    {
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x5, 6 ), /* str=000101 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x7, 8 ), /* str=00000111 */
        MKVLC( 0x4, 6 ), /* str=000100 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x7, 9 ), /* str=000000111 */
        MKVLC( 0x6, 8 ), /* str=00000110 */
        MKVLC( 0x5, 7 ), /* str=0000101 */
        MKVLC( 0x3, 5 ), /* str=00011 */

        MKVLC( 0x7, 10 ), /* str=0000000111 */
        MKVLC( 0x6, 9 ), /* str=000000110 */
        MKVLC( 0x5, 8 ), /* str=00000101 */
        MKVLC( 0x3, 6 ), /* str=000011 */

        MKVLC( 0x7, 11 ), /* str=00000000111 */
        MKVLC( 0x6, 10 ), /* str=0000000110 */
        MKVLC( 0x5, 9 ), /* str=000000101 */
        MKVLC( 0x4, 7 ), /* str=0000100 */

        MKVLC( 0xf, 13 ), /* str=0000000001111 */
        MKVLC( 0x6, 11 ), /* str=00000000110 */
        MKVLC( 0x5, 10 ), /* str=0000000101 */
        MKVLC( 0x4, 8 ), /* str=00000100 */

        MKVLC( 0xb, 13 ), /* str=0000000001011 */
        MKVLC( 0xe, 13 ), /* str=0000000001110 */
        MKVLC( 0x5, 11 ), /* str=00000000101 */
        MKVLC( 0x4, 9 ), /* str=000000100 */

        MKVLC( 0x8, 13 ), /* str=0000000001000 */
        MKVLC( 0xa, 13 ), /* str=0000000001010 */
        MKVLC( 0xd, 13 ), /* str=0000000001101 */
        MKVLC( 0x4, 10 ), /* str=0000000100 */

        MKVLC( 0xf, 14 ), /* str=00000000001111 */
        MKVLC( 0xe, 14 ), /* str=00000000001110 */
        MKVLC( 0x9, 13 ), /* str=0000000001001 */
        MKVLC( 0x4, 11 ), /* str=00000000100 */

        MKVLC( 0xb, 14 ), /* str=00000000001011 */
        MKVLC( 0xa, 14 ), /* str=00000000001010 */
        MKVLC( 0xd, 14 ), /* str=00000000001101 */
        MKVLC( 0xc, 13 ), /* str=0000000001100 */

        MKVLC( 0xf, 15 ), /* str=000000000001111 */
        MKVLC( 0xe, 15 ), /* str=000000000001110 */
        MKVLC( 0x9, 14 ), /* str=00000000001001 */
        MKVLC( 0xc, 14 ), /* str=00000000001100 */

        MKVLC( 0xb, 15 ), /* str=000000000001011 */
        MKVLC( 0xa, 15 ), /* str=000000000001010 */
        MKVLC( 0xd, 15 ), /* str=000000000001101 */
        MKVLC( 0x8, 14 ), /* str=00000000001000 */

        MKVLC( 0xf, 16 ), /* str=0000000000001111 */
        MKVLC( 0x1, 15 ), /* str=000000000000001 */
        MKVLC( 0x9, 15 ), /* str=000000000001001 */
        MKVLC( 0xc, 15 ), /* str=000000000001100 */

        MKVLC( 0xb, 16 ), /* str=0000000000001011 */
        MKVLC( 0xe, 16 ), /* str=0000000000001110 */
        MKVLC( 0xd, 16 ), /* str=0000000000001101 */
        MKVLC( 0x8, 15 ), /* str=000000000001000 */

        MKVLC( 0x7, 16 ), /* str=0000000000000111 */
        MKVLC( 0xa, 16 ), /* str=0000000000001010 */
        MKVLC( 0x9, 16 ), /* str=0000000000001001 */
        MKVLC( 0xc, 16 ), /* str=0000000000001100 */

        MKVLC( 0x4, 16 ), /* str=0000000000000100 */
        MKVLC( 0x6, 16 ), /* str=0000000000000110 */
        MKVLC( 0x5, 16 ), /* str=0000000000000101 */
        MKVLC( 0x8, 16 ), /* str=0000000000001000 */
    },

    /* table 1 */
    {
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0xb, 6 ), /* str=001011 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x7, 6 ), /* str=000111 */
        MKVLC( 0x7, 5 ), /* str=00111 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x7, 7 ), /* str=0000111 */
        MKVLC( 0xa, 6 ), /* str=001010 */
        MKVLC( 0x9, 6 ), /* str=001001 */
        MKVLC( 0x5, 4 ), /* str=0101 */

        MKVLC( 0x7, 8 ), /* str=00000111 */
        MKVLC( 0x6, 6 ), /* str=000110 */
        MKVLC( 0x5, 6 ), /* str=000101 */
        MKVLC( 0x4, 4 ), /* str=0100 */

        MKVLC( 0x4, 8 ), /* str=00000100 */
        MKVLC( 0x6, 7 ), /* str=0000110 */
        MKVLC( 0x5, 7 ), /* str=0000101 */
        MKVLC( 0x6, 5 ), /* str=00110 */

        MKVLC( 0x7, 9 ), /* str=000000111 */
        MKVLC( 0x6, 8 ), /* str=00000110 */
        MKVLC( 0x5, 8 ), /* str=00000101 */
        MKVLC( 0x8, 6 ), /* str=001000 */

        MKVLC( 0xf, 11 ), /* str=00000001111 */
        MKVLC( 0x6, 9 ), /* str=000000110 */
        MKVLC( 0x5, 9 ), /* str=000000101 */
        MKVLC( 0x4, 6 ), /* str=000100 */

        MKVLC( 0xb, 11 ), /* str=00000001011 */
        MKVLC( 0xe, 11 ), /* str=00000001110 */
        MKVLC( 0xd, 11 ), /* str=00000001101 */
        MKVLC( 0x4, 7 ), /* str=0000100 */

        MKVLC( 0xf, 12 ), /* str=000000001111 */
        MKVLC( 0xa, 11 ), /* str=00000001010 */
        MKVLC( 0x9, 11 ), /* str=00000001001 */
        MKVLC( 0x4, 9 ), /* str=000000100 */

        MKVLC( 0xb, 12 ), /* str=000000001011 */
        MKVLC( 0xe, 12 ), /* str=000000001110 */
        MKVLC( 0xd, 12 ), /* str=000000001101 */
        MKVLC( 0xc, 11 ), /* str=00000001100 */

        MKVLC( 0x8, 12 ), /* str=000000001000 */
        MKVLC( 0xa, 12 ), /* str=000000001010 */
        MKVLC( 0x9, 12 ), /* str=000000001001 */
        MKVLC( 0x8, 11 ), /* str=00000001000 */

        MKVLC( 0xf, 13 ), /* str=0000000001111 */
        MKVLC( 0xe, 13 ), /* str=0000000001110 */
        MKVLC( 0xd, 13 ), /* str=0000000001101 */
        MKVLC( 0xc, 12 ), /* str=000000001100 */

        MKVLC( 0xb, 13 ), /* str=0000000001011 */
        MKVLC( 0xa, 13 ), /* str=0000000001010 */
        MKVLC( 0x9, 13 ), /* str=0000000001001 */
        MKVLC( 0xc, 13 ), /* str=0000000001100 */

        MKVLC( 0x7, 13 ), /* str=0000000000111 */
        MKVLC( 0xb, 14 ), /* str=00000000001011 */
        MKVLC( 0x6, 13 ), /* str=0000000000110 */
        MKVLC( 0x8, 13 ), /* str=0000000001000 */

        MKVLC( 0x9, 14 ), /* str=00000000001001 */
        MKVLC( 0x8, 14 ), /* str=00000000001000 */
        MKVLC( 0xa, 14 ), /* str=00000000001010 */
        MKVLC( 0x1, 13 ), /* str=0000000000001 */

        MKVLC( 0x7, 14 ), /* str=00000000000111 */
        MKVLC( 0x6, 14 ), /* str=00000000000110 */
        MKVLC( 0x5, 14 ), /* str=00000000000101 */
        MKVLC( 0x4, 14 ), /* str=00000000000100 */
    },
    /* table 2 */
    {
        MKVLC( 0xf, 4 ), /* str=1111 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0xf, 6 ), /* str=001111 */
        MKVLC( 0xe, 4 ), /* str=1110 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0xb, 6 ), /* str=001011 */
        MKVLC( 0xf, 5 ), /* str=01111 */
        MKVLC( 0xd, 4 ), /* str=1101 */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x8, 6 ), /* str=001000 */
        MKVLC( 0xc, 5 ), /* str=01100 */
        MKVLC( 0xe, 5 ), /* str=01110 */
        MKVLC( 0xc, 4 ), /* str=1100 */

        MKVLC( 0xf, 7 ), /* str=0001111 */
        MKVLC( 0xa, 5 ), /* str=01010 */
        MKVLC( 0xb, 5 ), /* str=01011 */
        MKVLC( 0xb, 4 ), /* str=1011 */

        MKVLC( 0xb, 7 ), /* str=0001011 */
        MKVLC( 0x8, 5 ), /* str=01000 */
        MKVLC( 0x9, 5 ), /* str=01001 */
        MKVLC( 0xa, 4 ), /* str=1010 */

        MKVLC( 0x9, 7 ), /* str=0001001 */
        MKVLC( 0xe, 6 ), /* str=001110 */
        MKVLC( 0xd, 6 ), /* str=001101 */
        MKVLC( 0x9, 4 ), /* str=1001 */

        MKVLC( 0x8, 7 ), /* str=0001000 */
        MKVLC( 0xa, 6 ), /* str=001010 */
        MKVLC( 0x9, 6 ), /* str=001001 */
        MKVLC( 0x8, 4 ), /* str=1000 */

        MKVLC( 0xf, 8 ), /* str=00001111 */
        MKVLC( 0xe, 7 ), /* str=0001110 */
        MKVLC( 0xd, 7 ), /* str=0001101 */
        MKVLC( 0xd, 5 ), /* str=01101 */

        MKVLC( 0xb, 8 ), /* str=00001011 */
        MKVLC( 0xe, 8 ), /* str=00001110 */
        MKVLC( 0xa, 7 ), /* str=0001010 */
        MKVLC( 0xc, 6 ), /* str=001100 */

        MKVLC( 0xf, 9 ), /* str=000001111 */
        MKVLC( 0xa, 8 ), /* str=00001010 */
        MKVLC( 0xd, 8 ), /* str=00001101 */
        MKVLC( 0xc, 7 ), /* str=0001100 */

        MKVLC( 0xb, 9 ), /* str=000001011 */
        MKVLC( 0xe, 9 ), /* str=000001110 */
        MKVLC( 0x9, 8 ), /* str=00001001 */
        MKVLC( 0xc, 8 ), /* str=00001100 */

        MKVLC( 0x8, 9 ), /* str=000001000 */
        MKVLC( 0xa, 9 ), /* str=000001010 */
        MKVLC( 0xd, 9 ), /* str=000001101 */
        MKVLC( 0x8, 8 ), /* str=00001000 */

        MKVLC( 0xd, 10 ), /* str=0000001101 */
        MKVLC( 0x7, 9 ), /* str=000000111 */
        MKVLC( 0x9, 9 ), /* str=000001001 */
        MKVLC( 0xc, 9 ), /* str=000001100 */

        MKVLC( 0x9, 10 ), /* str=0000001001 */
        MKVLC( 0xc, 10 ), /* str=0000001100 */
        MKVLC( 0xb, 10 ), /* str=0000001011 */
        MKVLC( 0xa, 10 ), /* str=0000001010 */

        MKVLC( 0x5, 10 ), /* str=0000000101 */
        MKVLC( 0x8, 10 ), /* str=0000001000 */
        MKVLC( 0x7, 10 ), /* str=0000000111 */
        MKVLC( 0x6, 10 ), /* str=0000000110 */

        MKVLC( 0x1, 10 ), /* str=0000000001 */
        MKVLC( 0x4, 10 ), /* str=0000000100 */
        MKVLC( 0x3, 10 ), /* str=0000000011 */
        MKVLC( 0x2, 10 ), /* str=0000000010 */
    },

    /* table 3 */
    {
        MKVLC( 0x3, 6 ), /* str=000011 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x4, 6 ), /* str=000100 */
        MKVLC( 0x5, 6 ), /* str=000101 */
        MKVLC( 0x6, 6 ), /* str=000110 */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x8, 6 ), /* str=001000 */
        MKVLC( 0x9, 6 ), /* str=001001 */
        MKVLC( 0xa, 6 ), /* str=001010 */
        MKVLC( 0xb, 6 ), /* str=001011 */

        MKVLC( 0xc, 6 ), /* str=001100 */
        MKVLC( 0xd, 6 ), /* str=001101 */
        MKVLC( 0xe, 6 ), /* str=001110 */
        MKVLC( 0xf, 6 ), /* str=001111 */

        MKVLC( 0x10, 6 ), /* str=010000 */
        MKVLC( 0x11, 6 ), /* str=010001 */
        MKVLC( 0x12, 6 ), /* str=010010 */
        MKVLC( 0x13, 6 ), /* str=010011 */

        MKVLC( 0x14, 6 ), /* str=010100 */
        MKVLC( 0x15, 6 ), /* str=010101 */
        MKVLC( 0x16, 6 ), /* str=010110 */
        MKVLC( 0x17, 6 ), /* str=010111 */

        MKVLC( 0x18, 6 ), /* str=011000 */
        MKVLC( 0x19, 6 ), /* str=011001 */
        MKVLC( 0x1a, 6 ), /* str=011010 */
        MKVLC( 0x1b, 6 ), /* str=011011 */

        MKVLC( 0x1c, 6 ), /* str=011100 */
        MKVLC( 0x1d, 6 ), /* str=011101 */
        MKVLC( 0x1e, 6 ), /* str=011110 */
        MKVLC( 0x1f, 6 ), /* str=011111 */

        MKVLC( 0x20, 6 ), /* str=100000 */
        MKVLC( 0x21, 6 ), /* str=100001 */
        MKVLC( 0x22, 6 ), /* str=100010 */
        MKVLC( 0x23, 6 ), /* str=100011 */

        MKVLC( 0x24, 6 ), /* str=100100 */
        MKVLC( 0x25, 6 ), /* str=100101 */
        MKVLC( 0x26, 6 ), /* str=100110 */
        MKVLC( 0x27, 6 ), /* str=100111 */

        MKVLC( 0x28, 6 ), /* str=101000 */
        MKVLC( 0x29, 6 ), /* str=101001 */
        MKVLC( 0x2a, 6 ), /* str=101010 */
        MKVLC( 0x2b, 6 ), /* str=101011 */

        MKVLC( 0x2c, 6 ), /* str=101100 */
        MKVLC( 0x2d, 6 ), /* str=101101 */
        MKVLC( 0x2e, 6 ), /* str=101110 */
        MKVLC( 0x2f, 6 ), /* str=101111 */

        MKVLC( 0x30, 6 ), /* str=110000 */
        MKVLC( 0x31, 6 ), /* str=110001 */
        MKVLC( 0x32, 6 ), /* str=110010 */
        MKVLC( 0x33, 6 ), /* str=110011 */

        MKVLC( 0x34, 6 ), /* str=110100 */
        MKVLC( 0x35, 6 ), /* str=110101 */
        MKVLC( 0x36, 6 ), /* str=110110 */
        MKVLC( 0x37, 6 ), /* str=110111 */

        MKVLC( 0x38, 6 ), /* str=111000 */
        MKVLC( 0x39, 6 ), /* str=111001 */
        MKVLC( 0x3a, 6 ), /* str=111010 */
        MKVLC( 0x3b, 6 ), /* str=111011 */

        MKVLC( 0x3c, 6 ), /* str=111100 */
        MKVLC( 0x3d, 6 ), /* str=111101 */
        MKVLC( 0x3e, 6 ), /* str=111110 */
        MKVLC( 0x3f, 6 ), /* str=111111 */
    },

    /* table 4 */
    {
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x7, 6 ), /* str=000111 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x4, 6 ), /* str=000100 */
        MKVLC( 0x6, 6 ), /* str=000110 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x3, 6 ), /* str=000011 */
        MKVLC( 0x3, 7 ), /* str=0000011 */
        MKVLC( 0x2, 7 ), /* str=0000010 */
        MKVLC( 0x5, 6 ), /* str=000101 */

        MKVLC( 0x2, 6 ), /* str=000010 */
        MKVLC( 0x3, 8 ), /* str=00000011 */
        MKVLC( 0x2, 8 ), /* str=00000010 */
        MKVLC( 0x0, 7 ), /* str=0000000 */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */

        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    }
};

static const vlc_t x264_level_prefix[16] =
{
    MKVLC( 0x01,  1 ),
    MKVLC( 0x01,  2 ),
    MKVLC( 0x01,  3 ),
    MKVLC( 0x01,  4 ),
    MKVLC( 0x01,  5 ),
    MKVLC( 0x01,  6 ),
    MKVLC( 0x01,  7 ),
    MKVLC( 0x01,  8 ),
    MKVLC( 0x01,  9 ),
    MKVLC( 0x01, 10 ),
    MKVLC( 0x01, 11 ),
    MKVLC( 0x01, 12 ),
    MKVLC( 0x01, 13 ),
    MKVLC( 0x01, 14 ),
    MKVLC( 0x01, 15 ),
    MKVLC( 0x01, 16 )
};

/* [i_total_coeff-1][i_total_zeros] */
static const vlc_t x264_total_zeros[15][16] =
{
    { /* i_total 1 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x3, 4 ), /* str=0011 */
        MKVLC( 0x2, 4 ), /* str=0010 */
        MKVLC( 0x3, 5 ), /* str=00011 */
        MKVLC( 0x2, 5 ), /* str=00010 */
        MKVLC( 0x3, 6 ), /* str=000011 */
        MKVLC( 0x2, 6 ), /* str=000010 */
        MKVLC( 0x3, 7 ), /* str=0000011 */
        MKVLC( 0x2, 7 ), /* str=0000010 */
        MKVLC( 0x3, 8 ), /* str=00000011 */
        MKVLC( 0x2, 8 ), /* str=00000010 */
        MKVLC( 0x3, 9 ), /* str=000000011 */
        MKVLC( 0x2, 9 ), /* str=000000010 */
        MKVLC( 0x1, 9 ), /* str=000000001 */
    },
    { /* i_total 2 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x5, 4 ), /* str=0101 */
        MKVLC( 0x4, 4 ), /* str=0100 */
        MKVLC( 0x3, 4 ), /* str=0011 */
        MKVLC( 0x2, 4 ), /* str=0010 */
        MKVLC( 0x3, 5 ), /* str=00011 */
        MKVLC( 0x2, 5 ), /* str=00010 */
        MKVLC( 0x3, 6 ), /* str=000011 */
        MKVLC( 0x2, 6 ), /* str=000010 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 3 */
        MKVLC( 0x5, 4 ), /* str=0101 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 4 ), /* str=0100 */
        MKVLC( 0x3, 4 ), /* str=0011 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 4 ), /* str=0010 */
        MKVLC( 0x3, 5 ), /* str=00011 */
        MKVLC( 0x2, 5 ), /* str=00010 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 4 */
        MKVLC( 0x3, 5 ), /* str=00011 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x5, 4 ), /* str=0101 */
        MKVLC( 0x4, 4 ), /* str=0100 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 4 ), /* str=0011 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 4 ), /* str=0010 */
        MKVLC( 0x2, 5 ), /* str=00010 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x0, 5 ), /* str=00000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 5 */
        MKVLC( 0x5, 4 ), /* str=0101 */
        MKVLC( 0x4, 4 ), /* str=0100 */
        MKVLC( 0x3, 4 ), /* str=0011 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 4 ), /* str=0010 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x0, 5 ), /* str=00000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 6 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 7 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 8 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 9 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x0, 6 ), /* str=000000 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 10 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x0, 5 ), /* str=00000 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 11 */
        MKVLC( 0x0, 4 ), /* str=0000 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 12 */
        MKVLC( 0x0, 4 ), /* str=0000 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 13 */
        MKVLC( 0x0, 3 ), /* str=000 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 14 */
        MKVLC( 0x0, 2 ), /* str=00 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_total 15 */
        MKVLC( 0x0, 1 ), /* str=0 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
};

/* [i_total_coeff-1][i_total_zeros] */
static const vlc_t x264_total_zeros_dc[3][4] =
{
    {
        MKVLC( 0x01, 1 ), /* 1  */
        MKVLC( 0x01, 2 ), /* 01 */
        MKVLC( 0x01, 3 ), /* 001*/
        MKVLC( 0x00, 3 )  /* 000*/
    },
    {
        MKVLC( 0x01, 1 ), /* 1  */
        MKVLC( 0x01, 2 ), /* 01 */
        MKVLC( 0x00, 2 ), /* 00 */
        MKVLC( 0x00, 0 )  /*    */
    },
    {
        MKVLC( 0x01, 1 ), /* 1  */
        MKVLC( 0x00, 1 ), /* 0  */
        MKVLC( 0x00, 0 ), /*    */
        MKVLC( 0x00, 0 )  /*    */
    }
};

/* x264_run_before[__MIN( i_zero_left -1, 6 )][run_before] */
static const vlc_t x264_run_before[7][15] =
{
    { /* i_zero_left 1 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x0, 1 ), /* str=0 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 2 */
        MKVLC( 0x1, 1 ), /* str=1 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x0, 2 ), /* str=00 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 3 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x0, 2 ), /* str=00 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 4 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x1, 2 ), /* str=01 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 3 ), /* str=000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 5 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x2, 2 ), /* str=10 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x0, 3 ), /* str=000 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 6 */
        MKVLC( 0x3, 2 ), /* str=11 */
        MKVLC( 0x0, 3 ), /* str=000 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
        MKVLC( 0x0, 0 ), /* str= */
    },
    { /* i_zero_left 7 */
        MKVLC( 0x7, 3 ), /* str=111 */
        MKVLC( 0x6, 3 ), /* str=110 */
        MKVLC( 0x5, 3 ), /* str=101 */
        MKVLC( 0x4, 3 ), /* str=100 */
        MKVLC( 0x3, 3 ), /* str=011 */
        MKVLC( 0x2, 3 ), /* str=010 */
        MKVLC( 0x1, 3 ), /* str=001 */
        MKVLC( 0x1, 4 ), /* str=0001 */
        MKVLC( 0x1, 5 ), /* str=00001 */
        MKVLC( 0x1, 6 ), /* str=000001 */
        MKVLC( 0x1, 7 ), /* str=0000001 */
        MKVLC( 0x1, 8 ), /* str=00000001 */
        MKVLC( 0x1, 9 ), /* str=000000001 */
        MKVLC( 0x1, 10 ), /* str=0000000001 */
        MKVLC( 0x1, 11 ), /* str=00000000001 */
    },
};

// VLC decode
typedef struct
{
    int i_value;
    int i_size;
} vlc_lookup_t;

struct x264_vlc_table_t
{
    int          i_lookup_bits;

    int          i_lookup;
    vlc_lookup_t *lookup;
};

#ifdef __cplusplus
extern "C" {
#endif

extern struct x264_vlc_table_t *vlc_table_lookup_new(const vlc_t *vlc, int i_vlc, int i_lookup_bits);
void vlc_table_lookup_delete(struct x264_vlc_table_t *table);

#ifdef __cplusplus
}
#endif
#endif /* SVC_H_ */
