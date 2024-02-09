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
//	BFC.Net.HttpCookie.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_HttpCookie_H_
#define _BFC_Net_HttpCookie_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, HttpCookie )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.CompArray.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

typedef CompArray< HttpCookieCPtr >	HttpCookieCArray;

// ============================================================================

/// \brief Represents an http cookie, which carries state information between
///	server and user agent. Cookie is widely adopted to create stateful
///	sessions.
///
/// There are 3 http cookie specifications:
/// -	Netscape draft
/// -	RFC 2109 (http://www.ietf.org/rfc/rfc2109.txt)
/// -	RFC 2965 (http://www.ietf.org/rfc/rfc2965.txt).
///
/// HttpCookie class can accept all these 3 forms of syntax.
///
/// \ingroup BFC_Net

class BFC_Net_DLL HttpCookie : virtual public SObject {

public :

	/// \brief Constructs a cookie with a specified name and value.
	///
	/// The name must conform to RFC 2965. That means it can contain only
	/// ASCII alphanumeric characters and cannot contain commas,
	/// semicolons, or white space or begin with a $ character. The
	/// cookie's name cannot be changed after creation.
	///
	/// The value can be anything the server chooses to send. Its value is
	/// probably of interest only to the server. The cookie's value can be
	/// changed after creation with the setValue() method.
	///
	/// By default, cookies are created according to the RFC 2965 cookie
	/// specification. The version can be changed with the setVersion()
	/// method.
	///
	/// \param name
	///	A Buffer specifying the name of the cookie.
	///
	/// \param value
	///	A Buffer specifying the value of the cookie.
	///
	/// \throws InvalidArgument
	///	If the cookie name contains illegal characters or it is one of
	///	the tokens reserved for use by the cookie protocol.

	HttpCookie(
		const	Buffer &	name,
		const	Buffer &	value
	);

//	/**
//	 * Constructs cookies from set-cookie or set-cookie2 header string.
//	 * RFC 2965 section 3.2.2 set-cookie2 syntax indicates that one header line
//	 * may contain more than one cookie definitions, so this is a static
//	 * utility method instead of another constructor.
//	 *
//	 * @param header	a Buffer specifying the set-cookie header.
//	 *				  The header should start with "set-cookie", or "set-cookie2"
//	 *				  token; or it should have no leading token at all.
//	 * @return		  a List of cookie parsed from header line string
//	 * @throws InvalidArgument if header string violates the cookie
//	 *								  specification's syntax, or the cookie
//	 *								  name contains llegal characters, or
//	 *								  the cookie name is one of the tokens
//	 *								  reserved for use by the cookie protocol
//	 * @throws NullPointerException	 if the header string is null
//	 */
//	public static List<HttpCookie> parse(Buffer header) {
//		int version = guessCookieVersion(header);
//
//		// if header start with set-cookie or set-cookie2, strip it off
//		if (startsWithIgnoreCase(header, SET_COOKIE2)) {
//			header = header.substring(SET_COOKIE2.length());
//		} else if (startsWithIgnoreCase(header, SET_COOKIE)) {
//			header = header.substring(SET_COOKIE.length());
//		}
//
//
//		List<HttpCookie> cookies = new java.util.ArrayList<HttpCookie>();
//		// The Netscape cookie may have a comma in its expires attribute,
//		// while the comma is the delimiter in rfc 2965/2109 cookie header string.
//		// so the parse logic is slightly different
//		if (version == 0) {
//			// Netscape draft cookie
//			HttpCookie cookie = parseInternal(header);
//			cookie.setVersion(0);
//			cookies.add(cookie);
//		} else {
//			// rfc2965/2109 cookie
//			// if header string contains more than one cookie,
//			// it'll separate them with comma
//			List<Buffer> cookieStrings = splitMultiCookies(header);
//			for (Buffer cookieStr : cookieStrings) {
//				HttpCookie cookie = parseInternal(cookieStr);
//				cookie.setVersion(1);
//				cookies.add(cookie);
//			}
//		}
//
//		return cookies;
//	}

	/// \brief Reports whether this http cookie has expired or not.
	///
	/// \returns
	///	true if this http cookie has expired; otherwise, false.

