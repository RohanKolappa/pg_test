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
//	BFC.Crypto.PKCS5.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.PKCS5.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, PKCS5 )

// ============================================================================

Crypto::PKCS5::PKCS5() {



}

Crypto::PKCS5::~PKCS5() {



}

// ============================================================================

#if 0

// ============================================================================

void Crypto::PKCS5::pkcs_5_alg2(
	const	Buffer	&	password,
	const	Buffer &	salt,
	const	Uint32		iteration_count,
int hash_idx,
unsigned char *out,
unsigned long *outlen)
{
	int err, itts;
	ulong32  blkno;
	unsigned long stored, left, x, y;
	unsigned char *buf[2];
	hmac_state	 *hmac;

	/* test hash IDX */
	if ((err = hash_is_valid(hash_idx)) != CRYPT_OK) {
		return err;
	}

	buf[0] = XMALLOC(MAXBLOCKSIZE * 2);
	hmac	= XMALLOC(sizeof(hmac_state));
	/* buf[1] points to the second block of MAXBLOCKSIZE bytes */
	buf[1] = buf[0] + MAXBLOCKSIZE;

	left	= *outlen;
	blkno  = 1;
	stored = 0;
	while (left != 0) {
		 /* process block number blkno */
		 zeromem(buf[0], MAXBLOCKSIZE*2);
		 
		 /* store current block number and increment for next pass */
		 STORE32H(blkno, buf[1]);
		 ++blkno;

		 /* get PRF(P, S||int(blkno)) */
		 if ((err = hmac_init(hmac, hash_idx, password, password_len)) != CRYPT_OK) { 
			 goto LBL_ERR;
		 }
		 if ((err = hmac_process(hmac, salt, salt_len)) != CRYPT_OK) {
			 goto LBL_ERR;
		 }
		 if ((err = hmac_process(hmac, buf[1], 4)) != CRYPT_OK) {
			 goto LBL_ERR;
		 }
		 x = MAXBLOCKSIZE;
		 if ((err = hmac_done(hmac, buf[0], &x)) != CRYPT_OK) {
			 goto LBL_ERR;
		 }

		 /* now compute repeated and XOR it in buf[1] */
		 XMEMCPY(buf[1], buf[0], x);
		 for (itts = 1; itts < iteration_count; ++itts) {
			  if ((err = hmac_memory(hash_idx, password, password_len, buf[0], x, buf[0], &x)) != CRYPT_OK) {
				  goto LBL_ERR;
			  }
			  for (y = 0; y < x; y++) {
					buf[1][y] ^= buf[0][y];
			  }
		 }

		 /* now emit upto x bytes of buf[1] to output */
		 for (y = 0; y < x && left != 0; ++y) {
			  out[stored++] = buf[1][y];
			  --left;
		 }
	}
	*outlen = stored;

	err = CRYPT_OK;
LBL_ERR:

	XFREE(hmac);
	XFREE(buf[0]);

	return err;
}

// ============================================================================

#endif // 0

// ============================================================================

