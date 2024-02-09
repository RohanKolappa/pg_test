/*
 * dct.cpp
 *
 *  Created on: Oct 28, 2009
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include <math.h>
#include "dct.hh"

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

bool CFDCT::m_bIsInitialized = false;

double CFDCT::m_cos[8][8];

#define RANGE_CHECK(val) (val) > 255 ? 255 : (val) < 0 ? 0 : (val)

extern "C" {
    extern void _ff_fdct_sse2(short *block);
    extern void _ff_fdct_mmx(short *block);
    extern void _ff_fdct_mmx2(short *block);
}

void CFDCT::FDCT(short* block)
{

    // We now have somewhat faster DCT implementation
    FDCTReal(block);

    // TODO: Keep looking for a faster alternative

    // Original implementation from comp_utils.c can be invoked
    // by un-commenting the following

    //FDCTOrig(block);

    // This is another test implementation
    // It is faster but not as accurate
    //FDCTExp(block, 8);

    //_ff_fdct_mmx(block);
}


void CFDCT::FDCTFixed(short* block)
{
    // This is used for testing since it produces the
    // exact same result independent of optimization level, platform, os, etc...
    FDCTOrig(block);
}

void CFDCT::IDCT(short* block)
{
    IDCTReal(block);
}

void CFDCT::InitFDCT(void)
{
    int i, j;
    double s;

    for (i = 0; i < 8; i++) {
        s = (i == 0) ? sqrt(0.125) : 0.5;

        for (j = 0; j < 8; j++)
            m_cos[i][j] = s * cos((PI / 8.0) * i * (j + 0.5));
    }
}

void CFDCT::FDCTOrig(short* block)
{
    int i, j, k;
    double s;
    double tmp[64];

    // There is probably a better way to do this...
    if (!m_bIsInitialized) {
        InitFDCT();
        m_bIsInitialized = true;
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            s = 0.0;
            for (k = 0; k < 8; k++)
                s += m_cos[j][k] * block[8 * i + k];
            tmp[8 * i + j] = s;
        }
    }
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) {
            s = 0.0;
            for (k = 0; k < 8; k++)
                s += m_cos[i][k] * tmp[8 * k + j];
            block[8 * i + j] = (int) floor(s + 0.499999);
        }
    }

}


void CFDCT::FDCTReal(short *block) {

     double *a[8];
     double aarr[8][8];
     int i, j1, j2;


     // The FDCT implementation using ddct8x8s() (see below)
     // is still faster in spite of having to copy back and forth
     // the "short" input to "double" array expected by ddct8x8s()

     // Set up array memory
     for (i = 0; i < 8; i++) {
         a[i] = aarr[i];
     }

     // Copy in data
     for (j1 = 0; j1 < 8; j1++) {
         for (j2 = 0; j2 < 8; j2++) {
             a[j1][j2] = block[j1 * 8 + j2];
         }
     }

     // Do the FFT
     ddct8x8s(-1, a);

     // Copy result back
     for (j1 = 0; j1 < 8; j1++) {
         for (j2 = 0; j2 < 8; j2++) {
             block[j1 * 8 + j2] = (int) RINT(a[j1][j2]);
         }
     }
}

void CFDCT::IDCTReal(short *block) {

     double *a[8];
     double aarr[8][8];
     int i, j1, j2;


     // The FDCT implementation using ddct8x8s() (see below)
     // is still faster in spite of having to copy back and forth
     // the "short" input to "double" array expected by ddct8x8s()

     // Set up array memory
     for (i = 0; i < 8; i++) {
         a[i] = aarr[i];
     }

     // Copy in data
     for (j1 = 0; j1 < 8; j1++) {
         for (j2 = 0; j2 < 8; j2++) {
             a[j1][j2] = block[j1 * 8 + j2];
         }
     }

     // Do the Inverse FFT
     ddct8x8s(1, a);

     // Copy result back
     for (j1 = 0; j1 < 8; j1++) {
         for (j2 = 0; j2 < 8; j2++) {
             block[j1 * 8 + j2] = (int) RINT(a[j1][j2]);
         }
     }
}

// The following implementation is from fft2d library

/*
Short Discrete Cosine Transform
    data length :8x8, 16x16
    method      :row-column, radix 4 FFT
functions
    ddct8x8s  : 8x8 DCT
    ddct16x16s: 16x16 DCT
function prototypes
    void ddct8x8s(int isgn, double **a);
    void ddct16x16s(int isgn, double **a);
*/


