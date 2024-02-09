// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Crypto.DESCommon.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.Crypto.DESCommon.h"
#include "BFC.Crypto.Functions.h"

// ============================================================================

using namespace BFC;

// ============================================================================

void Crypto::DESCommon::deskey(
	const	Uchar *		key,
	const	Uint32		edf,
		Uint32 *	out ) {

	Uint32 i, j, l, m, n, kn[32];
	Uchar pc1m[56], pcr[56];

	for (j=0; j < 56; j++) {
		l = (Uint32)pc1[j];
		m = l & 7;
		pc1m[j] = (Uchar)((key[l >> 3U] & bytebit[m]) == bytebit[m] ? 1 : 0);
	}

	for (i=0; i < 16; i++) {
		if (edf == DE1) {
			m = (15 - i) << 1;
		} else {
			m = i << 1;
		}
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for (j=0; j < 28; j++) {
				l = j + (Uint32)totrot[i];
				if (l < 28) {
					pcr[j] = pc1m[l];
				} else {
					pcr[j] = pc1m[l - 28];
				}
		}
		for (/*j = 28*/; j < 56; j++) {
				l = j + (Uint32)totrot[i];
				if (l < 56) {
					pcr[j] = pc1m[l];
				} else {
					pcr[j] = pc1m[l - 28];
				}
		}
		for (j=0; j < 24; j++)  {
				if ((int)pcr[(int)pc2[j]] != 0) {
					kn[m] |= bigbyte[j];
				}
				if ((int)pcr[(int)pc2[j+24]] != 0) {
					kn[n] |= bigbyte[j];
				}
		}
	}

	cookey( kn, out );

}

// ============================================================================

void Crypto::DESCommon::desfunc(
		Uint32 *	block,
	const	Uint32 *	keys ) {

	Uint32 work, right, leftt;
	int cur_round;

	leftt = block[0];
	right = block[1];

	work = ((leftt >> 4)  ^ right) & 0x0f0f0f0fL;
	right ^= work;
	leftt ^= (work << 4);

	work = ((leftt >> 16) ^ right) & 0x0000ffffL;
	right ^= work;
	leftt ^= (work << 16);

	work = ((right >> 2)  ^ leftt) & 0x33333333L;
	leftt ^= work;
	right ^= (work << 2);

	work = ((right >> 8)  ^ leftt) & 0x00ff00ffL;
	leftt ^= work;
	right ^= (work << 8);

	right = ROL(right, 1);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	
	leftt ^= work;
	right ^= work;
	leftt = ROL(leftt, 1);

	for (cur_round = 0; cur_round < 8; cur_round++) {
		work  = ROR(right, 4) ^ *keys++;
		leftt ^= SP7[work		& 0x3fL]
				^ SP5[(work >>  8) & 0x3fL]
				^ SP3[(work >> 16) & 0x3fL]
				^ SP1[(work >> 24) & 0x3fL];
		work  = right ^ *keys++;
		leftt ^= SP8[ work		& 0x3fL]
				^  SP6[(work >>  8) & 0x3fL]
				^  SP4[(work >> 16) & 0x3fL]
				^  SP2[(work >> 24) & 0x3fL];

		work = ROR(leftt, 4) ^ *keys++;
		right ^= SP7[ work		& 0x3fL]
				^  SP5[(work >>  8) & 0x3fL]
				^  SP3[(work >> 16) & 0x3fL]
				^  SP1[(work >> 24) & 0x3fL];
		work  = leftt ^ *keys++;
		right ^= SP8[ work		& 0x3fL]
				^  SP6[(work >>  8) & 0x3fL]
				^  SP4[(work >> 16) & 0x3fL]
				^  SP2[(work >> 24) & 0x3fL];
	}

	right = ROR(right, 1);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	leftt ^= work;
	right ^= work;
	leftt = ROR(leftt, 1);
	work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
	right ^= work;
	leftt ^= (work << 8);
	/* -- */
	work = ((leftt >> 2) ^ right) & 0x33333333L;
	right ^= work;
	leftt ^= (work << 2);
	work = ((right >> 16) ^ leftt) & 0x0000ffffL;
	leftt ^= work;
	right ^= (work << 16);
	work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
	leftt ^= work;
	right ^= (work << 4);
	
	block[0] = right;
	block[1] = leftt;
}

// ============================================================================

