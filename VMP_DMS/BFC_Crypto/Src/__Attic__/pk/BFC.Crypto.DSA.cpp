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
//	BFC.Crypto.DSA.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.DSA.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, DSA )

// ============================================================================

Crypto::DSA::DSA() {



}

Crypto::DSA::~DSA() {



}

// ============================================================================

//void Crypto::DSA::dsa_make_key(
//		PRNGPtr		prng,
//	const	Uint32		wprng,
//	const	Uint32		group_size,
//	const	Uint32		modulus_size ) {
//
//	void			*tmp, *tmp2;
//	int				err, res;
//	unsigned char *buf;
//
//	/* check size */
//	if ( group_size >= MDSA_MAX_GROUP
//	  || group_size <= 15
//	  || group_size >= modulus_size
//	  || (modulus_size - group_size) >= MDSA_DELTA ) {
//		throw InvalidArgument();
//	}
//
//	/* allocate ram */
//	buf = XMALLOC(MDSA_DELTA);
//	if (buf == NULL) {
//		return CRYPT_MEM;
//	}
//
//	/* init mp_ints  */
//	if ((err = mp_init_multi(&tmp, &tmp2, &key->g, &key->q, &key->p, &key->x, &key->y, NULL)) != CRYPT_OK) {
//		XFREE(buf);
//		return err;
//	}
//
//	/* make our prime q */
//	if ((err = rand_prime(key->q, group_size, prng, wprng)) != CRYPT_OK)					{ goto error; }
//
//	/* double q  */
//	if ((err = mp_add(key->q, key->q, tmp)) != CRYPT_OK)										{ goto error; }
//
//	/* now make a random string and multply it against q */
//	if (prng_descriptor[wprng].read(buf+1, modulus_size - group_size, prng) != (unsigned long)(modulus_size - group_size)) {
//		err = CRYPT_ERROR_READPRNG;
//		goto error;
//	}
//
//	/* force magnitude */
//	buf[0] |= 0xC0;
//
//	/* force even */
//	buf[modulus_size - group_size - 1] &= ~1;
//
//	if ((err = mp_read_unsigned_bin(tmp2, buf, modulus_size - group_size)) != CRYPT_OK) { goto error; }
//	if ((err = mp_mul(key->q, tmp2, key->p)) != CRYPT_OK)										{ goto error; }
//	if ((err = mp_add_d(key->p, 1, key->p)) != CRYPT_OK)										{ goto error; }
//
//	/* now loop until p is prime */
//	for (;;) {
//		if ((err = mp_prime_is_prime(key->p, 8, &res)) != CRYPT_OK)							{ goto error; }
//		if (res == LTC_MP_YES) break;
//
//		/* add 2q to p and 2 to tmp2 */
//		if ((err = mp_add(tmp, key->p, key->p)) != CRYPT_OK)									{ goto error; }
//		if ((err = mp_add_d(tmp2, 2, tmp2)) != CRYPT_OK)										{ goto error; }
//	}
//
//	/* now p = (q * tmp2) + 1 is prime, find a value g for which g^tmp2 != 1 */
//	mp_set(key->g, 1);
//
//	do {
//		if ((err = mp_add_d(key->g, 1, key->g)) != CRYPT_OK)									{ goto error; }
//		if ((err = mp_exptmod(key->g, tmp2, key->p, tmp)) != CRYPT_OK)						{ goto error; }
//	} while (mp_cmp_d(tmp, 1) == LTC_MP_EQ);
//
//	/* at this point tmp generates a group of order q mod p */
//	mp_exch(tmp, key->g);
//
//	/* so now we have our DH structure, generator g, order q, modulus p
//		Now we need a random exponent [mod q] and it's power g^x mod p
//	*/
//	do {
//		if (prng_descriptor[wprng].read(buf, group_size, prng) != (unsigned long)group_size) {
//			err = CRYPT_ERROR_READPRNG;
//			goto error;
//		}
//		if ((err = mp_read_unsigned_bin(key->x, buf, group_size)) != CRYPT_OK)			{ goto error; }
//	} while (mp_cmp_d(key->x, 1) != LTC_MP_GT);
//	if ((err = mp_exptmod(key->g, key->x, key->p, key->y)) != CRYPT_OK)					{ goto error; }
//
//	key->type = PK_PRIVATE;
//	key->qord = group_size;
//
//	err = CRYPT_OK;
//	goto done;
//error:
//	mp_clear_multi(key->g, key->q, key->p, key->x, key->y, NULL);
//done:
//	mp_clear_multi(tmp, tmp2, NULL);
//	XFREE(buf);
//	return err;
//}

