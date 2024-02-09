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
//	BFC.Net.HttpURLConnection.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_HttpURLConnection_H_
#define _BFC_Net_HttpURLConnection_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, HttpURLConnection )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.IO.ByteInputStream.h"

#include "BFC.Net.URL.h"
#include "BFC.Net.URLConnection.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief A URLConnection with support for HTTP-specific features.
///
/// See <A HREF="http://www.w3.org/pub/WWW/Protocols/">the spec</A> for
/// details.
///
/// Each HttpURLConnection instance is used to make a single request
/// but the underlying network connection to the HTTP server may be
/// transparently shared by other instances. Calling the close() methods
/// on the InputStream or OutputStream of an HttpURLConnection
/// after a request may free network resources associated with this
/// instance but has no effect on any shared persistent connection.
/// Calling the disconnect() method may close the underlying socket
/// if a persistent connection is otherwise idle at that time.
///
/// \ingroup BFC_Net

class BFC_Net_DLL HttpURLConnection : public URLConnection {

protected :

	/// \brief Creates a new HttpURLConnection.
	///
	/// 
	/// \param u
	///	The URL.

	HttpURLConnection(
			URLPtr		u
	);

public :

	/// \brief Returns the key for the n<sup>th</sup> header field.
	///
	/// Some implementations may treat the 0<sup>th</sup> header field as
	/// special, i.e. as the status line returned by the HTTP server. In
	/// this case, getHeaderField(0) returns the status line, but
	/// getHeaderFieldKey(0) returns null.
	///
	/// \param n
	///	An index.
	///
	/// \returns
	///	The key for the n<sup>th</sup> header field, or null if the key
	///	does not exist.

	Buffer getHeaderFieldKey(
		const	Uint32		n
	) const;

	/**
	 * This method is used to enable streaming of a HTTP request body
	 * without internal buffering, when the content length is known in
	 * advance.
	 * <p>
	 * An exception will be thrown if the application
	 * attempts to write more data than the indicated
	 * content-length, or if the application closes the OutputStream
	 * before writing the indicated amount.
	 * <p>
	 * When output streaming is enabled, authentication
	 * and redirection cannot be handled automatically.
	 * A HttpRetryException will be thrown when reading
	 * the response if authentication or redirection are required.
	 * This exception can be queried for the details of the error.
	 * <p>
	 * This method must be called before the URLConnection is connected.
	 *
	 * \param   contentLength The number of bytes which will be written
	 *		  to the OutputStream.
	 *
	 * @throws  IllegalStateException if URLConnection is already connected
	 *		  or if a different streaming mode is already enabled.
	 *
	 * @throws  IllegalArgumentException if a content length less than
	 *		  zero is specified.
	 *
	 * @see	 #setChunkedStreamingMode(int)
	 * @since 1.5
	 */

	void setFixedLengthStreamingMode(
		const	Uint32		contentLength
	);

	/**
	 * This method is used to enable streaming of a HTTP request body
	 * without internal buffering, when the content length is <b>not</b>
	 * known in advance. In this mode, chunked transfer encoding
	 * is used to send the request body. Note, not all HTTP servers
	 * support this mode.
	 * <p>
	 * When output streaming is enabled, authentication
	 * and redirection cannot be handled automatically.
	 * A HttpRetryException will be thrown when reading
	 * the response if authentication or redirection are required.
	 * This exception can be queried for the details of the error.
	 * <p>
	 * This method must be called before the URLConnection is connected.
	 *
	 * \param   chunklen The number of bytes to write in each chunk.
	 *		  If chunklen is less than or equal to zero, a default
	 *		  value will be used.
	 *
	 * @throws  IllegalStateException if URLConnection is already connected
	 *		  or if a different streaming mode is already enabled.
	 *
	 * @see	 #setFixedLengthStreamingMode(int)
	 * @since 1.5
	 */

	void setChunkedStreamingMode(
		const	Uint32		chunklen
	);

	/**
	 * Returns the value for the n<sup>th</sup> header field.
	 * Some implementations may treat the 0<sup>th</sup>
	 * header field as special, i.e. as the status line returned by the HTTP
	 * server.
	 * <p>
	 * This method can be used in conjunction with the
	 * {@link #getHeaderFieldKey getHeaderFieldKey} method to iterate through all
	 * the headers in the message.
	 *
	 * \param   n   an index, where n>=0.
	 * \returns  the value of the n<sup>th</sup> header field,
	 *		  or null if the value does not exist.
	 * @see	 java.net.HttpURLConnection#getHeaderFieldKey(int)
	 */

