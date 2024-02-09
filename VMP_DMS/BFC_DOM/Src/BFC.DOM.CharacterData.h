// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::DOM".
// 
// "BFC::DOM" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::DOM"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.CharacterData.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_CharacterData_H_
#define _BFC_DOM_CharacterData_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CharacterDataImplPtr.h"
#include "BFC.DOM.Node.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief The DOM::CharacterData class represents a generic string in the DOM.
/// 
/// Character data as used in XML specifies a generic data string.
/// More specialized versions of this class are DOM::Text, DOM::Comment
/// and DOM::CDATASection.
/// 
/// The data string is set with setData() and retrieved with getData().
/// You can retrieve a portion of the data string using
/// substringData(). Extra data can be appended with appendData(), or
/// inserted with insertData(). Portions of the data string can be
/// deleted with deleteData() or replaced with replaceData(). The
/// length of the data string is returned by length().
/// 
/// The node type of the node containing this character data is
/// returned by getNodeType().
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL CharacterData : public Node {

public:

	/// \brief Constructs an empty character data object.

	CharacterData();

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	CharacterData(
		const	CharacterData	& x
	);

	/// \brief Assigns \a x to this character data.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	CharacterData& operator= (const CharacterData&);

	/// \brief Returns the substring of length \a count from position \a offset.

	Buffer substringData(const Uint32 offset, const Uint32 count);

	/// \brief Appends the string \a arg to the stored string.

	void appendData(const Buffer& arg);

	/// \brief Inserts the string \a arg into the stored string at position \a offset.

	void insertData(const Uint32 offset, const Buffer& arg);

	/// \brief Deletes a substring of length \a count from position \a offset.

	void deleteData(const Uint32 offset, const Uint32 count);

	/// \brief Replaces the substring of length \a count starting at position \a
	/// offset with the string \a arg.

	void replaceData(const Uint32 offset, const Uint32 count, const Buffer& arg);

	/// \brief Returns the length of the stored string.

	Uint32 length() const;

	/// \brief Returns the string stored in this object.
	/// 
	/// If the node is a \link isEmpty() null node\endlink, it will return
	/// an empty string.

	const Buffer & getData() const;

	/// \brief Sets this object's string to \a v.

	void setData(const Buffer&);

	virtual NodeType getNodeType() const;

private:

	Buffer	emptyBuffer;

	CharacterData(
			CharacterDataImplPtr
	);

	friend class Document;
	friend class Text;
	friend class Comment;
	friend class Node;
};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_CharacterData_H_

// ============================================================================