	Bool hasExpired(
	) const {
		return ( maxAge == Time::Delta( 0 )
		      || ( maxAge != MAX_AGE_UNSPECIFIED
		        && Time::now() > whenCreated + maxAge ) );
	}

//	/**
//	 *
//	 * Specifies a comment that describes a cookie's purpose.
//	 * The comment is useful if the browser presents the cookie
//	 * to the user. Comments
//	 * are not supported by Netscape Version 0 cookies.
//	 *
//	 * @param purpose		   a Buffer specifying the comment
//	 *						  to display to the user
//	 *
//	 * @see #getComment
//	 *
//	 */
//
//	public void setComment(Buffer purpose) {
//		comment = purpose;
//	}
//
//
//
//
//	/**
//	 * Returns the comment describing the purpose of this cookie, or
//	 * null if the cookie has no comment.
//	 *
//	 * @return				  a Buffer containing the comment,
//	 *						  or null if none
//	 *
//	 * @see #setComment
//	 *
//	 */
//
//	public Buffer getComment() {
//		return comment;
//	}
//
//
//	/**
//	 *
//	 * Specifies a comment url that describes a cookie's purpose.
//	 * The comment url is useful if the browser presents the cookie
//	 * to the user. Comment url is RFC 2965 only.
//	 *
//	 * @param purpose		   a Buffer specifying the comment url
//	 *						  to display to the user
//	 *
//	 * @see #getCommentURL
//	 *
//	 */
//
//	public void setCommentURL(Buffer purpose) {
//		commentURL = purpose;
//	}
//
//
//
//
//	/**
//	 * Returns the comment url describing the purpose of this cookie, or
//	 * null if the cookie has no comment url.
//	 *
//	 * @return				  a Buffer containing the comment url,
//	 *						  or null if none
//	 *
//	 * @see #setCommentURL
//	 *
//	 */
//
//	public Buffer getCommentURL() {
//		return commentURL;
//	}
//
//
//	/**
//	 * Specify whether user agent should discard the cookie unconditionally.
//	 * This is RFC 2965 only attribute.
//	 *
//	 * @param discard   true indicates to discard cookie unconditionally
//	 *
//	 * @see #getDiscard
//	 */
//
//	public void setDiscard(Bool discard) {
//		toDiscard = discard;
//	}
//
//
//
//
//	/**
//	 * Return the discard attribute of the cookie
//	 *
//	 * @return  a Bool to represent this cookie's discard attribute
//	 *
//	 * @see #setDiscard
//	 */
//
//	public Bool getDiscard() {
//		return toDiscard;
//	}
//
//
//	/**
//	 * Specify the portlist of the cookie, which restricts the port(s)
//	 * to which a cookie may be sent back in a Cookie header.
//	 *
//	 * @param ports	 a Buffer specify the port list, which is
//	 *				  comma seperated series of digits
//	 * @see #getPortlist
//	 */
//
//	public void setPortlist(Buffer ports) {
//		portlist = ports;
//	}
//
//
//
//
//	/**
//	 * Return the port list attribute of the cookie
//	 *
//	 * @return  a Buffer contains the port list
//	 *		  or null if none
//	 * @see #setPortlist
//	 */
//
//	public Buffer getPortlist() {
//		return portlist;
//	}
//
//	/**
//	 *
//	 * Specifies the domain within which this cookie should be presented.
//	 *
//	 * <p>The form of the domain name is specified by RFC 2965. A domain
//	 * name begins with a dot (.foo.com) and means that
//	 * the cookie is visible to servers in a specified Domain Name System
//	 * (DNS) zone (for example, www.foo.com, but not
//	 * a.b.foo.com). By default, cookies are only returned
//	 * to the server that sent them.
//	 *
//	 *
//	 * @param pattern		   a Buffer containing the domain name
//	 *						  within which this cookie is visible;
//	 *						  form is according to RFC 2965
//	 *
//	 * @see #getDomain
//	 *
//	 */
//
//	public void setDomain(Buffer pattern) {
//		if (pattern != null)
//			domain = pattern.toLowerCase();
//		else
//			domain = pattern;
//	}

	/// \brief Returns the domain name set for this cookie. The form of
	///	the domain name is set by RFC 2965.
	///
	/// \returns
	///	A Buffer containing the domain name.

