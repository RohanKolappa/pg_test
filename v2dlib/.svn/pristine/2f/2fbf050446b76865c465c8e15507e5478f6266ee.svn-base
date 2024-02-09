/*
 * sliceencoder.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: rkale
 */


#include "sliceencoder.hh"
#include "standardsliceencoder.hh"
#include "h264sliceencoder.hh"
#include "svcsliceencoder.hh"
#include "rgbtoyuv.hh"
#include "dct.hh"
#include "ipputils.h"
#include "imagemanip.h"
#include "bitstreamwriter.hh"

#include "svc.h"
#include "cavlc.hh"
#include "memory_tracker.h"

//#define DEBUG_ENCODER

#ifdef DEBUG_ENCODER
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif


//#define ORIG_FORMULA

#ifdef ORIG_FORMULA
unsigned char CSliceEncoder::quant_mat_trans[64] =
{
  8, 16, 16, 16, 16, 32, 32, 32,
 16, 16, 16, 16, 32, 32, 32, 32,
 16, 16, 32, 32, 32, 32, 32, 32,
 16, 16, 32, 32, 32, 32 ,32, 32,
 32, 32, 32, 32, 32, 32, 32, 32,
 32, 32, 32, 32, 32, 32, 32, 64,
 32, 32, 32, 32, 32, 32, 64, 64,
 32, 32, 32, 32, 32, 64, 64, 64
};
#else
unsigned char CSliceEncoder::quant_mat_trans[64] =
{
   8, 16, 16, 16, 32, 32, 32, 32,
  16, 16, 16, 16, 32, 32, 32, 32,
  16, 16, 32, 32, 32, 32, 32, 32,
  16, 16, 32, 32, 32, 32, 32, 32,
  16, 32, 32, 32, 32, 32, 32, 32,
  32, 32, 32, 32, 32, 32, 32, 64,
  32, 32, 32, 32, 32, 32, 64, 64,
  32, 32, 32, 32, 32, 64, 64, 64
  };
#endif

//Quantization Matrix used in FPGA
// 8, 16, 16, 16, 32, 32, 32, 32,
//16, 16, 16, 16, 32, 32, 32, 32,
//16, 16, 32, 32, 32, 32, 32, 32,
//16, 16, 32, 32, 32, 32, 32, 32,
//16, 32, 32, 32, 32, 32, 32, 32,
//32, 32, 32, 32, 32, 32, 32, 64,
//32, 32, 32, 32, 32, 32, 64, 64,
//32, 32, 32, 32, 32, 64, 64, 64

#ifdef ORIG_FORMULA
unsigned char CSliceEncoder::quant_mat_trans_mult[64] =
{
 3, 4, 4, 4, 4, 5, 5, 5,
 4, 4, 4, 4, 5, 5, 5, 5,
 4, 4, 5, 5, 5, 5, 5, 5,
 4, 4, 5, 5, 5, 5 ,5, 5,
 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 6,
 5, 5, 5, 5, 5, 5, 6, 6,
 5, 5, 5, 5, 5, 6, 6, 6
};
#else
unsigned char CSliceEncoder::quant_mat_trans_mult[64] =
{
 3, 4, 4, 4, 5, 5, 5, 5,
 4, 4, 4, 4, 5, 5, 5, 5,
 4, 4, 5, 5, 5, 5, 5, 5,
 4, 4, 5, 5, 5, 5 ,5, 5,
 4, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 6,
 5, 5, 5, 5, 5, 5, 6, 6,
 5, 5, 5, 5, 5, 6, 6, 6
};
#endif

// Unused??
VLCtable CSliceEncoder::dct_code_tab1[2][40] =
{
 /* run = 0, level = 1...40 */
 {
  {0x03, 2}, {0x04, 4}, {0x05, 5}, {0x06, 7},
  {0x26, 8}, {0x21, 8}, {0x0a,10}, {0x1d,12},
  {0x18,12}, {0x13,12}, {0x10,12}, {0x1a,13},
  {0x19,13}, {0x18,13}, {0x17,13}, {0x1f,14},
  {0x1e,14}, {0x1d,14}, {0x1c,14}, {0x1b,14},
  {0x1a,14}, {0x19,14}, {0x18,14}, {0x17,14},
  {0x16,14}, {0x15,14}, {0x14,14}, {0x13,14},
  {0x12,14}, {0x11,14}, {0x10,14}, {0x18,15},
  {0x17,15}, {0x16,15}, {0x15,15}, {0x14,15},
  {0x13,15}, {0x12,15}, {0x11,15}, {0x10,15}
 },
 /* run = 1, level = 1...18 */
 {
  {0x03, 3}, {0x06, 6}, {0x25, 8}, {0x0c,10},
  {0x1b,12}, {0x16,13}, {0x15,13}, {0x1f,15},
  {0x1e,15}, {0x1d,15}, {0x1c,15}, {0x1b,15},
  {0x1a,15}, {0x19,15}, {0x13,16}, {0x12,16},
  {0x11,16}, {0x10,16}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}
 }
};

VLCtable CSliceEncoder::dct_code_tab2[30][5] =
{
  /* run = 2...31, level = 1...5 */
  {{0x05, 4}, {0x04, 7}, {0x0b,10}, {0x14,12}, {0x14,13}},
  {{0x07, 5}, {0x24, 8}, {0x1c,12}, {0x13,13}, {0x00, 0}},
  {{0x06, 5}, {0x0f,10}, {0x12,12}, {0x00, 0}, {0x00, 0}},
  {{0x07, 6}, {0x09,10}, {0x12,13}, {0x00, 0}, {0x00, 0}},
  {{0x05, 6}, {0x1e,12}, {0x14,16}, {0x00, 0}, {0x00, 0}},
  {{0x04, 6}, {0x15,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x07, 7}, {0x11,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x05, 7}, {0x11,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x27, 8}, {0x10,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x23, 8}, {0x1a,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x22, 8}, {0x19,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x20, 8}, {0x18,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x0e,10}, {0x17,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x0d,10}, {0x16,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x08,10}, {0x15,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1a,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x19,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x17,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x16,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1e,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1d,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1c,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1b,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1e,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1d,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1c,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1b,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}}
};

/* Table B-15, DCT coefficients table one
 *
 * indexed by [run][level-1]
 * split into two tables (dct_code_tab1a, dct_code_tab2a) to reduce size
 * 'End of Block' is treated elsewhere
 * codes do not include s (sign bit)
 */

