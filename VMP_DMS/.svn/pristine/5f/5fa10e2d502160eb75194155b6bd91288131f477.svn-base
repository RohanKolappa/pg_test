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
//	BFC.Net.URL.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Net_URL_H_
#define _BFC_Net_URL_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.URLPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.URI.h"
#include "BFC.Net.URLStreamHandler.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents a Uniform Resource Locator, a pointer to a "resource" on
///	the World Wide Web.
///
/// A resource can be something as simple as a file or a directory, or it can
/// be a reference to a more complicated object, such as a query to a database
/// or to a search engine. More information on the types of URLs and their
/// formats can be found at:
/// <tt>http://www.socs.uts.edu.au/MosaicDocs-old/url-primer.html</tt>.
///
/// In general, a URL can be broken into several parts. The previous example of
/// a URL indicates that the protocol to use is \c http (HyperText Transfer
/// Protocol) and that the information resides on a host machine named
/// <tt>www.socs.uts.edu.au</tt>. The information on that host machine is named
/// <tt>/MosaicDocs-old/url-primer.html</tt>. The exact meaning of this name on
/// the host machine is both protocol dependent and host dependent. The
/// information normally resides in a file, but it could be generated on the
/// fly. This component of the URL is called the \c path component.
///
/// A URL can optionally specify a "port", which is the port number to which
/// the TCP/UDP connection is made on the remote host machine. If the port is
/// not specified, the default port for the protocol is used instead. For
/// example, the default port for \c http is \c 80. An alternative port could
/// be specified as:
/// <tt>http://www.socs.uts.edu.au:80/MosaicDocs-old/url-primer.html</tt>.
///
/// The syntax of URL is defined by
/// <a href="http://www.ietf.org/rfc/rfc2396.txt">RFC 2396: Uniform
/// Resource Identifiers (URI): Generic Syntax</a>, amended by <a
/// href="http://www.ietf.org/rfc/rfc2732.txt">RFC 2732: Format for
/// Literal IPv6 Addresses in URLs</a>.
///
/// A URL may have appended to it a "fragment", also known as a "ref" or a
/// "reference". The fragment is indicated by the sharp sign character "#"
/// followed by more characters. For example,
/// <tt>http://java.sun.com/index.html#chapter1</tt>. This fragment is not
/// technically part of the URL. Rather, it indicates that after the specified
/// resource is retrieved, the
/// application is specifically interested in that part of the
/// document that has the tag \c chapter1 attached to it. The
/// meaning of a tag is resource specific.
///
/// An application can also specify a "relative URL",
/// which contains only enough information to reach the resource
/// relative to another URL. Relative URLs are frequently used within
/// HTML pages. For example, if the contents of the URL
/// <tt>http://java.sun.com/index.html</tt>
/// contained within it the relative URL
/// <tt>FAQ.html</tt>, it would be a shorthand for:
/// <tt>http://java.sun.com/FAQ.html</tt>.
///
/// The relative URL need not specify all the components of a URL. If
/// the protocol, host name, or port number is missing, the value is
/// inherited from the fully specified URL. The file component must be
/// specified. The optional fragment is not inherited.
///
/// The URL class does not itself encode or decode any URL components
/// according to the escaping mechanism defined in RFC2396. It is the
/// responsibility of the caller to encode any fields, which need to be
/// escaped prior to calling URL, and also to decode any escaped fields,
/// that are returned from URL. Furthermore, because URL has no knowledge
/// of URL escaping, it does not recognise equivalence between the encoded
/// or decoded form of the same URL. For example, the two URLs:
/// <tt>http://foo.com/hello world/</tt> and
/// <tt>http://foo.com/hello%20world</tt> would be considered not equal to
/// each other.
///
/// BNF:
/// <pre>
///	<URL>       ::= <protocol> '://' <authority> '/' <filename>
///	<authority> ::= [ <username> [ ':' <password> ] '@' ] <hostname> [ ':' <hostport> ]
///	<filename>  ::= <pathname> [ '?' <subquery> ] [ '#' <reference> ]
/// </pre>
///
/// Example: URL <tt>http://lucky:luke@www.disney.com:8080/get.php?id=25#conclusion</tt>
/// corresponds to:
/// \li protocol = "http"
/// \li authority = "lucky:luke@www.disney.com:8080"
/// \li username = "lucky"
/// \li password = "luke"
/// \li hostname = "www.disney.com"
/// \li hostport = 8080
/// \li filename = "get.php?id=25#conclusion"
/// \li pathname = "get.php"
/// \li subquery = "id=25"
/// \li reference = "#conclusion"
///
/// \ingroup BFC_Net