/*
-------- 8x8 DCT (Discrete Cosine Transform) / Inverse of DCT --------
    [definition]
        <case1> Normalized 8x8 IDCT
            C[k1][k2] = (1/4) * sum_j1=0^7 sum_j2=0^7
                            a[j1][j2] * s[j1] * s[j2] *
                            cos(pi*j1*(k1+1/2)/8) *
                            cos(pi*j2*(k2+1/2)/8), 0<=k1<8, 0<=k2<8
                            (s[0] = 1/sqrt(2), s[j] = 1, j > 0)
        <case2> Normalized 8x8 DCT
            C[k1][k2] = (1/4) * s[k1] * s[k2] * sum_j1=0^7 sum_j2=0^7
                            a[j1][j2] *
                            cos(pi*(j1+1/2)*k1/8) *
                            cos(pi*(j2+1/2)*k2/8), 0<=k1<8, 0<=k2<8
                            (s[0] = 1/sqrt(2), s[j] = 1, j > 0)
    [usage]
        <case1>
            ddct8x8s(1, a);
        <case2>
            ddct8x8s(-1, a);
    [parameters]
        a[0...7][0...7] :input/output data (double **)
                         output data
                             a[k1][k2] = C[k1][k2], 0<=k1<8, 0<=k2<8
*/


/* Cn_kR = sqrt(2.0/n) * cos(pi/2*k/n) */
/* Cn_kI = sqrt(2.0/n) * sin(pi/2*k/n) */
/* Wn_kR = cos(pi/2*k/n) */
/* Wn_kI = sin(pi/2*k/n) */
#define C8_1R   0.49039264020161522456
#define C8_1I   0.09754516100806413392
#define C8_2R   0.46193976625564337806
#define C8_2I   0.19134171618254488586
#define C8_3R   0.41573480615127261854
#define C8_3I   0.27778511650980111237
#define C8_4R   0.35355339059327376220
#define W8_4R   0.70710678118654752440


