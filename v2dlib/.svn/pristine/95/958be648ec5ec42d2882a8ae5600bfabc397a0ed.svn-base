
#include "v2dlib_compatibility.h"
#include <math.h>

#include "comp_utils.h"


#define FILE  char
#define true 1
#define false 0

typedef unsigned char UChar;
typedef unsigned int  UINT;
//typedef char bool;



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


static void	       initEncode(FILE* a_outFp);
static void	       RGBtoYUV(UChar *BmpPos,UChar *yArrayPos,UChar *uArrayPos,
			  UChar *vArrayPos,int frameWidth,int frameHeight,int subsample);
static void           putStartFrameCode(void);
static void           encodeOneBlock(short* a_block, UChar* a_arrayPos, short a_stride, 
                              UINT a_iMQuant, int a_cc, short* a_dc_dct_pred, 
                              double* a_MSE, double* a_M1, double* a_M2, int a_firstBlkFlg);
static void           putEndFrameCode(void);
static void	       endEncode(void);


static void           putStartSliceCode(void);
static void           putEndSliceCode(void);

static void           init_fdct(void);
/* static void           init_idct(void); */
static void           getBlock(short* block, UChar* arrayPos, short stride);
static void	       addBlk(short* block, short offset);
static void	       doHWFDCT(short* blk);
static void	       quant_intra(short* blk,int mquant);
static void	       putintrablk(short* blk,int cc,short* dc_dct_pred, int a_firstBlkFlg);

#ifdef JUNK
static void           iquant_intra(short* blk, int mquant);
static void           doHWIDCT(short* blk);
static void           putBlock(short* block, UChar* arrayPos, short stride, 
			 double *MSE, double *M1, double *M2);
#endif

static void            putbits(int val,int n);
static void            writeLeftOverBits(void);
static void            transpose(short* block);
static void            putDClum(int val);
static void            putDCchrom(int val);
static void            putDC(sVLCtable *tab,int val);
static void            putAC(int run,int signed_level,int vlcformat);

#ifdef JUNK
static void            idctcol(short* blk);
static void            idctrow(short* blk);
#endif

#define     SLICE_ENCODING  1




static UChar quant_mat_trans[64] =
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

static VLCtable dct_code_tab1[2][40]=
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

static VLCtable dct_code_tab2[30][5]=
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

static VLCtable dct_code_tab1a[2][40]=
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

static VLCtable dct_code_tab2a[30][5]=
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

static UChar  zig_zag_scan[64] =
{
  0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,
  12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
  35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,
  58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
};

static sVLCtable DClumtabHW[12]=
{
  {0x0004,3}, {0x0000,2}, {0x0001,2}, {0x0005,3}, {0x0006,3}, {0x000e,4},
  {0x001e,5}, {0x003e,6}, {0x007e,7}, {0x00fe,8}, {0x01fe,9}, {0x01ff,9}
};


/* Variables */
static double		     m_cos[8][8]; 
//extern short		     m_iclip[1024]; 
#ifdef JUNK
static short*                m_iclp;
#endif

static short                 m_dcmult;
static FILE*	             m_outFile;
static UChar	 	     m_outbfr;
static int	             m_outcnt;
static int		     m_bytecnt;

static int            m_sliceNo;

static int MQUANT = 8;
static int DCQUANT = 8;
static int MQUANTP = 3;
static int DCQUANTP = 3;

/*********************
 * Utility Functions *
 *********************/

static int Y_R[256];
static int Y_B[256];
static int Y_G[256];
static int U_R[256];
static int U_G[256];
static int U_B[256];
static int V_R[256];
static int V_G[256];
static int V_B[256];


static void YuvRgbLookupTable(void)
{
    int i;

    for(i = 0; i < 256; i++)
	{
	    Y_R[i] = 8421  * i;
	    Y_G[i] = 16515 * i;
	    Y_B[i] = 3211  * i;
	    U_R[i] = 4850  * i;
	    U_G[i] = 9536  * i;
	    U_B[i] = 14385 * i;
	    V_R[i] = 14385 * i;
	    V_G[i] = 12059 * i;
	    V_B[i] = 2327  * i;						
	}
}

/* static YuvRgbLookupTable s_yuvRgbLookupTable; */


