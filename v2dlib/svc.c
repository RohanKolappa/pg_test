/*
 * svc.c
 *
 *  Created on: Sep 18, 2010
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include "aligned_malloc.h"
#include "stdlib.h"
#include "malloc.h"
#include "svc.h"

static inline int clip_uint8(int a)
{
    if (a & (~255))
        return (-a) >> 31;
    else
        return a;
}

// Prediction 4x4
static void predict_4x4_dc_128( uint8_t *src, int i_stride )
{
    int x,y;
    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[x] = 128;
        }
        src += i_stride;
    }
}

static void predict_4x4_dc_left( uint8_t *src, int i_stride )
{
    int x,y;
    int dc = ( src[-1+0*i_stride] + src[-1+i_stride]+
               src[-1+2*i_stride] + src[-1+3*i_stride] + 2 ) >> 2;

    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[x] = dc;
        }
        src += i_stride;
    }
}
static void predict_4x4_dc_top( uint8_t *src, int i_stride )
{
    int x,y;
    int dc = ( src[0 - i_stride] + src[1 - i_stride] +
               src[2 - i_stride] + src[3 - i_stride] + 2 ) >> 2;

    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[x] = dc;
        }
        src += i_stride;
    }
}
static void predict_4x4_dc( uint8_t *src, int i_stride )
{
    int x,y;
    int dc = ( src[-1+0*i_stride] + src[-1+i_stride]+
               src[-1+2*i_stride] + src[-1+3*i_stride] +
               src[0 - i_stride]  + src[1 - i_stride] +
               src[2 - i_stride]  + src[3 - i_stride] + 4 ) >> 3;

    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[x] = dc;
        }
        src += i_stride;
    }
}


static void predict_4x4_h( uint8_t *src, int i_stride )
{
    int i,j;

    for( i = 0; i < 4; i++ )
    {
        uint8_t v;

        v = src[-1];

        for( j = 0; j < 4; j++ )
        {
            src[j] = v;
        }
        src += i_stride;
    }
}
static void predict_4x4_v( uint8_t *src, int i_stride )
{
    int i,j;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            src[i * i_stride +j] = src[j - i_stride];
        }
    }
}

#define PREDICT_4x4_LOAD_LEFT \
    const int l0 = src[-1+0*i_stride];   \
    const int l1 = src[-1+1*i_stride];   \
    const int l2 = src[-1+2*i_stride];   \
    const int l3 = src[-1+3*i_stride];

#define PREDICT_4x4_LOAD_TOP \
    const int t0 = src[0-1*i_stride];   \
    const int t1 = src[1-1*i_stride];   \
    const int t2 = src[2-1*i_stride];   \
    const int t3 = src[3-1*i_stride];

#define PREDICT_4x4_LOAD_TOP_RIGHT \
    const int t4 = src[4-1*i_stride];   \
    const int t5 = src[5-1*i_stride];   \
    const int t6 = src[6-1*i_stride];   \
    const int t7 = src[7-1*i_stride];

static void predict_4x4_ddl( uint8_t *src, int i_stride )
{
    PREDICT_4x4_LOAD_TOP
    PREDICT_4x4_LOAD_TOP_RIGHT

    src[0*i_stride+0] = ( t0 + 2*t1+ t2 + 2 ) >> 2;

    src[0*i_stride+1] =
    src[1*i_stride+0] = ( t1 + 2*t2+ t3 + 2 ) >> 2;

    src[0*i_stride+2] =
    src[1*i_stride+1] =
    src[2*i_stride+0] = ( t2 + 2*t3+ t4 + 2 ) >> 2;

    src[0*i_stride+3] =
    src[1*i_stride+2] =
    src[2*i_stride+1] =
    src[3*i_stride+0] = ( t3 + 2*t4+ t5 + 2 ) >> 2;

    src[1*i_stride+3] =
    src[2*i_stride+2] =
    src[3*i_stride+1] = ( t4 + 2*t5+ t6 + 2 ) >> 2;

    src[2*i_stride+3] =
    src[3*i_stride+2] = ( t5 + 2*t6+ t7 + 2 ) >> 2;

    src[3*i_stride+3] = ( t6 + 3 * t7 + 2 ) >> 2;
}
static void predict_4x4_ddr( uint8_t *src, int i_stride )
{
    const int lt = src[-1-i_stride];
    PREDICT_4x4_LOAD_LEFT
    PREDICT_4x4_LOAD_TOP

    src[0*i_stride+0] =
    src[1*i_stride+1] =
    src[2*i_stride+2] =
    src[3*i_stride+3] = ( t0 + 2*lt +l0 + 2 ) >> 2;

    src[0*i_stride+1] =
    src[1*i_stride+2] =
    src[2*i_stride+3] = ( lt + 2 * t0 + t1 + 2 ) >> 2;

    src[0*i_stride+2] =
    src[1*i_stride+3] = ( t0 + 2 * t1 + t2 + 2 ) >> 2;

    src[0*i_stride+3] = ( t1 + 2 * t2 + t3 + 2 ) >> 2;

    src[1*i_stride+0] =
    src[2*i_stride+1] =
    src[3*i_stride+2] = ( lt + 2 * l0 + l1 + 2 ) >> 2;

    src[2*i_stride+0] =
    src[3*i_stride+1] = ( l0 + 2 * l1 + l2 + 2 ) >> 2;

    src[3*i_stride+0] = ( l1 + 2 * l2 + l3 + 2 ) >> 2;
}

static void predict_4x4_vr( uint8_t *src, int i_stride )
{
    const int lt = src[-1-i_stride];
    PREDICT_4x4_LOAD_LEFT
    PREDICT_4x4_LOAD_TOP
    (void)l3;
    /* produce warning as l3 is unused */

    src[0*i_stride+0]=
    src[2*i_stride+1]= ( lt + t0 + 1 ) >> 1;

    src[0*i_stride+1]=
    src[2*i_stride+2]= ( t0 + t1 + 1 ) >> 1;

    src[0*i_stride+2]=
    src[2*i_stride+3]= ( t1 + t2 + 1 ) >> 1;

    src[0*i_stride+3]= ( t2 + t3 + 1 ) >> 1;

    src[1*i_stride+0]=
    src[3*i_stride+1]= ( l0 + 2 * lt + t0 + 2 ) >> 2;

    src[1*i_stride+1]=
    src[3*i_stride+2]= ( lt + 2 * t0 + t1 + 2 ) >> 2;

    src[1*i_stride+2]=
    src[3*i_stride+3]= ( t0 + 2 * t1 + t2 + 2) >> 2;

    src[1*i_stride+3]= ( t1 + 2 * t2 + t3 + 2 ) >> 2;
    src[2*i_stride+0]= ( lt + 2 * l0 + l1 + 2 ) >> 2;
    src[3*i_stride+0]= ( l0 + 2 * l1 + l2 + 2 ) >> 2;
}