void CFDCT::ddct8x8s(int isgn, double **a)
{
    int j;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    double xr, xi;

    if (isgn < 0) {
        for (j = 0; j <= 7; j++) {
            x0r = a[0][j] + a[7][j];
            x1r = a[0][j] - a[7][j];
            x0i = a[2][j] + a[5][j];
            x1i = a[2][j] - a[5][j];
            x2r = a[4][j] + a[3][j];
            x3r = a[4][j] - a[3][j];
            x2i = a[6][j] + a[1][j];
            x3i = a[6][j] - a[1][j];
            xr = x0r + x2r;
            xi = x0i + x2i;
            a[0][j] = C8_4R * (xr + xi);
            a[4][j] = C8_4R * (xr - xi);
            xr = x0r - x2r;
            xi = x0i - x2i;
            a[2][j] = C8_2R * xr - C8_2I * xi;
            a[6][j] = C8_2R * xi + C8_2I * xr;
            xr = W8_4R * (x1i - x3i);
            x1i = W8_4R * (x1i + x3i);
            x3i = x1i - x3r;
            x1i += x3r;
            x3r = x1r - xr;
            x1r += xr;
            a[1][j] = C8_1R * x1r - C8_1I * x1i;
            a[7][j] = C8_1R * x1i + C8_1I * x1r;
            a[3][j] = C8_3R * x3r - C8_3I * x3i;
            a[5][j] = C8_3R * x3i + C8_3I * x3r;
        }
        for (j = 0; j <= 7; j++) {
            x0r = a[j][0] + a[j][7];
            x1r = a[j][0] - a[j][7];
            x0i = a[j][2] + a[j][5];
            x1i = a[j][2] - a[j][5];
            x2r = a[j][4] + a[j][3];
            x3r = a[j][4] - a[j][3];
            x2i = a[j][6] + a[j][1];
            x3i = a[j][6] - a[j][1];
            xr = x0r + x2r;
            xi = x0i + x2i;
            a[j][0] = C8_4R * (xr + xi);
            a[j][4] = C8_4R * (xr - xi);
            xr = x0r - x2r;
            xi = x0i - x2i;
            a[j][2] = C8_2R * xr - C8_2I * xi;
            a[j][6] = C8_2R * xi + C8_2I * xr;
            xr = W8_4R * (x1i - x3i);
            x1i = W8_4R * (x1i + x3i);
            x3i = x1i - x3r;
            x1i += x3r;
            x3r = x1r - xr;
            x1r += xr;
            a[j][1] = C8_1R * x1r - C8_1I * x1i;
            a[j][7] = C8_1R * x1i + C8_1I * x1r;
            a[j][3] = C8_3R * x3r - C8_3I * x3i;
            a[j][5] = C8_3R * x3i + C8_3I * x3r;
        }
    } else {
        for (j = 0; j <= 7; j++) {
            x1r = C8_1R * a[1][j] + C8_1I * a[7][j];
            x1i = C8_1R * a[7][j] - C8_1I * a[1][j];
            x3r = C8_3R * a[3][j] + C8_3I * a[5][j];
            x3i = C8_3R * a[5][j] - C8_3I * a[3][j];
            xr = x1r - x3r;
            xi = x1i + x3i;
            x1r += x3r;
            x3i -= x1i;
            x1i = W8_4R * (xr + xi);
            x3r = W8_4R * (xr - xi);
            xr = C8_2R * a[2][j] + C8_2I * a[6][j];
            xi = C8_2R * a[6][j] - C8_2I * a[2][j];
            x0r = C8_4R * (a[0][j] + a[4][j]);
            x0i = C8_4R * (a[0][j] - a[4][j]);
            x2r = x0r - xr;
            x2i = x0i - xi;
            x0r += xr;
            x0i += xi;
            a[0][j] = x0r + x1r;
            a[7][j] = x0r - x1r;
            a[2][j] = x0i + x1i;
            a[5][j] = x0i - x1i;
            a[4][j] = x2r - x3i;
            a[3][j] = x2r + x3i;
            a[6][j] = x2i - x3r;
            a[1][j] = x2i + x3r;
        }
        for (j = 0; j <= 7; j++) {
            x1r = C8_1R * a[j][1] + C8_1I * a[j][7];
            x1i = C8_1R * a[j][7] - C8_1I * a[j][1];
            x3r = C8_3R * a[j][3] + C8_3I * a[j][5];
            x3i = C8_3R * a[j][5] - C8_3I * a[j][3];
            xr = x1r - x3r;
            xi = x1i + x3i;
            x1r += x3r;
            x3i -= x1i;
            x1i = W8_4R * (xr + xi);
            x3r = W8_4R * (xr - xi);
            xr = C8_2R * a[j][2] + C8_2I * a[j][6];
            xi = C8_2R * a[j][6] - C8_2I * a[j][2];
            x0r = C8_4R * (a[j][0] + a[j][4]);
            x0i = C8_4R * (a[j][0] - a[j][4]);
            x2r = x0r - xr;
            x2i = x0i - xi;
            x0r += xr;
            x0i += xi;
            a[j][0] = x0r + x1r;
            a[j][7] = x0r - x1r;
            a[j][2] = x0i + x1i;
            a[j][5] = x0i - x1i;
            a[j][4] = x2r - x3i;
            a[j][3] = x2r + x3i;
            a[j][6] = x2i - x3r;
            a[j][1] = x2i + x3r;
        }
    }
}



// *********************************************************************************************************
// *                        FDCT.C                                                                         *
// *                                                                                                       *
// * Forward Discrete Cosine Transform                                                                     *
// * Used on 8x8 image blocks                                                                              *
// * to reassemble blocks in order to ease quantization compressing image information on the more          *
// * significant frequency components                                                                      *
// *                                                                                                       *
// *  Expected Result -> short int block[64]= { 699,164,-51,-16, 31,-15,-19,  8,                           *
// *                                             71, 14,-61, -2,-11,-12,  7, 12,                           *
// *                                            -58,-55, 13, 28,-20, -7, 14,-18,                           *
// *                                             29, 22,  3,  3,-11,  7, 11,-22,                           *
// *                                             -1,-28,-27, 10,  0, -7, 11,  6,                           *
// *                                              7,  6, 21, 21,-10, -8,  2,-14,                           *
// *                                              1, -7,-15,-15,-10, 15, 16,-10,                           *
// *                                              0, -1,  0, 15,  4,-13, -5,  4 };                         *
// *                                                                                                       *
// *  Exadecimal results: Block -> 02bb00a4 ffcdfff0 001ffff1 ffed0008 0047000e ffc3fffe 000bfff4 0007000c *
// *                               ffc6ffc9 000d001c ffecfff9 000effee 001d0016 00030003 fff50007 000bffea *
// *                               ffffffe4 ffe5000a 0000fff9 000b0006 00070006 00150015 fff6fff8 0002fff2 *
// *                               0001fff9 fff1fff1 fff6000f 0010fff6 0000ffff 0000000f 0004fff3 fffb0004 *
// *                                                                                                       *
// *  Number of clock cycles (with these inputs) -> 2132                                                   *
// *********************************************************************************************************