/* YuvRgbLookupTable YuvRgbLookupTable::s_yuvRgbLookupTable; */

#define RGB2Y(red, green, blue) (short)(((  Y_R[red] + Y_G[green] + Y_B[blue]) >> 15) +  16)
#define RGB2U(red, green, blue) (short)(((- U_R[red] - U_G[green] + U_B[blue]) >> 15) + 128)
#define RGB2V(red, green, blue) (short)(((  V_R[red] - V_G[green] - V_B[blue]) >> 15) + 128)
#define _S(a)	  (a)>255 ? 255 : (a)<0 ? 0 : (a)

static void CompFunctions(void)
{
  init_fdct();
/*   init_idct(); */
  m_sliceNo = 1;
}


static void init_fdct(void)
{
  int i, j;
  double s;

  for (i=0; i<8; i++)
  {
    s = (i==0) ? sqrt(0.125) : 0.5;

    
    for (j=0; j<8; j++)
      m_cos[i][j] = s * cos((PI/8.0)*i*(j+0.5));
  }
}

/* static void  */
/* init_idct(void) */
/* { */
/*   int i; */

/*   m_iclp = m_iclip + 512; */
/*   for (i= -512; i<512; i++) */
/*     m_iclp[i] = (i<-256) ? -256 : ((i>255) ? 255 : i); */
/* } */

static void 
initEncode(FILE* a_outFp)
{
  m_outFile = a_outFp;
	m_outcnt = 8;
	m_bytecnt = 0;
  m_dcmult = DCQUANT;
}

static void	         
RGBtoYUV(UChar *BmpPos,UChar *yArrayPos,UChar *uArrayPos,
	 UChar *vArrayPos,int frameWidth,int frameHeight,int subsample)
{
  
	UChar blue, green, red;
	int y;
	int row, col;

  if(subsample==0) // 4:4:4
	{
    for(row = 0; row < frameHeight; row++)
		{
			for(col=0; col<frameWidth; col++)
			{
				blue  = *BmpPos;
				green = *(BmpPos +1);
				red   = *(BmpPos + 2);
				*(yArrayPos++) = _S(RGB2Y(red, green, blue));			
				*(uArrayPos++) = _S(RGB2U(red, green, blue));
				*(vArrayPos++) = _S(RGB2V(red, green, blue));
				BmpPos += 4;
			}	
		}
	}
	else if(subsample == 1) // 4:2:2
	{
    for(row = 0; row < frameHeight; row++)
		{
			for(col=0; col<frameWidth; col++)
			{
				blue  = *BmpPos;
				green = *(BmpPos +1);
				red   = *(BmpPos + 2);
				
				*(yArrayPos++)    = y = _S(RGB2Y(red, green, blue));
				if(col%2 == 0)
				{
					*(uArrayPos++) = _S(RGB2U(red, green, blue));
					*(vArrayPos++) = _S(RGB2V(red, green, blue));
				}
				BmpPos += 4;
			}
    }
	}
  else // 4:2:0
  {
		for(row = 0; row < frameHeight; row++)
		{
			if(row%2 == 1)
			{
				for(col=0; col<frameWidth; col++)
				{
					blue  = *BmpPos;
					green = *(BmpPos +1);
					red   = *(BmpPos + 2);
					
					*(yArrayPos++)    = y = _S(RGB2Y(red, green, blue));
					if(col%2 == 0)
					{
						*(uArrayPos++) = _S(RGB2U(red, green, blue));
						*(vArrayPos++) = _S(RGB2V(red, green, blue));
					}
					BmpPos += 4;
				}
			}
			else
			{
				for(col=0; col<frameWidth; col++)
				{
					blue  = *BmpPos;
					green = *(BmpPos + 1);
					red   = *(BmpPos + 2);
					
					*(yArrayPos++)    = y = _S(RGB2Y(red, green, blue));
					BmpPos += 4;
				}
			}
			
		}
  } 		
}

static  int     m_sliceSize;

