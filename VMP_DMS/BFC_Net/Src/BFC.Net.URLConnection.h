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
//	BFC.Net.URLConnection.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if 0

#ifndef _BFC_Net_URLConnection_H_
#define _BFC_Net_URLConnection_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.URLConnectionPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.IO.ByteInputStream.h"

#include "BFC.Net.ContentHandler.h"
#include "BFC.Net.ContentHandlerFactory.h"
#include "BFC.Net.NObject.h"
#include "BFC.Net.URL.h"

#include "BFC.TL.Map.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Superclass of all classes that represent a communications link
///	between the application and a URL.
///
/// Instances of this class can be used both to
/// read from and to write to the resource referenced by the URL. In
/// general, creating a connection to a URL is a multistep process:
/// -	the connection object is created by invoking the
///	openConnection() method on a URL.
/// -	the setup parameters and general request properties are manipulated.
/// -	the actual connection to the remote object is made, using the
///	connect() method.
/// -	the remote object becomes available. The header fields and the contents
///	of the remote object can be accessed.
///
/// The setup parameters are modified using the following methods:
/// -	setAllowUserInteraction()
/// -	setDoInput()
/// -	setDoOutput()
/// -	setIfModifiedSince()
/// -	setUseCaches()
///
/// and the general request properties are modified using the method:
/// -	setRequestProperty().
///
/// Default values for the AllowUserInteraction and
/// UseCaches parameters can be set using the methods
/// setDefaultAllowUserInteraction and
/// setDefaultUseCaches.
///
/// Each of the above set methods has a corresponding
/// get method to retrieve the value of the parameter or
/// general request property. The specific parameters and general
/// request properties that are applicable are protocol specific.
///
/// The following methods are used to access the header fields and
/// the contents after the connection is made to the remote object:
/// -	getContent()
/// -	getHeaderField()
/// -	getInputStream()
/// -	getOutputStream().
///
/// Certain header fields are accessed frequently. The methods:
/// -	getContentEncoding
/// -	getContentLength
/// -	getContentType
/// -	getDate
/// -	getExpiration
/// -	getLastModifed
///
/// provide convenient access to these fields. The getContentType method is
/// used by the getContent method to determine the type of the remote object;
/// subclasses may find it convenient to override the getContentType method.
///
/// In the common case, all of the pre-connection parameters and general
/// request properties can be ignored: the pre-connection parameters and
/// request properties default to sensible values. For most clients of this
/// interface, there are only two interesting methods: getIODevice() and
/// getContent(), which are mirrored in the URL class by convenience methods.
///
/// Invoking the close() methods on the InputStream or OutputStream of an
/// URLConnection after a request may free network resources associated with this
/// instance, unless particular protocol specifications specify different behaviours
/// for it.
///
/// \ingroup BFC_Net

class BFC_Net_DLL URLConnection : virtual public SObject {

protected :

	URLConnection(
			URLPtr		__url
	);

public :

	/// \brief Destroys this object.

	virtual ~URLConnection(
	);

	/// \brief Opens a communications link to the resource referenced by
	///	the URL, if such a connection has not already been established.

	virtual void connect(
	) = 0;

	/// \brief Sets a specified timeout value to be used
	///	when opening a communications link to the resource referenced
	///	by this URLConnection.

	void setConnectTimeout(
		const	Time::Delta &	timeout
	) {
		connectTimeout = timeout;
	}

	/// \brief Returns setting for connect timeout.

	const Time::Delta & getConnectTimeout(
	) const {
		return connectTimeout;
	}

	/// \brief Sets the read timeout to a specified timeout.

	void setReadTimeout(
		const	Time::Delta &	timeout
	) {
		readTimeout = timeout;
	}

	/// \brief Returns setting for read timeout.

	const Time::Delta & getReadTimeout(
	) const {
		return readTimeout;
	}

	/// \brief Returns the value of this URLConnection's URL field.

	URLPtr getURL(
	) const {
		return url;
	}

	/// \brief Returns the value of the content-length header field.

	Uint32 getContentLength(
	) {
		return getHeaderField( "content-length" ).toUint32();
	}

	/// \brief Returns the value of the content-type header field.

	Buffer getContentType(
	) const {
		return getHeaderField( "content-type" );
	}

	/// \brief Returns the value of the content-encoding header field.

	Buffer getContentEncoding(
	) const {
		return getHeaderField( "content-encoding" );
	}

	/// \brief Returns the value of the expires header field.

	Time::Clock getExpiration(
	) const {
		return getHeaderFieldDate( "expires" );
	}

	/// \brief Returns the value of the date header field.

	Time::Clock getDate(
	) const {
		return getHeaderFieldDate( "date" );
	}

	/// \brief Returns the value of the last-modified header field.

	Time::Clock getLastModified(
	) const {
		return getHeaderFieldDate( "last-modified" );
	}

	/// \brief Returns the value of the named header field.

	virtual Buffer getHeaderField(
		const	Buffer &	name
	) const = 0;

	/// \brief Returns the value of the named field parsed as date.

	virtual Time::Clock getHeaderFieldDate(
		const	Buffer &	name
	) const = 0;

//	/// \brief Returns an unmodifiable Map of the header fields.
//
//	public Map<Buffer,List<Buffer>> getHeaderFields() {
//		return Collections.EMPTY_MAP;
//	}