	Buffer getHeaderField(
		const	Uint32		n
	) const;

	/**
	 * Sets whether HTTP redirects  (requests with response code 3xx) should
	 * be automatically followed by this class.  True by default.  Applets
	 * cannot change this variable.
	 * <p>
	 * If there is a security manager, this method first calls
	 * the security manager's checkSetFactory method
	 * to ensure the operation is allowed.
	 * This could result in a SecurityException.
	 *
	 * \param set a Bool indicating whether or not
	 * to follow HTTP redirects.
	 * @exception  SecurityException  if a security manager exists and its
	 *			 checkSetFactory method doesn't
	 *			 allow the operation.
	 * @see		SecurityManager#checkSetFactory
	 * @see #getFollowRedirects()
	 */

	static void setFollowRedirects(
		const	Bool		set
	);

	/**
	 * Returns a Bool indicating
	 * whether or not HTTP redirects (3xx) should
	 * be automatically followed.
	 *
	 * \returns true if HTTP redirects should
	 * be automatically followed, <tt>false</tt> if not.
	 * @see #setFollowRedirects(Bool)
	 */

	static Bool getFollowRedirects(
	);

	/**
	 * Sets whether HTTP redirects (requests with response code 3xx) should
	 * be automatically followed by this HttpURLConnection
	 * instance.
	 * <p>
	 * The default value comes from dfltRedirect, which defaults to
	 * true.
	 *
	 * \param dfltRedirect a Bool indicating
	 * whether or not to follow HTTP redirects.
	 *
	 * @see	java.net.HttpURLConnection#instanceFollowRedirects
	 * @see #getInstanceFollowRedirects
	 * @since 1.3
	 */

	void setInstanceFollowRedirects(
		const	Bool		dfltRedirect
	);

	 /**
	 * Returns the value of this HttpURLConnection's
	 * currRedirect field.
	 *
	 * \returns  the value of this HttpURLConnection's
	 *		  currRedirect field.
	 * @see	 java.net.HttpURLConnection#instanceFollowRedirects
	 * @see #setInstanceFollowRedirects(Bool)
	 * @since 1.3
	 */

	Bool getInstanceFollowRedirects(
	) const;

	/**
	 * Set the method for the URL request, one of:
	 * <UL>
	 *  <LI>GET
	 *  <LI>POST
	 *  <LI>HEAD
	 *  <LI>OPTIONS
	 *  <LI>PUT
	 *  <LI>DELETE
	 *  <LI>TRACE
	 * </UL> are legal, subject to protocol restrictions.  The default
	 * method is GET.
	 *
	 * \param method the HTTP method
	 * @exception ProtocolException if the method cannot be reset or if
	 *			  the requested method isn't valid for HTTP.
	 * @see #getRequestMethod()
	 */

	void setRequestMethod(
		const	Buffer &	method
	);

	/**
	 * Get the request method.
	 * \returns the HTTP request method
	 * @see #setRequestMethod(java.lang.Buffer)
	 */

	Buffer getRequestMethod(
	) const;

	/**
	 * Gets the status code from an HTTP response message.
	 * For example, in the case of the following status lines:
	 * <PRE>
	 * HTTP/1.0 200 OK
	 * HTTP/1.0 401 Unauthorized
	 * </PRE>
	 * It will return 200 and 401 respectively.
	 * Returns -1 if no code can be discerned
	 * from the response (i.e., the response is not valid HTTP).
	 * @throws IOException if an error occurred connecting to the server.
	 * \returns the HTTP Status-Code, or -1
	 */

	Int32 getResponseCode(
	);

	/**
	 * Gets the HTTP response message, if any, returned along with the
	 * response code from a server.  From responses like:
	 * <PRE>
	 * HTTP/1.0 200 OK
	 * HTTP/1.0 404 Not Found
	 * </PRE>
	 * Extracts the Strings "OK" and "Not Found" respectively.
	 * Returns null if none could be discerned from the responses
	 * (the result was not valid HTTP).
	 * @throws IOException if an error occurred connecting to the server.
	 * \returns the HTTP response message, or null
	 */

	Buffer getResponseMessage(
	);

	Time::Clock getHeaderFieldDate(
		const	Buffer &	name,
		const	Time::Clock	dflt
	);

	/**
	 * Indicates that other requests to the server
	 * are unlikely in the near future. Calling disconnect()
	 * should not imply that this HttpURLConnection
	 * instance can be reused for other requests.
	 */

	virtual void disconnect(
	) = 0;