void Crypto::DESCommon::cookey(
	const	Uint32 *	raw1,
		Uint32 *	out ) {

	Uint32 *cook;
	const Uint32 *raw0;
	Uint32 dough[32];
	Uint32 i;

	cook = dough;
	for(i=0; i < 16; i++, raw1++)
	{
		raw0 = raw1++;
		*cook	= (*raw0 & 0x00fc0000L) << 6;
		*cook	|= (*raw0 & 0x00000fc0L) << 10;
		*cook	|= (*raw1 & 0x00fc0000L) >> 10;
		*cook++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cook	= (*raw0 & 0x0003f000L) << 12;
		*cook	|= (*raw0 & 0x0000003fL) << 16;
		*cook	|= (*raw1 & 0x0003f000L) >> 4;
		*cook++ |= (*raw1 & 0x0000003fL);
	}

	System::memcpy(
		( Uchar * )out,
		( const Uchar * )dough,
		sizeof( dough ) );

}

// ============================================================================

const Uint32 Crypto::DESCommon::bytebit[8] =
{
	0200, 0100, 040, 020, 010, 04, 02, 01 
};

const Uint32 Crypto::DESCommon::bigbyte[24] =
{
	0x800000UL,  0x400000UL,  0x200000UL,  0x100000UL,
	0x80000UL,	0x40000UL,	0x20000UL,	0x10000UL,
	0x8000UL,	0x4000UL,	0x2000UL,	0x1000UL,
	0x800UL,	0x400UL,	0x200UL,	0x100UL,
	0x80UL,		0x40UL,		0x20UL,		0x10UL,
	0x8UL,		0x4UL,		0x2UL,		0x1L 
};

/* Use the key schedule specific in the standard (ANSI X3.92-1981) */

const Uchar Crypto::DESCommon::pc1[56] = {
	56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,  
	9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35, 
	62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3 
};

const Uchar Crypto::DESCommon::totrot[16] = {
	1,	2,  4,  6,
	8,  10, 12, 14, 
	15, 17, 19, 21, 
	23, 25, 27, 28
};

const Uchar Crypto::DESCommon::pc2[48] = {
	13, 16, 10, 23,  0,  4,		2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7,	15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54,	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,	45, 41, 49, 35, 28, 31
};


const Uint32 Crypto::DESCommon::SP1[64] =
{
	0x01010400UL, 0x00000000UL, 0x00010000UL, 0x01010404UL,
	0x01010004UL, 0x00010404UL, 0x00000004UL, 0x00010000UL,
	0x00000400UL, 0x01010400UL, 0x01010404UL, 0x00000400UL,
	0x01000404UL, 0x01010004UL, 0x01000000UL, 0x00000004UL,
	0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00010400UL,
	0x00010400UL, 0x01010000UL, 0x01010000UL, 0x01000404UL,
	0x00010004UL, 0x01000004UL, 0x01000004UL, 0x00010004UL,
	0x00000000UL, 0x00000404UL, 0x00010404UL, 0x01000000UL,
	0x00010000UL, 0x01010404UL, 0x00000004UL, 0x01010000UL,
	0x01010400UL, 0x01000000UL, 0x01000000UL, 0x00000400UL,
	0x01010004UL, 0x00010000UL, 0x00010400UL, 0x01000004UL,
	0x00000400UL, 0x00000004UL, 0x01000404UL, 0x00010404UL,
	0x01010404UL, 0x00010004UL, 0x01010000UL, 0x01000404UL,
	0x01000004UL, 0x00000404UL, 0x00010404UL, 0x01010400UL,
	0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00000000UL,
	0x00010004UL, 0x00010400UL, 0x00000000UL, 0x01010004UL
};

const Uint32 Crypto::DESCommon::SP2[64] =
{
	0x80108020UL, 0x80008000UL, 0x00008000UL, 0x00108020UL,
	0x00100000UL, 0x00000020UL, 0x80100020UL, 0x80008020UL,
	0x80000020UL, 0x80108020UL, 0x80108000UL, 0x80000000UL,
	0x80008000UL, 0x00100000UL, 0x00000020UL, 0x80100020UL,
	0x00108000UL, 0x00100020UL, 0x80008020UL, 0x00000000UL,
	0x80000000UL, 0x00008000UL, 0x00108020UL, 0x80100000UL,
	0x00100020UL, 0x80000020UL, 0x00000000UL, 0x00108000UL,
	0x00008020UL, 0x80108000UL, 0x80100000UL, 0x00008020UL,
	0x00000000UL, 0x00108020UL, 0x80100020UL, 0x00100000UL,
	0x80008020UL, 0x80100000UL, 0x80108000UL, 0x00008000UL,
	0x80100000UL, 0x80008000UL, 0x00000020UL, 0x80108020UL,
	0x00108020UL, 0x00000020UL, 0x00008000UL, 0x80000000UL,
	0x00008020UL, 0x80108000UL, 0x00100000UL, 0x80000020UL,
	0x00100020UL, 0x80008020UL, 0x80000020UL, 0x00100020UL,
	0x00108000UL, 0x00000000UL, 0x80008000UL, 0x00008020UL,
	0x80000000UL, 0x80100020UL, 0x80108020UL, 0x00108000UL
};