//void dsa_free(dsa_key *key)
//{
//	LTC_ARGCHKVD(key != NULL);
//	mp_clear_multi(key->g, key->q, key->p, key->x, key->y, NULL);
//}

// ============================================================================

#ifdef MDSA

// ============================================================================

int dsa_sign_hash_raw(const unsigned char *in,  unsigned long inlen,
											void	*r,	void *s,
										prng_state *prng, int wprng, dsa_key *key)
{
	void			*k, *kinv, *tmp;
	unsigned char *buf;
	int				err;

	LTC_ARGCHK(in  != NULL);
	LTC_ARGCHK(r	!= NULL);
	LTC_ARGCHK(s	!= NULL);
	LTC_ARGCHK(key != NULL);

	if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
		return err;
	}
	if (key->type != PK_PRIVATE) {
		return CRYPT_PK_NOT_PRIVATE;
	}

	/* check group order size  */
	if (key->qord >= MDSA_MAX_GROUP) {
		throw InvalidArgument();
	}

	buf = XMALLOC(MDSA_MAX_GROUP);
	if (buf == NULL) {
		return CRYPT_MEM;
	}

	/* Init our temps */
	if ((err = mp_init_multi(&k, &kinv, &tmp, NULL)) != CRYPT_OK)							{ goto ERRBUF; }

retry:

	do {
		/* gen random k */
		if (prng_descriptor[wprng].read(buf, key->qord, prng) != (unsigned long)key->qord) {
			err = CRYPT_ERROR_READPRNG;
			goto error;
		}

		/* read k */
		if ((err = mp_read_unsigned_bin(k, buf, key->qord)) != CRYPT_OK)					{ goto error; }

		/* k > 1 ? */
		if (mp_cmp_d(k, 1) != LTC_MP_GT)																{ goto retry; }

		/* test gcd */
		if ((err = mp_gcd(k, key->q, tmp)) != CRYPT_OK)											{ goto error; }
	} while (mp_cmp_d(tmp, 1) != LTC_MP_EQ);

	/* now find 1/k mod q */
	if ((err = mp_invmod(k, key->q, kinv)) != CRYPT_OK)											{ goto error; }

	/* now find r = g^k mod p mod q */
	if ((err = mp_exptmod(key->g, k, key->p, r)) != CRYPT_OK)									{ goto error; }
	if ((err = mp_mod(r, key->q, r)) != CRYPT_OK)													{ goto error; }

	if (mp_iszero(r) == LTC_MP_YES)																	{ goto retry; }

	/* now find s = (in + xr)/k mod q */
	if ((err = mp_read_unsigned_bin(tmp, (unsigned char *)in, inlen)) != CRYPT_OK)		{ goto error; }
	if ((err = mp_mul(key->x, r, s)) != CRYPT_OK)													{ goto error; }
	if ((err = mp_add(s, tmp, s)) != CRYPT_OK)														{ goto error; }
	if ((err = mp_mulmod(s, kinv, key->q, s)) != CRYPT_OK)										{ goto error; }

	if (mp_iszero(s) == LTC_MP_YES)																	{ goto retry; }

	err = CRYPT_OK;
error:
	mp_clear_multi(k, kinv, tmp, NULL);
ERRBUF:
#ifdef LTC_CLEAN_STACK
	zeromem(buf, MDSA_MAX_GROUP);
#endif
	XFREE(buf);
	return err;
}

int dsa_sign_hash(const unsigned char *in,  unsigned long inlen,
								unsigned char *out, unsigned long *outlen,
								prng_state *prng, int wprng, dsa_key *key)
{
	void			*r, *s;
	int			err;

	LTC_ARGCHK(in		!= NULL);
	LTC_ARGCHK(out	!= NULL);
	LTC_ARGCHK(outlen  != NULL);
	LTC_ARGCHK(key	!= NULL);

	if (mp_init_multi(&r, &s, NULL) != CRYPT_OK) {
		return CRYPT_MEM;
	}

	if ((err = dsa_sign_hash_raw(in, inlen, r, s, prng, wprng, key)) != CRYPT_OK) {
		goto error;
	}

	err = der_encode_sequence_multi(out, outlen,
									LTC_ASN1_INTEGER, 1UL, r,
									LTC_ASN1_INTEGER, 1UL, s,
									LTC_ASN1_EOL,	0UL, NULL);

error:
	mp_clear_multi(r, s, NULL);
	return err;
}