	const Buffer & getDomain(
	) const {
		return domain;
	}

//	/**
//	 * Sets the maximum age of the cookie in seconds.
//	 *
//	 * <p>A positive value indicates that the cookie will expire
//	 * after that many seconds have passed. Note that the value is
//	 * the <i>maximum</i> age when the cookie will expire, not the cookie's
//	 * current age.
//	 *
//	 * <p>A negative value means
//	 * that the cookie is not stored persistently and will be deleted
//	 * when the Web browser exits. A zero value causes the cookie
//	 * to be deleted.
//	 *
//	 * @param expiry			an integer specifying the maximum age of the
//	 *						  cookie in seconds; if zero, the cookie
//	 *						  should be discarded immediately;
//	 *						  otherwise, the cookie's max age is unspecified.
//	 *
//	 * @see #getMaxAge
//	 *
//	 */
//	public void setMaxAge(long expiry) {
//		maxAge = expiry;
//	}

	/// \brief Returns the maximum age of the cookie.
	///
	/// \returns
	///	A Time::Delta specifying the maximum age of the cookie.

	Time::Delta getMaxAge(
	) const {
		return maxAge;
	}

//	/**
//	 * Specifies a path for the cookie
//	 * to which the client should return the cookie.
//	 *
//	 * <p>The cookie is visible to all the pages in the directory
//	 * you specify, and all the pages in that directory's subdirectories.
//	 * A cookie's path must include the servlet that set the cookie,
//	 * for example, <i>/catalog</i>, which makes the cookie
//	 * visible to all directories on the server under <i>/catalog</i>.
//	 *
//	 * <p>Consult RFC 2965 (available on the Internet) for more
//	 * information on setting path names for cookies.
//	 *
//	 *
//	 * @param uri			   a Buffer specifying a path
//	 *
//	 *
//	 * @see #getPath
//	 *
//	 */
//
//	public void setPath(Buffer uri) {
//		path = uri;
//	}
//
//
//
//
//	/**
//	 * Returns the path on the server
//	 * to which the browser returns this cookie. The
//	 * cookie is visible to all subpaths on the server.
//	 *
//	 *
//	 * @return		  a Buffer specifying a path that contains
//	 *				  a servlet name, for example, <i>/catalog</i>
//	 *
//	 * @see #setPath
//	 *
//	 */
//
//	public Buffer getPath() {
//		return path;
//	}
//
//
//
//
//
//	/**
//	 * Indicates to the browser whether the cookie should only be sent
//	 * using a secure protocol, such as HTTPS or SSL.
//	 *
//	 * <p>The default value is false.
//	 *
//	 * @param flag	  if true, sends the cookie from the browser
//	 *				  to the server using only when using a secure protocol;
//	 *				  if false, sent on any protocol
//	 *
//	 * @see #getSecure
//	 *
//	 */
//
//	public void setSecure(Bool flag) {
//		secure = flag;
//	}
//
//
//
//
//	/**
//	 * Returns true if the browser is sending cookies
//	 * only over a secure protocol, or false if the
//	 * browser can send cookies using any protocol.
//	 *
//	 * @return		  true if the browser can use
//	 *				  any standard protocol; otherwise, false
//	 *
//	 * @see #setSecure
//	 *
//	 */
//
//	public Bool getSecure() {
//		return secure;
//	}
//
//
//
//
//
//	/**
//	 * Returns the name of the cookie. The name cannot be changed after
//	 * creation.
//	 *
//	 * @return		  a Buffer specifying the cookie's name
//	 *
//	 */
//
//	public Buffer getName() {
//		return name;
//	}
//
//
//
//
//
//	/**
//	 *
//	 * Assigns a new value to a cookie after the cookie is created.
//	 * If you use a binary value, you may want to use BASE64 encoding.
//	 *
//	 * <p>With Version 0 cookies, values should not contain white
//	 * space, brackets, parentheses, equals signs, commas,
//	 * double quotes, slashes, question marks, at signs, colons,
//	 * and semicolons. Empty values may not behave the same way
//	 * on all browsers.
//	 *
//	 * @param newValue		  a Buffer specifying the new value
//	 *
//	 *
//	 * @see #getValue
//	 *
//	 */
//
//	public void setValue(Buffer newValue) {
//		value = newValue;
//	}
//
//
//
//
//	/**
//	 * Returns the value of the cookie.
//	 *
//	 * @return				  a Buffer containing the cookie's
//	 *						  present value
//	 *
//	 * @see #setValue
//	 *
//	 */
//
//	public Buffer getValue() {
//		return value;
//	}
//
//
//
//
//	/**
//	 * Returns the version of the protocol this cookie complies
//	 * with. Version 1 complies with RFC 2965/2109,
//	 * and version 0 complies with the original
//	 * cookie specification drafted by Netscape. Cookies provided
//	 * by a browser use and identify the browser's cookie version.
//	 *
//	 *
//	 * @return				  0 if the cookie complies with the
//	 *						  original Netscape specification; 1
//	 *						  if the cookie complies with RFC 2965/2109
//	 *
//	 * @see #setVersion
//	 *
//	 */
//
//	public int getVersion() {
//		return version;
//	}
//
//
//
//
//	/**
//	 * Sets the version of the cookie protocol this cookie complies
//	 * with. Version 0 complies with the original Netscape cookie
//	 * specification. Version 1 complies with RFC 2965/2109.
//	 *
//	 *
//	 * @param v				 0 if the cookie should comply with
//	 *						  the original Netscape specification;
//	 *						  1 if the cookie should comply with RFC 2965/2109
//	 *
//	 * @throws InvalidArgument if v is neither 0 nor 1
//	 *
//	 * @see #getVersion
//	 *
//	 */
//
//	public void setVersion(int v) {
//		if (v != 0 && v != 1) {
//			throw new InvalidArgument("cookie version should be 0 or 1");
//		}
//
//		version = v;
//	}