static void predict_4x4_hd( uint8_t *src, int i_stride )
{
    const int lt= src[-1-1*i_stride];
    PREDICT_4x4_LOAD_LEFT
    PREDICT_4x4_LOAD_TOP
    /* produce warning as t3 is unused */
    (void)t3;

    src[0*i_stride+0]=
    src[1*i_stride+2]= ( lt + l0 + 1 ) >> 1;
    src[0*i_stride+1]=
    src[1*i_stride+3]= ( l0 + 2 * lt + t0 + 2 ) >> 2;
    src[0*i_stride+2]= ( lt + 2 * t0 + t1 + 2 ) >> 2;
    src[0*i_stride+3]= ( t0 + 2 * t1 + t2 + 2 ) >> 2;
    src[1*i_stride+0]=
    src[2*i_stride+2]= ( l0 + l1 + 1 ) >> 1;
    src[1*i_stride+1]=
    src[2*i_stride+3]= ( lt + 2 * l0 + l1 + 2 ) >> 2;
    src[2*i_stride+0]=
    src[3*i_stride+2]= ( l1 + l2+ 1 ) >> 1;
    src[2*i_stride+1]=
    src[3*i_stride+3]= ( l0 + 2 * l1 + l2 + 2 ) >> 2;
    src[3*i_stride+0]= ( l2 + l3 + 1 ) >> 1;
    src[3*i_stride+1]= ( l1 + 2 * l2 + l3 + 2 ) >> 2;
}

static void predict_4x4_vl( uint8_t *src, int i_stride )
{
    PREDICT_4x4_LOAD_TOP
    PREDICT_4x4_LOAD_TOP_RIGHT
    /* produce warning as t7 is unused */
    (void)t7;

    src[0*i_stride+0]= ( t0 + t1 + 1 ) >> 1;
    src[0*i_stride+1]=
    src[2*i_stride+0]= ( t1 + t2 + 1 ) >> 1;
    src[0*i_stride+2]=
    src[2*i_stride+1]= ( t2 + t3 + 1 ) >> 1;
    src[0*i_stride+3]=
    src[2*i_stride+2]= ( t3 + t4+ 1 ) >> 1;
    src[2*i_stride+3]= ( t4 + t5+ 1 ) >> 1;
    src[1*i_stride+0]= ( t0 + 2 * t1 + t2 + 2 ) >> 2;
    src[1*i_stride+1]=
    src[3*i_stride+0]= ( t1 + 2 * t2 + t3 + 2 ) >> 2;
    src[1*i_stride+2]=
    src[3*i_stride+1]= ( t2 + 2 * t3 + t4 + 2 ) >> 2;
    src[1*i_stride+3]=
    src[3*i_stride+2]= ( t3 + 2 * t4 + t5 + 2 ) >> 2;
    src[3*i_stride+3]= ( t4 + 2 * t5 + t6 + 2 ) >> 2;
}

