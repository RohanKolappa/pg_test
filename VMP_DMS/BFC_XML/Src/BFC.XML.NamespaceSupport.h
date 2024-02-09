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
//	BFC.XML.NamespaceSupport.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_NamespaceSupport_H_
#define _BFC_XML_NamespaceSupport_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.BufferArray.h"
#include "BFC.TL.Map.h"
#include "BFC.TL.Stack.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

typedef Map< Buffer, Buffer > NamespaceMap;

// ============================================================================

/// \brief Helper class for XML readers which want to include namespace
///	support.
///
/// You can set the prefix for the current namespace with setPrefix(),
/// and get the list of current prefixes ( or those for a given URI )
/// with getPrefixes(). The namespace URI is available from getUri(). Use
/// pushContext() to start a new namespace context, and popContext()
/// to return to the previous namespace context. Use splitName() or
/// processName() to split a name into its prefix and local name.
///
/// \ingroup BFC_XML

class BFC_XML_DLL NamespaceSupport {

public :

	/// \brief Constructs a new NamespaceSupport.

	NamespaceSupport(
	);

	/// \brief Destroys this object.

	virtual ~NamespaceSupport(
	);

	/// \brief Declares a prefix \a pre in the current namespace
	///	context to be the namespace URI \a uri.
	///
	/// The prefix remains in
	/// force until this context is popped, unless it is shadowed in a
	/// descendant context.
	///
	/// Note that there is an asymmetry in this library. getPrefix() does not
	/// return the default "" prefix, even if you have declared one; to
	/// check for a default prefix, you must look it up explicitly using
	/// getUri(). This asymmetry exists to make it easier to look up prefixes
	/// for attribute names, where the default prefix is not allowed.

	void setPrefix(
		const	Buffer &	pre,
		const	Buffer &	uri
	);

	/// \brief Returns one of the prefixes mapped to the namespace URI
	///	\a uri.
	///
	/// If more than one prefix is currently mapped to the same URI, this
	/// function makes an arbitrary selection; if you want all of the
	/// prefixes, use getPrefixes() instead.
	/// 
	/// \note To check for a default prefix, use the getUri() function with
	///	an argument of "".

	const Buffer & getPrefix(
		const	Buffer &	uri
	) const;

	/// \brief Looks up the prefix \a prefix in the current context and
	///	returns the currently-mapped namespace URI.
	///
	/// Use the empty string ( "" ) for the default namespace.

	const Buffer & getUri(
		const	Buffer &	prefix
	) const;

	/// \brief Splits the name \a qname at the ':' and returns the prefix
	///	in \a prefix and the local name in \a localname.

	void splitName(
		const	Buffer &	qname,
			Buffer &	prefix,
			Buffer &	localname
	) const;

	/// \brief Processes a raw XML 1.0 name in the current context by
	///	removing the prefix and looking it up among the prefixes
	///	currently declared.
	///
	/// \param qname
	///	[in] The raw XML 1.0 name to be processed.
	///
	/// \param isAttribute
	///	[in] True if the name is an attribute name.
	///
	/// \param nsuri
	///	[out] The namespace URI, or an empty string if the raw name
	///	has an undeclared prefix.
	///
	/// \param localname
	///	[out] The local name (without prefix), or an empty string if no
	///	namespace is in use.
	/// 
	/// \note Attribute names are processed differently than element
	///	names: an unprefixed element name gets the default namespace
	///	(if any), while an unprefixed element name does not.

	void processName(
		const	Buffer &	qname,
		const	Bool		isAttribute,
			Buffer &	nsuri,
			Buffer &	localname
	) const;

	/// \brief Returns a list of all the prefixes currently declared.
	///
	/// If there is a default prefix, this function does not return it in
	/// the list; check for the default prefix using getUri() with an
	/// argument of "".

	BufferArray getPrefixes(
	) const;

	/// \brief Returns a list of all prefixes currently declared for the
	///	namespace URI \a uri.
	///
	/// The "xml:" prefix is included. If you only want one prefix that is
	/// mapped to the namespace URI, and you don't care which one you get,
	/// use the getPrefix() function instead.
	/// 
	/// Note: The empty ( default ) prefix is never included in this list;
	/// to check for the presence of a default namespace, call getUri() with
	/// "" as the argument.

	BufferArray prefixes(
		const	Buffer &	uri
	) const;

	/// \brief Starts a new namespace context.
	/// 
	/// Normally, you should push a new context at the beginning of each
	/// XML element: the new context automatically inherits the
	/// declarations of its parent context, and it also keeps track of
	/// which declarations were made within this context.

	void pushContext(
	);

	/// \brief Reverts to the previous namespace context.
	/// 
	/// Normally, you should pop the context at the end of each XML
	/// element. After popping the context, all namespace prefix mappings
	/// that were previously in force are restored.

	void popContext(
	);

	/// \brief Resets this namespace support object ready for reuse.

	void reset(
	);

	const NamespaceMap & getNs(
	) const {
		return ns;
	}

private:

	Stack< NamespaceMap >	nsStack;
	NamespaceMap		ns;

	static const Buffer	emptyBuffer;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	NamespaceSupport(
		const	NamespaceSupport&
	);

	NamespaceSupport& operator = (
		const	NamespaceSupport&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_NamespaceSupport_H_

// ============================================================================