static void           
putStartSliceCode(void)
{
  int StartSliceCode = 0x0001B2;
/*   int tmp; */

  /* align to 4-byte boundary */
  writeLeftOverBits();
  while (m_bytecnt % 4 != 0) putbits(0,8);


  putbits(StartSliceCode,24);
  /* tmp = m_sliceNo << 3; */  /* Changed when we went from slice size to 128 to 32 */

/*   tmp = m_sliceNo << 1; */
/*   putbits(tmp,12); */
  putbits(m_sliceNo, 11); /* Slice Count */
  putbits(0,1); /* I_frame_d1 */
  putbits(0,1); /* special_slice */

  putbits(MQUANTP, 3);
  putbits(DCQUANTP, 3);

  m_sliceNo++;
}

static void           
putEndSliceCode(void)
{
  int EndSliceCode = 0x0001B9;
  putbits(EndSliceCode,24);
  putbits(0,8);
}

static void           
putStartFrameCode(void)
{
  int StartFrameCode = 0x00000100;

  /* align to 4-byte boundary */
  writeLeftOverBits();
  while (m_bytecnt % 4 != 0) putbits(0,8);

  putbits(StartFrameCode,32);
  putbits(MQUANTP, 3);
  putbits(DCQUANTP, 3);
/*   putbits(0,10); */
  m_sliceNo = 1;
}

static void           
encodeOneBlock(short* a_block, UChar* a_arrayPos, short a_stride, 
                              UINT a_iMQuant, int a_cc, short* a_dc_dct_pred, 
                              double* a_MSE, double* a_M1, double* a_M2, int a_firstBlkFlg)
{
  getBlock(a_block,a_arrayPos,a_stride);
  addBlk(a_block,-128);
  doHWFDCT(a_block);
  quant_intra(a_block,a_iMQuant);         
  putintrablk(a_block,a_cc,a_dc_dct_pred,a_firstBlkFlg);
/*   iquant_intra(a_block,a_iMQuant); */
/*   doHWIDCT(a_block); */
/*   addBlk(a_block,128); */
/*   putBlock(a_block,a_arrayPos,a_stride, a_MSE, a_M1, a_M2); */
}

static void           
putEndFrameCode(void)
{
  int EndFrameCode = 0x000001B0;
  putbits(EndFrameCode,32);

}

static void
endEncode(void)
{
  writeLeftOverBits();
/* 	fflush(m_outFile); */
}

// Private Functions
static void            
getBlock(short* block, UChar* arrayPos, short stride)
{
	int offset = 0;
  int i,j;
    
  for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
			block[8*i+j] = (short)(arrayPos[offset+j]);
		offset+=stride;
	}
}

static void	          
addBlk(short* block, short offset)
{
    int i;
	for(i=0;i<64;i++)
		block[i] += offset;
}

static void	          
doHWFDCT(short* block)
{
  int i, j, k;
	double s;
	double tmp[64];
	
	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			s = 0.0;
			for (k=0; k<8; k++)
				s += m_cos[j][k] * block[8*i+k];
			tmp[8*i+j] = s;
		}
	}
	for (j=0; j<8; j++)
	{
		for (i=0; i<8; i++)
		{
			s = 0.0;
			for (k=0; k<8; k++)
				s += m_cos[i][k] * tmp[8*k+j];
			block[8*i+j] = (int)floor(s+0.499999);
		}
	}
  transpose(block);
}

static void	          
quant_intra(short* blk,int mquant)
{
  int i;
	int x, y;

	x = blk[0];
	blk[0] = (x>=0) ? (x+(m_dcmult>>1))/m_dcmult : -((-x+(m_dcmult>>1))/m_dcmult);  

	for (i=1; i<64; i++)
	{
		x = blk[i];
		y = (32*(x>=0 ? x : -x) + (quant_mat_trans[i]>>1))/quant_mat_trans[i];  
		y = (y+((3*mquant+2)/4))/(2*mquant); 
		if (y > 2047)
			y = 2047; 
		blk[i] = (x>=0) ? y : -y;
	}
}

static void	          
putintrablk(short* blk,int cc,short* dc_dct_pred, int a_firstBlkFlg)
{
  int n, dct_diff, run, signed_level;

  if(a_firstBlkFlg)
    dc_dct_pred[cc]=0;

  dct_diff = blk[0] - dc_dct_pred[cc]; 
  dc_dct_pred[cc] = blk[0];
  if (cc==0)
    putDClum(dct_diff);
  else
    putDCchrom(dct_diff);
  run = 0;
  for (n=1; n<64; n++)
  {
    signed_level = blk[zig_zag_scan[n]];
    if (signed_level!=0)
    {
      putAC(run,signed_level,1);
      run = 0;
    }
    else
      run++; 
  }
  putbits(6,4);
}