//class BFC_Net_DLL URL : public URI {
class BFC_Net_DLL URL : virtual public SObject {

public :

	static const Uint16 InvalidPort;

	/// \brief Creates a new URL.

	URL(
	);

	/// \brief Creates a new URL.
	///
	/// \param pProtocol
	///	The name of the protocol to use.
	///
	/// \param pHostName
	///	The name of the host.
	///
	/// \param port
	///	The port number on the host.
	///
	/// \param pFileName
	///	The file on the host.

	URL(
		const	Buffer &	pProtocol,
		const	Buffer &	pHostName,
		const	Buffer &	pFileName,
		const	Uint16		port = InvalidPort
	);

	/// \brief Creates a new URL by parsing the \a pSpec within \a pContext.
	///
	/// The new URL is created from the given context URL and the spec
	/// argument as described in RFC2396 "Uniform Resource Identifiers : Generic * Syntax" :
	/// <pre>
	/// 	<scheme>://<authority><path>?<query>#<fragment>
	/// </pre>
	///
	/// The reference is parsed into the scheme, authority, path, query and
	/// fragment parts. If the path component is empty and the scheme,
	/// authority, and query components are undefined, then the new URL is a
	/// reference to the current document. Otherwise, the fragment and query
	/// parts present in the spec are used in the new URL.
	///
	/// If the scheme component is defined in the given spec and does not match
	/// the scheme of the context, then the new URL is created as an absolute
	/// URL based on the spec alone. Otherwise the scheme component is inherited
	/// from the context URL.
	///
	/// If the authority component is present in the spec then the spec is
	/// treated as absolute and the spec authority and path will replace
	/// the context authority and path. If the authority component is
	/// absent in the spec then the authority of the new URL will be
	/// inherited from the context.
	///
	/// If the spec's path component begins with a slash character "/",
	/// then the path is treated as absolute and the spec path replaces the
	/// context path.
	///
	/// Otherwise, the path is treated as a relative path and is appended
	/// to the context path, as described in RFC2396. Also, in this case,
	/// the path is canonicalized through the removal of directory
	/// changes made by occurences of ".." and ".".
	///
	/// For a more detailed description of URL parsing, refer to RFC2396.
	///
	/// \param pContext
	///	The context in which to parse the specification.
	///
	/// \param pSpec
	///	The Buffer to parse as a URL.

	URL(
		const	URL &		pContext,
		const	Buffer &	pSpec
	);

	URL(
		const	Buffer &	spec
	);

	/// \brief Copy constructor.

	URL(
		const	URL &		pOther
	);

	Bool isValid(
	) const {
		return valid;
	}

	/// \brief Copy operator.

	URL & operator = (
		const	URL &		pOther
	);

	Buffer getAuth(
	) const;

	/// \brief Returns the protocol name of this URL.

	const Buffer & getProtocol(
	) const {
		return prot;
	}

	void setProtocol(
		const	Buffer &	pProt
	) {
		prot = pProt;
	}

	void setAuth(
		const	Buffer &	pAuth
	) {
		decodeAuth( prot, pAuth, user, pass, host, port );
	}

	Buffer getUserInfo(
	) const {
		Buffer res = user;
		if ( pass.hasContent() ) {
			res += ":";
			res += pass;
		}
		return res;
	}

	const Buffer & getUsername(
	) const {
		return user;
	}

	const Buffer & getPassword(
	) const {
		return pass;
	}

	Buffer getAddr(
	) const {
		return ( getHost() + ":" + Buffer( getPort() ) );
	}

	/// \brief Returns the host name of this URL, if applicable.

	const Buffer & getHost(
	) const {
		return host;
	}

	/// \brief Sets the host name of this URL to \a pName.

	void setHost(
		const	Buffer &	pName
	) {
		host = pName;
	}

	/// \brief Returns the port number of this URL.

	Uint16 getPort(
	) const {
		return port;
	}

	void setFile(
		const	Buffer &	pFile
	) {
		decodeFile( pFile, path, qury, rfrn );
	}

	/// \brief Returns the file part of this URL.

	Buffer getFile(
		const	Bool		pEscape = false
	) const {
		return encodeFile( path, qury, rfrn, pEscape );
	}