static void predict_4x4_hu( uint8_t *src, int i_stride )
{
    PREDICT_4x4_LOAD_LEFT

    src[0*i_stride+0]= ( l0 + l1 + 1 ) >> 1;
    src[0*i_stride+1]= ( l0 + 2 * l1 + l2 + 2 ) >> 2;

    src[0*i_stride+2]=
    src[1*i_stride+0]= ( l1 + l2 + 1 ) >> 1;

    src[0*i_stride+3]=
    src[1*i_stride+1]= ( l1 + 2*l2 + l3 + 2 ) >> 2;

    src[1*i_stride+2]=
    src[2*i_stride+0]= ( l2 + l3 + 1 ) >> 1;

    src[1*i_stride+3]=
    src[2*i_stride+1]= ( l2 + 2 * l3 + l3 + 2 ) >> 2;

    src[2*i_stride+3]=
    src[3*i_stride+1]=
    src[3*i_stride+0]=
    src[2*i_stride+2]=
    src[3*i_stride+2]=
    src[3*i_stride+3]= l3;
}



x264_predict_t predict_4x4[9+3] = {
    predict_4x4_v,
    predict_4x4_h,
    predict_4x4_dc,
    predict_4x4_ddl,
    predict_4x4_ddr,
    predict_4x4_vr,
    predict_4x4_hd,
    predict_4x4_vl,
    predict_4x4_hu,
    predict_4x4_dc_left,
    predict_4x4_dc_top,
    predict_4x4_dc_128,
};

// Prediction 8x8 Chroma
/****************************************************************************
 * 8x8 prediction for intra chroma block DC, H, V, P
 ****************************************************************************/
static void predict_8x8c_dc_128( uint8_t *src, int i_stride )
{
    int x,y;

    for( y = 0; y < 8; y++ )
    {
        for( x = 0; x < 8; x++ )
        {
            src[x] = 128;
        }
        src += i_stride;
    }
}
static void predict_8x8c_dc_left( uint8_t *src, int i_stride )
{
    int x,y;
    int dc0 = 0, dc1 = 0;

    for( y = 0; y < 4; y++ )
    {
        dc0 += src[y * i_stride     - 1];
        dc1 += src[(y+4) * i_stride - 1];
    }
    dc0 = ( dc0 + 2 ) >> 2;
    dc1 = ( dc1 + 2 ) >> 2;

    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 8; x++ )
        {
            src[           x] = dc0;
            src[4*i_stride+x] = dc1;
        }
        src += i_stride;
    }
}
static void predict_8x8c_dc_top( uint8_t *src, int i_stride )
{
    int x,y;
    int dc0 = 0, dc1 = 0;

    for( x = 0; x < 4; x++ )
    {
        dc0 += src[x     - i_stride];
        dc1 += src[x + 4 - i_stride];
    }
    dc0 = ( dc0 + 2 ) >> 2;
    dc1 = ( dc1 + 2 ) >> 2;

    for( y = 0; y < 8; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[x    ] = dc0;
            src[x + 4] = dc1;
        }
        src += i_stride;
    }
}
static void predict_8x8c_dc( uint8_t *src, int i_stride )
{
    int x,y;
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    int dc0, dc1, dc2, dc3;
    int i;

    /* First do :
          s0 s1
       s2
       s3
    */
    for( i = 0; i < 4; i++ )
    {
        s0 += src[i - i_stride];
        s1 += src[i + 4 - i_stride];
        s2 += src[-1 + i * i_stride];
        s3 += src[-1 + (i+4)*i_stride];
    }
    /* now calculate
       dc0 dc1
       dc2 dc3
     */
    dc0 = ( s0 + s2 + 4 ) >> 3;
    dc1 = ( s1 + 2 ) >> 2;
    dc2 = ( s3 + 2 ) >> 2;
    dc3 = ( s1 + s3 + 4 ) >> 3;

    for( y = 0; y < 4; y++ )
    {
        for( x = 0; x < 4; x++ )
        {
            src[             x    ] = dc0;
            src[             x + 4] = dc1;
            src[4*i_stride + x    ] = dc2;
            src[4*i_stride + x + 4] = dc3;
        }
        src += i_stride;
    }
}

static void predict_8x8c_h( uint8_t *src, int i_stride )
{
    int i,j;

    for( i = 0; i < 8; i++ )
    {
        uint8_t v;

        v = src[-1];

        for( j = 0; j < 8; j++ )
        {
            src[j] = v;
        }
        src += i_stride;
    }
}
static void predict_8x8c_v( uint8_t *src, int i_stride )
{
    int i,j;

    for( i = 0; i < 8; i++ )
    {
        for( j = 0; j < 8; j++ )
        {
            src[i * i_stride +j] = src[j - i_stride];
        }
    }
}