	/// \brief Returns the key for the n<sup>th</sup> header field.

	virtual Buffer getHeaderFieldKey(
		const	Uint32		n
	) = 0;

	/// \brief Returns the value for the n<sup>th</sup> header field.

	virtual Buffer getHeaderField(
		const	Uint32		n
	) = 0;

	/// \brief Retrieves the contents of this URL connection.

	NObjectPtr getContent(
	);

	/// \brief Returns an IO::Device that reads/writes this open
	///	connection.

	virtual IO::DevicePtr getIODevice(
	) = 0;

	virtual IO::ByteInputStreamPtr getInputStream(
	) = 0;

	/// \brief Sets the value of the doInput flag.

	void setDoInput(
		const	Bool		doinput
	);

	/// \brief Returns the value of the doInput flag.

	Bool getDoInput(
	) const {
		return doInput;
	}

	/// \brief Sets the value of the doOutput flag.

	void setDoOutput(
		const	Bool		dooutput
	);

	/// \brief Returns the value of the doOutput flag.

	Bool getDoOutput(
	) const {
		return doOutput;
	}

	/// \brief Sets the value of the allowUserInteraction flag.

	void setAllowUserInteraction(
		const	Bool		allowuserinteraction
	);

	/// \brief Returns the value of the allowUserInteraction lag.

	Bool getAllowUserInteraction(
	) const {
		return allowUserInteraction;
	}

	/// \brief Sets the default value of the allowUserInteraction flag
	///	for all future URLConnection objects.

	static void setDefaultAllowUserInteraction(
		const	Bool		defaultallowuserinteraction
	) {
		defaultAllowUserInteraction = defaultallowuserinteraction;
	}

	/// \brief Returns the default value of the allowUserInteraction flag.

	static Bool getDefaultAllowUserInteraction(
	) {
		return defaultAllowUserInteraction;
	}

	/// \brief Sets the value of the useCaches flag.

	void setUseCaches(
		const	Bool		usecaches
	);

	/// \brief Returns the value of the useCaches flag.

	Bool getUseCaches(
	) const {
		return useCaches;
	}

	/// \brief Sets the default value of the useCaches field.

	static void setDefaultUseCaches(
		const	Bool		defaultusecaches
	) {
		defaultUseCaches = defaultusecaches;
	}

	/// \brief Returns the default value of the useCaches flag.

	static Bool getDefaultUseCaches(
	) {
		return defaultUseCaches;
	}

	/// \brief Sets the value of the ifModifiedSince field.

	void setIfModifiedSince(
		const	Time::Clock &	ifmodifiedsince
	);

	/// \brief Returns the value of the ifModifiedSince field.

	const Time::Clock & getIfModifiedSince(
	) const {
		return ifModifiedSince;
	}

	/// \brief Sets the general request property.

	void setRequestProperty(
		const	Buffer &	key,
		const	Buffer &	value
	);

	/// \brief Adds a general request property.

	void addRequestProperty(
		const	Buffer &	key,
		const	Buffer &	value
	);

	/// \brief Returns the value of the named general request property.

	const Buffer & getRequestProperty(
		const	Buffer &	key
	);

//	/**
//	 * Returns an unmodifiable Map of general request
//	 * properties for this connection. The Map keys
//	 * are Strings that represent the request-header
//	 * field names. Each Map value is a unmodifiable List
//	 * of Strings that represents the corresponding
//	 * field values.
//	 *
//	 * @return	a Map of the general request properties for this connection.
//	 * @throws IllegalStateException if already connected
//	 * @since 1.4
//	 */
//
//	public Map<Buffer,List<Buffer>> getRequestProperties() {
//		if (connected)
//			throw new IllegalStateException("Already connected");
//		if (requests == null)
//			return Collections.EMPTY_MAP;
//		return requests.getHeaders(null);
//	}

	/// \brief Sets the ContentHandlerFactory of an application.

	static void setContentHandlerFactory(
			ContentHandlerFactoryPtr	fac
	) {
		factory = fac;
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Net.URLConnection" );

	CLASS_EXCEPTION( IllegalState, "Illegal state" );

protected :

	/// \brief Gets the Content Handler appropriate for this connection.

	ContentHandlerPtr getContentHandler(
	);

	Buffer stripOffParameters(
		const	Buffer &	contentType
	);

	Bool isConnected(
	) const {
		return connected;
	}

private :

	URLPtr			url;
	Bool			connected;
	Bool			doInput;
	Bool			doOutput;
	Bool			allowUserInteraction;
	Bool			useCaches;
	Time::Clock		ifModifiedSince;
	Time::Delta		connectTimeout;
	Time::Delta		readTimeout;
	Map< Buffer, Buffer >	requests;

	static Bool		defaultAllowUserInteraction;
	static Bool		defaultUseCaches;

	static ContentHandlerFactoryPtr		factory;
	static Map< Buffer, ContentHandlerPtr >	handlers;

	/// \brief Forbidden default constructor.

	URLConnection(
	);

	/// \brief Forbidden copy constructor.

	URLConnection(
		const	URLConnection&
	);

	/// \brief Forbidden copy operator.

	URLConnection& operator = (
		const	URLConnection&
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_URLConnection_H_

#endif // 0

// ============================================================================