#ifdef JUNK
static void            
iquant_intra(short* blk, int mquant)
{
  int i, val, sum,d;

	blk[0] = blk[0]*8;
	sum = blk[0];
	for (i=1; i<64; i++)
	{
		d = quant_mat_trans[i] ;
		val = (int)(blk[i]*d*mquant)/16;
		sum+= blk[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
	}
}

static void	          
doHWIDCT(short* blk)
{  
  int i;

  for (i=0; i<8; i++)
    idctrow(blk+8*i);

  for (i=0; i<8; i++)
    idctcol(blk+i);
  transpose(blk);
}

static void            
putBlock(short* block, UChar* arrayPos, short stride, 
                        double *MSE, double *M1, double *M2)
{
  	int offset = 0;
  int i,j;
  double mse =0;
  double s1 = 0;
	double s2 = 0;
	for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
		{
			UChar outdata = _S(block[8*i+j]);
			UChar indata  = arrayPos[offset+j];
			s1 += indata;
			s2 += pow(indata,2);
			mse += pow((indata - outdata),2);
			arrayPos[offset+j] = outdata;
		}
		offset+=stride;
	}
	*M1  += s1;
	*M2	 += s2;
	*MSE += mse;
}
#endif

static void 
putbits(int val,int n)
{
  int i;
  UINT mask;

  mask = 1 << (n-1); 

  for (i=0; i<n; i++)
  {
    m_outbfr <<= 1;

    if (val & mask)
      m_outbfr|= 1;

    mask >>= 1; 
    m_outcnt--;

    if (m_outcnt==0) 
    {
/*       fputc(m_outbfr,m_outFile); */
	*m_outFile++ = m_outbfr;

      m_outcnt = 8;
      m_bytecnt++;
    }
  }
}

static void 
writeLeftOverBits()
{
  if(m_outcnt != 8)
  {
    m_outbfr <<= m_outcnt;
    *m_outFile++ = m_outbfr;

    m_outcnt = 8;
    m_outbfr = 0;
    m_bytecnt++;
  }
}

static void 
transpose(short* block)
{
  short tmp[64];
  int i,j;
  
  for(i=0;i<8;i++)
    for(j=0;j<8;j++)
      tmp[8*i+j] = block[8*j+i];
  
  for(i=0;i<64;i++)
    block[i]=tmp[i];
}

static void 
putDClum(int val)
{
  putDC(DClumtabHW,val);
}

static void 
putDCchrom(int val)
{
  putDC(DClumtabHW,val);
}

static void 
putDC(sVLCtable *tab,int val)
{
  int absval, size;

  absval = (val<0) ? -val : val; 
  if (absval>2047)
  {
      /* 	  exit(-1); */  /* TODO : Handle this*/
      return;
  }

  size = 0;
  while (absval)
  {
    absval >>= 1;
    size++;
  }

  putbits(tab[size].code,tab[size].len);
  if (size!=0)
  {
    if (val>=0)
      absval = val;
    else
      absval = val + (1<<size) - 1; 
    putbits(absval,size);
  }
}

static void putAC(int run,int signed_level,int vlcformat)
{
  int level, len;
  VLCtable *ptab = 0;

  level = (signed_level<0) ? -signed_level : signed_level;

  if (run<0 || run>63 || level==0 || level>2047)
  {
      /*  exit(-1);  */
      return;
  }
  len = 0;
  if (run<2 && level<41)
  {
    if (vlcformat)
      ptab = &dct_code_tab1a[run][level-1];
    else
      ptab = &dct_code_tab1[run][level-1];

    len = ptab->len;
  }
  else if (run<32 && level<6)
  {
    if (vlcformat)
      ptab = &dct_code_tab2a[run-2][level-1];
    else
      ptab = &dct_code_tab2[run-2][level-1];

    len = ptab->len;
  }

  if (len!=0) 
  {
    putbits(ptab->code,len);
    putbits(signed_level<0,1); 
  }
  else
  {
    putbits(1l,6);
    putbits(run,6);
    putbits(signed_level,12);
  }
}