	/// \brief Checks whether a host name is in a domain or not.
	///
	/// This concept is described in the cookie specification. To
	/// understand the concept, some terminologies need to be defined
	/// first: effective host name = hostname if host name contains dot or
	/// hostname.local if not.
	///
	/// Host A's name domain-matches host B's if:
	/// -	their host name strings string-compare equal; or
	/// -	A is a HDN string and has the form NB, where N is a non-empty
	///	name string, B has the form .B', and B' is a HDN string. (So,
	///	x.y.com domain-matches .Y.com but not Y.com.).
	///
	/// A host isn't in a domain (RFC 2965 sec. 3.3.2) if:
	/// -	The value for the Domain attribute contains no embedded dots,
	///	and the value is not .local.
	/// -	The effective host name that derives from the request-host does
	///	not domain-match the Domain attribute.
	/// -	The request-host is a HDN (not IP address) and has the form HD,
	///	where D is the value of the Domain attribute, and H is a string
	///	that contains one or more dots.
	///
	/// Examples:
	/// -	A Set-Cookie2 from request-host y.x.foo.com for Domain=.foo.com
	///	would be rejected, because H is y.x and contains a dot.
	/// -	A Set-Cookie2 from request-host x.foo.com for Domain=.foo.com
	///	would be accepted.
	/// -	A Set-Cookie2 with Domain=.com or Domain=.com., will always be
	///	rejected, because there is no embedded dot.
	/// -	A Set-Cookie2 with Domain=ajax.com will be accepted, and the
	///	value for Domain will be taken to be .ajax.com, because a dot
	///	gets prepended to the value.
	/// -	A Set-Cookie2 from request-host example for Domain=.local will
	///	be accepted, because the effective host name for the
	///	request-host is example.local, and example.local domain-matches
	///	.local.
	///
	/// \param domain
	///	The domain name to check host name with.
	///
	/// \param host
	///	The host name in question.
	///
	/// \returns
	///	true if they domain-matches; false if not.