VLCtable CSliceEncoder::dct_code_tab1a[2][40] =
{
 /* run = 0, level = 1...40 */
 {
  {0x02, 2}, {0x06, 3}, {0x07, 4}, {0x1c, 5},
  {0x1d, 5}, {0x05, 6}, {0x04, 6}, {0x7b, 7},
  {0x7c, 7}, {0x23, 8}, {0x22, 8}, {0xfa, 8},
  {0xfb, 8}, {0xfe, 8}, {0xff, 8}, {0x1f,14},
  {0x1e,14}, {0x1d,14}, {0x1c,14}, {0x1b,14},
  {0x1a,14}, {0x19,14}, {0x18,14}, {0x17,14},
  {0x16,14}, {0x15,14}, {0x14,14}, {0x13,14},
  {0x12,14}, {0x11,14}, {0x10,14}, {0x18,15},
  {0x17,15}, {0x16,15}, {0x15,15}, {0x14,15},
  {0x13,15}, {0x12,15}, {0x11,15}, {0x10,15}
 },
 /* run = 1, level = 1...18 */
 {
  {0x02, 3}, {0x06, 5}, {0x79, 7}, {0x27, 8},
  {0x20, 8}, {0x16,13}, {0x15,13}, {0x1f,15},
  {0x1e,15}, {0x1d,15}, {0x1c,15}, {0x1b,15},
  {0x1a,15}, {0x19,15}, {0x13,16}, {0x12,16},
  {0x11,16}, {0x10,16}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0},
  {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}
 }
};

VLCtable CSliceEncoder::dct_code_tab2a[30][5]=
{
  /* run = 2...31, level = 1...5 */
  {{0x05, 5}, {0x07, 7}, {0xfc, 8}, {0x0c,10}, {0x14,13}},
  {{0x07, 5}, {0x26, 8}, {0x1c,12}, {0x13,13}, {0x00, 0}},
  {{0x06, 6}, {0xfd, 8}, {0x12,12}, {0x00, 0}, {0x00, 0}},
  {{0x07, 6}, {0x04, 9}, {0x12,13}, {0x00, 0}, {0x00, 0}},
  {{0x06, 7}, {0x1e,12}, {0x14,16}, {0x00, 0}, {0x00, 0}},
  {{0x04, 7}, {0x15,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x05, 7}, {0x11,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x78, 7}, {0x11,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x7a, 7}, {0x10,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x21, 8}, {0x1a,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x25, 8}, {0x19,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x24, 8}, {0x18,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x05, 9}, {0x17,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x07, 9}, {0x16,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x0d,10}, {0x15,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1a,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x19,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x17,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x16,12}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1e,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1d,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1c,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1b,13}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1f,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1e,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1d,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1c,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}},
  {{0x1b,16}, {0x00, 0}, {0x00, 0}, {0x00, 0}, {0x00, 0}}
};

unsigned char CSliceEncoder::zig_zag_scan[64] =
{
   0, 1, 8,16, 9, 2, 3,10,
  17,24,32,25,18,11, 4, 5,
  12,19,26,33,40,48,41,34,
  27,20,13, 6, 7,14,21,28,
  35,42,49,56,57,50,43,36,
  29,22,15,23,30,37,44,51,
  58,59,52,45,38,31,39,46,
  53,60,61,54,47,55,62,63
};

unsigned char CSliceEncoder::m_progressive_scan[64] =
{
   0, 1, 9, 8,16,17,18,10,
   2, 3,11,19,27,26,25,24,
  32,33,34,35,36,28,20,12,
   4, 5,13,21,29,37,45,44,
  43,42,41,40,48,49,50,51,
  52,53,54,46,38,30,22,14,
   6, 7,15,23,31,39,47,55,
  63,62,61,60,59,58,57,56
};


sVLCtable CSliceEncoder::DClumtabHW[12] =
{
  {0x0004,3}, {0x0000,2}, {0x0001,2}, {0x0005,3}, {0x0006,3}, {0x000e,4},
  {0x001e,5}, {0x003e,6}, {0x007e,7}, {0x00fe,8}, {0x01fe,9}, {0x01ff,9}
};

#ifdef USE_IPP
const IppVCHuffmanSpec_32u CSliceEncoder::Y_DC_Tbl[12] =
{
  {0x0004,3}, {0x0000,2}, {0x0001,2}, {0x0005,3}, {0x0006,3}, {0x000e,4},
  {0x001e,5}, {0x003e,6}, {0x007e,7}, {0x00fe,8}, {0x01fe,9}, {0x01ff,9}
};