// ============================================================================

int dsa_verify_hash_raw(			void	*r,			void	*s,
						const unsigned char *hash, unsigned long hashlen,
												int *stat,		dsa_key *key)
{
	void			*w, *v, *u1, *u2;
	int			err;

	LTC_ARGCHK(r	!= NULL);
	LTC_ARGCHK(s	!= NULL);
	LTC_ARGCHK(stat != NULL);
	LTC_ARGCHK(key  != NULL);

	/* default to invalid signature */
	*stat = 0;

	/* init our variables */
	if ((err = mp_init_multi(&w, &v, &u1, &u2, NULL)) != CRYPT_OK) {
		return err;
	}

	/* neither r or s can be null or >q*/
	if (mp_iszero(r) == LTC_MP_YES || mp_iszero(s) == LTC_MP_YES || mp_cmp(r, key->q) != LTC_MP_LT || mp_cmp(s, key->q) != LTC_MP_LT) {
		err = CRYPT_INVALID_PACKET;
		goto error;
	}

	/* w = 1/s mod q */
	if ((err = mp_invmod(s, key->q, w)) != CRYPT_OK)													{ goto error; }

	/* u1 = m * w mod q */
	if ((err = mp_read_unsigned_bin(u1, (unsigned char *)hash, hashlen)) != CRYPT_OK)		{ goto error; }
	if ((err = mp_mulmod(u1, w, key->q, u1)) != CRYPT_OK)											{ goto error; }

	/* u2 = r*w mod q */
	if ((err = mp_mulmod(r, w, key->q, u2)) != CRYPT_OK)											{ goto error; }

	/* v = g^u1 * y^u2 mod p mod q */
	if ((err = mp_exptmod(key->g, u1, key->p, u1)) != CRYPT_OK)									{ goto error; }
	if ((err = mp_exptmod(key->y, u2, key->p, u2)) != CRYPT_OK)									{ goto error; }
	if ((err = mp_mulmod(u1, u2, key->p, v)) != CRYPT_OK)											{ goto error; }
	if ((err = mp_mod(v, key->q, v)) != CRYPT_OK)														{ goto error; }

	/* if r = v then we're set */
	if (mp_cmp(r, v) == LTC_MP_EQ) {
		*stat = 1;
	}

	err = CRYPT_OK;
error:
	mp_clear_multi(w, v, u1, u2, NULL);
	return err;
}