#ifdef JUNK
static void idctrow(short* blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;

  /* shortcut */
  if (!((x1 = blk[4]<<11) | (x2 = blk[6]) | (x3 = blk[2]) |
        (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
  {
    blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
    return;
  }

  x0 = (blk[0]<<11) + 128; /* for proper rounding in the fourth stage */

  /* first stage */
  x8 = W7*(x4+x5);
  x4 = x8 + (W1-W7)*x4;
  x5 = x8 - (W1+W7)*x5;
  x8 = W3*(x6+x7);
  x6 = x8 - (W3-W5)*x6;
  x7 = x8 - (W3+W5)*x7;
  
  /* second stage */
  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6*(x3+x2);
  x2 = x1 - (W2+W6)*x2;
  x3 = x1 + (W2-W6)*x3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  
  /* third stage */
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  
  /* fourth stage */
  blk[0] = (x7+x1)>>8;
  blk[1] = (x3+x2)>>8;
  blk[2] = (x0+x4)>>8;
  blk[3] = (x8+x6)>>8;
  blk[4] = (x8-x6)>>8;
  blk[5] = (x0-x4)>>8;
  blk[6] = (x3-x2)>>8;
  blk[7] = (x7-x1)>>8;
}

/* column (vertical) IDCT
 *
 *             7                         pi         1
 * dst[8*k] = sum c[l] * src[8*l] * cos( -- * ( k + - ) * l )
 *            l=0                        8          2
 *
 * where: c[0]    = 1/1024
 *        c[1..7] = (1/1024)*sqrt(2)
 */
static void idctcol(short* blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;

  /* shortcut */
  if (!((x1 = (blk[8*4]<<8)) | (x2 = blk[8*6]) | (x3 = blk[8*2]) |
        (x4 = blk[8*1]) | (x5 = blk[8*7]) | (x6 = blk[8*5]) | (x7 = blk[8*3])))
  {
    blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=
      m_iclp[(blk[8*0]+32)>>6];
    return;
  }

  x0 = (blk[8*0]<<8) + 8192;

  /* first stage */
  x8 = W7*(x4+x5) + 4;
  x4 = (x8+(W1-W7)*x4)>>3;
  x5 = (x8-(W1+W7)*x5)>>3;
  x8 = W3*(x6+x7) + 4;
  x6 = (x8-(W3-W5)*x6)>>3;
  x7 = (x8-(W3+W5)*x7)>>3;
  
  /* second stage */
  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6*(x3+x2) + 4;
  x2 = (x1-(W2+W6)*x2)>>3;
  x3 = (x1+(W2-W6)*x3)>>3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  
  /* third stage */
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  
  /* fourth stage */
  blk[8*0] = m_iclp[(x7+x1)>>14];
  blk[8*1] = m_iclp[(x3+x2)>>14];
  blk[8*2] = m_iclp[(x0+x4)>>14];
  blk[8*3] = m_iclp[(x8+x6)>>14];
  blk[8*4] = m_iclp[(x8-x6)>>14];
  blk[8*5] = m_iclp[(x0-x4)>>14];
  blk[8*6] = m_iclp[(x3-x2)>>14];
  blk[8*7] = m_iclp[(x7-x1)>>14];
}

#endif

/**********************
 * Exported API calls *
 **********************/

static  UChar   *m_yArray, *m_uArray, *m_vArray;
static  UChar   *m_yArrayPos, *m_uArrayPos, *m_vArrayPos;
static  int     m_frameWidth, m_frameHeight;


void initFrameParams(int a_frameWidth, int a_frameHeight, int mquant, int dcquant, int slicesize)
{

    YuvRgbLookupTable();
    CompFunctions();

  m_frameWidth  = a_frameWidth;
  m_frameHeight = a_frameHeight;
#ifdef SLICE_ENCODING
  if(m_frameWidth % slicesize == 0)
    m_sliceSize = slicesize;
  else 
    m_sliceSize = 128;
#else
  m_sliceSize = m_frameWidth/8;
#endif

  MQUANTP = mquant;
  DCQUANTP = dcquant;
  MQUANT = 1 << mquant;
  DCQUANT = 1 << dcquant;

}

int  encodeOneFrameToFile(UChar* a_bitmapArray,
			   FILE* a_outFp, int a_frameNo)
{
  int yStride = m_frameWidth;
  int uvStride = m_frameWidth/2;
  short block[64];
  short mQuant = MQUANT;
  double Y_MSE, Y_M1, Y_M2, U_MSE, U_M1, U_M2, V_MSE, V_M1, V_M2;
  short dc_dct_pred[3];
  int numYBlksInRow = yStride/8;
  int numYBlkRows = m_frameHeight/8;
  
  int totalNoOfBlks =  numYBlksInRow*numYBlkRows;
  int blockCount    = 0;
/*   int initBlkNo = 1; */

  int flag;
  int blkNo;

  m_yArray = (UChar *) malloc(m_frameWidth*m_frameHeight);
  m_uArray = (UChar *) malloc(m_frameWidth*m_frameHeight);
  m_vArray = (UChar *) malloc(m_frameWidth*m_frameHeight);

  if (m_yArray == NULL || m_uArray == NULL || m_vArray == NULL)
      return -1;


  Y_MSE = 0; Y_M1 = 0; Y_M2 = 0; U_MSE = 0; U_M1 = 0; U_M2 = 0; V_MSE = 0; V_M1 = 0; V_M2 = 0;
  m_yArrayPos = m_yArray; m_uArrayPos = m_uArray; m_vArrayPos = m_vArray;
  dc_dct_pred[0] = 0;
  dc_dct_pred[1] = 0;
  dc_dct_pred[2] = 0;


  initEncode(a_outFp);

  
  //NOTE: implementing only 4:2:2 encoding for now.
  RGBtoYUV(a_bitmapArray,m_yArrayPos,m_uArrayPos,m_vArrayPos,m_frameWidth,m_frameHeight,1);
  putStartFrameCode();


#ifdef SLICE_ENCODING
  while(blockCount<totalNoOfBlks)
  {  
    if(blockCount!=0)
      putStartSliceCode(); 

    /*
    if((a_frameNo>0) && (blockCount >= 7*m_sliceSize) && (blockCount < 12*m_sliceSize))
    {
      m_yArrayPos += 8*m_sliceSize;
      m_uArrayPos += 4*m_sliceSize;
      m_vArrayPos += 4*m_sliceSize;
      blockCount  += m_sliceSize;
      m_sliceNo += 1;
      continue;
    }
    else
    {
      if(blockCount!=0)
        putStartSliceCode(); 
    }*/
    flag = true;
    for(blkNo=0;blkNo<m_sliceSize/2;blkNo++)
    {
      encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,flag);
      m_yArrayPos +=8;
      encodeOneBlock(block,m_uArrayPos,uvStride,mQuant,1,dc_dct_pred,&U_MSE,&U_M1,&U_M2,flag);
      m_uArrayPos +=8;
      blockCount++;

      encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,false);
      m_yArrayPos +=8;
      encodeOneBlock(block,m_vArrayPos,uvStride,mQuant,2,dc_dct_pred,&V_MSE,&V_M1,&V_M2,flag);
      m_vArrayPos +=8;
      blockCount++;
      flag = false;
      if((m_yArrayPos-m_yArray) % m_frameWidth == 0)
        m_yArrayPos += yStride*7;
      if((m_uArrayPos-m_uArray) % (m_frameWidth/2) == 0)
      {
        m_uArrayPos += uvStride*7;
        m_vArrayPos += uvStride*7;
      }
    }
    if(blockCount+1 !=totalNoOfBlks)
    {
      putEndSliceCode();
    }
  }
#else //SLICE_ENCODING
  while(blockCount<totalNoOfBlks)
  {   
      int blkNo;
    encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,0);
    m_yArrayPos +=8;
    encodeOneBlock(block,m_uArrayPos,uvStride,mQuant,1,dc_dct_pred,&U_MSE,&U_M1,&U_M2,0);
    m_uArrayPos +=8;
    blockCount++;

    for(blkNo=1;blkNo<m_sliceSize/2;blkNo++)
    {
      encodeOneBlock(block,m_uArrayPos,uvStride,mQuant,1,dc_dct_pred,&U_MSE,&U_M1,&U_M2,0);
      m_uArrayPos +=8;
      encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,0);
      m_yArrayPos +=8;
      blockCount++;
      if((m_yArrayPos-m_yArray) % m_frameWidth == 0)
        m_yArrayPos += yStride*7;
      if((m_uArrayPos-m_uArray) % (m_frameWidth/2) == 0)
        m_uArrayPos += uvStride*7;
    }
    for(blkNo=m_sliceSize/2;blkNo<m_sliceSize;blkNo++)
    {
      encodeOneBlock(block,m_vArrayPos,uvStride,mQuant,2,dc_dct_pred,&V_MSE,&V_M1,&V_M2,0);
      m_vArrayPos +=8;
      encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,0);
      m_yArrayPos +=8;
      blockCount++;
      if((m_yArrayPos-m_yArray) % m_frameWidth == 0)
        m_yArrayPos += yStride*7;
      if((m_vArrayPos-m_vArray) % (m_frameWidth/2) == 0)
        m_vArrayPos += uvStride*7;
    }
  }