/* VL codes for DCT coefficients ISO/IEC 13818-2, B.5, Table B-15. */
const Ipp32s CSliceEncoder::Table15[] =
{
17, /* max bits */
3 ,  /* total subtables */
9 ,5, 3,/* subtable sizes */

 0, /* 1-bit codes */
 0, /* 2-bit codes */
 2 , /* 3-bit codes */
0x00000004, 0x00000000, 0x00000001, 0x00000005, 0x00000000, 0xffffffff,
 5 , /* 4-bit codes */
0x00000004, 0x00000001, 0x00000001, 0x00000005, 0x00000001, 0xffffffff, 0x00000006, IPPVC_ENDOFBLOCK, IPPVC_ENDOFBLOCK,
0x0000000c, 0x00000000, 0x00000002, 0x0000000d, 0x00000000, 0xfffffffe,
 2 , /* 5-bit codes */
0x0000000e, 0x00000000, 0x00000003, 0x0000000f, 0x00000000, 0xfffffffd,
 11, /* 6-bit codes */
0x00000001, IPPVC_ESCAPE, IPPVC_ESCAPE,
0x0000000a, 0x00000002, 0x00000001, 0x0000000b, 0x00000002, 0xffffffff, 0x0000000e, 0x00000003, 0x00000001,
0x0000000f, 0x00000003, 0xffffffff, 0x0000000c, 0x00000001, 0x00000002, 0x0000000d, 0x00000001, 0xfffffffe,
0x00000038, 0x00000000, 0x00000004, 0x00000039, 0x00000000, 0xfffffffc, 0x0000003a, 0x00000000, 0x00000005,
0x0000003b, 0x00000000, 0xfffffffb,
 8 , /* 7-bit codes */
0x0000000c, 0x00000004, 0x00000001, 0x0000000d, 0x00000004, 0xffffffff, 0x0000000e, 0x00000005, 0x00000001,
0x0000000f, 0x00000005, 0xffffffff, 0x0000000a, 0x00000000, 0x00000006, 0x0000000b, 0x00000000, 0xfffffffa,
0x00000008, 0x00000000, 0x00000007, 0x00000009, 0x00000000, 0xfffffff9,
 18, /* 8-bit codes */
0x0000000c, 0x00000006, 0x00000001, 0x0000000d, 0x00000006, 0xffffffff, 0x00000008, 0x00000007, 0x00000001,
0x00000009, 0x00000007, 0xffffffff, 0x0000000e, 0x00000002, 0x00000002, 0x0000000f, 0x00000002, 0xfffffffe,
0x0000000a, 0x00000008, 0x00000001, 0x0000000b, 0x00000008, 0xffffffff, 0x000000f0, 0x00000009, 0x00000001,
0x000000f1, 0x00000009, 0xffffffff, 0x000000f2, 0x00000001, 0x00000003, 0x000000f3, 0x00000001, 0xfffffffd,
0x000000f4, 0x0000000a, 0x00000001, 0x000000f5, 0x0000000a, 0xffffffff, 0x000000f6, 0x00000000, 0x00000008,
0x000000f7, 0x00000000, 0xfffffff8, 0x000000f8, 0x00000000, 0x00000009, 0x000000f9, 0x00000000, 0xfffffff7,

 28, /* 9-bit codes */
0x0000004c, 0x00000003, 0x00000002, 0x0000004d, 0x00000003, 0xfffffffe, 0x00000042, 0x0000000b, 0x00000001,
0x00000043, 0x0000000b, 0xffffffff, 0x0000004a, 0x0000000c, 0x00000001, 0x0000004b, 0x0000000c, 0xffffffff,
0x00000048, 0x0000000d, 0x00000001, 0x00000049, 0x0000000d, 0xffffffff, 0x0000004e, 0x00000001, 0x00000004,
0x0000004f, 0x00000001, 0xfffffffc, 0x000001f8, 0x00000002, 0x00000003, 0x000001f9, 0x00000002, 0xfffffffd,
0x000001fa, 0x00000004, 0x00000002, 0x000001fb, 0x00000004, 0xfffffffe, 0x00000046, 0x00000000, 0x0000000a,
0x00000047, 0x00000000, 0xfffffff6, 0x00000044, 0x00000000, 0x0000000b, 0x00000045, 0x00000000, 0xfffffff5,
0x00000040, 0x00000001, 0x00000005, 0x00000041, 0x00000001, 0xfffffffb, 0x000001f4, 0x00000000, 0x0000000c,
0x000001f5, 0x00000000, 0xfffffff4, 0x000001f6, 0x00000000, 0x0000000d, 0x000001f7, 0x00000000, 0xfffffff3,
0x000001fc, 0x00000000, 0x0000000e, 0x000001fd, 0x00000000, 0xfffffff2, 0x000001fe, 0x00000000, 0x0000000f,
0x000001ff, 0x00000000, 0xfffffff1,
 6 , /* 10-bit codes */
0x00000008, 0x00000005, 0x00000002, 0x00000009, 0x00000005, 0xfffffffe, 0x0000000a, 0x0000000e, 0x00000001,
0x0000000b, 0x0000000e, 0xffffffff, 0x0000000e, 0x0000000f, 0x00000001, 0x0000000f, 0x0000000f, 0xffffffff,

 4 , /* 11-bit codes */
0x0000001a, 0x00000010, 0x00000001, 0x0000001b, 0x00000010, 0xffffffff, 0x00000018, 0x00000002, 0x00000004,
0x00000019, 0x00000002, 0xfffffffc,
 0, /* 12-bit codes */
 20, /* 13-bit codes */
0x00000038, 0x00000003, 0x00000003, 0x00000039, 0x00000003, 0xfffffffd, 0x00000024, 0x00000004, 0x00000003,
0x00000025, 0x00000004, 0xfffffffd, 0x0000003c, 0x00000006, 0x00000002, 0x0000003d, 0x00000006, 0xfffffffe,
0x0000002a, 0x00000007, 0x00000002, 0x0000002b, 0x00000007, 0xfffffffe, 0x00000022, 0x00000008, 0x00000002,
0x00000023, 0x00000008, 0xfffffffe, 0x0000003e, 0x00000011, 0x00000001, 0x0000003f, 0x00000011, 0xffffffff,
0x00000034, 0x00000012, 0x00000001, 0x00000035, 0x00000012, 0xffffffff, 0x00000032, 0x00000013, 0x00000001,
0x00000033, 0x00000013, 0xffffffff, 0x0000002e, 0x00000014, 0x00000001, 0x0000002f, 0x00000014, 0xffffffff,
0x0000002c, 0x00000015, 0x00000001, 0x0000002d, 0x00000015, 0xffffffff,
 24, /* 14-bit codes */
0x0000002c, 0x00000001, 0x00000006, 0x0000002d, 0x00000001, 0xfffffffa, 0x0000002a, 0x00000001, 0x00000007,
0x0000002b, 0x00000001, 0xfffffff9, 0x00000028, 0x00000002, 0x00000005, 0x00000029, 0x00000002, 0xfffffffb,
0x00000026, 0x00000003, 0x00000004, 0x00000027, 0x00000003, 0xfffffffc, 0x00000024, 0x00000005, 0x00000003,
0x00000025, 0x00000005, 0xfffffffd, 0x00000022, 0x00000009, 0x00000002, 0x00000023, 0x00000009, 0xfffffffe,
0x00000020, 0x0000000a, 0x00000002, 0x00000021, 0x0000000a, 0xfffffffe, 0x0000003e, 0x00000016, 0x00000001,
0x0000003f, 0x00000016, 0xffffffff, 0x0000003c, 0x00000017, 0x00000001, 0x0000003d, 0x00000017, 0xffffffff,
0x0000003a, 0x00000018, 0x00000001, 0x0000003b, 0x00000018, 0xffffffff, 0x00000038, 0x00000019, 0x00000001,
0x00000039, 0x00000019, 0xffffffff, 0x00000036, 0x0000001a, 0x00000001, 0x00000037, 0x0000001a, 0xffffffff,

 32, /* 15-bit codes */
0x0000003e, 0x00000000, 0x00000010, 0x0000003f, 0x00000000, 0xfffffff0, 0x0000003c, 0x00000000, 0x00000011,
0x0000003d, 0x00000000, 0xffffffef, 0x0000003a, 0x00000000, 0x00000012, 0x0000003b, 0x00000000, 0xffffffee,
0x00000038, 0x00000000, 0x00000013, 0x00000039, 0x00000000, 0xffffffed, 0x00000036, 0x00000000, 0x00000014,
0x00000037, 0x00000000, 0xffffffec, 0x00000034, 0x00000000, 0x00000015, 0x00000035, 0x00000000, 0xffffffeb,
0x00000032, 0x00000000, 0x00000016, 0x00000033, 0x00000000, 0xffffffea, 0x00000030, 0x00000000, 0x00000017,
0x00000031, 0x00000000, 0xffffffe9, 0x0000002e, 0x00000000, 0x00000018, 0x0000002f, 0x00000000, 0xffffffe8,
0x0000002c, 0x00000000, 0x00000019, 0x0000002d, 0x00000000, 0xffffffe7, 0x0000002a, 0x00000000, 0x0000001a,
0x0000002b, 0x00000000, 0xffffffe6, 0x00000028, 0x00000000, 0x0000001b, 0x00000029, 0x00000000, 0xffffffe5,
0x00000026, 0x00000000, 0x0000001c, 0x00000027, 0x00000000, 0xffffffe4, 0x00000024, 0x00000000, 0x0000001d,
0x00000025, 0x00000000, 0xffffffe3, 0x00000022, 0x00000000, 0x0000001e, 0x00000023, 0x00000000, 0xffffffe2,
0x00000020, 0x00000000, 0x0000001f, 0x00000021, 0x00000000, 0xffffffe1,
 32, /* 16-bit codes */
0x00000030, 0x00000000, 0x00000020, 0x00000031, 0x00000000, 0xffffffe0, 0x0000002e, 0x00000000, 0x00000021,
0x0000002f, 0x00000000, 0xffffffdf, 0x0000002c, 0x00000000, 0x00000022, 0x0000002d, 0x00000000, 0xffffffde,
0x0000002a, 0x00000000, 0x00000023, 0x0000002b, 0x00000000, 0xffffffdd, 0x00000028, 0x00000000, 0x00000024,
0x00000029, 0x00000000, 0xffffffdc, 0x00000026, 0x00000000, 0x00000025, 0x00000027, 0x00000000, 0xffffffdb,
0x00000024, 0x00000000, 0x00000026, 0x00000025, 0x00000000, 0xffffffda, 0x00000022, 0x00000000, 0x00000027,
0x00000023, 0x00000000, 0xffffffd9, 0x00000020, 0x00000000, 0x00000028, 0x00000021, 0x00000000, 0xffffffd8,
0x0000003e, 0x00000001, 0x00000008, 0x0000003f, 0x00000001, 0xfffffff8, 0x0000003c, 0x00000001, 0x00000009,
0x0000003d, 0x00000001, 0xfffffff7, 0x0000003a, 0x00000001, 0x0000000a, 0x0000003b, 0x00000001, 0xfffffff6,
0x00000038, 0x00000001, 0x0000000b, 0x00000039, 0x00000001, 0xfffffff5, 0x00000036, 0x00000001, 0x0000000c,
0x00000037, 0x00000001, 0xfffffff4, 0x00000034, 0x00000001, 0x0000000d, 0x00000035, 0x00000001, 0xfffffff3,
0x00000032, 0x00000001, 0x0000000e, 0x00000033, 0x00000001, 0xfffffff2,
 32, /* 17-bit codes */
0x00000026, 0x00000001, 0x0000000f, 0x00000027, 0x00000001, 0xfffffff1, 0x00000024, 0x00000001, 0x00000010,
0x00000025, 0x00000001, 0xfffffff0, 0x00000022, 0x00000001, 0x00000011, 0x00000023, 0x00000001, 0xffffffef,
0x00000020, 0x00000001, 0x00000012, 0x00000021, 0x00000001, 0xffffffee, 0x00000028, 0x00000006, 0x00000003,
0x00000029, 0x00000006, 0xfffffffd, 0x00000034, 0x0000000b, 0x00000002, 0x00000035, 0x0000000b, 0xfffffffe,
0x00000032, 0x0000000c, 0x00000002, 0x00000033, 0x0000000c, 0xfffffffe, 0x00000030, 0x0000000d, 0x00000002,
0x00000031, 0x0000000d, 0xfffffffe, 0x0000002e, 0x0000000e, 0x00000002, 0x0000002f, 0x0000000e, 0xfffffffe,
0x0000002c, 0x0000000f, 0x00000002, 0x0000002d, 0x0000000f, 0xfffffffe, 0x0000002a, 0x00000010, 0x00000002,
0x0000002b, 0x00000010, 0xfffffffe, 0x0000003e, 0x0000001b, 0x00000001, 0x0000003f, 0x0000001b, 0xffffffff,
0x0000003c, 0x0000001c, 0x00000001, 0x0000003d, 0x0000001c, 0xffffffff, 0x0000003a, 0x0000001d, 0x00000001,
0x0000003b, 0x0000001d, 0xffffffff, 0x00000038, 0x0000001e, 0x00000001, 0x00000039, 0x0000001e, 0xffffffff,
0x00000036, 0x0000001f, 0x00000001, 0x00000037, 0x0000001f, 0xffffffff,
-1 /* end of table */
};

