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
//	BFC.NTLM.HTTPHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_NTLM_HTTPHandler_H_
#define _BFC_NTLM_HTTPHandler_H_

// ============================================================================

#include "BFC.NTLM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace NTLM {

BFC_PTR_DECL( BFC_NTLM_DLL, HTTPHandler )

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

class BFC_NTLM_DLL HTTPHandler : virtual public SObject {

public :

	/// \brief Creates a new HTTPHandler.

	HTTPHandler(
	);

	/// \brief Destroys this object.

	virtual ~HTTPHandler(
	);

//	/**
//	 * <p>
//	 * Build an NTLM authorization handler for the authentication credentials
//	 * specified by system properties.
//	 * </p>
//	 * <p>
//	 * To specify the authentication information set accordingly the following
//	 * properties:
//	 * <ul>
//	 * <li><var>com.luigidragone.net.ntlm.host</var>;</li>
//	 * <li><var>com.luigidragone.net.ntlm.user</var>;</li>
//	 * <li><var>com.luigidragone.net.ntlm.hostDomain</var>;</li>
//	 * <li><var>com.luigidragone.net.ntlm.userDomain</var>;</li>
//	 * <li><var>com.luigidragone.net.ntlm.password</var>;</li>
//	 * </ul>
//	 * </p>
//	 * @exception IllegalArgumentException if a required property is undefined
//	 * @exception javax.crypto.NoSuchPaddingException if there isn't any suitable padding method
//	 * @exception NoSuchAlgorithmException if there isn't any suitable cipher algorithm
//	 */
//	public NTLMAuthorizationHandler() throws IllegalArgumentException, javax.crypto.NoSuchPaddingException, java.security.NoSuchAlgorithmException {
//		this(
//			System.getProperty("com.luigidragone.net.ntlm.host", null),
//			System.getProperty("com.luigidragone.net.ntlm.hostDomain", null),
//			System.getProperty("com.luigidragone.net.ntlm.user", null),
//			System.getProperty("com.luigidragone.net.ntlm.userDomain", null),
//			System.getProperty("com.luigidragone.net.ntlm.password", null)
//		);
//	}
//
//	/**
//	 * <p>
//	 * Build an NTLM authorization handler for the specified authentication credentials.
//	 * </p>
//	 * <p>
//	 * All the arguments are mandatory (null value are not allowed).
//	 * </p>
//	 *
//	 * @param host the name of the host that is authenticating
//	 * @param hostDomain the name of the domain to which the host belongs
//	 * @param user the name of the user
//	 * @param userDomain the name of the domain to which the user belongs
//	 * @param lmPassword a 16-bytes array containing the Lan Manager hashed password
//	 * @param ntPassword a 16-bytes array containing the NT hashed password
//	 *
//	 * @exception IllegalArgumentException if a supplied argument is invalid
//	 */
//	public NTLMAuthorizationHandler(String host, String hostDomain, String user, String userDomain, byte[] lmPassword, byte[] ntPassword) throws IllegalArgumentException {
//		if(host == null)
//			throw new IllegalArgumentException("host : null value not allowed");
//		if(hostDomain == null)
//			throw new IllegalArgumentException("hostDomain : null value not allowed");
//		if(user == null)
//			throw new IllegalArgumentException("user : null value not allowed");
//		if(userDomain == null)
//			throw new IllegalArgumentException("userDomain : null value not allowed");
//		if(lmPassword == null)
//			throw new IllegalArgumentException("lmPassword : null value not allowed");
//		if(ntPassword == null)
//			throw new IllegalArgumentException("ntPassword : null value not allowed");
//		if(lmPassword.length != 16)
//			throw new IllegalArgumentException("lmPassword : illegal size");
//		if(ntPassword.length != 16)
//			throw new IllegalArgumentException("ntPassword : illegal size");
//		this.host = host;
//		this.hostDomain = hostDomain;
//		this.user = user;
//		this.userDomain = userDomain;
//		this.lmPassword = lmPassword;
//		this.ntPassword = ntPassword;
//	}
//
//	/**
//	 * <p>
//	 * Build an NTLM authorization handler for the specified authentication credentials.
//	 * </p>
//	 * <p>
//	 * All the arguments are mandatory (null value are not allowed).
//	 * </p>
//	 *
//	 * @param host the name of the host that is authenticating
//	 * @param hostDomain the name of the domain to which the host belongs
//	 * @param user the name of the user
//	 * @param userDomain the name of the domain to which the user belongs
//	 * @param password the user's password
//	 *
//	 * @exception IllegalArgumentException if a supplied argument is invalid
//	 * @exception javax.crypto.NoSuchPaddingException if there isn't any suitable padding method
//	 * @exception NoSuchAlgorithmException if there isn't any suitable cipher algorithm
//	 */
//	public NTLMAuthorizationHandler(String host, String hostDomain, String user, String userDomain, String password) throws IllegalArgumentException, javax.crypto.NoSuchPaddingException, java.security.NoSuchAlgorithmException {
//		this(host, hostDomain, user, userDomain, NTLM.computeLMPassword(password), NTLM.computeNTPassword(password));
//	}
//
//	public HTTPClient.AuthorizationInfo getAuthorization(HTTPClient.AuthorizationInfo parm1, HTTPClient.RoRequest parm2, HTTPClient.RoResponse parm3) throws HTTPClient.AuthSchemeNotImplException, java.io.IOException {
//		try {
//			String msg;
//			if(nonce != null)
//				msg = new String(Codecs.base64Encode(NTLM.formatResponse(host, user, userDomain, lmPassword, ntPassword, nonce)));
//			else
//				msg = new String(Codecs.base64Encode(NTLM.formatRequest(host, hostDomain)));
//			return new AuthorizationInfo(parm1.getHost(), parm1.getPort(), NTLM_TAG, "", msg);
//		} catch(Exception ex) {
//			ex.printStackTrace();
//			throw new IOException();
//		}
//	}
//
//	public HTTPClient.AuthorizationInfo fixupAuthInfo(HTTPClient.AuthorizationInfo parm1, HTTPClient.RoRequest parm2, HTTPClient.AuthorizationInfo parm3, HTTPClient.RoResponse parm4) throws HTTPClient.AuthSchemeNotImplException, java.io.IOException {
//		return parm1;
//	}
//
//	public void handleAuthHeaders(HTTPClient.Response parm1, HTTPClient.RoRequest parm2, HTTPClient.AuthorizationInfo parm3, HTTPClient.AuthorizationInfo parm4) throws java.io.IOException {
//		nonce = null;
//		try {
//			String challenge = parm1.getHeader(PROXY_AUTHENTICATE_HEADER);
//			if((challenge != null) && challenge.startsWith(NTLM_TAG) && challenge.length() > 4)
//				nonce = NTLM.getNonce(Codecs.base64Decode(challenge.substring(challenge.indexOf(' ') + 1).trim()).getBytes());
//			else {
//				challenge = parm1.getHeader(WWW_AUTHENTICATE_HEADER);
//				if((challenge != null) && challenge.startsWith(NTLM_TAG) && challenge.length() > 4)
//					nonce = NTLM.getNonce(Codecs.base64Decode(challenge.substring(challenge.indexOf(' ') + 1).trim()).getBytes());
//			}
//		} catch(Exception ex) {
//			ex.printStackTrace();
//		}
//	}
//
//	public void handleAuthTrailers(HTTPClient.Response parm1, HTTPClient.RoRequest parm2, HTTPClient.AuthorizationInfo parm3, HTTPClient.AuthorizationInfo parm4) throws java.io.IOException {}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.NTLM.HTTPHandler" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

//	byte[] nonce = null;
//	String host = null;
//	String user = null;
//	String hostDomain = null;
//	String userDomain = null;
//	byte[] lmPassword = null;
//	byte[] ntPassword = null;
//	private static final String NTLM_TAG = "NTLM";
//	private static final String PROXY_AUTHENTICATE_HEADER = "Proxy-Authenticate";
//	private static final String WWW_AUTHENTICATE_HEADER = "WWW-Authenticate";

	/// \brief Forbidden copy constructor.

	HTTPHandler(
		const	HTTPHandler &
	);

	/// \brief Forbidden copy operator.

	HTTPHandler & operator = (
		const	HTTPHandler &
	);

};

// ============================================================================

} // namespace NTLM
} // namespace BFC

// ============================================================================

#endif // _BFC_NTLM_HTTPHandler_H_

// ============================================================================

