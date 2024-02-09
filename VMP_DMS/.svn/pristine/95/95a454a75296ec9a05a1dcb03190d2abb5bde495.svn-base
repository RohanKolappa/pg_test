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
//	BFC.NTLM.Response.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_NTLM_Response_H_
#define _BFC_NTLM_Response_H_

// ============================================================================

#include "BFC.NTLM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace NTLM {

BFC_PTR_DECL( BFC_NTLM_DLL, Response )

} // namespace NTLM
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace NTLM {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_NTLM

class BFC_NTLM_DLL Response : virtual public SObject {

public :

	/// \brief Creates a new nonce Response message.
	///
	/// It requires the Lan Manager and NT hashed version of user password,
	/// that can be computed from the cleartext version by calling
	/// computeNTPassword() and computeNTLMResponse().
	///
	/// The returned message should be encoded according to protocol
	/// specific rules (e.g. base 64 encoding).
	///
	/// \param host
	///	The name of the host that is authenticating.
	///
	/// \param user
	///	The name of the user.
	///
	/// \param userDomain
	///	The name of the domain to which the user belongs.
	///
	/// \param lmPassword
	///	A 16-bytes array containing the Lan Manager hashed password.
	///
	/// \param ntPassword
	///	A 16-bytes array containing the NT hashed password.
	///
	/// \param nonce
	///	A 8-byte array containing the nonce sent by server to reply to
	///	the request message.

	Response(
		const	Buffer &	host,
		const	Buffer &	user,
		const	Buffer &	userDomain,
		const	Buffer &	lmPassword,
		const	Buffer &	ntPassword,
		const	Buffer &	nonce
	);

	/// \brief Creates a new nonce Response message.
	///
	/// It requires the Lan Manager and NT hashed version of user password,
	/// that can be computed from the cleartext version by calling
	/// computeNTPassword() and computeNTLMResponse().
	///
	/// The returned message should be encoded according to protocol
	/// specific rules (e.g. base 64 encoding).
	///
	/// \param host
	///	The name of the host that is authenticating.
	///
	/// \param user
	///	The name of the user.
	///
	/// \param userDomain
	///	The name of the domain to which the user belongs.
	///
	/// \param lmPassword
	///	A 16-bytes array containing the Lan Manager hashed password.
	///
	/// \param ntPassword
	///	A 16-bytes array containing the NT hashed password.
	///
	/// \param nonce
	///	A 8-byte array containing the nonce sent by server to reply to
	///	the request message.

	const Buffer & toBuffer(
	) const {
		return data;
	}

	/// \brief Computes the Lan Manager hashed version of \a pPassword.
	///
	/// \param pPassword
	///	The user password.
	///
	/// \returns
	///	The Lan Manager hashed version of the password in a 16-bytes array.

	static Buffer computeLMPassword(
		const	Buffer &	pPassword
	);

	/// \brief Computes the NT hashed version of \a pPassword.
	///
	/// \param pPassword
	///	The user password.
	///
	/// \returns
	///	The NT hashed version of the password in a 16-bytes array.

	static Buffer computeNTPassword(
		const	Buffer &	pPassword
	);

protected :

	/// \brief Computes the NTLM response to the nonce based on the
	///	supplied hashed passwords.
	///
	/// If the hashed passwords are not available, they can be computed
	/// from the cleartext password by calling computeLMPassword() and
	/// computeNTPassword() methods.
	///
	/// \param lmPassword
	///	A 16-bytes array containing the Lan Manager hashed password.
	///
	/// \param ntPassword
	///	A 16-bytes array containing the Lan Manager hashed password.
	///
	/// \param nonce
	///	A 8-bytes array representing the server's nonce.
	///
	/// \param lmResponse
	///	A 24-bytes array that will contain the Lan Manager response
	///	after the method invocation.
	///
	/// \param ntResponse
	///	A 24-bytes array that will contain the NT response after the
	///	method invocation.

	static void computeNTLMResponse(
		const	Buffer &	lmPassword,
		const	Buffer &	ntPassword,
		const	Buffer &	nonce,
			Buffer &	lmResponse,
			Buffer &	ntResponse
	);

	/// \brief Computes an odd DES key from 56 bits represented as a
	///	7-bytes array.
	///
	/// \param keyData
	///	A byte array containing the 56 bits used to compute the DES
	///	key.
	///
	/// \returns
	///	The odd DES key generated.

	static Buffer computeDESKey(
		const	Buffer &	keyData
	);

	/// \brief Encrypts the 8-bytes plain text three times with the 3
	///	56-bits DES keys and returns the result.
	///
	/// \param keys
	///	A 21-bytes array containing 3 56-bits DES keys.
	///
	/// \param plaintext
	///	A 8-bytes array to be encrypted.
	///
	/// \returns
	///	A 24-bytes array containing the plaintext DES encrypted with
	///	the supplied keys.

	static Buffer encrypt(
		const	Buffer &	keys,
		const	Buffer &	plaintext
	);

private :

	static const Uchar	MAGIC[ 8 ];

	Buffer		data;

	/// \brief Creates a new Response.

	Response(
	);

	/// \brief Forbidden copy constructor.

	Response(
		const	Response &
	);

	/// \brief Forbidden copy operator.

	Response & operator = (
		const	Response &
	);

};

// ============================================================================

} // namespace NTLM
} // namespace BFC

// ============================================================================

#endif // _BFC_NTLM_Response_H_

// ============================================================================