const Uint32 Crypto::DESCommon::SP3[64] =
{
	0x00000208UL, 0x08020200UL, 0x00000000UL, 0x08020008UL,
	0x08000200UL, 0x00000000UL, 0x00020208UL, 0x08000200UL,
	0x00020008UL, 0x08000008UL, 0x08000008UL, 0x00020000UL,
	0x08020208UL, 0x00020008UL, 0x08020000UL, 0x00000208UL,
	0x08000000UL, 0x00000008UL, 0x08020200UL, 0x00000200UL,
	0x00020200UL, 0x08020000UL, 0x08020008UL, 0x00020208UL,
	0x08000208UL, 0x00020200UL, 0x00020000UL, 0x08000208UL,
	0x00000008UL, 0x08020208UL, 0x00000200UL, 0x08000000UL,
	0x08020200UL, 0x08000000UL, 0x00020008UL, 0x00000208UL,
	0x00020000UL, 0x08020200UL, 0x08000200UL, 0x00000000UL,
	0x00000200UL, 0x00020008UL, 0x08020208UL, 0x08000200UL,
	0x08000008UL, 0x00000200UL, 0x00000000UL, 0x08020008UL,
	0x08000208UL, 0x00020000UL, 0x08000000UL, 0x08020208UL,
	0x00000008UL, 0x00020208UL, 0x00020200UL, 0x08000008UL,
	0x08020000UL, 0x08000208UL, 0x00000208UL, 0x08020000UL,
	0x00020208UL, 0x00000008UL, 0x08020008UL, 0x00020200UL
};

const Uint32 Crypto::DESCommon::SP4[64] =
{
	0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
	0x00802080UL, 0x00800081UL, 0x00800001UL, 0x00002001UL,
	0x00000000UL, 0x00802000UL, 0x00802000UL, 0x00802081UL,
	0x00000081UL, 0x00000000UL, 0x00800080UL, 0x00800001UL,
	0x00000001UL, 0x00002000UL, 0x00800000UL, 0x00802001UL,
	0x00000080UL, 0x00800000UL, 0x00002001UL, 0x00002080UL,
	0x00800081UL, 0x00000001UL, 0x00002080UL, 0x00800080UL,
	0x00002000UL, 0x00802080UL, 0x00802081UL, 0x00000081UL,
	0x00800080UL, 0x00800001UL, 0x00802000UL, 0x00802081UL,
	0x00000081UL, 0x00000000UL, 0x00000000UL, 0x00802000UL,
	0x00002080UL, 0x00800080UL, 0x00800081UL, 0x00000001UL,
	0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
	0x00802081UL, 0x00000081UL, 0x00000001UL, 0x00002000UL,
	0x00800001UL, 0x00002001UL, 0x00802080UL, 0x00800081UL,
	0x00002001UL, 0x00002080UL, 0x00800000UL, 0x00802001UL,
	0x00000080UL, 0x00800000UL, 0x00002000UL, 0x00802080UL
};

const Uint32 Crypto::DESCommon::SP5[64] =
{
	0x00000100UL, 0x02080100UL, 0x02080000UL, 0x42000100UL,
	0x00080000UL, 0x00000100UL, 0x40000000UL, 0x02080000UL,
	0x40080100UL, 0x00080000UL, 0x02000100UL, 0x40080100UL,
	0x42000100UL, 0x42080000UL, 0x00080100UL, 0x40000000UL,
	0x02000000UL, 0x40080000UL, 0x40080000UL, 0x00000000UL,
	0x40000100UL, 0x42080100UL, 0x42080100UL, 0x02000100UL,
	0x42080000UL, 0x40000100UL, 0x00000000UL, 0x42000000UL,
	0x02080100UL, 0x02000000UL, 0x42000000UL, 0x00080100UL,
	0x00080000UL, 0x42000100UL, 0x00000100UL, 0x02000000UL,
	0x40000000UL, 0x02080000UL, 0x42000100UL, 0x40080100UL,
	0x02000100UL, 0x40000000UL, 0x42080000UL, 0x02080100UL,
	0x40080100UL, 0x00000100UL, 0x02000000UL, 0x42080000UL,
	0x42080100UL, 0x00080100UL, 0x42000000UL, 0x42080100UL,
	0x02080000UL, 0x00000000UL, 0x40080000UL, 0x42000000UL,
	0x00080100UL, 0x02000100UL, 0x40000100UL, 0x00080000UL,
	0x00000000UL, 0x40080000UL, 0x02080100UL, 0x40000100UL
};