	/**
	 * Indicates if the connection is going through a proxy.
	 * \returns a Bool indicating if the connection is
	 * using a proxy.
	 */

	virtual Bool usingProxy(
	) = 0;

//	public Permission getPermission() throws IOException {
//		int port = url.getPort();
//		port = port < 0 ? 80 : port;
//		Buffer host = url.getHost() + ":" + port;
//		Permission permission = new SocketPermission(host, "connect");
//		return permission;
//	}

	/**
	* Returns the error stream if the connection failed
	* but the server sent useful data nonetheless. The
	* typical example is when an HTTP server responds
	* with a 404, which will cause a FileNotFoundException
	* to be thrown in connect, but the server sent an HTML
	* help page with suggestions as to what to do.
	*
	* <p>This method will not cause a connection to be initiated.  If
	* the connection was not connected, or if the server did not have
	* an error while connecting or if the server had an error but
	* no error data was sent, this method will return null. This is
	* the default.
	*
	* \returns an error stream if any, null if there have been no
	* errors, the connection is not connected or the server sent no
	* useful data.
	*/

	IO::ByteInputStreamPtr getErrorStream(
	);

	/**
	 * The response codes for HTTP, as of version 1.1.
	 */

	/**
	 * HTTP Status-Code 200: OK.
	 */
	static const Int32 HTTP_OK = 200;

	/**
	 * HTTP Status-Code 201: Created.
	 */
	static const Int32 HTTP_CREATED = 201;

	/**
	 * HTTP Status-Code 202: Accepted.
	 */
	static const Int32 HTTP_ACCEPTED = 202;

	/**
	 * HTTP Status-Code 203: Non-Authoritative Information.
	 */
	static const Int32 HTTP_NOT_AUTHORITATIVE = 203;

	/**
	 * HTTP Status-Code 204: No Content.
	 */
	static const Int32 HTTP_NO_CONTENT = 204;

	/**
	 * HTTP Status-Code 205: Reset Content.
	 */
	static const Int32 HTTP_RESET = 205;

	/**
	 * HTTP Status-Code 206: Partial Content.
	 */
	static const Int32 HTTP_PARTIAL = 206;

	/* 3XX: relocation/redirect */

	/**
	 * HTTP Status-Code 300: Multiple Choices.
	 */
	static const Int32 HTTP_MULT_CHOICE = 300;

	/**
	 * HTTP Status-Code 301: Moved Permanently.
	 */
	static const Int32 HTTP_MOVED_PERM = 301;

	/**
	 * HTTP Status-Code 302: Temporary Redirect.
	 */
	static const Int32 HTTP_MOVED_TEMP = 302;

	/**
	 * HTTP Status-Code 303: See Other.
	 */
	static const Int32 HTTP_SEE_OTHER = 303;

	/**
	 * HTTP Status-Code 304: Not Modified.
	 */
	static const Int32 HTTP_NOT_MODIFIED = 304;

	/**
	 * HTTP Status-Code 305: Use Proxy.
	 */
	static const Int32 HTTP_USE_PROXY = 305;

	/* 4XX: client error */

	/**
	 * HTTP Status-Code 400: Bad Request.
	 */
	static const Int32 HTTP_BAD_REQUEST = 400;

	/**
	 * HTTP Status-Code 401: Unauthorized.
	 */
	static const Int32 HTTP_UNAUTHORIZED = 401;

	/**
	 * HTTP Status-Code 402: Payment Required.
	 */
	static const Int32 HTTP_PAYMENT_REQUIRED = 402;

	/**
	 * HTTP Status-Code 403: Forbidden.
	 */
	static const Int32 HTTP_FORBIDDEN = 403;

	/**
	 * HTTP Status-Code 404: Not Found.
	 */
	static const Int32 HTTP_NOT_FOUND = 404;

	/**
	 * HTTP Status-Code 405: Method Not Allowed.
	 */
	static const Int32 HTTP_BAD_METHOD = 405;

	/**
	 * HTTP Status-Code 406: Not Acceptable.
	 */
	static const Int32 HTTP_NOT_ACCEPTABLE = 406;

	/**
	 * HTTP Status-Code 407: Proxy Authentication Required.
	 */
	static const Int32 HTTP_PROXY_AUTH = 407;

	/**
	 * HTTP Status-Code 408: Request Time-Out.
	 */
	static const Int32 HTTP_CLIENT_TIMEOUT = 408;

	/**
	 * HTTP Status-Code 409: Conflict.
	 */
	static const Int32 HTTP_CONFLICT = 409;

