/*
 * dct.hh
 *
 *  Created on: Oct 28, 2009
 *      Author: rkale
 */

#ifndef DCT_HH_
#define DCT_HH_

// TODO: Put this in a namespace

class CFDCT {
public:

    static void FDCT(short* block);
    static void FDCTFixed(short* block);
    static void IDCT(short* block);
    static void IDCT4x4_Add128_Transpose(short * block,
            unsigned char *dest, int destStride);
    static void IDCT2x2_Add128_Transpose(short * block,
            unsigned char *dest, int destStride);

private:

    static double m_cos[8][8];
    static bool m_bIsInitialized;

    static void InitFDCT(void);
    static void FDCTOrig(short* block);

    static void FDCTReal(short* block);
    static void IDCTReal(short* block);
    static void ddct8x8s(int isgn, double **a);

    static void FDCTExp(short int *blk, int lx);


};

#endif /* DCT_HH_ */
