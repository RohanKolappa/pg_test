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
//	BFC.Crypto.DSA.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_DSA_H_
#define _BFC_Crypto_DSA_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, DSA )

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

/* Max diff between group and modulus size in bytes */
#define MDSA_DELTA	  512

/* Max DSA group size in bytes (default allows 4k-bit groups) */
#define MDSA_MAX_GROUP 512

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL DSA : virtual public SObject {

public :

	/// \brief Creates a new DSA.

	DSA(
	);

	/// \brief Destroys this object.

	virtual ~DSA(
	);

/**
  Create a DSA key
  @param prng			 An active PRNG state
  @param wprng			The index of the PRNG desired
  @param group_size	 Size of the multiplicative group (octets)
  @param modulus_size  Size of the modulus (octets)
  @param key			  [out] Where to store the created key
  @return CRYPT_OK if successful, upon error this function will free all allocated memory
*/

	void dsa_make_key(
			PRNGPtr		prng,
		const	Uint32		wprng,
		const	Uint32		group_size,
		const	Uint32		modulus_size
	);

///**
//	Free a DSA key
//	@param key	The key to free from memory
//*/
//
//void dsa_free(dsa_key *key);
//
///**
//  Sign a hash with DSA
//  @param in		 The hash to sign
//  @param inlen	 The length of the hash to sign
//  @param r		  The "r" integer of the signature (caller must initialize with mp_init() first)
//  @param s		  The "s" integer of the signature (caller must initialize with mp_init() first)
//  @param prng	  An active PRNG state
//  @param wprng	 The index of the PRNG desired
//  @param key		A private DSA key
//  @return CRYPT_OK if successful
//*/
//int dsa_sign_hash_raw(const unsigned char *in,  unsigned long inlen,
//											  void *r,	void *s,
//										 prng_state *prng, int wprng, dsa_key *key);
//
///**
//  Sign a hash with DSA
//  @param in		 The hash to sign
//  @param inlen	 The length of the hash to sign
//  @param out		[out] Where to store the signature
//  @param outlen	[in/out] The max size and resulting size of the signature
//  @param prng	  An active PRNG state
//  @param wprng	 The index of the PRNG desired
//  @param key		A private DSA key
//  @return CRYPT_OK if successful
//*/
//int dsa_sign_hash(const unsigned char *in,  unsigned long inlen,
//								unsigned char *out, unsigned long *outlen,
//								prng_state *prng, int wprng, dsa_key *key);
//
///**
//  Verify a DSA signature
//  @param r		  DSA "r" parameter
//  @param s		  DSA "s" parameter
//  @param hash	  The hash that was signed
//  @param hashlen  The length of the hash that was signed
//  @param stat	  [out] The result of the signature verification, 1==valid, 0==invalid
//  @param key		The corresponding public DH key
//  @return CRYPT_OK if successful (even if the signature is invalid)
//*/
//int dsa_verify_hash_raw(			void *r,			 void *s,
//						  const unsigned char *hash, unsigned long hashlen,
//												int *stat,		dsa_key *key);
//
///**
//  Verify a DSA signature
//  @param sig		The signature
//  @param siglen	The length of the signature (octets)
//  @param hash	  The hash that was signed
//  @param hashlen  The length of the hash that was signed
//  @param stat	  [out] The result of the signature verification, 1==valid, 0==invalid
//  @param key		The corresponding public DH key
//  @return CRYPT_OK if successful (even if the signature is invalid)
//*/
//int dsa_verify_hash(const unsigned char *sig,  unsigned long siglen,
//						  const unsigned char *hash, unsigned long hashlen,
//								  int			  *stat, dsa_key		 *key);
//
///**
//  Encrypt a symmetric key with DSA
//  @param in			The symmetric key you want to encrypt
//  @param inlen		The length of the key to encrypt (octets)
//  @param out		  [out] The destination for the ciphertext
//  @param outlen	  [in/out] The max size and resulting size of the ciphertext
//  @param prng		 An active PRNG state
//  @param wprng		The index of the PRNG you wish to use
//  @param hash		 The index of the hash you want to use
//  @param key		  The DSA key you want to encrypt to
//  @return CRYPT_OK if successful
//*/
//int dsa_encrypt_key(const unsigned char *in,	unsigned long inlen,
//								  unsigned char *out,  unsigned long *outlen,
//								  prng_state *prng, int wprng, int hash,
//								  dsa_key *key);
//
///**
//  Decrypt an DSA encrypted key
//  @param in		 The ciphertext
//  @param inlen	 The length of the ciphertext (octets)
//  @param out		[out] The plaintext
//  @param outlen	[in/out] The max size and resulting size of the plaintext
//  @param key		The corresponding private DSA key
//  @return CRYPT_OK if successful
//*/
//int dsa_decrypt_key(const unsigned char *in,  unsigned long  inlen,
//								  unsigned char *out, unsigned long *outlen,
//								  dsa_key *key);
//
///**
//  Export a DSA key to a binary packet
//  @param out	 [out] Where to store the packet
//  @param outlen [in/out] The max size and resulting size of the packet
//  @param type	The type of key to export (PK_PRIVATE or PK_PUBLIC)
//  @param key	 The key to export
//  @return CRYPT_OK if successful
//*/
//
//int dsa_export(unsigned char *out, unsigned long *outlen, int type, dsa_key *key);
//
///**
//	Import a DSA key
//	@param in		 The binary packet to import from
//	@param inlen	 The length of the binary packet
//	@param key		[out] Where to store the imported key
//	@return CRYPT_OK if successful, upon error this function will free all allocated memory
//*/
//
//int dsa_import(const unsigned char *in, unsigned long inlen, dsa_key *key);
//
///**
//	Verify a DSA key for validity
//	@param key	The key to verify
//	@param stat  [out]  Result of test, 1==valid, 0==invalid
//	@return CRYPT_OK if successful
//*/
//int dsa_verify_key(dsa_key *key, int *stat);
//
///**
//  Create a DSA shared secret between two keys
//  @param private_key		The private DSA key (the exponent)
//  @param base				 The base of the exponentiation (allows this to be used for both encrypt and decrypt)
//  @param public_key		 The public key
//  @param out				  [out] Destination of the shared secret
//  @param outlen			  [in/out] The max size and resulting size of the shared secret
//  @return CRYPT_OK if successful
//*/
//int dsa_shared_secret(void			 *private_key, void *base,
//							 dsa_key		 *public_key,
//							 unsigned char *out,			unsigned long *outlen);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Crypto.DSA" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	/** The key type, PK_PRIVATE or PK_PUBLIC */
	int type;

	/** The order of the sub-group used in octets */
	int qord;

	/** The generator  */
	void *g;

	/** The prime used to generate the sub-group */
	void *q;

	/** The large prime that generats the field the contains the sub-group */
	void *p;

	/** The private key */
	void *x;

	/** The public key */
	void *y;

	/// \brief Forbidden copy constructor.

	DSA(
		const	DSA &
	);

	/// \brief Forbidden copy operator.

	DSA & operator = (
		const	DSA &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_DSA_H_

// ============================================================================