static void predict_8x8c_p( uint8_t *src, int i_stride )
{
    int i;
    int x,y;
    int a, b, c;
    int H = 0;
    int V = 0;
    int i00;

    for( i = 0; i < 4; i++ )
    {
        H += ( i + 1 ) * ( src[4+i - i_stride] - src[2 - i -i_stride] );
        V += ( i + 1 ) * ( src[-1 +(i+4)*i_stride] - src[-1+(2-i)*i_stride] );
    }

    a = 16 * ( src[-1+7*i_stride] + src[7 - i_stride] );
    b = ( 17 * H + 16 ) >> 5;
    c = ( 17 * V + 16 ) >> 5;
    i00 = a -3*b -3*c + 16;

    for( y = 0; y < 8; y++ )
    {
        for( x = 0; x < 8; x++ )
        {
            int pix;

            pix = (i00 +b*x) >> 5;
            src[x] = clip_uint8( pix );
        }
        src += i_stride;
        i00 += c;
    }
}

x264_predict_t predict_8x8c[4+3] = {
    predict_8x8c_dc,
    predict_8x8c_h,
    predict_8x8c_v,
    predict_8x8c_p,
    predict_8x8c_dc_left,
    predict_8x8c_dc_top,
    predict_8x8c_dc_128,
};

// Prediction functions

/* MAX = 9 */
void predict_4x4_mode_available(unsigned int i_neighbour, int *mode, int *pi_count)
{
    int b_l = i_neighbour & MB_LEFT;
    int b_t = i_neighbour & MB_TOP;
    int b_tr = i_neighbour & MB_TOPRIGHT;

    if (b_l && b_t) {
        *mode++ = I_PRED_4x4_DC;
        *mode++ = I_PRED_4x4_H;
        *mode++ = I_PRED_4x4_V;
        *mode++ = I_PRED_4x4_DDR;
        *mode++ = I_PRED_4x4_VR;
        *mode++ = I_PRED_4x4_HD;
        *mode++ = I_PRED_4x4_HU;
        *pi_count = 7;
    }
    else if (b_l) {
        *mode++ = I_PRED_4x4_DC_LEFT;
        *mode++ = I_PRED_4x4_H;
        *mode++ = I_PRED_4x4_HU;
        *pi_count = 3;
    }
    else if (b_t) {
        *mode++ = I_PRED_4x4_DC_TOP;
        *mode++ = I_PRED_4x4_V;
        *pi_count = 2;
    }
    else {
        *mode++ = I_PRED_4x4_DC_128;
        *pi_count = 1;
    }

    if (b_t && b_tr) {
        *mode++ = I_PRED_4x4_DDL;
        *mode++ = I_PRED_4x4_VL;
        (*pi_count) += 2;
    }
}

/* Max = 4 */
void predict_8x8chroma_mode_available( unsigned int i_neighbour, int *mode, int *pi_count )
{
    if( i_neighbour & MB_TOPLEFT )
    {
        /* top and left available */
        *mode++ = I_PRED_CHROMA_V;
        *mode++ = I_PRED_CHROMA_H;
        *mode++ = I_PRED_CHROMA_DC;
        *mode++ = I_PRED_CHROMA_P;
        *pi_count = 4;
    }
    else if( i_neighbour & MB_LEFT )
    {
        /* left available*/
        *mode++ = I_PRED_CHROMA_DC_LEFT;
        *mode++ = I_PRED_CHROMA_H;
        *pi_count = 2;
    }
    else if( i_neighbour & MB_TOP )
    {
        /* top available*/
        *mode++ = I_PRED_CHROMA_DC_TOP;
        *mode++ = I_PRED_CHROMA_V;
        *pi_count = 2;
    }
    else
    {
        /* none available */
        *mode = I_PRED_CHROMA_DC_128;
        *pi_count = 1;
    }
}

// We always call this routine with i_neighbour with MB_LEFT set or cleared.
void create_neighbour_list4(int i_neighbour, int i_neighbour4[16]) {


    i_neighbour4[0] = (i_neighbour & (MB_TOP | MB_LEFT | MB_TOPLEFT))
                            | ((i_neighbour & MB_TOP) ? MB_TOPRIGHT : 0);

    i_neighbour4[4] =
    i_neighbour4[1] = MB_LEFT | ((i_neighbour & MB_TOP) ? (MB_TOP | MB_TOPLEFT | MB_TOPRIGHT) : 0);

    i_neighbour4[2] =
    i_neighbour4[8] =
    i_neighbour4[10] = MB_TOP | MB_TOPRIGHT | ((i_neighbour & MB_LEFT) ? (MB_LEFT | MB_TOPLEFT) : 0);

    i_neighbour4[3] =
    i_neighbour4[7] =
    i_neighbour4[11] =
    i_neighbour4[13] =
    i_neighbour4[15] = MB_LEFT | MB_TOP | MB_TOPLEFT;

    i_neighbour4[5] = MB_LEFT | (i_neighbour & MB_TOPRIGHT)
                            | ((i_neighbour & MB_TOP) ? MB_TOP | MB_TOPLEFT : 0);

    i_neighbour4[6] =
    i_neighbour4[9] =
    i_neighbour4[12] =
    i_neighbour4[14] = MB_LEFT | MB_TOP | MB_TOPLEFT | MB_TOPRIGHT;

}