int dsa_verify_hash(const unsigned char *sig, unsigned long siglen,
						const unsigned char *hash, unsigned long hashlen,
						int *stat, dsa_key *key)
{
	int	err;
	void	*r, *s;

	if ((err = mp_init_multi(&r, &s, NULL)) != CRYPT_OK) {
		return CRYPT_MEM;
	}

	/* decode the sequence */
	if ((err = der_decode_sequence_multi(sig, siglen,
											LTC_ASN1_INTEGER, 1UL, r,
											LTC_ASN1_INTEGER, 1UL, s,
											LTC_ASN1_EOL,	0UL, NULL)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* do the op */
	err = dsa_verify_hash_raw(r, s, hash, hashlen, stat, key);

LBL_ERR:
	mp_clear_multi(r, s, NULL);
	return err;
}

// ============================================================================

int dsa_encrypt_key(const unsigned char *in,	unsigned long inlen,
								unsigned char *out,  unsigned long *outlen,
								prng_state *prng, int wprng, int hash,
								dsa_key *key)
{
	unsigned char *expt, *skey;
	void			*g_pub, *g_priv;
	unsigned long  x, y;
	int				err;

	LTC_ARGCHK(in		!= NULL);
	LTC_ARGCHK(out	!= NULL);
	LTC_ARGCHK(outlen  != NULL);
	LTC_ARGCHK(key	!= NULL);

	/* check that wprng/cipher/hash are not invalid */
	if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
		return err;
	}

	if ((err = hash_is_valid(hash)) != CRYPT_OK) {
		return err;
	}

	if (inlen > hash_descriptor[hash].hashsize) {
		return CRYPT_INVALID_HASH;
	}

	/* make a random key and export the public copy */
	if ((err = mp_init_multi(&g_pub, &g_priv, NULL)) != CRYPT_OK) {
		return err;
	}

	expt		= XMALLOC(mp_unsigned_bin_size(key->p) + 1);
	skey		= XMALLOC(MAXBLOCKSIZE);
	if (expt == NULL  || skey == NULL) {
		if (expt != NULL) {
			XFREE(expt);
		}
		if (skey != NULL) {
			XFREE(skey);
		}
		mp_clear_multi(g_pub, g_priv, NULL);
		return CRYPT_MEM;
	}

	/* make a random x, g^x pair */
	x = mp_unsigned_bin_size(key->q);
	if (prng_descriptor[wprng].read(expt, x, prng) != x) {
		err = CRYPT_ERROR_READPRNG;
		goto LBL_ERR;
	}

	/* load x */
	if ((err = mp_read_unsigned_bin(g_priv, expt, x)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* compute y */
	if ((err = mp_exptmod(key->g, g_priv, key->p, g_pub)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* make random key */
	x		= mp_unsigned_bin_size(key->p) + 1;
	if ((err = dsa_shared_secret(g_priv, key->y, key, expt, &x)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	y = MAXBLOCKSIZE;
	if ((err = hash_memory(hash, expt, x, skey, &y)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* Encrypt key */
	for (x = 0; x < inlen; x++) {
		skey[x] ^= in[x];
	}

	err = der_encode_sequence_multi(out, outlen,
												LTC_ASN1_OBJECT_IDENTIFIER,  hash_descriptor[hash].OIDlen,	hash_descriptor[hash].OID,
												LTC_ASN1_INTEGER,				1UL,									g_pub,
												LTC_ASN1_OCTET_STRING,		inlen,								skey,
												LTC_ASN1_EOL,					0UL,									NULL);

LBL_ERR:
#ifdef LTC_CLEAN_STACK
	/* clean up */
	zeromem(expt,	mp_unsigned_bin_size(key->p) + 1);
	zeromem(skey,	MAXBLOCKSIZE);
#endif

	XFREE(skey);
	XFREE(expt);

	mp_clear_multi(g_pub, g_priv, NULL);
	return err;
}

int dsa_decrypt_key(const unsigned char *in,  unsigned long  inlen,
								unsigned char *out, unsigned long *outlen,
								dsa_key *key)
{
	unsigned char  *skey, *expt;
	void			*g_pub;
	unsigned long  x, y, hashOID[32];
	int				hash, err;
	ltc_asn1_list  decode[3];

	LTC_ARGCHK(in	!= NULL);
	LTC_ARGCHK(out	!= NULL);
	LTC_ARGCHK(outlen != NULL);
	LTC_ARGCHK(key	!= NULL);

	/* right key type? */
	if (key->type != PK_PRIVATE) {
		return CRYPT_PK_NOT_PRIVATE;
	}

	/* decode to find out hash */
	LTC_SET_ASN1(decode, 0, LTC_ASN1_OBJECT_IDENTIFIER, hashOID, sizeof(hashOID)/sizeof(hashOID[0]));

	if ((err = der_decode_sequence(in, inlen, decode, 1)) != CRYPT_OK) {
		return err;
	}

	hash = find_hash_oid(hashOID, decode[0].size);
	if (hash_is_valid(hash) != CRYPT_OK) {
		return CRYPT_INVALID_PACKET;
	}

	/* we now have the hash! */

	if ((err = mp_init(&g_pub)) != CRYPT_OK) {
		return err;
	}

	/* allocate memory */
	expt	= XMALLOC(mp_unsigned_bin_size(key->p) + 1);
	skey	= XMALLOC(MAXBLOCKSIZE);
	if (expt == NULL || skey == NULL) {
		if (expt != NULL) {
			XFREE(expt);
		}
		if (skey != NULL) {
			XFREE(skey);
		}
		mp_clear(g_pub);
		return CRYPT_MEM;
	}

	LTC_SET_ASN1(decode, 1, LTC_ASN1_INTEGER,			g_pub,		1UL);
	LTC_SET_ASN1(decode, 2, LTC_ASN1_OCTET_STRING,		skey,		MAXBLOCKSIZE);

	/* read the structure in now */
	if ((err = der_decode_sequence(in, inlen, decode, 3)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* make shared key */
	x = mp_unsigned_bin_size(key->p) + 1;
	if ((err = dsa_shared_secret(key->x, g_pub, key, expt, &x)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	y = MIN(mp_unsigned_bin_size(key->p) + 1, MAXBLOCKSIZE);
	if ((err = hash_memory(hash, expt, x, expt, &y)) != CRYPT_OK) {
		goto LBL_ERR;
	}

	/* ensure the hash of the shared secret is at least as big as the encrypt itself */
	if (decode[2].size > y) {
		err = CRYPT_INVALID_PACKET;
		goto LBL_ERR;
	}

	/* avoid buffer overflow */
	if (*outlen < decode[2].size) {
		*outlen = decode[2].size;
		err = CRYPT_BUFFER_OVERFLOW;
		goto LBL_ERR;
	}

	/* Decrypt the key */
	for (x = 0; x < decode[2].size; x++) {
	out[x] = expt[x] ^ skey[x];
	}
	*outlen = x;

	err = CRYPT_OK;
LBL_ERR:
#ifdef LTC_CLEAN_STACK
	zeromem(expt,	mp_unsigned_bin_size(key->p) + 1);
	zeromem(skey,	MAXBLOCKSIZE);
#endif

	XFREE(expt);
	XFREE(skey);

	mp_clear(g_pub);

	return err;
}

// ============================================================================

int dsa_export(unsigned char *out, unsigned long *outlen, int type, dsa_key *key)
{
	unsigned char flags[1];

	LTC_ARGCHK(out	!= NULL);
	LTC_ARGCHK(outlen != NULL);
	LTC_ARGCHK(key	!= NULL);

	/* can we store the static header?  */
	if (type == PK_PRIVATE && key->type != PK_PRIVATE) {
		return CRYPT_PK_TYPE_MISMATCH;
	}

	if (type != PK_PUBLIC && type != PK_PRIVATE) {
		throw InvalidArgument();
	}

	flags[0] = (type != PK_PUBLIC) ? 1 : 0;

	if (type == PK_PRIVATE) {
		return der_encode_sequence_multi(out, outlen,
											LTC_ASN1_BIT_STRING,	1UL, flags,
											LTC_ASN1_INTEGER,		1UL, key->g,
											LTC_ASN1_INTEGER,		1UL, key->p,
											LTC_ASN1_INTEGER,		1UL, key->q,
											LTC_ASN1_INTEGER,		1UL, key->y,
											LTC_ASN1_INTEGER,		1UL, key->x,
											LTC_ASN1_EOL,			0UL, NULL);
	} else {
		return der_encode_sequence_multi(out, outlen,
											LTC_ASN1_BIT_STRING,	1UL, flags,
											LTC_ASN1_INTEGER,		1UL, key->g,
											LTC_ASN1_INTEGER,		1UL, key->p,
											LTC_ASN1_INTEGER,		1UL, key->q,
											LTC_ASN1_INTEGER,		1UL, key->y,
											LTC_ASN1_EOL,			0UL, NULL);
	}
}

int dsa_import(const unsigned char *in, unsigned long inlen, dsa_key *key)
{
	unsigned char flags[1];
	int			err;

	LTC_ARGCHK(in  != NULL);
	LTC_ARGCHK(key != NULL);
	LTC_ARGCHK(ltc_mp.name != NULL);

	/* init key */
	if (mp_init_multi(&key->p, &key->g, &key->q, &key->x, &key->y, NULL) != CRYPT_OK) {
		return CRYPT_MEM;
	}

	/* get key type */
	if ((err = der_decode_sequence_multi(in, inlen,
											LTC_ASN1_BIT_STRING, 1UL, flags,
											LTC_ASN1_EOL, 0UL, NULL)) != CRYPT_OK) {
		goto error;
	}

	if (flags[0] == 1) {
		if ((err = der_decode_sequence_multi(in, inlen,
											LTC_ASN1_BIT_STRING,	1UL, flags,
											LTC_ASN1_INTEGER,		1UL, key->g,
											LTC_ASN1_INTEGER,		1UL, key->p,
											LTC_ASN1_INTEGER,		1UL, key->q,
											LTC_ASN1_INTEGER,		1UL, key->y,
											LTC_ASN1_INTEGER,		1UL, key->x,
											LTC_ASN1_EOL,			0UL, NULL)) != CRYPT_OK) {
			goto error;
		}
		key->type = PK_PRIVATE;
	} else {
		if ((err = der_decode_sequence_multi(in, inlen,
											LTC_ASN1_BIT_STRING,	1UL, flags,
											LTC_ASN1_INTEGER,		1UL, key->g,
											LTC_ASN1_INTEGER,		1UL, key->p,
											LTC_ASN1_INTEGER,		1UL, key->q,
											LTC_ASN1_INTEGER,		1UL, key->y,
											LTC_ASN1_EOL,			0UL, NULL)) != CRYPT_OK) {
			goto error;
		}
		key->type = PK_PUBLIC;
  }
  key->qord = mp_unsigned_bin_size(key->q);

  if (key->qord >= MDSA_MAX_GROUP || key->qord <= 15 ||
		(unsigned long)key->qord >= mp_unsigned_bin_size(key->p) || (mp_unsigned_bin_size(key->p) - key->qord) >= MDSA_DELTA) {
		err = CRYPT_INVALID_PACKET;
		goto error;
	}

  return CRYPT_OK;
error:
	mp_clear_multi(key->p, key->g, key->q, key->x, key->y, NULL);
	return err;
}

// ============================================================================

int dsa_verify_key(dsa_key *key, int *stat)
{
	void	*tmp, *tmp2;
	int	res, err;

	LTC_ARGCHK(key  != NULL);
	LTC_ARGCHK(stat != NULL);

	/* default to an invalid key */
	*stat = 0;

	/* first make sure key->q and key->p are prime */
	if ((err = mp_prime_is_prime(key->q, 8, &res)) != CRYPT_OK) {
		return err;
	}
	if (res == 0) {
		return CRYPT_OK;
	}

	if ((err = mp_prime_is_prime(key->p, 8, &res)) != CRYPT_OK) {
		return err;
	}
	if (res == 0) {
		return CRYPT_OK;
	}

	/* now make sure that g is not -1, 0 or 1 and <p */
	if (mp_cmp_d(key->g, 0) == LTC_MP_EQ || mp_cmp_d(key->g, 1) == LTC_MP_EQ) {
		return CRYPT_OK;
	}
	if ((err = mp_init_multi(&tmp, &tmp2, NULL)) != CRYPT_OK)					{ return err; }
	if ((err = mp_sub_d(key->p, 1, tmp)) != CRYPT_OK)							{ goto error; }
	if (mp_cmp(tmp, key->g) == LTC_MP_EQ || mp_cmp(key->g, key->p) != LTC_MP_LT) {
		err = CRYPT_OK;
		goto error;
	}

	/* 1 < y < p-1 */
	if (!(mp_cmp_d(key->y, 1) == LTC_MP_GT && mp_cmp(key->y, tmp) == LTC_MP_LT)) {
		err = CRYPT_OK;
		goto error;
	}

	/* now we have to make sure that g^q = 1, and that p-1/q gives 0 remainder */
	if ((err = mp_div(tmp, key->q, tmp, tmp2)) != CRYPT_OK)				{ goto error; }
	if (mp_iszero(tmp2) != LTC_MP_YES) {
		err = CRYPT_OK;
		goto error;
	}

	if ((err = mp_exptmod(key->g, key->q, key->p, tmp)) != CRYPT_OK)	{ goto error; }
	if (mp_cmp_d(tmp, 1) != LTC_MP_EQ) {
		err = CRYPT_OK;
		goto error;
	}

	/* now we have to make sure that y^q = 1, this makes sure y \in g^x mod p */
	if ((err = mp_exptmod(key->y, key->q, key->p, tmp)) != CRYPT_OK)		{ goto error; }
	if (mp_cmp_d(tmp, 1) != LTC_MP_EQ) {
		err = CRYPT_OK;
		goto error;
	}

	/* at this point we are out of tests ;-( */
	err	= CRYPT_OK;
	*stat = 1;
error:
	mp_clear_multi(tmp, tmp2, NULL);
	return err;
}

// ============================================================================

int dsa_shared_secret(void			*private_key, void *base,
							dsa_key		*public_key,
							unsigned char *out,			unsigned long *outlen)
{
	unsigned long  x;
	void			*res;
	int				err;

	LTC_ARGCHK(private_key != NULL);
	LTC_ARGCHK(public_key  != NULL);
	LTC_ARGCHK(out			!= NULL);
	LTC_ARGCHK(outlen		!= NULL);

	/* make new point */
	if ((err = mp_init(&res)) != CRYPT_OK) {
		return err;
	}

	if ((err = mp_exptmod(base, private_key, public_key->p, res)) != CRYPT_OK) {
		mp_clear(res);
		return err;
	}

	x = (unsigned long)mp_unsigned_bin_size(res);
	if (*outlen < x) {
		*outlen = x;
		err = CRYPT_BUFFER_OVERFLOW;
		goto done;
	}
	zeromem(out, x);
	if ((err = mp_to_unsigned_bin(res, out + (x - mp_unsigned_bin_size(res))))	!= CRYPT_OK)			{ goto done; }

	err	= CRYPT_OK;
	*outlen = x;
done:
	mp_clear(res);
	return err;
}

// ============================================================================

#endif

// ============================================================================