#ifdef IO
#include "libp.c"
#include "arith.c"
#include "string.c"
#endif

// Cosine Transform Coefficients

#define W1 2841                 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676                 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408                 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609                 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108                 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565                  /* 2048*sqrt(2)*cos(7*pi/16) */

// Other FDCT Parameters
#define CONST_BITS  13
#define PASS1_BITS  2


/* Fast Discrete Cosine Transform */

void CFDCT::FDCTExp(short int *blk, int lx)
{
  int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  int tmp10, tmp11, tmp12, tmp13;
  int z1, z2, z3, z4, z5;
  int i;
  short int *block;

  int constant;

  /* Pass 1: process rows. */
  /* Note results are scaled up by sqrt(8) compared to a true DCT; */
  /* furthermore, we scale the results by 2**PASS1_BITS. */

  block=blk;

  for (i=0; i<8; i++)
  {
    tmp0 = block[0] + block[7];
    tmp7 = block[0] - block[7];
    tmp1 = block[1] + block[6];
    tmp6 = block[1] - block[6];
    tmp2 = block[2] + block[5];
    tmp5 = block[2] - block[5];
    tmp3 = block[3] + block[4];
    tmp4 = block[3] - block[4];

    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    block[0] = ((tmp10+tmp11) << PASS1_BITS);
    block[4] = ((tmp10-tmp11) << PASS1_BITS);

    constant= 4433;
    z1 = (tmp12 + tmp13) * constant;
    constant= 6270;
    block[2] = (z1 + (tmp13 * constant)) >> (CONST_BITS-PASS1_BITS);
    constant= -15137;
    block[6] = (z1 + (tmp12 * constant)) >> (CONST_BITS-PASS1_BITS);

    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents cos(K*pi/16).
     * i0..i3 in the paper are tmp4..tmp7 here.
     */

    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    constant= 9633;
    z5 = ((z3 + z4) * constant); /* sqrt(2) * c3 */

    constant= 2446;
    tmp4 = (tmp4 * constant); /* sqrt(2) * (-c1+c3+c5-c7) */
    constant= 16819;
    tmp5 = (tmp5 * constant); /* sqrt(2) * ( c1+c3-c5+c7) */
    constant= 25172;
    tmp6 = (tmp6 * constant); /* sqrt(2) * ( c1+c3+c5-c7) */
    constant= 12299;
    tmp7 = (tmp7 * constant); /* sqrt(2) * ( c1+c3-c5-c7) */
    constant= -7373;
    z1 = (z1 * constant); /* sqrt(2) * (c7-c3) */
    constant= -20995;
    z2 = (z2 * constant); /* sqrt(2) * (-c1-c3) */
    constant= -16069;
    z3 = (z3 * constant); /* sqrt(2) * (-c3-c5) */
    constant= -3196;
    z4 = (z4 * constant); /* sqrt(2) * (c5-c3) */

    z3 += z5;
    z4 += z5;

    block[7] = (tmp4 + z1 + z3) >> (CONST_BITS-PASS1_BITS);
    block[5] = (tmp5 + z2 + z4) >> (CONST_BITS-PASS1_BITS);
    block[3] = (tmp6 + z2 + z3) >> (CONST_BITS-PASS1_BITS);
    block[1] = (tmp7 + z1 + z4) >> (CONST_BITS-PASS1_BITS);



    /* advance to next row */
    block += lx;

  }

  /* Pass 2: process columns. */

  block=blk;

  for (i = 0; i<8; i++)
    {
    tmp0 = block[0] + block[7*lx];
    tmp7 = block[0] - block[7*lx];
    tmp1 = block[lx] + block[6*lx];
    tmp6 = block[lx]- block[6*lx];
    tmp2 = block[2*lx] + block[5*lx];
    tmp5 = block[2*lx] - block[5*lx];
    tmp3 = block[3*lx] + block[4*lx];
    tmp4 = block[3*lx] - block[4*lx];

    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    block[0] = (tmp10 + tmp11) >> (PASS1_BITS+3);
    block[4*lx] = (tmp10 - tmp11) >> (PASS1_BITS+3);

    constant = 4433;
    z1 = ((tmp12 + tmp13) * constant);
    constant= 6270;
    block[2*lx] = (z1 + (tmp13 * constant)) >> (CONST_BITS+PASS1_BITS+3);
    constant=-15137;
    block[6*lx] = (z1 + (tmp12 * constant)) >> (CONST_BITS+PASS1_BITS+3);

    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents cos(K*pi/16).
     * i0..i3 in the paper are tmp4..tmp7 here.
     */

    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    constant=9633;
    z5 = ((z3 + z4) * constant); /* sqrt(2) * c3 */

    constant=2446;
    tmp4 = (tmp4 * constant); /* sqrt(2) * (-c1+c3+c5-c7) */
    constant=16819;
    tmp5 = (tmp5 * constant); /* sqrt(2) * ( c1+c3-c5+c7) */
    constant=25172;
    tmp6 = (tmp6 * constant); /* sqrt(2) * ( c1+c3+c5-c7) */
    constant=12299;
    tmp7 = (tmp7 * constant); /* sqrt(2) * ( c1+c3-c5-c7) */
    constant=-7373;
    z1 = (z1 * constant); /* sqrt(2) * (c7-c3) */
    constant= -20995;
    z2 = (z2 * constant); /* sqrt(2) * (-c1-c3) */
    constant=-16069;
    z3 = (z3 * constant); /* sqrt(2) * (-c3-c5) */
    constant=-3196;
    z4 = (z4 * constant); /* sqrt(2) * (c5-c3) */

    z3 += z5;
    z4 += z5;

    block[7*lx] = (tmp4 + z1 + z3) >> (CONST_BITS+PASS1_BITS+3);
    block[5*lx] = (tmp5 + z2 + z4) >> (CONST_BITS+PASS1_BITS+3);
    block[3*lx] = (tmp6 + z2 + z3) >> (CONST_BITS+PASS1_BITS+3);
    block[lx] =  (tmp7 + z1 + z4) >> (CONST_BITS+PASS1_BITS+3);

    /* advance to next column */
    block++;
  }
}

