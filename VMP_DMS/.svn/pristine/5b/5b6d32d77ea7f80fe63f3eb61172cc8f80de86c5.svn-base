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
//	BFC.DOM.XCharData.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XCharData_H_
#define _BFC_DOM_XCharData_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XCharDataPtr.h"

// ============================================================================

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM CharcterData.
///
/// The CharacterData interface extends Node with a set of attributes and
/// methods for accessing character data in the DOM. For clarity this set is
/// defined here rather than on each object that uses these attributes and
/// methods. No DOM objects correspond directly to CharacterData, though Text
/// and others do inherit the interface from it. All offsets in this interface
/// start from 0.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XCharData : virtual public XNode {

public :

	/// \brief Returns the character data of the node that implements this
	///	interface.
	///
	/// The DOM implementation may not put arbitrary limits on the amount
	/// of data that may be stored in a CharacterData node. However,
	/// implementation limits may mean that the entirety of a node's data
	/// may not fit into a single DOMString. In such cases, the user may
	/// call substringData to retrieve the data in appropriately sized
	/// pieces.

	virtual const Buffer & getData(
	) const = 0;

	/// \brief Sets the character data of the node that implements this
	///	interface.
	///
	/// \throw Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised when the node is readonly.

	virtual void setData(
		const	Buffer &	pData
	) = 0;

	/// \brief Returns the number of 16-bit units that are available
	///	through data and the substringData method below.
	///
	/// This may have the value zero, i.e., CharacterData nodes may be
	/// empty.

	virtual Uint32 getLength(
	) const = 0;

	/// \brief Extracts a range of data from the node.
	///
	/// \param offset
	///	Start offset of substring to extract.
	///
	/// \param count
	///	The number of 16-bit units to extract.
	/// 
	/// \returns
	///	The specified substring. If the sum of offset and count exceeds
	///	the length, then all 16-bit units to the end of the data are
	///	returned.
	///
	/// \throws Exception( INDEX_SIZE_ERR )
	///	Raised if the specified offset is negative or greater than the
	///	number of 16-bit units in data, or if the specified count is
	///	negative.
	/// 
	/// \throws Exception( DOMSTRING_SIZE_ERR )
	///	Raised if the specified range of text does not fit into a
	///	DOMString.

	virtual Buffer substringData(
		const	Uint32		offset, 
		const	Uint32		count
	) const = 0;

	/// \brief Append the string to the end of the character data of the
	///	node.
	///
	/// Upon success, data provides access to the concatenation of data and
	/// the DOMString specified.
	///
	/// \param arg
	///	The DOMString to append.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.

	virtual void appendData(
		const	Buffer &	arg
	) = 0;

	/// \brief Insert a string at the specified 16-bit unit offset.
	///
	/// \param offset
	///	The character offset at which to insert.
	///
	/// \param arg
	///	The DOMString to insert.
	/// 
	/// \throws Exception( INDEX_SIZE_ERR )
	///	Raised if the specified offset is negative or greater than the
	///	number of 16-bit units in data.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.

	virtual void insertData(
		const	Uint32		offset, 
		const	Buffer &	arg
	) = 0;

	/// \brief Remove a range of 16-bit units from the node.
	///
	/// Upon success, data and length  reflect the change.
	///
	/// \param offset
	///	The offset from which to start removing.
	///
	/// \param count
	///	The number of 16-bit units to delete. If the sum of offset and
	///	count exceeds length then all 16-bit units from offset to the
	///	end of the data are deleted.
	/// 
	/// \throws Exception( INDEX_SIZE_ERR )
	///	Raised if the specified offset is negative or greater than the
	///	number of 16-bit units in data, or if the specified count is
	///	negative.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.

	virtual void deleteData(
		const	Uint32		offset, 
		const	Uint32		count
	) = 0;

	/// \brief Replace the characters starting at the specified 16-bit unit
	///	offset with the specified string.
	///
	/// \param offset
	///	The offset from which to start replacing.
	///
	/// \param count
	///	 The number of 16-bit units to replace. If the sum of offset
	///	and count exceeds length, then all 16-bit units to the end of
	///	the data are replaced; (i.e., the effect is the same as a
	///	remove method call with the same range, followed by an append
	///	method invocation).
	///
	/// \param arg
	///	The DOMString with which the range must be replaced.
	/// 
	/// \throws Exception( INDEX_SIZE_ERR )
	///	Raised if the specified offset is negative or greater than the
	///	number of 16-bit units in data, or if the specified count is
	///	negative.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.

	virtual void replaceData(
		const	Uint32		offset, 
		const	Uint32		count, 
		const	Buffer &	arg
	) = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XCharData_H_

// ============================================================================