#endif




#define bs_write(count, bits) m_pBitStream->PutBits((bits), (count))
#define bs_write_vlc(v) m_pBitStream->PutBits((v).i_bits, (v).i_size)


#ifdef COLLECT_STATS
static const char * bucketStr[] = {
    "Slice Headers",
    "Zilch",
    "DC Coefficients",
    "AC Coefficients 2",
    "AC Coefficients 3",
    "AC Coefficients 4",
    "AC Coefficients 5",
    "AC Coefficients 6",
    "AC Coefficients 7",
    "AC Coefficients 8",
    "End Of Block",
    "Zilch",
    "End Of Slice",
    "Padding",
    "Undefined",
    "Undefined",
    "Undefined",
    "Undefined",
};
#endif


// Factory methods to create a CSliceEncoder
CSliceEncoder * CSliceEncoder::CreateSliceEncoder(int a_nMQuant, int a_nDCQuant, int a_nSliceSize)
{
    return new CStandardSliceEncoder(a_nMQuant, a_nDCQuant, a_nSliceSize);
}

CSliceEncoder * CSliceEncoder::CreateSliceEncoder(int a_nMQuant, int a_nDCQuant,
        int a_nSliceWidth, int a_nSliceHeight)
{
    return new CSVCSliceEncoder(a_nMQuant, a_nDCQuant, a_nSliceWidth, a_nSliceHeight);
}