typedef long M_INT32;

#define DCTSIZE 8

#define FIX_0_298631336  ((M_INT32)  2446)    /* FIX(0.298631336) */
#define FIX_0_390180644  ((M_INT32)  3196)    /* FIX(0.390180644) */
#define FIX_0_541196100  ((M_INT32)  4433)    /* FIX(0.541196100) */
#define FIX_0_765366865  ((M_INT32)  6270)    /* FIX(0.765366865) */
#define FIX_0_899976223  ((M_INT32)  7373)    /* FIX(0.899976223) */
#define FIX_1_175875602  ((M_INT32)  9633)    /* FIX(1.175875602) */
#define FIX_1_501321110  ((M_INT32)  12299)   /* FIX(1.501321110) */
#define FIX_1_847759065  ((M_INT32)  15137)   /* FIX(1.847759065) */
#define FIX_1_961570560  ((M_INT32)  16069)   /* FIX(1.961570560) */
#define FIX_2_053119869  ((M_INT32)  16819)   /* FIX(2.053119869) */
#define FIX_2_562915447  ((M_INT32)  20995)   /* FIX(2.562915447) */
#define FIX_3_072711026  ((M_INT32)  25172)   /* FIX(3.072711026) */


#define MULTIPLY(var,const)  ((var) * (const))
#define RIGHT_SHIFT(x,shft) ((x) >> (shft))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)

#define ONE ((M_INT32) 1)