int mb_predict_intra4x4_mode(int idx, int intra4x4_pred_mode[SCAN8_SIZE])
{
    const int ma = intra4x4_pred_mode[x264_scan8[idx] - 1];
    const int mb = intra4x4_pred_mode[x264_scan8[idx] - 8];
    const int m  = SVC_MIN(x264_mb_pred_mode4x4_fix(ma),
                             x264_mb_pred_mode4x4_fix(mb));

    if( m < 0 )
        return I_PRED_4x4_DC;

    return m;
}

int mb_pred_mode4x4_valid(int first_block, int first_row, int idx, int i_mode)
{
    int b_a, b_b;
//    static const int needmb[16] =
//    {
//        MB_LEFT|MB_TOP, MB_TOP,
//        MB_LEFT,        MB_PRIVATE,
//        MB_TOP,         MB_TOP|MB_TOPRIGHT,
//        0,              MB_PRIVATE,
//        MB_LEFT,        0,
//        MB_LEFT,        MB_PRIVATE,
//        0,              MB_PRIVATE,
//        0,              MB_PRIVATE
//    };

//    b_a = (needmb[idx]&mb->i_neighbour&MB_LEFT) == (needmb[idx]&MB_LEFT);
//    b_b = (needmb[idx]&mb->i_neighbour&MB_TOP) == (needmb[idx]&MB_TOP);

    b_a = 1;
    b_b = 1;
    if (first_row && (idx == 0 || idx == 1 || idx == 4 || idx == 5)) {
        b_b = 0;
    }
    if (first_block && (idx == 0 || idx == 2 || idx == 8 || idx == 10)) {
        b_a = 0;
    }

    /* handle I_PRED_4x4_DC */
    if( i_mode == I_PRED_4x4_DC )
    {
        if( b_a && b_b )
        {
            return I_PRED_4x4_DC;
        }
        else if( b_a )
        {
            return I_PRED_4x4_DC_LEFT;
        }
        else if( b_b )
        {
            return I_PRED_4x4_DC_TOP;
        }
        return I_PRED_4x4_DC_128;
    }

    return i_mode;
}

int mb_pred_chromamode_valid(int first_block, int first_row, int idx, int i_mode)
{
    int b_a = 1;
    int b_b = 1;

    if (i_mode != I_PRED_CHROMA_DC)
        return i_mode;

    
    if (first_row && idx == 0) {
        b_b = 0;
    }
    if (first_block) {
        b_a = 0;
    }

    if (b_a && b_b) {
        return I_PRED_CHROMA_DC;
    }
    else if (b_a) {
        return I_PRED_CHROMA_DC_LEFT;
    }
    else if (b_b) {
        return I_PRED_CHROMA_DC_TOP;
    }
    return I_PRED_CHROMA_DC_128;

}

int mb_predict_non_zero_code(int idx, int non_zero_count[])
{
    const int za = non_zero_count[x264_scan8[idx] - 1];
    const int zb = non_zero_count[x264_scan8[idx] - 8];

    int i_ret = za + zb;

    if( i_ret < 0x80 )
    {
        i_ret = ( i_ret + 1 ) >> 1;
    }
    return i_ret & 0x7f;
}
// SAD

#define PIXEL_SAD_C( name, lx, ly ) \
int name( uint8_t *pix1, int i_stride_pix1,  \
                 uint8_t *pix2, int i_stride_pix2 ) \
{                                                   \
    int i_sum = 0;                                  \
    int x, y;                                       \
    for( y = 0; y < ly; y++ )                       \
    {                                               \
        for( x = 0; x < lx; x++ )                   \
        {                                           \
            i_sum += abs( pix1[x] - pix2[x] );      \
        }                                           \
        pix1 += i_stride_pix1;                      \
        pix2 += i_stride_pix2;                      \
    }                                               \
    return i_sum;                                   \
}


//PIXEL_SAD_C( pixel_sad_16x16, 16, 16 )
//PIXEL_SAD_C( pixel_sad_16x8,  16,  8 )
//PIXEL_SAD_C( pixel_sad_8x16,   8, 16 )
PIXEL_SAD_C( pixel_sad_8x8,    8,  8 )
//PIXEL_SAD_C( pixel_sad_8x4,    8,  4 )
//PIXEL_SAD_C( pixel_sad_4x8,    4,  8 )
PIXEL_SAD_C( pixel_sad_4x4,    4,  4)



// DCT

static inline void pixel_sub_wxh(int16_t *diff, int i_size, uint8_t *pix1,
        int i_pix1, uint8_t *pix2, int i_pix2)
{
    int y, x;
    for (y = 0; y < i_size; y++) {
        for (x = 0; x < i_size; x++) {
            diff[x + y * i_size] = pix1[x] - pix2[x];
        }
        pix1 += i_pix1;
        pix2 += i_pix2;
    }
}