CSliceEncoder * CSliceEncoder::CreateSliceEncoder(int a_nMQuant, int a_nDCQuant)
{
    return new CH264SliceEncoder(a_nMQuant, a_nDCQuant);
}

CSliceEncoder::CSliceEncoder(int a_nMQuant, int a_nDCQuant,
        int a_nSliceSize, int a_nSliceWidth, int a_nSliceHeight)
{
    m_nSliceSize = a_nSliceSize;
    m_nSliceWidth = a_nSliceWidth;
    m_nSliceHeight = a_nSliceHeight;

    // If slice width/height is set, slice size is computed, else
    // use slice size to compute slice width/height
    if (m_nSliceWidth > 0 && m_nSliceHeight > 0) {
        m_nWidth = m_nSliceWidth * 8;
        m_nHeight = m_nSliceHeight * 8;
        m_nSliceSize = m_nSliceWidth * m_nSliceHeight;
    }
    else {
        m_nWidth = m_nSliceSize * 8;
        m_nHeight = 8;
        m_nSliceWidth = m_nSliceSize;
        m_nSliceHeight = 1;
    }
    m_nMQuant = a_nMQuant;
    m_nDCQuant = a_nDCQuant;
    m_nMMult = 1 << a_nMQuant;
    m_nDCMult = 1 << a_nDCQuant;
    m_nBytesPerPixel = 4;
    m_bPassTests = false;
    m_nLRFlag = 0;

    m_pBitStream = new CBitStreamWriter();

#ifndef USE_IPP
    m_block = (short *) malloc(64 * sizeof(short));
    m_yArray = (uint8_t *) malloc(m_nWidth * m_nHeight);
    m_uArray = (uint8_t *) malloc(m_nWidth * m_nHeight);
    m_vArray = (uint8_t *) malloc(m_nWidth * m_nHeight);
#else
    m_block = ippsMalloc_16s(64 * sizeof(Ipp16s));
    m_yArray = ippsMalloc_8u(m_nWidth * m_nHeight);
    m_uArray = ippsMalloc_8u(m_nWidth * m_nHeight);
    m_vArray = ippsMalloc_8u(m_nWidth * m_nHeight);

    m_sliceRoi.width = m_nWidth;
    m_sliceRoi.height = 8;
    m_blockRoi.width = 8;
    m_blockRoi.height = 8;
    m_yuvarray[0] = m_yArray;
    m_yuvarray[1] = m_uArray;
    m_yuvarray[2] = m_vArray;
    m_dstStep[0] = m_nWidth;
    m_dstStep[1] = m_nWidth / 2;
    m_dstStep[2] = m_nWidth / 2;

    for (int i = 0; i < 64; i++) {
        m_quant_matrix[i] = 1.0/quant_mat_trans[i];
    }

    ippiCreateRLEncodeTable(Table15, &m_vlcTableB15);

#endif


    m_pBitStreamStats = new CBitStreamStats();
    m_pBitStreamStats->SetBitStream(m_pBitStream);

    m_nBlocksEncoded = 0;

}

CSliceEncoder::~CSliceEncoder()
{
#ifdef COLLECT_STATS
    PrintStats();
#endif
#ifdef USE_IPP
    ippsFree(m_block);
    ippsFree(m_yArray);
    ippsFree(m_uArray);
    ippsFree(m_vArray);

    if (m_vlcTableB15 != NULL)
      ippiHuffmanTableFree_32s(m_vlcTableB15);

#else
    if (m_block != NULL)
        free(m_block);
    if (m_yArray != NULL)
        free(m_yArray);
    if (m_uArray != NULL)
        free(m_uArray);
    if (m_vArray != NULL)
        free(m_vArray);
#endif


    delete m_pBitStreamStats;
    delete m_pBitStream;


}

void CSliceEncoder::SetTestable(bool a_value)
{
    m_bPassTests = a_value;
}

void CSliceEncoder::SetQuantizationParameter(int a_qp) {
    // Subclasses override
}


void CSliceEncoder::PrintStats() {
#ifdef COLLECT_STATS
    uint64_t total = m_pBitStreamStats->GetTotal();
    for (int i = 0; i < NUMB_BUCKETS; i++) {
        printf("%-2d %-20s Bytes: %-10"PRIu64" %-5.2f %% %-5"PRIu64"\n",
                i, bucketStr[i],
                    m_pBitStreamStats->GetBucket(i) >> 3,
                100.0 * m_pBitStreamStats->GetBucket(i)/total,
                    m_pBitStreamStats->GetCounter(i));
    }
    printf("Total = %"PRIu64"\n", total/8);
    printf("Distribution 1:4:8 %0.2f %% %0.2f %% %0.2f %%\n",
            100.0 * (m_pBitStreamStats->GetBucket(2))/total,
            100.0 * (m_pBitStreamStats->GetBucket(3) +
                    m_pBitStreamStats->GetBucket(4) +
                    m_pBitStreamStats->GetBucket(5))/total,
            100.0 * (m_pBitStreamStats->GetBucket(6) +
                    m_pBitStreamStats->GetBucket(7) +
                    m_pBitStreamStats->GetBucket(8) +
                    m_pBitStreamStats->GetBucket(9))/total);
    printf("Distribution 2:4:8 %0.2f %% %0.2f %% %0.2f %%\n",
            100.0 * (m_pBitStreamStats->GetBucket(2) +
                    m_pBitStreamStats->GetBucket(3))/total,
            100.0 * (m_pBitStreamStats->GetBucket(4) +
                    m_pBitStreamStats->GetBucket(5))/total,
            100.0 * (m_pBitStreamStats->GetBucket(6) +
                    m_pBitStreamStats->GetBucket(7) +
                    m_pBitStreamStats->GetBucket(8) +
                    m_pBitStreamStats->GetBucket(9))/total);

    printf("Distribution 3:5:8 %0.2f %% %0.2f %% %0.2f %%\n",
            100.0 * (m_pBitStreamStats->GetBucket(2) +
                    m_pBitStreamStats->GetBucket(3) +
                    m_pBitStreamStats->GetBucket(4))/total,
            100.0 * (m_pBitStreamStats->GetBucket(5) +
                    m_pBitStreamStats->GetBucket(6))/total,
            100.0 * (m_pBitStreamStats->GetBucket(7) +
                    m_pBitStreamStats->GetBucket(8) +
                    m_pBitStreamStats->GetBucket(9))/total);
    printf("Blocks Encoded = %u\n", m_nBlocksEncoded);
#endif
}