	/// \brief Returns the path part of this URL.

	const Buffer & getPath(
	) const {
		return path;
	}

	/// \brief Returns the query part of this URL.

	const Buffer & getQuery(
	) const {
		return qury;
	}

	void setQuery(
		const	Buffer &	pQuery
	) {
		qury = pQuery;
	}

	/// \brief Gets the anchor (also known as the "reference") of this
	///	object.

	const Buffer & getRef(
	) const {
		return rfrn;
	}

	/// \brief Compares this object for equality with \a pOther.
	///
	/// Two URL objects are equal if they have the same protocol, reference
	/// equivalent hosts, have the same port number on the host, and the same
	/// file and fragment of the file.
	///
	/// Two hosts are considered equivalent if both host names can be resolved
	/// into the same IP addresses; else if either host name can't be
	/// resolved, the host names must be equal without regard to case; or both
	/// host names equal to null.
	///
	/// Since hosts comparison requires name resolution, this operation is a
	/// blocking operation.

	Bool operator == (
		const	URL &		pOther
	) const;

	Bool operator != (
		const	URL &		pOther
	) const {
		return ( ! operator == ( pOther ) );
	}

	static Uint16 getDefaultPort(
		const	Buffer &	pProt
	);

// FIXME: put Authority and Filename in separate (sub)classes.

	/// \brief Decodes the authority part of an URL.
	///
	/// <authority> ::= [ <username> [ ':' <password> ] '@' ] <hostname> [ ':' <hostport> ]

	static void decodeAuth(
		const	Buffer &	pProt,
		const	Buffer &	pAuth,
			Buffer &	pUser,
			Buffer &	pPass,
			Buffer &	pHost,
			Uint16 &	pPort
	);

	/// \brief Encodes the authority part of an URL.
	///
	/// <authority> ::= [ <username> [ ':' <password> ] '@' ] <hostname> [ ':' <hostport> ]

	static Buffer encodeAuth(
		const	Buffer &	pProt,
		const	Buffer &	pUser,
		const	Buffer &	pPass,
		const	Buffer &	pHost,
		const	Uint16		pPort
	);

	/// \brief Decodes the filename part of an URL.
	///
	/// <filename> ::= <pathname> [ '?' <subquery> ] [ '#' <reference> ]

	static void decodeFile(
		const	Buffer &	pFile,
			Buffer &	pPath,
			Buffer &	pQury,
			Buffer &	pRfrn
	);

	/// \brief Encodes the filename part of an URL.
	///
	/// <filename> ::= <pathname> [ '?' <subquery> ] [ '#' <reference> ]

	static Buffer encodeFile(
		const	Buffer &	pPath,
		const	Buffer &	pQury,
		const	Buffer &	pRfrn,
		const	Bool		pEscape = false
	);

	Buffer toBuffer(
	) const;

	virtual void kill(
	);

//	URLStreamHandlerPtr getHandler(
//	) const {
//		return handler;
//	}

	IPAddrCPtr getHostAddress(
	) const {
		return hostAddr;
	}

	void setHostAddress(
			IPAddrCPtr	pAddr
	) {
		hostAddr = pAddr;
	}

	void set(
		const	Buffer &	protocol,
		const	Buffer &	host,
		const	Uint16		port,
		const	Buffer &	authority,
		const	Buffer &	userInfo,
		const	Buffer &	path,
		const	Buffer &	query,
		const	Buffer &	ref
	);

	operator Buffer (
	) const {
		return toBuffer();
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Net.URL" );

	CLASS_EXCEPTION( MalformedURL, "Malformed URL" );

protected :

	void parseURL(
		const	Buffer &	spec
	);

private :

	Buffer	prot;		///< Protocol to use (ex. "ftp", "http", "nntp", ...).
	Buffer	user;		///< Optional user name (ex. "lucky").
	Buffer	pass;		///< Optional password (ex. "luke").
	Buffer	host;		///< Host name to connect to (ex. "www.disney.com").
	Uint16	port;		///< Protocol port to connect to (ex. 8080).
	Buffer	path;		///< Path part of this URL (ex. "get.php").
	Buffer	qury;		///< Query part of this URL (ex. "id=25").
	Buffer	rfrn;		///< '#' reference (ex. "conclusion").

	Bool	valid;

//	URLStreamHandlerPtr	handler;
	IPAddrCPtr		hostAddr;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_URL_H_

// ============================================================================