const Uint32 Crypto::DESCommon::SP6[64] =
{
	0x20000010UL, 0x20400000UL, 0x00004000UL, 0x20404010UL,
	0x20400000UL, 0x00000010UL, 0x20404010UL, 0x00400000UL,
	0x20004000UL, 0x00404010UL, 0x00400000UL, 0x20000010UL,
	0x00400010UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
	0x00000000UL, 0x00400010UL, 0x20004010UL, 0x00004000UL,
	0x00404000UL, 0x20004010UL, 0x00000010UL, 0x20400010UL,
	0x20400010UL, 0x00000000UL, 0x00404010UL, 0x20404000UL,
	0x00004010UL, 0x00404000UL, 0x20404000UL, 0x20000000UL,
	0x20004000UL, 0x00000010UL, 0x20400010UL, 0x00404000UL,
	0x20404010UL, 0x00400000UL, 0x00004010UL, 0x20000010UL,
	0x00400000UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
	0x20000010UL, 0x20404010UL, 0x00404000UL, 0x20400000UL,
	0x00404010UL, 0x20404000UL, 0x00000000UL, 0x20400010UL,
	0x00000010UL, 0x00004000UL, 0x20400000UL, 0x00404010UL,
	0x00004000UL, 0x00400010UL, 0x20004010UL, 0x00000000UL,
	0x20404000UL, 0x20000000UL, 0x00400010UL, 0x20004010UL
};

const Uint32 Crypto::DESCommon::SP7[64] =
{
	0x00200000UL, 0x04200002UL, 0x04000802UL, 0x00000000UL,
	0x00000800UL, 0x04000802UL, 0x00200802UL, 0x04200800UL,
	0x04200802UL, 0x00200000UL, 0x00000000UL, 0x04000002UL,
	0x00000002UL, 0x04000000UL, 0x04200002UL, 0x00000802UL,
	0x04000800UL, 0x00200802UL, 0x00200002UL, 0x04000800UL,
	0x04000002UL, 0x04200000UL, 0x04200800UL, 0x00200002UL,
	0x04200000UL, 0x00000800UL, 0x00000802UL, 0x04200802UL,
	0x00200800UL, 0x00000002UL, 0x04000000UL, 0x00200800UL,
	0x04000000UL, 0x00200800UL, 0x00200000UL, 0x04000802UL,
	0x04000802UL, 0x04200002UL, 0x04200002UL, 0x00000002UL,
	0x00200002UL, 0x04000000UL, 0x04000800UL, 0x00200000UL,
	0x04200800UL, 0x00000802UL, 0x00200802UL, 0x04200800UL,
	0x00000802UL, 0x04000002UL, 0x04200802UL, 0x04200000UL,
	0x00200800UL, 0x00000000UL, 0x00000002UL, 0x04200802UL,
	0x00000000UL, 0x00200802UL, 0x04200000UL, 0x00000800UL,
	0x04000002UL, 0x04000800UL, 0x00000800UL, 0x00200002UL
};

const Uint32 Crypto::DESCommon::SP8[64] =
{
	0x10001040UL, 0x00001000UL, 0x00040000UL, 0x10041040UL,
	0x10000000UL, 0x10001040UL, 0x00000040UL, 0x10000000UL,
	0x00040040UL, 0x10040000UL, 0x10041040UL, 0x00041000UL,
	0x10041000UL, 0x00041040UL, 0x00001000UL, 0x00000040UL,
	0x10040000UL, 0x10000040UL, 0x10001000UL, 0x00001040UL,
	0x00041000UL, 0x00040040UL, 0x10040040UL, 0x10041000UL,
	0x00001040UL, 0x00000000UL, 0x00000000UL, 0x10040040UL,
	0x10000040UL, 0x10001000UL, 0x00041040UL, 0x00040000UL,
	0x00041040UL, 0x00040000UL, 0x10041000UL, 0x00001000UL,
	0x00000040UL, 0x10040040UL, 0x00001000UL, 0x00041040UL,
	0x10001000UL, 0x00000040UL, 0x10000040UL, 0x10040000UL,
	0x10040040UL, 0x10000000UL, 0x00040000UL, 0x10001040UL,
	0x00000000UL, 0x10041040UL, 0x00040040UL, 0x10000040UL,
	0x10040000UL, 0x10001000UL, 0x10001040UL, 0x00000000UL,
	0x10041040UL, 0x00041000UL, 0x00041000UL, 0x00001040UL,
	0x00001040UL, 0x00040040UL, 0x10000000UL, 0x10041000UL
};

// ============================================================================