int CSliceEncoder::EncodeSlice(uint8_t * a_outBuff, uint8_t * a_bitmapArray,
        int a_nStride, int a_nSliceNumber, int a_nSliceSplit)
{
    m_pBufferStart = a_outBuff;
    m_nSliceNumber = a_nSliceNumber;
    m_pBitStream->Init(a_outBuff);
    m_pBitStreamStats->SetBitStream(m_pBitStream);

    // Initialize data
    InitEncode();

    // Convert input pixels to YUV
    RgbToYuv(a_bitmapArray, a_nStride, a_nSliceSplit);

    // Encode Slice Headers etc
    EncodeHeaders();

    // Encode pixel data in the slice
    EncodeData();

    // Finish the slice encoding
    int total_size = EncodeFooters();

    // Ensure slice is clean
    //CheckSlice(a_outBuff, total_size);

    return total_size;
}

void CSliceEncoder::InitEncode()
{
}

void CSliceEncoder::EncodeHeaders()
{
    if (m_nSliceNumber == 0)
        PutStartFrameCode();
    else
        PutStartSliceCode(m_nSliceNumber);
    m_pBitStreamStats->FillBucket(0);
}

int CSliceEncoder::EncodeFooters()
{
    m_pBitStreamStats->FillBucket(11);

    PutEndSliceCode();

    m_pBitStreamStats->FillBucket(12);

    m_pBitStream->FlushToWordBoundary();

    m_pBitStreamStats->FillBucket(13);

    return m_pBitStream->GetByteCount();
}

void CSliceEncoder::RgbToYuv(uint8_t * a_bitmapArray, int a_nStride, int a_nSliceSplit) {
    // RGB to YUV (NOTE: implementing only 4:2:2 encoding for now)
    if (a_nSliceSplit == 0)
        a_nSliceSplit = m_nWidth;
#if defined(USE_IPP) && !defined(PASS_TESTS)
    ippiBGRToYCbCr422_8u_AC4P3R(a_bitmapArray, a_nStride * 4,
            m_yuvarray, m_dstStep, m_sliceRoi);
#else
    CRGBToYUV::RGBToYUV(a_bitmapArray, m_yArray, m_uArray, m_vArray,
            m_nWidth, m_nHeight, a_nStride * m_nBytesPerPixel, m_nBytesPerPixel,
            a_nSliceSplit, 1);
#endif

}



// Check slice for unexpected patterns
void CSliceEncoder::CheckSlice(uint8_t *a_buffer, int a_len)
{
    // Skip first 4 bytes, since we know it has SOS/SOF
    int nLen = 4;
    uint8_t *buff = a_buffer + 4;

    while (nLen < a_len) {
        if (*(buff) == 0) {
            if (*((buff) + 1) == 0) {
                if (*((buff) + 2) == 0x01) {
                    if (*((buff) + 3) == 0xa0) {
                        printf("CheckSlice: Found Start of Frame (L)\n");
                    }
                    else if (*((buff) + 3) == 0) {
                        printf("CheckSlice: Found Start of Frame (R)\n");
                    }
                }
            }
            else if (*((buff) + 1) == 0x01) {
                if (*((buff) + 2) == 0xb2) {
                    printf("CheckSlice: Found Start of Slice\n");
                }
            }
        }
        nLen += 4;
        buff += 4;
    }
}

void CSliceEncoder::SetLeftRightFlag(int a_lrflag)
{
    m_nLRFlag = a_lrflag;
}

int CSliceEncoder::AddFrameHeader(uint8_t * a_outBuff,
        const SVCFrameHeader & frameHeader)
{
    // Standard encode does not have this header
    return 0;
}

int CSliceEncoder::AddEndOfFrame(uint8_t * a_outBuff)
{
    // Assumes input is already 4 byte alligned
    uint8_t * cp = a_outBuff;
    *cp++ = 0x00;
    *cp++ = 0x00;
    *cp++ = 0x01;
    *cp++ = 0xb0;
    return 4;
}

void CSliceEncoder::SetRefreshSliceBit(uint8_t * a_outBuff)
{
    uint8_t * cp = a_outBuff;
    *(cp+4) |= 0x08;
}

void CSliceEncoder::SetBytesPerPixel(int a_nBytesPerPixel)
{
    m_nBytesPerPixel = a_nBytesPerPixel;
}

void CSliceEncoder::PutStartFrameCode(void)
{
    int StartFrameCode = (m_nLRFlag == 0) ? 0x00000100 : 0x000001a0;

    m_pBitStream->PutBits(StartFrameCode, 32);

    PutQuantBits();
}

void CSliceEncoder::PutStartSliceCode(int a_nSliceNumber)
{
    int StartSliceCode = 0x0001B2;

    m_pBitStream->PutBits(StartSliceCode, 24);
    m_pBitStream->PutBits(a_nSliceNumber, 11); /* Slice Count */
    m_pBitStream->PutBits((m_nLRFlag == 0) ? 0 : 1, 1); /* I_frame_d1 (0: left, 1: right) */
    m_pBitStream->PutBits(0, 1); /* special_slice */

    PutQuantBits();
}

void CSliceEncoder::PutQuantBits(void)
{
    m_pBitStream->PutBits(m_nMQuant, 3);
    m_pBitStream->PutBits(m_nDCQuant, 3);
}

void CSliceEncoder::PutEndSliceCode(void)
{
    int EndSliceCode = 0x0001B9;
    m_pBitStream->PutBits(EndSliceCode, 24);
    m_pBitStream->PutBits(0, 8);
}