void dct2x2dc( int16_t d[2][2] )
{
    int tmp[2][2];

    tmp[0][0] = d[0][0] + d[0][1];
    tmp[1][0] = d[0][0] - d[0][1];
    tmp[0][1] = d[1][0] + d[1][1];
    tmp[1][1] = d[1][0] - d[1][1];

    d[0][0] = tmp[0][0] + tmp[0][1];
    d[0][1] = tmp[1][0] + tmp[1][1];
    d[1][0] = tmp[0][0] - tmp[0][1];
    d[1][1] = tmp[1][0] - tmp[1][1];
}

void sub4x4_dct(int16_t dct[4][4], uint8_t *pix1, int i_pix1,
        uint8_t *pix2, int i_pix2)
{
    int16_t d[4][4];
    int16_t tmp[4][4];
    int i;

    pixel_sub_wxh((int16_t*) d, 4, pix1, i_pix1, pix2, i_pix2);

#if 0
    printf("Residual\n");
    for( i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            printf("%3d ", d[i][j]);
        }
        printf("\n");
    }
#endif

    for (i = 0; i < 4; i++) {
        const int s03 = d[i][0] + d[i][3];
        const int s12 = d[i][1] + d[i][2];
        const int d03 = d[i][0] - d[i][3];
        const int d12 = d[i][1] - d[i][2];

        tmp[0][i] = s03 + s12;
        tmp[1][i] = 2 * d03 + d12;
        tmp[2][i] = s03 - s12;
        tmp[3][i] = d03 - 2 * d12;
    }

    for (i = 0; i < 4; i++) {
        const int s03 = tmp[i][0] + tmp[i][3];
        const int s12 = tmp[i][1] + tmp[i][2];
        const int d03 = tmp[i][0] - tmp[i][3];
        const int d12 = tmp[i][1] - tmp[i][2];

        dct[0][i] = s03 + s12;
        dct[1][i] = 2 * d03 + d12;
        dct[2][i] = s03 - s12;
        dct[3][i] = d03 - 2 * d12;
    }

#if 0
    printf("DCT result\n");
    for( i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            printf("%d ", dct[i][j]);
        }
        printf("\n");
    }
#endif
}


void add4x4_idct(uint8_t *p_dst, int i_dst, int16_t dct[4][4])
{
    int16_t d[4][4];
    int16_t tmp[4][4];
    int x, y;
    int i;

#if 0
    printf("IDCT input\n");
    for( i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            printf("%d ", dct[i][j]);
        }
        printf("\n");
    }
#endif

    for (i = 0; i < 4; i++) {
        const int s02 = dct[i][0] + dct[i][2];
        const int d02 = dct[i][0] - dct[i][2];
        const int s13 = dct[i][1] + (dct[i][3] >> 1);
        const int d13 = (dct[i][1] >> 1) - dct[i][3];

        tmp[i][0] = s02 + s13;
        tmp[i][1] = d02 + d13;
        tmp[i][2] = d02 - d13;
        tmp[i][3] = s02 - s13;
    }

    for (i = 0; i < 4; i++) {
        const int s02 = tmp[0][i] + tmp[2][i];
        const int d02 = tmp[0][i] - tmp[2][i];
        const int s13 = tmp[1][i] + (tmp[3][i] >> 1);
        const int d13 = (tmp[1][i] >> 1) - tmp[3][i];

        d[0][i] = (s02 + s13 + 32) >> 6;
        d[1][i] = (d02 + d13 + 32) >> 6;
        d[2][i] = (d02 - d13 + 32) >> 6;
        d[3][i] = (s02 - s13 + 32) >> 6;
    }

#if 0
    printf("Before Add\n");
    for( i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            printf("%d ", d[i][j]);
        }
        printf("\n");
    }

#endif

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            p_dst[x] = clip_uint8(p_dst[x] + d[y][x]);
        }
        p_dst += i_dst;
    }
}

void sub8x8_dct( int16_t dct[4][4][4], uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    sub4x4_dct( dct[0], &pix1[0], i_pix1, &pix2[0], i_pix2 );
    sub4x4_dct( dct[1], &pix1[4], i_pix1, &pix2[4], i_pix2 );
    sub4x4_dct( dct[2], &pix1[4*i_pix1+0], i_pix1, &pix2[4*i_pix2+0], i_pix2 );
    sub4x4_dct( dct[3], &pix1[4*i_pix1+4], i_pix1, &pix2[4*i_pix2+4], i_pix2 );
}

void sub16x16_dct( int16_t dct[16][4][4], uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    sub8x8_dct( &dct[ 0], pix1, i_pix1, pix2, i_pix2 );
    sub8x8_dct( &dct[ 4], &pix1[8], i_pix1, &pix2[8], i_pix2 );
    sub8x8_dct( &dct[ 8], &pix1[8*i_pix1], i_pix1, &pix2[8*i_pix2], i_pix2 );
    sub8x8_dct( &dct[12], &pix1[8*i_pix1+8], i_pix1, &pix2[8*i_pix2+8], i_pix2 );
}

void add8x8_idct( uint8_t *p_dst, int i_dst, int16_t dct[4][4][4] )
{
    add4x4_idct( p_dst, i_dst,             dct[0] );
    add4x4_idct( &p_dst[4], i_dst,         dct[1] );
    add4x4_idct( &p_dst[4*i_dst+0], i_dst, dct[2] );
    add4x4_idct( &p_dst[4*i_dst+4], i_dst, dct[3] );
}

