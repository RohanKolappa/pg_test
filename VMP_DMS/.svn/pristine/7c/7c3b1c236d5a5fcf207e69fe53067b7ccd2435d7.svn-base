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
//	BFC.DOM.Comment.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Comment_H_
#define _BFC_DOM_Comment_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CharacterData.h"
#include "BFC.DOM.CommentImplPtr.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief Represents an XML comment.
/// 
/// A comment in the parsed XML such as this:
/// \code
/// 	<!-- this is a comment -->
/// \endcode
/// is represented by DOM::Comment objects in the parsed Dom tree.
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Comment : public CharacterData {

public:

	/// \brief Constructs an empty comment. To construct a comment with
	///	content, use the DOM::Document::createComment() function.

	Comment(
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Comment(
		const	Comment &	x
	);

	/// \brief Assigns \a x to this DOM comment.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Comment & operator = (
		const	Comment &
	);

	virtual NodeType getNodeType() const {
		return CommentNode;
	}

private:

	Comment(
			CommentImplPtr
	);

	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Comment_H_

// ============================================================================