void CSliceEncoder::PutDC(sVLCtable *tab, int val)
{
    int absval, size;

    absval = (val < 0) ? -val : val;
    if (absval > 2047) {
        return;
    }

    size = 0;
    while (absval) {
        absval >>= 1;
        size++;
    }
//    printf("DC: Value: %d Size: %d Code: 0x%x Len %d\n",
//            val, size, tab[size].code, tab[size].len);

    m_pBitStream->PutBits(tab[size].code, tab[size].len);
    if (size != 0) {
        if (val >= 0)
            absval = val;
        else
            absval = val + (1 << size) - 1;
//        printf("DC: Putting value %d\n", absval);
        m_pBitStream->PutBits(absval, size);
    }
    //exit(0);
}

void CSliceEncoder::PutAC(int run, int signed_level)
{
#ifdef USE_IPP
    Ipp32s level, code, len;
    Ipp32s maxRun;
    Ipp32s addr;
    Ipp32s * table;
    Ipp32u val;

    maxRun = m_vlcTableB15[0] >> 20;
    if (run > maxRun) {
        /* no VLC for this (run, level) combination: use escape coding (7.2.2.3)*/
        /* ISO/IEC 13818-2 uses a 12 bit code, Table B-16*/
        signed_level &= (1 << 12) - 1;
        val = signed_level + (run << 12) + (1 << 18);
        m_pBitStream->PutBits(val, 24);
        return;
    }

    level = (signed_level < 0) ? -signed_level : signed_level;

    addr = m_vlcTableB15[run + 1];
    table = (Ipp32s*) ((Ipp8s*) m_vlcTableB15 + addr);
    if (level <= table[0]) {
        len = *(table + signed_level) & 0x1f;
        code = (*(table + signed_level) >> 16);
        m_pBitStream->PutBits(code, len);
    }
    else {
        /* no VLC for this (run, level) combination: use escape coding (7.2.2.3)*/
        /* ISO/IEC 13818-2 uses a 12 bit code, Table B-16*/
        signed_level &= (1 << 12) - 1;
        val = signed_level + (run << 12) + (1 << 18);
        m_pBitStream->PutBits(val, 24);
    }

#else
    int level, len;
    VLCtable *ptab = 0;

    level = (signed_level < 0) ? -signed_level : signed_level;

    if (run < 0 || run > 63 || level == 0 || level > 2047) {
        return;
    }
    len = 0;
    if (run < 2 && level < 41) {
        ptab = &dct_code_tab1a[run][level - 1];
        len = ptab->len;
    }
    else if (run < 32 && level < 6) {
        ptab = &dct_code_tab2a[run - 2][level - 1];
        len = ptab->len;
    }


    if (len != 0) {
//        printf("AC: Code: 0x%x Len: %d Level: %d Run: %d\n",
//                ptab->code, len, signed_level, run);
        m_pBitStream->PutBits(ptab->code, len);
        m_pBitStream->PutBits(signed_level < 0, 1);
    }
    else {
//        printf("AC: Run: %d Level: %d\n",
//                run, signed_level);
        m_pBitStream->PutBits(1l, 6);
        m_pBitStream->PutBits(run, 6);
        m_pBitStream->PutBits(signed_level & 0xFFF, 12);
    }
#endif
}


void CSliceEncoder::GetBlock(short* block, uint8_t * arrayPos,
        short stride)
{
    int offset = 0;
    int i, j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++)
            block[8 * i + j] = (short) (arrayPos[offset + j]);
        offset += stride;
    }
}

void CSliceEncoder::GetBlock(uint8_t * block, uint8_t * arrayPos,
        short stride)
{
    int offset = 0;
    int i, j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++)
            block[8 * i + j] = (arrayPos[offset + j]);
        offset += stride;
    }
}

void CSliceEncoder::AddBlk(short* block, short offset)
{
    int i;
    for (i = 0; i < 64; i++)
        block[i] += offset;
}


void CSliceEncoder::Transpose(short* block)
{
    short tmp[64];
    int i, j;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            tmp[8 * i + j] = block[8 * j + i];

    for (i = 0; i < 64; i++) {
        block[i] = tmp[i];
    }
}

// Original QuantIntra routine for reference. Not used. To be deleted eventually
// New one uses shifts in place of divisions and multiplications where possible
void CSliceEncoder::QuantIntraOrig(short* blk, int mquant)
{
    int i;
    int x, y;

    x = blk[0];
    blk[0] = (x >= 0) ? (x + (m_nDCMult >> 1)) / m_nDCMult : -((-x + (m_nDCMult
            >> 1)) / m_nDCMult);

    for (i = 1; i < 64; i++) {
        x = blk[i];
        y = (32 * (x >= 0 ? x : -x) + (quant_mat_trans[i] >> 1))
                / quant_mat_trans[i];
        y = (y + ((3 * mquant + 2) / 4)) / (2 * mquant);

        if (y > 2047)
            y = 2047;
        blk[i] = (x >= 0) ? y : -y;
    }
}


void CSliceEncoder::QuantIntra(short* blk)
{
    int i;
    int x, y, abs_x;

    x = blk[0];
    abs_x = x >= 0 ? x : -x;
#ifdef ORIG_FORMULA
    y = (abs_x + (m_nDCMult >> 1)) >> m_nDCQuant;
#else
    y = (abs_x) >> m_nDCQuant;
#endif
    blk[0] = (x >= 0) ? y : -y;


    for (i = 1; i < 64; i++) {
        x = blk[i];
        abs_x = x >= 0 ? x : -x;
#ifdef ORIG_FORMULA
        y = ((abs_x << 5) + (quant_mat_trans[i] >> 1)) >> quant_mat_trans_mult[i];
        y = (y + ((3 * mquant + 2) >> 2)) >> (m_nMQuant + 1);
#else
        y = (abs_x << 5) >> quant_mat_trans_mult[i];
        y = (y) >> (m_nMQuant + 1);
#endif
        if (y > 2047)
            y = 2047;
        blk[i] = (x >= 0) ? y : -y;
    }
}

#define SCANNER zig_zag_scan
#define BKT2 9
#define BKT3 18
#define BKT4 27
#define BKT5 36
#define BKT6 45
#define BKT7 54
#define BKT8 63