void add16x16_idct( uint8_t *p_dst, int i_dst, int16_t dct[16][4][4] )
{
    add8x8_idct( &p_dst[0], i_dst, &dct[0] );
    add8x8_idct( &p_dst[8], i_dst, &dct[4] );
    add8x8_idct( &p_dst[8*i_dst], i_dst, &dct[8] );
    add8x8_idct( &p_dst[8*i_dst+8], i_dst, &dct[12] );
}

// Quant tables and functions

static const int dequant4_scale[6][3] =
{
    { 10, 13, 16 },
    { 11, 14, 18 },
    { 13, 16, 20 },
    { 14, 18, 23 },
    { 16, 20, 25 },
    { 18, 23, 29 }
};
static const int quant4_scale[6][3] =
{
    { 13107, 8066, 5243 },
    { 11916, 7490, 4660 },
    { 10082, 6554, 4194 },
    {  9362, 5825, 3647 },
    {  8192, 5243, 3355 },
    {  7282, 4559, 2893 },
};

/* default quant matrices */
static const uint8_t x264_cqm_jvt4i[16] =
{
      6,13,20,28,
     13,20,28,32,
     20,28,32,37,
     28,32,37,42
};
static const uint8_t x264_cqm_jvt4p[16] =
{
    10,14,20,24,
    14,20,24,27,
    20,24,27,30,
    24,27,30,34
};

void create_quant_tables_4x4(int quant4_mf[6][4][4], int dequant4_mf[6][4][4])
{

    int def_quant4[6][16];
    int def_dequant4[6][16];
    int q, i;

    for (q = 0; q < 6; q++) {
        for (i = 0; i < 16; i++) {
            int j = (i & 1) + ((i >> 2) & 1);
            def_dequant4[q][i] = dequant4_scale[q][j];
            def_quant4[q][i] = quant4_scale[q][j];
        }
    }

    for (q = 0; q < 6; q++) {
        for (i = 0; i < 16; i++) {
            dequant4_mf[q][0][i] = def_dequant4[q][i] * x264_cqm_jvt4i[i];
            quant4_mf[q][0][i] = def_quant4[q][i] * 16 / x264_cqm_jvt4i[i];
        }
    }

}

void quant_4x4(int16_t dct[4][4], int quant_mf[6][4][4], int i_qscale,
        int b_intra)
{
    const int i_qbits = 15 + i_qscale / 6;
    const int i_mf = i_qscale % 6;
    const int f = (1 << i_qbits) / (b_intra ? 3 : 6);

    int x, y;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            if (dct[y][x] > 0)
                dct[y][x] = (f + dct[y][x] * quant_mf[i_mf][y][x]) >> i_qbits;
            else
                dct[y][x] = -((f - dct[y][x] * quant_mf[i_mf][y][x]) >> i_qbits);
        }
    }
}

void quant_2x2_dc( int16_t dct[2][2], int quant_mf[6][4][4], int i_qscale, int b_intra )
{
    int const i_qbits = 15 + i_qscale / 6;
    const int f2 = ( 2 << i_qbits ) / ( b_intra ? 3 : 6 );
    const int i_qmf = quant_mf[i_qscale%6][0][0];

    int x,y;
    for( y = 0; y < 2; y++ )
    {
        for( x = 0; x < 2; x++ )
        {
            if( dct[y][x] > 0 )
                dct[y][x] =( f2 + dct[y][x]  * i_qmf) >> ( 1 + i_qbits );
            else
                dct[y][x] = - ( ( f2 - dct[y][x]  * i_qmf ) >> (1 + i_qbits ) );
        }
    }
}

void dequant_4x4(int16_t dct[4][4], int dequant_mf[6][4][4],
        int i_qscale)
{
    const int i_mf = i_qscale % 6;
    const int i_qbits = i_qscale / 6 - 4;
    int y;

    if (i_qbits >= 0) {
        for (y = 0; y < 4; y++) {
            dct[y][0] = (dct[y][0] * dequant_mf[i_mf][y][0]) << i_qbits;
            dct[y][1] = (dct[y][1] * dequant_mf[i_mf][y][1]) << i_qbits;
            dct[y][2] = (dct[y][2] * dequant_mf[i_mf][y][2]) << i_qbits;
            dct[y][3] = (dct[y][3] * dequant_mf[i_mf][y][3]) << i_qbits;
        }
    }
    else {
        const int f = 1 << (-i_qbits - 1);
        for (y = 0; y < 4; y++) {
            dct[y][0] = (dct[y][0] * dequant_mf[i_mf][y][0] + f) >> (-i_qbits);
            dct[y][1] = (dct[y][1] * dequant_mf[i_mf][y][1] + f) >> (-i_qbits);
            dct[y][2] = (dct[y][2] * dequant_mf[i_mf][y][2] + f) >> (-i_qbits);
            dct[y][3] = (dct[y][3] * dequant_mf[i_mf][y][3] + f) >> (-i_qbits);
        }
    }
}

