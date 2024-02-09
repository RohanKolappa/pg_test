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
//	BFC.XML.Attributes.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_Attributes_H_
#define _BFC_XML_Attributes_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief SAX Attributes.
/// 
/// If attributes are reported by ContentHandler::startElement(),
/// this class is used to pass the attribute values.
/// 
/// Use index() to locate the position of an attribute in the list,
/// getCount() to retrieve the number of attributes, and clear() to
/// remove the attributes. New attributes can be added with append().
/// Use type() to get an attribute's type and value() to get its
/// value. The attribute's name is available from localName() or
/// qName(), and its namespace URI from getUri().
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL Attributes {

public :

	/// \brief Constructs a new empty Attributes list.

	Attributes(
	);

	/// \brief Destroys this object.

	virtual ~Attributes(
	);

	/// \brief Looks up the index of an attribute by the qualified name
	///	\a qName.
	/// 
	/// \returns The index of the attribute.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	Uint32 getIndex(
		const	Buffer &	qName
	) const;

	/// \brief Looks up the index of an attribute by a namespace name.
	/// 
	/// \a uri specifies the namespace URI, or an empty string if the name
	/// has no namespace URI. \a localPart specifies the attribute's local
	/// name.
	/// 
	/// \returns The index of the attribute.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	Uint32 getIndex(
		const	Buffer &	uri,
		const	Buffer &	localPart
	) const;

	/// \brief Returns the number of attributes in the list.

	Uint32 getLength(
	) const {
		return attList.getSize();
	}

	/// \brief Returns the number of attributes in the list.

	Uint32 getCount(
	) const {
		return getLength();
	}

	/// \brief Looks up an attribute's local name for the attribute at
	///	position \a index.
	///
	/// If no namespace processing is done, the local name is an empty
	/// string.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getLocalName(
		const	Uint32		index
	) const;

	/// \brief Looks up an attribute's XML 1.0 qualified name for the
	///	attribute at position \a index.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getQualifiedName(
		const	Uint32		index
	) const;

	/// \brief Looks up an attribute's namespace URI for the attribute at
	///	position \a index.
	///
	/// If no namespace processing is done or if the attribute has no
	/// namespace, the namespace URI is an empty string.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getUri(
		const	Uint32		index
	) const;

	/// \brief Looks up an attribute's type for the attribute at position
	///	\b index.
	///
	/// Currently only "CDATA" is returned.

	const Buffer & getType(
		const	Uint32		/* index */
	) const {
		return cdataBuffer;
	}

	/// \brief Looks up an attribute's type for the qualified name \b qName.
	///
	/// Currently only "CDATA" is returned.

	const Buffer & getType(
		const	Buffer &	/* qName */
	) const {
		return cdataBuffer;
	}

	/// \brief Looks up an attribute's type by namespace name.
	///
	/// \b uri specifies the namespace URI and \b localName specifies the
	/// local name. If the name has no namespace URI, use an empty string
	/// for \b uri.
	///
	/// Currently only "CDATA" is returned.

	const Buffer & getType(
		const	Buffer &	/* uri */,
		const	Buffer &	/* localName */
	) const {
		return cdataBuffer;
	}

	/// \brief Returns an attribute's value for the attribute at position
	///	\a index.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getValue(
		const	Uint32		index
	) const;

	/// \brief Returns an attribute's value for the qualified name \a
	///	qName.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getValue(
		const	Buffer &	qName
	) const;

	/// \brief Returns an attribute's value by namespace name.
	///
	/// \a uri specifies the namespace URI, or an empty string if the name
	/// has no namespace URI. \a localName specifies the attribute's local
	/// name.
	///
	/// \throws NoSuchAttribute
	///	If there is no such attribute.

	const Buffer & getValue(
		const	Buffer &	uri,
		const	Buffer &	localName
	) const;

	/// \brief Clears the list of attributes.

	void kill(
	) {
		attList.kill();
	}

	/// \brief Appends a new attribute entry to the list of attributes. The
	///	qualified name of the attribute is \a qName, the namespace URI is
	///	\a uri and the local name is \a localPart. The value of the
	///	attribute is \a value.

	void append(
		const	Buffer &	qName,
		const	Buffer &	uri,
		const	Buffer &	localPart,
		const	Buffer &	value
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.XML.Attributes" );

	CLASS_EXCEPTION( NoSuchAttribute, "No such attribute" );

private:

	struct Attribute {
		Buffer	qname;
		Buffer	uri;
		Buffer	localname;
		Buffer	value;
	};

	typedef Array< Attribute > AttributeList;

	AttributeList		attList;
	static const Buffer	emptyBuffer;
	static const Buffer	cdataBuffer;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Attributes(
		const	Attributes&
	);

	Attributes& operator = (
		const	Attributes&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_Attributes_H_

// ============================================================================