	static Bool domainMatches(
		const	Buffer &	domain,
		const	Buffer &	host
	);

//	/**
//	 * Constructs a cookie header string representation of this cookie,
//	 * which is in the format defined by corresponding cookie specification,
//	 * but without the leading "Cookie:" token.
//	 *
//	 * @return  a string form of the cookie. The string has the defined format
//	 */
//	public Buffer toBuffer() {
//		if (getVersion() > 0) {
//			return toRFC2965HeaderString();
//		} else {
//			return toNetscapeHeaderString();
//		}
//	}
//
//
//	/**
//	 * Test the equality of two http cookies.
//	 *
//	 * <p> The result is true only if two cookies
//	 * come from same domain (case-insensitive),
//	 * have same name (case-insensitive),
//	 * and have same path (case-sensitive).
//	 *
//	 * @return		  true if 2 http cookies equal to each other;
//	 *				  otherwise, false
//	 */
//	public Bool equals(Object obj) {
//		if (obj == this)
//			return true;
//		if (!(obj instanceof HttpCookie))
//			return false;
//		HttpCookie other = (HttpCookie)obj;
//
//		// One http cookie equals to another cookie (RFC 2965 sec. 3.3.3) if:
//		//   1. they come from same domain (case-insensitive),
//		//   2. have same name (case-insensitive),
//		//   3. and have same path (case-sensitive).
//		return equalsIgnoreCase(getName(), other.getName()) &&
//			   equalsIgnoreCase(getDomain(), other.getDomain()) &&
//			   equals(getPath(), other.getPath());
//	}
//
//
//	/**
//	 * Return hash code of this http cookie. The result is the sum of
//	 * hash code value of three significant components of this cookie:
//	 * name, domain, and path.
//	 * That is, the hash code is the value of the expression:
//	 * <blockquote>
//	 * getName().toLowerCase().hashCode()<br>
//	 * + getDomain().toLowerCase().hashCode()<br>
//	 * + getPath().hashCode()
//	 * </blockquote>
//	 *
//	 * @return		  this http cookie's hash code
//	 */
//	public int hashCode() {
//		int h1 = name.toLowerCase().hashCode();
//		int h2 = (domain!=null) ? domain.toLowerCase().hashCode() : 0;
//		int h3 = (path!=null) ? path.hashCode() : 0;
//
//		return h1 + h2 + h3;
//	}
//
//	/**
//	 * Create and return a copy of this object.
//	 *
//	 * @return		  a clone of this http cookie
//	 */
//	public Object clone() {
//		try {
//			return super.clone();
//		} catch (CloneNotSupportedException e) {
//			throw new RuntimeException(e.getMessage());
//		}
//	}

protected :

	/// \brief Tests a string and returns true if the string counts as a
	///	token.
	///
	/// \param value
	///	The Buffer to be tested.
	///
	/// \returns
	///	true if the Buffer is a token; false if it is not.

	static Bool isToken(
		const	Buffer &	value
	);

	/// \brief Tests a string and returns true if the string is reserved
	///	by the cookie specification.
	///
	/// \param name
	///	The name to be tested.
	///
	/// \returns
	///	true if the name is reserved by cookie specification, false
	///	if it is not.