void dequant_2x2_dc( int16_t dct[2][2], int dequant_mf[6][4][4], int i_qscale )
{
    const int i_qbits = i_qscale/6 - 5;

    if( i_qbits >= 0 )
    {
        const int i_dmf = dequant_mf[i_qscale%6][0][0] << i_qbits;

        dct[0][0] *= i_dmf;
        dct[0][1] *= i_dmf;
        dct[1][0] *= i_dmf;
        dct[1][1] *= i_dmf;
    }
    else
    {
        const int i_dmf = dequant_mf[i_qscale%6][0][0];
        // chroma DC is truncated, not rounded

        dct[0][0] = ( dct[0][0] * i_dmf ) >> (-i_qbits);
        dct[0][1] = ( dct[0][1] * i_dmf ) >> (-i_qbits);
        dct[1][0] = ( dct[1][0] * i_dmf ) >> (-i_qbits);
        dct[1][1] = ( dct[1][1] * i_dmf ) >> (-i_qbits);
    }
}


// VLC decode
static int  vlc_table_realloc(struct x264_vlc_table_t *table, int i_size )
{
    int i_index;

    i_index = table->i_lookup;

    table->i_lookup += i_size;
    table->lookup = (vlc_lookup_t *) realloc( table->lookup, sizeof( vlc_lookup_t ) * table->i_lookup );

    return( i_index );
}

static int vlc_table_create_part(struct x264_vlc_table_t *table, const vlc_t *vlc, int i_lookup_bits, int i_nb_vlc, int i_prefix_code, int i_prefix_length )
{
    int i;
    int i_nb_lookup;
    vlc_lookup_t *lookup;
    int i_table_index;

    i_nb_lookup = 1 << i_lookup_bits;

    i_table_index = vlc_table_realloc( table, i_nb_lookup );
    lookup = &table->lookup[i_table_index];

    for( i = 0; i < i_nb_lookup; i++ )
    {
        lookup[i].i_value  = -1;
        lookup[i].i_size = 0;
    }

    for( i = 0; i < i_nb_vlc; i++ )
    {
        int i_bits;
        if( vlc[i].i_size <= 0 )
        {
            continue;
        }

        i_bits = vlc[i].i_size - i_prefix_length;
        if( i_bits > 0 && ( vlc[i].i_bits >> i_bits ) == i_prefix_code )
        {
            if( i_bits <= i_lookup_bits )
            {
                int i_lookup_index;
                int nb;

                i_lookup_index = ( vlc[i].i_bits << ( i_lookup_bits - i_bits ) )%i_nb_lookup;
                nb = 1 << ( i_lookup_bits - i_bits );
                for( nb = 0; nb < (1 << ( i_lookup_bits - i_bits)); nb++ )
                {
                    lookup[i_lookup_index].i_value = i; /* vlc[i].i_value; */
                    lookup[i_lookup_index].i_size = i_bits;
                    i_lookup_index++;
                }
            }
            else
            {
                int i_bits_max;
                int i_lookup_index;
                /* need another table */
                i_lookup_index = ( vlc[i].i_bits >> (i_bits - i_lookup_bits ) )%i_nb_lookup;

                i_bits_max =  -lookup[i_lookup_index].i_size;
                if( i_bits_max < i_bits - i_lookup_bits )
                {
                    i_bits_max = i_bits - i_lookup_bits;
                }
                lookup[i_lookup_index].i_size = -i_bits_max;
            }
        }
    }

    /* create other level table */
    for( i = 0; i < i_nb_lookup; i++ )
    {
        if( lookup[i].i_size < 0 )
        {
            int i_bits;
            int i_index;
            i_bits = -lookup[i].i_size;
            if( i_bits > i_lookup_bits )
            {
                lookup[i].i_size = -i_lookup_bits;
                i_bits = i_lookup_bits;
            }

            i_index = vlc_table_create_part( table, vlc, i_bits, i_nb_vlc,
                                             (i_prefix_code << i_lookup_bits)|i,
                                              i_lookup_bits+i_prefix_length );
            lookup = &table->lookup[i_table_index]; // reallocated
            lookup[i].i_value = i_index;
        }
    }

    return( i_table_index );
}


struct x264_vlc_table_t *vlc_table_lookup_new( const vlc_t *vlc, int i_vlc, int i_lookup_bits )
{
    struct x264_vlc_table_t *table = (struct x264_vlc_table_t *) aligned_malloc(sizeof(struct x264_vlc_table_t ), 16);

    table->i_lookup_bits = i_lookup_bits;
    table->i_lookup = 0;
    table->lookup   = NULL;

    vlc_table_create_part( table, vlc, i_lookup_bits, i_vlc, 0, 0 );

    return table;
}

void vlc_table_lookup_delete(struct x264_vlc_table_t *table )
{
    free( table->lookup );
    aligned_free( table );
}