#endif 
  putEndFrameCode();
  endEncode();

  free(m_yArray);
  free(m_uArray);
  free(m_vArray);

  return m_bytecnt;
}

int  encodeOneSlice(UChar* a_bitmapArray,
                    FILE* a_outFp)
{

    int yStride = m_frameWidth;
    int uvStride = m_frameWidth/2;
    short block[64];
    short mQuant = MQUANT;
    double Y_MSE, Y_M1, Y_M2, U_MSE, U_M1, U_M2, V_MSE, V_M1, V_M2;
    short dc_dct_pred[3];
    int numYBlksInRow = yStride/8;
    int numYBlkRows = m_frameHeight/8;
    
    int totalNoOfBlks =  numYBlksInRow*numYBlkRows;
    int blockCount    = 0;
    int flag;
    int blkNo;

    /* ensure we were configured with a frame size of exactly one slice */
    if (totalNoOfBlks != m_sliceSize)
        return -1;
    
    m_yArray = (UChar *) malloc(m_frameWidth*m_frameHeight);
    m_uArray = (UChar *) malloc(m_frameWidth*m_frameHeight);
    m_vArray = (UChar *) malloc(m_frameWidth*m_frameHeight);
    
    if (m_yArray == NULL || m_uArray == NULL || m_vArray == NULL)
        return -1;
    
    
    Y_MSE = 0; Y_M1 = 0; Y_M2 = 0; U_MSE = 0; U_M1 = 0; U_M2 = 0; V_MSE = 0; V_M1 = 0; V_M2 = 0;
    m_yArrayPos = m_yArray; m_uArrayPos = m_uArray; m_vArrayPos = m_vArray;
    dc_dct_pred[0] = 0;
    dc_dct_pred[1] = 0;
    dc_dct_pred[2] = 0;
    
    
    initEncode(a_outFp);
    
    
    //NOTE: implementing only 4:2:2 encoding for now.
    RGBtoYUV(a_bitmapArray,m_yArrayPos,m_uArrayPos,m_vArrayPos,m_frameWidth,m_frameHeight,1);
    

    putStartSliceCode(); 
    
    flag = true;
    for(blkNo=0;blkNo<m_sliceSize/2;blkNo++)
    {
        encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,flag);
        m_yArrayPos +=8;
        encodeOneBlock(block,m_uArrayPos,uvStride,mQuant,1,dc_dct_pred,&U_MSE,&U_M1,&U_M2,flag);
        m_uArrayPos +=8;
        blockCount++;
        
        encodeOneBlock(block,m_yArrayPos,yStride,mQuant,0,dc_dct_pred,&Y_MSE,&Y_M1,&Y_M2,false);
        m_yArrayPos +=8;
        encodeOneBlock(block,m_vArrayPos,uvStride,mQuant,2,dc_dct_pred,&V_MSE,&V_M1,&V_M2,flag);
        m_vArrayPos +=8;
        blockCount++;
        flag = false;
        if((m_yArrayPos-m_yArray) % m_frameWidth == 0)
            m_yArrayPos += yStride*7;
        if((m_uArrayPos-m_uArray) % (m_frameWidth/2) == 0)
        {
            m_uArrayPos += uvStride*7;
            m_vArrayPos += uvStride*7;
        }
    }
    putEndSliceCode();
    
    endEncode();
    
    free(m_yArray);
    free(m_uArray);
    free(m_vArray);
    
    return m_bytecnt;
    
}
