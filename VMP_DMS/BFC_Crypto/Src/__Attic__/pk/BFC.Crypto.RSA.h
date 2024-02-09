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
//	BFC.Crypto.RSA.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_RSA_H_
#define _BFC_Crypto_RSA_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, RSA )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Crypto.PRNG.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/* Min and Max RSA key sizes (in bits) */
#define MIN_RSA_SIZE 1024
#define MAX_RSA_SIZE 4096

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL RSA : virtual public SObject {

public :

	/// \brief Creates a new RSA.

	RSA(
	);

	/// \brief Destroys this object.

	virtual ~RSA(
	);

	/// \brief Creates an RSA key.
	///
	/// \param prng
	///	An active PRNG state.
	///
	/// \param size
	///	The size of the modulus (key size) desired (octets).
	///
	/// \param e
	///	The "e" value (public key).  e==65537 is a good choice.

//	void rsa_make_key(
//			PRNGPtr		prng,
//		const	Uint32		size,
//		const	Uint32		e
//	);

//int rsa_exptmod(const unsigned char *in,	unsigned long inlen,
//							unsigned char *out,  unsigned long *outlen, int which,
//							rsa_key *key);

/* These use PKCS #1 v2.0 padding */
#define rsa_encrypt_key(_in, _inlen, _out, _outlen, _lparam, _lparamlen, _prng, _prng_idx, _hash_idx, _key) \
  rsa_encrypt_key_ex(_in, _inlen, _out, _outlen, _lparam, _lparamlen, _prng, _prng_idx, _hash_idx, LTC_PKCS_1_OAEP, _key)

#define rsa_decrypt_key(_in, _inlen, _out, _outlen, _lparam, _lparamlen, _hash_idx, _stat, _key) \
  rsa_decrypt_key_ex(_in, _inlen, _out, _outlen, _lparam, _lparamlen, _hash_idx, LTC_PKCS_1_OAEP, _stat, _key)

#define rsa_sign_hash(_in, _inlen, _out, _outlen, _prng, _prng_idx, _hash_idx, _saltlen, _key) \
  rsa_sign_hash_ex(_in, _inlen, _out, _outlen, LTC_PKCS_1_PSS, _prng, _prng_idx, _hash_idx, _saltlen, _key)

#define rsa_verify_hash(_sig, _siglen, _hash, _hashlen, _hash_idx, _saltlen, _stat, _key) \
  rsa_verify_hash_ex(_sig, _siglen, _hash, _hashlen, LTC_PKCS_1_PSS, _hash_idx, _saltlen, _stat, _key)

///* These can be switched between PKCS #1 v2.x and PKCS #1 v1.5 paddings */
//int rsa_encrypt_key_ex(const unsigned char *in,	 unsigned long inlen,
//									 unsigned char *out,	unsigned long *outlen,
//							 const unsigned char *lparam, unsigned long lparamlen,
//							 prng_state *prng, int prng_idx, int hash_idx, int padding, rsa_key *key);
//
//int rsa_decrypt_key_ex(const unsigned char *in,		unsigned long  inlen,
//									 unsigned char *out,		unsigned long *outlen,
//							 const unsigned char *lparam,	unsigned long  lparamlen,
//									 int				hash_idx, int				padding,
//									 int			 *stat,	 rsa_key		*key);
//
//int rsa_sign_hash_ex(const unsigned char *in,		unsigned long  inlen,
//									unsigned char *out,		unsigned long *outlen,
//									int				padding,
//									prng_state	*prng,	 int				prng_idx,
//									int				hash_idx, unsigned long  saltlen,
//									rsa_key *key);
//
//int rsa_verify_hash_ex(const unsigned char *sig,		unsigned long siglen,
//							 const unsigned char *hash,	 unsigned long hashlen,
//									 int				padding,
//									 int				hash_idx, unsigned long saltlen,
//									 int			 *stat,	 rsa_key		*key);
//
///* PKCS #1 import/export */
//int rsa_export(unsigned char *out, unsigned long *outlen, int type, rsa_key *key);
//int rsa_import(const unsigned char *in, unsigned long inlen, rsa_key *key);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Crypto.RSA" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	int type;	/** Type of key, PK_PRIVATE or PK_PUBLIC */
#if 0
	Math::BigInt	e;	/** The public exponent */
	Math::BigInt	d;	/** The private exponent */
	Math::BigInt	N;	/** The modulus */
	Math::BigInt	p;	/** The p factor of N */
	Math::BigInt	q;	/** The q factor of N */
	Math::BigInt	qP;	/** The 1/q mod p CRT param */
	Math::BigInt	dP;	/** The d mod (p - 1) CRT param */
	Math::BigInt	dQ;	/** The d mod (q - 1) CRT param */
#endif // 0

	/// \brief Forbidden copy constructor.

	RSA(
		const	RSA &
	);

	/// \brief Forbidden copy operator.

	RSA & operator = (
		const	RSA &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_RSA_H_

// ============================================================================