void CSliceEncoder::CollectACStats(int n, int signed_level) {
    if (SCANNER[n] == BKT2) {
        m_pBitStreamStats->FillBucket(3);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(3);
    }
    else if (SCANNER[n] == BKT3) {
        m_pBitStreamStats->FillBucket(4);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(4);
    }
    else if (SCANNER[n] == BKT4) {
        m_pBitStreamStats->FillBucket(5);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(5);
    }
    else if (SCANNER[n] == BKT5) {
        m_pBitStreamStats->FillBucket(6);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(6);
    }
    else if (SCANNER[n] == BKT6) {
        m_pBitStreamStats->FillBucket(7);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(7);
    }
    else if (SCANNER[n] == BKT7) {
        m_pBitStreamStats->FillBucket(8);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(8);
    }
    else if (SCANNER[n] == BKT8) {
        m_pBitStreamStats->FillBucket(9);
        if (signed_level == 0)
            m_pBitStreamStats->FillCounter(9);
    }
}

void CSliceEncoder::PutIntraBlk(short* blk, int cc, short* dc_dct_pred,
        int a_firstBlkFlg, int nonZeroCoefficients)
{
    int n, dct_diff, run, signed_level;
    m_pBitStreamStats->FillBucket(1);

    if (a_firstBlkFlg)
        dc_dct_pred[cc] = 0;

    dct_diff = blk[0] - dc_dct_pred[cc];
    dc_dct_pred[cc] = blk[0];

    // Encode the DC coefficient
#ifndef USE_IPP
    PutDC(DClumtabHW, dct_diff);
#else
    int m, absval, size1;
    if(dct_diff == 0) {
        m_pBitStream->PutBits(Y_DC_Tbl[0].code, Y_DC_Tbl[0].len);
    }
    else {
        m = dct_diff >> 31;
        absval = (dct_diff + m) ^ m;
        for (size1=1; absval >>= 1; size1++);

        absval = ((Y_DC_Tbl[size1].code - m) << size1) + (dct_diff + m);
        n  = Y_DC_Tbl[size1].len + size1;
//        printf("DC: IPP Value: %d Size: %d Code: 0x%x Len %d\n",
//                dct_diff, size1, absval, n);
        m_pBitStream->PutBits(absval, n);
    }
#endif


    m_pBitStreamStats->FillBucket(2);

    run = 0;
    int nonZeroFound = 0;
    for (n = 1; nonZeroFound < nonZeroCoefficients && n < 64; n++) {
#ifdef USE_PROGRESSIVE_SCAN
        signed_level = blk[m_progressive_scan[n]];
#else
        signed_level = blk[zig_zag_scan[n]];
#endif
        if (signed_level != 0) {
            PutAC(run, signed_level);
            run = 0;
            nonZeroFound++;
        }
        else
            run++;
#ifdef COLLECT_STATS
        CollectACStats(n, signed_level);
#endif
    }
    m_pBitStream->PutBits(6, 4);

    m_pBitStreamStats->FillBucket(10);
    m_pBitStreamStats->FillCounter(1);
}

//#define DUMP_DCT 1

void CSliceEncoder::EncodeOneBlock(short* a_block, uint8_t * a_arrayPos,
        short a_stride, unsigned int a_iMQuant, int a_cc, short* a_dc_dct_pred,
        int a_firstBlkFlg)
{
    int nonZeroCoefficients;

    if (a_cc == 0)
        m_nBlocksEncoded++;

#ifndef USE_IPP
    // Get data into a block
    GetBlock(a_block, a_arrayPos, a_stride);
    AddBlk(a_block, -128);

#ifdef DUMP_DCT
    {
        char buff[32];
        sprintf(buff, "BEFORE_DCT %d", m_nBlocksEncoded);
        V2DLib::dumpBlock(a_block, buff);
    }
#endif

    // Forward DCT
    if (!m_bPassTests)
        CFDCT::FDCT(a_block);
    else
        CFDCT::FDCTFixed(a_block);

#ifdef DUMP_DCT
    {
        char buff[32];
        sprintf(buff, "AFTER_DCT %d", m_nBlocksEncoded);
        V2DLib::dumpBlock(a_block, buff);
    }
#endif

    // Transpose
    Transpose(a_block);

    // Quantize
    nonZeroCoefficients = 64;
    QuantIntra(a_block);
#ifdef DUMP_DCT
    {
        char buff[32];
        sprintf(buff, "AFTER_QUANT %d", m_nBlocksEncoded);
        V2DLib::dumpBlock(a_block, buff);
    }
#endif

#else
    // Get data into a block
    ippiSub128_JPEG_8u16s_C1R(a_arrayPos, a_stride, a_block, 16, m_blockRoi);

    // Forward DCT
#ifdef PASS_TESTS
    CFDCT::FDCT(a_block);
#else
    ippiDCT8x8Fwd_16s_C1I(a_block);
#endif

    // Transpose
    ippiTranspose_16s_C1IR(a_block, 16, m_blockRoi);

    // Quantize
    // DC Coefficient
    a_block[0] = (a_block[0] > 0) ?
        (a_block[0] >> m_nDCQuant) : -((-a_block[0]) >> m_nDCQuant);

    // AC Coefficients
    ippiQuantIntra_MPEG2_16s_C1I(a_block, m_nMMult, m_quant_matrix, &nonZeroCoefficients);
#endif

    // Forward VLC
    PutIntraBlk(a_block, a_cc, a_dc_dct_pred, a_firstBlkFlg, nonZeroCoefficients);
}

void CSliceEncoder::BuildTables() {
    // Unused code. TODO: delete this
    int tablesize = sizeof(DClumtabHW)/sizeof(sVLCtable);
    int len = 0;
    sVLCtable *tab = DClumtabHW;
    for (int i = 0; i < tablesize; i++) {
        if (tab[i].len > len)
            len = tab[i].len;
    }
    int maxBits = len;
    printf("Max bits : %d\n", maxBits);
    for (int bits = 1; bits <= maxBits; bits++) {
        int count = 0;
        for (int i = 0; i < tablesize; i++) {
            if (tab[i].len == bits) {
                count++;
                printf("Code: 0x%04x idx: 0x%04x\n",
                        tab[i].code, i);
            }
        }
        printf("%d bit codes: %d\n", bits, count);
    }
}