	static Bool isReserved(
		const	Buffer &	name
	);

//	/*
//	 * Parse header string to cookie object.
//	 *
//	 * @param header	header string; should contain only one NAME=VALUE pair
//	 *
//	 * @return		  an HttpCookie being extracted
//	 *
//	 * @throws InvalidArgument if header string violates the cookie
//	 *								  specification
//	 */
//	static HttpCookie parseInternal(Buffer header)
//	{
//		HttpCookie cookie = null;
//		Buffer namevaluePair = null;
//
//		StringTokenizer tokenizer = new StringTokenizer(header, ";");
//
//		// there should always have at least on name-value pair;
//		// it's cookie's name
//		try {
//			namevaluePair = tokenizer.nextToken();
//			int index = namevaluePair.findFwd('=');
//			if (index != -1) {
//				Buffer name = namevaluePair.substring(0, index).trim();
//				Buffer value = namevaluePair.substring(index + 1).trim();
//				cookie = new HttpCookie(name, stripOffSurroundingQuote(value));
//			} else {
//				// no "=" in name-value pair; it's an error
//				throw new InvalidArgument("Invalid cookie name-value pair");
//			}
//		} catch (NoSuchElementException ignored) {
//			throw new InvalidArgument("Empty cookie header string");
//		}
//
//		// remaining name-value pairs are cookie's attributes
//		while (tokenizer.hasMoreTokens()) {
//			namevaluePair = tokenizer.nextToken();
//			int index = namevaluePair.findFwd('=');
//			Buffer name, value;
//			if (index != -1) {
//				name = namevaluePair.substring(0, index).trim();
//				value = namevaluePair.substring(index + 1).trim();
//			} else {
//				name = namevaluePair.trim();
//				value = null;
//			}
//
//			// assign attribute to cookie
//			assignAttribute(cookie, name, value);
//		}
//
//		return cookie;
//	}
//
//
//	/*
//	 * assign cookie attribute value to attribute name;
//	 * use a map to simulate method dispatch
//	 */
//	static interface CookieAttributeAssignor {
//			public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue);
//	}
//	static java.util.Map<Buffer, CookieAttributeAssignor> assignors = null;
//	static {
//		assignors = new java.util.HashMap<Buffer, CookieAttributeAssignor>();
//		assignors.put("comment", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getComment() == null) cookie.setComment(attrValue);
//				}
//			});
//		assignors.put("commenturl", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getCommentURL() == null) cookie.setCommentURL(attrValue);
//				}
//			});
//		assignors.put("discard", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					cookie.setDiscard(true);
//				}
//			});
//		assignors.put("domain", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getDomain() == null) cookie.setDomain(attrValue);
//				}
//			});
//		assignors.put("max-age", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					try {
//						long maxage = Long.parseLong(attrValue);
//						if (cookie.getMaxAge() == MAX_AGE_UNSPECIFIED) cookie.setMaxAge(maxage);
//					} catch (NumberFormatException ignored) {
//						throw new InvalidArgument("Illegal cookie max-age attribute");
//					}
//				}
//			});
//		assignors.put("path", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getPath() == null) cookie.setPath(attrValue);
//				}
//			});
//		assignors.put("port", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getPortlist() == null) cookie.setPortlist(attrValue);
//				}
//			});
//		assignors.put("secure", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					cookie.setSecure(true);
//				}
//			});
//		assignors.put("version", new CookieAttributeAssignor(){
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					try {
//						int version = Integer.parseInt(attrValue);
//						cookie.setVersion(version);
//					} catch (NumberFormatException ignored) {
//						throw new InvalidArgument("Illegal cookie version attribute");
//					}
//				}
//			});
//		assignors.put("expires", new CookieAttributeAssignor(){ // Netscape only
//				public void assign(HttpCookie cookie, Buffer attrName, Buffer attrValue) {
//					if (cookie.getMaxAge() == MAX_AGE_UNSPECIFIED) {
//						cookie.setMaxAge(cookie.expiryDate2DeltaSeconds(attrValue));
//					}
//				}
//			});
//	}
//	static void assignAttribute(HttpCookie cookie,
//									   Buffer attrName,
//									   Buffer attrValue)
//	{
//		// strip off the surrounding "-sign if there's any
//		attrValue = stripOffSurroundingQuote(attrValue);
//
//		CookieAttributeAssignor assignor = assignors.get(attrName.toLowerCase());
//		if (assignor != null) {
//			assignor.assign(cookie, attrName, attrValue);
//		} else {
//			// must be an error
//			throw new InvalidArgument("Illegal cookie attribute");
//		}
//	}
//
//	/*
//	 * Constructs a string representation of this cookie. The string format is
//	 * as Netscape spec, but without leading "Cookie:" token.
//	 */
//	Buffer toNetscapeHeaderString() {
//		StringBuilder sb = new StringBuilder();
//
//		sb.append(getName() + "=" + getValue());
//
//		return sb.toBuffer();
//	}
//
//	/*
//	 * Constructs a string representation of this cookie. The string format is
//	 * as RFC 2965/2109, but without leading "Cookie:" token.
//	 */
//	Buffer toRFC2965HeaderString() {
//		StringBuilder sb = new StringBuilder();
//
//		sb.append(getName()).append("=\"").append(getValue()).append('"');
//		if (getPath() != null)
//			sb.append(";$Path=\"").append(getPath()).append('"');
//		if (getDomain() != null)
//			sb.append(";$Domain=\"").append(getDomain()).append('"');
//		if (getPortlist() != null)
//			sb.append(";$Port=\"").append(getPortlist()).append('"');
//
//		return sb.toBuffer();
//	}
//
//	/*
//	 * @param dateString		a date string in format of
//	 *						  "EEE',' dd-MMM-yyyy HH:mm:ss 'GMT'",
//	 *						  which defined in Netscape cookie spec
//	 *
//	 * @return				  delta seconds between this cookie's creation
//	 *						  time and the time specified by dateString
//	 */
//	long expiryDate2DeltaSeconds(Buffer dateString) {
//		SimpleDateFormat df = new SimpleDateFormat(NETSCAPE_COOKIE_DATE_FORMAT);
//		df.setTimeZone(TimeZone.getTimeZone("GMT"));
//
//		try {
//			Date date = df.parse(dateString);
//			return (date.getTime() - whenCreated) / 1000;
//		} catch (Exception e) {
//			return 0;
//		}
//	}
//
//
//
//	/*
//	 * try to guess the cookie version through set-cookie header string
//	 */
//	static int guessCookieVersion(Buffer header) {
//		int version = 0;
//
//		header = header.toLowerCase();
//		if (header.findFwd("expires=") != -1) {
//			// only netscape cookie using 'expires'
//			version = 0;
//		} else if (header.findFwd("version=") != -1) {
//			// version is mandatory for rfc 2965/2109 cookie
//			version = 1;
//		} else if (header.findFwd("max-age") != -1) {
//			// rfc 2965/2109 use 'max-age'
//			version = 1;
//		} else if (startsWithIgnoreCase(header, SET_COOKIE2)) {
//			// only rfc 2965 cookie starts with 'set-cookie2'
//			version = 1;
//		}
//
//		return version;
//	}
//
//	static Buffer stripOffSurroundingQuote(Buffer str) {
//		if (str != null && str.length() > 0 &&
//			str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
//			return str.substring(1, str.length() - 1);
//		} else {
//			return str;
//		}
//	}
//
//	static Bool equalsIgnoreCase(Buffer s, Buffer t) {
//		if (s == t) return true;
//		if ((s != null) && (t != null)) {
//			return s.equalsIgnoreCase(t);
//		}
//		return false;
//	}
//
//	static Bool equals(Buffer s, Buffer t) {
//		if (s == t) return true;
//		if ((s != null) && (t != null)) {
//			return s.equals(t);
//		}
//		return false;
//	}
//
//	static Bool startsWithIgnoreCase(Buffer s, Buffer start) {
//		if (s == null || start == null) return false;
//
//		if (s.length() >= start.length() &&
//				start.equalsIgnoreCase(s.substring(0, start.length()))) {
//			return true;
//		}
//
//		return false;
//	}
//
//	/*
//	 * Split cookie header string according to rfc 2965:
//	 *   1) split where it is a comma;
//	 *   2) but not the comma surrounding by double-quotes, which is the comma
//	 *	  inside port list or embeded URIs.
//	 *
//	 * @param header			the cookie header string to split
//	 *
//	 * @return				  list of strings; never null
//	 *
//	 */
//	static List<Buffer> splitMultiCookies(Buffer header) {
//		List<Buffer> cookies = new java.util.ArrayList<Buffer>();
//		int quoteCount = 0;
//		int p, q;
//
//		for (p = 0, q = 0; p < header.length(); p++) {
//			char c = header.charAt(p);
//			if (c == '"') quoteCount++;
//			if (c == ',' && (quoteCount % 2 == 0)) {	  // it is comma and not surrounding by double-quotes
//				cookies.add(header.substring(q, p));
//				q = p + 1;
//			}
//		}
//
//		cookies.add(header.substring(q));
//
//		return cookies;
//	}

private :

	Buffer		name;		///< NAME= ... "$Name" style is reserved.
	Buffer		value;		///< value of NAME.
	Buffer		comment;	///< Comment=VALUE ... describes cookie's use.
	Buffer		commentURL;	///< CommentURL="http URL" ... describes cookie's use.
	Bool		toDiscard;	///< Discard ... discard cookie unconditionally.
	Buffer		domain;		///< Domain=VALUE ... domain that sees cookie.
	Time::Delta	maxAge;		///< Max-Age=VALUE ... cookies auto-expire.
	Buffer		path;		///< Path=VALUE ... URLs that see the cookie.
	Buffer		portlist;	///< Port[="portlist"] ... the port cookie may be returned to.
	Bool		secure;		///< Secure ... e.g. use SSL.
	Uint32		version;	///< Version=1 ... RFC 2965 style.
	Time::Clock	whenCreated;	///< Cookie creation time (in seconds).

	static const Time::Delta MAX_AGE_UNSPECIFIED;
	static const Buffer NETSCAPE_COOKIE_DATE_FORMAT;	///< Date format used by Netscape's cookie draft.
	static const Buffer SET_COOKIE;
	static const Buffer SET_COOKIE2;

	/// \brief Forbidden default constructor.

	HttpCookie(
	);

	/// \brief Forbidden copy constructor.

	HttpCookie(
		const	HttpCookie &
	);

	/// \brief Forbidden copy operator.

	HttpCookie & operator = (
		const	HttpCookie &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_HttpCookie_H_

// ============================================================================

