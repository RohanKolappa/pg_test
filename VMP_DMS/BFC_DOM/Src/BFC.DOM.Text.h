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
//	BFC.DOM.Text.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Text_H_
#define _BFC_DOM_Text_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CharacterData.h"
#include "BFC.DOM.TextImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief The Text class represents text data in the parsed XML document.
/// 
/// You can split the text in a Text object over two Text
/// objecs with splitText().
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Text : public CharacterData {

public:

	/// \brief Constructs an empty Text object.
	/// 
	/// To construct a Text with content, use Document::createTextNode().

	Text(
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Text(
		const	Text &		x
	);

	/// \brief Assigns \a x to this DOM text.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Text & operator = (
		const	Text &		other
	);

	/// \brief Splits this DOM text object into two Text objects.
	///
	/// This object
	/// keeps its first \a offset characters and the second (newly
	/// created) object is inserted into the document tree after this
	/// object with the remaining characters.
	/// 
	/// The function returns the newly created object.

	Text splitText(
		const	Uint32		offset
	);

	virtual NodeType getNodeType(
	) const;

private:

	Text(
		const	TextImplPtr &
	);

	friend class CDATASection;
	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Text_H_

// ============================================================================