// Do a 4x4 inverse dct transposing and saturating to  0..255 at the same time
void CFDCT::IDCT4x4_Add128_Transpose(short * coef_block, unsigned char *dest,
        int destStride)
{
    M_INT32 tmp0, tmp2, tmp10, tmp12;
    M_INT32 z1, z2, z3;
    short * inptr;
    int * wsptr;
    unsigned char * destptr;
    int ctr;
    int workspace[4 * 4]; /* buffers data between passes */

    /* Pass 1: process columns from input, store into work array. */

    inptr = coef_block;
    wsptr = workspace;
    for (ctr = 0; ctr < 4; ctr++, inptr++, wsptr++) {
        /* Even part */

        tmp0 = inptr[DCTSIZE * 0];
        tmp2 = inptr[DCTSIZE * 2];

        tmp10 = (tmp0 + tmp2) << PASS1_BITS;
        tmp12 = (tmp0 - tmp2) << PASS1_BITS;

        /* Odd part */
        /* Same rotation as in the even part of the 8x8 LL&M IDCT */

        z2 = inptr[DCTSIZE * 1];
        z3 = inptr[DCTSIZE * 3];

        z1 = MULTIPLY(z2 + z3, FIX_0_541196100); /* c6 */
        /* Add fudge factor here for final descale. */
        z1 += ONE << (CONST_BITS - PASS1_BITS - 1);
        tmp0 = RIGHT_SHIFT(z1 + MULTIPLY(z2, FIX_0_765366865), /* c2-c6 */
                CONST_BITS-PASS1_BITS);
        tmp2 = RIGHT_SHIFT(z1 - MULTIPLY(z3, FIX_1_847759065), /* c2+c6 */
                CONST_BITS-PASS1_BITS);

        /* Final output stage */

        wsptr[4 * 0] = (int) (tmp10 + tmp0);
        wsptr[4 * 3] = (int) (tmp10 - tmp0);
        wsptr[4 * 1] = (int) (tmp12 + tmp2);
        wsptr[4 * 2] = (int) (tmp12 - tmp2);
    }

    /* Pass 2: process 4 rows from work array, store into output array. */

    wsptr = workspace;
    for (ctr = 0; ctr < 4; ctr++) {

        /* Even part */

        /* Add fudge factor here for final descale. */
        tmp0 = (M_INT32) wsptr[0] + (ONE << (PASS1_BITS + 2));
        tmp2 = (M_INT32) wsptr[2];

        tmp10 = (tmp0 + tmp2) << CONST_BITS;
        tmp12 = (tmp0 - tmp2) << CONST_BITS;

        /* Odd part */
        /* Same rotation as in the even part of the 8x8 LL&M IDCT */

        z2 = (M_INT32) wsptr[1];
        z3 = (M_INT32) wsptr[3];

        z1 = MULTIPLY(z2 + z3, FIX_0_541196100); /* c6 */
        tmp0 = z1 + MULTIPLY(z2, FIX_0_765366865); /* c2-c6 */
        tmp2 = z1 - MULTIPLY(z3, FIX_1_847759065); /* c2+c6 */

        /* Final output stage */

        destptr = dest + ctr;

        *destptr = RANGE_CHECK((int) RIGHT_SHIFT(tmp10 + tmp0,
                        CONST_BITS+PASS1_BITS+3) + 128);
        *(destptr + 3 * destStride)
                = RANGE_CHECK((int) RIGHT_SHIFT(tmp10 - tmp0,
                                CONST_BITS+PASS1_BITS+3) + 128);
        *(destptr + 1 * destStride)
                = RANGE_CHECK((int) RIGHT_SHIFT(tmp12 + tmp2,
                                CONST_BITS+PASS1_BITS+3) + 128);
        *(destptr + 2 * destStride)
                = RANGE_CHECK((int) RIGHT_SHIFT(tmp12 - tmp2,
                                CONST_BITS+PASS1_BITS+3) + 128);

        wsptr += 4; /* advance pointer to next row */
    }
}


//#define DESCALE(x,y ) x

// Do a 2x2 inverse dct transposing and saturating to  0..255 at the same time
void CFDCT::IDCT2x2_Add128_Transpose(short * coef_block, unsigned char *dest,
        int destStride)
{
    M_INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
    //JSAMPROW outptr;

    /* Pass 1: process columns from input. */

    /* Column 0 */
    tmp4 = coef_block[DCTSIZE * 0];
    tmp5 = coef_block[DCTSIZE * 1];

    tmp0 = tmp4 + tmp5;
    tmp2 = tmp4 - tmp5;

    /* Column 1 */
    tmp4 = coef_block[DCTSIZE * 0 + 1];
    tmp5 = coef_block[DCTSIZE * 1 + 1];

    tmp1 = tmp4 + tmp5;
    tmp3 = tmp4 - tmp5;

    /* Pass 2: process 2 rows, store into output array. */

    /* Row 0 */
    //outptr = output_buf[0] + output_col;

    *(dest) =
    /* outptr[0] */RANGE_CHECK((int) DESCALE(tmp0 + tmp1, 3) + 128);
    *(dest + destStride) =
    /* outptr[1] */RANGE_CHECK((int) DESCALE(tmp0 - tmp1, 3) + 128);

    /* Row 1 */

    // outptr = output_buf[1] + output_col;

    *(dest + 1) =
    /* outptr[0] */RANGE_CHECK((int) DESCALE(tmp2 + tmp3, 3) + 128);
    *(dest + destStride + 1) =
    /* outptr[1] */RANGE_CHECK((int) DESCALE(tmp2 - tmp3, 3) + 128);
}