	/**
	 * HTTP Status-Code 410: Gone.
	 */
	static const Int32 HTTP_GONE = 410;

	/**
	 * HTTP Status-Code 411: Length Required.
	 */
	static const Int32 HTTP_LENGTH_REQUIRED = 411;

	/**
	 * HTTP Status-Code 412: Precondition Failed.
	 */
	static const Int32 HTTP_PRECON_FAILED = 412;

	/**
	 * HTTP Status-Code 413: Request Entity Too Large.
	 */
	static const Int32 HTTP_ENTITY_TOO_LARGE = 413;

	/**
	 * HTTP Status-Code 414: Request-URI Too Large.
	 */
	static const Int32 HTTP_REQ_TOO_LONG = 414;

	/**
	 * HTTP Status-Code 415: Unsupported Media Type.
	 */
	static const Int32 HTTP_UNSUPPORTED_TYPE = 415;

	/* 5XX: server error */

	/**
	 * HTTP Status-Code 500: Internal Server Error.
	 * @deprecated   it is misplaced and shouldn't have existed.
	 */
	static const Int32 HTTP_SERVER_ERROR = 500;

	/**
	 * HTTP Status-Code 500: Internal Server Error.
	 */
	static const Int32 HTTP_INTERNAL_ERROR = 500;

	/**
	 * HTTP Status-Code 501: Not Implemented.
	 */
	static const Int32 HTTP_NOT_IMPLEMENTED = 501;

	/**
	 * HTTP Status-Code 502: Bad Gateway.
	 */
	static const Int32 HTTP_BAD_GATEWAY = 502;

	/**
	 * HTTP Status-Code 503: Service Unavailable.
	 */
	static const Int32 HTTP_UNAVAILABLE = 503;

	/**
	 * HTTP Status-Code 504: Gateway Timeout.
	 */
	static const Int32 HTTP_GATEWAY_TIMEOUT = 504;

	/**
	 * HTTP Status-Code 505: HTTP Version Not Supported.
	 */
	static const Int32 HTTP_VERSION = 505;

protected :

private :

	/**
	 * The HTTP method (GET,POST,PUT,etc.).
	 */

	Buffer		method;

	/**
	 * The chunk-length when using chunked encoding streaming mode for output.
	 * A value of -1 means chunked encoding is disabled for output.
	 * @since 1.5
	 */

	Int32		chunkLength;

	/**
	 * The fixed content-length when using fixed-length streaming mode.
	 * A value of -1 means fixed-length streaming mode is disabled
	 * for output.
	 * @since 1.5
	 */

	Int32		fixedLength;

	/**
	 * An int representing the three digit HTTP Status-Code.
	 * <ul>
	 * <li> 1xx: Informational
	 * <li> 2xx: Success
	 * <li> 3xx: Redirection
	 * <li> 4xx: Client Error
	 * <li> 5xx: Server Error
	 * </ul>
	 */

	Int32		responseCode;

	/**
	 * The HTTP response message.
	 */

	Buffer		responseMessage;

	/**
	 * If true, the protocol will automatically follow redirects.
	 * If false, the protocol will not automatically follow
	 * redirects.
	 * <p>
	 * This field is set by the setInstanceFollowRedirects
	 * method. Its value is returned by the getInstanceFollowRedirects
	 * method.
	 * <p>
	 * Its default value is based on the value of the static dfltRedirect
	 * at HttpURLConnection construction time.
	 *
	 * @see	 java.net.HttpURLConnection#setInstanceFollowRedirects(Bool)
	 * @see	 java.net.HttpURLConnection#getInstanceFollowRedirects()
	 * @see	 java.net.HttpURLConnection#setFollowRedirects(Bool)
	 */

	Bool		currRedirect;

	/* do we automatically follow redirects? The default is true. */

	static Bool	dfltRedirect;

	/* valid HTTP methods */

	static const BufferArray methods;

	/* Default chunk size (including chunk header) if not specified;
	 * we want to keep this in sync with the one defined in
	 * sun.net.www.http.ChunkedOutputStream
	 */

	static const Uint32 DEFAULT_CHUNK_SIZE = 4096;

	/// \brief Forbidden default constructor.

	HttpURLConnection(
	);

	/// \brief Forbidden copy constructor.

	HttpURLConnection(
		const	HttpURLConnection &
	);

	/// \brief Forbidden copy operator.

	HttpURLConnection & operator = (
		const	HttpURLConnection &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_HttpURLConnection_H_

#endif // 0

// ============================================================================

